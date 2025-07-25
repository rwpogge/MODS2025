/*!
  \file exposure.c
  \brief AzCam Exposure Control Functions

  The following are a suite of interface functions that provide access
  to all of the azcam server exposure control functions.

  Most function names recapitulate the analogous azcam server commands,
  while others serve as interface functions that provide access to
  server commands that provide more than one function (e.g., setMode)

  All routines call the communication layer routines in iosubs.c to take
  care of common handling of timeout, errors, and reply processing.

  All of the server exposure control commands documented in Section 9
  of the <i>AzCam Programmers Reference Manual</i> have been
  implemented except the following:
  <pre>
  Expose - not implemented (doesn't work well with the OSU cameras)
  readImage - deprecated in recent AzCam versions
  setSyntheticImage - future expansion
  Guide - future expansion
  </pre>
  Two new functions
  <pre>
  setShutterMode()
  setReadoutMode()
  </pre>
  Are defined to implement the "setMode 1 X" and "setMode 2 X"
  commands, respectively.

  Finally, ParShift has been renamed RowShift in this implementation.

  \author R. Pogge, OSU Astronomy Dept. (pogge.1@osu.edu)
  \original 2005 May 17
  \date 2025 July 23
*/

#include "azcam.h" // AzCam client API header 

/*!
  \brief Clear (flush) the detector array
  
  \param cam pointer to an #azcam struct with the server parameters
  \param reply string to contain any reply text
  \return 0 if successful, -1 on errors, with error text in reply

  Erases the detector array preparatory to starting an integration.

*/

int
clearArray(azcam_t *cam, char *reply)
{
  char cmdStr[64];

  strcpy(cmdStr,"mods.flush");

  if (azcamCmd(cam,cmdStr,reply)<0)
    return -1;

  // success, set various flags as required...

  strcpy(reply,"CCD Erased");
  return 0;

}

/*!
  \brief Start an exposure (integration)
  
  \param cam pointer to an #azcam struct with the server parameters
  \param wait sets whether the azcam server waits until the exposure, 
  is complete before replying.  One of #EXP_NOWAIT or #EXP_WAIT.
  \param reply string to contain any reply text
  \return 0 if successful, -1 on errors, with error text in reply

  Starts an exposure on the detector of the current exposure time (see
  setExposure()).  The array is not erased first, this must be done
  explicitly using clearArray().

  If wait=#EXP_WAIT, the azcam server will not return status until the
  integration is completed.  This is usually only done for zero-length
  (ZERO or BIAS) images, or very short integrations where the
  application decides not to poll the exposure progress using
  readExposure() calls.

  If wait=#EXP_NOWAIT, the azcam server will return status immediately
  after starting the integration.  Integration progress can be monitored
  using the readExposure() function.

  IMPORTANT: if wait=#EXP_WAIT, the timeout interval for the AzCam
  server communications (#azcam::Timeout) must be set long enough as as
  to not timeout before the integration is complete.  To ensure that
  this does not happen, we store the current default timeout, compute a
  new timeout of the exposure time + 10 seconds, and then make the call,
  resetting the default timeout after completion (or error).  The
  exposure time used is the value in the #azcam::ExpTime data member.

  \sa setExposure()
*/

int
startExposure(azcam_t *cam, int wait, char *reply)
{
  char cmdStr[64];
  long default_to;

  // A gotcha here: if we are waiting, we better have the command
  // timeout interval longer than the integration time, or we'll
  // timeout.  Usually only done for short zero-length exposures.


  switch(wait) {
  case EXP_WAIT:
    strcpy(cmdStr,"mods.expwait");
    default_to = cam->Timeout;
    cam->Timeout = (long)(cam->ExpTime) + 10L;
    break;

  case EXP_NOWAIT:
    strcpy(cmdStr,"mods.expose");
    break;
  }

  // If we are waiting, save the default timeout, compute
  // a new timeout of exptime+10s

  if (azcamCmd(cam,cmdStr,reply)<0) {
    if (wait) cam->Timeout = default_to;
    return -1;
  }

  // success, set various flags as required...

  switch(wait) {
  case EXP_WAIT:
    cam->Timeout = default_to;
    strcpy(reply,"Exposure Sequence Completed");
    cam->State = READOUT;
    break;

  case EXP_NOWAIT:
    strcpy(reply,"Exposure Sequence Started");
    cam->State = EXPOSING;
    break;
  }

  return 0;
  
}

/*!
  \brief Query exposure status
  
  \return exposure state code (see azcam.h or azcam server docs)
  
  Queries the server and returns the current exposure status as
  an integer code.  Tranlsate this to a string and set the value
  of cam->State
  
*/

int
expStatus(azcam_t *cam, char *reply)
{
   char cmdStr[64];
   char msgStr[64];
   char status[32];
   int expCode;
   
    
    sprintf(cmdStr,"mods.expstatus");
    memset(msgStr,0,sizeof(msgStr));
    
    if (azcamCmd(cam,cmdStr,msgStr)<0) {
        sprintf(reply,"Cannot get exposure status - %s",msgStr);
        return -1;
    }
    
    sscanf(msgStr,"%d %s",&expCode,status);
    
    cam->State = expCode;
    sprintf(reply,"ExpStatus=%s",status);
    
    return 0;

}
  
/*!
  \brief Set the exposure (integration) time
  
  \param cam pointer to an #azcam struct with the server parameters
  \param exptime exposure time in decimal seconds
  \param reply string to contain any reply text
  \return 0 if successful, -1 on errors, with error text in reply

  Sets the exposure time for subsequent integrations.  The user provides
  the exposure time in seconds to millisecond precision.

*/

int
setExposure(azcam_t *cam, float exptime, char *reply)
{
  char cmdStr[64];

  if (exptime < 0) {
    sprintf(reply,"Invalid exposure time %.3f, must be positive",exptime);
    return -1;
  }

  sprintf(cmdStr,"mods.set_exptime %f.3",exptime);

  if (azcamCmd(cam,cmdStr,reply)<0)
    return -1;

  // Successful, save the exposure time in the cam struct

  cam->ExpTime = exptime;
  sprintf(reply,"ExpTime=%.3f sec",exptime);
  return 0;

}

/*!
  \brief Query the azcam server for the current elapsed exposure time
  
  \param cam pointer to an #azcam struct with the server parameters
  \param reply string to contain any reply text
  \return elapsed exposure time in milliseconds, or -1 if errors,
          with error text in reply

  Query the azcam server and return the elapsed exposure (integration)
  time in milliseconds.

  \par Note 

  In some azcam server implementions (i.e., every one we've encountered
  or heard about so far), if you send a readExposure command to the
  azcam server while a readout is in progress, the server will crash
  after readout is done, rebooting the machine (meaning total system
  crash).  We avoid this by not allowing the user to send this directive
  during readout by checking the value of the #azcam::State data member.

*/

int
readExposure(azcam_t *cam, char *reply)
{
  char cmdStr[64];

  if (cam->State == READOUT) return 0; 
  cam->timeLeft = 0.0;
  
  // Hope we're safe, do it...

  strcpy(cmdStr,"mods.timeleft");

  if (azcamCmd(cam,cmdStr,reply)<0)
    return -1;

  cam->timeLeft = atof(reply);
  sprintf(reply,"TimeLeft=%.3f sec",cam->timeLeft);
  return cam->timeLeft;

}

/*!
  \brief Abort an exposure in progress.
  
  \param cam pointer to an #azcam struct with the server parameters
  \param reply string to contain any reply text
  \return 0 if successful, -1 on errors, with error text in reply

  Issues an abort command to the azcam server and sets our internal
  Abort flag (kept in the #azcam::Abort data member).

  We check the azcam server exposure status and issue an exposure or
  readout abort as required.  If the CCD is neither exposing nor
  reading, there are no abort available for setup or writing phases,
  we just have to let those finish (at least they are quick).

*/

int
abortExposure(azcam_t *cam, char *reply)
{
  char cmdStr[64];

  // we are cautious to do this by querying the
  // exposure status

  if (expStatus(cam,reply)<0)
    return -1;

  if (cam->State == EXPOSING) {
    strcpy(cmdStr,"exposure.abort");

    if (azcamCmd(cam,cmdStr,reply)<0) {
      strcat(reply," - abortExposure Failed");
      return -1;
    }

    sprintf(reply,"Exposure Aborted");
  }
  else if (cam->State == READOUT) {
    strcpy(cmdStr,"mods.abort_readout");
    
    if (azcamCmd(cam,cmdStr,reply)<0) {
      strcat(reply," - abortExposure Failed");
      return -1;
    }
    sprintf(reply,"Exposure Readout Aborted");
  }    
  else {
    strcpy(reply,"No exposure in progress to abort");
    return -1;
  }

  // success, set various abort flags as required...

  cam->Abort = 1;
  
  return 0;

}

/*!
  \brief Pause an exposure in progress.
  
  \param cam pointer to an #azcam struct with the server parameters
  \param reply string to contain any reply text
  \return 0 if successful, -1 on errors, with error text in reply

  Issues an exposure.pause command to the azcam server and sets
  the #azcam::State flag to PAUSE on success.  If it fails, the
  #azcam::State flag is left unchanged.

  A pause should be followed by either abort or resume.

  We test the value of the #azcam::State flag and only send a
  pauseExposure command if #EXPOSING.  Sending a pauseExposure
  directive otherwise may be unpredictable.
  
  \sa abortExposure(), resumeExposure()
*/

int
pauseExposure(azcam_t *cam, char *reply)
{
  char cmdStr[64];

  if (cam->State != EXPOSING) {
    strcpy(reply,"No exposure in-progress to PAUSE");
    return -1;
  }

  strcpy(cmdStr,"exposure.pause");

  if (azcamCmd(cam,cmdStr,reply)<0) {
    strcat(reply," - pauseExposure Failed");
    return -1;
  }

  // success, set various flags as required...

  cam->State = PAUSE;
  strcpy(reply,"Exposure Paused");
  return 0;

}

/*!
  \brief Resume a paused exposure
  
  \param cam pointer to an #azcam struct with the server parameters
  \param reply string to contain any reply text
  \return 0 if successful, -1 on errors, with error text in reply

  Issues a resumeExposure command to the azcam server.  Sets the
  #azcam::State flag to #EXPOSING on success.  If it fails, the
  #azcam::State flag is left unchanged.

  Does not send resumeExposure if the controller is not in a #PAUSE
  state.

  If you send a resumeExposure() command when the azcam server is
  not actually in a paused state, very bad things can happen (e.g.,
  it crashes and reboots its host computer).

  \sa pauseExposure()
*/

int
resumeExposure(azcam_t *cam, char *reply)
{
  char cmdStr[64];

  if (cam->State != PAUSE) {
    strcpy(reply,"No PAUSEd exposure to RESUME");
    return -1;
  }

  strcpy(cmdStr,"exposure.resume");

  if (azcamCmd(cam,cmdStr,reply)<0) {
    strcat(reply," - resumeExposure Failed");
    return -1;
  }

  // success, set various flags as required...

  cam->State = EXPOSING;
  strcpy(reply,"Exposure Resumed");
  return 0;

}

/*!
  \brief Define the detector format in unbinned pixels

  \param cam pointer to an #azcam struct with the server parameters
  \param reply string to contain any reply text
  \return 0 if successful, -1 on errors, with error text in reply

  Defines the detector format in unbinned pixels.  Since this function
  is very complicated and important, involving 9 parameters, we do this
  by using the relevant data members of the #azcam struct to define the
  parameters, rather than having a lot of function arguments.  The
  relevant data members are:
  <pre>
    #azcam::NCtotal      - Total number of columns (serial pixels)
    #azcam::NCpredark    - Number of physical dark prescan columns
    #azcam::NCunderscan  - Number of underscan columns to read
    #azcam::NCoverscan   - Number of overscan columns to read
    #azcam::NRtotal      - Total number of rows (lines)
    #azcam::NRpredark    - Number of physical dark prescan rows
    #azcam::NRunderscan  - Number of underscan rows to read
    #azcam::NRoverscan   - Number of overscan rows to read
    #azcam::NRframexfer  - Number of rows to shift for frame transfer mode
  </pre>
  A calling application would first set the various parameters in the
  #azcam struct and then call this function to send them to the AzCam
  server.

  \sa setROI()
*/

int
setFormat(azcam_t *cam, char *reply)
{
  char cmdStr[64];

  // It can be somewhat bad to load a bogus detector format
  // so the default set by InitAzCam() has coded "don't know"
  // values.  Basically, if either NCtotal or NRtotal
  // are zero (0), we don't know the format and won't load it

  if (cam->NCtotal == 0 || cam->NRtotal ==0) {
    sprintf(reply,"Invalid detector format (NCtotal=%d NRtotal=%d)",
	    cam->NCtotal,cam->NRtotal);
    return -1;
  }

  // We hope we're valid (hard to globally validate this, let the server
  // gripe if it has problems, no guarantees...)

  sprintf(cmdStr,"exposure.set_format %d %d %d %d %d %d %d %d %d",
	  cam->NCtotal, cam->NCpredark, cam->NCunderscan,
	  cam->NCoverscan, cam->NRtotal, cam->NRpredark,
	  cam->NRunderscan, cam->NRoverscan, cam->NRframexfer);

  if (azcamCmd(cam,cmdStr,reply)<0)
    return -1;

  // set flags as required...

  strcpy(reply,cmdStr);
	  
  return 0;

}
  
  
/*!
  \brief Define the detector region of interest for the next readout

  \param cam pointer to an #azcam struct with the server parameters
  \param reply string to contain any reply text
  \return 0 if successful, -1 on errors, with error text in reply

  Sets the region of interest for the next detector readout in 
  units of unbinned pixels.  Since this is complicated and would
  involve a lot of arguments, we set this by using the relevant data 
  members of the #azcam struct to define the parameters:
  <pre>
    #azcam::FirstCol - first column to read in unbinned pixels
    #azcam::LastCol  - last column to read in unbinned pixels
    #azcam::ColBin   - column-axis binning factor
    #azcam::FirstRow - first row to read in unbinned pixels
    #azcam::LastRow  - last row to read in unbinned pixels
    #azcam::RowBin   - row-axis binning factor
  </pre>
  Note that regions of interest that are smaller than the physical
  size of the device in unbinned pixels are only supported for
  single-amplifier readout modes.

*/

int
setROI(azcam_t *cam, char *reply)
{
  char cmdStr[64];

  // Quick check, if FirstCol or LastCol is 0, that signals "don't know"
  // we we should not set the ROI

  if (cam->FirstCol == 0 || cam->FirstRow == 0) {
    sprintf(reply,"Invalid ROI, FirstCol=%d FirstRow=%d",
	    cam->FirstCol,cam->FirstRow);
    return -1;
  }

  // Another is if either ColBin or RowBin are zero

  if (cam->ColBin == 0 || cam->RowBin == 0) {
    sprintf(reply,"Invalid ROI, ColBin=%d RowBin=%d",
	    cam->ColBin,cam->RowBin);
    return -1;
  }
    
  // We hope we're OK, send it up

  sprintf(cmdStr,"mods.set_roi %d %d %d %d %d %d",
	  cam->FirstCol,cam->LastCol,
	  cam->FirstRow,cam->LastRow,
	  cam->ColBin,cam->RowBin);

  if (azcamCmd(cam,cmdStr,reply)<0)
    return -1;

  // set flags as required...

  sprintf(reply,"ROI=[%d:%d,%d:%d] XBin=%d YBin=%d",
	  cam->FirstCol,cam->LastCol,
	  cam->FirstRow,cam->LastRow,
	  cam->ColBin,cam->RowBin);
	  
  return 0;

}

// set CCD binning

int setCCDBin(azcam_t *cam, int xbin, int ybin, char *reply)
{
  char cmdStr[64];

  if (xbin<=0) xbin = -1;  // means "no change" 
  if (ybin<=0) ybin = -1;

  // set one or both
  
  if (xbin > 0 || ybin > 0) {
    sprintf(cmdStr,"mods.set_ccdbin %d %d",xbin,ybin);
    if (azcamCmd(cam,cmdStr,reply)<0)
      return -1;
  }

  // query to confirm

  strcpy(cmdStr,"mods.get_ccdbin");
  if (azcamCmd(cam,cmdStr,reply)<0)
    return -1;

  sscanf(reply,"%d %d",&xbin,&ybin);
  sprintf(cmdStr,"XBIN=%d YBIN=%d",xbin,ybin);

  cam->ColBin = xbin;
  cam->RowBin = ybin;
  
  return CMD_OK;
    
}

// set CCD x (column) binning

int setXBin(azcam_t *cam, int xbin, char *reply)
{
  char cmdStr[64];
  int ybin;
  
  if (xbin > 1) {
    sprintf(cmdStr,"mods.set_ccdbin %d -1",xbin);
    if (azcamCmd(cam,cmdStr,reply)<0)
      return -1;
  }

  // query to confirm

  strcpy(cmdStr,"mods.get_ccdbin");
  if (azcamCmd(cam,cmdStr,reply)<0)
    return -1;

  sscanf(reply,"%d %d",&xbin,&ybin);
  sprintf(cmdStr,"XBIN=%d YBIN=%d",xbin,ybin);

  cam->ColBin = xbin;
  cam->RowBin = ybin;
  
  return CMD_OK;
    
}

// set CCD y (row) binning

int setYBin(azcam_t *cam, int ybin, char *reply)
{
  char cmdStr[64];
  int xbin;
  
  if (ybin > 1) {
    sprintf(cmdStr,"mods.set_ccdbin -1 %d",ybin);
    if (azcamCmd(cam,cmdStr,reply)<0)
      return -1;
  }

  // query to confirm

  strcpy(cmdStr,"mods.get_ccdbin");
  if (azcamCmd(cam,cmdStr,reply)<0)
    return -1;

  sscanf(reply,"%d %d",&xbin,&ybin);
  sprintf(cmdStr,"XBIN=%d YBIN=%d",xbin,ybin);

  cam->ColBin = xbin;
  cam->RowBin = ybin;
  
  return CMD_OK;
    
}

/*!
  \brief Open the Shutter.
  
  \param cam pointer to an #azcam struct with the server parameters
  \param reply string to contain any reply text
  \return 0 if successful, -1 on errors, with error text in reply

  Instructs the azcam server to open the shutter.

  \sa CloseShutter()
*/

int
openShutter(azcam_t *cam, char *reply)
{
  char cmdStr[64];

  strcpy(cmdStr,"mods.shopen");

  if (azcamCmd(cam,cmdStr,reply)<0)
    return -1;

  // success, set various flags as required...

  cam->Shutter = SH_OPEN;
  strcpy(reply,"Shutter=1 (Open)");
  return 0;

}

/*!
  \brief Close the Shutter.
  
  \param cam pointer to an #azcam struct with the server parameters
  \param reply string to contain any reply text
  \return 0 if successful, -1 on errors, with error text in reply

  Instructs the azcam server to close the shutter.

  \sa OpenShutter()
*/

int
closeShutter(azcam_t *cam, char *reply)
{
  char cmdStr[64];

  strcpy(cmdStr,"mods.shclose");

  if (azcamCmd(cam,cmdStr,reply)<0)
    return -1;

  // success, set various flags as required...

  cam->Shutter = SH_CLOSED;
  strcpy(reply,"Shutter=0 (Closed)");
  return 0;

}

/*!
  \brief Query the azcam server for the current image size
  
  \param cam pointer to an #azcam struct with the server parameters
  \param reply string to contain any reply text
  \return the total number of pixels in the image if successful, -1 if
  errors.

  Queries the azcam server for the current total image size in pixels
  (including effects of binning, overscan regions, etc).  It also sets
  the #azcam::Ncols, #azcam::Nrows, and #azcam::Npixels data members in
  the cam struct for the convenience of the calling application.

*/

int
getDetPars(azcam_t *cam, char *reply)
{
  char cmdStr[64];
  char msgStr[64];
  int ncols;
  int nc_pdk;
  int nc_usc;
  int nc_osc;
  int nrows;
  int np_pdk;
  int np_usc;
  int np_osc;
  int np_ft;
  int npix;

  strcpy(cmdStr,"exposure.get_format");
  memset(msgStr,0,sizeof(msgStr));

  if (azcamCmd(cam,cmdStr,msgStr)<0) {
    sprintf(reply,"Cannot get detector parameters - %s",msgStr);
    return -1;
  }

  // extract the data

  sscanf(msgStr,"%d %d %d %d %d %d %d %d %d",&ncols,&nc_pdk,&nc_usc,&nc_osc,&nrows,&np_pdk,&np_usc,&np_osc,&np_ft);

  npix = nrows * ncols;

  // store the results in the cam struct

  cam->Nrows = nrows;
  cam->Ncols = ncols;
  cam->Npixels = npix;
  sprintf(reply,"Ncols=%d Nrows=%d Npix=%d",nrows,ncols,npix);
  return npix;
 
}

/*!
  \brief Query the azcam server for the number of pixels readout
  
  \param cam pointer to an #azcam struct with the server parameters
  \param reply string to contain any reply text
  \return Count of the number of pixels readout if successful, -1 
          on errors, with error text in reply

  Queries the azcam server and returns the number of pixels readout from
  the detector array.  Repeated calls to getPixelCount() are often used
  by applications to monitor readout progress.  When the array is
  finished reading out, the pixel count returned should equal the total
  pixel size returned by getDetPars().  After each call to
  getPixelCount(), it stores the current number of pixels read in
  #azcam::Nread.

  Note that unlike readExposure(), experiments have so far shown that
  calling getPixelCount() is benign in all circumstances.

  \sa getDetPars()
*/

int
getPixelCount(azcam_t *cam, char *reply)
{
  char cmdStr[64];
  int pixcount;

  strcpy(cmdStr,"mods.pixelsLeft"); // pixread = numpix - pixelsLeft...

  if (azcamCmd(cam,cmdStr,reply)<0)
    return -1;

  pixcount = atoi(reply);
  sprintf(reply,"PixCount=%d",pixcount);
  cam->Nread = pixcount; 
  return pixcount;

}

