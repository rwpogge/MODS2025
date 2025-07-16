#ifndef ISLUTILS_H
#define ISLUTILs_H

//
// islutils.h - Custom client application header
//

/*!
  \file islutils.h
  \brief Application Header

  header for application.

  \date 2005 May 05

  2025 Jun 18 - AlmaLinux 9 port [rwp/osu]

*/

// Various site-dependent but system-independent default values 
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

// Serial (comtrol box) port I/O

#include "instrutils.h"   // INSTRUtils comtrol IP:Socket Serial I/O API header

//extern ttyport_t commport[MAX_ML];
//----------------------------------------------------------------
//
// Custom client application function prototypes 
//
 
int LoadConfig(char *);  // Load/parse the agent runtime config file (see loadconfig.c)
void KeyboardCommand(char *);  // process keyboard (cli) commands (see commands.c)
void SocketCommand(char *);    // process messages from the client socket (see commands.c)
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
void shm_att(key_t);
int  shm_det();
void skd_run(char[],char,long[]);
void skd_par(long []);
void putpname(char *);
void skd_islmgr_inject_w(long *, char *,int );
int  skd_end_inject_qc(char [],long []);
void cls_snd(long *,char *,int,int,int);
int  cls_rcv(long ,char*,int,int*,int*,int,int);
void cls_clr(long);
int  cls_rel(key_t);
void skd_wait(char [],long [],unsigned);
int  rte_prior(int);
unsigned rte_alarm(unsigned);
unsigned rte_sleep(unsigned);
void rte_time(int [],int *);
void islcmd(int, char [], int);
void skd_run(char*, char, long[]);
void displayP(int, int, char[], char []);
int  nsem_test(char []);
extern int MilliSleep(long msec);
extern int SCLOpenPort(char*);

// Signal Handlers

void HandleInt(int);  // SIGINT handler
void abortall(void);  // abort all moves

#endif  // ISLUTILS_H
