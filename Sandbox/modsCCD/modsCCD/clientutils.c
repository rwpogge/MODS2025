//
// clientutils - client utilities go here
//

/*!
  \file clientutils.c
  \brief Client utility functions

  Because they have to go somewhere...

  \author R. Pogge, OSU Astronomy Dept. (pogge.1@osu.edu)
  \original 2005 May 4
  \date 2025 July 24
  
*/

#include "client.h" // custom client application header 

//---------------------------------------------------------------------------

/*!
  \brief Setup and Take a Normal Exposure

  \param cam pointer to an azcam_t struct for an open azcam server
  \param obs pointer to an obspars_t struct with the observation parameters
  \param reply string to carry any messages returned from the server
  \return 0 on success, -1 if failure

  Sets up and initiates an exposure on the CCD camera.  It sets up and
  launches the exposure, returning control to the calling process for
  further monitoring and handling.  This allows the client to watch for
  abort directives either from the console or remote sockets.  The main
  client loop handles polling during exposures to monitor integration and
  readout progress, and then triggers the SaveImage() function to save
  the image.

  \sa DoBias(), DoFocus()

*/

int
doExposure(azcam_t *cam, obsPars_t *obs, char *reply)
{
  char temp[32];  // working string

  // Setup exposure parameters
  
  if (getDetPars(cam,reply)<0) // get the image size to be readout
    return -1;

  if (setImageInfo(cam,obs->imgType,obs->imgTitle,reply)<0) // image type and title
    return -1;
  
  // Upload the basic header parameters from the instrument

  uploadfits(cam,obs,reply);

  // Erase (clear) the CCD array

  if (clearArray(cam,reply)<0)
    return -1;

  // Start the Exposure, but do not wait for exposure completion

  if (!strcasecmp(obs->imgType,"dark") || !strcasecmp(obs->imgType,"bias") || !strcasecmp(obs->imgType,"zero")) {
    notifyClient(obs,"Starting Integration, Shutter=0 (Closed)...",STATUS);
  }
  else {
    notifyClient(obs,"Starting Integration, Shutter=1 (Open)...",STATUS);
  }

  if (startExposure(cam,EXP_NOWAIT,reply)<0)
    return -1;
  
  obs->tStart = SysTimestamp();  // Note the time we started

  if (obs->ExpTime > 5.0) // only do a count down if exptime>5 sec
    obs->doCountDown = 1;
  else
    obs->doCountDown = 0;

  // If the integration time is <1 second, treat as effectively instantaneous

  if (obs->ExpTime < 1.0) {
    cam->State = READOUT;
    obs->t1 = SysTimestamp();
    notifyClient(obs,"Integration Complete, Reading Out, Shutter=0 (Closed)...",STATUS);
  }
  else {
    cam->State = expStatus(cam,reply); // query exposure status from the server
  }

  return 0;
}

/*!
  \brief Setup and Take a Bias (Zero) Image

  \param cam pointer to an azcam_t struct for an open azcam server
  \param obs pointer to an obsPars_t struct with the observation parameters
  \param reply string to carry any messages returned from the server
  \return 0 on success, -1 if failure

  A Bias (or Zero) image is one in which the CCD is erased then readout
  immediately, giving a measurment of the 2-dimensional bias signal on
  the CCD that is a consequence of readout, not actual photons hitting
  the array.

  This function acquires a bias image by forcing the integration time to
  0.0sec, then taking an image that is immediately readout.  We bypass
  any integration countdown monitoring by setting the server state
  to READOUT immediately upon completion of the StartExposure call.

  \sa DoExposure(), DoFocus()

*/

int
doBias(azcam_t *cam, obsPars_t *obs, char *reply)
{

  // Setup exposure parameters

  if (getDetPars(cam,reply)<0) // get the image size to be readout
    return -1;

  strcpy(obs->imgType,"bias");
  
  if (setImageInfo(cam,obs->imgType,obs->imgTitle,reply)<0) // image type and title
    return -1;
  
  // Upload the basic header parameters from the instrument

  uploadfits(cam,obs,reply);

  // Erase (clear) the CCD array

  if (clearArray(cam,reply)<0)
    return -1;

  // Start the Exposure - since this is a bias, we wait for completion

  notifyClient(obs,"Starting Bias Image - Shutter=0 (Closed)",STATUS);
  if (startExposure(cam,EXP_WAIT,reply)<0)
    return -1;
  
  cam->State = READOUT;  // since exposure is instantaneous...
  obs->t1 = SysTimestamp();
  notifyClient(obs,"Reading Out",STATUS);

  return 0;
}


/*!
  \brief Poll the azcam Server to monitor exposure progress

  \param cam pointer to an azcam_t struct for an open azcam server
  \param obs pointer to an obsPars_t struct with the observation parameters
  \param reply string to carry any messages returned from the server
  \return 0 on success, -1 if failure

  Query the azcam server to determine the time remaining on the
  exposure.  Update exposure status info as well.

  QUESTION: does the python azcam server crash if you try to read
  time remaining before it is done?  Dunno.  We will check the
  azcam server exposure state flag and only do the time left
  query if we are actually exposing.

  \sa pollReadout()

*/

int
pollExposure(azcam_t *cam, obsPars_t *obs, char *reply)
{
  float tleft;

  // Only query for exposure time left if we are still exposing
  
  cam->State = expStatus(cam,reply);
  if (cam->State == EXPOSING) {
    obs->tNow = SysTimestamp(); // check time now

    // compute the time remaining in the current integration

    if (timeLeft(cam,reply)<0)
      return -1;

    obs->tLeft atof(reply);

    if (obs->tLeft <= 0.0) 
      obs->tLeft = 0.0;
  }
  return 0;
  
}

/*!
  \brief Poll the azcam server to monitor readout progress

  \param cam pointer to an azcam_t struct for an open azcam server
  \param obs pointer to an obsPars_t struct with the observation parameters
  \param reply string to carry any messages returned from the server
  \return 0 on success, -1 if failure

  Query the azcam server to determine the number of pixels that have
  been readout from the CCD.

  \sa pollExposure()
*/

int
pollReadout(azcam_t *cam, obsPars_t *obs, char *reply)
{
  double dt;
  int pixLeft;
  
  cam->State = expStatus(cam,reply);
  if (cam->State != READOUT) 
    return 0;

  pixLeft = pixelsLeft(cam,reply);
  if (pixLeft<0)
    return -1;

  cam->pixLeft = pixLeft;

  return 0;
}

/*!
  \brief Notify the client process who initiated an exposure of its progress

  \param obs pointer to an obsPars_t struct with the observation parameters
  \param msgStr message string to send to the client
  \param msgtype type of message being sent
  \return 0 on success, -1 if failure
  
  This is needed because we need some way to tell the client process who
  initiated an exposure what the progress of that exposure is.

  The typedef MsgType is defined in isisclient.h
  
*/

int
notifyClient(obsPars_t *obs, char *msgStr, MsgType msgType)
{
  char outMsg[256];

  if (strlen(msgStr)<0) return 0;

  // if msgType = STATUS and cam->State > 0 (taking data), augment the
  // msgStr with an appropriate EXPSTATUS flag

  if (msgType == STATUS) {
    switch(cam->State) {
    case SETUP:
      sprintf(msgStr,"%s EXPSTATUS=INITIALIZING",msgStr);
      break;
      
    case EXPOSING:
      sprintf(msgStr,"%s EXPSTATUS=INTEGRATING",msgStr);
      break;

    case ABORT:
      sprintf(msgStr,"%s EXPSTATUS=ABORTING",msgStr);
      break;

    case PAUSED:
      sprintf(msgStr,"%s EXPSTATUS=PAUSED",msgStr);
      break;

    case RESUME:
      sprintf(msgStr,"%s EXPSTATUS=INTEGRATING",msgStr);
      break;

    case READOUT:
    case READ:
      sprintf(msgStr,"%s EXPSTATUS=READOUT",msgStr);
      break;
      
    case WRITING:
      sprintf(msgStr,"%s EXPSTATUS=WRITING",msgStr);
      break;
      
    default:
      break;
    }
  }

  // Message was from the console, echo to stdout
  
  if (strcasecmp(obs->expHost,"console")==0) {
    switch(msgType) {
    case STATUS:
      printf("STATUS: %s\n",msgStr);
      break;

    case DONE:
      printf("DONE: %s\n",msgStr);
      break;

    case ERROR:
      printf("ERROR: %s\n",msgStr);
      break;

    case WARNING:
      printf("WARNING: %s\n",msgStr);
      break;

    case FATAL:
      printf("FATAL: %s\n",msgStr);
      break;

    default:
      printf(">> %s\n",msgStr);
      break;
    }

  }

  // command initiated by a remote client, more fun
  
  else {
    strcpy(outMsg,"");
    switch(msgType) {
    case STATUS:
      sprintf(outMsg,"%s>%s STATUS: %s\n",client.ID,obs->expHost,msgStr);
      break;

    case DONE:
      sprintf(outMsg,"%s>%s DONE: %s\n",client.ID,obs->expHost,msgStr);
      break;

    case ERROR:
      sprintf(outMsg,"%s>%s ERROR: %s\n",client.ID,obs->expHost,msgStr);
      break;

    case WARNING:
      sprintf(outMsg,"%s>%s WARNING: %s\n",client.ID,obs->expHost,msgStr);
      break;

    case FATAL:
      sprintf(outMsg,"%s>%s FATAL: %s\n",client.ID,obs->expHost,msgStr);
      break;

    default:
      sprintf(outMsg,"%s>%s %s\n",client.ID,obs->expHost,msgStr);
      break;
    }

    // We have something to send the remote client, do it
    
    if (strlen(outMsg)>0) {
      if (client.useISIS)
	SendToISISServer(&client,outMsg);
      else 
	ReplyToRemHost(&client,outMsg);
      
      if (client.isVerbose) {
	outMsg[strlen(outMsg)-1]='\0';
	printf("OUT: %s\n",outMsg);
      }
    } // end of reply handling

  }
  return 0;

}

/*!
  \brief Write out an image

  \param cam pointer to an azcam_t struct for an open azcam server
  \param obs pointer to an obsPars_t struct with the observation parameters
  \param reply string to carry any messages returned from the server
  \return 0 on success, -1 if failure

  Instructs the azcam server to write the contents of the current image
  buffer to disk in the specified format.  The filename and format information
  are in the cam and obs data structures.  If the write is successful, it
  advances the file counter.

*/

int
writeCCDImage(azcam_t *cam, obsPars_t *obs, char *reply)
{
  char fullname[256];  // full filename including path
  char filename[256];  // filename less path
  char msgStr[256];    // string for replies from the azcam server
  double dt;

  // check the file descriptor and make sure we have an active connection

  if (cam->FD<0) {
    strcpy(reply,"No azcam server connection active.");
    return -1;
  }

  // Build the full filename including path and counters

  memset(fullname,0,sizeof(fullname));
  memset(filename,0,sizeof(filename));
  sprintf(filename,"%s.%4.4d.fits",cam->fileName,cam->fileNum);
  sprintf(fullname,"%s\\%s",cam->filePath,filename);

  // Write out the image in the appropriate format

  memset(msgStr,0,sizeof(msgStr));
  obs->t1 = SysTimestamp();
  if (writeImage(cam,fullname,msgStr)<0) {
    sprintf(reply,"Write failed for %s - %s",fullname,msgStr);
    cam->State = IDLE;
    return -1;
  }
  obs->t2 = SysTimestamp();
  if (client.isVerbose) {
    dt = obs->t2 - obs->t1;
    printf("Image Write Time = %.3f seconds\n",dt);
  }

  // Advance the file counter

  cam->fileNum++;
  strcpy(cam->LastFile,filename);
  sprintf(reply,"Wrote LastFile=%s to Path=%s",
	  cam->LastFile,cam->filePath);
  cam->State = IDLE;

  return 0;

}

/*!
  \brief Send an image to the Data Manager for Processing

  \param cam pointer to an azcam_t struct for an open azcam server
  \param obs pointer to an obsPars_t struct with the observation parameters
  \param fname name of the image (no path, but .fits)
  \param reply string to carry any messages returned from the server
  \return 0 on success, -1 if failure

  Instructs the Data Manager agent, if active, to perform further
  processing on the image fname.  This can include transfering the
  image from a disk on the azcam server to (for example) a Linux host
  running this client, displaying an image, etc.  Whatever the steps
  performed by the Data Manager, they happen "offline" as far as the
  azcam server is concerned (once it successfully writes a FITS image
  to disk, the azcam server is "done").

  We fire off the "process %s" command to the Data Manager and then do
  not wait for a reply.  We also make sure that the apparent sender ID
  is #obspars::expHost, so that any subsequent status messages from
  the Data Manager go to that host, not (necessarily) to this client
  (unless the exposure command was from the console).  The process
  that started the exposure is responsible for all subsequent output
  from the Data Manager.  The correct routing is only assured on an
  ISIS network, or with a very savvy Data Manager.

*/

int
processImage(azcam_t *cam, obsPars_t *obs, char *fname, char *reply)
{
  char cmdStr[256];    // command string to send
  char srcID[16];      // source ID of whoever started this

  // can't do this if we don't have a data manager handy

  if (!dm.useDM) {
    sprintf(reply,"No Data Manager agent enabled.");
    return -1;
  }

  // set the srcID for the message as obs->expHost

  if (strcasecmp(obs->expHost,"console")==0)
    strcpy(srcID,client.ID);
  else
    strcpy(srcID,obs->expHost);

  // build the command string

  sprintf(cmdStr,"%s>%s process %s\r",srcID,dm.ID,fname);

  // Upload the command to DataMan and return

  if (writeDataMan(&dm,cmdStr)<0) {
    strcpy(reply,"Post-Processing command upload failed - cannot write to socket");
    return -1;
  }

  sprintf(reply,"Post-Processing command uploaded to %s...",dm.ID);

  return 0;

}

/*!
  \brief Load an azcam config file contents into an azcam struct

  \param cam pointer to an azcam_t struct for an open azcam server
  \param config string with the azcam camera configuration file name
  \param reply string to carry any messages returned from the server
  \return 0 on success, -1 if failure
  
  Opens and parses the contents of a azcam server configuration file
  containing all of the basic parameters required to setup the server
  nad detector for operation.

  It is assumed that the azcam config file resides on a path visible to
  the system running the azcam *client* application, not on the server
  system.  

  \sa loadconfig()

*/

int
loadAzCamConfig(azcam_t *cam, char *config, char *reply)
{

  char inbuf[MAXCFGLINE];   // Generic line buffer
  char keyword[MAXCFGLINE]; // File is organized into KEYWORD VALUE pairs
  char argbuf[MAXCFGLINE];  // Generic argument buffer

  FILE *cfgFP;              // config file pointer
  int i;
  char c;                    

  // Open the config file readonly

  if (!(cfgFP=fopen(config, "r"))) {
    sprintf(reply,"Cannot open azcam config file %s - %s\n",
	    config,strerror(errno));
    return -1;
  }

  strcpy(cam->cfgFile,config); // save the name of this file

  while(fgets(inbuf, MAXCFGLINE, cfgFP)) {

    // Skip comments (#) and blank lines

    if ((inbuf[0]!='#') && (inbuf[0]!='\n') && inbuf[0]!='\0') {
      inbuf[MAXCFGLINE] ='\0';
      GetArg(inbuf, 1, argbuf);
      strcpy(keyword, argbuf);
      
      // Parse Keywords

      // azcam Server Host 

      if (strcasecmp(keyword,"azcamHost")==0) {
	GetArg(inbuf,2,argbuf);
	strcpy(cam->Host,argbuf);
      }

      // azcam Server Port

      else if (strcasecmp(keyword,"azcamPort")==0) {
	GetArg(inbuf,2,argbuf);
	cam->Port = atoi(argbuf);
      }

      // azcam server comm timeout

      else if (strcasecmp(keyword,"Timeout")==0) {
	GetArg(inbuf,2,argbuf);
	cam->Timeout = (long)(atoi(argbuf));
	if (cam->Timeout < 0) cam->Timeout = 10L;
      }

      // azcam initialization script (on the azcam server proper)

      else if (strcasecmp(keyword,"IniFile")==0) {
	GetArg(inbuf,2,argbuf);
	if (strlen(argbuf)>0) {
	  strcpy(cam->IniFile,argbuf);
	}
	else {
	  sprintf(reply,"IniFile keyword blank in %s, must contain an entry",
		  config);
	  if (cfgFP !=0) fclose(cfgFP);
	  return -1;
	}
      }

      // CCD detector and control parameters for azcam

      // Region-of-Interest

      else if (strcasecmp(keyword,"FirstCol")==0) {
	GetArg(inbuf,2,argbuf);
	cam->FirstCol = atoi(argbuf);
      }

      else if (strcasecmp(keyword,"LastCol")==0) {
	GetArg(inbuf,2,argbuf);
	cam->LastCol = atoi(argbuf);
      }

      else if (strcasecmp(keyword,"ColBin")==0) {
	GetArg(inbuf,2,argbuf);
	cam->ColBin = atoi(argbuf);
      }

      else if (strcasecmp(keyword,"FirstRow")==0) {
	GetArg(inbuf,2,argbuf);
	cam->FirstRow = atoi(argbuf);
      }

      else if (strcasecmp(keyword,"LastRow")==0) {
	GetArg(inbuf,2,argbuf);
	cam->LastRow = atoi(argbuf);
      }

      else if (strcasecmp(keyword,"RowBin")==0) {
 	GetArg(inbuf,2,argbuf);
	cam->RowBin = atoi(argbuf);
      }

      // Detector Configuration

      else if (strcasecmp(keyword,"NCtotal")==0) {
 	GetArg(inbuf,2,argbuf);
	cam->NCtotal = atoi(argbuf);
      }

      else if (strcasecmp(keyword,"NCpredark")==0) {
 	GetArg(inbuf,2,argbuf);
	cam->NCpredark = atoi(argbuf);
      }

      else if (strcasecmp(keyword,"NCunderscan")==0) {
 	GetArg(inbuf,2,argbuf);
	cam->NCunderscan = atoi(argbuf);
      }

      else if (strcasecmp(keyword,"NCoverscan")==0) {
 	GetArg(inbuf,2,argbuf);
	cam->NCoverscan = atoi(argbuf);
      }

      else if (strcasecmp(keyword,"NRtotal")==0) {
 	GetArg(inbuf,2,argbuf);
	cam->NRtotal = atoi(argbuf);
      }

      else if (strcasecmp(keyword,"NRpredark")==0) {
 	GetArg(inbuf,2,argbuf);
	cam->NRpredark = atoi(argbuf);
      }

      else if (strcasecmp(keyword,"NRunderscan")==0) {
 	GetArg(inbuf,2,argbuf);
	cam->NRunderscan = atoi(argbuf);
      }

      else if (strcasecmp(keyword,"NRoverscan")==0) {
 	GetArg(inbuf,2,argbuf);
	cam->NRoverscan = atoi(argbuf);
      }

      else if (strcasecmp(keyword,"NRframexfer")==0) {
 	GetArg(inbuf,2,argbuf);
	cam->NRframexfer = atoi(argbuf);
      }

      // CCD Detector Temperature setpoint

      else if (strcasecmp(keyword,"CCDTemp")==0) {
	GetArg(inbuf,2,argbuf);
	cam->SetPoint = atof(argbuf);
      }

      // Output File Info

      else if (strcasecmp(keyword,"FileFormat")==0) {
 	GetArg(inbuf,2,argbuf);
	if (strcasecmp(argbuf,"STDFITS")==0) {
	  cam->fileFormat = STDFITS;
	}
	else if (strcasecmp(argbuf,"MEF")==0) {
	  cam->fileFormat = MEF;
	}
	else if (strcasecmp(argbuf,"BINARY")==0) {
	  cam->fileFormat = BINARY;
	}
	else {
	  sprintf(reply,"FileFormat option '%s' unrecognized in %s",
		 argbuf,config);
	  if (cfgFP !=0) fclose(cfgFP);
	  return -1;
	}
      }

      else if (strcasecmp(keyword,"FileName")==0) {
	GetArg(inbuf,2,argbuf);
	strcpy(cam->fileName,argbuf);
      }

      else if (strcasecmp(keyword,"FilePath")==0) {
	GetArg(inbuf,2,argbuf);
	strcpy(cam->filePath,argbuf);
      }

      // Any other junk? gripe to stdout but don't generate an error

      else { 
	printf("Ignoring unrecognized config file entry '%s'", inbuf);
	
      }
    }
    memset(inbuf,0,sizeof(inbuf));
  }

  // All done, close the config file and report success

  if (cfgFP !=0) fclose(cfgFP);
  
  sprintf(reply,"Loaded azcam config file %s successfully",config);
  return 0;

}

/*!
  \brief (re)Initialize the CCD Configuration

  \param cam pointer to an azcam_t struct for an open azcam server
  \param reply string to carry any messages returned from the server
  \return 0 on success, -1 if failure
  
  Re-initialize the CCD configuration by instructing the azcam Server to
  execute the initialization script (cam->IniFile) normall loaded at
  startup time.

*/

int
initCCDConfig(azcam_t *cam, char *reply)
{

  // Check the file descriptor and make sure we have an active connection

  if (cam->FD<0) {
    strcpy(reply,"No azcam server connection active");
    return -1;
  }

  // cam->iniFile file needs to be defined for this to work.

  if (strlen(cam->iniFile)==0) {
    strcpy(reply,"No CCD Initialization File Defined, Cannot Initialize");
    return -1;
  }

  // Tell the azcam Server to run the script in cam->iniFile

  if (RunScript(cam,cam->iniFile,reply)<0)
    return -1;

  // Reset some state variables
    
  cam->State = IDLE;
  cam->Abort = 0;

  sprintf(reply,"azcam Server initialized using the %s script",cam->iniFile);
  return 0;
}

/*!
  \brief Upload basic information for the Image FITS headers

  \param cam pointer to an azcam_t struct for an open azcam server
  \param obs pointer to an obsPars_t struct with the observation parameters
  \param reply string to carry any messages returned from the server
  \return 0 on success, -1 if failure

  Uploads client and observation info to the azcam server header
  database for inclusing with image FITS headers.  Only items
  not already handled by the azcam server are uploaded.

*/
  
int
uploadFITS(azcam_t *cam, obsPars_t *obs, char *reply)
{
  char keyval[32]; // keyword value string
  char comment[32]; // comment string
  double t1, t2, dt;

  // Data-Taking System Info

  if (client.Debug)
    t1 = SysTimestamp();

  // Observing properties (who, project ID, etc.)

  setKeyword(cam,"OBSERVER",obs->Observer,"Observer(s)",reply);
  setKeyword(cam,"PARTNER",obs->Partner,"LBT Project Partner(s)",reply);
  setKeyword(cam,"PROPID",obs->PropID,"Observing Proposal ID",reply);
  setKeyword(cam,"PI_NAME",obs->PIName,"Project PI Name(s)",reply);
  setKeyword(cam,"SUPPORT",obs->Support,"LBT Support Scientist(s)",reply);
  setKeyword(cam,"TELOPS",obs->TelOps,"LBT Telescope Operator(s)",reply);

  // where we would get ISTATUS info and pass up to azcam (ugly)
  
  // All done with custom header cards

  if (client.Debug) {
    t2 = SysTimestamp();
    dt = t2 - t1;
    printf("FITS Parameter Upload required %.6f seconds\n",dt);
  }

  return 0;
}

/*!
  \brief Initialize the obspars struct with sensible defaults

  \param obs pointer to an obsPars_t struct with the observation parameters

  Resets the values of the obspars struct with default values to avoid
  delinquent values leaking into the system.

*/

void
initObsPars(obsPars_t *obs) 
{
  strcpy(obs->imgType,"BIAS");
  strcpy(obs->imgTitle,"NONE");
  obs->expTime = 0.0;
  obs->doCountDown = 0;
  obs->tStart = 0.0;
  obs->tNow = 0.0;
  obs->tLeft = 0.0;

  strcpy(obs->expHost,"console");

  strcpy(obs->Observer,"NONE");
  strcpy(obs->Partner,"ENGINEERING");
  strcpy(obs->PropID,"ENGINEERING");
  strcpy(obs->PIName,"ENGINEERING");
  strcpy(obs->Support,"NONE");
  strcpy(obs->TelOps,"NONE");

  // here is where we add other bits as needed
  
}

/*!
  \brief Read the CCD and detector mount base temperatures

  \param cam pointer to an azcam_t struct for an open azcam server
  \param reply string to contain the temperature report
  \return 0 if successful, -1 on errors.  Success or error text is 
  in reply.

  Queries the azcam server to get the CCD and detector mount base
  temperatures and report them back to the calling application.

  The azcam server will prevent this from working during an exposure,
  but then the client should not be allowing this query during an
  exposure, either.

*/

int
readTemps(azcam_t *cam, char *reply)
{
  double dtemp;

  if (cam->FD < 0) return -1;
  
  if (getTemp(cam,reply)<0)
    return -1;

    // and report them.

  strcpy(reply,"");

  sprintf(reply,"CCDTemp=%.1f BaseTemp=%.1f",cam->ccdTemp,cam->baseTemp);

  return 0;
}

//---------------------------------------------------------------------------

// Other utilities we need?

