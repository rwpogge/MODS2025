/*!
  \file loadconfig.c
  \brief Load/Parse ISIS client's runtime configuration file.

  ISIS-style runtime configuration files (e.g., named myclient.ini,
  .myclientrc, whatever) contain simple Keyword-Value pairs that are
  parsed into global-scope variables for the client and its various
  subroutines to use.
 
  We use # as a comment character, making a comment line when it
  appears as the first character in a line by itself.  Inline comments
  are not supported by this simple parser, but are generally ignored
  since it assumes (again for simplicity) that value arguments are
  numbers or strings without spaces.  Fancier parsers can be implemented
  as needed.  Blank lines are ignored by the parser.  We adopt the
  convention that keywords and values are case insensitive, to remove
  any ambiguity.
  
 
  \author R. Pogge, OSU Astronomy Dept. (pogge.1@osu.edu)
  \date 2003 September 14 (original version based on the ISIS server ParseIniFile())

  \par Mods Modification History:
<pre>  
2005 Jul 28 - new application
2007 Jul 02 - added opMode parameter (LIVE or SIM)
2009 Dec 19 - start of rewrite to handle new ICE hooks following 481s013c
2010 Jan 20 - near-final command set for the ICE version of the IIF
2010 March  - remote testing with a live LBT
2010 Dec - modifications on-site, live LBT+MOD1 [rwp/osu]
2014 Dec - LBT binocular operation updates [rwp/osu]
2025 Aug - MODS Archon CCD controller updates [rwp/osu]
</pre>
 
*/

#include "client.h"   // Custom client application header file

/*!
  \brief Load/Parse ISIS client's runtime configuration file.
  \param cfgfile Path/name of the client runtime configuration file
  \return 0 if success, <0 if failure.  All error message are printed 
  to the client's console.

  The precise actions of LoadConfig() are tailored to the client application.
*/

int 
loadConfig(char *cfgfile)
{
  char keyword[MAXCFGLINE];  // File is organized into KEYWORD VALUE pairs
  char args[MAXCFGLINE];     // Generic argument buffer
  char argStr[MAXCFGLINE];   // Generic sub-arg buffer
  char inStr[MAXCFGLINE];    // Generic input buffer
  char reply[256];           // reply buffer

  FILE *cfgFP;               // Configuration file pointer
  int i;
  char c;                    

  // If we need to initialize any default parameter values, do it here.
  // Note that as-written these variables have been defined in global scope
  // for the entire client application, e.g., in main.c for the
  // application.

  // Record the runtime config file in use.

  strcpy(client.rcFile,cfgfile);

  // ISIS server information (Defaults defined in the client.h header):

  client.useISIS = 0;  // default: STANDALONE mode rather than an ISIS client
  strcpy(client.isisHost,DEFAULT_ISISHOST); 
  client.isisPort = DEFAULT_ISISPORT;       
  strcpy(client.isisID,DEFAULT_ISISID);     

  // Client information (defaults in client.h):

  strcpy(client.ID,DEFAULT_MYID);     // client default ISIS node name
  client.Port = DEFAULT_MYPORT;       // client default port number

  gethostname(client.Host,sizeof(client.Host));   // client hostname

  // Client runtime parameters

  client.doLogging = 0;                   // default: runtime logging enabled 
  strcpy(client.logFile,DEFAULT_LOGFILE); // default client runtime log filename

  client.isVerbose = 0;                   // default: not verbose (concise)
  client.Debug = 0;                       // default: no debugging

  // Reset the client global data structures

  initLBTInfo(&lbt);

  useCLI = 1; // default: interactive shell enabled, but config can override

  // Now open the config file, if not, gripe and return -1.  Opening the
  // file here ensures that sensible defaults are set even if the config
  // file stuff was in error.

  if (!(cfgFP=fopen(cfgfile,"r"))) {
    printf("ERROR: Cannot open runtime configuration file %s\n",cfgfile);
    printf("       %s\n",strerror(errno));
    return(-1);
  }

  //----------------------------------------------------------------
  //
  // Config file parser loop
  //
  // Read in each line of the config file and process it 
  //

  while(fgets(inStr, MAXCFGLINE, cfgFP)) {

    // Skip comments (#) and blank lines

    if ((inStr[0]!='#') && (inStr[0]!='\n') && inStr[0]!='\0') {
      inStr[MAXCFGLINE] ='\0';

      sscanf(inStr,"%s %[^\n]",keyword,args);

      //------------------------------
      // Keywords:
      //

      // Mode: the application's operating mode.  2 options:
      //       STANDALONE: no ISIS server present
      //       ISISClient: we're an ISIS client
      //

      if (strcasecmp(keyword,"MODE")==0) {
	GetArg(inStr,2,argStr);
	if (strcasecmp(argStr,"STANDALONE")==0) {
	  client.useISIS = 0;
	}
	else if (strcasecmp(argStr,"ISISCLIENT")==0) {
	  client.useISIS = 1;
	}
	else {
	  printf("ERROR: Mode option '%s' unrecognized\n",argStr);
	  printf("       Must be STANDALONE or ISISCLIENT\n");
	  printf("Aborting - fix the config file (%s) and try again\n",
		 client.rcFile);
	  if (cfgFP !=0) fclose(cfgFP);
	  return -1;
	}
      }

      // ID: node name of this client 

      else if (strcasecmp(keyword,"ID")==0) {
	GetArg(inStr,2,argStr);
	strcpy(client.ID,argStr);
      }

      // PORT: network socket port number of this client.  Host is
      //       assumed to be localhost (since it can't be anything else)

      else if (strcasecmp(keyword,"PORT")==0) {
	GetArg(inStr, 2, argStr);
	client.Port = atoi(argStr);
      }

      // ISISID: Node name of the ISIS server.
      // 
      // Only meaningful if MODE ISISCLIENT has been set.
      //

      else if (strcasecmp(keyword,"ISISID")==0) {
	GetArg(inStr, 2, argStr);
	strcpy(client.isisID, argStr);
      }

      // ISISHost: Hostname of the machine running the ISIS server.
      //             May be a resolvable name or an IP address.

      else if (strcasecmp(keyword,"ISISHOST")==0) {
	GetArg(inStr,2,argStr);
	strcpy(client.isisHost,argStr);
      }
	
      // ISISPort: network socket port number used by the ISIS server 
      //             running on ServerHost
							  
      else if (strcasecmp(keyword,"ISISPORT")==0) {
	GetArg(inStr, 2, argStr);
	client.isisPort = atoi(argStr);
      }

       // UseTTY: enable/disable the interactive command shell
      //
      // Usage: UseTTY [T|F]  --> recognizes aliases T=Y and F=N
      //

      else if (strcasecmp(keyword,"USETTY")==0) {
        GetArg(inStr,2,argStr);
        if (strcasecmp(argStr,"T")==0 || strcasecmp(argStr,"Y")==0) {
          useCLI = 1;
        }
        else if (strcasecmp(argStr,"F")==0 || strcasecmp(argStr,"N")==0) {
          useCLI = 0;
        }
      }

      // Telescope: formal telescope ID for FITS headers (e.g., LBT-SX)

      else if (strcasecmp(keyword,"TELESCOPE")==0) {
	GetArg(inStr, 2, argStr);
	strcpy(lbt.telescope,argStr);
      }

      // Properties: root name of the ICE properties file

      else if (strcasecmp(keyword,"PROPFILE")==0) {
	GetArg(inStr,2,argStr);
	strcpy(lbt.iceProps,argStr);
      }

      // PROXY: IIF ICE Proxy Name

      else if (strcasecmp(keyword,"PROXY")==0) {
	GetArg(inStr,2,argStr);
	strcpy(lbt.proxy,argStr);
      }

      // InstID: IIF Instrument ID

      else if (strcasecmp(keyword,"INSTID")==0) {
	GetArg(inStr,2,argStr);
	strcpy(lbt.instID,argStr);
      }

      // FocStation: Telescope focal station ID

      else if (strcasecmp(keyword,"FOCSTATION")==0) {
	GetArg(inStr,2,argStr);
	strcpy(lbt.focStation,argStr);
      }

      // Side: Telescope Side (left, right, or both)

      else if (strcasecmp(keyword,"SIDE")==0) {
	GetArg(inStr,2,argStr);
	strcpy(lbt.side,argStr);
      }

      // LogFile: Runtime log file rootname (including path) 
      //
      // The .log extension will be appended to this rootname. 

      else if (strcasecmp(keyword,"LOGFILE")==0) { 
	GetArg(inStr, 2, argStr);
	strcpy(client.logFile, argStr);
	client.doLogging = 1;
      }

      // NOLOG: Explicitly disable the runtime logging

      else if (strcasecmp(keyword,"NOLOG")==0) {
	client.doLogging = 0;
	
      }
      
      // Verbose: Enable verbose output mode (e.g., for debugging)

      else if (strcasecmp(keyword,"VERBOSE")==0) {
	client.isVerbose = 1;
	
      }

      // Debug: Enable runtime debugging out (superverbose mode)

      else if (strcasecmp(keyword,"DEBUG")==0) {
	client.Debug = 1;
	
      }

      // opMode: operations mode (LIVE or SIM)

      else if (strcasecmp(keyword,"OPMODE")==0) {
	GetArg(inStr, 2, argStr);
	if (strcasecmp(argStr,"LIVE")==0 ) {
	  lbt.opMode = LIVE;
	}
	else if (strcasecmp(argStr,"SIM")==0) {
	  lbt.opMode = LABSIM;
	}
	else {
	  printf("Warning, unknown opMode=%s... assuming LIVE mode\n",argStr);
	  lbt.opMode = LIVE;
	}
      }

      // Gripe if junk is in the config file

      else { 
	printf("Ignoring unrecognized config file entry - %s", inStr);

      }
    }

    memset(inStr,0,sizeof(inStr)); 

  }

  /* all done, close the config file and return */

  if (cfgFP!=0)
    fclose(cfgFP);

  return(0);

}
