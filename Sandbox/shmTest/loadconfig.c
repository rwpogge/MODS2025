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
       # Modified 2003 June 21 by Barry Boron (boron.1@osu.edu)
       # for the fauxMODS spectrograph system echoclient.
       #
       ID   echo
       Port 7890
       LogFile    /data/Logs/myclient
       Verbose
       #
       ServerID   isis1
       ServerHost mods1.lbto.org
       ServerPort 6789
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
 
  This version is custom for the MODS IE server application. 
 
  \author Ray Gonzalez (rdg@astronomy.ohio-state.edu)
  \date 2005 May 05 (original version based on the ISIS server ParseIniFile())

  \par Mods Modification History:
<pre>  
2005 May 05 - modified for the mods IE app wiht the mechanisms.ini file [rdg/osu]
2025 Jun 25 - MODS2025 controller upgrade port, see notes [rwp/osu]
</pre>
 
*/
// Custom client application header file

#include "isisclient.h"
#include "mmc_client.h"
#include "instrutils.h"
#include "isl_types.h"
#include "params.h" // Common parameters and defines (inludes path to the config file)
#include "islcommon.h"   // Shared memory common data structure
#include "isl_shmaddr.h" // Shared memory attachment.

// maximum mumber of characters/line of the file

#define MAXCFGLINE 80 //!< Maximum mumber of characters/line in the runtime config file

/*!
  \brief Load/Parse ISIS client's runtime configuration file.
  \param cfgfile Path/name of the client runtime configuration file
  \return 0 if success, <0 if failure.  All error message are printed 
  to the client's console.

  The precise actions of LoadConfig() are tailored to the client application.

  Changes:
    2025 June - added QC_PORT keyword to replace QCIP_PORT for WAGO-based
                quadcell readout [rwp/osu]
*/

int 
LoadConfig(char *cfgfile)
{
  char keyword[MAXCFGLINE]; // File is organized into KEYWORD VALUE pairs
  char argbuf[MAXCFGLINE];  // Generic argument buffer
  char inbuf[MAXCFGLINE];   // Generic input buffer

  FILE *cfgFP;              // Configuration file pointer
  int i=0;
  int ipcnt=0;              // Mechanisms and misc.                      
  int wagoCnt=0;            // WAGO unit count (max 6 per MODS) defined by WAGOIP_PORT keywords
  int qcCnt=0;              // IMCS quad cell system count (max 2 per MODS instance)
  int nominal_cnt=0;        // Nominal values
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

  // Now open the config file, if not, gripe and return -1.  Opening the
  // file here ensures that sensible defaults are set even if the config
  // file stuff was in error.

  if (!(cfgFP=fopen(cfgfile, "r"))) {
    printf("ERROR: Cannot open runtime configuration file %s\n",cfgfile);
    printf("       %s\n",strerror(errno));
    exit(-1);
  }

  //----------------------------------------------------------------
  //
  // Config file parser loop
  //
  // Read in each line of the config file and process it 
  //

  while(fgets(inbuf, MAXCFGLINE, cfgFP)) {

    // Skip comments (#) and blank lines
    i=0;
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
 
      else if (strcasecmp(keyword, "PORT")==0) {
        GetArg(inbuf, 2, argbuf);
        client.Port = atoi(argbuf);
      }

      // ISISID: Node name of the ISIS server.
      // 
      // Only meaningful if MODE ISISCLIENT has been set.
      //

      else if (strcasecmp(keyword, "ISISID")==0) {
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
							  
      else if (strcasecmp(keyword, "ISISPORT")==0) {
	GetArg(inbuf, 2, argbuf);
	client.isisPort = atoi(argbuf);
      }

      // LogFile: Runtime log file rootname (including path) 
      //
      // The .log extension will be appended to this rootname. 

      else if (strcasecmp(keyword, "LOGFILE")==0) { 
	GetArg(inbuf, 2, argbuf);
	strcpy(client.logFile, argbuf);
	client.doLogging = 1;
      }
      // PGMFILE: Runtime MicroLYNX file rootname (including path) 
      //
      // This is a common.pgm file for all MicroLYNX
      // File is stored in /home2/isl/plc/common.plc

      else if (strcasecmp(keyword, "PGMFILE")==0) { 
	GetArg(inbuf, 2, argbuf);
	strcpy(client.pgmFile, argbuf);
      }

      // NOLOG: Explicitly disable the runtime logging

      else if (strcasecmp(keyword, "NOLOG")==0) {
	client.doLogging = 0;
	
      }
      
      // Verbose: Enable verbose output mode (e.g., for debugging)

      else if (strcasecmp(keyword, "VERBOSE")==0) {
	client.isVerbose = 1;
	
      }

      // Debug: Enable runtime debugging out (superverbose mode)

      else if (strcasecmp(keyword, "DEBUG")==0) {
	client.Debug = 1;
	
      }


      //---------------------------------------------------------------
      //
      // Visible and IR Laser minimum and maximum power request
      //
      // 4 May 2009 - TJA
      //

      else if (strcasecmp(keyword, "IRLASER")==0) {
	GetArg(inbuf,2,argbuf);
        shm_addr->MODS.lasers.irlaser_minPower = atof(argbuf);
	GetArg(inbuf,3,argbuf);
        shm_addr->MODS.lasers.irlaser_maxPower = atof(argbuf);
      }

      else if (strcasecmp(keyword, "VISLASER")==0) {
	GetArg(inbuf,2,argbuf);
        shm_addr->MODS.lasers.vislaser_minPower = atof(argbuf);
	GetArg(inbuf,3,argbuf);
        shm_addr->MODS.lasers.vislaser_maxPower = atof(argbuf);
      }


      //---------------------------------------------------------------
      //
      // IR Laser Coefficients 
      //
      // Get the coefficients required for transforming values
      // from the register value for PSET (in DN) to the 
      // value in physical units (degrees Celsius) or the other
      // the other way around (user input power in mW to register DN)
      // 
      // What follows is eight nearly identical snippets of code. I have
      // commented only the first else if statement, but these comments
      // are relevant for all eight. What changes is merely which 
      // variable in shared memory is getting its assignment in each
      // small bit of code. 
      //
      // 30 April 2009 - TJA
      //

      else if (strcasecmp(keyword, "IRRTOPSET")==0) {
 	int coeffCnt = 0;	
        GetArg(inbuf,2,argbuf);
        coeffCnt = atoi(argbuf);

        // initialize all the values of the array to zero
        for (int j=0; j < 4; j++) {
          shm_addr->MODS.lasers.irlaserRegToPSetCoeff[j] = 0; 
        }

        // if the config file tells us that there are some non-zero
        // coefficients, grab those values
	if (coeffCnt > 0) {
	  for (int j=0; j < coeffCnt; j++) {
	    GetArg(inbuf,3+j,argbuf);
	    shm_addr->MODS.lasers.irlaserRegToPSetCoeff[j] = atof(argbuf);
          }
          shm_addr->MODS.lasers.irlaserRegToPSetNCoeff = coeffCnt;
        }
      }

      else if (strcasecmp(keyword, "IRRTOPOUT")==0) {
 	int coeffCnt = 0;	
        GetArg(inbuf,2,argbuf);
        coeffCnt = atoi(argbuf);

        for (int j=0; j < 4; j++) {
          shm_addr->MODS.lasers.irlaserRegToPOutCoeff[j] = 0; 
        }

	if (coeffCnt > 0) {
	  for (int j=0; j < coeffCnt; j++) {
	    GetArg(inbuf,3+j,argbuf);
	    shm_addr->MODS.lasers.irlaserRegToPOutCoeff[j] = atof(argbuf);
          }
          shm_addr->MODS.lasers.irlaserRegToPOutNCoeff = coeffCnt;
        }
      }

      else if (strcasecmp(keyword, "IRRTOTSET")==0) {
 	int coeffCnt = 0;	
        GetArg(inbuf,2,argbuf);
        coeffCnt = atoi(argbuf);

        for (int j=0; j < 4; j++) {
          shm_addr->MODS.lasers.irlaserRegToTSetCoeff[j] = 0; 
        }

	if (coeffCnt > 0) {
	  for (int j=0; j < coeffCnt; j++) {
	    GetArg(inbuf,3+j,argbuf);
	    shm_addr->MODS.lasers.irlaserRegToTSetCoeff[j] = atof(argbuf);
          }
          shm_addr->MODS.lasers.irlaserRegToTSetNCoeff = coeffCnt;
        }
      }

      else if (strcasecmp(keyword, "IRRTOTOUT")==0) {
 	int coeffCnt = 0;	
        GetArg(inbuf,2,argbuf);
        coeffCnt = atoi(argbuf);

        for (int j=0; j < 4; j++) {
          shm_addr->MODS.lasers.irlaserRegToTOutCoeff[j] = 0; 
        }

	if (coeffCnt > 0) {
	  for (int j=0; j < coeffCnt; j++) {
	    GetArg(inbuf,3+j,argbuf);
	    shm_addr->MODS.lasers.irlaserRegToTOutCoeff[j] = atof(argbuf);
          }
          shm_addr->MODS.lasers.irlaserRegToTOutNCoeff = coeffCnt;
        }
      }

      else if (strcasecmp(keyword, "IRPSETTOR")==0) {
 	int coeffCnt = 0;	
        GetArg(inbuf,2,argbuf);
        coeffCnt = atoi(argbuf);

        for (int j=0; j < 4; j++) {
          shm_addr->MODS.lasers.irlaserPSetToRCoeff[j] = 0; 
        }

	if (coeffCnt > 0) {
	  for (int j=0; j < coeffCnt; j++) {
	    GetArg(inbuf,3+j,argbuf);
	    shm_addr->MODS.lasers.irlaserPSetToRCoeff[j] = atof(argbuf);
          }
          shm_addr->MODS.lasers.irlaserPSetToRNCoeff = coeffCnt;
        }
      }


      //---------------------------------------------------------------
      //
      // Visible Laser Coefficients 
      //

      else if (strcasecmp(keyword, "VISRTOPSET")==0) {
 	int coeffCnt = 0;	
        GetArg(inbuf,2,argbuf);
        coeffCnt = atoi(argbuf);

        for (int j=0; j < 4; j++) {
          shm_addr->MODS.lasers.vislaserRegToPSetCoeff[j] = 0; 
        }

	if (coeffCnt > 0) {
	  for (int j=0; j < coeffCnt; j++) {
	    GetArg(inbuf,3+j,argbuf);
	    shm_addr->MODS.lasers.vislaserRegToPSetCoeff[j] = atof(argbuf);
          }
          shm_addr->MODS.lasers.vislaserRegToPSetNCoeff = coeffCnt;
        }
      }

      else if (strcasecmp(keyword, "VISRTOPOUT")==0) {
 	int coeffCnt = 0;	
        GetArg(inbuf,2,argbuf);
        coeffCnt = atoi(argbuf);

        for (int j=0; j < 4; j++) {
          shm_addr->MODS.lasers.vislaserRegToPOutCoeff[j] = 0; 
        }

	if (coeffCnt > 0) {
	  for (int j=0; j < coeffCnt; j++) {
	    GetArg(inbuf,3+j,argbuf);
	    shm_addr->MODS.lasers.vislaserRegToPOutCoeff[j] = atof(argbuf);
          }
          shm_addr->MODS.lasers.vislaserRegToPOutNCoeff = coeffCnt;
        }
      }

      else if (strcasecmp(keyword, "VISPSETTOR")==0) {
 	int coeffCnt = 0;	
        GetArg(inbuf,2,argbuf);
        coeffCnt = atoi(argbuf);

        for (int j=0; j < 4; j++) {
          shm_addr->MODS.lasers.vislaserPSetToRCoeff[j] = 0; 
        }

	if (coeffCnt > 0) {
	  for (int j=0; j < coeffCnt; j++) {
	    GetArg(inbuf,3+j,argbuf);
	    shm_addr->MODS.lasers.vislaserPSetToRCoeff[j] = atof(argbuf);
          }
          shm_addr->MODS.lasers.vislaserPSetToRNCoeff = coeffCnt;
        }
      }


      //---------------------------------------------------------------
      //
      // Microstep drive Parameters
      //

      // TTYPort = device port for microstep drive serial communications

      else if (strcasecmp(keyword, "IP_PORT")==0) {

	// IEB id (0?,1,2) (int)

        GetArg(inbuf,2,argbuf);
        shm_addr->MODS.ieb_i[ipcnt]=atoi(argbuf);

	// PORT and SOCKET id (000.000.000.000:8000) (char)

	memset(shm_addr->MODS.commport[ipcnt].Port,
	       0,
	       sizeof(shm_addr->MODS.commport[ipcnt].Port));
	memset(shm_addr->MODS.TTYIP[ipcnt],
	       0,
	       sizeof(shm_addr->MODS.TTYIP[ipcnt]));
        GetArg(inbuf,3,argbuf);
        sprintf(shm_addr->MODS.commport[ipcnt].Port,"%s",argbuf);
        sprintf(shm_addr->MODS.TTYIP[ipcnt],"%s",argbuf);

	// MECHANISM id (char name)

	memset(shm_addr->MODS.who[ipcnt],
	       0,
	       sizeof(shm_addr->MODS.who[ipcnt]));
        GetArg(inbuf,4,argbuf);
        sprintf(shm_addr->MODS.who[ipcnt],"%s",argbuf);

	// Minimum position (float - microLynx controllers revs)

        GetArg(inbuf,5,argbuf);
        shm_addr->MODS.min[ipcnt]=atof(argbuf);

	// Maximum position (float - microLynx controllers revs)

        GetArg(inbuf,6,argbuf);
        shm_addr->MODS.max[ipcnt]=atof(argbuf);

	// Time allowed for travel (int - secs)

        GetArg(inbuf,7,argbuf);
        shm_addr->MODS.timeout[ipcnt]=atoi(argbuf);

	// Conversion factor (float)

        GetArg(inbuf,8,argbuf);
        shm_addr->MODS.convf[ipcnt]=atof(argbuf);
        shm_addr->MODS.portcnt=ipcnt;
	ipcnt++;
      }

      else if (strcasecmp(keyword, "WAGOIP_PORT")==0) {

	// WAGO PORT and SOCKET id (000.000.000.000:8000) (char)

        GetArg(inbuf,2,argbuf);
	memset(shm_addr->MODS.WAGOIP[wagoCnt],
	       0,
	       sizeof(shm_addr->MODS.WAGOIP[wagoCnt]));
        strcpy(shm_addr->MODS.WAGOIP[wagoCnt],argbuf);

	// WAGO Controller id (char name)

        GetArg(inbuf,3,argbuf);
	memset(shm_addr->MODS.WAGOWHO[wagoCnt],
	       0,
	       sizeof(shm_addr->MODS.WAGOWHO[wagoCnt]));
        strcpy(shm_addr->MODS.WAGOWHO[wagoCnt],argbuf);

        wagoCnt++;
      }

      // New QC_PORT parameter for the WAGO IMCS quad cell for MODS2025
      //   3 arguments:
      //      ipAddr = WAGO IP address
      //      qcName = name for quad cell system (bimcs or rimcs)
      //      regAddr = analog input (ADC) module register address (int)
      // Limits: 2 max per MODS instance (blue and red)
      // Syntax: QC_ADDR ipAddr qcName regAddr
      // Example:  QC_ADDR 192.168.139.141 bimcs 0
      //
      // 2025 Jun 25 [rwp/osu]
      
      else if (strcasecmp(keyword, "QC_PORT")==0) {

	GetArg(inbuf,2,argbuf); // WAGO IP address
	memset(shm_addr->MODS.QC_IP[qcCnt],
	       0,
	       sizeof(shm_addr->MODS.QC_IP[qcCnt]));
	strcpy(shm_addr->MODS.QC_IP[qcCnt],argbuf);

        GetArg(inbuf,3,argbuf); // name for this IMCS quad cell (e.g., bimcs, rimcs)
	memset(shm_addr->MODS.QC_WHO[qcCnt],
	       0,
	       sizeof(shm_addr->MODS.QC_WHO[qcCnt]));
        strcpy(shm_addr->MODS.QC_WHO[qcCnt],argbuf);

	GetArg(inbuf,4,argbuf); // analog input module register address (int, e.g., 0)
        shm_addr->MODS.QC_REG[qcCnt] = atoi(argbuf);
	
	qcCnt++;
	
      }

      // Retire QCIP_PORT, see QC_PORT above [rwp/osu - 2025 Jun 25]
      
      /*
      else if (strcasecmp(keyword, "QCIP_PORT")==0) {

	// QuadCells PORT and SOCKET id (000.000.000.000) (char)

        GetArg(inbuf,2,argbuf);
	memset(shm_addr->MODS.qc_port[qc_ipcnt],
	       0,
	       sizeof(shm_addr->MODS.qc_port[qc_ipcnt]));
        strcpy(shm_addr->MODS.qc_port[qc_ipcnt],argbuf);

	// QuadCells Controller id (char name)

        GetArg(inbuf,3,argbuf);

	memset(shm_addr->MODS.qc_who[qc_ipcnt],
	       0,
	       sizeof(shm_addr->MODS.qc_who[qc_ipcnt]));
        strcpy(shm_addr->MODS.qc_who[qc_ipcnt],argbuf);
	qc_ipcnt++;
      }
      */
      
      else if (strcasecmp(keyword, "B_COLTTF")==0) {
        GetArg(inbuf,2,argbuf);
        shm_addr->MODS.b_imcsNominal[0]=atoi(argbuf);
        GetArg(inbuf,3,argbuf);
        shm_addr->MODS.b_imcsNominal[1]=atoi(argbuf);
        GetArg(inbuf,4,argbuf);
        shm_addr->MODS.b_imcsNominal[2]=atoi(argbuf);

      }
      else if (strcasecmp(keyword, "R_COLTTF")==0) {
        GetArg(inbuf,2,argbuf);
        shm_addr->MODS.r_imcsNominal[0]=atoi(argbuf);
        GetArg(inbuf,3,argbuf);
        shm_addr->MODS.r_imcsNominal[1]=atoi(argbuf);
        GetArg(inbuf,4,argbuf);
        shm_addr->MODS.r_imcsNominal[2]=atoi(argbuf);
      }
      else if (strcasecmp(keyword, "B_CAMFOC")==0) {
        GetArg(inbuf,2,argbuf);
        shm_addr->MODS.b_camfocNominal=atoi(argbuf);
      }
      else if (strcasecmp(keyword, "R_CAMFOC")==0) {
        GetArg(inbuf,2,argbuf);
        shm_addr->MODS.r_camfocNominal=atoi(argbuf);
      }
      else if (strcasecmp(keyword, "B_GTILT")==0) {
        GetArg(inbuf,2,argbuf);
        shm_addr->MODS.b_gtiltNominal=atoi(argbuf);
      }
      else if (strcasecmp(keyword, "R_GTILT")==0) {
        GetArg(inbuf,2,argbuf);
        shm_addr->MODS.r_gtiltNominal=atoi(argbuf);
      }

      // SPEED - port speed in baud (1200,2400,4800,9600,19200,38400)

      else if (strcasecmp(keyword, "SPEED")==0) {
        GetArg(inbuf,2,argbuf);
        shm_addr->MODS.commport[i].Speed = atof(argbuf);
      }
                                                                                
      // DATABITS - port databits
                                                                                
      else if (strcasecmp(keyword, "DATABITS")==0) {
        GetArg(inbuf,2,argbuf);
        shm_addr->MODS.commport[i].DataBits = atoi(argbuf);
      }
                                                                                
      // STOPBITS - port stopbits
                                                                                
      else if (strcasecmp(keyword, "STOPBITS")==0) {
        GetArg(inbuf,2,argbuf);
        shm_addr->MODS.commport[i].StopBits = atoi(argbuf);
      }

      // Parity - port Parity (0 or 1)

      else if (strcasecmp(keyword, "PARITY")==0) {
        GetArg(inbuf,2,argbuf);
        shm_addr->MODS.commport[i].Parity = atoi(argbuf);
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

  return(0);

}
