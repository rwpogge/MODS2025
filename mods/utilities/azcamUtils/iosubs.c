/*!
  \file iosubs.c
  \brief azxam client API, server I/O utilities

  A set of basic TCP socket I/O routines that define the common
  communications layer for the azcam client API.

  \author R. Pogge, OSU Astronomy Dept (pogge.1@osu.edu)
  \date 2025 July 23
  (original 2005)

  Last Update: 2025 Aug 3 [rwp/osu]
*/

#include "azcam.h" // All the header we should need

/*!
  \brief Open a TCP socket connection to an azcam server
  \param cam pointer to an #azcam struct with the azcam server parameters
  \param reply string to carry any messages from the open process
  \return File descriptor of the open comm port connection, 
  or -1 if an error, with any error message text in reply.

  Performs the necessary initialization functions to open a TCP socket
  connection to an azcam server.  To use, you must fill in the relevant
  data members of the #azcam struct.  The cam->Host and cam->Port
  members are used to define the server TCP address.

  For socket communications we use INET streams (SOCK_STREAM) with a
  persistent client connection.  

  \sa closeAzCam()
*/

int
openAzCam(azcam_t *cam, char *reply)
{ 
  int portFD = -1;

  // sockaddr and hostent structs for the TCP address of the azcam server

  char hostID[64];
  int portID;
  struct sockaddr_in PortServer;
  int PortServer_len;
  struct hostent *camHost;

  // The server hostname better be plausible

  if (strlen(cam->Host)<=0) {
    sprintf(reply,"azcam server hostname NULL!\n");
    return -1;
  }

  strcpy(hostID,cam->Host);
  portID = cam->Port;

  // Try to resolve the network port server's host ID.  If not resolvable
  // we're screwed - abort with informative error messages.

  if (!(camHost=gethostbyname(hostID))) {
    sprintf(reply,"Cannot resolve azcam server hostname %s - %s\n",
	   hostID,hstrerror(h_errno));
    return -1;
  }

  // Initialize the socket port attributes

  if (cam->FD > 0) {
    close(cam->FD);
    cam->FD = -1;  // the no-port value
  }

  // Build the sockaddr database: protocol family is AF_INET

  PortServer.sin_family = AF_INET;
  memcpy(&PortServer.sin_addr,camHost->h_addr, camHost->h_length); 
  PortServer.sin_port = htons(portID);

  // Get a TCP socket configured for INET streams

  portFD = socket(AF_INET,SOCK_STREAM,0);
  if (portFD < 0) {
    sprintf(reply,"Cannot open socket for azcam server %s:%d - %s\n",
	   hostID,portID,strerror(errno));
    return -1;
  }

  // Connect to the network serial port server

  if (connect(portFD,(struct sockaddr *) &PortServer, sizeof(PortServer))<0) {
    sprintf(reply,"Cannot connect to azcam server %s:%d - %s\n",
	   hostID,portID,strerror(errno));
    close(portFD);
    return -1;
  }
  
  // Make sure this port is non-blocking

  fcntl(portFD,F_SETFL,O_NONBLOCK);

  // Success: return the file descriptor of the open socket
  
  cam->FD = portFD;

  return 0;

}

/*!
  \brief Terminate an azcam client session and close the server socket

  \param cam pointer to an #azcam struct with the azcam server parameters
  (see openAzCam())
  
  Sends the "closeConnection" command to the azcam server and closes the
  client TCP socket.

  \sa openAzCam().

*/

void
closeAzCam(azcam_t *cam) 
{
  char cmdStr[128];
  char reply[128];

  // Send the info to close the azcam server connection

  // closeConnection(cam,reply); // nothing equivalent for Archon

  // Close the client-side socket and reset the file descriptor

  if (cam->FD > 0)
    close(cam->FD);

  cam->FD = -1;

}
  
/*!
  \brief Send data to an azcam server TCP socket

  \param cam pointer to an #azcam struct with the azcam server parameters
  \param cmdStr message string to write
  \return Number of bytes written if successful, 0 or -1 if unsuccessful.

  Sends the message string provided to the azcam server described by the
  cam struct.  An azcam client session must have been previously initiated
  using the openAzCam() function.

  On errors, it returns an error message in cmdStr.

  \sa readAzCam()
*/

int  
sendAzCam(azcam_t *cam, char *cmdStr)
{
  int nsent = 0;

  nsent = write(cam->FD,cmdStr,strlen(cmdStr));
  if (nsent < 0) 
    sprintf(cmdStr,"ERROR(sendAzCam()) - Cannot send to TCP azcam server %s:%d - %s\n",
	   cam->Host,cam->Port,strerror(errno));
  return nsent;
}

/*!
  \brief Read data from an azcam server TCP socket 

  \param cam Pointer to an #azcam struct with the azcam server parameters
  \param msgStr Message string to carry the input string
  \return The number of characters read, or <0 if an error.  -1 on error
  or timeout, with \e msgStr containing the error message text.

  Uses select() to read data from the specified comm port with the
  timeout interval specified.  Note that because TCP sockets are streams
  rather than line-buffered, the data can arive in bursts depending on
  the system state and the degree of stream synchronization.  As such we
  read in chunks of data w/o blocking until we have read everything from
  the port, which means looking for the \\r (ASCII 13 = Ctrl+M) or \\n
  terminator.  This makes the logic tricky, but robust against most comm
  glitches.  Use of a timeout allows us to break out of cases where the
  message is unterminated because of a comm fault, not because of the
  usual stream buffering/sync issues.

  \note 
  If select() is interrupted by Ctrl+C, it returns an error message in the
  \e reply string.

*/

int  
readAzCam(azcam_t *cam, char *msgStr)
{
  int keepReading;
  char inbuf[256];  // working buffer
  int lastchar;

  // for select()

  fd_set readfds;     
  int nready;
  struct timeval tv;

  // timeout interval is given in seconds.  if <0 or 0, set to 0, which
  // means we are polling the port

  if (cam->Timeout <= 0L) {
    tv.tv_sec = 0;
    tv.tv_usec = 0;
  }
  else {
    tv.tv_sec = cam->Timeout;
    tv.tv_usec = 0;
  }

  memset(inbuf,0,sizeof(inbuf));

  keepReading = 1;

  while (keepReading) {

    // we use a select() call to enable read with timeout

    FD_ZERO(&readfds);  // clear file descriptors
    FD_SET(cam->FD,&readfds);

    // select() returns <0 if interrupted by a signal before the timeout
    // interval has expired, or if an error occurs, otherwise it returns
    // 0 when a timeout occurs, or >0 if it got something

    nready = select(cam->FD+1,&readfds,(fd_set *)NULL, (fd_set *)NULL, &tv);

    if (nready < 0) {
      if (errno == EINTR) { // got a Ctrl+C interrupt
	sprintf(msgStr,"(readAzCam) Socket %s:%d read aborted Ctrl+C",
		cam->Host,cam->Port);
	return -1;
      }
      sprintf(msgStr,"(readAzCam) Socket %s:%d read select() error - %s",
	      cam->Host,cam->Port,strerror(errno));
      return -1;
    }
    else if (nready == 0 && cam->Timeout > 0L) {
      sprintf(msgStr,"(readAzCam) Socket %s:%d read timed out after %d sec",
	      cam->Host,cam->Port,cam->Timeout);
      return -1;
    }
    else { 
      if (FD_ISSET(cam->FD,&readfds)) {
	memset(inbuf,0,sizeof(inbuf));
	if (read(cam->FD,inbuf,sizeof(inbuf))<0) {
	  sprintf(msgStr,"(readAzCam) Cannot read socket %s:%d - %s",
		  cam->Host,cam->Port,strerror(errno));
	  return -1;
	}
	
	// Now the fun bit, this is a stream terminated by either
	// \r or \n, and since we're not line-buffered, we have to buffer
	// ourselves on account of lack of stream synch etc.

	if (strlen(inbuf) > 0) {
	  strcat(msgStr,inbuf);  
	  lastchar = strlen(msgStr)-1;
	  // if the last char is \r or \n the string is complete then
	  // null terminate & return
	  if (msgStr[lastchar]=='\r' || msgStr[lastchar]=='\n') {
	    msgStr[lastchar]='\0'; 
	    return strlen(msgStr);
	  }
	}
      }
    }

  } // select() event loop

  return 0;

}

/*!
  \brief Send a command to the azcam server and await a reply

  \param cam pointer to an #azcam struct with the azcam server parameters
  \param cmdStr command to send
  \param reply  string returned by the azcam server
  \return Number of bytes written if successful, 0 or -1 if unsuccessful.

  Sends a command to the azcam server and waits for a reply up to a
  specified timeout interval (defined by #azcam::timeout).  On reply,
  the message is parsed into components.  A successful command receives
  a reply prefixed by "OK" followed (most times) by a message string
  with information (e.g., the parameters requested).  Errors are
  prefixed by "ERROR", followed by error message text.  This routine
  does no processing of the return reply, but instead packages it as a
  string (with the OK/ERROR tag removed) for further handling by the
  calling routine.
 
  \sa sendAzCam(), readAzCam()
*/

int  
azcamCmd(azcam_t *cam, char *cmdStr, char *reply)
{
  int nsent = 0;
  char msgStr[256];
  char status[32];
  char msgBody[256];
  int sl;
  
  // terminate the command string with \n and send it.  We do not validate
  // commands at this stage.  If the azcam server doesn't like the command,
  // it will complain

  if (cmdStr[strlen(cmdStr)-1] != '\n') strcat(cmdStr,"\n");

  if (sendAzCam(cam,cmdStr)<0) {
    sprintf(reply,"ERROR: %s",cmdStr);
    return -1;
  }

  // Now wait timeout for a reply

  memset(msgStr,0,sizeof(msgStr));

  if (readAzCam(cam,msgStr)<0) { 
    strcpy(reply,msgStr);
    return -1;
  }

  // Split the reply into components and check for errors

  sscanf(msgStr,"%s %[^\n]",status,msgBody);

  // strip off any residual termination, \n or \r

  // if (msgBody[strlen(msgBody)-1] == '\n') msgBody[strlen(msgBody-1)] = '\0';

  msgBody[strcspn(msgBody, "\r\n")] = 0;
  
  // if (msgBody[strlen(msgBody)-1] == '\r') msgBody[strlen(msgBody-1)] = '\0';

  // What we do depends on the value of status

  if (strcasecmp(status,"OK")==0) {
    strcpy(reply,msgBody);
    return 0;
  }

  if (strcasecmp(status,"ERROR")==0 || strcasecmp(status,"ERROR:")==0) {
    sprintf(reply,"AZCAM ERROR %s",msgBody);
    return -1;
  }

  // We got something weird, send back as-is for debugging purposes

  return -1;

}

