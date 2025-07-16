#ifndef CLIENT_H
#define CLIENT_H

//
// client.h - Custom client application header
//

/*!
  \file client.h
  \brief AGW Client Application Header

  ISIS client application header for the MODS AGW control application.

  \date 2005 February 16
*/

// Various site-dependent but system-independent default values 

#ifndef APP_CFGFILE
#define APP_CFGFILE "./agw.ini"
#endif

// Default client application values (override/set in loadconfig.c)

#define DEFAULT_MYID      "AGW"  //!< default client ISIS node name
#define DEFAULT_MYPORT    10433  //!< default client socket port   
#define DEFAULT_RCFILE    APP_CFGFILE //!< default client runtime config file

// Default ISIS server information (see loadconfig.c if used)

#define DEFAULT_ISISID   "IS"    //!< default ISIS server node name
#define DEFAULT_ISISHOST "isis"  //!< default ISIS server host
#define DEFAULT_ISISPORT 6600    //!< default ISIS server port number

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
#define APP_VERSION "0.0.0" //!< placeholder version number, set in Makefile
#endif

#ifndef APP_COMPDATE
#define APP_COMPDATE "2004-01-01" //!< placeholder compilation date, set by build script
#endif

#ifndef APP_COMPTIME
#define APP_COMPTIME "00:00:00" //!< placeholder compilation time, set by build script
#endif

// ISIS common client utilties library header

#include "isisclient.h"  // should be in -I path in Makefile, no paths here!

extern isisclient_t appClient;  // global client runtime config table

// This app uses the agwutils.h header to define an AGW stage instance

#include "agwutils.h"

extern agwpar_t appAGW; // global AGW mechanism configuration table

//----------------------------------------------------------------
//
// Custom client application function prototypes 
//
 
int LoadConfig(agwpar_t *, isisclient_t *, char *);  // Load/parse the agent runtime config file (see loadconfig.c)
void GCSCommand(char *, char []);  // process (gcs) commands (see commands.c)

// Signal Handlers

void HandleInt(int);  // SIGINT handler
void abortall(void);  // abort all moves
double positionToShrMem(int,char []);  // send position to shared memory, and return value to caller
int mlcCheckBits(int,char []);  // check bits 21 - 26
int sendCommand(int, char [], char []);  // send a command
int sendTwoCommand(int, char [], char [], char []);  // send two commands
int mlcCheckMovingBit(int, char []);  // check mechanism moving bit
int mlcCheckPLCBit(int, char []);  // check mechanism PLC bit
int mlcCheckStatusBits(int,char []);  // check mechanism status bits
int mlcLiveCheck(int, char []);  // check if mechanism is alive

#endif  // CLIENT_H
