#ifndef ISISCLIENT_H
#define ISISCLIENT_H

/*!
  \mainpage ISISClient API Library
  \author Richard Pogge, OSU Astronomy Department (pogge\@astronomy.ohio-state.edu)

  \section Introduction

  The ISISClient API library provides all of the basic utilities needed
  to create ISIS client applications.  These include

  <ul>
  <li>IMPv2 message handling functions (isismessage.c)
  <li>UDP Socket I/O functions (isissocket.c)
  <li>Serial Port I/O functions (isisserial.c)
  <li>Common client string handling and time utilities (isisutils.c)
  </ul>

  An ISIS client application needs only include the isisclient.h header
  and link to the libisis.a library to access these functions.

  In addition to the ISIS client library, most client applications make
  use of a number of common code stubs for loading/parsing runtime config
  files, keyboard and socket I/O, and command parsing and execution.
  These are described in a separate document.

  \section Make Compilation and Dependencies

  The Makefile for libisis.a is setup to compile using g++.
  Applications linking to libisis.a need to also link to the C math
  library (-lm), but there are no other dependencies in this version.

  \section Notes

  \section Mods Modification History
<pre>
2003 Sep 15 - First release for beta testing [rwp/osu]
2003 Nov 2  - Cleaned up UDP socket api code, lots-o-bugs [rwp/osu]
2004 Mar 18 - Numerous changes during CTIO 1-m deployment run-up,
              much cleaner ways of handling globals (What was I thinking?)
              and added new functions [rwp/osu]
2004 May 9  - Added comment blocks with Doxygen markup, first crack at
              using Doxygen for this stuff.  Also first compile with g++,
              not as bad as it could have been, but much cleaner now. [rwp/osu]
2004 Jun 9  - Introduced isisclient_t typedef for creating common isisclient
              runtime configuration tables for all client applications. [rwp/osu]
2004 Jul 22 - Overhauled API (v2.0) to correct various problems that emerged
              during testing, and eliminated much clumsiness in v1.x [rwp/osu]
</pre>
*/

/*!
  \file isisclient.h
  \brief ISIS client API header file.

  This is the main header file for the ISIS client applications library
  (libisis.a).  Client applications only need to include isisclient.h,
  which encapsulates all of the various bits a client needs to become a
  basic ISIS client application.

  The main client library is divided into 4 modules:
  
  \arg \c isismessage.c IMPv2 message handling routines
  \arg \c isissocket.c ISIS UDP socket I/O handling routines
  \arg \c isisserial.c Serial port I/O handling routines 
  \arg \c isisutils.c Handy client application utilties (string and time handling)

*/

// System header files

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <errno.h>
#include <termios.h>   
#include <fcntl.h>   
#include <sys/time.h>
#include <sys/times.h>
#include <sys/file.h>
#include <sys/types.h> 
#include <sys/stat.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h> 
#include <netdb.h>

// Global definitions common to all ISIS client applications

#define ISIS_MSGSIZE 2048   //!< Maximum size of an IMPv2 message
#define ISIS_NODESIZE   9   //!< Maximum size of an IMPv2 host node name

// Typical string lengths 

#define LONG_STR_SIZE     4096  //!< Length of a generic long string (longer than ISIS_MSGSIZE)
#define MED_STR_SIZE      512   //!< Length of a generic medium-sized string
#define BIG_STR_SIZE      1024   //!< Length of a generic "big" string, bigger than medium, smaller than long
#define SHORT_STR_SIZE    32    //!< Length of a generic short string  

//----------------------------------------------------------------
//
// client_info: client information struct
//

/*!
  \brief ISIS client runtime configuration table

  Contains the runtime parameters common to all ISIS client
  applications.  Defines the isisclient_t typedef to be used by client
  applications.
*/

typedef struct client_info
{
  // ISIS server info
  int    useISIS;                  //!< 1 = ISIS client, 0 = standalone mode
  char   isisID[ISIS_NODESIZE];    //!< ISIS Server Node ID (IMPv2 style) 
  char   isisHost[MED_STR_SIZE];   //!< ISIS server's hostname
  int    isisPort;                 //!< ISIS server's socket port
  sockaddr_in isisAddr;            //!< ISIS server's network socket address database

  // ISIS client info
  int    FD;                       //!< Client socket file descriptor  
  char   ID[ISIS_NODESIZE];        //!< Client Node ID (IMPv2 style)      
  char   Host[MED_STR_SIZE];       //!< Client hostname (localhost)
  int    Port;                     //!< Client socket port              

  // Remote client info (for STANDALONE mode)
  char   remHost[MED_STR_SIZE];    //!< hostname of a remote socket host
  int    remPort;                  //!< port number of a remote socket host
  sockaddr_in remAddr;             //!< remote host's network socket address database

  // Client runtime info and flags
  char   rcFile[MED_STR_SIZE];     //!< path/name of the runtime config file
  int    KeepGoing;                //!< Client "Keep Going" flag       
  int    isVerbose;                //!< Client Verbose/Silent flag 
  int    Debug;                    //!< client level debugging flag
  int    doLogging;                //!< runtime log enable/disable flag
  char   logFile[MED_STR_SIZE];    //!< path/name of the runtime log file

} isisclient_t;

//----------------------------------------------------------------
//
// IMPv2 Message Types
//

/*!
  \enum IMPv2_MsgType
  
  This enum is used by ISIS client routines to specify the IMPv2 message
  type.
*/

/*!
  \var typedef enum IMPv2_MsgType MsgType

  MsgType is a global typedef used by isismessage utilities and any user
  routines must access IMPv2 message-type codes.
*/

// Message Types:

enum IMPv2_MsgType {
  REQ,     //!< Command Request
  EXEC,    //!< Executive Command Request
  STATUS,  //!< Informational Status message
  DONE,    //!< Command completion acknowledgment
  ERROR,   //!< Error message
  WARNING, //!< Warning message
  FATAL    //!< Fatal error condition message
};  

typedef enum IMPv2_MsgType MsgType;

//----------------------------------------------------------------
//
// isisclient Function Prototypes 
//

// isismessage function prototypes

char *ISISMessage(char *fromID, char *destID, MsgType msgtype, char *msgbody);
int SplitMessage(char *msgstr, char *fromID, char *destID, 
                 MsgType *msgtype, char *msgbody);

// isissocket function prototypes

int  InitISISServer(isisclient_t *);
int  OpenClientSocket(isisclient_t *);
int  ReadClientSocket(isisclient_t *, char *);
int  SendToISISServer(isisclient_t *, char *);
int  ReplyToRemHost(isisclient_t *, char *);
void CloseClientSocket(isisclient_t *); 

// isisserial function prototypes

int  OpenSerialPort(char *);
void CloseSerialPort(int);
int  SetSerialPort(int, int, int, int, int);
int  WriteSerialPort(int, char *);
int  ReadSerialPort(int, char *);
void FlushSerialPort(int, int);

// isisutils function prototypes

void LeftStr(char *substr, char *str, int nchar);
void RightStr(char *substr, char *str, int nchar);
void MidStr(char *substr, char *str, int start, int nchar);
void SubStr(char *substr, char *str, int first, int last);
void UpperCase(char *str);
void GetArg(char *cmdstr, int argnum, char *argstr);
char *UTCDate(void);
char *UTCTime(void);
char *ISODate(void);
char *UTCDateTag(void);
char *GetFineTime(void);
double SysTimestamp(void);
int MilliSleep(long);

#endif  // ISISCLIENT_H


