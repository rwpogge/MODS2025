#ifndef MODSHEB_CLIENT_H
#define MODSHEB_CLIENT_H

/*!
  \file client.h
  \brief MODS HEB Monitor and Control Agent - Client Application Header
  \date 2024 August 14

  ISIS client application header for the modsheb application.
*/

//// START of Site-Dependent Setup. ---------------------------------------

// Default client application values (override/set in loadconfig.c)
#define DEFAULT_MYID      "ENV"                     //!< default client ISIS node name
#define DEFAULT_MYPORT    10901                     //!< default client socket port   
#define DEFAULT_RCFILE    (char *)"./modsheb.ini"   //!< default client runtime config file


#define ENV_LOGS "/home/dts/Logs/Env/modsheb"       //!< Default enviromental log file path and rootname
#define HDF_LOGS "/home/dts/Logs/Env/modsheb-hdf5"  //!< Default hdf5 telemetry log file path

// Default ISIS server information (see loadconfig.c if used)
#define DEFAULT_ISISID   "IS"        //!< default ISIS server node name
#define DEFAULT_ISISHOST "localhost" //!< default ISIS server host
#define DEFAULT_ISISPORT 6600        //!< default ISIS server port number

//// END of Site-Dependent Setup. -----------------------------------------
//// START of System Header Files. ----------------------------------------

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
#include <limits.h>

#include <telcollection.hxx>  // LBTO telemetry library
#include <isisclient.h>       // ISISClient library
#include "modbusutils.h"      // Utility functions for libmodbus

//// END of System Header Files. -------------------------------------------
//// START of System Versioning. -------------------------------------------

// In case the version and compilation data are not defined at compilation, put in some placeholders.

#ifndef APP_VERSION
#define APP_VERSION "0.0.0"         //!< placeholder version number, set in Makefile
#endif

#ifndef APP_COMPDATE
#define APP_COMPDATE "2010-01-01"   //!< placeholder compilation date, set by build script
#endif

#ifndef APP_COMPTIME
#define APP_COMPTIME "00:00:00"     //!< placeholder compilation time, set by build script
#endif

//// END of System Versioning. ---------------------------------------------
//// START of Constants and Working Parameters. ----------------------------

#ifndef MAXCFGLINE
#define MAXCFGLINE 128        //!< Maximum characters/line in runtime config files
#endif

#define DEFAULT_CADENCE  300  //!< default monitoring cadence in seconds

#define MAX_TELEMETRY_BUFFER_BYTES 12000000                       //!< Maximum number of bytes that the lbt telemetry collector can use to store samples.
#define LEAP_SECONDS_FILE "/usr/share/lbto/UT/leap-seconds.list"  //!< Default path to the leap-seconds.list file.
#define FALLBACK_LEAP_SECONDS_FILE "./leap-seconds.list"          //!< If an updated leap-seconds file can't be found, this one is used instead.


//// END of Constants and Working Parameters. ------------------------------
//// START of Class and Struct Definitions. --------------------------------

//An enum to make the integer values of process types more readable.
typedef enum {DO, DO_NC, RTD, AI} PROCESS;
 
//This maps the string values of the enum above to their numerical values.
const static struct{
    PROCESS value;
    const char* string;
}PROCESS_MAP[] = {
    {DO, "DO"},
    {RTD, "RTD"},
    {AI, "AI"}
};
const int PROCESS_MAP_LEN = sizeof(PROCESS_MAP)/sizeof(PROCESS_MAP[0]);

/*!
  \brief The data that will be stored for each device.
*/
typedef struct DeviceProfile{
    float data;                     // The most recently collected data for the device
    char name[MAXCFGLINE];          // The name of the device
    int address;                    // The address of the device in the module

    char description[MAXCFGLINE];   // The description of the device for HDF logs
    int logEntry;                   // Non-Zero if the device data should be logged

    int nc;                         // Non-Zero if this is a DO that is Normally closed, zero otherwise.

    lbto::tel::float_measure::buf_proxy floatMeasure;  // The data stream for HDF logs
}device_t;

/*!
  \brief The data that will be stored for each module.
*/
typedef struct ModuleProfile{
    device_t* devices;        // An array of connected devices and their data
    char name[MAXCFGLINE];    // The name of the module
    int processingType;       // The type of processing to do to the device data
    int baseAddress;          // The address of the module
    int numDevices;           // The number of connected devices
    int maxOffset;            // The largest offset of any device
}device_module_t;

/*!
  \brief A struct which holds all of the enviornment and telemetry data for this modsHeb instance.
*/
typedef struct envData {

  // WAGO Addresses
  char hebAddr[64];   //!< IP address of the HEB WAGO FieldBus controller

  // WAGO Modules
  int numModules;
  device_module_t* modules; //!< An array containing all connected modules
  uint16_t* rawWagoData;    //!< The most recently collected data from the WAGO.

  // Environmental monitoring parameters
  long  cadence;   //!< Monitor update cadence in seconds
  int   pause;     //!< Pause monitoring if 1, continue monitoring if 0

  // Logging information
  int  doLogging;                  //!< Is logging enabled?  
  char utcDate[MED_STR_SIZE];      //!< UTC data/time of the last query
  char logRoot[MED_STR_SIZE];      //!< Full path/rootname of the enviromental data log
  char logFile[MED_STR_SIZE];      //!< Full path/name of the current enviromental data log
  char lastDate[SHORT_STR_SIZE];   //!< Most recent UTC date tag for the data log
  int  logFD;                      //!< File descriptor of the open data log

  // HDF5 logging information
  int hdfInitalized;                  //!< The first time hdf is used, it needs to be initalized.
  int useHdf5;                        //!< Should HDF5 logs be created?
  char hdfRoot[MED_STR_SIZE];         //!< Full path/rootname of the HDF5 log directory
  char leapSecondsFile[MED_STR_SIZE]; //!< Full path/name of the leap-seconds.list

  std::shared_ptr<lbto::tel::collector> modsCollector;  //!< The telemetry collection interface

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

//// END of Class and Struct Declerations. ---------------------------------
//// START of Global Variable Declerations. --------------------------------

// Remember to declare explicitly in main.c
extern int useCLI;                //!< Use the command-line interface. 1=use it, 0=no cli
extern isisclient_t client;       //!< Global client runtime config table
extern const int NUM_DEVICES;     //!< The number of entries in the device table.

//// END of Global Variable Declerations. ----------------------------------
//// START of Custom Client Function Declerations. -------------------------

// Load/parse the agent runtime config file (see loadconfig.c)
int  loadConfig(char *);

// Functions for handling command input (see commands.c)
void KeyboardCommand(char *); // process keyboard (cli) commands (see commands.c)
void SocketCommand(char *);   // process messages from the client socket (see commands.c)

// Client utility routines (defined in clientutils.c)
void initEnvData(envdata_t *);    // initialize the envdata_t struct
void printEnvData(envdata_t *);   // print the contents of the envdata_t struct (engineering)
void freeEnvData(envdata_t *);    // frees all dynamically allocated memory in the envdata_t struct

// Utility routines for device data (defined in devices.c)
int  getAllDeviceData(envdata_t *);                           // get environmental data from the sensor WAGO
int  getDeviceModuleData(envdata_t *, int i);                 // get environmental data from the sensor WAGO for one module
void freeAllDeviceData(envdata_t *);                          // frees memory allocated for devices in the envdata_t struct
int strToProcessType(char*);                                  // given the string value for a process_type, returns the integer value for that process type
int getDeviceModuleStatus(envdata_t *, int, int, char*, int);   // turns device data from envdata into a status string


// Log utility routines (defined in logutils.c)
int  initTelemetryData(envdata_t *);        // initlaize the telemetry structures in envdata_t if HDF5 will be used
void closeTelemetryData(envdata_t *);       // cleanly closes the telemetry structures in envdata_t if HDF5 was used
int  logTelemetryData(envdata_t *);         // append data to the telemetry stream for the HDF5 file
int  initEnvLog(envdata_t *);               // initialize the enviromental data log
int  logEnvData(envdata_t *);               // append data to the environmental data log
int  logMessage(envdata_t *, const char *); // append a message (comment) to the data log
int  fileExists(char *);                    // test to see if a file exists

// Signal Handlers
void HandleInt(int);  // SIGINT handler
void HandleKill(int); // SIGKILL handler
void CleanExit();     // Used to cleanly exit the program after the main select loop has started

//// END of Custom Client Function Declerations. ---------------------------

#endif  // MODSHEB_CLIENT_H
