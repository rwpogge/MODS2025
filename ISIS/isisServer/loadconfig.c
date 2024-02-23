//
// loadConfig() - parse the .ini to set up initial runtime configuration 
//                  structures
//
// Author:
//   R. Pogge, OSU Astronomy Dept.
//   pogge@astronomy.ohio-state.edu
//   2003 January 5
//
// Modification History:
//   2009 Mar 18 - updates for v2 [rwp/osu]
//   2010 Jun 21 - updates stemming from LBT/MODS deployment [rwp/osu]

/*!
  \file loadconfig.c
  \brief Load/parse the server runtime configuration file.

  ... info about server runtime config files...

*/

#include "isisserver.h"

#define CFG_BUFSIZE  80   //!< Max size of lines in the runtime config file

/*!
  \brief Load/parse the server runtime configuration file.
  \param cfgfile path/name of the server runtime configuration file.
  \return 0 if successful, <0 on failure

  Reads and parses the contents of the server's runtime configuration
  file, loading the parameters into the relevant internal variables.

  The parameters in the runtime config file take the form of keyword/value
  pairs.  Comments are denoted by a # and skipped by the parser.  Blank
  lines are also ignored.  

  Unrecognized configuration parameters are ignored except to print a
  warning message for debugging purposes.

*/

int 
loadConfig(char *cfgFile)
{
  char keyStr[MED_STR_SIZE]; // Keyword string
  char valStr[MED_STR_SIZE]; // Value string
  char argStr[MED_STR_SIZE]; // Generic argument string
  char inStr[CFG_BUFSIZE];   // Generic input string                    

  FILE *iniFP;                // file pointer                

  // Try to open the config file, if not, gripe and return -1 

  if (!(iniFP=fopen(cfgFile, "r"))) {
    printf("ERROR: Cannot open ISIS server runtime config file %s\n",cfgFile);
    printf("       %s\n",strerror(errno));
    return(-1);
  }

  // If we need to initialize any flags, do it here 

  isis.sockFD   = 0;                   // no socket FD yet               
  isis.sockPort = DEFAULT_ISISPORT;    // default server port number     
  strcpy(isis.serverID,DEFAULT_LOCALHOST); // default server hostname 
  sprintf(isis.sockAddr,"none");       // no socket IP address yet       

  isis.isVerbose   = isis_FALSE;       // default: not verbose (concise) 
  isis.doLogging   = isis_TRUE;        // default: runtime log enabled   
  isis.doHandShaking = isis_TRUE;      // default: handshake at startup  
  
  sprintf(isis.instID,"None");         // default: no instrument defined yet

  strcpy(isis.logRoot,ISIS_LOGS);    // default runtime log path and rootname 

  isis.logDate = OBSDAY; // default is observing day date for log file names

  // clear the serial port and present UDP port counters

  isis.numSerial = 0;  
  isis.numPreset = 0;

  //****************************************************************
  // 
  // Configuration File Parser Loop
  //
  // Read in each line of the config file and process it 

  while(fgets(inStr, CFG_BUFSIZE, iniFP)) {

    // Skip comments (#) and blank lines 

    if ((inStr[0]!='#') && (inStr[0]!='\n') && inStr[0]!=NUL) {

      sscanf(inStr,"%s %[^\n]",keyStr,valStr);

      //
      // HostID of the Server
      //
      // usage: ServerID %s
	 
      if (strcasecmp(keyStr, "SERVERID")==0) {
	getArg(valStr, 1, argStr);
	strcpy(isis.serverID, argStr);
      }

      // Socket Port for the ISIS Server 
      //
      // usage: ServerPort %d
      
      else if (strcasecmp(keyStr, "SERVERPORT")==0) {
	getArg(valStr, 1, argStr);
	isis.sockPort = atoi(argStr);
	
      }

      // runtime log file rootname (including path) 
      //
      // usage: ServerLog %s
      //
      // The date tag CCYYMMDD format and .log extension will be
      // appended to this rootname and path.  The default date tag is
      // noon-to-noon local time, but this can be set to UTC by using
      // the LOGDATE keyword below.  This parameter overrides the
      // compile-time default defined in isisserver.h
	 
      else if (strcasecmp(keyStr, "SERVERLOG")==0) { 
	getArg(valStr, 1, argStr);
	strcpy(isis.logRoot, argStr);
	isis.doLogging = isis_TRUE;
      }

      // Set the date convention for log filenames
      // 
      // usage: LogDate UTC  - use the UTC date
      //        LogDate OBS  - use the noon-to-noon local date
      //

      else if (strcasecmp(keyStr, "LOGDATE")==0) {
	getArg(valStr, 1, argStr);
	if (strcasecmp(argStr,"UTC")==0)
	  isis.logDate = UTCDATE;
	else
	  isis.logDate = OBSDAY;  // default
      }

      // disable the runtime log (not advised), but...
      //
      // usage: NOLOG
	 
      else if (strcasecmp(keyStr, "NOLOG")==0) {
	isis.doLogging = isis_FALSE;
	
      }

      // Enable verbose mode 
      //
      // usage: VERBOSE
      
      else if (strcasecmp(keyStr, "VERBOSE")==0) {
	isis.isVerbose = isis_TRUE;
	
      }
      
      // Disable serial port handshaking on start/restart
      //
      // usage: NOHANDSHAKING
      
      else if (strcasecmp(keyStr, "NOHANDSHAKING")==0) {
	isis.doHandShaking = isis_FALSE;
	
      }
      
      // TTY (serial) Port Assignments
      //
      // usage: TTYPORT %s
      //
      // up to MAXSERIAL allowed - warns if over the limit
      
      else if (strcasecmp(keyStr, "TTYPORT")==0) {
	if (isis.numSerial == MAXSERIAL) {
	  printf("ERROR: Cannot define more than %d serial (tty) ports\n",
		 MAXSERIAL);
	  printf("       extra port ignored.\n");
	} 
	else {
	  getArg(valStr, 1, argStr);		      
	  sprintf(ttyTab[isis.numSerial].devName, argStr);
	  isis.numSerial++;  // increment serial port counter 
	}
      }
      
      // Preset UDP Socket Port Assignments
      //
      // usage: UDPPORT %s
      //
      // up to MAXPRESET allowed - warns if over the limit
      
      else if (strcasecmp(keyStr, "UDPPORT")==0) {
	if (isis.numPreset == MAXPRESET) {
	  printf("ERROR: Cannot define more than %d preset UDP socket ports\n",
		 MAXPRESET);
	  printf("       extra port ignored.\n");
	} 
	else {
	  getArg(valStr, 1, argStr);		      
	  sprintf(udpTab[isis.numPreset].host, argStr);
	  getArg(valStr, 2, argStr);
	  udpTab[isis.numPreset].port = atoi(argStr);
	  isis.numPreset++;  // increment preset UDP port counter 
	}
      }
      
      // Instrument information
      // 
      // usage: Instrument %s
      //        Inst %s
      
      else if (strcasecmp(keyStr,"INSTRUMENT")==0 ||
	       strcasecmp(keyStr,"INST")==0) {
	getArg(argStr,1,argStr);
	if (strlen(argStr) == 0) {
	  printf("WARNING: %s argument must be non-blank - InstID not set\n",keyStr);
	} 
	else {
	  strcpy(isis.instID,argStr);
	}
      }
      
      // Unrecognized entry handler 	
      
      else { 
	printf("Ignoring unrecognized config file entry - %s", inStr);
	
      }
      memset(inStr,0,sizeof(inStr));
    }
  }

  // all done, close the ini file and return 

  if (iniFP!=0)
    fclose(iniFP);

  return(0);

}
