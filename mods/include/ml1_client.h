#ifndef CLIENT_H
#define CLIENT_H

//
// client.h - Custom client application header
//

/*!
  \file client.h
  \brief MULTISC Application Header

  ISIS client application header for the userin application.

  \date 2005 May 05
*/

// Various site-dependent but system-independent default values 

// Default client application values (override/set in loadconfig.c)

#define DEFAULT_MYID      "ML1"  //!< default client ISIS node name
#define DEFAULT_MYPORT    7101   //!< default client socket port   
#define DEFAULT_RCFILE    "/home2/isl/Config/ml1.ini" //!< default client runtime config file
#define DEFAULT_LOGFILE   "/home2/log/isl.log" //!< default client runtime log file (unimplemented)

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
#define APP_VERSION "1.0 Beta" //!< placeholder version number, set in Makefile
#endif

#ifndef APP_COMPDATE
#define APP_COMPDATE "2004-01-01" //!< placeholder compilation date, set by build script
#endif

#ifndef APP_COMPTIME
#define APP_COMPTIME "00:00:00" //!< placeholder compilation time, set by build script
#endif

// ISIS common client utilties library header

#include "isisclient.h"  // should be in -I path in Makefile, no paths here!

extern isisclient_t client;  // global client runtime config table

// Serial <Commport> port I/O

#include "instrutils.h"   // INSTRUtils I/O API header

extern ttyport_t commport[MAX_ML];

//----------------------------------------------------------------
//
// Custom client application function prototypes 
//
 
int LoadConfig(char *);  // Load/parse the agent runtime config file (see loadconfig.c)
void KeyboardCommand(char *);  // process keyboard (cli) commands (see commands.c)
void SocketCommand(char *);    // process messages from the client socket (see commands.c)
void logisl(char*,char*,char*,char*);
void logitf(char *);
extern void logitS(char*);
void logit(char*,int,char*);
void logite(char*,int,char*);
void logitn(char*,int,char*,int);
void logita(char*,int,char*,char*);
void setup_ids();
void skd_run(char[],char,long[]);
void skd_par(long []);
void setup_ids();
void putpname(char *);
void cls_snd(long *,char *,int,int,int);
extern int MilliSleep(long msec);
extern int SCLOpenPort(char*);

// Signal Handlers

void HandleInt(int);  // SIGINT handler
void abortall(void);  // abort all moves

#endif  // CLIENT_H
