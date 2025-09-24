#ifndef CLIENT_H
#define CLIENT_H

//
// client.h - Custom client application header
//

/*!
  \file client.h
  \brief MODS Environmental Monitor Agent Client Application Header
  \date 2010 June 21
  \date 2025 Aug 17 - Archon HEB updates [rwp/osu]

  ISIS client application header for the modsenv application.

  Encapsulates the various sensor readings derived from the MODS
  instrument controller system.
*/

// Various site-dependent but system-independent default values 

// Default client application values (override/set in loadconfig.c)

#define DEFAULT_MYID      "ENV"  //!< default client ISIS node name
#define DEFAULT_MYPORT    10901 //!< default client socket port   
#define DEFAULT_RCFILE    (char *)"/home/dts/Config/modsenv.ini" //!< default client runtime config file


#define ENV_LOGS "/home/dts/Logs/Env/modsenv"       //!< Default enviromental log file path and rootname
#define HDF_LOGS "/home/dts/Logs/Env/modsenv-hdf5"  //!< Default hdf5 telemetry log file path

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

// libmodbus replaces fieldtalk 2025

#include "modbusutils.h"

// ion gauge

#include "ionutils.h"

// MODS shared memory segment

#include <instrutils.h>
#include <islcommon.h>
#include <isl_shmaddr.h>

void setup_ids();  // shmem setup function, do this to not need all of isl_funcs.h

#define DEFAULT_CADENCE  300   //!< default monitoring cadence in seconds

/*!
  \brief A struct which holds all of the enviornment and telemetry data for this modsEnv instance.
*/

typedef struct envData {

  char  modsID[8];   //!< MODS instrument ID (MODS1 or MODS2)

  // Environmental monitoring parameters

  long  cadence;   //!< Monitor update cadence in seconds
  int   pause;     //!< Pause monitoring if 1, continue monitoring if 0

  // Instrument Utility Box (IUB) temperature and pressure sensor data

  char  iub_Addr[64];      //!< IP address of the IUB WAGO FieldBus controller
  float ambientTemp;       //!< Outside ambient air temperature in degrees C
  float glycolSupplyPres;  //!< Instrument glycol supply pressure in psi-g
  float glycolReturnPres;  //!< Instrument glycol return pressure in psi-g
  float glycolSupplyTemp;  //!< Instrument glycol supply temperature in degrees C
  float glycolReturnTemp;  //!< Instrument glycol return temperature in degrees C
  float utilBoxTemp;       //!< IUB inside-box air temperature in degrees C
  float agwHSTemp;         //!< AGw camera controller heat sink temperature in degrees C

  // IUB AC power control state data

  int   iebB_Switch;       //!< Blue IEB AC power remote switch state (1=On/0=Off)
  int   iebB_Breaker;      //!< Blue IEB current flow at breaker output (1=On/0=Off)
  int   iebR_Switch;       //!< Red IEB AC power remote switch state (1=On/0=Off)
  int   iebR_Breaker;      //!< Red IEB current flow at breaker output (1=On/0=Off)
  int   hebB_Switch;       //!< Blue HEB AC power remote switch state (1=On/0=Off)
  int   hebB_Breaker;      //!< Blue HEB current flow at breaker output (1=On/0=Off)
  int   hebR_Switch;       //!< Red HEB AC power remote switch state (1=On/0=Off)
  int   hebR_Breaker;      //!< Red HEB current flow at breaker output (1=On/0=Off)
  int   llb_Switch;        //!< Lamp/Laser Box AC power remote switch state (1=On/0=Off)
  int   llb_Breaker;       //!< Lamp/Laser Box current flow at breaker output (1=On/0=Off)
  int   gcam_Switch;       //!< AGw Guide Camera controller AC power remote switch state (1=On/0=Off)
  int   gcam_Breaker;      //!< AGw Guide Camera controller current flow at breaker output (1=On/0=Off)
  int   wfs_Switch;        //!< AGw WFS Camera controller AC power remote switch state (1=On/0=Off)
  int   wfs_Breaker;       //!< AGw WFS Camera controller current flow at breaker output (1=On/0=Off)
  
  // Red Channel Instrument Electronics Box (IEB_R) temperature sensor data

  char  iebR_Addr[64];     //!< IP address of the Red IEB WAGO FieldBus controller
  float iebR_AirTemp;      //!< Red IEB box air temperature in degrees C
  float iebR_ReturnTemp;   //!< Red IEB glycol return temperature in degrees C
  float airTopTemp;        //!< MODS instrument Top inside air temperature in degrees C
  float airBotTemp;        //!< MODS instrument Bottom inside air temperature in degrees C

  // Blue Channel Instrument Electronics Box (IEB_B) temperature sensor data

  char  iebB_Addr[64];     //!< IP address of the Blue IEB WAGO FieldBus controller
  float iebB_AirTemp;      //!< Blue IEB box air temperature in degrees C
  float iebB_ReturnTemp;   //!< Blue IEB glycol return temperature in degrees C
  float trussTopTemp;      //!< MODS collimator truss tube Top temperature in degrees C
  float trussBotTemp;      //!< MODS collimator truss tube Bottom temperature in degrees C

  // Lamp/Laser Box (LLB) IR laser state data (unimplemented, future expansion)

  char  llb_Addr[64];      //!< IP address of the LLB WAGO FieldBus controller
  int   irlaserState;      //!< IR Laser Unit AC power state (1=on, 0=off)
  int   irlaserBeam;       //!< IR Laser beam state (1=enable/0=disabled)
  float irlaserPowerSet;   //!< IR Laser output beam power requested setting in mW
  float irlaserPowerOut;   //!< IR Laser output beam power measured in mW
  float irlaserTemp;       //!< IR Laser head temperature in degrees C
  float irlaserTempSet;    //!< IR laser head temperature control set point in degrees C

  // Red Head Electronics Box (HEB_R) sensor data

  char  hebR_Addr[64];     //!< IP address of the Red HEB WAGO FieldBus controller
  float hebR_AirTemp;      //!< Red HEB interior air temperature
  float redDewTemp;        //!< Red dewar temperature (read by the red HEB)
  int   redArchon;         //!< Red channel Archon controller power state (1=On/0=Off)
  int   redIonGauge;       //!< Red dewar ion gauage power state (1=On/0=Off)

  // Blue Head Electronics Box (HEB_B) sensor data

  char  hebB_Addr[64];      //!< IP address of the Blue HEB WAGO FieldBus controller
  float hebB_AirTemp;       //!< Blue HEB interior air temperature
  float blueDewTemp;        //!< Blue dewar temperature (read by the red HEB)
  int   blueArchon;         //!< Blue channel Archon controller power state (1=On/0=Off)
  int   blueIonGauge;       //!< Blue dewar ion gauage power state (1=On/0=Off)

  // Red dewar inonization gauge

  char  redIG_Addr[64];     //!< Red dewar ionization gauge Comtrol IP address
  int   redIG_Port;         //!< Red dewar ionization gauge Comtrol port
  int   redIG_Chan;         //!< Red dewar ionization gauge RS485 channel (usually 5)
  float redDewPres;         //!< Red dewar pressure in torr
  
  // Blue dewar inonization gauge

  char  blueIG_Addr[64];    //!< Blue dewar ionization gauge Comtrol IP address
  int   blueIG_Port;        //!< Blue dewar ionization gauge Comtrol port
  int   blueIG_Chan;        //!< Blue dewar ionization gauge RS485 channel (usually 5)
  float blueDewPres;        //!< Blue dewar pressure in torr
  
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

  lbto::tel::float_measure::buf_proxy ambientTempMeasure;       //!< The ambientTemp data in the telemetry stream
  lbto::tel::float_measure::buf_proxy glycolSupplyPresMeasure;  //!< The glycolSupplyPres data in the telemetry stream
  lbto::tel::float_measure::buf_proxy glycolReturnPresMeasure;  //!< The glycolReturnPres data in the telemetry stream
  lbto::tel::float_measure::buf_proxy glycolSupplyTempMeasure;  //!< The glycolSupplyTemp data in the telemetry stream
  lbto::tel::float_measure::buf_proxy glycolReturnTempMeasure;  //!< The glycolReturnTemp data in the telemetry stream
  lbto::tel::float_measure::buf_proxy utilBoxTempMeasure;       //!< The utilBoxTemp data in the telemetry stream
  lbto::tel::float_measure::buf_proxy agwHSTempMeasure;         //!< The agwHSTemp data in the telemetry stream
  lbto::tel::float_measure::buf_proxy iebBAirTempMeasure;       //!< The iebBAirTemp data in the telemetry stream
  lbto::tel::float_measure::buf_proxy iebBReturnTempMeasure;    //!< The iebBReturnTemp data in the telemetry stream
  lbto::tel::float_measure::buf_proxy iebRAirTempMeasure;       //!< The iebRAirTemp data in the telemetry stream
  lbto::tel::float_measure::buf_proxy iebRReturnTempMeasure;    //!< The iebRReturnTemp data in the telemetry stream
  lbto::tel::float_measure::buf_proxy airTopTempMeasure;        //!< The airTopTemp data in the telemetry stream
  lbto::tel::float_measure::buf_proxy airBotTempMeasure;        //!< The airBotTemp data in the telemetry stream
  lbto::tel::float_measure::buf_proxy trussTopTempMeasure;      //!< The trussTopTemp data in the telemetry stream
  lbto::tel::float_measure::buf_proxy trussBotTempMeasure;      //!< The trussBotTemp data in the telemetry stream
  lbto::tel::float_measure::buf_proxy hebBAirTempMeasure;       //!< blueHEBAirTemp data in the telemetry stream
  lbto::tel::float_measure::buf_proxy hebRAirTempMeasure;       //!< redHEBAirTemp data in the telemetry stream
  lbto::tel::float_measure::buf_proxy blueDewTempMeasure;       //!< blueDewTemp data in the telemetry stream
  lbto::tel::float_measure::buf_proxy redDewTempMeasure;        //!< redDewTemp data in the telemetry stream
  lbto::tel::float_measure::buf_proxy blueDewPresMeasure;       //!< blueDewPres data in the telemetry stream
  lbto::tel::float_measure::buf_proxy redDewPresMeasure;        //!< redDewPres data in the telemetry stream

  std::shared_ptr<lbto::tel::collector> modsCollector;          //!< The telemetry collection interface

} envdata_t;

extern envdata_t env;

/*!
  \brief A class to provide callback methods for Hdf5 output.

  This class initalizes the LBTO telemetry library. The telemetry 
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

//----------------------------------
//
// IUB power and breaker state register addresses
//
// Updated: 2025 Aug 15 [rwp/osu] for new IUB wiring for the Archon HEBs (power changed, breakers the same)
//

#define IEB_R_POWER   1
#define IEB_B_POWER   2
#define HEB_R_POWER   8
#define HEB_B_POWER   32
#define WFS_POWER     64
#define GCAM_POWER    128
#define LLB_POWER     256

#define IEB_R_BREAKER 1
#define IEB_B_BREAKER 2
#define HEB_R_BREAKER 4
#define HEB_B_BREAKER 8
#define WFS_BREAKER   16
#define GCAM_BREAKER  32
#define LLB_BREAKER   64

// HEB power state register addresses (same for red and blue)

#define ARCHON_POWER  1
#define IG_POWER      2

// Power State Indicators

#define POWER_OFF    0
#define POWER_ON     1
#define POWER_MANUAL 2
#define POWER_FAULT -1

// MODS Channel Indexes

#define MODS_BLUE 0  // Blue channel params map into even indexes
#define MODS_RED  1  // Red channel params map into odd idexes

// Mappings for IEB temperatures into iebTemp[4]
//
// index = MODS_<Chan> + 2*MODS_<Param>

#define MODS_IEBTEMP 0
#define MODS_IEBGRT  1

// Mappings for GCam and WFS controller power state into UTIL_i[MAX_ML]

#define MODS_GCAMPOWER 0
#define MODS_WFSPOWER  1

// HDF5 telemetry leap seconds file and fallback version

#define LEAP_SECONDS_FILE "/usr/share/lbto/UT/leap-seconds.list"  //!< Default path to the leap-seconds.list file.
#define FALLBACK_LEAP_SECONDS_FILE "./leap-seconds.list"          //!< If the leap-seconds file can't be found at the path above, this one is tried next.

// LBTO Alarm Handler (ALH) no-data "magic" values

#define ALH_NOTEMP -999.0
#define ALH_NOPRES -1.0

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

void initEnvData(envdata_t *);   // initialize the envdata_t struct
void printEnvData(envdata_t *);  // print the contents of the envdata_t struct (engineering)
int  getEnvData(envdata_t *);    // get environmental data from the sensor WAGOs
float ptRTD2C(short);            // convert WAGO Pt RTD raw sensor data to degrees C

// Log utility routines (defined in logutils.c)

int  initTelemetryData(envdata_t *);    // initialize the telemetry structures in envdata_t if HDF5 will be used
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
