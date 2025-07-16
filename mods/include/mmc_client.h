#ifndef ICS_CLIENT_H
#define ICS_CLIENT_H

//
// ics_client.h - Custom client application header
//

/*!
  \file ics_client.h
  \brief ICS Application Header

  header for the ics application.

  \date 2005 May 05
*/

// Various site-dependent but system-independent default values 

// Default client application values (override/set in loadconfig.c)

#define DEFAULT_MYID      "M1.IE"  //!< default client ISIS node name
#define DEFAULT_MYPORT    7101   //!< default client socket port   
#define DEFAULT_RCFILE    (char*)"/home/dts/mods/Config/mechanisms.ini" //!< default client runtime config file
#define DEFAULT_LOGFILE   (char*)"/home/dts/Logs/MMC/mmc.log" //!< default client runtime log file (unimplemented)
#define DEFAULT_AGWLOG    (char*)"/home/dts/Logs/AGW/agw.log" //!< default client runtime log file (unimplemented)

// Default ISIS server information (see loadconfig.c if used)

#define DEFAULT_ISISID   "IS"    //!< default ISIS server node name
#define DEFAULT_ISISHOST "mods1" //!< default ISIS server host
#define DEFAULT_ISISPORT 6600    //!< default ISIS server port number

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
//----------------------------------------------------------------
//
// Custom client application function prototypes 
//
 
int LoadConfig(char *);  // Load/parse the agent runtime config file (see loadconfig.c)
void KeyboardCommand(char *);  // process keyboard (cli) commands (see commands.c)
void KeyCommand(char *, char []);  // isl commands (see commands.c)
void SocketCommand(char *);    // process messages from the client socket (see commands.c)
void SocketCmd(char *, char []);    // process messages from the client socket (see commands.c)
void logisl(char*,char*,char*,char*);
void logitf(char *);
void logitTO(char *);
void logitTR(char *);
extern void logitS(char*);
void logit(char*,int,char*);
void logite(char*,int,char*);
void logitn(char*,int,char*,int);
void logita(char*,int,char*,char*);
void setup_ids();
void skd_run(char[],char,long[]);
void skd_par(long []);
void putpname(char *);
void skd_islmgr_inject_w(long *, char *,int );
int skd_end_inject_qc(char [],long []);
void cls_snd(long *,char *,int,int,int);
int cls_rcv(long ,char*,int,int*,int*,int,int);
void cls_clr(long);
int cls_rel(key_t);
void skd_wait(char [],long [],unsigned);
int rte_prior(int);
unsigned rte_alarm(unsigned);
unsigned rte_sleep(unsigned);
void rte_time(int [],int *);
void rte_rawt(long *);
long rte_secs();
void skd_run(char*, char, long[]);
int nsem_test(char []);
extern int MilliSleep(long msec);
extern int SCLOpenPort(char*);

// Signal Handlers

void HandleInt(int);  // SIGINT handler
void abortall(void);  // abort all moves
double positionToShrMem(int, char []);  // send position to shared memory, and return value to caller
int mlcCheckBits(int,char []);  // check bits 21 - 26
int mlcCheckIO30(int,char []);  // check IO 30 which holds bits 31-36 mask
int sendCommand(int, char [], char []);  // send a command to the mechanism
int mlcCheckMovingBit(int, char []);  // check mechanism moving bit
int mlcCheckStatusBits(int, char []);  // check mechanism status bits
int checkForError(int, char *);  // check mechanism status bits
int mlcRecover(int, char *);  // check mechanism status bits

#endif  // CLIENT_H
