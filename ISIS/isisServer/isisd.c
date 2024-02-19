//
// isisd - Integrated Science Instrument Server Daemon
//
// 

/*!
  \mainpage ISIS - Integrated Science Instrument Server

  \author Richard Pogge, OSU Astronomy Department (pogge\@astronomy.ohio-state.edu)
  \date 2010 April 14

  \section Usage

  Usage: isisd

  \section Introduction

  ISIS is a fully interactive message-passing server that uses
  connectionless Unix Datagram Protocol (UDP) network sockets and RS232
  serial ports for interprocess communications.  It features a fully
  interactive command-line interface and runtime logging capability with
  precision timetagging.

  The message passed by ISIS follow the ICIMACS Messaging Protocol
  Version 2 (IMPv2) used by OSU-built instruments.  The details of this
  protocol are described in the <a
  href="/LBT/MODS/Software/IMPv2.pdf">IMPv2 Specification</a> (300k
  PDF).

  ISIS is as a replacement for the older ariel/WC system we used in the
  past.  ISIS assumes the basic functions of the ariel daemon and most
  of the functions of the old WC ("Workstation Computer") MS-DOS PC
  system.

  \section Commands ISIS Commands

  The following are the ISIS interactive commands:
  <pre>
  Executive Commands (keyboard-only):
     QUIT    Terminate the server session and exit</dd>
     HOSTS   Print a table of all known client hosts</dd>
     INFO    Display detailed server information on the screen</dd>
     HELP    List of server commands</dd>
     VERBOSE Echo all server communications to the screen</dd>
     CONCISE Suppress echoing of communications (opposite of VERBOSE)</dd>
     HISTORY List recently typed commands (like Unix history)</dd>

  Communications Control:
     PING Communications initialization request, responds with a PONG </dd>
     PONG Communications initialization acknowledge, no response required</dd>
     RESTART Warm restart the server, clear runtime tables, PING fixed ports</dd>
     PORTS lists the active tty ports</dd>
     FLUSH \c n   Flush tty port \c n of junk</dd>
     REMOVE \c xx Remove host \c xx from the client host table</dd>

  Utilities:
     VERSION   Return program version info</dd>
     HOST \c xx   Return info on a client host \c xx (or \c all reports all hosts)</dd>
     STATUS    Return a server status message</dd>
     CONFIG    Displays current configuration with instrument info</dd>
     TIME      Report the UTC time and date as FITS-like keywords</dd>
     -LOG      Disable the server's runtime log temporarily</dd>
     +LOG      (Re)Enable the server's runtime log after -LOG</dd>
  </pre>
  Note: while the command names are given in all caps to distinguish
  them, all ISIS commands are case-insensitive):

  \section Config Runtime Configuration File

  The runtime behavior of ISIS is setup via a runtime config file.
  Here is an example:

  \verbinclude isis.ini

  The # character denotes comments, and appears in the first character
  of the line.  Inline comments are not supported by this simple parser.
  Blank lines are ignored by the parser, and may be included in any
  number desired to improve the human readability of a config file.

  Runtime parameters are designated by keyword/value pairs, for example:
  \verbatim ServerID   IS\endverbatim
  We adopt the convention that keywords and values are case insensitive,
  to remove any ambiguity.

  If an unknown keyword is given in a config file, a warning message
  is printed but the rest of the config file is read. 

  In operational settings, a default runtime config file is provided,
  defined at compile time by the #ISIS_CONFIG parameter in the
  isisserver.h header file.  A user can override this default by
  giving the name of the desired config file on the command line.

  Many runtime parameters can be changed during a session with
  interactive commands, but these changes will not be preserved for
  future sessions.  The provides a means for safeguarding a public
  server configuration against change by careless users (or forgetful
  support staff who ignore the protocol that after doing diagnostics
  it is required that all public instrument systems be cold restarted 
  to ensure that the default startup configurations are recovered).

  \section Notes

  This program started from the examples given in Gray J.S.,
  Interprocess Communications in Unix, 1997 (Prentice Hall),
  specifically example programs 10.10 and 10.13, and then evolved from
  there.  The more advanced parts of the network socket interface also
  made heavy use of W. Richard Stevens' excellent "Unix Network
  Programming, Volume 1, Network APIs: Sockets and XTI" (Prentice Hall,
  2nd edition).

  Uses select() for I/O multiplexing.  No multithreading at the present
  time, but probably should if we get into services that take a long
  time to execute.

  Uses the GNU readline and history utilties for the CLI.  

  \section Mods Modification History
  <pre>
  2003 Jan 4 - initial testbed version [rwp/osu]
  2003 Jan 15 - first production version, deployed at CTIO [rwp/osu]
  2003 Feb 15 - post-run updates from CTIO field testing [rwp/osu]
  2003 Sept 14 - Modified extensively to generalize code and
                 introduce better timing practices [rwp/osu]
  2004 May 11 - overhauled package organization, now compiles under g++
                (not pretty at first), also introduced Doxygen markup
                for all comment blocks to bring up to our current
		documentation standard [rwp/osu]
  2009 Mar 18 - modifications for version 2.x, including daemon mode
                and cleanup of old junk we never use anymore [rwp/osu]
  2010 Apr 14 - further modifications for operation as a daemon [rwp/osu]
  </pre>
*/

/*!
  \file isisd.c
  \brief ISIS server daemon main program and I/O event handler.
*/

#include "isisserver.h"

// The ISIS cli uses the GNU readline and history utilities 

#include <readline/readline.h>
#include <readline/history.h>

// Global data structures used by the ISIS server.  These are declared
// extern in the isisserver.h header

struct server isis;                   //!< Server runtime parameter table
struct clients clientTab[MAXCLIENTS]; //!< Server client table
struct serial ttyTab[MAXSERIAL];      //!< Server serial port table
struct udpPreset udpTab[MAXPRESET];   //!< Server preset UDP port table

// Function prototypes only used in main()

// The main event... 

/*!
  \brief ISIS server initializer and I/O event handler
*/

int
main(int argc, char *argv[]) 
{
  int i;
  int n;
  int numReady;
  int c;
  char logStr[256];

  // stuff for select() 

  fd_set fdList;
  static int selWidth;

  // timeval structs for timeouts 

  struct timeval tv;

  // Our process ID and Session ID

  pid_t pid, sid;

  // Initialize and read our runtime config file.  If this fails, the rest is moot

  strcpy(isis.exeFile,argv[0]);       // how this executable was invoked 
  strcpy(isis.userID,getenv("USER")); // and by whom                     
  strcpy(isis.iniFile,ISIS_DCONFIG);  // default daemon-mode runtime init file
  if (loadConfig(isis.iniFile) < 0) {
    printf("ERROR parsing the isisd ini file %s - aborting\n",
	   isis.iniFile);
    exit(EXIT_FAILURE);
  }
  isis.useCLI = isis_FALSE;  // we never use the command-line in daemon mode
  isis.isVerbose = isis_FALSE; // we never use verbose output in daemon mode

  // Before we do anything else, since this is a daemon fork off the
  // parent process

  pid = fork();
  if (pid < 0) {
    printf("ERROR: cannot fork() off isisd - %s - aborting\n",
	   strerror(errno));
    exit(EXIT_FAILURE);
  }

  // If we got a good PID, we can now exit the parent

  if (pid > 0) 
    exit(EXIT_SUCCESS);

  // Change the file mode mask

  umask(0);

  // start logging

  initLog();

  // Create a new SID for the child process

  sid = setsid();
  if (sid < 0) {
    sprintf(logStr,"Error getting SID for child process - %s",strerror(errno));
    logMessage(logStr);
    exit(EXIT_FAILURE);
  }

  // Change the working directory

  if (chdir("/") < 0) {
    sprintf(logStr,"Error could not cd to root - %s",strerror(errno));
    logMessage(logStr);
    exit(EXIT_FAILURE);
  }

  // We're ready to go, close out the standard file descriptors to
  // block the obvious security holes

  close(STDIN_FILENO);
  close(STDOUT_FILENO);
  close(STDERR_FILENO);

  // Do it

  logMessage((char *)"isisd daemon session start");

  // Initialize the selection width and keyboard file descriptor

  selWidth = getdtablesize();

  // Initialize the serial ports requested in the ini file (if any) 

  if (isis.numSerial > 0) {
    if (initSerialPorts() < isis.numSerial) {
      logMessage((char *)"Error initializing serial port - aborting");
      exit(EXIT_FAILURE);
    }
  }

  // Initialize the client host table 

  initHostTable();

  // Open the server network socket 

  if (openSocket(isis.sockPort) < 0) {
    logMessage((char *)"Error: server socket startup failure - aborting");
    exit(EXIT_FAILURE);
  }

  sprintf(logStr,"Server started as host %s on port %d",
	  isis.serverID,isis.sockPort);
  logMessage(logStr);

  // Set the server process flags 

  isis.keepGoing = isis_TRUE;   // server runs while KeepGoing is TRUE   
  isis.doStartup = COLD_START;  // cold start server communications      

  //----------------------------------------------------------------
  // 
  // Main Server Loop 
  //
  // Two start/restart modes:
  //
  //   COLD START: no known clients, so do fixed-port handshaking
  //               if enabled (isis.doHandShaking set).
  //   WARM START: re-read/parse the ini file, and since we have
  //               known clients, ping them all.
  //

  while (isis.keepGoing) {

    // Do communications startup before entering the select() loop again? 

    if (isis.doStartup != NO_STARTUP) {
      switch (isis.doStartup) {

      case COLD_START:
	logMessage((char *)"Server cold start initiated");
	if (isis.doHandShaking) handShake();
	logMessage((char *)"Server cold start done");
	break;

      case WARM_START:
	logMessage((char *)"Server warm start initiated");
	sprintf(logStr,"Reloading initialization file %s",isis.iniFile);
 	logMessage(logStr);
	loadConfig(isis.iniFile);
	logMessage((char *)"Pinging clients");
	broadcastMessage(ISIS_SERVER,(char *)"PING");
	logMessage((char *)"Warm restart completed");
	break;

      default:
	break;

      }
      isis.doStartup = NO_STARTUP;
    }

    // Set up the file descriptor table for select() 

    FD_ZERO(&fdList);
    FD_SET(isis.sockFD, &fdList) ;
    if (isis.numSerial > 0) {
      for (i=0; i<isis.numSerial; i++) {
	FD_SET(ttyTab[i].fd, &fdList) ;
      }
    }

    // See if anyone has anything to say to us 

    numReady = select(selWidth, &fdList, NULL, NULL, NULL) ;

    // select() Error Handling:
    //
    //   If an error other than a timeout occurs, deset the
    //   isis.keepGoing flag, which will exit the server loop and
    //   teardown the socket, aborting the program.  Right now we just
    //   tell stdout we're going down, later we'll broadcast this to all
    //   known clients.
    //
    //   If a server timeout occurred, just echo it and keep
    //   rolling. During testing we don't enable timeouts anyway.
    //   Eventually timeouts will be used to create a crude heartbeat,
    //   but for now it just notes timeouts if set for testing.
     

    if (numReady < 0) { 
      sprintf(logStr,"WARNING: Server select() Error - %s",strerror(errno));
      logMessage(logStr);
      continue;
    } 
    else if (numReady == 0) {
      logMessage((char *)"WARNING: server select() timeout");
      continue;
    }
    else {

      // we got something, deal with it 

      if (FD_ISSET(isis.sockFD,&fdList)) {  // pending socket input 
	socketHandler(isis.sockFD);
      }
      else {
	if (isis.numSerial > 0) {
	  for (i=0; i<isis.numSerial; i++) {
	    if (FD_ISSET(ttyTab[i].fd, &fdList)) { // pending serial input 
	      serialHandler(ttyTab[i].fd);
	    }
	  }
	} // end of serial port checking 
      } // ed of FD_ISSET() checking 
    } // end of select() input handling 
    
  }
  // 
  // Bottom of the Server Loop 
  //
  //----------------------------------------------------------------

  // Tear down the server socket

  closeSocket(isis.sockFD);

  // all done! 

  logMessage((char *)"Server shutdown normally");

  exit(EXIT_SUCCESS);

}

