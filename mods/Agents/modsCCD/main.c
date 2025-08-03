/*!
  \mainpage modsCCD - Interactive azcam Client

  \author R. Pogge, OSU Astronomy Dept. (pogge.1@osu.edu)
  \date 2005 May 2

  \section Usage

  Usage: modsCCD [rcfile]

  Where: \c rcfile is an optional runtime config file to load.  

  By default, modsCCD uses the runtime config file defined by
  #DEFAULT_RCFILE in the client.h header.

  \section Introduction

  ...

  modsCCD can be run as either a standalone interactive program with a
  command-line interface and backdoor socket interface, or as a client
  in an ISIS system.  Using the agent in standalone mode with the
  backdoor socket interface (the same UDP socket used for ISIS server
  communications), we have successfully run the agent from a Perl script
  and other external processes that know how to communicate in the IMPv2
  messaging syntax.

  \section Commands

  These are the interactive commands for modsCCD:
  <pre>
  info           - report client information
  version        - report modsCCD version & compile info
  reset          - reset runtime & controller parameters
  verbose        - toggle verbose output mode
  debug          - toggle debugging output
  quit           - quit modsCCD
  history        - show command history
  !!             - repeat last command
  !cmd           - repeat last command matching 'cmd'
  help or ?      - view this list
  </pre>
  Note that all commands are <em>case-insensitive</em>.

  \section Config Runtime Configuration File

  This is a typical runtime config file for the modsCCD agent:
  \verbinclude modsCCD.ini

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
  \brief modsCCD main program and I/O event handler.
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
  char msgStr[256];

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
    printf("Unable to load the runtime config file, modsCCD aborting\n");
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
  if (ccd.FD>0) {
    printf("Opened connection to azcam server host %s:%d\n",
	   ccd.Host,ccd.Port);
    if (initCCDConfig(&ccd,reply)<0) {
      printf("CCD Initialization Failed - %s\n",reply);
    }
    else {
      printf("%s\n",reply);
    }
  }
  else {
    printf("ERROR: Could not open connection to azcam server host %s:%d\n",
	   ccd.Host,ccd.Port);
    printf("       Make sure the azcam server and archon are running,\n");
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
      printf("Failed to PING the ISIS server\n",strerror(errno));
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
  // state, which it tracks using the ccd.State flag.
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
  // If ccd.State = IDLE, the handler just waits for input (select()
  // call with no timeout).  If, however, there is an azcam server
  // connected, then every 120 seconds it queries the azcam server to
  // read the CCD and base temperatures and do housekeeping tasks
  //
  // During exposures, ccd.State can change as fast as 180msec, so
  // run the select() loop with a 50msec timeout (Archons are *FAST*!)
  //
  // If ccd.State = PAUSE indicating a paused exposure, so we go into
  // a minimal polling state like being idle until ccd.state is
  // set to RESUME, and we resume the fast exposure cadence.
  // 
  // When ccd.State changes to READOUT, it sets up the select() loop
  // with a 1 second timeout and polls the azcam server for the
  // readout status by watching the pixel counter, reporting readout
  // progress (typically few 10s of seconds) to the console or 
  // remote client.
  //
  // When readout is complete, ccd.State changes to READ while the
  // controller does post-readout cleanup in preparation for
  // image write.  We keep the 1-second timeout running, and
  // inform the console or remote client that readout is complete.
  //
  // When ccd.State changes to WRITING, we inform the console
  // or remote client that writing has commenced.
  //
  // When the image is written and closed, the azcam server
  // sets ccd.State = IDLE and we switch back to the normal
  // non-exposure select() loop timing.
  // 
  //
  // !*** this is where the exposure progress event loop starts ***!
  
  client.KeepGoing = 1;

  if (ccd.FD>0) 
    expStatus(&ccd,reply);

  while (client.KeepGoing) {
    
    FD_ZERO(&read_fd); // clear the table of active file descriptors
    
    // we always listen for console keyboard input
    
    FD_SET(kbdFD, &read_fd);
    
    // if enabled, listen to this application's UDP socket
    
    if (client.FD > 0) 
      FD_SET(client.FD, &read_fd);
    
    // Also listen to the azcam server comm port, if active
    
    if (ccd.FD > 0) 
      FD_SET(ccd.FD, &read_fd);    

    /*
    // Listen to DataMan if active and we're standalone

    if (dm.FD > 0 && (!client.useISIS)) 
      FD_SET(dm.FD, &read_fd);
    */
    
    // Do the select() call and wait for activity on any of our comm
    // ports or the console keyboard
     
    n_ready = 0;

    // Setup the select() loop timeout depending on the exposure state

    switch(ccd.State) {
      
    case EXPOSING:
    case SETUP:
    case RESUME:
      // azcam server is busy integrating. Poll at 50-100msec

      timeout.tv_sec = 0;
      timeout.tv_usec = 100000;
      n_ready = select(sel_wid, &read_fd, NULL, NULL, &timeout);
      break;

    case READ:
    case READOUT:
    case WRITING:
    case ABORT:
      // azcam server is busy reading out, writing, servicing abort,
      //   setup select() for 0.1s polling. Shortest readout is ~1.5 sec
      
      timeout.tv_sec = 0;
      timeout.tv_usec = 200000;
      n_ready = select(sel_wid, &read_fd, NULL, NULL, &timeout);
      break;

    default:
      // azcam server is idle or paused
      //   setup select() for 60s polling for idle-time housekeeping

      if (ccd.FD>0) {
	timeout.tv_sec = 60; // was 120s
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

      switch(ccd.State) {      

      case SETUP: // we were setting up, poll current status
	if (expStatus(&ccd,reply)<0) {
	  sprintf(reply,"GO %s",reply);
	  notifyClient(&ccd,&obs,reply,STATUS);
	  //ccd.State = IDLE;
	}
	
	switch(ccd.State) {

	case EXPOSING:
	case RESUME:
	  printf("\nStarted %.1f sec exposure              \r",obs.expTime);
	  fflush(stdout);
	  sprintf(reply,"GO started %.1f sec exposure",obs.expTime);
	  notifyClient(&ccd,&obs,reply,STATUS);
	  break;

	case READOUT: // could happen if a bias/zero image
	  printf("\nReadout Started                           \r");
	  fflush(stdout);
	  sprintf(reply,"GO Readout Started PCTREAD=0");
	  notifyClient(&ccd,&obs,reply,STATUS);
	  break;
	  
	default:
	  break; // nothing to report
	}
	// end of SETUP handling
	break;
	
      case READOUT: // we're reading out, poll current readout status
	if (pollReadout(&ccd,&obs,reply)<0) {
	  sprintf(reply,"GO %s",reply);
	  notifyClient(&ccd,&obs,reply,ERROR);
	  //ccd.State = IDLE;
	}
	
	// pollReadout() sets ccd.State depending on the polling outcome
	
	switch(ccd.State) {

	case READOUT: // still reading out, status to console and remote as needed
	  printf("Read out %d pixels of %d                   \r",ccd.Nread,ccd.Npixels);
	  fflush(stdout);
	  pctRead = 100.0*float(ccd.Nread)/float(ccd.Npixels); // percent readout
	  sprintf(reply,"GO PCTREAD=%d",int(pctRead));
 	  notifyClient(&ccd,&obs,reply,STATUS);
	  break;

	case READ:  // readout is complete, preparing to write
	  printf("\nReadout Complete, preparing to write image\n");
	  strcpy(msgStr,"GO Readout Complete PCTREAD=100");
	  notifyClient(&ccd,&obs,msgStr,STATUS);
	  break;
	  
	case WRITING: // started writing the image (in case READ state was missed)
	  printf("\nWriting image to disk\n");
	  fflush(stdout);
	  strcpy(msgStr,"GO Writing Image PCTREAD=100");
	  sprintf(reply,msgStr,int(pctRead));
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
	  notifyClient(&ccd,&obs,reply,STATUS);
	  //ccd.State = IDLE;
	}
	
	// pollExposure() sets ccd.State depending on the polling outcome
	
	switch(ccd.State) {
	  
	case EXPOSING:  // still exposing, update the countdown on console as required
	case RESUME:
	  if (obs.doCountDown) {
	    if (obs.tLeft > 0.0) {
	      printf("Exposing: %.1f of %.1f sec                \r",obs.tLeft,obs.expTime);
	      fflush(stdout);
	    }
	  }

	  // notify client of exposure countdown on interval obs.keepAlive if needed

	  if (obs.expTime > obs.keepAlive) {
	    obs.tNow = SysTimestamp();
	    dt = obs.tNow - obs.t1;
	    // printf("dt=%.3f obs.t1=%.3f obs.tNow=%.3f obs.keepAlive=%.3f\n",dt,obs.t1,obs.tNow,obs.keepAlive);
	    if (dt >= 120.0) {
	      obs.t1 = obs.tNow; // reset timer
	      sprintf(msgStr,"GO Exposing: %.1f of %.1f sec remaining",obs.tLeft,obs.expTime);
	      notifyClient(&ccd,&obs,msgStr,STATUS);
	    }
	  }
	  
	  break;
	  
	case READOUT:  // started readout since last time we polled
	  printf("\nExposure Completed, Reading out                   \n");
	  strcpy(msgStr,"GO Exposure Completed, Shutter=0 (Closed), Readout started PCTREAD=0");
	  notifyClient(&ccd,&obs,msgStr,STATUS);
	  break;
	  
	case ABORT:  // exposure done since the last time we polled
	  printf("\nExposure Aborting, wait for completion\n");
	  strcpy(msgStr,"GO Exposure Aborting, waiting for completion");
	  notifyClient(&ccd,&obs,msgStr,STATUS);
	  break;

	case PAUSE: // pause requested
	  printf("\nExposure paused, waiting for RESUME or ABORT\n");
	  strcpy(msgStr,"GO Exposure Paused, waiting for RESUME or ABORT");
	  notifyClient(&ccd,&obs,msgStr,STATUS);
	  break;

	default:
	  break;
	}
	// end of EXPOSING handling
	break;
	
      case PAUSE: // exposure was paused, did this change?
	if (expStatus(&ccd,reply)<0) {
	  notifyClient(&ccd,&obs,reply,ERROR);
	  //ccd.State = IDLE;
	}
	
	switch(ccd.State) {

	case RESUME:
	case EXPOSING:
	  pollExposure(&ccd,&obs,reply);
	  printf("\nExposure resumed\n");
	  printf("Exposing: %.1f of %.1f sec remaining           \r",obs.tLeft,obs.expTime);
	  fflush(stdout);
	  strcpy(msgStr,"Exposure Resumed");
	  notifyClient(&ccd,&obs,msgStr,STATUS);
	  break;

	case ABORT:
	  printf("\nPAUSED Exposure Aborted\n");
	  strcpy(msgStr,"GO Paused Exposure ABORTED, waiting for completion");
	  notifyClient(&ccd,&obs,msgStr,STATUS);
	  break;
	  
	default:
	  break; // otherwise nothing to do
	}
	// end of PAUSE handling
	break;

      case READ:  // readout complete, waiting for write
	if (expStatus(&ccd,reply)<0) {
	  notifyClient(&ccd,&obs,reply,ERROR);
	  //ccd.State = IDLE;
	}
	
	switch(ccd.State) {

	case WRITING:
	  printf("\nWriting CCD to disk\n");
	  strcpy(msgStr,"GO Writing image to disk PCTREAD=100");
	  notifyClient(&ccd,&obs,msgStr,STATUS);
	  break;

	default:
	  break;
	}
	// end of READ handling
	break;
		 
      case WRITING: // azcam server is writing the image to disk, waiting for IDLE
	if (expStatus(&ccd,reply)<0) {
	  notifyClient(&ccd,&obs,reply,ERROR);
	  //ccd.State = IDLE;
	}
	
	switch(ccd.State) {

	case IDLE:
	  printf("Done: Image readout and written to disk\n");
	  strcpy(msgStr,"GO Exposure finished. EXPSTATUS=DONE");
	  notifyClient(&ccd,&obs,msgStr,DONE);
	  break;

	default:
	  break;
	}
	// end of WRITING handlng
	break;

      case ABORT: // exposure is aborting, waiting for IDLE
	if (expStatus(&ccd,reply)<0) {
	  notifyClient(&ccd,&obs,reply,ERROR);
	  //ccd.State = IDLE;
	}

	switch(ccd.State) {

	case IDLE:
	  printf("\nDone: Exposure abort complete.\n");
	  strcpy(msgStr,"GO Exposure Aborted. EXPSTATUS=DONE");
	  notifyClient(&ccd,&obs,msgStr,DONE);
	  break;

	default:
	  break;
	}
	// End of ABORT handling
	break;
      
      case IDLE: // azcam server is IDLE, check for SETUP, otherwise housekeeping
	if (expStatus(&ccd,reply)<0) {
	  notifyClient(&ccd,&obs,reply,STATUS);
	  //ccd.State = IDLE;
	}

	switch(ccd.State) {

	case SETUP:
	  printf("\nExposure Setup started                     \r");
	  fflush(stdout);
	  strcpy(msgStr,"GO Exposure Setup started");
	  notifyClient(&ccd,&obs,msgStr,STATUS);
	  break;

	default:
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
	  printf("select() interrupted by Ctrl+C, continuing\n");
      }
      else { // something else bad happened, let us know
	printf("Warning: select() failed - %s - pressing on anyway\n",
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
      
      if (ccd.FD > 0) {
	if (FD_ISSET(ccd.FD, &read_fd)) {
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

  printf("\nMODS CCD client shutting down\n");

  // Tear down the azcam server link
  
  if (ccd.FD>0) 
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

  if (ccd.FD>0)
    abortExposure(&ccd,reply);

  printf("Ctrl+C Abort requested - Aborts Sent\n");

}

