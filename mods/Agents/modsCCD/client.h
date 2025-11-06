#ifndef CLIENT_H
#define CLIENT_H

//
// client.h - Custom client application header
//

/*!
  \file client.h
  \brief modsCCD azcam client header

  ISIS client header for the modsCCD agent

  Based on y4kcam from May 2005.

  \date 2025 July 23
*/

// Various site-dependent but system-independent default values 

// Default client application values (override/set in config.c)

#define DEFAULT_MYID      "IC"  //!< default client ISIS node name
#define DEFAULT_MYPORT    10501 //!< default client socket port   
#define DEFAULT_RCFILE    (char*)"/home/dts/Config/modsccd.ini" //!< default client runtime config file
#define DEFAULT_LOGFILE   (char*)"/home/dts/Logs/modsccd.log" //!< default client runtime log file (unimplemented)

// Default ISIS server information (see config.c if used)

#define DEFAULT_ISISID   "IS"        //!< default ISIS server node name
#define DEFAULT_ISISHOST "darkstar"  //!< default ISIS server host
#define DEFAULT_ISISPORT 6600        //!< default ISIS server port number

//
// END of Site-Dependent Setup
// 
//----------------------------------------------------------------

// System header files 

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/times.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/file.h>
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
#define APP_VERSION "1.0.0" //!< placeholder version number, set in Makefile
#endif

#ifndef APP_COMPDATE
#define APP_COMPDATE "2025-07-01" //!< placeholder compilation date, set by build script
#endif

#ifndef APP_COMPTIME
#define APP_COMPTIME "00:00:00" //!< placeholder compilation time, set by build script
#endif

// Useful working parameters

#ifndef MAXCFGLINE
#define MAXCFGLINE 128 //!< Maximum characters/line in runtime config files
#endif

// ISIS common client utilties library header

#include "isisclient.h"    // should be in -I path in Makefile, no paths here!

extern isisclient_t client;  // global client runtime config table

#include "azcam.h"   // azcam clinet utilities header

extern azcam_t ccd;  // azcam server info data structure

//#include "dataman.h" // Data Manager (dataMan) agent header

//extern dataman_t dm;

//---------------------------------------------------------------------------

/*!
  \brief obsPars: azcam client observation parameters data structure

  Contains the parameters related to an observation made with the azcam
  system.  Most of this information is eventually stored in the image
  FITS headers.

*/

typedef struct obsPars {

  // Image parameters

  char imgTitle[64];  //!< image title (goes into OBJECT keyword).
  char imgType[12];   //!< image type, one of {object,bias,flat,dark,comp,zero}

  // Exposure Parameters 

  double expTime;    //!< integration time in seconds
  double tStart;     //!< time at start of integration
  double tNow;       //!< time right now
  double tLeft;      //!< time remaining in the integration
  double keepAlive;  //!< remote host keep-alive notification interval in seconds
  int doCountDown;   //!< countdown timer flag

  // Exposure control info

  char expHost[ISIS_NODESIZE]; //!< IMPv2 node name of the client requesting the exposure

  // Info required for the LBTO Archive for data ownership tracking

  char Observer[64];   //!< name(s) of the observer(s)
  char Partner[40];    //!< LBT project partner ID(s)
  char PropID[40];     //!< LBT observing proposal ID
  char PIName[40];     //!< Project PI names(s)
  char Support[40];    //!< LBT support scientist(s) on duty
  char TelOps[40];     //!< LBT telescope operator(s) on duty

  // Miscellaneous

  char instID[12];     //!< instrument ID (e.g., MODS1B)
  double t1;           //!< diagnostic timetag (1 of 2)
  double t2;           //!< diagnostic timetag (2 of 2)

} obsPars_t;

extern obsPars_t obs;  // observation info for client (declare in main)

//----------------------------------------------------------------
//
// Custom client application function prototypes 
//
 
int  loadConfig(char *);      // Load/parse the agent runtime config file (see config.c)
int  saveConfig(char *);      // Save the agent runtime configuration in a file (see config.c)
void KeyboardCommand(char *); // process keyboard (cli) commands (see commands.c)
void SocketCommand(char *);   // process messages from the client socket (see commands.c)

// Client utility routines

int  notifyClient(azcam_t *, obsPars_t *, char *, MsgType);
int  doExposure(azcam_t *, obsPars_t *, char *);
int  pollExposure(azcam_t *, obsPars_t *, char *);
int  pollReadout(azcam_t *, obsPars_t *, char *);

int  doBias(azcam_t *, obsPars_t *, char *);

int  writeCCDImage(azcam_t *, obsPars_t *, char *);
int  initCCDConfig(azcam_t *, char *);
int  uploadFITS(azcam_t *, obsPars_t *, char *);

void initObsPars(obsPars_t *);
int  processImage(azcam_t *, obsPars_t *, char *, char *);
int  readTemps(azcam_t *, char *);

// Signal Handlers

void HandleInt(int);  // SIGINT handler

#endif  // CLIENT_H
