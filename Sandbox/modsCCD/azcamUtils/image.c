
/*!
  \file image.c
  \brief azcam Image Writing Command Functions

  The following are a suite of interface functions that provide access
  to all of the azcam server image writing functions.

  All routines call the communication layer routines in iosubs.c to take
  care of common handling of timeout, errors, and reply processing.

  \author R. Pogge, OSU Astronomy Dept. (pogge.1@osu.edu)
  \original 2005 May 17
  \date 2025 July 26 for MODS
*/

#include "azcam.h" // azcam client API header 

// imgFilename - set/get the next image file to be written
// empty string returns the current full name

int
imgFilename(azcam_t *cam, char *fileStr, char *reply)
{
  char cmdStr[64];
  
  if (strlen(fileStr) > 0) {
    sprintf(cmdStr,"mods.set_filename %s",fileStr);
    if (azcamCmd(cam,cmdStr,reply)<0)
      return -1;
  }

  // confirm
  
  strcpy(cmdStr,"mods.get_filename");
  if (azcamCmd(cam,cmdStr,reply)<0)
    return -1;

  strcpy(cam->fileName,reply);

  sprintf(reply,"filename=%s",cam->fileName);
  
  return 0;
}

// imgPath - set/get the azcam server image file path
// empty string returns the current path

int
imgPath(azcam_t *cam, char *pathStr, char *reply)
{
  char cmdStr[64];

  if (strlen(pathStr) > 0) {
    sprintf(cmdStr,"mods.set_path %s",pathStr);
    if (azcamCmd(cam,cmdStr,reply)<0)
      return -1;
  }

  // confirm
  
  strcpy(cmdStr,"mods.get_path");
  if (azcamCmd(cam,cmdStr,reply)<0)
    return -1;

  strcpy(cam->path,reply);

  sprintf(reply,"PATH=%s",cam->path);
  
  return 0;

}

// imgExpNum - set/get counter for the next image file
// argument <= 0 returns current expnum

int
imgExpNum(azcam_t *cam, int expNum, char *reply)
{
  char cmdStr[64];

  if (expNum >= 0) {
    sprintf(cmdStr,"mods.set_expnum %d",expNum);
    if (azcamCmd(cam,cmdStr,reply)<0)
      return -1;
  }

  // confirm (or query)
  
  strcpy(cmdStr,"mods.get_expnum");
  if (azcamCmd(cam,cmdStr,reply)<0)
      return -1;
  
  cam->fileNum = atoi(reply);

  sprintf(reply,"EXPNUM=%d",cam->fileNum);
  return 0;
}

// getLastFile - return name of the last file written

int
getLastFile(azcam_t *cam, char *reply)
{
  if (azcamCmd(cam,"mods.get_lastfile",reply)<0)
    return -1;

  strcpy(cam->lastFile,reply);
  
  sprintf(reply,"lastfile=%s",cam->lastFile);
  return 0;
}

/*!
  \brief Open/Close socket connections to various azcam server clients.
  
  \param cam pointer to an #azcam struct with the server parameters
  \param client client code, one of (#GUIDER_CLIENT, #GUIDER_CLOSE,
  #MESSAGE_CLIENT, #MESSAGE_CLOSE, #DISPLAY_CLIENT, #FILE_CLIENT).
  \param host string with the hostname of the target client
  \param port port number of the target client
  \param reply string to contain any reply text
  \return 0 if successful, -1 on errors, with error text in reply

  Instructs the azcam server to open a client connection socket to one
  of a set of possible remote clients.  For Guider and Message clients,
  there are also options to close that client socket.  Those clients
  designed to handle image data are sent image data using the
  SendImage() function.

  Supported client codes are:
  <pre>
    #GUIDER_CLIENT  - opens a connection to an autoguider client
    #GUIDER_CLOSE   - closes a connection to an autoguider client
    #MESSAGE_CLIENT - opens a connection to a message client (sees all status messages)
    #MESSAGE_CLOSE  - closes a connection to a messge client
    #DISPLAY_CLIENT - defines a display client (SendImage() will open/close socket)
    #FILE_CLIENT    - defines a file handler client (SendImage() will open/close socket)
  </pre>

  There are two classes of clients at work: persistent clients whose
  socket connections are opened or closed by this function (Guider and
  Message clients), and non-persistent clients that are opened/closed by
  the SendImage() function.

  \sa sendImage()
*/

int
setSocket(azcam_t *cam, int client, char *host, int port, char *reply)
{
  char cmdStr[64];

  switch(client) {
  case GUIDER_CLIENT:
    sprintf(cmdStr,"exposure.sendimage.set_remote_imageserver %s %d",host,port);
    break;

  case GUIDER_CLOSE:
    sprintf(cmdStr,"exposure.sendimage.set_remote_imageserver %s %d",host,port);
    break;

  case MESSAGE_CLIENT:
    sprintf(cmdStr,"exposure.sendimage.set_remote_imageserver %s %d",host,port);
    break;

  case MESSAGE_CLOSE:
    sprintf(cmdStr,"exposure.sendimage.set_remote_imageserver %s %d",host,port);
    break;

  case DISPLAY_CLIENT:
    sprintf(cmdStr,"exposure.sendimage.set_remote_imageserver %s %d",host,port);
    break;

  case FILE_CLIENT:
    sprintf(cmdStr,"exposure.sendimage.set_remote_imageserver %s %d",host,port);
    break;

  default:
    sprintf(reply,"Unknown socket client code: %d",client);
    return -1;  // unknown client type
    break;
  }

  if (azcamCmd(cam,cmdStr,reply)<0)
    return -1;

  // success, set various flags as required...

  return 0;

}

/*!
  \brief Write an image from memory to disk
  
  \param cam pointer to an #azcam struct with the server parameters
  \param filename full path and name of the file to create.
  \param reply string to contain any reply text
  \return 0 if successful, -1 on errors, with error text in reply

  Writes the contents of the azcam server's internal image buffer to a
  disk file.  This function is issued after readout has finished.  If
  the image is a multi-detector mosaic, the format code is ignored and
  the image is written as a multi-extension FITS (MEF) format image
  (same as format=#MEF) as indicated by the #azcam::fileFormat
  value.

  The filename provided must be the full name of the file including all
  counters, paths, etc.  The calling application is responsible for
  building a full valid filename; this function cannot do validation.
  The path must be a valid file path on the azcam server proper.
  
  On successful completion, this function sets the #azcam::LastFile
  datum to tell the calling application the name of the file just
  written.  It does not, however advance file counters.  That is the
  responsibility of the calling application (since counters are
  optional).

*/

int
writeImage(azcam_t *cam, char *filename, char *reply)
{
  char cmdStr[128];
  char msgStr[256];

  switch(cam->fileFormat) {
  case STDFITS:
    sprintf(cmdStr,"exposure.image.write_file %s 0",filename);
    break;

  case MEF:
    sprintf(cmdStr,"exposure.image.write_file %s 1",filename);
    break;

  case BINARY:
    sprintf(cmdStr,"exposure.image.write_file %s 2",filename);
    break;

  default:
    sprintf(reply,"Unknown file format code: %d",cam->fileFormat);
    return -1; // unknown format
    break;
  }

  memset(msgStr,0,sizeof(msgStr));
  if (azcamCmd(cam,cmdStr,msgStr)<0) {
    strcpy(reply,msgStr);
    return -1;
  }

  // success, set various flags as required...

  strcpy(cam->lastFile,filename);
  sprintf(reply,"LastFile=%s",filename);
  return 0;

}

/*!
  \brief Send the image to remote file server or client
  
  \param cam pointer to an #azcam struct with the server parameters
  \param localFile name of the local file to send
  \param remoteFile name for the remote file system
  \param reply string to contain any reply text
  \return 0 if successful, -1 on errors, with error text in reply

  Sends the current image in memory to the specified client.  

  If client is #DISPLAY_CLIENT, the azcam server opens the client
  socket, sends the data, then closes the client socket.

  If client is #FILE_CLIENT, the azcam server opens the client socket,
  sends the complete image (header+data), then closes the client socket.

  If client is #GUIDER_CLIENT, the azcam server sends the image data
  to the client.  The azcam server does not first open the guider
  client, this must be done with a previous call to SetSocket().

*/

int
sendImage(azcam_t *cam, char *localFile, char *remoteFile, char *reply)
{
  char cmdStr[64];

  sprintf(cmdStr,"exposure.sendimage.send_image %s %s",localFile,remoteFile);

  if (azcamCmd(cam,cmdStr,reply)<0)
    return -1;

  // success, set various flags as required...

  sprintf(reply,"Image %s sent to remote server as %s",localFile,remoteFile);
  return 0;

}
