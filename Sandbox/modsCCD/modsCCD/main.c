/*!
  \mainpage y4kcam - Interactive azcam Client

  \author R. Pogge, OSU Astronomy Dept. (pogge@astronomy.ohio-state.edu)
  \date 2005 May 2

  \section Usage

  Usage: y4kcam [rcfile]

  Where: \c rcfile is an optional runtime config file to load.  

  By default, y4kcam uses the runtime config file defined by
  #DEFAULT_RCFILE in the client.h header.

  \section Introduction

  ...

  y4kcam can be run as either a standalone interactive program with a
  command-line interface and backdoor socket interface, or as a client
  in an ISIS system.  Using the agent in standalone mode with the
  backdoor socket interface (the same UDP socket used for ISIS server
  communications), we have successfully run the agent from a Perl script
  and other external processes that know how to communicate in the IMPv2
  messaging syntax.

  \section Commands

  These are the interactive commands for y4kcam:
  <pre>
  info           - report client information
  version        - report y4kcam version & compile info
  reset          - reset runtime & controller parameters
  verbose        - toggle verbose output mode
  debug          - toggle debugging output
  quit           - quit y4kcam
  history        - show command history
  !!             - repeat last command
  !cmd           - repeat last command matching 'cmd'
  help or ?      - view this list
  </pre>
  Note that all commands are <em>case-insensitive</em>.

  \section Config Runtime Configuration File

  This is a typical runtime config file for the y4kcam agent:
  \verbinclude y4kcam.ini

  Note that all parameter names are \e case-insensitive.

  \section Notes

  This application uses the ISISclient library (link).

  \section Mods Modification History

<pre>
2005 February 16 - new application [rwp/osu]
</pre>

\todo 
<ul>
<li>Add commands for opening/closing/changing comm ports
<li>Add commands for loading/saving config files by name
<li>Add a command for executing an input sensor scan w/output
<li>Add a command for operating a brake
<li>Add a command apropos facility
</ul>
*/

/*!
  \file main.c
  \brief y4kcam main program and I/O event handler.
*/

#include "isisclient.h"  // ISIS common client library header
#include "client.h"      // Custom client application header

// The client cli uses the GNU readline and history utilities

#include <readline/readline.h>
#include <readline/history.h>

// Global data structures

isisclient_t client; // ISIS Client data structure

azcam_t ccd;         // azcam server parameters data structure

obsPars_t obs;       // Observation parameters data structure

// dataman_t dm;        // Data Manager data structure

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
  
  int expCount = 0;   // running exposure tick reports
  int numExpRep = 60; // max number of exposure tick reports
  
  double dt;
  float pctRead;

  char buf[ISIS_MSGSIZE]; // command/message buffer
  char reply[256];   // generic reply string

  char camData[256];   // raw azcam socket port string
  int lastchar;
  char cmdstr[64];

  // readline & history handling stuff
  
  char cliPrompt[ISIS_NODESIZE+2]; // the console prompt is our ISIS node name
  
  // select() event handler parameters
  
  fd_set read_fd;
  int kbdFD;
  int n_ready;
  struct timeval timeout;
  static int sel_wid;
  
  // Basic initializations
  
  sel_wid = getdtablesize();
  kbdFD = fileno(stdin);  // file descriptor of stdin, safe definition
  
  // Parse the command line 
  
  if (argc>2) {
    printf("usage: %s [rcfile]\n", argv[0]);
    printf("where: rcfile = optional runtime config file (default %s)\n",
	   DEFAULT_RCFILE);
    exit(1);
  }
  
  // So far so good, give the welcome information
  
  printf("\n");
  printf("  -------------------------------------------\n");
  printf("                    modsCCD\n");
  printf("  Interactive MODS CCD (aka IC) Control Agent\n\n");

  printf("  Version: %s (%s %s)\n",APP_VERSION,APP_COMPDATE,APP_COMPTIME);
  printf("  -------------------------------------------\n\n");
  printf("\n");

  // Load the specified runtime config file, or use the default if none given

  if (argc==2)
    n = loadConfig(argv[1]);
  else
    n = loadConfig(DEFAULT_RCFILE);

  if (n!=0) {
    printf("Unable to load the runtime config file...modsCCD aborting\n");
    exit(1);
  }

  // Now we do various initializations

  // If required, initialize the socket connection to the ISIS server
  // We can disable ISIS interaction by specifying "ServerID None" in
  // the runtime config file

  if (client.useISIS) {
    if (InitISISServer(&client)<0) {
      printf("ISIS server connection initialization failed - aborting\n");
      exit(2);
    }
  }

  // Open the client network socket port for interprocess communications
  
  if (OpenClientSocket(&client)<0) {
    printf("Client socket initialization failed - aborting\n");
    exit(3);
  }

  if (client.useISIS)
    printf("Started modsCCD as ISIS client node %s on %s port %d\n",
	   client.ID, client.Host, client.Port);
  else
    printf("Started modsCCD as standalone agent %s on %s port %d\n",
	   client.ID, client.Host, client.Port);
  
  // Now, open the azcam server connection and attempt to initialize the session
  
  openAzCam(&ccd,reply);
  if (cam.FD>0) {
    printf("Opened connection to azcam server host %s:%d\n",
	   cam.Host,cam.Port);
    if (initCCDConfig(&ccd,reply)<0) {
      printf("CCD Initialization Failed - %s\n",reply);
      printf("  Options: 1) Quit and fix the problem with the %s script\n",cam.iniFile);
      printf("           2) Try to configure using interactive commands\n");
    }
    else {
      printf("%s\n",reply);
    }
  }
  else {
    printf("ERROR: Could not open connection to azcam server host %s:%d\n",
	   cam.Host,cam.Port);
    printf("       Make sure the system is on and properly configured,\n");
    printf("       then try again.\n");
    printf("modsCCD aborting\n");
    exit(3);
  }
  
  // Broadcast a PING to the ISIS server, if enabled.  If it fails,
  // we'll have to do the ping by hand after the comm loop starts.

  if (client.useISIS) {
    memset(buf,0,ISIS_MSGSIZE);
    sprintf(buf,"%s>%s ping\r",client.ID,client.isisID);
    if (SendToISISServer(&client,buf)<0) 
      printf("Failed to PING the ISIS server...\n",strerror(errno));
    if (client.isVerbose)
      printf("OUT: %s\n",buf);
  }

  // Initialize the DataMan link, if used

  /*
  strcpy(dm.Name,client.ID); // copy client ID into dm.Name member

  if (dm.useDM) {
    if (client.useISIS) {
      dm.FD = client.FD;  // use the ISIS socket
    }
    else {
      if (openDataMan(&dm,0)<0) { // open on any free localhost port
	printf("\nWARNING: Could not connect to the DataMan Agent\n");
	printf("Disabling DataMan Link ...\n");
	dm.FD = -1;
      }
    }
    if (dm.FD>0) {
      if (client.useISIS)
	printf("DataMan Link via ISIS initialized\n");
      else
	printf("DataMan Direct Link initialized\n");
    }
  }
  else {
    printf("NOTE: Started with no DataMan interface\n");
  }
  */
  
  // Now that all components are connected, upload the
  // baseline FITS header database

  getTemp(&ccd,reply);
  uploadFITS(&ccd,&obs,reply);

  // All set to rock-n-roll...

  printf("\n----------------------------------------------\n");
  printf("Type 'quit' to terminate the interactive session\n");
  printf("Type 'help' to see a list of commands\n");
  printf("Type 'reset' to reset the session\n");
  printf("----------------------------------------------\n");

  // Startup the command-line history mechanism

  using_history();

  // Setup the command prompt and install the readline() callback
  // handler for this application (KeyboardCommand() in commands.c)

  sprintf(cliPrompt,"%s%% ",client.ID);
  rl_callback_handler_install(cliPrompt,KeyboardCommand);
  
  // Set the SIGINT signal trap 
  
  signal(SIGINT,HandleInt); // Ctrl+C sends a move abort to controller
  signal(SIGPIPE,SIG_IGN);  // ignore broken pipes

  //----------------------------------------------------------------------
  //
  // Start the I/O event handling loop.
  //
  // The event handler has to have some awareness of the azcam server
  // state, which it tracks using the cam.State data member.
  //
  // For the Archon controller, the sequence of exposure states are:
  //
  //   (IDLE)
  //    SETUP
  //    EXPOSING
  //       PAUSE/RESUME/ABORT
  //    READOUT
  //    READ
  //    WRITING
  //    (IDLE)
  //    
  // If cam.State = IDLE, the handler just waits for input (select()
  // call with no timeout).  If, however, there is an azcam server
  // connected, then every 120 seconds it queries the azcam server to
  // read the CCD and base temperatures and do housekeeping tasks
  //
  // When cam.State changes to SETUP or EXPOSING, it sets up the
  // select() loop with a 0.5 second timeout to watch for any
  // Abort/Pause/Resume directives from the console or remote clients,
  // and then polls the azcam for the current integration status,
  // maintaining a parallel countdown.  On integration completion, it
  // switches the state to cam.State=READOUT as appropriate.
  //
  // If cam.State = PAUSE indicating a paused exposure, so we go into
  // a minimal polling state like being idle until cam.state is
  // set to RESUME, and we resume the fast exposure cadence.
  // 
  // When cam.State changes to READOUT, it sets up the select() loop
  // with a 1 second timeout and polls the azcam server for the
  // readout status by watching the pixel counter, reporting readout
  // progress (typically few 10s of seconds) to the console or 
  // remote client.
  //
  // When readout is complete, cam.State changes to READ while the
  // controller does post-readout cleanup in preparation for
  // image write.  We keep the 1-second timeout running, and
  // inform the console or remote client that readout is complete.
  //
  // When cam.State changes to WRITING, we inform the console
  // or remote client that writing has commenced.
  //
  // When the image is written and closed, the azcam server
  // sets cam.State = IDLE and we switch back to the normal
  // non-exposure select() loop timing.
  // 
  //
  // !*** this is where the exposure progress event loop starts ***!
  
  client.KeepGoing = 1;
  cam.State = IDLE;

  while (client.KeepGoing) {
    
    FD_ZERO(&read_fd); // clear the table of active file descriptors
    
    // we always listen for console keyboard input
    
    FD_SET(kbdFD, &read_fd);
    
    // if enabled, listen to this application's UDP socket
    
    if (client.FD > 0) 
      FD_SET(client.FD, &read_fd);
    
    // Also listen to the azcam server comm port, if active
    
    if (cam.FD > 0) 
      FD_SET(cam.FD, &read_fd);    

    /*
    // Listen to DataMan if active and we're standalone

    if (dm.FD > 0 && (!client.useISIS)) 
      FD_SET(dm.FD, &read_fd);
    */
    
    // Do the select() call and wait for activity on any of our comm
    // ports or the console keyboard
     
    n_ready = 0;

    // Setup the select() loop timeout depending on the exposure state

    switch(cam.State) {
      
    case EXPOSING:
    case RESUME:
      // azcam server is busy integrating
      //   setup select() for 0.5s polling

      timeout.tv_sec = 0;
      timeout.tv_usec = 500000;
      n_ready = select(sel_wid, &read_fd, NULL, NULL, &timeout);
      break;

    case READOUT:
    case SETUP:
    case WRITING:
    case ABORT:
    case READ:
      // azcam server is busy reading out, writing, setup, servicing abort,
      //   setup select() for 1s polling
      
      timeout.tv_sec = 1;
      timeout.tv_usec = 0;
      n_ready = select(sel_wid, &read_fd, NULL, NULL, &timeout);
      break;

    default:
      // azcam server is idle or paused
      //   setup select() for 120s polling for idle-time housekeeping
      
      if (cam.FD>0) {
	timeout.tv_sec = 120;
	timeout.tv_usec = 0;
	n_ready = select(sel_wid, &read_fd, NULL, NULL, &timeout);
      }
      else {
	n_ready = select(sel_wid, &read_fd, NULL, NULL, NULL);
      }
      break;
      
    }
    
    //----------------------------------------------------------------
    //
    // select() done, take action if something is ready
    //

    // select() timed out, do activities after timeout as required by the azcam state

    if (n_ready == 0) {

      switch(cam.State) {      

      case SETUP: // we were setting up, poll current status
	if (expStatus(&ccd,reply)<0) {
	  notifyClient(&ccd,&obs,reply,ERROR);
	  cam.State = IDLE;
	}
	
	switch(cam.State) {

	case EXPOSING:
	case RESUME:
	  printf("\nStarted %.2f sec exposure...              \r",obs.expTime);
	  fflush(stdout);
	  sprintf(reply,"%.2f sec exposure started",obs.expTime);
	  notifyClient(&ccd,&obs,reply,STATUS);
	  break;

	case READOUT: // could happen if a bias/zero image
	  printf("\nReadout Started                           \r");
	  fflush(stdout);
	  sprintf(reply,"Readout Started PCTREAD=0");
	  notifyClient(&ccd,&obs,reply,STATUS);
	  break;
	  
	default:
	  break; // nothing to report
	}
	// end of SETUP handling
	break;
	
      case READOUT: // we're reading out, poll current readout status
	if (pollReadout(&ccd,&obs,reply)<0) {
	  notifyClient(&ccd,&obs,reply,ERROR);
	  cam.State = IDLE;
	}
	
	// pollReadout() sets cam.State depending on the polling outcome
	
	switch(cam.State) {

	case READOUT: // still reading out... status to console and remote as needed
	  printf("Read out %d pixels of %d...                \r",cam.Nread,cam.Npixels);
	  fflush(stdout);
	  pctRead = 100.0*float(cam.Nread)/float(cam.Npixels); // percent readout
	  sprintf(reply,"PCTREAD=%d",int(pctRead));
 	  notifyClient(&ccd,&obs,reply,STATUS);
	  break;

	case READ:  // readout is complete, preparing to write
	  printf("\nReadout Complete, preparing to write image");
	  notifyClient(&ccd,&obs,(char*)"Readout Complete PCTREAD=100",STATUS);
	  break;
	  
	case WRITING: // started writing the image (in case READ state was missed)
	  printf("\nWriting image %s to disk",cam.fileName);
	  fflush(stdout);	
	  sprintf(reply,"Writing Image PCTREAD=100",int(pctRead));
 	  notifyClient(&ccd,&obs,reply,STATUS);
	  break;
	  	  
	default:  // nothing else requires action
	  break;
	}
	// end of READOUT handling
	break;

      case EXPOSING: // poll current exposure status - RESUME set instead of EXPOSING after PAUSE
      case RESUME:
	if (pollExposure(&ccd,&obs,reply)<0) {
	  notifyClient(&ccd,&obs,reply,ERROR);
	  cam.State = IDLE;
	}
	
	// pollExposure() sets cam.State depending on the polling outcome
	
	switch(cam.State) {
	  
	case EXPOSING:  // still exposing, update the countdown on console as required
	case RESUME:
	  if (obs.doCountDown) {
	    if (obs.tLeft > 0.0) {
	      printf("ExpTime %.2f sec - Time Remaining %d sec...             \r",obs.expTime,(int)(obs.tLeft));
	      fflush(stdout);
	    }
	  }
	  
	  // notify client of exposure countdown - need code to keep reasonable, helps
	  // keep-alive state for remote UDP socket clients during long (>5min) exposures
	  
	  break;
	  
	case READOUT:  // started readout since last time we polled
	  printf("\nExposure Completed, Readout started...                  \n");
	  notifyClient(&ccd,&obs,(char*)"Exposure Completed, Shutter=0 (Closed), Readout started PCTREAD=0",STATUS);
	  break;
	  
	case ABORT:  // exposure done since the last time we polled
	  printf("\nExposure Aborting, wait for completion...\n");
	  notifyClient(&ccd,&obs,(char*)"Exposure Aborting, waiting for completion",STATUS);
	  break;

	case PAUSE: // pause requested
	  printf("\nExposure paused, waiting for RESUME or ABORT...\n");
	  notifyClient(&ccd,&obs,(char*)"Exposure Paused, waiting for RESUME or ABORT",STATUS);
	  break;

	default:
	  break;
	}
	// end of EXPOSING handling
	break;
	
      case PAUSE: // exposure was paused, did this change?
	if (expStatus(&ccd,reply)<0) {
	  notifyClient(&ccd,&obs,reply,ERROR);
	  cam.State = IDLE;
	}
	
	switch(cam.State) {

	case RESUME:
	case EXPOSING:
	  pollExposure(&ccd,&obs,reply);
	  printf("\nExposure resumed...\n");
	  printf("ExpTime %.2f sec - Time Remaining %d sec...             \r",obs.expTime,(int)(obs.tLeft));
	  fflush(stdout);
	  notifyClient(&ccd,&obs,(char*)"Exposure Resumed",STATUS);
	  break;

	case ABORT:
	  printf("\nPAUSED Exposure Aborted...\n");
	  notifyClient(&ccd,&obs,(char*)"Paused Exposure ABORTED, waiting for completion",STATUS);
	  break;
	  
	default:
	  break; // otherwise nothing to do
	}
	// end of PAUSE handling
	break;

      case READ:  // readout complete, waiting for write
	if (expStatus(&ccd,reply)<0) {
	  notifyClient(&ccd,&obs,reply,ERROR);
	  cam.State = IDLE;
	}
	
	switch(cam.State) {

	case WRITING:
	  printf("\nWriting CCD to disk...\n");
	  notifyClient(&ccd,&obs,(char*)"Writing image to disk PCTREAD=0",STATUS);
	  break;

	default:
	  break;
	}
	// end of READ handling
	break;
		 
      case WRITING: // azcam server is writing the image to disk, waiting for IDLE
	if (expStatus(&ccd,reply)<0) {
	  notifyClient(&ccd,&obs,reply,ERROR);
	  cam.State = IDLE;
	}
	
	switch(cam.State) {

	case IDLE:
	  printf("Done: Image readout and written to disk\n");
	  notifyClient(&ccd,&obs,(char*)"Exposure finished. EXPSTATUS=DONE",DONE);
	  break;

	default:
	  break;
	}
	// end of WRITING handlng
	break;

      case ABORT: // exposure is aborting, waiting for IDLE
	if (expStatus(&ccd,reply)<0) {
	  notifyClient(&ccd,&obs,reply,ERROR);
	  cam.State = IDLE;
	}

	switch(cam.State) {

	case IDLE:
	  printf("\nDone: Exposure abort complete.\n");
	  notifyClient(&ccd,&obs,(char*)"Exposure Aborted. EXPSTATUS=DONE",DONE);
	  break;

	default:
	  break;
	}
	// End of ABORT handling
	break;
      
      case IDLE: // azcam server is IDLE, check for SETUP, otherwise housekeeping
	if (expStatus(&ccd,reply)<0) {
	  notifyClient(&ccd,&obs,reply,ERROR);
	  cam.State = IDLE;
	}

	switch(cam.State) {

	case SETUP:
	  printf("\nExposure Setup started..                 \r");
	  fflush(stdout);
	  notifyClient(&ccd,&obs,(char*)"Exposure Setup started",STATUS);
	  break;

	default:
	  if (cam.FD>0)
	    getTemp(&ccd,reply);
	  // do something with this someday, shmem, DD?
	  break;
	}
	// end of IDLE handling
	break;
	
      default:  // nothing to do, keep rolling
	break;
      }
      
    } 
    
    // select() returned an error, handle it
    
    else if (n_ready < 0) {
      if (errno == EINTR) { // caught Ctrl+C, hopefully sigint handler caught it
	if (client.Debug)
	  printf("select() interrupted by Ctrl+C...continuing\n");
      }
      else { // something else bad happened, let us know
	printf("Warning: select() failed - %s - pressing on anyway...\n",
	       strerror(errno));
      }
      rl_refresh_line(0,0);
      continue;
      
    }
    
    // select() has input to process on one of the input channels
    
    else {
      
      // Console keyboard input
      
      if (FD_ISSET(kbdFD, &read_fd)) {
	rl_callback_read_char();  // readline() command handler
	signal(SIGINT,HandleInt);  // reset the SIGINT handler
      }
      
      // Client socket input
      
      if (FD_ISSET(client.FD, &read_fd)) {
	memset(buf,0,ISIS_MSGSIZE);
	if (ReadClientSocket(&client,buf)>0) {
	  if (client.isVerbose) printf("IN: %s\n",buf);
	  SocketCommand(buf);
	  rl_refresh_line(0,0);
	}
      }
      
      // Unexpected input from the azcam server - just echo it to the
      // console for now.
      
      if (cam.FD > 0) {
	if (FD_ISSET(cam.FD, &read_fd)) {
	  nread = readAzCam(&ccd,camData);  // direct read, no timeout
	  if (nread > 0) {
	    printf("AzCam> %s\n",camData);
	    memset(camData,0,sizeof(camData));
	    rl_refresh_line(0,0);
	  }
	}
      } // end of azcam server TCP port handling
      
      // Message from the DataMan agent - just echo it to the console
      // for now.

      /*
      if (dm.FD > 0 && (!client.useISIS)) {
	if (FD_ISSET(dm.FD, &read_fd)) {
	  nread = ReadDataMan(&dm,camData);  // direct read, no timeout
	  if (nread > 0) {
	    printf("DM> %s\n",camData);
	    memset(camData,0,sizeof(camData));
	    rl_refresh_line(0,0);
	  }
	}
      }
      */
      
      // add any new FD handlers here...
      
    } // end of select() I/O handling checking
    
  } // bottom of the while(client.KeepGoing) loop
  
  //----------------------------------------------------------------
  //
  // If we got here, the client was instructed to shut down
  //

  printf("\nMODS CCD client shutting down...\n");

  // Tear down the azcam server link
  
  if (cam.FD>0) 
    closeAzCam(&ccd);

  // Tear down the DataMan link, if any

  /*
  if (dm.FD>0 && (!client.useISIS))
    CloseDataMan(&dm);
  */
  
  // Tear down the application's client socket

  CloseClientSocket(&client);

  // Remove the readline() callback handler

  rl_callback_handler_remove();

  // all done, say goodbye...

  printf("bye\n");
  
  exit(0);

}

//---------------------------------------------------------------------------

/*!
  \brief Service Ctrl+C Interrupts (SIGINT signals)

  SIGINT signal trap for trapping Ctrl+C interrupts.

  Calls Aborts for active exposures and/or filter wheel moves in
  progress when the abort was received.

*/

void
HandleInt(int signalValue)
{
  char reply[256];
  if (client.Debug)
    printf("Caught Ctrl+C Abort...\n");
  
  // If the CCD link is up, send an abort exposure (harmless if not exposing)

  if (cam.FD>0)
    abortExposure(&ccd,reply);

  printf("Ctrl+C Abort requested - Aborts Sent\n");

}

