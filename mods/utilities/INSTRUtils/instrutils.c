/*!
  \file instrutils.c
  \brief Generic instrument I/O handling utilties.

  This is a set of simple functions for using serial (tty) ports.  It
  allows relatively seamless communications with either direct serial
  ports (/dev/ttyXX) on a host computer, or networked serial port
  servers like the Commtrol DeviceMaster RTS-x series devices.  They
  encapsulate a number of useful basic functions for opening, closing,
  setting attributes, reading, writing, and flushing junk from serial
  ports, relieving writers of ISIS client applications from the pain of
  getting all the arcane fcntl and termios calls right.

  \author Rick Pogge, OSU Astronomy Dept (pogge@astronomy.ohio-state.edu)
  \date 2005 March 15
  \Staff, OSU Astronomy Dept (staff@astronomy.ohio-state.edu)
  \date 2005 March 15

  \date 2025 June 19 - AlmaLinux 9 port and cleanup [rwp/osu]
*/

// headers we need, no paths! use -I in Makefile

#include "system_dep.h" 
#include "instrutils.h" 
#include "isl_funcs.h" 
#include "params.h"
#include "islcommon.h"
#include "isl_shmaddr.h"

/*!
  \brief Open a port
  \param port pointer to a ttyport_t struct with the port configuration
  \return File descriptor of the open comm port connection, or -1 if an error.

  Performs the necessary initialization functions to open a serial port.
  The device name syntax is as follows:
  <pre>
     /dev/ttyXX#  = regular host serial port (tty port)
     host:port    = network serial port server address (IP and port #).
  </pre>
  The former is for a serial device connected to a direct serial port on
  the host computer, or to a virtual serial port (e.g., on a remote
  terminal server like a Comtrol DeviceMaster RTS using a virtual serial
  port device driver).  The latter is for a direct network socket
  connection to a remote serial port server (e.g., a Comtrol RTS
  configured as a socket server).

  To use, you must fill in the relevant data members of the ttyport_t
  struct.  The port->Port member is used to set the device name as
  above.  If the port is a direct host serial port, you must also define
  the Speed, DataBits, StopBits, and Parity attributes.  This API
  assumes no default values; those are the responsibility of the
  applications using these routines.  Note that you can change the
  attributes of any open direct serial port anytime using the
  SetTTYPort() function.

  For socket communications with a network serial port server, we use
  INET streams (SOCK_STREAM) with a persistent client connection.  This
  is pretty much standard for most devices on the market.  For these
  ports there is no need to define attributes, and indeed if any are
  set, the routine recognizes the interface type from the name and sets
  them to "blank" values (0).

  \sa CloseTTYPort(), SetTTYPort()
*/

void reusePort(int s);

int
OpenTTYPort(ttyport_t *port)
{ 
  int portFD = -1;
  int i,k,flags;
  // sockaddr and hostent structs for possible network terminal server

  char hostID[64];
  int portID;
  struct sockaddr_in PortServer;
  int PortServer_len;
  struct hostent *PortHost;

  // termios struct for a possible serial (tty) port

  struct termios tty;

  // portname better be plausible

  if (strlen(port->Port)<=0) {
    return -1;
  }

  // Look for a ":" in port->Port.  This denotes a network host:port
  // device, otherwise assume it is a valid /dev/ttyXXX type device

  if (strstr(port->Port,":")) {
    sscanf(port->Port,"%[^:]:%d",hostID,&portID);

    // Try to resolve the network port server's host ID.  If not resolvable
    // we're screwed - abort with informative error messages.

    if (!(PortHost=gethostbyname(hostID))) {
      return -1;
    }

    // Initialize the network serial port attributes

    port->Interface = TTY_NETWORK;
    port->Speed = 0;    // placeholders, not relevant to this interface
    port->DataBits = 0;
    port->StopBits = 0;
    port->Parity = 0;

    if (port->FD > 0) {
      close(port->FD);
      port->FD = -1;  // the no-port value
    }

    // Build the sockaddr database: protocol family is AF_INET

    PortServer.sin_family = AF_INET;
    memcpy(&PortServer.sin_addr,PortHost->h_addr, PortHost->h_length); 
    PortServer.sin_port = htons(portID);

    // Get a socket: all network serial port servers we've encountered thus
    // far use INET streams.  Not sure UDP datagrams would work...

    portFD = socket(AF_INET,SOCK_STREAM,0);

    if (portFD < 0) {
      printf("TIME_WAIT\n");
      // this allow the server to re-start quickly instead of fully wait
      // for TIME_WAIT which can be as long as 3 minutes
      reusePort(portFD);
    }
    // Connect to the network serial port server
    if (connect(portFD,(struct sockaddr *) &PortServer, sizeof(PortServer))<0){
      close(portFD);
      return -1;
    }

    // Make sure this port is non-blocking
    fcntl(port->FD,F_SETFL,O_NONBLOCK);
  }

  // port->Port is a normal host-computer serial device.  We open it and
  // set its attributes

  else {
    
    // set the port interface

    port->Interface = TTY_SERIAL;
    if (port->FD > 0) {
      close(port->FD);
      port->FD = -1;  // the no-port value
    }

    // Attempt to open the named serial port

    portFD = open(port->Port, O_RDWR|O_NDELAY);

    if (portFD <= 0) {
      //printf("ERROR(OpenTTYPort): Cannot open serial port %s - %s\n", 
      //     port->Port,strerror(errno));
      return -1;
    }

    if (SetTTYPort(port)<0) {
      //printf("ERROR(OpenTTYPort): Failed to initialize port %s attributes - %s\n",
      //     port->Port,strerror(errno));
      return -1;
    }
  }

  // Success: return the file descriptor of the open comm port
  
  port->FD = portFD;

  return 0;
}

/*!
  \brief Set the serial port attributes

  \param port pointer to a ttyport_t struct with the port parameters
  \return 0 if successful, -1 if an error.

  Sets the attributes of the open serial port: speed, data bits
  (character size mask), stop bits (1 or 2), and partiy (no parity, or
  parity generation on output/checking on input).  The normal mode for
  most tty ports we use in the data-taking system are set to
  \verbatim 9600 baud, 8 data bits, 1 stop bit, no parity \endverbatim
  corresponding to Speed=9600, DataBits=8, StopBits=2, Parity=0.
  Attributes are set by putting the desired values into the relevant 
  data members of the ttyport_t struct passed to this function.

  The port must have already been opened using OpenTTYPort().

  Note that this has no effect if using a network serial port where the
  attributes are set in the server proper (and not all servers allow
  remote setting of attributes from host programs), and will complain if
  called with an unknown or network interface flag set.

  \sa OpenTTYPort()
*/

int
SetTTYPort(ttyport_t *port)
{
  struct termios tty;

  // validate the arguments passed

  if (port->FD < 0) {
    //printf("ERROR(SetTTYPort()) - port is not open\n");
    return -1;
  }

  // Setting attributes is only possible for TTY_SERIAL interfaces

  switch (port->Interface) {
  case TTY_NETWORK:
    //printf("ERROR(SetTTYPort()) - Cannot set port attributes for a Network serial port\n");
    return -1;
    break;
    //case TTY_UNKNOWN:
  case TTY_SERIAL:
    break;
  default:
    //printf("ERROR(SetTTYPort()) - Unknown port interface, cannot set attributes\n");
    return -1;
    break;

  }

  // DataBits must be 5..8 inclusive, usually 8

  if (port->DataBits < 5 || port->DataBits > 8) {
    //printf("ERROR(SetTTYPort()) - DataBits is %d - must be 5..8\n",
    //   port->DataBits);
    return -1;
  }

  // StopBits must be 1 or 2, usually 1

  if (port->StopBits < 1 || port->StopBits > 2) {
    //printf("ERROR(SetTTYPort()) - StopBits is %d - must be 1 or 2\n",
    //   port->StopBits);
    return -1;
  }

  // Parity must be 0 or 1, usually 0

  if (port->Parity < 0 || port->Parity > 1) {
    //printf("ERROR(SetTTYPort()) - Parity is %d - must be 0 or 1\n",
    //   port->Parity);
    return -1;
  }

  // port->Speed must be one of (1200,2400,4800,9600,19200,38400)

  switch(port->Speed) {
  case 1200:
  case 2400:
  case 4800:
  case 9600:
  case 19200:
  case 38400:
    break;
  default:
    //printf("ERROR(SetTTYPort()) - speed is %d - must be one of"
    //   " (1200,2400,4800,9600,19200,38400)\n",port->Speed);
    return -1;
    break;
  }

  // we have good parameters, set up the port

  if (tcgetattr(port->FD,&tty)<0) {
    //printf("ERROR(SetTTYPort() - could not get port attributes for %s - %s\n",
    //   port->Port, strerror(errno));
    return -1;
  }

  tty.c_iflag &= ~ISTRIP;     
  tty.c_lflag |= ICANON;
  tty.c_lflag &= ~ECHO;
  switch(port->DataBits) {
  case 5:
    tty.c_cflag |= CS5;
    break;
  case 6:
    tty.c_cflag |= CS6;
    break;
  case 7:
    tty.c_cflag |= CS7;
    break;
  case 8:
    tty.c_cflag |= CS8;
    break;
  }
  tty.c_cflag |= CREAD;
  if (port->StopBits == 1)
    tty.c_cflag &= ~CSTOPB;   // 1 stop bit
  else
    tty.c_cflag &= CSTOPB;    // 2 stop bits
  if (port->Parity==0)
    tty.c_cflag &= ~PARENB;   // no parity
  else
    tty.c_cflag &= PARENB;    // parity enabled
  tty.c_cc[VMIN] = 1;
  tty.c_cc[VTIME] = 0;

  switch(port->Speed) {
  case 1200:
    cfsetispeed(&tty, (speed_t) B1200);
    cfsetospeed(&tty, (speed_t) B1200);
    break;
  case 2400:
    cfsetispeed(&tty, (speed_t) B2400);
    cfsetospeed(&tty, (speed_t) B2400);
    break;
  case 4800:
    cfsetispeed(&tty, (speed_t) B4800);
    cfsetospeed(&tty, (speed_t) B4800);
    break;
  case 9600:
    cfsetispeed(&tty, (speed_t) B9600);
    cfsetospeed(&tty, (speed_t) B9600);
    break;
  case 19200:
    cfsetispeed(&tty, (speed_t) B19200);
    cfsetospeed(&tty, (speed_t) B19200);
    break;
  case 38400:
    cfsetispeed(&tty, (speed_t) B38400);
    cfsetospeed(&tty, (speed_t) B38400);
    break;
  }
  tcflush(port->FD, TCIFLUSH);
  tcsetattr(port->FD, TCSAFLUSH, &tty);

  return port->FD;
  //  return 0;
}

/*!
  \brief Close an open serial port.

  \param port pointer to a ttyport_t struct with the port parameters
  (see OpenTTYPort())
  
  Closes an open serial port.  Used as a simple wrapper for the close()
  function to provide a logical functional opposite to OpenTTYPort().
  Also sets the FD and Interface data members of the port struct to
  indicate that the port is closed and its interface is unknown.  Other
  attributes (Speed, etc. as relevant) are left alone to permit the user
  to preserve memory of these if they wish.

*/

void
CloseTTYPort(ttyport_t *port) 
{
  if (port->FD > 0)
    close(port->FD);
  port->FD = -1;
  port->Interface = TTY_UNKNOWN;
}
  
/*!
  \brief Write a message string to a serial port

  \param port pointer to a ttyport_t struct with the port parameters
  \param msgstr message string to write
  \return Number of bytes written if successful, 0 or -1 if unsuccessful.

  Writes the message string provided to the serial port described by the
  port struct.  This serial port must have been previously opened and
  setup using the OpenTTYPort() function.

  \sa ReadTTYPort()
*/

int  
WriteTTYPort(ttyport_t *port, char *msgstr)
{
  int nsent = 0;

  nsent = write(port->FD,msgstr,strlen(msgstr));
  //if (nsent < 0)
  //printf("ERROR(WriteTTYPort()) - Cannt write to serial port %s - %s\n",
  //port->Port,strerror(errno));
  return nsent;

}

/*!
  \brief Read data from a serial port

  \param port Pointer to a ttyport_t struct with the port parameters
  \param msgstr Message string to carry the input string
  \param timeout Wait timeout seconds for input

  \return The number of characters read, or <0 if an error.  -1 on error
  or timeout, with \e msgstr containing the error message text.

  Uses select() to read data from the specified comm port with the
  timeout interval specified.  Note that because we could be reading a
  stream rather than a line-buffered source (e.g., port->FD points to a
  socket stream open to a network serial port server like a Comtrol
  DeviceMaster RTS instead of a direct serial port connection on the
  host computer proper), the data could come in bursts depending on the
  system state and stream synchronization.  As such we read in chunks of
  data w/o blocking until we have read everything from the port, which
  means looking for the \\r (ASCII 13 = Ctrl+M) terminator.  This makes
  the logic tricky, but robust against most comm glitches.  Use of a
  timeout allows us to break out of cases where the message is
  unterminated because of a comm fault, not because of the usual stream
  buffering/sync issues.

  \note 
  If select() is interrupted by Ctrl+C, it returns an error message in the
  \e reply string.

*/

int  
ReadTTYPort(ttyport_t *port, char *msgstr, long timeout)
{
  int keepReading;
  char inbuf[PAGE_SIZE];  // working buffer
  int lastchar,i,j=0,ierr;
 
 // for select()

  fd_set readfds;     
  int nready;
  struct timeval tv;
  // timeout interval is given in seconds.  if <0 or 0, set to 0, which
  // means we are polling the port

  if (timeout <= 0L) {
    tv.tv_sec = 0;
    tv.tv_usec = 0;
  }
  else {
    tv.tv_sec = timeout;
    tv.tv_usec = 0;
  }

  memset(inbuf,0,sizeof(inbuf));

  lastchar=0;
  keepReading = 1;

  while (keepReading) {

    // we use a select() call to enable read with timeout

    FD_ZERO(&readfds);  // clear file descriptors
    FD_SET(port->FD,&readfds);

    // select() returns <0 if interrupted by a signal before the timeout
    // interval has expired, or if an error occurs, otherwise it returns
    // 0 when a timeout occurs, or >0 if it got something

    nready = select(port->FD+1,&readfds,(fd_set *)NULL, (fd_set *)NULL, &tv);

    if (nready < 0) {
      if (errno == EINTR) { // got a Ctrl+C interrupt
	memset(msgstr,0,sizeof(msgstr));
	sprintf(msgstr,"(ReadTTYPort) Port %s read aborted Ctrl+C\n",
		port->Port);
	return -10;
      }
      memset(msgstr,0,sizeof(msgstr));
      sprintf(msgstr,"(ReadTTYPort) Port %s read select() error - %s\n",
	      port->Port,strerror(errno));
      return -11;

    } else if (nready == 0 && timeout > 0L) {
      memset(msgstr,0,sizeof(msgstr));
      sprintf(msgstr,"(ReadTTYPort) Port %s read timed out after %d sec\n",
	      port->Port,timeout);
      return -104;
    } else { 
      if (FD_ISSET(port->FD,&readfds)) {
	memset(inbuf,0,sizeof(inbuf));
	if (ierr=read(port->FD,inbuf,sizeof(inbuf))<=0) {
	  memset(msgstr,0,sizeof(msgstr));
	  sprintf(msgstr,"(ReadTTYPort) Cannot read port %s - %s\n",
		  port->Port,strerror(errno));
	  return ierr;
	}
	lastchar=strlen(inbuf);
	inbuf[lastchar]='\0';
	/*
	  A message from the Microlynx Controller is not complete 
	  unless we see '>' at the end. Using "block" reads, and test 
	  for '>' or '?' in the last character to tell us if we're done.  
	  If no '>' or '?' appears, go back into the loop to get the rest.
	  In practice, at most 2 reads are required, and 95% or more require
	  only 1 read.  Note that if port->FD points to a line-buffered device
	*/
	if (lastchar > 0) {
	  strcat(msgstr,inbuf);
	  j+=lastchar;
	  if(strstr(&msgstr[0],">")) return j;
	  else if(strstr(&msgstr[0],"?")) return j;
	  else if(strstr(&msgstr[lastchar],">")) return j;
	  else if(msgstr[0]=='0') return j;
	} // end of strlen.
      } // end of FD_ISSET 
    } // end of else
    TTYMSleep(1);
  } // select() event loop
  return 0;
} // end of ReadTTYPort

/*!
  \brief Flush junk from a serial port

  \param port pointer to a ttyport_t struct with the port parameters
  \param ntimes number of times to read the port for flushing

   Reads the serial port ntimes to attempt to flush any extraneous port
   junk.  This port must have been previously opened and setup using the
   OpenTTYPort() function.  Assumes that the buffer size is at least the
   default BUFSIZ defined in stdio.h, though whether or not this is a
   good idea remains to be seen.

   \sa TTYMSleep()
*/

void 
FlushTTYPort(ttyport_t *port, int ntimes)
{
  char *junk;
  int i;

  if (ntimes <= 0)
    return;

  if (port->FD < 0) {
    //printf ("ERROR(FlushTTYPort): cannot flush port - port not open\n");
    return;
  }

  junk = (char *)malloc(BUFSIZ*sizeof(char));
  for (i=0; i<ntimes; i++) {
    read(port->FD,junk,sizeof(junk));
    TTYMSleep(50);  // sleep 50msec between flushes
  } // for loop
  free(junk);

}  // end of FlushTTYPort

/*!
  \brief Pause execution for a certain number of milliseconds

  \param msec - time to sleep in milliseconds
  \return 0 if successful, -1 if there were problems.  errno says
  what is bothering it.

  Acts as a convenience function for using nanosleep(), the POSIX.1b
  conformal way of suspending a process for a certain time.  nanosleep()
  is a replacement for the obsolete usleep() when a sleep interval more
  fine-grained than sleep() is required. sleep() only allows intervals
  in integer seconds, which is not short enough for some applications.
  For most of what we do, msec are the appropriate interval, and
  nanosleep() can be somewhat involved to setup.  This function
  encapsulates the nasty setup for calling nanosleep() as a convenience
  for users.

  See "man 2 nanosleep" for details on the nanosleep() function.
*/

int
TTYMSleep(long msec)
{
  struct timespec treq, trem;
  long tsec, nsec;

  if (msec < 0)
    return -1;

  if (msec >= 1000) {
    tsec = (long)(floor((double)(msec)/1000.0));  // seconds part
    nsec = 1000000000*(((double)(msec)/1000.0) - (double)(tsec));  // ns part
  }
  else {
    tsec = 0;
    nsec = msec*1000000;   // msec -> nsec
  }

  treq.tv_sec = (time_t)(tsec);
  treq.tv_nsec = nsec;

  return nanosleep(&treq,&trem);

}  // end of TTYMSleep

// this allow the service to re-start quickly instead of fully wait
// for TIME_WAIT which can be as large as 2 minutes
void reusePort(int s)
{
  int one=1;

  if ( setsockopt(s,SOL_SOCKET,SO_REUSEADDR,(char *) &one,sizeof(one)) == -1 )
  {
    printf("error in setsockopt,SO_REUSEPORT \n");
    //    exit(-1);
  }
}
