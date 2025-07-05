#ifndef CLIENT_H
#define CLIENT_H

//
// client.h - Custom client application header
//

/*!
  \file client.h
  \brief LBT TCS Client Application Header

  ISIS client application header for the lbttcs application.

  Version 3 which implements binocular operations.

  \date 2015 Aug 11
*/

// Various site-dependent but system-independent default values 

#undef __UseOldDD    //!< Use old-style fully-qualifed DD names

// Default client application values (override/set in loadconfig.c)

#define DEFAULT_MYID      "TC"  //!< default client ISIS node name
#define DEFAULT_MYPORT    10801 //!< default client socket port   
#define DEFAULT_RCFILE    (char*)"/home/mods/Config/IIF/lbttcs.ini" //!< default client runtime config file
#define DEFAULT_LOGFILE   (char*)"/home/mods/Logs/lbttcs.log" //!< default client runtime log file (unimplemented)

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
#define APP_COMPDATE "2004-01-01" //!< placeholder compilation date, set by build script
#endif

#ifndef APP_COMPTIME
#define APP_COMPTIME "00:00:00" //!< placeholder compilation time, set by build script
#endif

// XTerm Color Printing Macros - always pair a color with TXTRESET

#define REDTEXT  printf("%c[0;31m",27)   //!< Red normal for most errors
#define GRNTEXT  printf("%c[0;32m",27)   //!< Green normal, sometimes for diagnostics
#define YELTEXT  printf("%c[0;33m",27)   //!< Yellow normal, unused (unreadable)
#define BLUTEXT  printf("%c[0;34m",27)   //!< Blue normal, unused
#define MAGTEXT  printf("%c[1;35m",27)   //!< Magenta bold for fatal errors
#define CYATEXT  printf("%c[0;36m",27)   //!< Cyan normal for warnings

#define TXTRESET printf("%c[0m",27)      //!< reset default text color

// Useful working parameters

#ifndef MAXCFGLINE
#define MAXCFGLINE 128 //!< Maximum characters/line in runtime config files
#endif

// ISIS common client utilties library header

#include "isisclient.h"     // should be in -I path in Makefile, no paths here!
extern isisclient_t client; // global client runtime config table

// Sky Utilities (in -I path)

#include "skyutils.h"  // celestial calculation utilties header

// MODS IIF Utilities

#include "modsIIFUtils.h"  // MODS/LBT ICE/IIF Utilities
#include "modsIIFCalls.h"  // MODS/LBT ICE/IIF function prototypes
extern lbtData_t lbtTCS;   // global LBT telescope data structure
extern modsData_t mods;    // global MODS status data structure

// Other Globals - use very sparingly

extern int useCLI; //!< Boolean: use the command-line interface

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

  // Observatory Site Parameters

  double Latitude;              //!< Site latitude in decimal degrees North
  double Longitude;             //!< Site longitude in decimal degrees West
  double Altitude;              //!< Site elevation in meters above mean sealevel

  // Telescope Parameters

  char utcDate[SHORT_STR_SIZE]; //!< UTC Date in ccyy-mm-dd format            
  char utcTime[SHORT_STR_SIZE]; //!< UTC Time in hh:mm:ss format
  char objName[SHORT_STR_SIZE]; //!< Target object Name
  char objRA[SHORT_STR_SIZE];   //!< Target object Right Ascension in hh:mm:ss.s format     
  char objDec[SHORT_STR_SIZE];  //!< Target object Declination in +dd:mm:ss.s format        
  double objPMRA;               //!< Target object RA proper motion in mas/year
  double objPMDec;              //!< Target object Dec proper motion in mas/year
  double objEpoch;              //!< Target object coordinate Epoch (decimal years)
  char guiName[SHORT_STR_SIZE]; //!< Guide object Name
  char guiRA[SHORT_STR_SIZE];   //!< Guide object Right Ascension in hh:mm:ss.s format     
  char guiDec[SHORT_STR_SIZE];  //!< Guide object Declination in +dd:mm:ss.s format  
  double guiPMRA;               //!< Guide object RA proper motion in mas/year
  double guiPMDec;              //!< Guide object Dec proper motion in mas/year
  double guiEpoch;              //!< Guide object coordinate Epoch (decimal years)
  double guiMag;                //!< Guide object magnitude
  char telRA[SHORT_STR_SIZE];   //!< Telescope Right Ascension in hh:mm:ss.s format     
  char telDec[SHORT_STR_SIZE];  //!< Telescope Declination in +dd:mm:ss.s format        
  char HA[SHORT_STR_SIZE];   //!< Telescope Hour Angle in hh:mm:ss.s format          
  char LST[SHORT_STR_SIZE];     //!< Local Siderial Time in hh:mm:ss.s format 
  double ZD;                    //!< Zenith Distance (decimal degrees)
  double SecZ;                  //!< Secant of Zenith Distance (aka Airmass)
  double Equinox;               //!< Equinox of coordinates in decimal years
  double JD;                    //!< Julian Day Number
  double MJD;                   //!< Modified Julian Date
  double HJD;                   //!< Heliocentric Julian Date
  double telAz;                 //!< Telescope azimuth angle in degrees
  double telEl;                 //!< Telescope elevation angle in degrees
  double telRot;                //!< Telescope focus raw rotator angle in degrees
  float  lambda;                //!< Target reference wavelength in microns
  double rotAngle;              //!< Instrument Rotator Angle in degrees
  double posAngle;              //!< Instrument Celestial Position Angle in degrees
  double parAngle;              //!< Telescope parallactic angle in degrees
  double airMass;               //!< Telescope-reported airmass (dimensionless)
  char rotMode[SHORT_STR_SIZE]; //!< Instrument Rotator Mode (POSITION|PARALLACTIC|IDLE)
  double reqPA;                 //!< Requested rotator position angle [degrees]
  char presetMode[SHORT_STR_SIZE]; //!< Telescope Preset Mode (Active|Guide|Track|Static)

  // Non-Sidereal Target information (populates the nonsidereal structure in the IIFUtils)

  char nsType[SHORT_STR_SIZE];  //!< N-S Object Type: planet name, "FILE","DIFFERENTIAL"
  float nsLambda;               //!< Effective wavelength in microns
  char nsFile[BIG_STR_SIZE];    //!< Ephemeriis file name for nsType="FILE"
  double dMJD;                  //!< Differential MJD in UTC days
  double dRA;                   //!< Differential RA in radians
  double dDec;                  //!< Differental Dec in radians
  double dRAdt;                 //!< Differential dRA/dt * cos(Dec) in radians/day
  double dDecdt;                //!< Differential dDec/dt in radians/day

  // Binocular Operations information

  
  // IIF instance information

  char iceProps[SHORT_STR_SIZE];   //!< Rootname of the ICE properties file
  char proxy[SHORT_STR_SIZE];      //!< IIF ICE proxy ID
  char instID[SHORT_STR_SIZE];     //!< LBT Instrument ID (e.g., MODS or IRTC)
  char focStation[SHORT_STR_SIZE]; //!< LBT focal station (directGregorian)
  char side[SHORT_STR_SIZE];       //!< Instrument telescope side (left|right|both)
  char telescope[SHORT_STR_SIZE];  //!< Telescope Name
  int authorized;                  //!< Is this IIF instance the authorized instrument?
  char authInst[SHORT_STR_SIZE];   //!< Authorized instrument on the current side

  // Coordinates in radians for the IIF

  double objRARad;   //!< Target object RA in radians
  double objDecRad;  //!< Target object Dec in radians
  double guiRARad;   //!< Guide star RA in radians
  double guiDecRad;  //!< Guide star Dec in radians

  // SX focal station
  int  sxAuthorized;            //!< SX focal station authorized (1) or not (0)
  int  sxFocus;                 //!< SX focal station secondary mirror focus
  double sxRotAngle;            //!< SX instrument rotator angle in degrees
  double sxPosAngle;            //!< SX instrument celestial PA in degrees

  // DX focal station
  int  dxAuthorized;            //!< DX focal station authorized (1) or not (0)
  int  dxFocus;                 //!< DX focal station secondary mirror focus
  double dxRotAngle;            //!< DX instrument rotator angle in degrees
  double dxPosAngle;            //!< DX instrument celestial PA in degrees

} lbtinfo_t;

extern lbtinfo_t lbt;

#define LIVE   0  //!< Operations Mode = Live LBT TCS
#define LABSIM 1  //!< Operations Mode = Laboratory Simulation

// Site Latitude, Longitude and Altitude

#define LBTLAT +32.701309  //!< LBT 2x8.4m Site Latitude in decimal degrees North
#define LBTLON 109.889064  //!< LBT Site Longitude in decimal degrees West
#define LBTALT     3221.0  //!< LBT Site Altitude in meters above mean sea level

// Telescope focus limits

#define LBT_FOC_MIN -4.0 //!< LBT minimum focus limit
#define LBT_FOC_MAX  4.0 //!< LBT maximum focus limit

#define LBT_FSTEP_MAX 2.0 //!< Maximum allowed step size in mm

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
  double JD;     //!< Julian Day number of the current UTC date/time
  double MJD;    //!< Modified JD of the current UTC date/time

} utcinfo_t;

extern utcinfo_t utc;

//----------------------------------------------------------------
//
// Custom client application function prototypes 
//
 
int  loadConfig(char *);      // Load/parse the agent runtime config file (see loadconfig.c)
void KeyboardCommand(char *); // process keyboard (cli) commands (see commands.c)
void SocketCommand(char *);   // process messages from the client socket (see commands.c)

// Client utility routines (defined in clientutils.c)

void initLBTInfo(lbtinfo_t *);
void printLBTInfo(lbtinfo_t *);
int  initLBTTCS(char *);
int  queryLBTTCS();
void getUTCTime(utcinfo_t *);
int  HAZDCalc(lbtinfo_t *, utcinfo_t *);

// Signal Handlers

void HandleInt(int);  // SIGINT handler
void HandleKill(int); // SIGKILL handler

#endif  // CLIENT_H
