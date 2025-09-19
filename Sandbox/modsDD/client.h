#ifndef CLIENT_H
#define CLIENT_H

//
// client.h - Custom client application header
//

/*!
  \file client.h
  \brief MODS data dictionary client application header

  Client application header for the modsDD application.

  \date 2025 Sept 19
*/

// Default client application values (override/set in loadconfig.c)

#define DEFAULT_RCFILE    (char*)"/home/dts/Config/modsDD.ini" //!< default client runtime config file
#define DEFAULT_LOGFILE   (char*)"/home/dts/Logs/modsDD.log" //!< default client runtime log file (unimplemented)

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
#define APP_COMPDATE "2025-09-01" //!< placeholder compilation date, set by build script
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

// MODS IIF Utilities

#include "modsIIFUtils.h"  // MODS/LBT ICE/IIF Utilities
#include "modsIIFCalls.h"  // MODS/LBT ICE/IIF function prototypes
extern lbtData_t modsDD;   // global LBT telescope data structure
extern modsData_t mods;    // global MODS status data structure

/*!
  \brief LBT telescope info data structure

  Encapsulates the various data we need to keep track of the
  telescope and MIS status.

*/

typedef struct lbtinfo {

  // TCS link and configuration

  int  Link;                    //!< LBT TCS server link status (1=up/0=down)
  int  opMode;                  //!< Operating Mode, one of LIVE or LABSIM
  int  Turbo;                   //!< Turbo TCS Mode 1=on, 0=off (off=default)

  // IIF instance information

  char iceProps[SHORT_STR_SIZE];   //!< Rootname of the ICE properties file
  char proxy[SHORT_STR_SIZE];      //!< IIF ICE proxy ID
  char instID[SHORT_STR_SIZE];     //!< LBT Instrument ID (e.g., MODS or IRTC)
  char focStation[SHORT_STR_SIZE]; //!< LBT focal station (directGregorian)
  char side[SHORT_STR_SIZE];       //!< Instrument telescope side (left|right|both)
  char telescope[SHORT_STR_SIZE];  //!< Telescope Name

} lbtinfo_t;

extern lbtinfo_t lbt;

/*!
  \brief UTC Time/Date Data structure

  Encapsulates the components of the current date and time in
  a convenient way.  Also computes aggregate representations
  in various ways, all conforming to the ISO8601 standard.

*/

typedef struct utcinfo {

  // UTC date/time components

  int Year;      //!< UTC Year (ccyy format)
  int Month;     //!< UTC Month number 1..12
  int Day;       //!< UTC day of Month
  int Hour;      //!< UTC hour (0..23)
  int Min;       //!< UTC minutes (0..59)
  double Sec;    //!< UTC seconds, double precision

  // UTC aggregate representations

  char Date[SHORT_STR_SIZE]; //!< UTC Date string, ISO8601 CCYY-MM-DD format
  char Time[SHORT_STR_SIZE]; //!< UTC Time string, ISO8601 HH:MM:SS.SSS format
  char ISO[SHORT_STR_SIZE];  //!< UTC Date/Time string, ISO8601 CCYY-MM-DDTHH:MM:SS.SSS format
  double dtime;  //!< UTC Time in decimal hours

} utcinfo_t;

extern utcinfo_t utc;

// Flags we might need

#define LIVE   0  //!< Operations Mode = Live LBT TCS
#define LABSIM 1  //!< Operations Mode = Laboratory Simulation

//----------------------------------------------------------------
//
// Custom client application function prototypes 
//
 
int  loadConfig(char *);      // Load/parse the agent runtime config file (see loadconfig.c)

// Client utility routines (defined in clientutils.c)

void initLBTInfo(lbtinfo_t *);
void getUTCTime(utcinfo_t *);

#endif  // CLIENT_H
