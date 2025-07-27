#ifndef ISISSERVER_H
#define ISISSERVER_H

//
// Main ISIS header file
//
// R. Pogge, OSU Astronomy Dept.
// pogge.1@osu.edu
// 2003 January 20
//
// Updated:
//   2024 Feb 19 - v3.0 for AlmaLinux 9.1
//   2025 Jul 27 - v3.1 for AlmaLinux 9.3, pre-installation

/*!
  \file isisserver.h
  \brief ISIS server application main header
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
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
#include <ctype.h>

// In case the version and compilation data are not defined at
// compilation, put in some placeholders to prevent code barfing

#ifndef ISIS_VERSION
#define ISIS_VERSION "0.0"          //!< Version number placeholder
#endif

#ifndef ISIS_COMPDATE
#define ISIS_COMPDATE "0000-00-00"  //!< Compilation date placeholder
#endif

#ifndef ISIS_COMPTIME
#define ISIS_COMPTIME "00:00:00"    //!< Compilation time placeholder
#endif

#ifndef ISIS_CONFIG
#define ISIS_CONFIG  "/home/dts/Config/isis.ini"   //!< ISIS config 
#endif

#ifndef ISIS_DCONFIG
#define ISIS_DCONFIG  "/home/dts/Config/isisd.ini"   //!< isisd daemon config file
#endif

#ifndef ISIS_LOGS
#define ISIS_LOGS  "/home/Logs/ISIS/isis.log"   //!< ISIS runtime log
#endif

// Various site-dependent but system-independent default values 

#define DEFAULT_LOCALHOST "IS"  //!< Default ISIS server host ID
#define DEFAULT_ISISPORT  0  //!< default ISIS server port number

//
// END of Site-Dependent Setup
//
// Touch the stuff below this at your own risk..
//
//--------------------------------------------------------------------------

// String sizes 

#define POSIX

#define ISIS_MSGSIZE      4096  //!< ISIS maximum message size
#define IMPv2_HOST_SIZE   9     //!< maximum size of IMPv2 host names 8+1 
#define LONG_STR_SIZE     4096  //!< size of generic a long string
#define MED_STR_SIZE      256   //!< size of generic a medium-sized string
#define BIG_STR_SIZE      2048  //!< size of generic a big string
#define SHORT_STR_SIZE    32    //!< size of generic a short string

// Some useful flags 

#define isis_TRUE       1       //!< condition is TRUE value
#define isis_FALSE      0       //!< condition is FALSE value
#define NUL             '\0'    //!< convenience definition for null string

// Pseudo Client Indices for the server and broadcast 

#define ISIS_SERVER   777       //!< host table index for the server ("us")
#define BROADCAST     666       //!< host table index for broadcast (host AL)

// Message action flags 

#define MSG_NOOP    0  //!< No reply or forwarding required              
#define MSG_REPLY   1  //!< Send a reply back to the sender              
#define MSG_FORWARD 2  //!< Forward message to the destination           
#define MSG_ECHO    3  //!< Echo the message unprocessed                 

// Message erorr codes 

#define ERR_UNKNOWN_HOST  -1  //!< Recipient is an unknown host                 
#define ERR_UNKNOWN_FD    -2  //!< File descriptor is unknown to the host table 
#define ERR_HOSTS_FULL    -3  //!< Server's client host table is full 

//--------------------------------------------------------------------------
//
// ISIS server global system table structure 
//
// Parameters are loaded from the isis.ini file
//

/*!
  \brief ISIS server application runtime system table

  Contains the runtime parameters of the ISIS server application.
*/

extern struct server {
  char serverID[IMPv2_HOST_SIZE];     //!< Server Host ID (IMPv2 style)     
  int  sockFD;                        //!< Server socket file descriptor 
  int  sockPort;                      //!< Server socket port number        
  char sockAddr[INET6_ADDRSTRLEN];    //!< Server socket IP Address (IPv4)
  char hostName[64];                  //!< Server's full resolved hostname
  char localHost[64];                 //!< Name of the local host (w/o domain)
  char startTime[24];                 //!< UTC time the server was started
  char userID[SHORT_STR_SIZE];        //!< User who launched the server
  int  keepGoing;                     //!< Server "Keep Going" flag
  int  isVerbose;                     //!< Server Verbose/Silent output flag
  int  useCLI;                        //!< Server uses the CLI, otherwise running with no CLI (e.g., daemon)
  int  doLogging;                     //!< Server runtime logging enabled flag
  int  doStartup;                     //!< Do server comm init at startup
  int  doHandShaking;                 //!< Do port handshaking at startup
  char utcDate[SHORT_STR_SIZE];       //!< UTC Date CCYY-MM-DD format    
  char utcTime[SHORT_STR_SIZE];       //!< UTC Time hh:mm:ss format      
  char dateTag[SHORT_STR_SIZE];       //!< UTC date tag CCYYMMDD format  
  char lastDate[SHORT_STR_SIZE];      //!< Most recent date tag    
  char exeFile[MED_STR_SIZE];         //!< path/name of server executable file 
  char iniFile[MED_STR_SIZE];         //!< path/name of the server config file
  char logRoot[MED_STR_SIZE];         //!< path/rootname for server log files
  char logFile[MED_STR_SIZE];         //!< name of the current server log file
  int  logFD;                         //!< Server log file descriptor
  int  logDate;                       //!< Server log date convention, either UTCDATE or OBSDAY
  int  numClients;                    //!< Number of connected clients   
  int  numSerial;                     //!< Number of serial ports   
  int  numPreset;                     //!< Number of preset UDP socket ports
  char instID[MED_STR_SIZE];          //!< ID of instrument connected
} isis;

// ISIS Start/Restart Flags 

#define NO_STARTUP   0  //!< no startup tasks required
#define COLD_START   1  //!< cold startup
#define WARM_START   2  //!< warm startup

// ISIS logging date conventions

#define UTCDATE      0  //!< Logs use the UTC date
#define OBSDAY       1  //!< Logs use the noon-to-noon local date [Default]

//--------------------------------------------------------------------------
//
// ISIS Client Table
//
 
#define MAXCLIENTS 32  //!< Maximum number of client hosts 

/*!
  \brief ISIS server client table

  Contains the parameters of known ISIS clients.  ISIS clients are
  added to the client table every time they pass messages through the
  server.  This allows clients to come and go dynamically.

  The maximum number of allowed clients is #MAXCLIENTS.

*/

extern struct clients {
  char ID[IMPv2_HOST_SIZE];     //!< Client host ID (IMPv2 protocol)           
  int  method;                  //!< Client transport method
  int  fd;                      //!< File descriptor for client I/O port
  long addr;                    //!< Client IP address if method=SOCKET 
  int  port;                    //!< Client port @ addr if method=SOCKET or serial table index 
  double tstamp;                //!< Time since last message in seconds since UTC1970-01-01
} clientTab[MAXCLIENTS];

// Client transport method codes 

#define UNASSIGNED 0   //!< No transport method assigned to client
#define SERIAL     1   //!< Client uses a serial port
#define SOCKET     2   //!< Client uses a UDP network socket

//--------------------------------------------------------------------------
//
// ISIS serial port table
//

#define MAXSERIAL 8  //!< maximum number of serial port connections 

/*!
  \brief ISIS server serial port table

  Contains the parameters of serial ports opened by the ISIS server.
  The maximum number of allowed ports opened at startup is #MAXSERIAL.
*/

extern struct serial {
  char devName[MED_STR_SIZE];  //!< Serial port physical device name
  int  fd;                     //!< Serial port file descriptor
} ttyTab[MAXSERIAL];

//--------------------------------------------------------------------------
//
// ISIS preset UDP socket port table
//
// May define up to MAXPRESET UDP socket port targets to ping
// on startup or restart.  These are not put into the host table
// unless we get a response from one of the ports.
//
// increased to 12 - the 2025 system has more agents
//

#define MAXPRESET  12  //!< Maximum number of preset UDP ports

/*!
  \brief ISIS preset UDP socket port table

  Contains the parameters of up to #MAXPRESET preset UDP socket hosts
  and ports.  Preset UDP ports are pinged on startup and restart to
  re-establish connections on server restart with active clients.
*/

extern struct udpPreset {
  char host[MED_STR_SIZE];  //!< UDP socket host name (IP or full name)
  int  port;                //!< UDP socket port number
} udpTab[MAXPRESET];

//--------------------------------------------------------------------------
//
// Function prototypes 
//
 
// Server startup and shutdown 

int  openSocket(int );
void closeSocket(int );
int  loadConfig(char *);
void handShake(void);

// Client Host Table Functions 

void initHostTable();
int  updateHosts(char *, int, int, long, int);
int  removeHost(char *);
int  isKnownHost(char *);
void printHosts(char *);
void hostInfo(char *, char *);

// Serial I/O Utilties 

int  initSerialPorts(void);
int  getSerialInfo(int );
void portInfo(char *);

// I/O handlers 

void ttyHandler(char *line);
void socketHandler(int );
void serialHandler(int );
void sendMessage(int, char *);
void handleMessage(int, int, char *);
void broadcastMessage(int, char *);

// Command Utilities 

int  isisCommand(char *, char *);
int  serverInfo(char *);
void printInfo(void);
void printHelp(void);

// Runtime Logging Utilities 

void initLog(void);
int  logMessage(char *);

// Utilities 

void   getUTCTime(void);
char   *getFineTime(void);
char   *getDateTime(void);
double sysTimeStamp(void);
char   *noonDateTag(void);
void   getArg(char *, int, char *);
void   upperCase(char *);

#endif  // ISISSERVER_H
