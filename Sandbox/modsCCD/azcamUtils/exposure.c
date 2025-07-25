/*!
  \file exposure.c
  \brief azcam Exposure Control Functions

  The following are a suite of interface functions that provide access
  to all of the azcam server exposure control functions.

  Most function names recapitulate the analogous azcam server commands,
  while others serve as interface functions that provide access to
  server commands that provide more than one function (e.g., setMode)

  All routines call the communication layer routines in iosubs.c to take
  care of common handling of timeout, errors, and reply processing.

  \author R. Pogge, OSU Astronomy Dept. (pogge.1@osu.edu)
  \original 2005 May 17
  \date 2025 July 23
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
  exposure time used is the value in the #azcam::expTime data member.

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
  \brief Query the exposure status
  
  \return exposure state code (see azcam.h or azcam server docs)
  
  Queries the server and returns the current exposure status as
  an integer code.  Tranlsate this to a string and set the value
  of cam->State and also return it.
  
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
    
  return expCode;

}
  
/*!
  \brief Set the exposure (integration) time
  
  \param cam pointer to an #azcam struct with the server parameters
  \param exptime exposure time in decimal seconds
  \param reply string to contain any reply text
  \return 0 if successful, -1 on errors, with error text in reply

  Sets the exposure time for subsequent integrations.  The user provides
  the exposure time in seconds to millisecond precision (0.001 sec)

  If exptime = -1, it queries for the current exposure time and
  returns it.

*/

int
setExposure(azcam_t *cam, float exptime, char *reply)
{
  char cmdStr[64];
  int isSet;
  
  if (exptime < 0) {
    isSet = 0;
    strcpy(cmdStr,"mods.get_exptime");
  }
  else {
    isSet = 1;
    sprintf(cmdStr,"mods.set_exptime %f.3",exptime);
  }

  if (azcamCmd(cam,cmdStr,reply)<0)
    return -1;

  if isSet
    cam->expTime = exptime;
  else
    cam->expTime = atof(reply);
  
  sprintf(reply,"EXPTIME=%.3f sec",exptime);
  return 0;

}

/*!
  \brief Query the azcam server for the time left on the current exposure
  
  \param cam pointer to an #azcam struct with the server parameters
  \param reply string to contain any reply text
  \return elapsed exposure time in milliseconds, or -1 if errors,
          with error text in reply

  Query the azcam server and return the time remaining on the current
  exposure in seconds.

*/

int
timeLeft(azcam_t *cam, char *reply)
{
  char cmdStr[64];

  // don't read time left on exposure if we're reading out
  // report 0.0 seconds left
  
  if (cam->State == READOUT) {
    cam->timeLeft = 0.000;
    return 0;
  }

  // Ask server for time left
  
  strcpy(cmdStr,"mods.timeleft");

  if (azcamCmd(cam,cmdStr,reply)<0)
    return -1;

  cam->timeLeft = atof(reply);
  sprintf(reply,"timeLeft=%.3f sec",cam->timeLeft);
  return 0;

}

/*!
  \brief Abort an exposure in progress.
  
  \param cam pointer to an #azcam struct with the server parameters
  \param reply string to contain any reply text
  \return 0 if successful, -1 on errors, with error text in reply

  Issues an AbortExposure command to the azcam server and sets the API's
  ABORT flag (kept in the #azcam::Abort data member).  It also sets the
  server state flag to #IDLE (#azcam::State data member).

  Because of some odd hardware interactions that we have observed, an
  AbortExposure() command must be preceeded by PauseExposure().  We do
  not know if this is generic to all ARC controllers or just the ones
  we're working with at OSU.

  Applications calling this API should call AbortExposure() with some
  circumspection.

*/

int
abortExposure(azcam_t *cam, char *reply)
{
  char cmdStr[64];

  strcpy(cmdStr,"exposure.abort");

  if (azcamCmd(cam,cmdStr,reply)<0) {
    strcat(reply," - abortExposure Failed");
    return -1;
  }

  // success, set various abort flags as required...

  cam->Abort = 1;
  cam->State = IDLE;
  strcpy(reply,"Exposure Aborted");
  return 0;

}

/*!
  \brief Pause an exposure in progress.
  
  \param cam pointer to an #azcam struct with the server parameters
  \param reply string to contain any reply text
  \return 0 if successful, -1 on errors, with error text in reply

  Issues a PauseExposure command to the azcam server.  Sets the
  #azcam::State flag to #PAUSE on success.  If it fails, the
  #azcam::State flag is left unchanged.

  A PauseExposure should be followed by either AbortExposure() or
  ResumeExposure().

  We test the value of the #azcam::State flag and only send a
  PauseExposure command if #EXPOSING.  Sending a PauseExposure directive
  otherwise may be unpredictable.
  
  \sa AbortExposure(), ResumeExposure()
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

  Issues a ResumeExposure command to the azcam server.  Sets the
  #azcam::State flag to #EXPOSING on success.  If it fails, the
  #azcam::State flag is left unchanged.

  Does not send ResumeExposure if the controller is not in a #PAUSE
  state.

  If you send a ResumeExposure() command when the azcam server is
  not actually in a paused state, very bad things can happen (e.g.,
  it crashes and reboots its host computer).

  \sa PauseExposure()
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
  \param sc int, starting column
  \param ec int, ending column
  \param sr int, starting row
  \param er int, ending row
  \param reply string to contain any reply text
  \return 0 if successful, -1 on errors, with error text in reply

  Sets the region of interest for the next detector readout in 
  units of unbinned pixels.

*/

int
setROI(azcam_t *cam, int sc, int ec, int sr, int er, char *reply)
{
  char cmdStr[64];

  // azcam roi command includes bin factors as last two, -1 means "keep same"
  
  sprintf(cmdStr,"mods.set_roi %d %d %d %d -1 -1",sc,ec,sr,er);

  if (azcamCmd(cam,cmdStr,reply)<0)
    return -1;

  cam->FirstCol = sc;
  cam->LastCol = ec;
  cam->FirstRow = sr;
  cam->LastRow = er;
  
  // set flags as required...

  sprintf(reply,"ROI=(%d,%d,%d,%d) XBin=%d YBin=%d",
	  cam->FirstCol,cam->LastCol,
	  cam->FirstRow,cam->LastRow,
	  cam->ColBin,cam->RowBin);
	  
  return 0;

}

// getROI - to complicated to put into setROI like we do for others

int
getROI(azcam_t *cam, char *reply)
{
  char cmdStr[64];
  int sc, ec, sr, er, bx, by;
  
  strcpy(cmdStr,"mods.get_roi");

  if (azcamCmd(cam,cmdStr,reply)<0)
    return -1;

  // extract the data: sc ec sr er binx biny

  sscanf(reply,"%d %d %d %d %d %d",&sc,&ec,&sr,&er,&bx,&bn);
  cam->FirstCol = sc;
  cam->LastCol = ec;
  cam->FirstRow = sr;
  cam->LastRow = er;
  cam->ColBin = bx;
  cam->RowBin = by;

  sprintf(reply,"ROI=(%d,%d,%d,%d) XBin=%d YBin=%d",
	  cam->FirstCol,cam->LastCol,
	  cam->FirstRow,cam->LastRow,
	  cam->ColBin,cam->RowBin);
	  
  return 0;  

}

// reset ROI to full frame unbinned (used for ROI OFF)

int
resetROI(azcam_t *cam, char *reply)
{
  char cmdStr[64];
  int sc, ec, sr, er, bx, by;
  
  strcpy(cmdStr,"mods.reset_roi");

  if (azcamCmd(cam,cmdStr,reply)<0)
    return -1;

  // verify

  strcpy(cmdStr,"mods.get_roi");

  if (azcamCmd(cam,cmdStr,reply)<0)
    return -1;

  // extract the data: sc ec sr er binx biny

  sscanf(reply,"%d %d %d %d %d %d",&sc,&ec,&sr,&er,&bx,&bn);
  cam->FirstCol = sc;
  cam->LastCol = ec;
  cam->FirstRow = sr;
  cam->LastRow = er;
  cam->ColBin = bx;
  cam->RowBin = by;

  sprintf(reply,"ROI=(%d,%d,%d,%d) XBin=%d YBin=%d",
	  cam->FirstCol,cam->LastCol,
	  cam->FirstRow,cam->LastRow,
	  cam->ColBin,cam->RowBin);
	  
  return 0;  
  
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
  int nr_pdk;
  int nr_usc;
  int nr_osc;
  int nr_ft;
  int npix;

  strcpy(cmdStr,"exposure.get_format");
  memset(msgStr,0,sizeof(msgStr));

  if (azcamCmd(cam,cmdStr,msgStr)<0) {
    sprintf(reply,"Cannot get detector parameters - %s",msgStr);
    return -1;
  }

  // extract the data

  sscanf(msgStr,"%d %d %d %d %d %d %d %d %d",&ncols,&nc_pdk,&nc_usc,&nc_osc,&nrows,&nr_pdk,&nr_usc,&nr_osc,&nr_ft);

  npix = nrows * ncols;

  // store the results in the cam struct

  cam->Nrows = nrows;
  cam->Ncols = ncols;
  cam->Npixels = npix;
  sprintf(reply,"Ncols=%d Nrows=%d Npix=%d",nrows,ncols,npix);

  // store the format parameters in the azcam struct

  cam->NCtotal = ncols;
  cam->NCpredark = nc_pdk;
  cam->NCunderscan = nc_usc;
  cam->NCoverscan = nc_osc;
  cam->NRtotal = nrows;
  cam->NRpredark = nr_pdk;
  cam->NRunderscan = nr_usc;
  cam->NRoverscan = nr_osc;
  cam->NRframexfer = nr_ft;
  
  return npix;
 
}

/*!
  \brief Query the azcam server for the number of pixels readout
  
  \param cam pointer to an #azcam struct with the server parameters
  \param reply string to contain any reply text
  \return Count of the number of pixels readout if successful, -1 
          on errors, with error text in reply

  Queries the azcam server and returns the number of pixels readout from
  the detector array.  Repeated calls to pixelsLeft() are often used
  by applications to monitor readout progress.  When the array is
  finished reading out, the pixel count returned should equal the total
  pixel size returned by getDetPars().  After each call to
  pixelsLeft(), it stores the current number of pixels read in
  #azcam::Nread.

  Note that unlike readExposure(), experiments have so far shown that
  calling pixelsLeft() is benign in all circumstances.

  \sa getDetPars()
*/

int
pixelsLeft(azcam_t *cam, char *reply)
{
  char cmdStr[64];
  int pixcount;

  strcpy(cmdStr,"mods.pixelsLeft"); // pixread = numpix - pixelsLeft...

  if (azcamCmd(cam,cmdStr,reply)<0)
    return -1;

  pixcount = atoi(reply);
  sprintf(reply,"pixelsLeft=%d",pixcount);
  cam->Nread = pixcount; 
  return pixcount;

}

/*!
  \brief Set the image type and image title for the next image(s)
  
  \param cam pointer to an #azcam struct with the server parameters
  \param imgType string - image type, one of {object,bias,flat,dark,comp,zero}
  \param imgTitle string - image title (aka OBJECT)
  \param reply string to contain any reply text
  \return 0 on success, -1 on errors, error text in reply

  Tells the azcam server the image type and image title to use for the next
  image(s).  imgType must be one of the valid types, lowercase is enforced
  on the server side.  The shutter mode (light or dark) is defined internally
  on the server by the imgType.

  If either parameter is omitted, it uses the current value
  
 */

int
setImageInfo(azcam_t *cam, char *imgType, char *imgTitle, char *reply)
{
  char cmdStr[64];
  
  if (strlen(imgType)>0 && strlen(imgTitle)>0) {
    if (strlen(imgType)==0)
      strcpy(imgType,cam->imgType); // no imgType given, use current

    if (strlen(imgTitle)==0)
      strcpy(imgTitle,cam->imgTitle); // no imgTitle given, use current

    sprintf(cmdStr,"mods.set_imageInfo %s %s",imgType,imgTitle);

    if (azcamCmd(cam,cmdStr,reply)<0)
      return -1;
  }

  // get values from the server
  
  if (azcamCmd(cam,"exposure.get_image_type()",reply)<0)
    return -1;
  strcpy(cam->imgType,reply);
  
  if (azcamCmd(cam,"exposure.get_image_title()",reply)<0)
    return -1;
  strcpy(cam->imgTitle,reply);
    
  sprintf(reply,"IMGTYPE=%s Title=(%s)",cam->imgType,cam->imgTitle);
  
  return 0;
}
