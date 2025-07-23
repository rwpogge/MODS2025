/*!
  \file ccdtemp.c
  \brief azcam Server CCD Temperature Control Functions

  The following are a suite of interface functions that provide access
  to all of the azcam server temperature control commands.

  All routines call the communication layer routines in iosubs.c to take
  care of common handling of timeout, errors, and reply processing.

  \author R. Pogge, OSU Astronomy Dept. (pogge.1@osu.edu)
  \date 2025 July 23
  \orginal 2005 May 17
*/

#include "azcam.h" // azcam client API header 

/*!
  \brief Get the Detector and Dewar temperatures
  
  \param cam pointer to an #azcam struct with the server parameters
  \param reply string to contain any reply text
  \return 0 if successful, -1 on errors, with error text in reply.
  Temperature values are in #azcam::CCDTemp and #azcam::DewarTemp.

  Queries the azcam server and readout out the detector and dewar
  temperatures in degrees C, storing the results in the #azcam::CCDTemp
  and #azcam::DewarTemp data members.

  \sa setTemp(), setTempCal()
*/

int
getTemp(azcam_t *cam, char *reply)
{
  char cmdstr[16];
  char msgstr[64];
  float t1, t2;

  memset(cmdstr,0,sizeof(cmdstr));
  strcpy(cmdstr,"mods.ccdTemps");

  if (azcamCmd(cam,cmdstr,msgstr)<0) {
    strcpy(reply,msgstr);
    return -1;
  }

  // process the message string
  
  if (sscanf(msgstr,"%f %f",&t1,&t2) == 2) {
    cam->ccdTemp = t1;
    cam->baseTemp = t2;
    sprintf(reply,"CCDTEMP=%.1f BASETEMP=%.1f",t1,t2);
    return 0;
  }
  else { // got something weird, treat as an error
    strcpy(reply,msgstr);
    return -1;
  }

}

/*!
  \brief Set the CCD temperature set point
  
  \param cam pointer to an #azcam struct with the server parameters
  \param temp CCD detector set point in degrees Celsius
  \param reply string to contain any reply text
  \return 0 if successful, -1 on errors, with error text in reply

  Sets the target detector temperature setpoint in degrees C.  Temperatures
  may be given to the nearest 0.1C.

  The new setpoint temperature is stored in #azcam::setPoint data member
  for use by the application.

  setTemp implements the set_control_temperature server command.  The name
  was shortened to match the syntax of the related getTemp command

  \sa getTemp()
*/

int
setTemp(azcam_t *cam, float temp, char *reply)
{
  char cmdstr[64];

  sprintf(cmdstr,"mods.set_CCDsetPoint %.1f",temp);

  if (azcamCmd(cam,cmdstr,reply)<0)
    return -1;

  // success, set various flags as required...

  cam->setPoint = temp;
  sprintf(reply,"SETPOINT=%.1f",temp);
  return 0;

}
