/*!
  \mainpage modsDD - MODS data dictionary agent

  \author R. Pogge, OSU Astronomy Dept. (pogge.1@osu.edu)
  \date 2025 Sept 19 - first version

  \section Usage

  Usage: modsDD [rcfile]

  Where: \c rcfile is an optional runtime config file to load.  

  By default, modsDD uses the runtime config file defined by
  #DEFAULT_RCFILE in the client.h header.

  \section Overview

  modsDD is a standalone (non-client) program that interrogates the
  MODS shared memory segment on an active MODS instrument server
  machine (mods1 or mods2) and uploads relevant MODS instrument state
  information into the observatory IIF data dictionary (DD) at a set
  cadence (notionally 5-10 seconds).

  This program requires IIF Build 2025A or greater which has the
  extended MODS data dictionary entries.

  modsDD pprovides LBTO with full-time updates of instantaneous
  instrument status info that may be used by observatory dashboards or
  alarm state monitoring systems without their needing to directly
  interrogate the MODS instrument systems. This should eliminate risks
  associated with asynchronous third-party status queries interrupting
  real-time instrument control and data-acqusition systems.

  The code is designed to be run non-interactively as a systemd service.

  \section Modification History

 <pre>
 2025 Sept 19 - new application based on lbttcs [rwp/osu]
 </pre>

*/

/*!
  \file main.c
  \brief modsDD main program and DD SetParameter loop
*/

// client application header

#include "client.h"

// all the includes we need

#include <unistd.h>

#include <iostream>
#include <fstream>
#include <sstream>

// Ice/IIF implementation

#include <Ice/Ice.h>
#include <Factory.h>
#include <IIFServer.h>

// MODS shared memory segment

#include "instrutils.h"  // instrument utilities
#include "islcommon.h"   // shared memory segment layout (Islcommon struct)
#include "isl_shmaddr.h" // declares pointer to islcommon

// About time

#include <time.h>

// internal defaults (mostly from testing)

#define INST_ID "MODS"
#define FOCSTATION "directGregorian left"
#define LBT_SIDE "left"
#define CLIENT_PROXYNAME "MODS1_DD"

#define MODS_NAME "MODS1"

// namespaces used by the LBTO software

using namespace std;
using namespace lbto;

// ICE/ICF Globals

Ice::CommunicatorPtr communicator; // Ice communicator
lbto::iifres res;                  // IIF result object
lbto::IIFServerPrx iif;            // IIF server proxy
lbto::FactoryPrx factory;          // IIF factory proxy

char clientProxy_Name[80];
char instrument_Name[80];
char focalStation_Name[80];

char iifmsgs[200][100];
int iifcount;

// Shared memory segment setup

void setup_ids();

// isisclient bits we use, even though we're not a full ISIS client

#include "isisclient.h"
isisclient_t client;

lbtinfo_t lbt;  // LBT IIF info data structure (read from loadconfig)
utcinfo_t utc;  // UTC date/time handling

// main() program

int
main(int argc, char* argv[])
{
  int status = EXIT_SUCCESS;
  int updateCadence = 10; // DD update cadence in seconds
  char varStr[64];
  int n;
  string side;
  
  // Ice client properties file, usually lbtIIF.client, etc.
  
  char iceFile[256]; 

  // Attach the shared memory segment

  setup_ids();
  
  // Load and parse the runtime configuration file

  if (argc == 2) {
    n = loadConfig(argv[1]);
    argc--;
  }
  else
    n = loadConfig(DEFAULT_RCFILE);
    
  if (n!=0) {
    printf("\nUnable to load config file %s - modsDD aborting n=%d\n\n",
	   client.rcFile,n);
    exit(11);
  }

  sprintf(iceFile,"%s/%s",DEFAULT_IIFDIR,lbt.iceProps);

  // which side of LBT are we on?
  
  if (!strncasecomp(lbt.side,"left"))
    side = "L";
  else
    side = "R";
  
  // Load the ice properties
  
  Ice::PropertiesPtr props = Ice::createProperties();
  props->load(iceFile);
  Ice::InitializationData id;
  id.properties = props;

  // Establish a connection to the IIF
  
  communicator = Ice::initialize(argc, argv, id);

  factory = FactoryPrx::checkedCast(communicator->propertyToProxy("Factory.Proxy"));

  string proxyName = props->getPropertyWithDefault("MODS.ProxyName",lbt.proxy);
  string instrumentID = props->getPropertyWithDefault("MODS.InstrumentID","MODS"); // no 1 or 2!
  string focalStation = props->getPropertyWithDefault("MODS.FocalStation","directGregorian left");

  strcpy(clientProxy_Name,lbt.proxy); 
  strcpy(focalStation_Name,&focalStation[0]);
  strcpy(instrument_Name,&instrumentID[0]);

  try {
    if (!factory)
      throw "Invalid proxy: IIF Server not found.";
      
    // Get a proxy object for this instrument. If the proxy already exists, the arbitrator will
    // link the proper IIF instance with it, otherwise it will create a new IIF instance for this
    // client.
      
    iif = factory->create(clientProxy_Name,focalStation_Name,instrument_Name);

    if (!iif)
      throw "Invalid proxy: Invalid instrument/focal station combination or invalid side.";
    else
      res.resmsg.clear();
  }
  catch(const Ice::Exception& ex) {
    cerr << ex << endl;
    status = EXIT_FAILURE;
  }
  catch (const char* msg) {
    cerr << msg << endl;
    status = EXIT_FAILURE;
  }
  catch (string msg)  {
    cerr << msg << endl;
    status = EXIT_FAILURE;
  }

  //---------------------------------------------------------------------------
  //
  // DD Update Loop
  //
  // Run in an infinite loop until the program is killed or we get a fatal
  // exception - usually happens when the proxy dies when the IIF server
  // goes down.
  //
  // Update MODS instance DD data of interest then sleep for updateCadence
  // seconds.
  //

  int keepGoing = 1;

  // sim mechanisms
  
  int redGrating = 1;
  int blueGrating = 2;
  
  while (keepGoing) {

    getUTCTime(&utc);

    DDstruct dd;
    SeqDD ddList;

    dd.DDname = side + "_MODSName";
    dd.DDkey = (string)lbt.instID;
    ddList.push_back(dd);

    dd.DDname = side + "_MODSUpdateTime";
    dd.DDkey = (string)utc.ISO;
    ddList.push_back(dd);

    dd.DDname = side + "_MODSRedDewPres";
    sprintf(varStr,"%8.2e",shm_addr->MODS.redDewarPressure);
    dd.DDkey = (string)varStr;
    ddList.push_back(dd);
    
    dd.DDname = side + "_MODSBlueDewPres";
    sprintf(varStr,"%8.2e",shm_addr->MODS.blueDewarPressure);
    dd.DDkey = (string)varStr;
    ddList.push_back(dd);
    
    dd.DDname = side + "_MODSRedDewTemp";
    sprintf(varStr,"%.1f",shm_addr->MODS.redDewarTemperature);
    dd.DDkey = (string)varStr;
    ddList.push_back(dd);
    
    dd.DDname = side + "_MODSBlueDewTemp";
    sprintf(varStr,"%.1f",shm_addr->MODS.blueDewarTemperature);
    dd.DDkey = (string)varStr;
    ddList.push_back(dd);

    dd.DDname = side + "_MODSBlueGratingPosition";
    sprintf(varStr,"%d",blueGrating);
    dd.DDkey = (string)varStr;
    ddList.push_back(dd);

    dd.DDname = side + "_MODSBlueGrating";
    dd.DDkey = "G400L";
    ddList.push_back(dd);
    
    dd.DDname = side + "_MODSRedGratingPosition";
    sprintf(varStr,"%d",redGrating);
    dd.DDkey = (string)varStr;
    ddList.push_back(dd);

    dd.DDname = side + "_MODSRedGrating";
    dd.DDkey = "Flat";
    ddList.push_back(dd);

    // send it!
    
    res = iif->SetParameter(ddList);
    if (res.rescode != EXIT_SUCCESS) {
      keepGoing = 0;
    }

    // sleep for updateCadence if we didn't get an error

    if (keepGoing) sleep(updateCadence);
    
  }
  
  //---------------------------------------------------------------------------
  //
  // Cleanup on exiting
  //
  
  // destroy the IIF instance from the factory

  factory->destroy(iif);

  // close the communicator
    
  if (communicator)
    communicator->destroy();
  
  return status;
}

//---------------------------------------------------------------------------
//
// HandleKill() - SIGKILL signal handler to make sure we don't leave
//                delinquent ICE proxies behind.
//

void
HandleKill(int signalValue)
{
  if (factory) factory-destroy(iif);
  if (communicator) communicator->destroy();
}
