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

// DD update loop keep-going flag

int keepGoing = 1;

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
  char varStr[64];
  int n;
  string side;
  double t0, dt;
  
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

  sprintf(iceFile,"%s/%s.client",DEFAULT_IIFDIR,lbt.iceProps);

  // which side of LBT are we on?
  
  if (!strcasecmp(lbt.side,"left"))
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

  // Set signal traps

  signal(SIGINT,HandleExit);   // Trap Ctrl+C, exit and close Ice proxy
  signal(SIGKILL,HandleExit); // Trap kill and close Ice proxy

  //---------------------------------------------------------------------------
  //
  // DD Update Loop
  //
  // Run in an infinite loop until the program is killed or we get a fatal
  // exception - usually happens when the proxy dies when the IIF server
  // goes down.
  //
  // Update MODS instance DD data of interest then sleep for lbt.cadence
  // seconds.
  //

  keepGoing = 1;

  int redGrating = 1;
  int blueGrating = 2;
  
  while (keepGoing) {

    getUTCTime(&utc);
    t0 = SysTimestamp();

    DDstruct dd;
    SeqDD ddList;

    // MODS name and update time stamps (text and integer)
    
    dd.DDname = side + "_MODSName";
    dd.DDkey = (string)lbt.instID;
    ddList.push_back(dd);

    dd.DDname = side + "_MODSUpdateTime";
    dd.DDkey = (string)utc.ISO;
    ddList.push_back(dd);

    dd.DDname = side + "_MODSTimeStamp";
    sprintf(varStr,"%d",t0);
    dd.DDkey = (string)varStr;
    ddList.push_back(dd);

    // MODS instrument configuration: CALMODE, OBSMODE, or UNKNOWN

    dd.DDname = side + "_MODSInstConfig";
    if (shm_addr->MODS.MODS.instrMode == 0)
      dd.DDkey = "OBSMODE";
    else if (shm_addr->instrMode == 1)
      dd.DDkey = "CALMODE";
    else
      dd.DDkey = "UNKNOWN";
    ddList.push_back(dd);

    // MODS subsystem power states (On/Off/Fault)

    powerState(shm_addr->MODS.utilState,varStr); // If the utility box is off, all of MODS is off
    dd.DDname = side + "_MODSPowerState"; 
    dd.DDkey = (string)varStr;
    ddList.push_back(dd);

    powerState(shm_addr->MODS.blueIEBState,varStr);
    dd.DDname = side + "_MODSBlueIEBPower";
    dd.DDkey = (string)varStr;
    ddList.push_back(dd);

    powerState(shm_addr->MODS.redIEBState,varStr);
    dd.DDname = side + "_MODSRedIEBPower";
    dd.DDkey = (string)varStr;
    ddList.push_back(dd);

    powerState(shm_addr->MODS.blueHEBState,varStr);
    dd.DDname = side + "_MODSBlueHEBPower";
    dd.DDkey = (string)varStr;
    ddList.push_back(dd);

    powerState(shm_addr->MODS.redHEBState,varStr);
    dd.DDname = side + "_MODSRedHEBPower";
    dd.DDkey = (string)varStr;
    ddList.push_back(dd);

    powerState(shm_addr->MODS.blueArchonState,varStr);
    dd.DDname = side + "_MODSBlueArchonPower";
    dd.DDkey = (string)varStr;
    ddList.push_back(dd);

    powerState(shm_addr->MODS.redArchonState,varStr);
    dd.DDname = side + "_MODSRedArchonPower";
    dd.DDkey = (string)varStr;
    ddList.push_back(dd);

    powerState(shm_addr->MODS.blueIonGaugeState,varStr);
    dd.DDname = side + "_MODSBlueVacuumGaugePower";
    dd.DDkey = (string)varStr;
    ddList.push_back(dd);

    powerState(shm_addr->MODS.redIonGaugeState,varStr);
    dd.DDname = side + "_MODSRedVacuumGaugePower";
    dd.DDkey = (string)varStr;
    ddList.push_back(dd);

    powerState(shm_addr->MODS.guideCamState,varStr);
    dd.DDname = side + "_MODSGuideCamPower";
    dd.DDkey = (string)varStr;
    ddList.push_back(dd);

    powerState(shm_addr->MODS.wfsCamState,varStr);
    dd.DDname = side + "_MODSWFSCamPower";
    dd.DDkey = (string)varStr;
    ddList.push_back(dd);

    powerState(shm_addr->MODS.llbState,varStr);
    dd.DDname = side + "_MODSLLBPower";
    dd.DDkey = (string)varStr;
    ddList.push_back(dd);

    // MODS environmental sensor data (temperature and pressure)

    dd.DDname = side + "_MODSIUBTemp";
    sprintf(varStr,".1f",shm_addr->MODS.utilBoxAirTemperature);
    dd.DDkey = (string)varStr;
    ddList.push_back(dd);
    
    dd.DDname = side + "_MODSGlycolSupplyTemp";
    sprintf(varStr,".2f",shm_addr->MODS.glycolSupplyTemperature);
    dd.DDkey = (string)varStr;
    ddList.push_back(dd);
    
    dd.DDname = side + "_MODSGlycolSupplyPres";
    sprintf(varStr,".2f",shm_addr->MODS.glycolSupplyPressure);
    dd.DDkey = (string)varStr;
    ddList.push_back(dd);
    
    dd.DDname = side + "_MODSGlycolReturnTemp";
    sprintf(varStr,".2f",shm_addr->MODS.glycolReturnTemperature);
    dd.DDkey = (string)varStr;
    ddList.push_back(dd);
    
    dd.DDname = side + "_MODSGlycolReturnPres";
    sprintf(varStr,".2f",shm_addr->MODS.glycolReturnPressure);
    dd.DDkey = (string)varStr;
    ddList.push_back(dd);
    
    dd.DDname = side + "_MODSBlueIEBTemp";
    sprintf(varStr,".1f",shm_addr->MODS.blueTemperature[0]);
    dd.DDkey = (string)varStr;
    ddList.push_back(dd);
    
    dd.DDname = side + "_MODSCollTempTop";
    sprintf(varStr,".1f",shm_addr->MODS.blueTemperature[2]); // note: blueTemperature[1] is not reported
    dd.DDkey = (string)varStr;
    ddList.push_back(dd);
    
    dd.DDname = side + "_MODSCollTempBottom";
    sprintf(varStr,".1f",shm_addr->MODS.blueTemperature[3]); 
    dd.DDkey = (string)varStr;
    ddList.push_back(dd);
    
    dd.DDname = side + "_MODSRedIEBTemp";
    sprintf(varStr,".1f",shm_addr->MODS.redTemperature[0]);
    dd.DDkey = (string)varStr;
    ddList.push_back(dd);
    
    dd.DDname = side + "_MODSAirTempTop";
    sprintf(varStr,".1f",shm_addr->MODS.redTemperature[2]); // note: redTemperature[1] is not reported
    dd.DDkey = (string)varStr;
    ddList.push_back(dd);
    
    dd.DDname = side + "_MODSAirTempBottom";
    sprintf(varStr,".1f",shm_addr->MODS.redTemperature[3]); 
    dd.DDkey = (string)varStr;
    ddList.push_back(dd);

    dd.DDname = side + "_MODSBlueDewPres";
    sprintf(varStr,"%8.2e",shm_addr->MODS.blueDewarPressure);
    dd.DDkey = (string)varStr;
    ddList.push_back(dd);
    
    dd.DDname = side + "_MODSBlueDewTemp";
    sprintf(varStr,"%.1f",shm_addr->MODS.blueDewarTemperature);
    dd.DDkey = (string)varStr;
    ddList.push_back(dd);

    dd.DDname = side + "_MODSBlueHEBTemp";
    sprintf(varStr,"%.1f",shm_addr->MODS.blueHEBTemperature);
    dd.DDkey = (string)varStr;
    ddList.push_back(dd);

    dd.DDname = side + "_MODSRedDewPres";
    sprintf(varStr,"%8.2e",shm_addr->MODS.redDewarPressure);
    dd.DDkey = (string)varStr;
    ddList.push_back(dd);
    
    dd.DDname = side + "_MODSRedDewTemp";
    sprintf(varStr,"%.1f",shm_addr->MODS.redDewarTemperature);
    dd.DDkey = (string)varStr;
    ddList.push_back(dd);
    
    dd.DDname = side + "_MODSRedHEBTemp";
    sprintf(varStr,"%.1f",shm_addr->MODS.redHEBTemperature);
    dd.DDkey = (string)varStr;
    ddList.push_back(dd);

    // IMCS IR laser state

    dd.DDname = side + "_MODSIMCSLaser";
    if (shm_addr->MODS.lasers.irlaser_state == 0)
      dd.DDkey = "OFF";
    else
      dd.DDkey = "ON";
    ddList.push_back(dd);

    dd.DDname = side + "_MODSIMCSLaserBeam";
    if (shm_addr->MODS.lasers.irbeam_state == 0)
      dd.DDkey = "DISABLED";
    else
      dd.DDkey = "ENABLED";
    ddList.push_back(dd);

    dd.DDname = side + "_MODSIMCSLaserPower";
    sprintf(varStr,"%.3f",shm_addr->MODS.lasers.irlaser_power);
    dd.DDkey = (string)varStr;
    ddList.push_back(dd);
    
    dd.DDname = side + "_MODSIMCSLaserTemp";
    sprintf(varStr,"%.1f",shm_addr->MODS.lasers.irlaser_temp);
    dd.DDkey = (string)varStr;
    ddList.push_back(dd);

    // Calibration Lamp states

    sprintf(varStr,"");
    if (shm_addr->MODS.lamps.lamp_state[0]) sprintf(varStr,"%sAr ",varStr);
    if (shm_addr->MODS.lamps.lamp_state[1]) sprintf(varStr,"%sXe ",varStr);
    if (shm_addr->MODS.lamps.lamp_state[2]) sprintf(varStr,"%sNe ",varStr);
    if (shm_addr->MODS.lamps.lamp_state[3]) sprintf(varStr,"%sHg ",varStr);
    if (shm_addr->MODS.lamps.lamp_state[4]) sprintf(varStr,"%sKr ",varStr);
    if (shm_addr->MODS.lamps.lamp_state[6]) sprintf(varStr,"%sQTH1 ",varStr);
    if (shm_addr->MODS.lamps.lamp_state[7]) sprintf(varStr,"%sQTH2 ",varStr);
    if (shm_addr->MODS.lamps.lamp_state[8]) sprintf(varStr,"%sVFLAT ",varStr);

    dd.DDname = side + "_MODSCalibLamps";
    dd.DDkey = (string)varStr;
    ddList.push_back(dd);
    
    dd.DDname = side + "_MODSVFLATIntensity";
    sprintf(varStr,"%.2f",shm_addr->MODS.vflat_power);
    dd.DDkey = (string)varStr;
    ddList.push_back(dd);
    
    // Mechanism states (positions, element names, etc.)

    // ...
    
    // Set the DD parameters
    
    res = iif->SetParameter(ddList);
    if (res.rescode != EXIT_SUCCESS) {
      keepGoing = 0;
    }

    dt = SysTimestamp() - t0;
    
    // sleep for lbt.cadence if we didn't get an error

    if (keepGoing) sleep(lbt.cadence);
    
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
// HandleExit() - SIGKILL/SIGINT signal handler to exit gracefully
//
// This ensures SIGINT or SIGKILL signals close any open Ice proxies
// for a clean shutdown.
//
// SIGINT would be seen if testing by hand and stopping with Ctrl+C
// SIGKILL is how it terminates when run as a systemd service
//

void
HandleExit(int signalValue)
{
  if (factory) factory->destroy(iif);
  if (communicator) communicator->destroy();
}

