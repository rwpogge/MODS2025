/*!
  \mainpage y4kcam - Interactive AzCam Client

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
azcam_t ccd;         // AzCam server parameters data structure
obspars_t obs;       // Observation parameters data structure
pctcs_t tcs;         // PC-TCS agent data structure
fwheel_t fw;         // Y4KCam Filter Wheel data structure
dataman_t dm;        // Data Manager data structure

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

  char buf[ISIS_MSGSIZE]; // command/message buffer
  char reply[256];   // generic reply string

  char camData[256];   // raw AzCam socket port string
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
  printf("                    y4kcam\n");
  printf("  Interactive Y4KCam Instrument Control Agent\n\n");

  printf("  Version: %s (%s %s)\n",APP_VERSION,APP_COMPDATE,APP_COMPTIME);
  printf("  -------------------------------------------\n\n");
  printf("\n");

  // Load the specified runtime config file, or use the default if none given

  if (argc==2)
    n = LoadConfig(argv[1]);
  else
    n = LoadConfig(DEFAULT_RCFILE);

  if (n!=0) {
    printf("Unable to load the runtime config file...y4kcam aborting\n");
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
    printf("Started y4kcam as ISIS client node %s on %s port %d\n",
	   client.ID, client.Host, client.Port);
  else
    printf("Started y4kcam as standalone agent %s on %s port %d\n",
	   client.ID, client.Host, client.Port);
  
  // Now, open the AzCam server connection and attempt to initialize the session
  
  OpenAzCam(&ccd,reply);
  if (ccd.FD>0) {
    printf("Opened connection to AzCam Server host %s:%d\n",
	   ccd.Host,ccd.Port);
    if (InitCCDConfig(&ccd,reply)<0) {
      printf("CCD Initialization Failed - %s\n",reply);
      printf("  Options: 1) Quit and fix the problem with the %s script\n",
	     ccd.IniFile);
      printf("           2) Try to configure using interactive commands\n");
    }
    else {
      printf("%s\n",reply);
    }
  }
  else {
    printf("ERROR: Could not open connection to AzCam server host %s:%d\n",
	   ccd.Host,ccd.Port);
    printf("       Make sure the system is on and properly configured,\n");
    printf("       then try again.\n");
    printf("y4kcam aborting\n");
    exit(3);
  }
  
  // Initialize the filter wheel if enabled

  if (fw.useFW) {
    
    // Initialize the filter table
    
    fw.FilterID = (char **)malloc(MAX_FILTERS * sizeof(char *));
    for (i=0; i<MAX_FILTERS; i++)
      (fw.FilterID)[i] = (char *)calloc(FWHEEL_IDSIZE, sizeof(char));
    
    // Startup the filter wheel and read the starting position
    
    if (FWStartup(&fw)<0) {
      printf("\nWARNING: Could not initialize the filter wheel\n");
      printf("Disabling filter wheel - restart later with FWINIT.\n");
      fw.Link = 0;
    }
    else {
      printf("Filter Wheel initialized, starting position %d [%s]\n",
	     fw.beampos,fw.FilterID[fw.beampos-1]);
      obs.Filter = fw.beampos;
      strcpy(obs.FilterID,(fw.FilterID)[fw.beampos-1]);
    }
  }
  else {
    printf("NOTE: Started with no Filter Wheel enabled.\n");
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

  // Initialize the PC-TCS interface if used
  
  strcpy(tcs.Name,client.ID); // copy client ID into tcs.Name member

  if (tcs.useTCS) {
    if (client.useISIS) {
      tcs.FD = client.FD;  // use the ISIS socket
    }
    else {
      if (OpenPCTCS(&tcs,0)<0) { // open on any free localhost port
	printf("\nWARNING: Could not connect to the PC-TCS Agent\n");
	printf("Disabling TCS Link - restart later with TCINIT.\n");
	tcs.Link = 0;
	tcs.FD = -1;
      }
    }
    if (tcs.FD>0) {
      if (PCTCSCmd(&tcs,(char*)"tcinit",reply)<0) {
	printf("PC-TCS init failed - %s\n",reply);
	printf("Try again using tcinit by hand later\n");
	tcs.Link = 0;
      }
      else {
	tcs.Link = 1;
	GetPCTCSInfo(&tcs,reply);
      }
      if (client.useISIS)
	printf("PC-TCS Link via ISIS initialized\n");
      else
	printf("PC-TCS Direct Link initialized\n");
    }
  }
  else {
    printf("NOTE: Started with no PC-TCS interface\n");
  }

  // Initialize the DataMan link, if used
  
  strcpy(dm.Name,client.ID); // copy client ID into dm.Name member

  if (dm.useDM) {
    if (client.useISIS) {
      dm.FD = client.FD;  // use the ISIS socket
    }
    else {
      if (OpenDataMan(&dm,0)<0) { // open on any free localhost port
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

  // Now that all components are connected, upload the
  // baseline FITS header database

  GetTemp(&ccd,reply);
  UploadFITS(&ccd,&obs,reply);

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
  // The event handler has to have some awareness of the AzCam server
  // state, which it tracks using the ccd.State data member.
  //
  // If ccd.State = IDLE, the handler just waits for input (select()
  // call with no timeout).  If, however, there is an AzCam server
  // connected, then every 120 seconds it queries the AzCam server to
  // read the CCD and Dewar temperature.
  //
  // If ccd.State = EXPOSING, then it sets up select() with a 1
  // second timeout to watch for Abort directives from the console or
  // remote clients, and then polls the AzCam for the current
  // integration status, maintaining a parallel countdown.  On
  // integration completion, it switches the state to ccd.State=READOUT
  // as appropriate.
  // 
  // If ccd.State = READOUT, it similarly sets up select()  with a 1
  // second timeout and polls the AzCam server for the readout
  // status by watching the pixel counter.  When the pixel counter
  // reaches the expected number (ccd.Npixels), it fires off the
  // Write command and when write is done, sets the AzCam server state
  // flag back to IDLE.
  // 
  // If ccd.State = PAUSE, we have a paused exposure, so we go into
  // a minimal polling state like being idle.
  //

  client.KeepGoing = 1;
  ccd.State = IDLE;

  while (client.KeepGoing) {
    
    FD_ZERO(&read_fd); // clear the table of active file descriptors
    
    // we always listen for console keyboard input
    
    FD_SET(kbdFD, &read_fd);
    
    // if enabled, listen to this application's UDP socket
    
    if (client.FD > 0) 
      FD_SET(client.FD, &read_fd);
    
    // Also listen to the AzCam server comm port, if active
    
    if (ccd.FD > 0) 
      FD_SET(ccd.FD, &read_fd);    

    // Listen to DataMan if active and we're standalone

    if (dm.FD > 0 && (!client.useISIS)) 
      FD_SET(dm.FD, &read_fd);

    // Listen to PC-TCS agent if active and we're standalone

    if (tcs.FD > 0 && (!client.useISIS))
      FD_SET(tcs.FD, &read_fd);

    // Do the select() call and wait for activity on any of our comm
    // ports or the console keyboard
     
    n_ready = 0;

    // Setup select depending on the AzCam state

    switch(ccd.State) {
    case EXPOSING: // AzCamServer is busy integrating, setup select() for 0.5s exposure polling
      timeout.tv_sec = 0;
      timeout.tv_usec = 500000;
      n_ready = select(sel_wid, &read_fd, NULL, NULL, &timeout);
      break;

    case READOUT: // AzCamServer is busy reading out, setup select() for 1s readout polling
      timeout.tv_sec = 1;
      timeout.tv_usec = 0;
      n_ready = select(sel_wid, &read_fd, NULL, NULL, &timeout);
      break;
      
    default: // AzCam server is idle or paused, setup select() for 120s polling
      if (ccd.FD>0) {
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
    // select() done, take action depending on the value of n_ready
    // returned
    //

    // select() timed out, do activities after timeout as required by the AzCam state

    if (n_ready == 0) {

      switch(ccd.State) {

      case READOUT: // we're reading out, poll current readout status
	if (PollReadout(&ccd,&obs,reply)<0) {
	  NotifyClient(&obs,reply,ERROR);
	  ccd.State = IDLE;
	}
	
	// PollReadout() sets ccd.State depending on the polling outcome
	
	switch(ccd.State) {
	case IDLE:  // readout has completed since last time we polled, write image and wrap up
	  printf("\n");
	  NotifyClient(&obs,(char*)"Readout Complete, Writing image...",STATUS);
	  if (WriteCCDImage(&ccd,&obs,reply)<0)
	    NotifyClient(&obs,reply,ERROR);
	  else
	    NotifyClient(&obs,reply,STATUS);

	  // Any post-processing is done here

	  if (dm.useDM) {
	    if (ProcessCCDImage(&ccd,&obs,ccd.LastFile,reply)<0)
	      NotifyClient(&obs,reply,ERROR);
	    else
	      NotifyClient(&obs,reply,STATUS);
	  }

	  // All done at long last...

	  NotifyClient(&obs,(char*)"Exposure Completed.",DONE);
	  rl_refresh_line(0,0);
	  
	  break;
	  
	case READOUT: // still reading out, status to console and keep rolling
	  printf("Read out %d pixels of %d...                \r",
		 ccd.Nread,ccd.Npixels);
	  fflush(stdout);
	  break;
	  
	default:  // whatever, keep moving
	  break;
	}
	break;
	
      case EXPOSING: // poll current exposure status
	if (PollExposure(&ccd,&obs,reply)<0) {
	  NotifyClient(&obs,reply,ERROR);
	  ccd.State = IDLE;
	}
	
	// PollExposure() sets ccd.State depending on the polling outcome
	
	switch(ccd.State) {
	case EXPOSING:  // still exposing, give countdown on console as required
	  if (obs.doCountDown) {
	    if (obs.Tleft > 0.0) {
	      printf("ExpTime %.2f sec - Time Remaining %d sec...             \r",
		     obs.ExpTime,(int)(obs.Tleft));
	      fflush(stdout);
	    }
	  }
	  break;
	  
	case READOUT:  // started readout since last time we polled
	  printf("Exposure Completed, Reading out CCD...                  \n");
	  NotifyClient(&obs,(char*)"Exposure Completed, Shutter=0 (Closed), Reading out CCD...",STATUS);
	  break;
	  
	case IDLE:  // exposure done since the last time we polled
	  printf("Exposure Completed...                                   \n");
	  NotifyClient(&obs,(char*)"Exposure Completed, Shutter=0 (Closed), Readout Deferred",DONE);
          rl_refresh_line(0,0);
	  break;
	  
	} // end of state switch
	break;
	
      case PAUSE: // AzCam server has a PAUSEd exposure, do nothing (for now)
	break;
	
      case IDLE: // AzCam server is IDLE, check the CCD and dewar temperatures
	if (ccd.FD>0)
	  if (CheckTemp(&ccd,reply)==0)
	    if (strcasecmp(obs.expHost,"console")!=0)
	      NotifyClient(&obs,reply,STATUS);

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
      
      // Unexpected input from the AzCam server - just echo it to the
      // console for now.
      
      if (ccd.FD > 0) {
	if (FD_ISSET(ccd.FD, &read_fd)) {
	  nread = ReadAzCam(&ccd,camData);  // direct read, no timeout
	  if (nread > 0) {
	    printf("AzCam> %s\n",camData);
	    memset(camData,0,sizeof(camData));
	    rl_refresh_line(0,0);
	  }
	}
      } // end of AzCam server TCP port handling
      
      // Message from the DataMan agent - just echo it to the console
      // for now.
      
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
      
      // Message from the PC-TCS agent - just echo it to the console
      // for now.
      
      if (tcs.FD > 0 && (!client.useISIS)) {
	if (FD_ISSET(tcs.FD, &read_fd)) {
	  nread = ReadPCTCS(&tcs,camData);  // direct read, no timeout
	  if (nread > 0) {
	    printf("TCS> %s\n",camData);
	    memset(camData,0,sizeof(camData));
	    rl_refresh_line(0,0);
	  }
	}
      }
      
      // add any new FD handlers here...
      
    } // end of select() I/O handling checking
    
  } // bottom of the while(client.KeepGoing) loop
  
  //----------------------------------------------------------------
  //
  // If we got here, the client was instructed to shut down
  //

  printf("\nY4KCam Instrument Control Client Shutting Down...\n");

  // Tear down the AzCam server link
  
  if (ccd.FD>0) 
    CloseAzCam(&ccd);

  // Tear down the Filter Wheel link, if any

  if (fw.FD>0)
    FWClose(&fw);

  // Tear down the PC-TCS link, if any

  if (tcs.FD>0 && (!client.useISIS))
    ClosePCTCS(&tcs);

  // Tear down the DataMan link, if any

  if (dm.FD>0 && (!client.useISIS))
    CloseDataMan(&dm);

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
    AbortExposure(&ccd,reply);

  // If the Filter Wheel is active, abort any motions

  if (fw.FD>0)
    FWAbort(&fw);

  printf("Ctrl+C Abort requested - Aborts Sent\n");

}

