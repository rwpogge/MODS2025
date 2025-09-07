/*!
  \file loadconfig.c
  \brief Load/Parse ISIS client's runtime configuration file.

  \param cfgfile name of the configuration file to read
  \return 0 on success, -1 on errors

  \par Overview

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

  \par Runtime Config Files

  A typical runtime config file has the following structure:
  \code 
   #
   # modsCCD client runtime config file
   #
   # R. Pogge, OSU Astronomy Dept.
   # pogge.1@osu.edu
   # Updated: 2025 July 24 [rwp/osu]
   #
   # MODS1B LBTO Archon system
   #
   ################################################################

   # modsCCD's ISIS client info (Host=localhost is implicit)

   ID   M1.BC
   Port 10401

   # Application Mode: either STANDALONE or ISISclient

   #Mode Standalone
   Mode ISISclient

   # Instrument Name

   Instrument MODS1B

   # azcam server info

   AzCamHost 192.168.139.132
   AzCamPort 2402
   TimeOut 10
   
   # ISIS server info - only used if Mode=ISISclient

   ISISID   IS
   ISISHost 192.168.139.130
   ISISPort 6600

   # Runtime flags 

   VERBOSE
   #nolog
   #debug
  \endcode
 
  As this example shows, the goal is that runtime configuration files
  are easily read and created by humans.  A common syntax makes
  maintenance of many clients easier.
 
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
  char args[MAXCFGLINE];     // Generic argument buffer
  char argbuf[MAXCFGLINE];   // Generic sub-arg buffer
  char inbuf[MAXCFGLINE];    // Generic input buffer
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
  // default: STANDALONE mode rather than an ISIS client
  
  client.useISIS = 0;   
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

  // Reset the obsPars, azcam data structures.  This ensures we don't
  // have delinquent data upon startup

  initObsPars(&obs);
  initAzCam(&ccd);
  
  //initDataMan(&dm);

  // Now open the config file, if not, gripe and return -1.  Opening the
  // file here ensures that sensible defaults are set even if the config
  // file stuff was in error.

  if (!(cfgFP=fopen(cfgfile,"r"))) {
    printf("ERROR: Cannot open runtime configuration file %s\n",cfgfile);
    printf("       %s\n",strerror(errno));
    return -1;
  }

  //----------------------------------------------------------------
  //
  // Config file parser loop
  //
  // Read in each line of the config file and process it 
  //

  while(fgets(inbuf, MAXCFGLINE, cfgFP)) {

    // Skip comments (#) and blank lines

    if ((inbuf[0]!='#') && (inbuf[0]!='\n') && inbuf[0]!='\0') {
      inbuf[MAXCFGLINE] ='\0';

      sscanf(inbuf,"%s %[^\n]",keyword,args);

      //------------------------------
      // Keywords:
      //

      // Mode: the application's operating mode.  2 options:
      //       STANDALONE: no ISIS server present
      //       ISISClient: we're an ISIS client
      //

      if (strcasecmp(keyword,"MODE")==0) {
	GetArg(inbuf,2,argbuf);
	if (strcasecmp(argbuf,"STANDALONE")==0) {
	  client.useISIS = 0;
	}
	else if (strcasecmp(argbuf,"ISISCLIENT")==0) {
	  client.useISIS = 1;
	}
	else {
	  printf("ERROR: Mode option '%s' unrecognized\n",argbuf);
	  printf("       Must be STANDALONE or ISISCLIENT\n");
	  printf("Aborting - fix the config file (%s) and try again\n",
		 client.rcFile);
	  if (cfgFP !=0) fclose(cfgFP);
	  return -1;
	}
      }

      // ID: node name of this client 

      else if (strcasecmp(keyword,"ID")==0) {
	GetArg(inbuf,2,argbuf);
	strcpy(client.ID,argbuf);
      }

      // PORT: network socket port number of this client.  Host is
      //       assumed to be localhost (since it can't be anything else)

      else if (strcasecmp(keyword,"PORT")==0) {
	GetArg(inbuf, 2, argbuf);
	client.Port = atoi(argbuf);
      }

      // ISISID: Node name of the ISIS server.
      // 
      // Only meaningful if MODE ISISCLIENT has been set.
      //

      else if (strcasecmp(keyword,"ISISID")==0) {
	GetArg(inbuf, 2, argbuf);
	strcpy(client.isisID, argbuf);
      }

      // ISISHost: Hostname of the machine running the ISIS server.
      //             May be a resolvable name or an IP address.

      else if (strcasecmp(keyword,"ISISHOST")==0) {
	GetArg(inbuf,2,argbuf);
	strcpy(client.isisHost,argbuf);
      }
	
      // ISISPort: network socket port number used by the ISIS server 
      //             running on ServerHost
							  
      else if (strcasecmp(keyword,"ISISPORT")==0) {
	GetArg(inbuf, 2, argbuf);
	client.isisPort = atoi(argbuf);
      }

      // LogFile: Runtime log file rootname (including path) 
      //
      // The .log extension will be appended to this rootname. 

      else if (strcasecmp(keyword,"LOGFILE")==0) { 
	GetArg(inbuf, 2, argbuf);
	strcpy(client.logFile, argbuf);
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

      // Instrument Info: Name of the instrument 

      else if (strcasecmp(keyword,"INSTRUMENT")==0) {
	GetArg(inbuf,2,argbuf);
	strcpy(obs.instID,argbuf);
      }

      // AzCamHost: Hostname of the machine running the instrument's azcam server.
      //            May be a resolvable name or an IP address.

      else if (strcasecmp(keyword,"AzCamHost")==0) {
	GetArg(inbuf,2,argbuf);
	strcpy(ccd.Host,argbuf);
      }
	
      // AzCamPort: network socket port number of the azcam server on AzCamHost
							  
      else if (strcasecmp(keyword,"AzCamPort")==0) {
	GetArg(inbuf, 2, argbuf);
	ccd.Port = atoi(argbuf);
      }

      // Timeout: AzCam server comms timeout in seconds

      else if (strcasecmp(keyword,"Timeout")==0) {
	GetArg(inbuf,2,argbuf);
	ccd.Timeout = atol(argbuf);
      }

      // Remote client notification "keep-alive" time in seconds

      else if (strcasecmp(keyword,"keepAlive")==0) {
	GetArg(inbuf,2,argbuf);
	obs.keepAlive = atof(argbuf);
      }
      
      // Gripe if junk is in the config file

      else { 
	printf("Ignoring unrecognized config file entry - %s", inbuf);

      }
    }

    memset(inbuf,0,sizeof(inbuf)); 

  }

  /* all done, close the config file and return */

  if (cfgFP!=0)
    fclose(cfgFP);

  return 0;

}

//---------------------------------------------------------------------------

/*!
  \brief Save the runtime aconfiguration
  
  \param cfgfile name of the configuration file to save
  \return 0 on success, -1 on failure

  Save a subset of runtime parameters that allow restarting the
  modsCCD agent from a known state. These are stored in
  a .modsCCD file in the user's home directory

  Parameters stored are in the obsPars struct
    obsPars.Observer
    obsPars.Partner
    obsPars.PropID
    obsPars.PIName
    obsPars.Support
    obsPars.TelOps
    ...

  \sa loadConfig()
*/

int 
saveConfig(char *cfgfile)
{
  char keyword[MAXCFGLINE];  // File is organized into KEYWORD VALUE pairs
  char args[MAXCFGLINE];     // Generic argument buffer
  char argbuf[MAXCFGLINE];   // Generic sub-arg buffer
  char inbuf[MAXCFGLINE];    // Generic input buffer
  char reply[256];           // reply buffer

  FILE *cfgFP;               // Configuration file pointer
  int i;
  char c;                    

  // Now open the config file, if not, gripe and return -1.  Note that
  // this will OVERWRITE any existing file.  It is up to the calling
  // routine to test for overwrite if it matters.

  if (!(cfgFP=fopen(cfgfile,"w+"))) {
    printf("ERROR: Cannot open runtime configuration file %s\n",cfgfile);
    printf("       %s\n",strerror(errno));
    return(-1);
  }

  // Start with the header, time tagged

  fprintf(cfgFP,"#\n");
  fprintf(cfgFP,"# modsCCD agent runtime configuration file\n");
  fprintf(cfgFP,"#\n");
  fprintf(cfgFP,"# Generated by saveconfig on %s [%s]\n",
	  UTCDate(),UTCTime());
  fprintf(cfgFP,"# by modsCCD running on host %s\n",client.Host);
  fprintf(cfgFP,"#\n");

  // Client information

  fprintf(cfgFP,"\n# modsCCD ISIS client info\n\n");
  fprintf(cfgFP,"ID %s\n",client.ID);
  fprintf(cfgFP,"Port %d\n",client.Port);
  if (client.useISIS) 
    fprintf(cfgFP,"MODE isisClient\n");
  else 
    fprintf(cfgFP,"MODE STANDALONE\n");

  // ISIS Server Info

  fprintf(cfgFP,"\n# ISIS server info - relevant if Mode=isisClient\n\n");
  fprintf(cfgFP,"isisID %s\n",client.isisID);
  fprintf(cfgFP,"isisHost %s\n",client.isisHost);
  fprintf(cfgFP,"isisPort %d\n",client.isisPort);

  // Instrument Info

  fprintf(cfgFP,"\n# Instrument Info\n\n");
  fprintf(cfgFP,"Instrument %s\n",obs.instID);

  // azcam config file

  fprintf(cfgFP,"\n# azcam server info\n\n");
  fprintf(cfgFP,"azcamHost %s\n",ccd.Host);
  fprintf(cfgFP,"azcamPort %s\n",ccd.Port);
  fprintf(cfgFP,"Timeout %ld\n",ccd.Timeout);

  // Data Manager info if enabled

  /*
  if (dm.useDM) {
    fprintf(cfgFP,"\n# Data Manager agent config file\n\n");
    fprintf(cfgFP,"\DATAMAN %s\n",dm.cfgFile);
  }
  */
  
  // Session Restart Information

  fprintf(cfgFP,"\n# Observing Session Restart Information\n\n");
  fprintf(cfgFP,"Observer %s\n",obs.Observer);
  fprintf(cfgFP,"Partner %s\n",obs.Partner);
  fprintf(cfgFP,"PropID  %s\n",obs.PropID);
  fprintf(cfgFP,"PI_Name %s\n",obs.PIName);
  fprintf(cfgFP,"Support %s\n",obs.Support);
  fprintf(cfgFP,"TelOps %s\n",obs.TelOps);
  fprintf(cfgFP,"FileName %s\n",ccd.fileName);
  fprintf(cfgFP,"FileNum %d\n",ccd.fileNum);
  fprintf(cfgFP,"ImgType %s\n",obs.imgType);
  fprintf(cfgFP,"ImgTitle %s\n",obs.imgTitle);
  fprintf(cfgFP,"ExpTime %.2f\n",ccd.expTime);
  
  // Client runtime flags

  fprintf(cfgFP,"\n# Client runtime flags\n\n");

  if (client.isVerbose)
    fprintf(cfgFP,"VERBOSE\n");
  else
    fprintf(cfgFP,"#VERBOSE\n");

  if (client.Debug)
    fprintf(cfgFP,"DEBUG\n");
  else
    fprintf(cfgFP,"#DEBUG\n");

  if (client.doLogging) 
    fprintf(cfgFP,"LOGFULE %s\n",client.logFile);
  else
    fprintf(cfgFP,"NOLOG\n");

  // All done, close it

  fprintf(cfgFP,"\n");
  fclose(cfgFP);
  
  return 0;

}
