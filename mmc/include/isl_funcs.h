#ifndef ISL_FUNCS_H
#define ISL_FUNCS_H

//
// isl_funcs.h - isl function declarations
//

/*!
  \file isl_funcs.h
  \brief isl function declarations

  \author Staff, OSU Astronomy Dept. (rgonzale@astronomy.ohio-state.edu)
  \date 2004 Mar 13

  \par Modification History:

  \date 2025 Jun 18 - AlmaLinux 9 port [rwp/osu]

<pre>
</pre>
*/

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

// Serial Port Parameters
#define TTY_UNKNOWN 0 //!< Port interface unknown (should be starting default)
#define TTY_SERIAL  1 //!< Port is a direct serial device on the host computer
#define TTY_NETWORK 2 //!< Port is a serial port connected through a TCP network port server

// Isl_Funcs Function Prototypes

int LoadConfig(char *);  // Load/parse the agent runtime config file (see loadconfig.c)
void KeyboardCommand(char *);  // process keyboard (cli) commands (see commands.c)
void SocketCommand(char *);    // process messages from the client socket (see commands.c)
int islSysTask(char []);
void logisl(char*,char*,char*,char*);
void logitf(char *);
void StrToUpper(char *);
void islLogMsg(char *);
void logitTO(char *);
void logitTR(char *);
void logitNL(char *);
extern void logitS(char*);
void logit(char*,int,char*);
void logite(char*,int,char*);
void logitn(char*,int,char*,int);
void logita(char*,int,char*,char*);
void setup_ids();
void sig_ignore();
void skd_run(char[],char,long[]);
void skd_run_arg(char[],char,long[],char *);
void skd_arg(int, char *, int);
void skd_par(long []);
void skd_end(long []);
void putpname(char *);
void skd_islmgr_inject_w(long *, char *,int );
int skd_end_inject_qc(char [],long []);
void cls_snd(long *,char *,int,int,int);
int cls_rcv(long ,char*,int,int*,int*,int,int);
void cls_clr(long);
void cls_alc(long);
int cls_rel(key_t);
void skd_wait(char [],long [],unsigned);
int rte_prior(int);
unsigned rte_alarm(unsigned);
unsigned rte_sleep(unsigned);
void rte_time(int [],int *);
void skd_run(char*, char, long[]);
int nsem_test(char []);
int rmcrlf(char [], char []);
int islcmd(int, char [], int);
int islsmc(int, char []);
//int getComm(int, char []);
int getComm(int, char [], float *);
int setComm(int, char []);
int smccmd(int, char [], int);
int commcmd(int, char []);
int smcmech(char[], char [], float);
int smcbusy(int);
int smcrelease(int);
void display_it(int, int, char*, char*);
void flt2str(char *, float, int, int);
void int2str(char *, int,int,int);
void cnvrt(int,double,double,double *,double *,int [6],double,double);
void cnvrt2(int,double,double,double *,double *,int [6],float,double,double);
void cnvrt0(int,double,double,double *,double *,double,double,double);

extern int MilliSleep(long msec);
extern int SCLOpenPort(char*);

// Signal Handlers

void HandleInt(int);  // SIGINT handler
void abortall(void);  // abort all moves

int OpenTTYPort(ttyport_t *);
int SetTTYPort(ttyport_t *);
void CloseTTYPort(ttyport_t *);
int WriteTTYPort(ttyport_t *, char *);
int ReadTTYPort(ttyport_t *, char *, long);
void FlushTTYPort(ttyport_t *, int);
int TTYMSleep(long);

#endif  // ISL_FUNCS_H

