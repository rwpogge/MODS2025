//**************************************************************************
//
// Loadconfig() - load an ISIS client's runtime configuration file
//

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
       # myclient runtime configuration file
       #
       # Modified 2003 June 21 by R. Pogge, OSU Astronomy Dept.
       # for the fauxMODS spectrograph system echoclient.
       #
       ID  echo
       Port 7890
       Verbose
       #
       ServerID   isis1
       ServerHost mods1.lbto.org
       ServerPort 6789
       #
       Filter 1 Blue
       Filter 2 SDSS i
       Filter 3 SDSS g
       Filter 4 Clear
       # 
       XCenter   92000
       YCenter  202000
       Focus0    42000
       WFSXOffset    0
       WFSYOffset 1234
       #
  \endcode
 
  Here we are telling myclient that it is a client of an ISIS server
  named "isis1" running on host mods1.lbto.org and listening to network
  socket port 6789.  We are to be an ISIS client node named "echo"
  listing to network socket port 7890 on the localhost.  All runtime
  logging will be recorded to a file named /data/Logs/myclient.log, and
  both logging and screen output will be Verbose for debugging purposes.
 
  As this example shows, the goal is that runtime configuration files
  are easily read and created by humans.  A common syntax makes
  maintenance of many clients easier.
 
  This version is custom for the agwsim application. 
 
  \author R. Pogge, OSU Astronomy Dept. (pogge@astronomy.ohio-state.edu)
  \date 2003 September 14 (original version based on the ISIS server ParseIniFile())

  \par Mods Modification History:
<pre>  
   2006 Feb 07 - implemented filter ID table [rwp/osu]
   2008 Jan 24 - various changes for the actual AGW stage system [rwp/osu]
   2008 Aug 19 - overhauled to remove sim mode and other changes [rwp/osu]
</pre>
 
*/

#if SIMULATION

#include "instrutils.h"  // ISL Instrument header
#include "isl_types.h"   // ISL types of header files
#include "isl_funcs.h"   // ISL isl function files
#include "params.h"      // ISL parameters
#include "islcommon.h"   // ISL SHRMEM structure
#include "isl_shmaddr.h" // ISL SHRMEM pointer

#endif

// custom header for AGW in ../include

#include "../include/client.h"   // Custom client application header file

// maximum mumber of characters/line of the file

#define MAXCFGLINE 80 //!< Maximum mumber of characters/line in the runtime config file

/*!
  \brief Load/Parse ISIS client's runtime configuration file.
  \param agw pointer to the agwpar_t for this AGW instance
  \param client pointer to the isisclient_t for this app instance
  \param cfgfile Path/name of the client runtime configuration file
  \return 0 if success, <0 if failure.  All error message are printed 
  to the client's console.

  The precise actions of LoadConfig() are tailored to the client application.
*/

int 
LoadConfig(agwpar_t *agw, isisclient_t *client, char *cfgfile)
{
  char keyword[MAXCFGLINE];  // File is organized into KEYWORD VALUE pairs
  char argbuf[MAXCFGLINE];   // Generic argument buffer
  char inbuf[MAXCFGLINE];    // Generic input buffer

  FILE *cfgFP;               // Configuration file pointer
  int i;
  char c;                    
  int nfilt;
  int nconv;
  int mods_id;
  double foc0;
  double xcen;
  double ycen;

  // If we need to initialize any default parameter values, do it here.
  // Note that as-written these variables have been defined in global scope
  // for the entire client application, e.g., in main.c for the
  // application.

  // Record the runtime config file in use.

  strcpy(client->rcFile,cfgfile);

  // ISIS server information (Defaults defined in the client->h header):

  client->useISIS = 0; // default: STANDALONE mode rather than an ISIS client
  strcpy(client->isisHost,DEFAULT_ISISHOST); 
  client->isisPort = DEFAULT_ISISPORT;       
  strcpy(client->isisID,DEFAULT_ISISID);     

  // Client information (defaults in client->h):

  strcpy(client->ID,DEFAULT_MYID);     // client default ISIS node name
  client->Port = DEFAULT_MYPORT;       // client default port number

  gethostname(client->Host,sizeof(client->Host));   // client hostname

  // Client runtime parameters

  client->isVerbose = 0;                   // default: not verbose (concise)
  client->Debug = 0;                       // default: no debugging

  // Defaults for the MODS AGW instance

  agwInitConfig(agw);

  // Now open the config file, if not, gripe and return -1.  Opening the
  // file here ensures that sensible defaults are set even if the config
  // file stuff was in error.

  if (!(cfgFP=fopen(cfgfile, "r"))) {
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

  while(fgets(inbuf, MAXCFGLINE, cfgFP)) {

    // Skip comments (#) and blank lines

    if ((inbuf[0]!='#') && (inbuf[0]!='\n') && inbuf[0]!='\0') {
      inbuf[MAXCFGLINE] ='\0';
      GetArg(inbuf, 1, argbuf);
      strcpy(keyword, argbuf);
      
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
	  client->useISIS = 0;
	}
	else if (strcasecmp(argbuf,"ISISCLIENT")==0) {
	  client->useISIS = 1;
	}
	else {
	  printf("ERROR: Mode option '%s' unrecognized\n",argbuf);
	  printf("       Must be STANDALONE or ISISCLIENT\n");
	  printf("Aborting - fix the config file (%s) and try again\n",
		 client->rcFile);
	  return -1;
	}
      }

      // ID: node name of this client 

      else if (strcasecmp(keyword,"ID")==0) {
	GetArg(inbuf,2,argbuf);
	strcpy(client->ID,argbuf);
      }

      // PORT: network socket port number of this client->  Host is
      //       assumed to be localhost (since it can't be anything else)

      else if (strcasecmp(keyword, "PORT")==0) {
	GetArg(inbuf, 2, argbuf);
	client->Port = atoi(argbuf);
      }

      // ISISID: Node name of the ISIS server.
      // 
      // Only meaningful if MODE ISISCLIENT has been set.
      //

      else if (strcasecmp(keyword, "ISISID")==0) {
	GetArg(inbuf, 2, argbuf);
	strcpy(client->isisID, argbuf);
      }

      // ISISHost: Hostname of the machine running the ISIS server.
      //             May be a resolvable name or an IP address.

      else if (strcasecmp(keyword,"ISISHOST")==0) {
	GetArg(inbuf,2,argbuf);
	strcpy(client->isisHost,argbuf);
      }
	
      // ISISPort: network socket port number used by the ISIS server 
      //             running on ServerHost
							  
      else if (strcasecmp(keyword, "ISISPORT")==0) {
	GetArg(inbuf, 2, argbuf);
	client->isisPort = atoi(argbuf);
      }

      // Verbose: Enable verbose output mode (e.g., for debugging)

      else if (strcasecmp(keyword, "VERBOSE")==0) {
	client->isVerbose = 1;
	
      }

      // Debug: Enable runtime debugging out (superverbose mode)

      else if (strcasecmp(keyword, "DEBUG")==0) {
	client->Debug = 1;
	
      }

      else if (strcasecmp(keyword, "LATITUDE")==0) {
        GetArg(inbuf, 2, argbuf);
        agw->LATITUDE = (double)(atof(argbuf));
      }

      else if (strcasecmp(keyword, "LONGITUDE")==0) {
        GetArg(inbuf, 2, argbuf);
        agw->LONGITUDE = (double)(atof(argbuf));
      }

      //---------------------------------------------------------------
      //
      // AGW Stage runtime parameters go here
      //

      // Filter: entry for the filter ID table
      // Syntax:  FILTER n ID Code

      else if (strcasecmp(keyword,"FILTER")==0) {
	nconv = sscanf(inbuf,"%s %d %[^\n]",keyword,&nfilt,argbuf);
	if (nconv != 3) {
	  printf("ERROR: '%s' invalid, usage: FILTER n FilterID String\n",inbuf);
	  printf("Aborting - fix the config file (%s) and try again\n",
		 client->rcFile);
	  return -1;
	}

	if (nfilt < 1 || nfilt > AGW_NFILTERS) {
	  printf("WARNING: '%s' invalid filter number, must be 1..%d\n",
		 argbuf,AGW_NFILTERS);
	  printf("          Keyword ignored (please fix before running again\n");
	}
	else {
	  strncpy((agw->filterID)[nfilt-1],argbuf,AGW_FILTSIZE);
	}
      }

      // Focus0 - focus zero point

      else if (strcasecmp(keyword, "FOCUS0")==0) {
	GetArg(inbuf, 2, argbuf);
	foc0 = atof(argbuf);
	if (foc0 < AGW_FOCMIN || foc0 > AGW_FOCMAX) {
	  printf("ERROR: Focus0 value '%s' invalid, most be %d..%df\n",
		 argbuf,AGW_FOCMIN,AGW_FOCMAX);
	  printf("       Keyword ignored (please fix before running again\n");
	}
	else {
	  agw->foc0 = foc0;
	}
      }

      // XCenter - Nominal field center X-axis position in AGW stage units

      else if (strcasecmp(keyword, "XCENTER")==0) {
	GetArg(inbuf, 2, argbuf);
	xcen = atof(argbuf);
	if (xcen < AGW_XSMIN || xcen > AGW_XSMAX) {
	  printf("ERROR: XCenter value '%s' invalid, most be %d..%d\n",
		 argbuf,AGW_XSMIN,AGW_XSMAX);
	  printf("       Keyword ignored (please fix before running again\n");
	}
	else {
	  agw->xCen = xcen;
	}
      }

      // YCenter - Nominal field center Y-axis position in AGW stage units

      else if (strcasecmp(keyword, "YCENTER")==0) {
	GetArg(inbuf, 2, argbuf);
	ycen = atof(argbuf);
	if (ycen < AGW_YSMIN || ycen > AGW_YSMAX) {
	  printf("ERROR: YCenter value '%s' invalid, most be %d..%d\n",
		 argbuf,AGW_YSMIN,AGW_YSMAX);
	  printf("       Keyword ignored (please fix before running again\n");
	}
	else {
	  agw->yCen = ycen;
	}
      }

      // XWFSOffset - X-axis offset of the WFS Camera in microns

      else if (strcasecmp(keyword, "WFSXOffset")==0) {
	GetArg(inbuf, 2, argbuf);
	agw->dxWFS = atof(argbuf);
      }

      // YWFSOffset - Y-axis offset of the WFS Camera in microns

      else if (strcasecmp(keyword, "WFSYOffset")==0) {
	GetArg(inbuf, 2, argbuf);
	agw->dyWFS = atof(argbuf);
      }

      // AGWtoSFP - transformation coefficients

      else if (strcasecmp(keyword, "AGWToSFP")==0) {
	GetArg(inbuf, 2, argbuf);
	mods_id=atoi(argbuf)-1;
	GetArg(inbuf, 3, argbuf);
	agw->gp_x0[mods_id] = atof(argbuf);
	GetArg(inbuf, 4, argbuf);
	agw->gp_xx[mods_id] = atof(argbuf);
	GetArg(inbuf, 5, argbuf);
	agw->gp_xy[mods_id] = atof(argbuf);
	GetArg(inbuf, 6, argbuf);
	agw->gp_y0[mods_id] = atof(argbuf);
	GetArg(inbuf, 7, argbuf);
	agw->gp_yx[mods_id] = atof(argbuf);
	GetArg(inbuf, 8, argbuf);
	agw->gp_yy[mods_id] = atof(argbuf);
      }

      // SFPtoAGW - transformation coefficients

      else if (strcasecmp(keyword, "SFPToAGW")==0) {
	GetArg(inbuf, 2, argbuf);
	mods_id=atoi(argbuf)-1;
	GetArg(inbuf, 3, argbuf);
	agw->sfp_x0[mods_id] = atof(argbuf);
	GetArg(inbuf, 4, argbuf);
	agw->sfp_xx[mods_id] = atof(argbuf);
	GetArg(inbuf, 5, argbuf);
	agw->sfp_xy[mods_id] = atof(argbuf);
	GetArg(inbuf, 6, argbuf);
	agw->sfp_y0[mods_id] = atof(argbuf);
	GetArg(inbuf, 7, argbuf);
	agw->sfp_yx[mods_id] = atof(argbuf);
	GetArg(inbuf, 8, argbuf);
	agw->sfp_yy[mods_id] = atof(argbuf);
      }

      // CCDtoAGW - transformation coefficients

      else if (strcasecmp(keyword, "CCDToAGW")==0) {
	GetArg(inbuf, 2, argbuf);
	mods_id=atoi(argbuf)-1;
	GetArg(inbuf, 3, argbuf);
	agw->ccd_xx[mods_id] = atof(argbuf);
	GetArg(inbuf, 4, argbuf);
	agw->ccd_xy[mods_id] = atof(argbuf);
	GetArg(inbuf, 5, argbuf);
	agw->ccd_yx[mods_id] = atof(argbuf);
	GetArg(inbuf, 6, argbuf);
	agw->ccd_yy[mods_id] = atof(argbuf);
      }


      //----------------------------------------------------------------
      //
      // Finally, gripe if there is junk in the config file
      //

      else { 
	printf("AGW Ignoring unrecognized config file entry - %s", inbuf);

      }
    }
    memset(inbuf,0,sizeof(inbuf)); 
  }

  /* all done, close the config file and return */
  if (cfgFP!=0)
    fclose(cfgFP);

  return(0);

}
