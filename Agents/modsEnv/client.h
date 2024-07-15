#ifndef CLIENT_H
#define CLIENT_H

//
// client.h - Custom client application header
//

/*!
  \file client.h
  \brief MODS Environmental Monitor Agent Client Application Header
  \date 2010 June 21

  ISIS client application header for the modsenv application.

  Encapsulates the various sensor readings derived from the MODS
  instrument controller system.
*/

// Various site-dependent but system-independent default values 

// Default client application values (override/set in loadconfig.c)

#define DEFAULT_MYID      "ENV"  //!< default client ISIS node name
#define DEFAULT_MYPORT    10901 //!< default client socket port   
#define DEFAULT_RCFILE    (char *)"./modsenv.ini" //!< default client runtime config file


#define ENV_LOGS "/home/dts/Logs/Env/modsenv"       //!< Default enviromental log file path and rootname
#define HDF_LOGS "/home/dts/Logs/Env/modsenv-hdf5"  //!< Default hdf5 telemetry log file path

#define LEAP_SECONDS_FILE "/usr/share/lbto/UT/leap-seconds.list"  //!< Default path to the leap-seconds.list file.

// Default ISIS server information (see loadconfig.c if used)

#define DEFAULT_ISISID   "IS"        //!< default ISIS server node name
#define DEFAULT_ISISHOST "localhost" //!< default ISIS server host
#define DEFAULT_ISISPORT 6600        //!< default ISIS server port number

//
// END of Site-Dependent Setup
// 
//----------------------------------------------------------------

// System header files   

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <unistd.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/times.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <termios.h>
#include <fcntl.h>
#include <signal.h>
#include <exception>

#include <telcollection.hxx>  //LBTO Telemetry Library

// In case the version and compilation data are not defined
// at compilation, put in some placeholders to prevent code barfing

#ifndef APP_VERSION
#define APP_VERSION "0.0.0" //!< placeholder version number, set in Makefile
#endif

#ifndef APP_COMPDATE
#define APP_COMPDATE "2010-01-01" //!< placeholder compilation date, set by build script
#endif

#ifndef APP_COMPTIME
#define APP_COMPTIME "00:00:00" //!< placeholder compilation time, set by build script
#endif

// Useful working parameters

#ifndef MAXCFGLINE
#define MAXCFGLINE 128 //!< Maximum characters/line in runtime config files
#endif

#define MAX_TELEMETRY_BUFFER_BYTES 12000000 //!< Maximum number of bytes that the lbt telemetry collector can use to store samples.


// ISIS common client utilties library header

#include "isisclient.h"     // should be in -I path in Makefile, no paths here!
extern isisclient_t client; // global client runtime config table

// FieldTalk WAGO communication utilities (in the -I path) - defunct 2024
/*
#include "MbusTcpMasterProtocol.hpp" // WAGO FieldTalk Modbus/TCP master protocol header
*/

#include "modbusutils.h"

// MODS Instrument Server Shared Memory

/*
#include <instrutils.h>
#include <isl_funcs.h>
#include <islcommon.h>
#include <isl_shmaddr.h>
*/

#define DEFAULT_CADENCE  300   //!< default monitoring cadence in seconds

/*!
  \brief A struct which holds all of the enviornment and telemetry data for this modsEnv instance.
*/
typedef struct envData {

  char  modsID[8];   //!< MODS instrument ID (MODS1 or MODS2)

  // Environmental monitoring parameters

  long  cadence;   //!< Monitor update cadence in seconds
  int   pause;     //!< Pause monitoring if 1, continue monitoring if 0

  // Head Electronics Box (HEB) temperature and quadcell data.

  char  heb_Addr[64]; //!< IP address of the HEB WAGO FieldBus controller
  float ambientTemp;  //!< Outside ambient air temperature in degrees C
  float quadcell[4];  //!< Quadcell readings measured in volts.

  // Logging information

  int  doLogging;                  //!< Is logging enabled?  
  char utcDate[MED_STR_SIZE];      //!< UTC data/time of the last query
  char logRoot[MED_STR_SIZE];      //!< Full path/rootname of the enviromental data log
  char logFile[MED_STR_SIZE];      //!< Full path/name of the current enviromental data log
  char lastDate[SHORT_STR_SIZE];   //!< Most recent UTC date tag for the data log
  int  logFD;                      //!< File descriptor of the open data log

  // HDF5 logging information

  int useHdf5;                        //!< Should HDF5 logs be created?  
  char hdfRoot[MED_STR_SIZE];         //!< Full path/rootname of the HDF5 log directory
  char leapSecondsFile[MED_STR_SIZE]; //!< Full path/rootname of the leap-seconds.list

  lbto::tel::float_measure::buf_proxy ambientTempMeasure;       //!< The numeric data in the telemetry stream
  lbto::tel::float_measure::buf_proxy quadcellMeasure[4];

  std::shared_ptr<lbto::tel::collector> modsCollector;          //!< The telemetry collection interface

} envdata_t;

extern envdata_t env;

/*!
  \brief A class which provides callback methods for Hdf5 output.

  This class is used to initalize the telemetry library. The telemetry 
  library is used to generate Hdf5 output files.
*/
class TelemetryCallback : public lbto::tel::ambassador{
public:
  TelemetryCallback(){}
  ~TelemetryCallback(){}

  lbto::tel::name get_name(){return lbto::tel::name("mods");}
  std::string get_hdf_path_name(){return std::string(env.hdfRoot);}
  std::string get_leap_seconds_file_name(){return std::string(env.leapSecondsFile);}
  std::string get_disk_buffer_dir(){return "/tmp";}
  uint64_t get_disk_buffer_size(){return MAX_TELEMETRY_BUFFER_BYTES;}

  void notice(const std::string msg){
    printf("Telemetry Notice: %s\n", msg.c_str());
  }

  void handle_critical(const std::string msg){
    printf("Telemetry Critical: %s\n", msg.c_str());
  }

  void notify_connected(const lbto::tel::name& telemeterName, const lbto::tel::system& system){
    printf("%s collection connected to telemetry\n", std::string(telemeterName).c_str());
  }

  void notify_disconnected(const lbto::tel::name& telemeterName, const lbto::tel::system& system){
    printf("%s collection disconnected to telemetry\n", std::string(telemeterName).c_str());
  }
};

//------------------------------
// Globals - use very sparingly
//
// Remember to declare explicitly in main.c
//

extern int useCLI;   //!< Use the command-line interface. 1=use it, 0=no cli

//----------------------------------------------------------------
//
// Custom client application function prototypes 
//
 
int  loadConfig(char *);      // Load/parse the agent runtime config file (see loadconfig.c)
void KeyboardCommand(char *); // process keyboard (cli) commands (see commands.c)
void SocketCommand(char *);   // process messages from the client socket (see commands.c)

// Client utility routines (defined in clientutils.c)

void initEnvData(envdata_t *);                                    // initialize the envdata_t struct
void printEnvData(envdata_t *);                                   // print the contents of the envdata_t struct (engineering)
int  getEnvData(envdata_t *);                                     // get environmental data from the sensor WAGOs
void qc2vdc(uint16_t* rawData, float* outputData);                // converts raw qcell data into a DC voltage.
void ptRTD2C(uint16_t* rawData, float* outputData, int numRtds);  // converts raw RTD data into a temperature.

// Log utility routines (defined in logutils.c)

int  initTelemetryData(envdata_t *);    // initlaize the telemetry structures in envdata_t if HDF5 will be used
void closeTelemetryData(envdata_t *);   // cleanly closes the telemetry structures in envdata_t if HDF5 was used
int  logTelemetryData(envdata_t *);     // append data to the telemetry stream for the HDF5 file. 
int  initEnvLog(envdata_t *);           // initialize the enviromental data log
int  logEnvData(envdata_t *);           // append data to the environmental data log
int  logMessage(envdata_t *, char *);   // append a message (comment) to the data log
int  fileExists(char *);                // test to see if a file exists

// Signal Handlers

void HandleInt(int);  // SIGINT handler
void HandleKill(int); // SIGKILL handler

#endif  // CLIENT_H
