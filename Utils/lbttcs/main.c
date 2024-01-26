/*!
  \mainpage lbttcs - Interactive LBT TCS Interface Client

  \author R. Pogge, OSU Astronomy Dept. (pogge@astronomy.ohio-state.edu)
  \date 2009 July 13

  \section Usage

  Usage: lbttcs [rcfile]

  Where: \c rcfile is an optional runtime config file to load.  

  By default, lbttcs uses the runtime config file defined by
  #DEFAULT_RCFILE in the client.h header.

  \section Introduction

  ...

  \section Commands

  These are the interactive commands for lbttcs:
  <pre>
  info           - report client information
  version        - report lbttcs version & compile info
  reset          - reset runtime & controller parameters
  verbose        - toggle verbose output mode
  debug          - toggle debugging output
  quit           - quit lbttcs
  history        - show command history
  !!             - repeat last command
  !cmd           - repeat last command matching 'cmd'
  help or ?      - view this list
  </pre>
  Note that all commands are <em>case-insensitive</em>.

  \section Config Runtime Configuration File

  This is a typical runtime config file for the lbttcs agent:
  \verbinclude lbttcs.ini

  Note that all parameter names are \e case-insensitive.

  \section Notes

  This application uses the ISISclient library (link).

  \section Mods Modification History

<pre>
2009 July 13 - based on mdmtcs [rwp/osu]
2010 March - first (remote) on-site tests with live LBT, lost of bugs squashed [rwp/osu]
2010 June - first on-site test (mountain lab) of MODS1+LBT both live [rwp/osu]
2010 Sept - some changes for new TCS builds and DD entry issues [rwp/osu]
2010 Nov - new header entries from the DD, numerous bug fixes [rwp/osu]
2010 Dec - overhauled offsets, added useCLI to enable background operation [rwp/osu]
</pre>

*/

/*!
  \file main.c
  \brief lbttcs main program and I/O event handler.
*/

#include "client.h"      // Custom client application header

// The client cli uses the GNU readline and history utilities

#include <readline/readline.h>
#include <readline/history.h>

// Global data structures 

isisclient_t client; // Client ISIS common data structure
lbtinfo_t lbt;       // LBT information data structure
utcinfo_t utc;       // UTC date/time data structure
lbtData_t lbtTCS;    // LBT TCS data structure for ICE calls
modsData_t mods;     // MODS status information data structure

// ICE/IIF globals

#include "modsIIFGlobals.h"

// Other globals (see client.h)

int useCLI = 1;

//----------------------------------------------------------------
//
// The main event...

int
main(int argc, char *argv[]) 
{
  int n;
  int i=0;
  int nread;
  int readout = 0;
  int countdown = 0;
  double dt;
  int itick=0;

  char buf[ISIS_MSGSIZE]; // command/message buffer
  char reply[2048];       // generic reply string

  int lastchar;
  
  // readline & history handling stuff
  
  char cliPrompt[ISIS_NODESIZE+2]; // the console prompt is our ISIS node name
  
  // select() event handler parameters
  
  fd_set read_fd;
  int kbdFD;
  int numReady;
  struct timeval timeout;
  static int sel_wid;

  // Parameters for the display in main

  // Basic initializations
  
  sel_wid = getdtablesize();
  kbdFD = fileno(stdin);  // file descriptor of stdin, safe definition
  
  // Command line arguments are needed by the ICE/IIF stuff...

  if (argc>2) {
    printf("usage: %s [rcfile]\n", argv[0]);
    printf("where: rcfile = optional runtime config file (default %s)\n",
	   DEFAULT_RCFILE);
    exit(10);
  }

  // Load and parse the runtime configuration file

  if (argc == 2) {
    n = loadConfig(argv[1]);
    argc--;
  }
  else
    n = loadConfig(DEFAULT_RCFILE);
    
  if (n!=0) {
    printf("\nUnable to load config file %s - lbttcs aborting n=%d\n\n",
	   client.rcFile,n);
    exit(11);
  }

  // So far so good, give the welcome information

  if (useCLI) {
    printf("\n");
    printf("  -------------------------------------------------\n");
    printf("                     lbttcs\n");
    printf("  Interactive LBT ICE/IIF-based TCS Interface Agent\n\n");
    
    printf("  Version: %s (%s %s)\n",APP_VERSION,APP_COMPDATE,APP_COMPTIME);
    printf("  -------------------------------------------------\n");
    printf("\n");
  }

  // Now we do various initializations

  // If required, initialize the socket connection to the ISIS server
  // We can disable ISIS interaction by specifying "ServerID None" in
  // the runtime config file

  if (client.useISIS) {
    if (InitISISServer(&client)<0) {
      printf("\nISIS server connection initialization failed - lbttcs aborting\n\n");
      exit(20);
    }
  }

  // Open the client network socket port for interprocess communications
  
  if (OpenClientSocket(&client)<0) {
    printf("\nClient socket initialization failed - lbttcs aborting\n\n");
    exit(21);
  }

  if (client.useISIS)
    printf("Started lbttcs as ISIS client node %s on %s port %d\n",
	   client.ID, client.Host, client.Port);
  else
    printf("Started lbttcs as standalone agent %s on %s port %d\n",
	   client.ID, client.Host, client.Port);

  // Try to open ICE communications if we are not in simulation mode

  if (lbt.opMode == LIVE) {
    if (mods_OpenIceComm(lbt.iceProps,argc,argv)!=0) {
      printf("*** ABORT: ICE Communication Initialization Failed\n");
      printf("*** Reason: %s\n",mods_error());
      printf("*** lbttcs aborting with errors ***\n");
      exit(30);
    }
    printf("ICE/IIF communications opened successfully.\n");
    printf("Client Proxy %s created for %s at %s\n",
	   clientProxy_Name,instrument_Name,focalStation_Name);
  }

  // Try to initialize the TCS link, we can do this later if it fails

  if (initLBTTCS(reply)<0) {
    printf("*** WARNING: LBT TCS link not enabled at startup\n");
    printf("*** Reason: %s\n",reply);
  }
  else
    if (useCLI) printf("%s\n",reply);
      
  // All set to rock-n-roll...

  if (useCLI) {
    printf("\n----------------------------------------------\n");
    printf("Type 'quit' to terminate the interactive session\n");
    printf("Type 'help' to see a list of commands\n");
    printf("Type 'reset' to reset the session\n");
    printf("----------------------------------------------\n");
  }

  // Startup the command-line history mechanism

  if (useCLI) using_history();

  // Setup the command prompt and install the readline() callback
  // handler for this application (KeyboardCommand() in commands.c)

  sprintf(cliPrompt,"%s%% ",client.ID);
  if (useCLI) rl_callback_handler_install(cliPrompt,KeyboardCommand);
  
  // Broadcast a PING to the ISIS server, if enabled.  If it fails,
  // we'll have to do the ping by hand after the comm loop starts.

  if (client.useISIS) {
    memset(buf,0,ISIS_MSGSIZE);
    sprintf(buf,"%s>AL ping\r",client.ID);
    if (SendToISISServer(&client,buf)<0) {
      printf("Failed to PING the ISIS server...\n",strerror(errno));
      printf("  Probably bad - maybe quit now, check server status, and retry\n");
    }
    if (client.isVerbose && useCLI)
      printf("OUT: %s\n",buf);
  }

  // Set the SIGINT signal trap 
  
  signal(SIGINT,HandleInt);   // Ctrl+C sends a move abort to controller
  signal(SIGKILL,HandleKill); // Trap kill, make sure ICE proxy is closed
  signal(SIGPIPE,SIG_IGN);    // Ignore broken pipes

  //----------------------------------------------------------------------
  //
  // Start the I/O event handling loop.
  //

  client.KeepGoing = 1;

  while (client.KeepGoing) {
    
    FD_ZERO(&read_fd); // clear the table of active file descriptors
    
    // If using the command-line interface, listen for console keyboard input
    
    if (useCLI) FD_SET(kbdFD, &read_fd);
    
    // Listen to this application's UDP socket
    
    if (client.FD > 0) FD_SET(client.FD, &read_fd);
    
    // Do the select() call and wait for activity on any of our comm
    // ports or the console keyboard
     
    numReady = 0;

    // Setup for 300 second timeout, do housekeeping as required.

    timeout.tv_sec = 300;
    timeout.tv_usec = 0;
    numReady = select(sel_wid, &read_fd, NULL, NULL, &timeout);
      
    //----------------------------------------------------------------
    //
    // select() done, take action depending on the value of numReady
    // returned
    //

    // select() timed out, do various housekeeping activities as required

    if (numReady == 0) {

      // We haven't done anything for 300sec, query the TCS if active
      // Otherwise, attemp to connect if the link was down the last time

      switch(lbt.opMode) {
      case LIVE:
	if (lbt.Link)
	  queryLBTTCS();
        else {
	  if (initLBTTCS(reply)<0) { // link down, attempt to reconnect
            if (useCLI) printf("*** WARNING: LBT TCS not responding - %s - is it active?\n",reply);
          }
          else {
            if (queryLBTTCS()>=0) {
              if (useCLI) printf("*** %s ***\n",reply);
            }
          }
        }
	break;
      case LABSIM:
	getUTCTime(&utc);
	if (itick)
	  printf("  %s                          \r",utc.ISO);
	else {
	  printf("\n  %s                        \r",utc.ISO);
	  itick = 1;
	}
	fflush(stdout);
	// if (client.isVerbose) printf("...tick...\n");
	break;
      }
    }
    
    // select() returned an error, handle it
    
    else if (numReady < 0) {
      itick = 0;
      if (errno == EINTR) { // caught Ctrl+C, hopefully sigint handler caught it
	if (client.Debug && useCLI)
	  printf("select() interrupted by Ctrl+C...continuing\n");
      }
      else { // something else bad happened, let us know
	if (useCLI) printf("Warning: select() failed - %s - pressing on anyway...\n",strerror(errno));
      }
      if (useCLI) rl_refresh_line(0,0);
      continue;
      
    }
    
    // select() has input to process on one of the input channels
    
    else {
      
      itick = 0;

      // Console keyboard input
      
      if (useCLI && FD_ISSET(kbdFD, &read_fd)) {
	rl_callback_read_char();  // readline() command handler
	signal(SIGINT,HandleInt);  // reset the SIGINT handler
      }
      
      // Client socket input
      
      if (client.FD > 0 && FD_ISSET(client.FD, &read_fd)) {
	memset(buf,0,ISIS_MSGSIZE);
	if (ReadClientSocket(&client,buf)>0) {
	  if (client.isVerbose && useCLI) printf("IN: %s\n",buf);
	  SocketCommand(buf);
	  if (useCLI) rl_refresh_line(0,0);
	}
      }
      
      // add any new FD handlers here...
      
    } // end of select() I/O handling checking
    
  } // bottom of the while(client.KeepGoing) loop
  
  //----------------------------------------------------------------
  //
  // If we got here, the client was instructed to shut down
  //

  printf("\nMODS LBT TCS Interface Client Shutting Down...\n");

  // Tear down the application's client socket

  CloseClientSocket(&client);

  // Shutdown the IIF proxy

  mods_IIFClose();

  // Remove the readline() callback handler

  if (useCLI) rl_callback_handler_remove();

  exit(0);

}

//---------------------------------------------------------------------------

/*!
  \brief Service Ctrl+C Interrupts (SIGINT signals)

  SIGINT signal trap for trapping Ctrl+C interrupts.  Calls
  abortall() to immediately abort all moves in progress.

  \sa abortall()
*/

void
HandleInt(int signalValue)
{
  char reply[256];
  if (client.Debug && useCLI)
    printf("Caught Ctrl+C Abort...\n");
  
  if (useCLI) printf("Ctrl+C Abort requested - whatever\n");

}

//---------------------------------------------------------------------------
//
// HandleKill() - SIGKILL signal handler to make sure we don't leave
//                delinquent ICE proxies behind.
//

void
HandleKill(int signalValue)
{
  if (client.Debug && useCLI)
    printf("Caught KILL...\n");

  mods_IIFClose();
}

