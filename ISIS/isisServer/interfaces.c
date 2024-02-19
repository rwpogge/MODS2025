//
// interfaces.c - ISIS server interface routines
//
// Contents:
//    openSocket()  - Open and setup the ISIS server UDP network socket
//    closeSocket() - Close the ISIS server UDP network socket
//    initSerialPorts() - Open/Initialize server serial ports
//    getSerialInfo()   - Map a serial port fd onto a serial port table entry
//    handShake()       - handshake with the serial ports
//    serialHandler()   - handle messages from serial ports
//    socketHandler()   - handle messages from the server network socket
//    ttyHandler()      - keyboard input from the server console
//
// Author: 
//   R. Pogge, OSU Astronomy Dept.
//   pogge@astronomy.ohio-state.edu
//   2003 January 7
//
// Modification History:
//   2004 May 11 - introduced Doxygen documentation comment blocks [rwp/osu]
//   2009 Mar 18 - v2 updates [rwp/osu]
//

#include "isisserver.h"

/*!
  \file interfaces.c
  \brief ISIS server interface routines
  
  ... description of the ISIS server interfaces ...

  Three basic routines are provided for handling input message traffic 
  on the various server interfaces:
  \arg serialHandler() Server serial port input input handler
  \arg socketHandler() Server UDP network socket input handler
  \arg ttyHandler() Keyboard input at the server console (readline callback)
  These handlers are designed to be called from within the select()
  communications multiplexing loop when input is available on the
  watched file descriptors.

*/


//---------------------------------------------------------------------------
//
// openSocket()
//

/*!
  \brief Open and setup the ISIS server UDP network socket

  \param ISISPort Port number on localhost for the server socket.
  \return 0 if successful, -1 if errors occurred.

  Creates a UDP socket for the ISIS server applications and sets the
  file descriptor in the system table.  It then binds the socket to the
  named port on localhost, and stores relevant information in the ISIS
  system table for future use without having to use thread-unsafe getxxx
  calls.  

  Errors are reported to stdout by this routine.

  \sa closeSocket()
*/

int
openSocket(int isisPort)
{
  struct sockaddr_in server;
  int server_len;
  struct hostent *hostID;
  char errStr[256];
  char inetStr[INET6_ADDRSTRLEN];

  // Create a server socket and get the file descriptor 

  if ((isis.sockFD = socket(AF_INET,SOCK_DGRAM, 0)) < 0) {
    sprintf(errStr,"ERROR: Could not create ISIS server socket - %s",
	    strerror(errno));
    if (isis.useCLI)
      printf("%s\n",errStr);
    else
      logMessage(errStr);
    return(-1);
  }

  // Setup the server protocol and address info 

  server.sin_family = AF_INET;
  server.sin_addr.s_addr = htonl(INADDR_ANY) ;  // use local host 

  if (isisPort <= 0) // Port 0 means allocate the next free port 
    isisPort = 0;  
  server.sin_port = htons(isisPort) ;

  // Bind the socket to the port 

  if (bind(isis.sockFD,(struct sockaddr *) &server, sizeof(server)) < 0) {
    sprintf(errStr,"ERROR: Could not bind server socket to port %d - %s",
	    ntohs(server.sin_port),strerror(errno));
    if (isis.useCLI)
      printf("%s\n",errStr);
    else
      logMessage(errStr);
    return(-1);
  }

  // Get some socket ID information 

  server_len=sizeof(server);
  if (getsockname(isis.sockFD, 
		  (struct sockaddr *) &server, 
		  (socklen_t *) &server_len) < 0) {
    sprintf(errStr,"ERROR: could not get socket name info - %s",
	    strerror(errno));
    if (isis.useCLI)
      printf("%s\n",errStr);
    else
      logMessage(errStr);
    return(-1);
  }

  // Store the relevant server information in the systab struct 

  gethostname(isis.localHost,sizeof(isis.localHost));
  hostID = gethostbyname(isis.localHost);

  /*
   * This stuff is toxic - it uses deprecated functions (fail on CentOS)
   *
  sprintf(isis.hostName,"%s",hostID->h_name);
  sprintf(isis.sockAddr,"%s",inet_ntop(AF_INET,hostID->h_addr_list[0],
				       inetStr,sizeof(inetStr)));

  */
  isis.sockPort = ntohs(server.sin_port);

  // Record the server starting time 

  getUTCTime();
  sprintf(isis.startTime,"%s %s UTC",isis.utcDate,isis.utcTime);

  // All done, signal success 

  return(0);

}

//---------------------------------------------------------------------------
//
// closeSocket()
//

/*!
  \brief Close the ISIS server socket.
  
  \param sockFD File descriptor of the open ISIS server socket (see openSocket())

  Tears down the ISIS server socket and releases the file descriptor.
  Currently this is little more than a convenience wrapper for close(),
  but someday it might do more.

  \sa openSocket()
*/

void
closeSocket(int sockFD)
{
  close(sockFD);

  return;
}

//---------------------------------------------------------------------------
//
// initSerialPorts()
//

/*!
  \brief Initialize the serial ports listed in the server's serial port table
  \return Number of serial ports opened successfully, <0 if any failed.

  Initializes all of the serial ports listed in the server's serial port
  table (ttyTab struct, defined by entries in the runtime config file).
  It also attempts to close them first, allowing this function to also
  be used to re-initialize the ports.  One failure is enough to cause an
  abort.

*/

int
initSerialPorts()
{
  struct termios tty;  // Port configuration structure 
  int i;
  int status;
  int numOpen = 0;
  char errStr[256];

  // If there are no serial ports in the table, return now 

  if (isis.numSerial == 0) 
    return(0);

  // Check to see if any of the serial port file handles are open
  // and close them now.  This lets us use InitSerial to re-init ports.
  // In any case, set the port fd's to -1 to mark as disabled.
  
  for (i=0; i<isis.numSerial; i++) {
    if (ttyTab[i].fd > 0) 
      status = close(ttyTab[i].fd);
    ttyTab[i].fd = -1;
  }

  // Attempt to open the ports and set the attributes 

  for (i=0; i<isis.numSerial; i++) {
    
    ttyTab[i].fd = open(ttyTab[i].devName, O_RDWR|O_NDELAY);

    if (ttyTab[i].fd < 0) {
      sprintf(errStr,"ERROR: Could not open serial port %s - %s", 
	      ttyTab[i].devName,strerror(errno));
      if (isis.useCLI)
	printf("%s\n",errStr);
      else
	logMessage(errStr);
      return(-1);
    }
    tcgetattr(ttyTab[i].fd, &tty);
    tty.c_iflag &= ~ISTRIP;     
    tty.c_lflag |= ICANON;
    tty.c_lflag &= ~ECHO;
    tty.c_cflag |= CS8;
    tty.c_cflag |= CREAD;
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~PARENB;
    tty.c_cc[VMIN] = 1;
    tty.c_cc[VTIME] = 0;
    cfsetispeed(&tty, (speed_t) B9600);
    cfsetospeed(&tty, (speed_t) B9600);
    tcflush(ttyTab[i].fd, TCIFLUSH);
    tcsetattr(ttyTab[i].fd, TCSAFLUSH, &tty);
    numOpen++;
    
  } // isis.numSerial loop 

  return(numOpen); // Return the number of open ports 

}

//---------------------------------------------------------------------------
//
// getSerialInfo() 
//

/*!
  \brief Map a serial port file descriptor onto a serial port table index.
  \param testFD Serial port file descriptor to map
  \return The serial port table index of the test file descriptor
  if present, otherwise it returns ERR_UNKNOWN_FD if no match is found.

  Searches for \c testFD in the server's active serial port table, and
  returns the index of the port table entry if found.  The serial port
  table is the ttyTab struct defined in the isisserver.h header file.
*/

int 
getSerialInfo(int testFD)
{
  int i;

  // Search for the file descriptor in the serial port table
  
  for (i=0; i<MAXSERIAL; i++) {
    if (testFD == ttyTab[i].fd)
      return (i);
  }

  return(ERR_UNKNOWN_FD);

}

//---------------------------------------------------------------------------
//
// handShake()
// 

/*!
  \brief Handshake with the clients on serial ports and preset UDP ports.

  Sends a ">AL PING" message to all open serial ports and all preset
  UDP ports.  This is a blind broadcast that should lead to "PONG"s
  back from any ISIS clients on the serial lines or connected to the
  UDP socket ports.

  This is a placeholder that gives us minimal IMPv2 software
  handshaking until we can work with the system some and work out
  better procedures.

  All strings transmitted to raw ports must be in proper
  IMPv2-conformal format, terminated by \\r (ASCII 13).
*/

void
handShake()
{
  char message[MED_STR_SIZE];
  int iPort; 
  int serialFD;
  int bytesSent;
  struct sockaddr_in client;  // network client socket info   
  int clientLen;       
  int numSent;
  char errStr[256];

  // If there are no serial ports in the table, return now 

  if (isis.numSerial == 0 && isis.numPreset == 0) {
    if (isis.useCLI)
      printf("No open serial ports or preset UDP socket ports defined, no initial handshaking done.\n");
    else
      logMessage("NOTICE: No open serial ports or preset UDP socket ports defined"
		 ", no initial handshaking done.");
    return;
  }

  // Build the broadcast message (properly terminated) 

  memset(message,0,sizeof(message));
  sprintf(message,"%s>AL PING\r",isis.serverID);

  // Send it out on all serial ports 

  for (iPort=0; iPort<isis.numSerial; iPort++) {
    serialFD = ttyTab[iPort].fd;
    if (serialFD > 0) 
      tcflush(serialFD,TCIOFLUSH);
    bytesSent = write(serialFD,message,strlen(message));
    if (bytesSent < 0) {
      sprintf(errStr,"ERROR: Serial Port write() Error on %s - %s\n",
	      ttyTab[iPort].devName,strerror(errno));
      if (isis.useCLI)
	printf("%s\n",errStr);
      else
	logMessage(errStr);
    }

  } // end of serial port loop 

  // Now send it out on all preset UDP socket ports

  if (isis.numPreset > 0) {
    for (iPort=0; iPort<isis.numPreset; iPort++) {
      client.sin_family = AF_INET;
      if (inet_pton(AF_INET,udpTab[iPort].host,&client.sin_addr)<=0) {
	sprintf(errStr,"ERROR: Preset UDP socket port %d has invalid host IPAddress '%s'",
		iPort,udpTab[iPort].host);
	if (isis.useCLI)
	  printf("%s\n",errStr);
	else
	  logMessage(errStr);
      }
      else {
	client.sin_port = htons(udpTab[iPort].port);
	clientLen = sizeof(client);
	numSent = sendto(isis.sockFD,message,strlen(message),0, 
			 (struct sockaddr *) &client, clientLen);
	if (numSent < strlen(message)) {
	  sprintf(errStr,"ERROR: Preset UDP Port write() Error to %s:%d - %s\n",
		  udpTab[iPort].host,udpTab[iPort].port,strerror(errno));
	  if (isis.useCLI)
	    printf("%s\n",errStr);
	  else
	    logMessage(errStr);
	}
      }
    }
  }
  
  return;

}

//---------------------------------------------------------------------------
//
// serialHandler()
//

/*!
  \brief Handle input from server serial ports.
  \param serialFD File descriptor of the serial port with input ready.

  Reads a line of input from the specified serial port file descriptor,
  and spits the message into components: source host ID, destination
  host ID, and message body (including message type).

  If the source and destination are valid, and the message is not blank
  or malformed, it then passes the message off to HandleMessage() to
  route the message to its intended recipient.

  When generating IMPv2 messages for direct transmission through the
  server, the strings must be terminated with return (\\r = ASCII 13)
  not a newline (\\n = ASCII 10).

  This function is designed to be invoked by a select() loop that watches
  particular file descriptors.  See main.c for how it is implemented.
*/

void
serialHandler(int serialFD) 
{
  char srcID[IMPv2_HOST_SIZE];
  char destID[IMPv2_HOST_SIZE];
  char argStr[MED_STR_SIZE];
  char *reply;
  char *msgBody;

  int sendHost;   // index of the srcID in the host table                    
  int destHost;   // index of the destID in the host table                   
  int numBytes;   // number of bytes read from the serial port          
  int iPort;      // index of the serial port in the serial table            

  // Dynamically allocate the message string buffer

  char *message = (char *)malloc((ISIS_MSGSIZE)*sizeof(char));

  // Clear it 

  memset(message, 0, ISIS_MSGSIZE);
 
  // Allocate a reply buffer now (needed to log errors) 

  reply = (char *)malloc((ISIS_MSGSIZE)*sizeof(char));
  memset(reply, 0, ISIS_MSGSIZE);

  // Get information about this serial port for later.  If we fail
  // something evil is happening, so squawk, log and exit with a fatal
  // error.  
  
  iPort = getSerialInfo(serialFD);
  if (iPort == ERR_UNKNOWN_FD) {
    sprintf(reply,"ERROR Unknown serial FD %d, serial table corrupted?",
	    serialFD);
    if (isis.useCLI) printf("\n%s\n",reply);
    logMessage(reply);
    free(reply);
    free(message);
    return;
  }    

  // read the message from the serial port 

  numBytes = read(serialFD, message, ISIS_MSGSIZE);

  // read error: report back to the server console, free buffers, and return 

  if (numBytes < 0) {
    sprintf(reply,"ERROR cannot read() from serial port %s - %s",
	    ttyTab[iPort].devName,strerror(errno));
    if (isis.useCLI) printf("\n%s\n",reply);
    logMessage(reply);
    free(reply);
    free(message);
    return;
  }

  // Allocate the message body buffer 

  msgBody = (char *)malloc((ISIS_MSGSIZE)*sizeof(char));
  memset(msgBody,0,ISIS_MSGSIZE);

  // strip off any \n or \r terminators at the end of the message string 

  if (message[strlen(message)-1]=='\n') message[strlen(message)-1]='\0';
  if (message[strlen(message)-1]=='\r') message[strlen(message)-1]='\0';

  // Extract the first token from the message, this should be the the
  // address header of the IMPv2 message in "SourceID>DestID" form

  getArg(message,1,argStr);
  
  // Check to see if there is a ">" in the first token.  If present,
  // break it into components.

  if (strstr(argStr,">")) {
    sscanf(message,"%[^>]>%s %[^\n]",srcID,destID,msgBody);
    upperCase(srcID);
    upperCase(destID);

  }
  else {
    // Malformed message: move on and don't bother logging except to
    // give a warning on the console in CLI mode
    sprintf(reply,"ERROR: Malformed message received on serial port %s: '%s' [size: %d bytes]",
	    ttyTab[iPort].devName,message,strlen(message));
    
    if (isis.useCLI) printf("\n%s\n",reply);
    free(message);
    free(reply);
    free(msgBody);
    return;
  }

  // We are all done with the raw message, and from here on out we only
  // work with its components.  Log it, echo it to the screen if in
  // verbose mode, and then free the incoming message memory now so
  // we don't forget later (leak abatement).  We only log non-blank
  // messages.
   
  if (isis.isVerbose && isis.useCLI) printf("\n%s\n",message);
  logMessage(message);
  free(message);

  // We have an IMPv2-conformal message string and its components:
  //   srcID = hostID of the sender
  //   destID = hostID if the intended recipient
  //   msgBody = message test to be delivered to destID
  // Now we deal with it.

  // Update the host table with the srcID client information 
  
  sendHost = updateHosts(srcID,SERIAL,serialFD,0,iPort);

  // If the host table is full, we got problems.  Hit the console screen
  // and runtime log with an error message, try to echo one back to the
  // sender port, clean up, and return.

  if (sendHost == ERR_HOSTS_FULL) {
    sprintf(reply,
    "ERROR: ISIS Host Table Full, cannot create entry for tty port client %s",srcID);
    if (isis.useCLI) printf("\n%s\n",reply);
    logMessage(reply);
    memset(reply,0,ISIS_MSGSIZE);
    sprintf(reply,
	    "%s>%s ERROR: ISIS Server Host Table Full, Cannot Service Request\r",
	    isis.serverID,srcID);
    numBytes = write(serialFD, reply, sizeof(reply));
    free(reply);
    free(msgBody);
    return;
  }    

  // If msgBody is blank, make note of it but take no further action.  This
  // is a "heartbeat" message in IMPv2.

  if (strlen(msgBody)==0) {
    free(msgBody);
    return;
  }

  // Validate the destination host 

  if (strcasecmp(destID,isis.serverID)==0) {
    destHost = ISIS_SERVER;

  } 
  else if (strcasecmp(destID,"AL")==0) {
    destHost = BROADCAST;

  }
  else {
    destHost = isKnownHost(destID);

    // Oops! destination is an unknown host, so we don't know how to
    // route the message.  Gripe.

    if (destHost == ERR_UNKNOWN_HOST) {
      sprintf(reply,"%s>%s ERROR: No Route to Destination Host %s - host is unknown/unlisted\r",
	      isis.serverID,srcID,destID);
      sendMessage(sendHost,reply);
      if (reply[strlen(reply)-1]=='\r') reply[strlen(reply)-1]='\0';
      logMessage(reply);
      if (isis.isVerbose && isis.useCLI) printf("\n<< %s >>\n",reply);
      free(msgBody);
      free(reply);
      return;
    }

  }

  // All done with the reply buffer, free it 

  free(reply);

  // We have valid source and destination hosts, and a non-blank
  // message.  Hand them off to handleMessage() for processing, return
  // when it is done.  [future multithreading point?]

  handleMessage(sendHost,destHost,msgBody);

  // all done 

  free(msgBody);
  return;

}

//---------------------------------------------------------------------------
//
// socketHandler()
//

/*!
  \brief Handle input from a network client socket.
  \param sockFD File descriptor of the server's socket.

  Read a line of input from the specified network socket file
  descriptor, and parse the message into components (source host ID,
  destination host ID, and message body).

  If the source and destination are valid, and the message is not blank
  or malformed, it then passes control off to handleMessage() to do the
  dirty work.  This may permit us to enable multithreading in the
  future.

  When generating IMPv2-compliant messages, the strings must be
  terminated with return (\\r = ASCII 13) not a newline (\\n = ASCII 10).

  This function is designed to be invoked by a select() loop that watches
  particular file descriptors.  See main.c for how it is implemented.
*/

void
socketHandler(int sockFD) 
{
  char srcID[IMPv2_HOST_SIZE];
  char destID[IMPv2_HOST_SIZE];
  char argStr[BIG_STR_SIZE];
  char *reply;
  char *msgBody;

  // Network client socket addressing stuff 

  int clientLen;
  struct sockaddr_in client;

  char clientAddr[INET6_ADDRSTRLEN];
  long clientHost;
  int  clientPort;

  char inetStr[INET6_ADDRSTRLEN];

  // Working variables 

  int sendHost;   // index of the source client in the client host table      
  int destHost;   // index of the destination client in the client host table 
  int numBytes;   // number of bytes read from the server socket         
  int bytesSent;  // number of bytes sent                                     

  // Dynamically allocate the incoming message buffer 

  char *message = (char *)malloc((ISIS_MSGSIZE)*sizeof(char));
  memset(message, 0, ISIS_MSGSIZE);

  // Allocate a reply buffer now (needed to log errors) 

  reply = (char *)malloc((ISIS_MSGSIZE)*sizeof(char));
  memset(reply, 0, ISIS_MSGSIZE);

  // Read the message from the socket 

  clientLen = sizeof(client);
  numBytes = recvfrom(sockFD, message, ISIS_MSGSIZE, 0,
		     (struct sockaddr *) &client, 
		     (socklen_t *) &clientLen);

  // Error, report back to the server console, free memory, and return 

  if (numBytes < 0) {
    sprintf(reply,"ERROR: Cannot recvfrom() network socket - %s",
	    strerror(errno));
    if (isis.useCLI) printf("\n%s\n",reply);
    logMessage(reply);
    free(reply);
    free(message);
    return;
  }

  // Allocate the message body buffer 

  msgBody = (char *)malloc((ISIS_MSGSIZE)*sizeof(char));
  memset(msgBody,0,ISIS_MSGSIZE);

  // Get basic information about the client for later use 

  clientHost = ntohl(client.sin_addr.s_addr);
  clientPort = ntohs(client.sin_port);
  strcpy(clientAddr,inet_ntop(client.sin_family,&client.sin_addr,
			      inetStr,sizeof(inetStr)));

  // Strip off any \n or \r terminators at the end of the message string 

  if (message[strlen(message)-1]=='\n') message[strlen(message)-1]='\0';
  if (message[strlen(message)-1]=='\r') message[strlen(message)-1]='\0';

  // Extract the first token from the message, this should be the the
  // address header of the IMPv2 message in "SourceID>DestID" form
  
  getArg(message,1,argStr);
  
  // Check to see if there is a ">" in the first token.  If so, split
  // the message into components.  If not, we have a malformed message.
  // Warn the console if interactive, otherwise just ignore it and return.

  if (strstr(argStr,">")) {
    sscanf(message,"%[^>]>%s %[^\n]",srcID,destID,msgBody);
    upperCase(srcID);
    upperCase(destID);

  }
  else {
    sprintf(reply,
	    "ERROR: malformed message from network client %s (%s:%d): '%s' [size %d bytes]",
	    srcID,clientAddr,clientPort,message,strlen(message));

    if (isis.useCLI) printf("\n%s\n",reply);
    free(message);
    free(reply);
    free(msgBody);
    return;
  }

  // We are all done with the raw message, and from here on out we only
  // work with its components.  Free the incoming message memory now so
  // we don't forget later (leak abatement).  We only log non-blank
  // messages.

  if (isis.isVerbose && isis.useCLI) printf("<< %s >>\n",message);
  if (strlen(msgBody)>0) 
    logMessage(message);
  free(message);

  // We have an IMPv2-conformal message string and its components:
  //   srcID = hostID of the sender
  //   destID = hostID of the intended recipient
  //   msgBody = message test to be delivered to destID
  // Now we deal with it.

  // Update the host table with the srcID information 
  
  sendHost = updateHosts(srcID,SOCKET,sockFD,clientHost,clientPort);

  // If the host table is full, we got problems.  Hit the console screen
  // and runtime log with an error message, try to echo one back to the
  // sender port, clean up, and return.

  if (sendHost == ERR_HOSTS_FULL) {
    sprintf(reply,
	    "ERROR: Host Table Full, cannot create entry for network client %s",
	    srcID);

    if (isis.useCLI) printf("\n%s\n",reply);
    logMessage(reply);
    memset(reply,0,ISIS_MSGSIZE);
    sprintf(reply,
	    "%s>%s ERROR: Server Host Table Full, Cannot Service Request\r",
	    isis.serverID,srcID);
    clientLen = sizeof(client);
    bytesSent = sendto(sockFD,reply,strlen(reply),0, 
		   (struct sockaddr *) &client, clientLen);
    free(reply);
    free(msgBody);
    return;
  }    

  // If msgBody is blank, make note of it but take no further action 

  if (strlen(msgBody)==0) {
    sprintf(reply,"heartbeat %s (%s:%d)",srcID,clientAddr,clientPort);
    if (isis.useCLI) printf("\n%s\n",reply);
    logMessage(reply);
    free(reply);
    free(msgBody);
    return;
  }

  // Validate the destination host 

  if (strcasecmp(destID,isis.serverID)==0) {
    destHost = ISIS_SERVER;

  } 
  else if (strcasecmp(destID,"AL")==0) {
    destHost = BROADCAST;

  }
  else {
    destHost = isKnownHost(destID);
    
    // Oops! Destination is an unknown host, we have no idea how to
    // route the message, so gripe.

    if (destHost == ERR_UNKNOWN_HOST) {
      sprintf(reply,"%s>%s ERROR: No Route to Destination Host %s - host is unknown/unlisted\r",
	      isis.serverID,srcID,destID);
      sendMessage(sendHost,reply);
      if (reply[strlen(reply)-1]=='\r') reply[strlen(reply)-1]='\0';
      if (isis.isVerbose && isis.useCLI) printf("<< %s >>\n",reply);
      logMessage(reply);
      free(reply);
      free(msgBody);
      return;
    }

  }

  // All done with the reply buffer, free it 

  free(reply);

  // We have valid source and destination hosts, and a non-blank
  // message.  Hand them off to handleMessage() for routing, return
  // when it is done.  [future multithreading point?]

  handleMessage(sendHost,destHost,msgBody);

  // all done 

  free(msgBody);

  return;

}

//---------------------------------------------------------------------------
//
// ttyHandler()
//

/*!
  \brief Handle input from the command line (readline callback function)
  \param ttyStr pointer to the text returned by readline()

  This is the callback for readline() that lets our cli make use of the
  GNU readline and history mechanism (arrow keys and command-line
  editing).  The callback is installed using the
  \c rl_callback_hander_install() function of the readline library.

  Once we get an error-free string we can use, we then try to interpret
  it.  This is the main cli handler for the server.

  When generating IMPv2 messages for direct transmission onto the
  system, the strings must be terminated with return (\\r = ASCII 13)
  not a newline (\\n = ASCII 10).
*/

#include <readline/readline.h>  // ttyHandler uses Gnu readline & history
#include <readline/history.h>

void
ttyHandler(char *ttyStr)
{

  // IMPv2 protocol variables 

  char destID[IMPv2_HOST_SIZE];  // destination address for messages   
  char argStr[MED_STR_SIZE];       // working "argument" string          
  char msgBody[BIG_STR_SIZE];      

  int sendHost = ISIS_SERVER;         // "sending" host is the server       
  int destHost;                       // index of the destination host      
  int doReply;

  // Network client socket stuff 

  struct sockaddr_in client;
  int clientLen;
  int bytesSent;

  // Pointers for the outgoing message buffer 

  char *message;

  // Stuff for the history facility 

  register HIST_ENTRY **the_list;
  register int iHist;
  char *expansion;
  int result;

  if (ttyStr==NULL) return;

  // If the ttyStr is blank, return 

  if (strlen(ttyStr)==0) {
    free(ttyStr);
    return;
  }

  // Allocate memory for the message buffer and clear it 

  message = (char *)malloc((ISIS_MSGSIZE)*sizeof(char));
  memset(message,0,ISIS_MSGSIZE);

  // Copy the keyboard input line into the message buffer 

  strcpy(message,ttyStr);

  // Do history expansion (!, !!, etc.) if required 

  if (ttyStr[0]) {
    result = history_expand(ttyStr,&expansion);
    if (result)
      printf("%s\n",expansion);
    
    if (result < 0 || result==2) {
      free(expansion);
      return;
    }

    add_history(expansion);
    memset(message,0,ISIS_MSGSIZE);
    sprintf(message,"%s",expansion);
    free(expansion);
  }

  // All done with the original string from readline(), free it 

  free(ttyStr);

  // Remove any \n terminator on the message string 

  if (message[strlen(message)-1]=='\n') message[strlen(message)-1]='\0';

  // Start processing the keyboard input 

  getArg(message,1,argStr);

  // >XX means send a message to client XX 

  if (strncasecmp(argStr,">",1)==0) {

    sscanf(message,">%s %[^\n]",destID,msgBody);

    // We have a destination, now what? 

    // If destID=AL, this is a broadcast message 

    if (strcasecmp(destID,"AL")==0) {
      if (isis.useCLI) printf("broadcast message - %s\n",msgBody);
      broadcastMessage(sendHost,msgBody);
      memset(message,0,ISIS_MSGSIZE);
      sprintf(message,"%s>AL %s",isis.serverID,msgBody);
      logMessage(message);
      free(message);
      return;

    } 

    // destID is some client host.  If known to us, pass it along 

    else {
      
      // Check destID against the client host table 

      if ((destHost=isKnownHost(destID))==ERR_UNKNOWN_HOST) {
	printf("ERROR: unknown client host %s\n",destID);
	free(message);
	return;

      } else {

	// Valid destination: build & terminate the outgoing message ... 

	memset(message,0,ISIS_MSGSIZE);
	sprintf(message,"%s>%s %s\r",isis.serverID,destID,msgBody);

	// ... pass it along via the appropriate transport method ... 

	sendMessage(destHost,message);

	// ... log it... 

	if (message[strlen(message)-1]=='\r') message[strlen(message)-1]='\0';
	if (message[strlen(message)-1]=='\n') message[strlen(message)-1]='\0';

	if (isis.isVerbose && isis.useCLI) printf("<< %s >>\n",message);
	logMessage(message);
	
	// ... then free the message memory and return. 

	free(message);
	return;
	
      } 
      
    } // end of message passing 
    
  } 

  // Otherwise, the keyboard input is an interactive command for the
  // server to execute.  Print results on the console as required.

  else {  

    // This set of commands is only accessible from the keyboard 
    
    // quit - shutdown the server 
    //
    // usage: quit <no args>

    if (strcasecmp(argStr,"quit")==0) {
      isis.keepGoing = isis_FALSE;
      memset(msgBody,0,sizeof(msgBody));
      sprintf(msgBody,"STATUS: MODE=OFFLINE %s=DISABLED Server Shutting Down",
	      isis.serverID);
      destHost = BROADCAST;

      handleMessage(sendHost,destHost,msgBody);

    } 

    // hosts - print a detailed client host table
    //
    // usage: hosts <no args>
     
    else if (strcasecmp(argStr,"hosts")==0) {
      strcpy(argStr,"all");
      printHosts(argStr);

    }

    // info - detailed server info
    //
    // usage: info <no args>
     
    else if (strcasecmp(argStr,"info")==0) {
      printInfo();

    }

    // help - interactive command summary
    //
    // usage: help <no args>
     
    else if (strcasecmp(argStr,"help")==0) {
      printHelp();
      
    }

    // verbose - echo all server communications to the console
    //
    // usage: verbose <no args>
     
    else if (strcasecmp(argStr,"verbose")==0) {
      if (isis.useCLI) isis.isVerbose = isis_TRUE;

    }

    // concise - suppress echoing of server communications (disable verbose)
    //
    // usage: concise <no args>
     
    else if (strcasecmp(argStr,"concise")==0) {
      isis.isVerbose = isis_FALSE;

    }

    // history - show the history list 
    //
    // usage: history <no args>
     
    else if (strcasecmp(argStr,"history")==0) {
      the_list = history_list();
      if (the_list) {
	for (iHist=0; the_list[iHist]; iHist++) {
	  printf("%5d   %s\n",iHist+history_base,the_list[iHist]->line);
	}
      }
    }

    // add other executive-level (keyboard-only) commands here ... 

    // All that's left are generic ISIS commands 

    else {
      doReply = isisCommand(message,message);
      
      if (doReply == MSG_REPLY)	printf("%s\n",message);
      
    } // end of ISIS command processing 

  } // end of TTY processing 

  // Free the message buffer and return 

  free(message);
  return;
    
}
