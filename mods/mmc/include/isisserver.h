#ifndef ISISSERVER_H
#define ISISSERVER_H

//
// Main ISIS header file
//
// R. Pogge, OSU Astronomy Dept.
// pogge@astronomy.ohio-state.edu
// 2003 January 20
//

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

// Various site-dependent but system-independent default values 

// These are for ISIS running on the isis at OSU 

#define DEFAULT_LOCALHOST "IS"  //!< Default ISIS server host ID
#define DEFAULT_INIFILE   "/home2/isl/Config/isis.ini"  //!< default server runtime config file
#define DEFAULT_LOGFILE   "/home2/log/isis.log"  //!< default server runtime log file
#define DEFAULT_ISISPORT  0  //!< default ISIS server port number

//
// END of Site-Dependent Setup
//
// Touch the stuff below this at your own risk..
//
//--------------------------------------------------------------------------

// String sizes 

#define POSIX

#define ISIS_MSGSIZE      2048  //!< ISIS maximum message size
#define IMPv2_HOST_SIZE   9     //!< maximum size of IMPv2 host names 8+1 
#define LONG_STR_SIZE     4096  //!< size of generic a long string
#define MED_STR_SIZE      256   //!< size of generic a medium-sized string
#define BIG_STR_SIZE      512   //!< size of generic a big string
#define SHORT_STR_SIZE    32    //!< size of generic a short string

// Some useful flags 

#define isis_TRUE       1       //!< condition is TRUE value
#define isis_FALSE      0       //!< condition is FALSE value
#define NUL             '\0'    //!< convenience definition for null string

// Pseudo Client Indices for the server and broadcast 

#define ISIS_SERVER   777       //!< host table index for the server ("us")
#define BROADCAST     666       //!< host table index for broadcast (host AL)

// Message action flags 

#define DoNothing     0  //!< No reply or forwarding required              
#define ReplyBack     1  //!< Send a reply back to the sender              
#define ForwardMsg    2  //!< Forward message to the destination           
#define EchoMsg       3  //!< Echo the message unprocessed                 

// Message erorr codes 

#define MessageOK     0  //!< Valid IMPv2 Message
#define UnknownHost  -1  //!< Recipient is an unknown host                 
#define BlankMessage -2  //!< Message body was blank                       
#define BadMessage   -3  //!< Message was malformed (not IMPv2 compliant)
#define UnknownFD    -4  //!< File descriptor is unknown to the host table 
#define HostsFull    -5  //!< Server's client host table is full 
#define UnknownType  -6  //!< Host interface method is unknown

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
  char ServerID[IMPv2_HOST_SIZE];     //!< Server Host ID (IMPv2 style)     
  int  SockFD;                        //!< Server socket file descriptor 
  int  SockPort;                      //!< Server socket port number        
  char SockAddr[INET6_ADDRSTRLEN];    //!< Server socket IP Address (IPv4)
  char HostName[64];                  //!< Server's full resolved hostname
  char LocalHost[64];                 //!< Name of the local host (w/o domain)
  char StartTime[24];                 //!< UTC time the server was started
  char UserID[SHORT_STR_SIZE];        //!< User who launched the server
  int  KeepGoing;                     //!< Server "Keep Going" flag
  int  Verbose;                       //!< Server Verbose/Silent output flag
  int  Logging;                       //!< Server runtime logging enabled flag
  int  DoStartup;                     //!< Do server comm init at startup
  int  Handshaking;                   //!< Do port handshaking at startup
  int  TCS;                           //!< TCS interface code (unimplemented)
  char UTCDate[SHORT_STR_SIZE];       //!< UTC Date CCYY-MM-DD format    
  char UTCTime[SHORT_STR_SIZE];       //!< UTC Time hh:mm:ss format      
  char DateTag[SHORT_STR_SIZE];       //!< UTC date tag CCYYMMDD format  
  char LastDate[SHORT_STR_SIZE];      //!< Most recent UTC date tag    
  char exeFile[MED_STR_SIZE];         //!< path/name of server executable file 
  char iniFile[MED_STR_SIZE];         //!< path/name of server config file
  char logFile[MED_STR_SIZE];         //!< path/name of server log file
  char pgmFile[MED_STR_SIZE];         //!< path/name of server procedure file
  int  logFD;                         //!< Server log file descriptor
  int  NumClients;                    //!< Number of connected clients   
  int  NumSerial;                     //!< Number of serial ports        
  int  NumFIFOs;                      //!< Number of fifo pipe pairs     
  char InstID[MED_STR_SIZE];          //!< ID of instrument connected
} isis;

// TCS interface types (unimplemented)

#define NOTCS   0  //!< No TCS enabled 
#define MDMTCS  1  //!< MDM xtcs TCS system (unimplemented)
#define FAUXTCS 2  //!< Lab FauxTCS simulator (unimplemented)
#define PCTCS   3  //!< PC-TCS system (unimplemented)
#define LBTTCS  4  //!< LBT TCS system (unimplemented)
#define SOARTCS 5  //!< SOAR TCS system (unimplemented)

// ISIS Start/Restart Flags 

#define NO_STARTUP   0  //!< no startup tasks required
#define COLD_START   1  //!< cold startup
#define WARM_START   2  //!< warm startup

//--------------------------------------------------------------------------
//
// ISIS Client Table
//
 
#define MAXCLIENTS 16  //!< Maximum number of client hosts 

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
  int  port;                    //!< Client port @ addr if method=SOCKET or serial/fifo table index 
  double tstamp;                //!< Time since last message in seconds since UTC1970-01-01
} cli_tab[MAXCLIENTS];

// Client transport method codes 

#define UNASSIGNED 0   //!< No transport method assigned to client
#define SERIAL     1   //!< Client uses a serial port
#define SOCKET     2   //!< Client uses a UDP network socket
#define FIFO       3   //!< Client uses FIFO pipes (unimplemented)
#define RPC        4   //!< Client uses RPC (unimplemented)

//--------------------------------------------------------------------------
//
// ISIS serial port table
//

#define MAXSERIAL 4  //!< maximum number of serial port connections 

/*!
  \brief ISIS server serial port table

  Contains the parameters of serial ports opened by the ISIS server.
  The maximum number of allowed ports opened at startup is #MAXSERIAL.
*/

extern struct serial {
  char devname[MED_STR_SIZE];  //!< Serial port physical device name
  int  fd;                     //!< Serial port file descriptor
} tty_tab[MAXSERIAL];

//--------------------------------------------------------------------------
//
// Function prototypes 
//
 
// Server startup and shutdown 

int  OpenISISSocket(int );
void CloseISISSocket(int );
int  LoadConfig(char *);
void DoHandshaking(void);

// Client Host Table Functions 

void InitHostTable();
int  UpdateHosts(char *, int, int, long, int);
int  RemoveHost(char *);
int  IsKnownHost(char *);
void PrintHosts(char *);
void HostInfo(char *, char *);

// Serial I/O Utilties 

int  InitSerialPorts(void);
int  GetSerialInfo(int );
void PortInfo(char *);

// I/O handlers 

void TTYHandler(char *line);
void SocketHandler(int );
void SerialHandler(int );
void SendMessage(int, char *);
void HandleMessage(int, int, char *);
void BroadcastMessage(int, char *);

// Command Utilities 

int  ISISCommand(char *, char *);
int  ServerInfo(char *);
void PrintInfo(void);
void PrintHelp(void);

// Runtime Logging Utilities 

void InitLog(void);
int  LogMessage(char *);

// Utilities 

void   GetUTCTime(void);
char   *GetFineTime(void);
double SysTimestamp(void);
void GetArg(char *, int, char *);
void UpperCase(char *);

#endif  // ISISSERVER_H
