#ifndef CLIENT_H
#define CLIENT_H

//
// client.h - Custom client application header
//

/*!
  \file client.h
  \brief MODS Environmental Monitor Agent Client Application Header

  ISIS client application header for the modsenv application.

  \date 2010 June 21
*/

// Various site-dependent but system-independent default values 

// Default client application values (override/set in loadconfig.c)

#define DEFAULT_MYID      "ENV"  //!< default client ISIS node name
#define DEFAULT_MYPORT    10901 //!< default client socket port   
#define DEFAULT_RCFILE    (char *)"./modsenv.ini" //!< default client runtime config file

// Default enviromental log file path and rootname 

#define ENV_LOGS   "/home/dts/Logs/Env/modsenv"

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

/*!
  \brief MODS instrument environmental sensor data structure

  Encapsulates the various sensor readings derived from the MODS
  instrument controller system.

*/

#define DEFAULT_CADENCE  300   //!< default monitoring cadence in seconds

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

  // Logging information

  int  doLogging;                  //!< Is logging enabled?  
  char utcDate[MED_STR_SIZE];      //!< UTC data/time of the last query
  char logRoot[MED_STR_SIZE];      //!< Full path/rootname of the enviromental data log
  char logFile[MED_STR_SIZE];      //!< Full path/name of the current enviromental data log
  char lastDate[SHORT_STR_SIZE];   //!< Most recent UTC date tag for the data log
  int  logFD;                      //!< File descriptor of the open data log

} envdata_t;

extern envdata_t env;

//----------------------------------
//
// IUB Power/Breaker State Addresses
//

#define IEB_R_POWER   1
#define IEB_R_BREAKER 1
#define IEB_B_POWER   2
#define IEB_B_BREAKER 2
#define HEB_R_POWER   4
#define HEB_R_BREAKER 4
#define HEB_B_POWER   16
#define HEB_B_BREAKER 8
#define LLB_POWER     256
#define LLB_BREAKER   64
#define WFS_POWER     64
#define WFS_BREAKER   16
#define GCAM_POWER    128
#define GCAM_BREAKER  32

// Power State Indicators

#define POWER_OFF    0
#define POWER_ON     1
#define POWER_MANUAL 2
#define POWER_FAULT -1

//--------------------------------------------
//
// IEB Temperature SHRMEM variable addresses
//
// A kludge to utilize various generic SHRMEM
// "deposit" arrays
//

// MODS Channel Indexes

#define MODS_BLUE 0  // Blue channel params map into even indexes
#define MODS_RED  1  // Red channel params map into odd idexes

// Mappings for IEB temperatures into iebTemp[4]
//
// index = MODS_<Chan> + 2*MODS_<Param>
//

#define MODS_IEBTEMP 0
#define MODS_IEBGRT  1

// Mappings for GCam and WFS controller power state into UTIL_i[MAX_ML]

#define MODS_GCAMPOWER 0
#define MODS_WFSPOWER  1

// ... more as needed ...

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

void initEnvData(envdata_t *);  // initialize the envdata_t struct
void printEnvData(envdata_t *); // print the contents of the envdata_t struct (engineering)
int  getEnvData(envdata_t *);   // get environmental data from the sensor WAGOs
int  initEnvLog(envdata_t *);   // initialize the enviromental data log
int  logEnvData(envdata_t *);   // append data to the environmental data log
int  logMessage(envdata_t *, char *); // append a message (comment) to the data log
int  wagoSetGet(int, char *, int, int, uint16_t []); // WAGO set/get data function - 20240220
int  fileExists(char *);        // test to see if a file exists

// Signal Handlers

void HandleInt(int);  // SIGINT handler
void HandleKill(int); // SIGKILL handler

#endif  // CLIENT_H
