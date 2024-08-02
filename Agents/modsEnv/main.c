/*!
  \mainpage modsenv - Interactive MODS Enviromental Monitor Agent

  \author R. Pogge, OSU Astronomy Dept. (pogge@astronomy.ohio-state.edu)
  \date 2010 June 21

  \section Usage

  Usage: modsenv [rcfile]

  Where: \c rcfile is an optional runtime config file to load.  

  By default, modsenv uses the runtime config file defined by
  #DEFAULT_RCFILE in the client.h header.

  \section Introduction

  modsenv is an IMPv2 client that directly connects to the WAGO
  units inside the MODS instrument electronics boxes and reads
  the temperature and pressure sensors, producing a log of readings.

  At present modsenv only reads the sensors operated by the WAGO
  field-bus controllers as the CCD environmental data are only
  accessible via the IC interface, which will require a more
  sophisticated version in the works.

  \section Commands

  These are the interactive commands for modsenv:
  <pre>
  quit     - Terminate the client session
  verbose	 - Toggle verbose output mode on/off
  debug    - Toggle super-verbose debugging output mode on/off
  history	 - Show the command history (console only)
  info     - Report client session runtime info
  status	 - Report the client runtime status (alias for info)
  version	 - Report the client version and compilation time
  estatus	 - Report the current MODS environmental sensor status
  pstatus  - Report the current MODS AC power control system sensor status
  cadence	 - Set/Query the monitoring cadence in seconds
  pause    - Pause monitoring (see RESUME)
  resume	 - Resume monitoring after a PAUSE
  log <enable|disable> - Enable/Disable enviromental sensor data logging
  logging <enable|disable> - Enable/Disable enviromental sensor data logging
  hdflog <enable|disable> - Enable/Disable hdf formatted telemetry logging
  hdflogging <enable|disable> - Enable/Disable hdf formatted telemetry logging
  comment \<message\> - Append a comment to the log
  config	 - Report agent configuration [engineering]
  help \<cmd\>	 - Help command (alias: ? \<cmd\>)

  Command History (console only):
  !!  		 - repeat last command
  !cmd		 - repeat last command matching 'cmd'
  arrow keys for command-line & history editing
  </pre>
  Note that all commands are <em>case-insensitive</em>.

  \section Config Runtime Configuration File

  This is a typical runtime config file for the modsenv agent:
  \verbinclude modsenv.ini

  Note that all parameter names are \e case-insensitive.

  modsenv is normally run as a background task with the command-line
  interface disabled (<kbd>UseTTY N</kbd>), but in engineering
  situations the interactive command utility is enabled by setting
  <kbd>UseTTY Y</kbd>.  

  A sampling cadence of 60 seconds is typical.  The default cadence
  is set in the modsenv.ini file, but can be changed during a session
  with the <kbd>cadence</kbd> command.  Note that changes to these
  parameters made during a session are not saved for future sessions, and
  restarting modsenv would restore the defaults in the modsenv.ini file.

  \section Notes

  This application uses the ISISclient library for UDP socket
  communications and IMPv2 message processing and creation.

  Release notes are tracked in the [releases.md](releases.md) file.

*/

/*!
  \file main.c
  \brief modsenv main program and I/O event handler.
*/

#include "client.h"      // Custom client application header

// The client cli uses the GNU readline and history utilities

#include <readline/readline.h>
#include <readline/history.h>

// Global data structures 

isisclient_t client; // Client ISIS common data structure
envdata_t env;       // MODS enviromental sensor data structure

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
  int ierr;

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

  //HDF5 variables

  int useHdf5 = 0;

  // Basic initializations
  
  // setup_ids();  // setup access to the MODS SHRMEM sector

  sel_wid = getdtablesize();
  kbdFD = fileno(stdin);  // file descriptor of stdin, safe definition

  // Parse command-line arguments

  if (argc>2) {
    printf("usage: %s [rcfile]\n", argv[0]);
    printf("where: rcfile = optional runtime config file (default %s)\n",
	   DEFAULT_RCFILE);
    printf("modsenv aborted with errors on startup.\n");
    exit(1);
  }

  // Before we do anything, load the runtime configuration file

  if (argc == 2)
    n = loadConfig(argv[1]);
  else
    n = loadConfig(DEFAULT_RCFILE);
    
  if (n!=0) {
    printf("\nUnable to load config file %s - modsenv aborting\n",client.rcFile);
    exit(1);
  }

  // So far so good, give the welcome information
  
  if (useCLI) {
    printf("\n");
    printf("  ---------------------------------------------------\n");
    printf("                       modsenv\n");
    printf("  Interactive MODS Enviromental Sensor Monitor/Logger\n");
    printf("  Version: %s (%s %s)\n",APP_VERSION,APP_COMPDATE,APP_COMPTIME);
    printf("  ---------------------------------------------------\n");
    printf("\n");
  }

  // Now we do various initializations

  // If required, initialize the socket connection to the ISIS server
  // We can disable ISIS interaction by specifying "ServerID None" in
  // the runtime config file

  if (client.useISIS) {
    if (InitISISServer(&client)<0) {
      printf("\nISIS server connection initialization failed - modsenv aborting\n\n");
      exit(2);
    }
  }

  // Open the client network socket port for interprocess communications
  
  if (OpenClientSocket(&client)<0) {
    printf("\nClient socket initialization failed - modsenv aborting\n\n");
    exit(3);
  }

  if (client.useISIS)
    printf("Started modsenv as ISIS client node %s on %s port %d\n", client.ID, client.Host, client.Port);
  else
    printf("Started modsenv as standalone agent %s on %s port %d\n", client.ID, client.Host, client.Port);

  // Check if the leapseconds file (needed for hdf5 export) exists. If it doesn't use the fallback one.
  
  if (!fileExists(env.leapSecondsFile)){
    printf("The leap-seconds.list file could not be loaded from [%s]. The fallback file will be used instead.", env.leapSecondsFile);
    strcpy(env.leapSecondsFile, FALLBACK_LEAP_SECONDS_FILE);
  }

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
  
  signal(SIGINT ,HandleInt);  // Trap Ctrl+C 
  signal(SIGKILL,HandleKill); // Trap kill and killall
  signal(SIGPIPE,SIG_IGN);    // Ignore broken pipes

  // One last thing, make an initial sensor query

  ierr = getEnvData(&env);
  if(ierr == 0){
    if (env.doLogging) logEnvData(&env);
    if (env.useHdf5) logTelemetryData(&env);
  }

  //----------------------------------------------------------------------
  //
  // Start the I/O event handling loop.
  //

  client.KeepGoing = 1;

  while (client.KeepGoing) {
    
    FD_ZERO(&read_fd); // clear the table of active file descriptors
    
    // I using the interactive CLI mode, listen for console keyboard input
    
    if (useCLI) FD_SET(kbdFD, &read_fd);
    
    // Listen to this application's UDP socket
    
    if (client.FD > 0) FD_SET(client.FD, &read_fd);
    
    // Do the select() call and wait for activity on any of our comm
    // ports or the console keyboard
     
    numReady = 0;

    // The select timeout is the sensor sampling cadence, either the
    // client default or the value set in the runtime initialization
    // file.  It can also be changed on-the-fly with the CADENCE
    // command.

    timeout.tv_sec = env.cadence;
    timeout.tv_usec = 0;
    numReady = select(sel_wid, &read_fd, NULL, NULL, &timeout);
      
    //----------------------------------------------------------------
    //
    // select() done, take action depending on the value of numReady
    // returned
    //

    // select() timed out on the monitor cadence, do something...

    if (numReady == 0) {

      // We haven't done anything for timeout, so query the instrument
      // enviromental sensors unless we are in a monitor pause

      if (!env.pause) {
	      ierr = getEnvData(&env);

        if(ierr == 0){
          if (env.doLogging) logEnvData(&env); 
          if (env.useHdf5) logTelemetryData(&env); 
        }
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
	if (useCLI) printf("Warning: select() failed - %s - pressing on anyway...\n", strerror(errno));
      }
      if (useCLI) rl_refresh_line(0,0);
      continue;
      
    }
    
    // select() has input to process on one of the input channels
    
    else {
      
      itick = 0;
      
      // Console keyboard input
      
      if (FD_ISSET(kbdFD, &read_fd)) {
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
  
  printf("\nMODS Enviromental Monitor Agent Shutting Down...\n");
  
  // Tear down the application's client socket
  
  CloseClientSocket(&client);

  // Close any open data logs

  logMessage(&env,(char *)"modsEnv agent shutting down");

  if (env.logFD > 0) close(env.logFD);          //Closes the ascii log.
  closeTelemetryData(&env);                     //Closes the hdf5 log.

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
  if (client.Debug && useCLI) printf("Caught Ctrl+C Abort...\n");
}

//---------------------------------------------------------------------------
//
// HandleKill() - SIGKILL signal handler
//

void
HandleKill(int signalValue)
{
  if (client.Debug && useCLI) printf("Caught KILL...\n");
  if (env.logFD > 0) close(env.logFD);
}

