//
// Based on a synchronous IIF client example from the IIF user manual section 9.1
//

#include <unistd.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <Ice/Ice.h>
#include <Factory.h>


#include "instrutils.h"  // ISL instrument header
#include "islcommon.h"   // shred memory segment layout (Islcommon struct)
#include "isl_shmaddr.h" // declares pointer to islcommon

#include <time.h>

#define INST_ID "MODS"
#define FOCSTATION "directGregorian left"
#define LBT_SIDE "left"

#define MODS_NAME "MODS1"

// Just to identify the clients.

#define CLIENT_PROXYNAME "MODS1_DD"

// commands

using namespace std;
using namespace lbto;

// Print result messages coming from the TCS

void showResults(const lbto::iifres _res, const string _cmd);

// Shared memory segment setup

void setup_ids();

// isisclient bits we use, even if we're not a full ISIS client

#include "isisclient.h"
isisclient_t client;

// main() program

int
main(int argc, char* argv[])
{
  int status = EXIT_SUCCESS;
  int updateCadence = 10; // DD update cadence in seconds
  char varStr[64];
  
  string side = LBT_SIDE;

  if (side[0]=="l")
    side = "L";
  else
    side = "R";

  // Shared memory segment attachment

  setup_ids();
  
  // Ice communicator and results object (note iifres for C++ starting w/Ice v3.7)
  
  Ice::CommunicatorPtr communicator; // Ice communicator
  lbto::iifres res;

  // Establish a connection to the IIF
  
  try {
    communicator = Ice::initialize(argc, argv);
    FactoryPrx factory = FactoryPrx::checkedCast(communicator->stringToProxy("Factory:tcp -p 10000"));

    if (!factory)
      throw "Invalid proxy: IIF Server not found.";
      
    // Get a proxy object for this instrument. If the proxy already exists, the arbitrator will
    // link the proper IIF instance with it, otherwise it will create a new IIF instance for this
    // client.
      
    lbto::IIFServerPrx iif = factory->create(CLIENT_PROXYNAME, FOCSTATION, INST_ID);

    if (!iif)
      throw "Invalid proxy: Invalid instrument/focal station combination or invalid side.";

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

    DDstruct dd;
    SeqDD ddList;

    dd.DDname = side + "_MODSName";
    dd.DDkey = MODS_NAME;
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
    showResults(res,"SetParameter");
    if (res.rescode != EXIT_SUCCESS) {
      if (iifcount == 0) 
	strcpy(iifmsgs[0],"Error: mods_SetParameter Failed");
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

void
showResults(const lbto::iifres _res, const string _cmd)
{
  if (_res.rescode == 0) cout << _cmd << " command status: SUCCESS" << endl;
  for ( unsigned int i=0; i< _res.resmsg.size(); i++)
    cout << _res.resmsg[i] << endl;
}
