/*!
  \mainpage modsheb - Interactive MODS Head Electronics Box Monitor and Control Agent

  \author X. Carroll
  \author R. Pogge, OSU Astronomy Dept. (pogge@astronomy.ohio-state.edu)
  \date 2024 August 14

  \section Notes

  This application uses the ISISclient library for UDP socket
  communications and IMPv2 message processing and creation.

  Release notes are tracked in the [releases.md](releases.md) file.

*/

/*!
  \file main.c
  \brief modsheb main program and I/O event handler.
*/

// Custom client application header
#include "client.h"

// The client cli uses the GNU readline and history utilities
#include <readline/readline.h>
#include <readline/history.h>

// Global data structures 
isisclient_t client;  // Client ISIS common data structure
envdata_t env;        // MODS enviromental sensor data structure

// Other globals (see client.h)
int useCLI = 1;

//---------------------------------------------------------------------------

int main(int argc, char *argv[]){
  int configStatus;
  double dt;
  int ierr;

  char buf[ISIS_MSGSIZE]; // command/message buffer
  char reply[2048];       // generic reply string
  
  // Readline & history handling
  char cliPrompt[ISIS_NODESIZE+2]; // the console prompt is our ISIS node name
  
  // select() event handler parameters
  fd_set read_fd;
  int kbdFD;
  int numReady;
  struct timeval timeout;
  static int sel_wid;

  // HDF5 variables
  int useHdf5 = 0;

  // Basic initializations
  timeout.tv_usec = 0;        // The microseconds field of timeout won't be used.

  sel_wid = getdtablesize();  // The maximum number of files that the process can have open.
  kbdFD = fileno(stdin);      // File descriptor of stdin, safe definition.

  // Parse command-line arguments
  if (argc>2) {
    printf("usage: %s [rcfile]\n", argv[0]);
    printf("  where: rcfile = optional runtime config file (default %s)\n",DEFAULT_RCFILE);
    printf("modsheb aborted with errors on startup.\n");
    return 1;
  }

  // Before we do anything, load the runtime configuration file
  configStatus = loadConfig((argc == 2) ? argv[1] : DEFAULT_RCFILE);
  if (configStatus!=0) {
    printf("\nUnable to load config file %s - modsheb aborting\n",client.rcFile);
    freeEnvData(&env);
    return 1;
  }

  // So far so good, give the welcome information
  if (useCLI) {
    printf("\n");
    printf("  ---------------------------------------------------\n");
    printf("                       modsheb\n");
    printf("  Interactive MODS Head Electronics Box Monitor/Logger\n");
    printf("  Version: %s (%s %s)\n",APP_VERSION,APP_COMPDATE,APP_COMPTIME);
    printf("  ---------------------------------------------------\n");
    printf("\n");
  }

  // Now we do various initializations

  // If required, initialize the socket connection to the ISIS server.
  // We can disable ISIS interaction by specifying "ServerID None" in
  // the runtime config file.
  if (client.useISIS) {
    if (InitISISServer(&client)<0) {
      printf("\nISIS server connection initialization failed - modsheb aborting\n\n");
      freeEnvData(&env);
      return 2;
    }
  }

  // Open the client network socket port for interprocess communications
  if (OpenClientSocket(&client)<0) {
    printf("\nClient socket initialization failed - modsheb aborting\n\n");
    freeEnvData(&env);
    return 3;
  }

  if (client.useISIS)
    printf("Started modsheb as ISIS client node %s on %s port %d\n", client.ID, client.Host, client.Port);
  else
    printf("Started modsheb as standalone agent %s on %s port %d\n", client.ID, client.Host, client.Port);

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
  ierr = getDeviceData(&env);
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
    
    // Clear the table of active file descriptors
    FD_ZERO(&read_fd);
    
    // If using the interactive CLI mode, listen for console keyboard input
    if (useCLI) FD_SET(kbdFD, &read_fd);
    
    // Listen to this application's UDP socket
    if (client.FD > 0) FD_SET(client.FD, &read_fd);
    
    // Do the select() call and wait for activity on any of our comm ports or the console keyboard
    numReady = 0;

    // The select timeout is the sensor sampling cadence, either the client default or the value set 
    // in the runtime initialization file. It can also be changed on-the-fly with the CADENCE command.
    timeout.tv_sec = env.cadence;
    numReady = select(sel_wid, &read_fd, NULL, NULL, &timeout);
      
    //----------------------------------------------------------------
    //
    // select() done, take action depending on the value of numReady returned
    //

    // select() timed out on the monitor cadence, do something...
    if (numReady == 0) {

      // We timed-out without doing anything, so query the instrument enviromental sensors unless we are in a monitor pause.
      if (!env.pause) {
	      ierr = getDeviceData(&env);

        if(ierr == 0){
          if (env.doLogging) logEnvData(&env); 
          if (env.useHdf5) logTelemetryData(&env); 
        }
      }
    
    }
    
    // select() returned an error, handle it
    else if (numReady < 0) {
      if (errno == EINTR){
        // Caught Ctrl+C, hopefully sigint handler caught it
	      if (client.Debug && useCLI) printf("select() interrupted by Ctrl+C...continuing\n");
      }else{ 
        // Something else bad happened, let us know
	      if (useCLI) printf("Warning: select() failed - %s - pressing on anyway...\n", strerror(errno));
      }
      if (useCLI) rl_refresh_line(0,0);
      continue;
    }
    
    // select() has input to process on one of the input channels
    else {      
      // Console keyboard input
      if (FD_ISSET(kbdFD, &read_fd)) {
	      rl_callback_read_char();    // readline() command handler
	      signal(SIGINT,HandleInt);   // reset the SIGINT handler
      }
      
      // Client socket input
      if (client.FD > 0 && FD_ISSET(client.FD, &read_fd)){
	      memset(buf,0,ISIS_MSGSIZE);
	      if (ReadClientSocket(&client,buf)>0){
	        if(client.isVerbose && useCLI) printf("IN: %s\n",buf);
	        SocketCommand(buf);
	        if(useCLI) rl_refresh_line(0,0);
	      }
      }
      
      // add any new FD handlers here...
      
    } // end of select() I/O handling checking
    
  } // bottom of the while(client.KeepGoing) loop
	 
  //----------------------------------------------------------------
  // If we got here, the client was instructed to shut down
  
  printf("\nMODS Enviromental Monitor Agent Shutting Down...\n");
  
  // Tear down the application's client socket
  CloseClientSocket(&client);

  // Close any open data logs
  logMessage(&env,(char *)"modsHeb agent shutting down");

  if (env.logFD > 0) close(env.logFD);  //Closes the ascii log.
  closeTelemetryData(&env);             //Closes the hdf5 log.

  // Free dynamically allocated memory
  freeEnvData(&env);

  // Remove the readline() callback handler
  if (useCLI) rl_callback_handler_remove();

  return 0;
}

//---------------------------------------------------------------------------

/*!
  \brief Service Ctrl+C Interrupts (SIGINT signals)

  SIGINT signal trap for trapping Ctrl+C interrupts.
*/
void HandleInt(int signalValue){
  if (client.Debug && useCLI) printf("Caught Ctrl+C Abort...\n");
}

/*!
  \brief Service Kill Interrupts (SIGINT signals)

  SIGINT signal trap for trapping Kill interrupts.
*/
void HandleKill(int signalValue){
  if (client.Debug && useCLI) printf("Caught KILL...\n");
  if (env.logFD > 0) close(env.logFD);
}

