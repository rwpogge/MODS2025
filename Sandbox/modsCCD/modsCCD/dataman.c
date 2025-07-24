/*!
  \file  dataman.c
  \brief DataMan agent socket communications routines

  Simple API for our network-based DataMan interface agent application.
  These routines provide the socket I/O routines needed to talk with a
  dataman agent application running on the same (or a nearby) system.
  
  \author R. Pogge, OSU Astronomy Dept. (pogge@astronomy.ohio-state.edu)
  \date 2005 June 7

*/

#include "dataman.h"  // all the header we should need...

/*!
  \brief OpenDataMan - open a socket connection to the DataMan agent

  \param dm pointer to a dataman_config data structure
  \param Port localhost port to bind to for replies - must be unused
  \return file descriptor of the open socket or \<0 if an error

  Performs the necessary initialization functions to open a UDP inet
  socket for subsequent IMPv2-style DataMan agent communications.  If
  Port is 0, it picks the first available port, though for
  cleanliness, we usually keep a local port table and assign a port
  number to each application we run.

  Communications with the DataMan agent uses non-persistent
  "connectionless" UDP datagram (SOCK_DGRAM) sockets.  This opens and
  binds a local socket allowing us to send and receive UDP datagrams.

*/

int
OpenDataMan(dataman_t *dm, int Port)
{ 

  // sockaddr and hostent structs for the DataMan agent

  struct sockaddr_in DMclient;
  int DMclient_len;
  struct hostent *DMhost;

  // Build the sockaddr database: protocol family is AF_INET

  DMclient.sin_family = AF_INET;
  DMclient.sin_addr.s_addr = htonl(INADDR_ANY); // generic "any" address
  DMclient.sin_port = htons(Port);

  // get a socket for INET UDP datagrams (IMPv2-compliant)

  dm->FD = socket(AF_INET,SOCK_DGRAM,0);
  if (dm->FD < 0) {
    printf("ERROR(OpenDataMan): Cannot open DataMan comm socket - %s\n",
	   strerror(errno));
    dm->FD = -1;
    return -1;
  }

  // bind this socket to the requested port

  if (bind(dm->FD, (struct sockaddr *) &DMclient, sizeof(DMclient)) < 0) {
    printf("ERROR (OpenDataMan): Cannot bind DataMan comm socket %s\n",
           strerror(errno));
    close(dm->FD);
    dm->FD = -1;
    return -1;
  }

  // Success!  Return the file descriptor of the open/bound UDP socket

  return dm->FD;

}

/*!
  \brief ReadDataMan - read data sent from the DataMan agent to our UDP socket

  \param dm pointer to a dataman_config data structure
  \param buf character string to hold the data read

  \return number of bytes read, or -1 if an error

  We communicate with the DataMan agent using UDP datagrams.  This
  routine sets up a socket read with a timeout interval. If timeout=0,
  it simply polls the socket.  If #dataman_config::Timeout<0, it
  blocks indefinitely until either there is input on the socket, or a
  SIGINT (Ctrl+C) signal is received.  If #dataman_config::Timeout>0,
  it will wait "timeout" seconds for input on the socket.  If the
  timeout expires before data is received, it issues a timeout error
  and returns.  The calling routine must figure out what to do next.

*/

int
ReadDataMan(dataman_t *dm, char *buf)
{
  int nread=0;
  struct sockaddr_in sock;
  int sock_len;
  int nready;

  fd_set readfds;
  struct timeval tv;

  // If we have no active file descriptor, we can't do this...

  if (dm->FD <= 0) {
    printf("ERROR(ReadDataMan): No DataMan socket open.\n");
    return -1;
  }

  // Clear the incoming message string buffer

  memset(buf,0,DM_MSGSIZE);
  sock_len = sizeof(sock);

  // Set the timeout interval.  If <0, block forever (null), otherwise
  // if 0sec, poll and return empty handed if that's what the deal is

  if (dm->Timeout <= 0L) {
    tv.tv_sec = 0;
    tv.tv_usec = 0;
  }
  else {
    tv.tv_sec = dm->Timeout;
    tv.tv_usec = 0;
  }

  FD_ZERO(&readfds);        // clear the file descriptor table
  FD_SET(dm->FD,&readfds);  // set the socket file descriptor for watching

  if (dm->Timeout < 0) 
    nready = select(dm->FD+1,&readfds,(fd_set *)NULL, (fd_set *)NULL, NULL);
  else
    nready = select(dm->FD+1,&readfds,(fd_set *)NULL, (fd_set *)NULL, &tv);

  if (nready < 0) {
    if (errno == EINTR) { // got a Ctrl+C interrupt
      printf("ReadDataMan: caught interrupt, aborting\n");
      return -2;
    }
    printf("ERROR (ReadDataMan): select error - %s\n",strerror(errno));
    return -2;
  }
  else if (nready == 0) {
    if (dm->Timeout > 0) {
      printf("ERROR (ReadDataMan): %d sec timeout expired\n",dm->Timeout);
      return -1;
    }
    else
      return 0;   // nothing to read when we polled the socket
  }

  // If we got here, we have something to read from the socket - do it!

  nread=recvfrom(dm->FD, buf, DM_MSGSIZE, 0,
		 (struct sockaddr *) &sock, (socklen_t *) &sock_len);

  if (nread<0)
    printf("ERROR(ReadDataMan): network socket recvfrom() error - %s\n",
           strerror(errno));

  // all done, return the number of bytes read

  return nread;

}

/*!
  \brief WriteDataMan - write data to the DataMan agent via our UDP socket

  \param dm pointer to a #dataman_config data structure
  \param buf character string with the data to write to the DataMan socket
  \return number of bytes written, or -1 if an error

  We communicate with the DataMan agent using UDP datagram sockets.
  This routine sends a command string to the DataMan agent at the
  specified host:port.

  How do we handle string termination?  In the calling routine.  buf
  must contain a properly configured, IMPv2-compliant message string.
*/

int
WriteDataMan(dataman_t *dm, char *buf)
{
  int nwrote;
  struct hostent *host;
  struct sockaddr_in hostaddr;
  int hostaddr_len;
  
  if (strlen(buf)<=0)
    return 0;

  if (dm->FD <= 0) {
    printf("ERROR(WriteDataMan): No DataMan comm socket open.\n");
    return -1;
  }

  // Translate the DM agent's hostname into an IP address 

  if (!(host=gethostbyname(dm->Host))){
    printf("ERROR(WriteDataMan): cannot resolve hostname %s - %s\n",
           dm->Host,hstrerror(h_errno));
    return -1;
  }

  // Setup the DM agent host's socket address database 

  hostaddr.sin_port = htons(dm->Port);
  hostaddr.sin_family = AF_INET;
  memcpy(&hostaddr.sin_addr,host->h_addr, host->h_length);
  hostaddr_len = sizeof(hostaddr);

  // Send the message string to the remote host

  nwrote = sendto(dm->FD,buf,strlen(buf),0,
		  (struct sockaddr *) &hostaddr, sizeof(hostaddr));

  if (nwrote<0)
    printf("ERROR(WriteDataMan): could not sendto() %s:%d - %s\n",
           dm->Host,dm->Port,strerror(errno));

  return nwrote;

}

/*!
  \brief CloseDataMan - close a DataMan agent connection

  \param dm pointer to a dataman_config struct

  Simple function for closing an open DataMan agent communications
  socket. Provided for syntatic consistency with the rest of the API.

  Eventually, we may find we need to do some housekeeping or other
  post-termination cleanup chores, and this function will become the
  logical home for such actions.

*/

void
CloseDataMan(dataman_t *dm)
{
  if (dm->FD > 0)
    close(dm->FD);
  dm->FD = -1;
}

/*!
  \brief InitDataMan - initialize a DataMan data structure

  \param dm pointer to a #dataman_config data structure to initialize

  Initializes the contents of a #dataman_config data structure to
  sensible default values.  Should be called to remove any delinquent
  data values before setting it up with real values (Note,
  LoadDMConfig() calls this function).

  \sa LoadDMConfig()
*/

void
InitDataMan(dataman_t *dm)
{

  if (dm->FD < 0) { // only change these if we are no live (allows warm restart)
    strcpy(dm->Host,"NONE");
    dm->Port = 0;
    dm->FD = -1;
  }

  dm->useDM = 0;  // start out assuming no DataMan in use

  strcpy(dm->CfgFile,"NONE");
  strcpy(dm->Name,"NONE");

  dm->Timeout = DM_TIMEOUT;
  strcpy(dm->ID,"NONE");

}

/*!
  \brief LoadDMConfig - load the contents of a DataMan configuration file

  \param dm pointer to a dataman_config data stucture
  \param config name of a configuration file to load
  \param reply string to contain any replies from the function
  \return 0 on success, -1 on errors

  Opens and parses the contents of a DataMan configuration file
  containing all of the basic parameters required to setup the
  communications with a dataman agent.

  The config file resides on a path visible to the system running the
  application; whether or not the config file is visible to the dataman
  agent itself is irrelevant (most systems have the dataman agent running
  on the same host, but this is not required).

  A basic DataMan configuration file might looks like this:
  <pre>
  #
  # DataMan Agent on the CTIO Yale 1-meter telescope
  #
  # R. Pogge, OSU Astronomy
  # 2005 May 20
  #
  Name TC
  Host 139.229.12.50
  Port 10701
  Timeout 10
  </pre>
  A # marks a comment to be ignored (but no inline comments allowed, the
  parser is very simpleminded).  Keywords are followed with their values
  separated by spaces.

  The minimum information needed is the name, host, port, and timeout of
  the DataMan agent.  Other data may be added later as required.

*/

int
LoadDMConfig(dataman_t *dm, char *config, char *reply)
{ 
  char inbuf[MAXCFGLINE];   // Generic line buffer
  char keyword[MAXCFGLINE]; // File is organized into KEYWORD VALUE pairs
  char argbuf[MAXCFGLINE];  // Generic argument buffer

  FILE *cfgFP;              // config file pointer
  int i;
  char c;                    

  // Open the config file readonly
  
  if (!(cfgFP=fopen(config, "r"))) {
    sprintf(reply,"Cannot open DataMan config file %s - %s\n",
	    config,strerror(errno));
    return -1;
  }
  
  // Clear the DataMan struct of delinquent data

  InitDataMan(dm);

  // Save the name of this config file for future reference

  strcpy(dm->CfgFile,config);
  
  dm->useDM = 1;

  // Read in each line skipping comments (#) and blank lines.  Gripe to
  // stdout if any junk is encountered

  while(fgets(inbuf, MAXCFGLINE, cfgFP)) {
    
    if ((inbuf[0]!='#') && (inbuf[0]!='\n') && inbuf[0]!='\0') {
      inbuf[MAXCFGLINE] ='\0';
      GetArg(inbuf, 1, argbuf);
      strcpy(keyword, argbuf);
      
      // Name of the DataMan Agent

      if (strcasecmp(keyword,"Name")==0) {
	GetArg(inbuf,2,argbuf);
	strcpy(dm->ID,argbuf);
      }

      // DataMan Agent Hostname

      else if (strcasecmp(keyword,"Host")==0) {
	GetArg(inbuf,2,argbuf);
	strcpy(dm->Host,argbuf);
      }

      // DataMan Agent Port

      else if (strcasecmp(keyword,"Port")==0) {
	GetArg(inbuf,2,argbuf);
	dm->Port = atoi(argbuf);
      }

      // UDP Socket Communications Timeout in seconds

      else if (strcasecmp(keyword,"Timeout")==0) {
	GetArg(inbuf,2,argbuf);
	dm->Timeout = (long)(atoi(argbuf));
	if (dm->Timeout < 0) dm->Timeout = 10L;
      }

      // Any other junk? gripe to stdout but don't generate an error

      else { 
	printf("Ignoring unrecognized DataMan config file entry '%s'", inbuf);
	
      }
    }
    memset(inbuf,0,sizeof(inbuf));
  }

  // All done, close the config file and report success

  if (cfgFP !=0) fclose(cfgFP);
  
  sprintf(reply,"Loaded DataMan interface config file %s successfully",config);
  return 0;

}

/*!
  \brief DataManInfo - Print the DataMan agent interface configuration
  
  \param dm pointer to the #dataman_config data structure

  Prints the contents of the #dataman_config data structure describing
  the DataMan agent interface configuration to stdout.

*/

void
DataManInfo(dataman_t *dm)
{
  printf("DataMan Agent Interface Configuration:\n");

  if (dm->useDM)
    printf("  Interface: ENABLED\n");
  else
    printf("  Interface: DISABLED\n");

  printf("  Config File: %s\n",dm->CfgFile);

  printf("Communications:\n");
  if (dm->FD < 0) 
    printf("  Comm Port CLOSED\n");
  else
    printf("  Comm Port OPEN\n");

  printf("  DataMan Agent Node: %s\n",dm->ID);
  printf("  DataMan Agent Host: %s:%d\n",dm->Host,dm->Port);
  printf("  Comm Timeout: %d seconds\n",dm->Timeout);

  printf("\n");

}

