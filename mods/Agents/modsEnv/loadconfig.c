/*!
  \file loadconfig.c
  \brief Load/Parse ISIS client's runtime configuration file.

  ISIS-style runtime configuration files (e.g., named myclient.ini,
  .myclientrc, whatever) contain simple Keyword-Value pairs that are
  parsed into global-scope variables for the client and its various
  subroutines to use.
 
  The # is used as a comment character, making a comment line when it
  appears as the first character in a line by itself.  Inline comments
  are not supported by this simple parser, but are generally ignored
  since it assumes (again for simplicity) that value arguments are
  numbers or strings without spaces.  Fancier parsers can be implemented
  as needed.  Blank lines are ignored by the parser.  We adopt the
  convention that keywords and values are case insensitive, to remove
  any ambiguity.
  
  This template provides a good example of common client initialiation
  file parameters and syntax.  The idea is to make the runtime config
  files for all ISIS clients look pretty much the same in terms of
  having a common syntax as appearance.  The utility function GetArg()
  used here is from isisutils.c, with the prototype defined in the
  isisclient.h header.
 
  A typical runtime config file has the following structure:
  \code 
   #
   # modsenv client runtime config file
   #
   # R. Pogge, OSU Astronomy Dept.
   # pogge@astronomy.ohio-state.edu
   # 2010 June 21
   #
   ################################################################

   # modsenv ISIS client info (Host=localhost is implicit)

   ID   M1.ENV
   Port 10901

   # Application Mode: either STANDALONE or ISISclient

   #Mode Standalone
   Mode ISISclient

   # ISIS Server Info - only releveant if Mode=ISISclient

   ISISID   IS
   ISISHost 192.168.139.131
   ISISPort 6600

   # Environmental Monitor Parameters

   InstID  MODS1
   Cadence 60
   DataLog /home/dts/Logs/Env/mods1
   IUB     192.168.139.122
   IEB_R   192.168.139.100
   IEB_B   192.168.139.110

   # Runtime flags 
   USEHDF5
   NoLog

   VERBOSE
   #debug
  \endcode
 
  As this example shows, the goal is that runtime configuration files
  are easily read and created by humans.  A common syntax makes
  maintenance of many clients easier.
 
  \author R. Pogge, OSU Astronomy Dept. (pogge@astronomy.ohio-state.edu)
  \date 2010 June 21

  \par Mods Modification History:
<pre>  
  2010 Jan 21 - first cut [rwp/osu]
</pre>
 
*/

#include "client.h"   // Custom client application header file

// maximum mumber of characters/line of the file

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
  char argList[MAXCFGLINE];  // Generic argument list string
  char argStr[MAXCFGLINE];   // Generic argument token string
  char inStr[MAXCFGLINE];    // Generic input string
  char reply[256];           // Reply string

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

  strcpy(client.ID,DEFAULT_MYID);   // client default ISIS node name
  client.Port = DEFAULT_MYPORT;     // client default port number

  gethostname(client.Host,sizeof(client.Host));   // client hostname

  // Client runtime parameters

  client.doLogging = 0;            // default: runtime logging enabled 
  strcpy(client.logFile,ENV_LOGS); // default client runtime log filename

  client.isVerbose = 0;            // default: not verbose (concise)
  client.Debug = 0;                // default: no debugging

  // Reset the client global data structures to the compile-time defaults

  initEnvData(&env);

  useCLI = 1; // default: interactive shell enabled (also set in main)

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

      sscanf(inStr,"%s %[^\n]",keyword,argList);

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

      // ID: IMPv2 node name of this client 

      else if (strcasecmp(keyword,"ID")==0) {
	GetArg(inStr,2,argStr);
	strcpy(client.ID,argStr);
      }

      // Port: network UDP socket port number of this client.  Host is
      //       assumed to be localhost (since it can't be anything else)

      else if (strcasecmp(keyword,"PORT")==0) {
	GetArg(inStr, 2, argStr);
	client.Port = atoi(argStr);
      }

      // ISISID: IMPv2 Host name of the ISIS server.
      // 
      // Only meaningful if MODE ISISCLIENT has been set.
      //

      else if (strcasecmp(keyword,"ISISID")==0) {
	GetArg(inStr, 2, argStr);
	strcpy(client.isisID, argStr);
      }

      // ISISHost: Hostname of the machine running the ISIS server.
      //           May be a resolvable name or an IP address.

      else if (strcasecmp(keyword,"ISISHOST")==0) {
	GetArg(inStr,2,argStr);
	strcpy(client.isisHost,argStr);
      }
	
      // ISISPort: network UDP socket port number used by the ISIS server 
      //           running on ISISHost
							  
      else if (strcasecmp(keyword,"ISISPORT")==0) {
	GetArg(inStr, 2, argStr);
	client.isisPort = atoi(argStr);
      }

      // InstID: Formal ID of the MODS we're monitoring (e.g., MODS1)

      else if (strcasecmp(keyword,"INSTID")==0) {
	GetArg(inStr, 2, argStr);
	strcpy(env.modsID,argStr);
      }

      // UseTTY: enable/disable the interactive command shell
      //
      // Usage: UseTTY [T|F]  --> recognizes aliases T=Y and F=N
      //

      else if (strcasecmp(keyword,"USETTY")==0) {
	GetArg(inStr,2,argStr);
	if (strcasecmp(argStr,"T")==0 ||
	    strcasecmp(argStr,"Y")==0) {
	  useCLI = 1;
	}
	else if (strcasecmp(argStr,"F")==0 ||
		 strcasecmp(argStr,"N")==0) {
	  useCLI = 0;
	}
      }

      // Cadence: Instrument environmental sensor sampling cadence in seconds
      //
      // Nothing changes fast, so keep it slow

      else if (strcasecmp(keyword,"CADENCE")==0) {
	GetArg(inStr,2,argStr);
	int dt = atoi(argStr);
	if (dt <= 0) 
	  env.cadence = DEFAULT_CADENCE;
	else
	  env.cadence = (long)(dt);
      }

      // DataLog: Full path and rootname of the environmental data log
      //
      // The agent will append the UTC CCYYMMDD date tag and .log extension
      // to create the full qualified name /path/rootname.CCYYMMDD.log
      //

      else if (strcasecmp(keyword,"DATALOG")==0) {
	GetArg(inStr,2,argStr);
	strcpy(env.logRoot,argStr);
	env.doLogging = 1;
      }

      else if (strcasecmp(keyword,"HDFLOG")==0) {
	GetArg(inStr,2,argStr);
	strcpy(env.hdfRoot,argStr);
      }

      else if (strcasecmp(keyword,"LEAPSECONDSFILE")==0) {
	GetArg(inStr,2,argStr);
	strcpy(env.leapSecondsFile,argStr);
      }

      // IUB: IP address of the Instrument Utility Box WAGO FieldBus controller

      else if (strcasecmp(keyword,"IUB")==0) {
	GetArg(inStr,2,argStr);
	strcpy(env.iub_Addr,argStr);
      }

      // IEB_R: IP address of the Red Channel Instrument Electronics Box (IEB)
      //        WAGO FieldBus controller

      else if (strcasecmp(keyword,"IEB_R")==0) {
	GetArg(inStr,2,argStr);
	strcpy(env.iebR_Addr,argStr);
      }

      // IEB_B: IP address of the Blue Channel Instrument Electronics Box (IEB)
      //        WAGO FieldBus controller

      else if (strcasecmp(keyword,"IEB_B")==0) {
	GetArg(inStr,2,argStr);
	strcpy(env.iebB_Addr,argStr);
      }

      // LLB: IP address of the Lamp/Laser Box (LLB)
      //      WAGO FieldBus controller

      else if (strcasecmp(keyword,"LLB")==0) {
	GetArg(inStr,2,argStr);
	strcpy(env.llb_Addr,argStr);
      }

      // NoLog: Explicitly disable data logging

      else if (strcasecmp(keyword,"NOLOG")==0) {
	env.doLogging = 0;
	
      }
      
      // Verbose: Enable verbose output mode (e.g., for debugging)

      else if (strcasecmp(keyword,"VERBOSE")==0) {
	client.isVerbose = 1;
	
      }

      // HDF5 Output: When enabeled, HDF5 log files will be created.
      
      else if (strcasecmp(keyword,"USEHDF5")==0) {
	env.useHdf5 = 1;
      
      }

      // Debug: Enable runtime debugging out (superverbose mode)

      else if (strcasecmp(keyword,"DEBUG")==0) {
	client.Debug = 1;
	
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
