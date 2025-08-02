/*!
  \file server.c
  \brief azcam Server Command Functions

  The following are a suite of interface functions that provide access
  to all of the azcam server control and database functions.

  Most function names recapitulate the analogous azcam server commands,
  while others serve as "meta" functions that encapsulate a number of
  AzCam commands under a single logical function.  
  
  All routines call the communication layer routines in iosubs.c to take 
  care of common handling of timeout, errors, and reply processing.

  \author R. Pogge, OSU Astronomy Dept. (pogge.1@osu.edu)
  \original 2005 May 17
  \date 2005 July 23
*/

#include "azcam.h" // AzCam client API header 

/*!
  \brief Set a FITS card in the azcam server's header database
  
  \param cam pointer to an #azcam struct with the server parameters
  \param keyword string with the FITS keyword to set
  \param value string with the data value associated with the keyword
  \param comment string describing the keyword
  \param reply string to contain any reply text
  \return 0 if successful, -1 on errors, with error text in reply

  Uploads a new FITS header parameter into the azcam server's FITS
  header database using the setKeyword command.  

  \sa clearKeywords(), getKeyword()
*/

int 
setKeyword(azcam_t *cam, char *keyword, char *value, char *comment, char *reply)
{
  char cmdStr[128];

  sprintf(cmdStr,"mods.set_keyword %s \'%s\' \'%s\'",keyword,value,comment);

  if (azcamCmd(cam,cmdStr,reply)<0)
    return -1;

  // success, set various flags as required...

  sprintf(reply,"%s=%s",keyword,value);
  return 0;

}


/*!
  \brief Get the value of a FITS card in the azcam server's header database
  
  \param cam pointer to an #azcam struct with the server parameters.
  \param keyword string with the keyword to retrieve
  \param value string with the keyword value
  \param reply string to contain any reply text
  \return 0 if successful, -1 on errors, with error text in reply

  Gets the value of a FITS header keyword from the azcam server's FITS
  header database, putting the result into the value string.  This
  function only returns the current keyword value, not the text of the
  comment associated with the keyword.  Unknown keywords generate an
  error.

  Note that the azcam server stores all user-defined header parameters
  as strings.  The calling application will need to translate that
  string into the appropriate data type (int, float, boolean, or string)
  as required.

  \sa setKeyword(), clearKeywords()

*/

int
getKeyword(azcam_t *cam, char *keyword, char *value, char *reply)
{
  char cmdStr[64];
  char msgStr[128];

  sprintf(cmdStr,"mods.get_keyword %s",keyword);

  if (azcamCmd(cam,cmdStr,msgStr)<0) {
    strcpy(reply,msgStr);
    return -1;
  }

  strcpy(value,msgStr);
  sprintf(reply,"%s=%s",keyword,value);

  return 0;

}

/*!
  \brief Clear the azcam server's FITS header database
  
  \param cam pointer to an #azcam struct with the server parameters
  \param dbName string with the database name to clear.  Default: 'exposure'
  \param reply string to contain any reply text.
  \return 0 if successful, -1 on errors, with error text in reply

  Clears all parameters from an azcam server FITS header keyword
  database. If no database is specified, it will assume the
  generic "exposure" database.  Alternatives are "telescope"
  and "instrument".

  \sa setKeyword(), getKeyword()
*/

int
clearKeywords(azcam_t *cam, char *dbName, char *reply) 
{
  char cmdStr[64];

  if (strlen(dbName)==0)
    strcpy(cmdStr,"mods.clearKeywords");
  else
    sprintf(cmdStr,"mods.clearKeywords %s",dbName);

  if (azcamCmd(cam,cmdStr,reply)<0)
    return -1;

  strcpy(reply,"Server FITS header database cleared");
  return 0;

}

