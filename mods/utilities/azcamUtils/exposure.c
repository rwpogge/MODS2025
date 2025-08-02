/*!
  \file exposure.c \brief azcam server exposure control functions

  The following are a suite of interface functions that provide access
  to all of the azcam server exposure control functions.

  Most function names recapitulate the analogous azcam server commands,
  while others serve as interface functions that provide access to
  server commands that provide more than one function (e.g., setMode)

  All routines call the communication layer routines in iosubs.c to take
  care of common handling of timeout, errors, and reply processing.

  \author R. Pogge, OSU Astronomy Dept. (pogge.1@osu.edu)
  \original 2005 May 17
  \date 2025 July 25
*/

#include "azcam.h" // azcam client utility library header 

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
    cam->Timeout = (long)(cam->expTime) + 10L;
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
    break;

  case EXP_NOWAIT:
    strcpy(reply,"Exposure Sequence Started");
    break;
  }

  return 0;
  
}

/*!
  \brief Query exposure status
  
  \return exposure state code (see azcam.h or azcam server docs)
  
  Queries the server and returns the current exposure status as
  an integer code.  Translate this to a string and set the value
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

//---------------------------------------------------------------------------
//
// Exposure Setup
//

int
setImageInfo(azcam_t *cam, char *imgType, char *imgTitle, char *reply)
{
  char cmdStr[64];

  if (strlen(imgType)==0) {
    strcpy(cmdStr,"exposure.get_image_type");
    if (azcamCmd(cam,cmdStr,reply)<0)
      return -1;
    strcpy(imgType,reply);
  }

  if (strlen(imgTitle)==0) {
    strcpy(cmdStr,"exposure.get_image_title");
    if (azcamCmd(cam,cmdStr,reply)<0)
      return -1;
    strcpy(imgTitle,reply);
  }
			  
  sprintf(cmdStr,"mods.set_imageInfo %s \'%s\'",imgType,imgTitle);
  if (azcamCmd(cam,cmdStr,reply)<0)
    return -1;

  strcpy(cam->imgType,imgType);
  strcpy(cam->imgTitle,imgTitle);
  sprintf(reply,"IMAGETYP=%s OBJECT=(%s)",imgType,imgTitle);
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

  // if exptime is <0, this is a query
  
  if (exptime >= 0.0) {
    sprintf(cmdStr,"mods.set_exptime %.3f",exptime);
    if (azcamCmd(cam,cmdStr,reply)<0)
      return -1;
  }

  // confirm

  strcpy(cmdStr,"mods.get_exptime");
  if (azcamCmd(cam,cmdStr,reply)<0)
    return -1;

  cam->expTime = atof(reply);
  sprintf(reply,"ExpTime=%.3f sec",cam->expTime);

  return 0;

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

  // query the exposure status
  
  if (expStatus(cam,reply)<0)
    return -1;

  // We can only abort if EXPOSING, PAUSE, or RESUME

  switch(cam->State) {

  case EXPOSING:
  case PAUSE:
  case RESUME:

    strcpy(cmdStr,"exposure.abort");

    if (azcamCmd(cam,cmdStr,reply)<0) {
      strcat(reply," - abortExposure Failed");
      return -1;
    }

    sprintf(reply,"Exposure Aborted");
    cam->Abort = 1;
    break;

  default:
    strcpy(reply,"No exposure in progress to abort");
    cam->Abort = 0;
    break;
  }
  
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

  \sa pauseExposure()
*/

int
resumeExposure(azcam_t *cam, char *reply)
{
  char cmdStr[64];

  if (cam->State != PAUSE) {
    strcpy(reply,"No PAUSED exposure to RESUME");
    return -1;
  }

  strcpy(cmdStr,"exposure.resume");

  if (azcamCmd(cam,cmdStr,reply)<0) {
    strcat(reply," - resumeExposure Failed");
    return -1;
  }

  strcpy(reply,"Exposure Resumed");
  return 0;

}
  
/*!
  \brief Set the CCD subframe region of interest for the next readout

  \param cam pointer to an #azcam struct with the server parameters
  \param sc int starting column (x) in unbinned full-frame pixels
  \param ec int ending column (x) in unbinned full-frame pixels
  \param sr int starting row (y) in unbinned full-frame pixels
  \param er int ending row (y) in unbinned full-frame pixels
  \param reply string to contain any reply text
  \return 0 if successful, -1 on errors, with error text in reply

  Sets the region of interest for the next detector readout in 
  units of unbinned pixels.

  If all arguments are -1, treat as a query and return the
  current ROI (-1 to azcam means "don't change")

*/

int
setROI(azcam_t *cam, int sc, int ec, int sr, int er, char *reply)
{
  char cmdStr[64];
  int bc, br;

  // check parameters

  if (sc>0 && ec>0 && sr>0 && er>0) {
    sprintf(cmdStr,"mods.set_roi %d %d %d %d",sc,ec,sr,er);
    if (azcamCmd(cam,cmdStr,reply)<0)
      return -1;
  }

  // query and return

  strcpy(cmdStr,"mods.get_roi");
  if (azcamCmd(cam,cmdStr,reply)<0)
    return -1;

  sscanf(reply,"%d %d %d %d %d %d",&sc,&ec,&sr,&er,&bc,&br);
  sprintf(reply,"ROI=(%d,%d,%d,%d) XBIN=%d YBIN=%d",sc,ec,sr,er,bc,br);

  cam->firstCol = sc;
  cam->lastCol = ec;
  cam->firstRow = sr;
  cam->lastRow = er;
  cam->colBin = bc;
  cam->rowBin = br;
  
  return 0;

}

// get the ROI - get the current CCD ROI including binning

int
getROI(azcam_t *cam, char *reply)
{
  char cmdStr[64];
  int sc, ec, sr, er, bc, br;

  strcpy(cmdStr,"mods.get_roi");
  if (azcamCmd(cam,cmdStr,reply)<0)
    return -1;

  sscanf(reply,"%d %d %d %d %d %d",&sc,&ec,&sr,&er,&bc,&br);
  sprintf(reply,"ROI=(%d,%d,%d,%d) XBIN=%d YBIN=%d",sc,ec,sr,er,bc,br);

  // load into the cam struct
  
  cam->firstCol = sc;
  cam->lastCol = ec;
  cam->firstRow = sr;
  cam->lastRow = er;
  cam->colBin = bc;
  cam->rowBin = br;
  
  return 0;

}

// reset ROI - restore CCD to full-frame *unbinned* readout mode

int
resetROI(azcam_t *cam, char *reply)
{
  char cmdStr[64];
  int sc, ec, sr, er, bc, br;

  strcpy(cmdStr,"mods.reset_roi");
  if (azcamCmd(cam,cmdStr,reply)<0)
    return -1;

  // query and return

  strcpy(cmdStr,"mods.get_roi");
  if (azcamCmd(cam,cmdStr,reply)<0)
    return -1;

  sscanf(reply,"%d %d %d %d %d %d",&sc,&ec,&sr,&er,&bc,&br);
  sprintf(reply,"ROI=(%d,%d,%d,%d) XBIN=%d YBIN=%d",sc,ec,sr,er,bc,br);

  cam->firstCol = sc;
  cam->lastCol = ec;
  cam->firstRow = sr;
  cam->lastRow = er;
  cam->colBin = bc;
  cam->rowBin = br;
  
  return 0;
  
}

// set CCD binning

int
setCCDBin(azcam_t *cam, int xbin, int ybin, char *reply)
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
  sprintf(reply,"XBIN=%d YBIN=%d",xbin,ybin);

  cam->colBin = xbin;
  cam->rowBin = ybin;
  
  return 0;
    
}

// set CCD x (column) binning

int
setXBin(azcam_t *cam, int xbin, char *reply)
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
  sprintf(reply,"XBIN=%d YBIN=%d",xbin,ybin);

  cam->colBin = xbin;
  cam->rowBin = ybin;
  
  return 0;
    
}

// set CCD y (row) binning

int
setYBin(azcam_t *cam, int ybin, char *reply)
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
  sprintf(reply,"XBIN=%d YBIN=%d",xbin,ybin);

  cam->colBin = xbin;
  cam->rowBin = ybin;
  
  return 0;
    
}

/*!
  \brief Open the Shutter.
  
  \param cam pointer to an #azcam struct with the server parameters
  \param reply string to contain any reply text
  \return 0 if successful, -1 on errors, with error text in reply

  Instructs the azcam server to open the shutter.

  \sa closeShutter()
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

  \sa openShutter()
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
  \brief Query the azcam server for the current image format
  
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
getFormat(azcam_t *cam, char *reply)
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
  \brief Query the azcam server for the remaining exposure time
  
  \param cam pointer to an #azcam struct with the server parameters
  \param reply string to contain any reply text
  \return remaining exposure time in seconds or -1 if errors

  Query the azcam server and return the remaining exposure (integration)
  time in seconds.
*/

int
getTimeLeft(azcam_t *cam, char *reply)
{
  char cmdStr[64];

  if (cam->State == READOUT) return 0; 
  cam->timeLeft = 0.0;
  
  strcpy(cmdStr,"mods.timeleft");

  if (azcamCmd(cam,cmdStr,reply)<0)
    return -1;

  cam->timeLeft = atof(reply);
  
  sprintf(reply,"TimeLeft=%.3f sec",cam->timeLeft);
  return cam->timeLeft;

}

/*!
  \brief Query the azcam server for the pixels left to be readout
  
  \param cam pointer to an #azcam struct with the server parameters
  \param reply string to contain any reply text
  \return Count of the number of pixels readout if successful, -1 
          on errors, with error text in reply

  Queries the azcam server and returns the number of pixels readout from
  the detector array.  Repeated calls to getPixelCount() are often used
  by applications to monitor readout progress.  When the array is
  finished reading out, the pixel count returned should equal the total
  pixel size returned by getFormat().  After each call to
  getPixelCount(), it stores the current number of pixels read in
  #azcam::Nread.

  \sa getFormat()
*/

int
getPixLeft(azcam_t *cam, char *reply)
{
  char cmdStr[64];
  
  strcpy(cmdStr,"mods.pixelsLeft"); // Nread = Npixels - pixelsLeft...

  if (azcamCmd(cam,cmdStr,reply)<0)
    return -1;

  cam->pixLeft = atoi(reply);
  cam->Nread = cam->Npixels - cam->pixLeft;

  sprintf(reply,"PixelsLeft=%d",cam->pixLeft);
  return cam->pixLeft;

}

