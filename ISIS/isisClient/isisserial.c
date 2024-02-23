/*!
  \file isisserial.c
  \brief ISIS client serial port handling utilties.

  This is a set of simple functions for using serial (tty) ports in an
  ISIS client application.  They encapsulate a number of useful basic
  functions for opening, closing, setting attributes, reading, writing,
  and flushing junk from serial ports, relieving writers of ISIS client
  applications from the pain of getting all the arcane fcntl and termios
  calls right.

  \author R. Pogge, OSU Astronomy Dept (pogge@astronomy.ohio-state.edu)
  \date 2003 September 15

*/

#include "isisclient.h" // ISIS client master header file

/*!
  \brief Open a serial port

  \param ttydev Name of the port (e.g., /dev/ttya)
  \return File descriptor of the open serial port or -1 if failed

  Opens the named serial port for unblocking read/write (the only
  mode we support), and returns the file descriptor of the open port.
  All error messages are reported internally.  This function only
  opens the port; port attributes (speed, parity, etc.) need to be
  set subsequently using the SetSerialPort() call
*/

int
OpenSerialPort(char *ttydev)
{
  int fd;

  if ((fd=open(ttydev,O_RDWR|O_NDELAY)) > 0)
    return fd;
  else {
    printf("ERROR(OpenSerialPort()) - cannot open serial port %s - %s\n",
	   ttydev,strerror(errno));
    return -1;
  }
}

/*!
  \brief Set the serial port attributes

  \param ttyFD File descriptor of the open port (see OpenSerialPort())
  \param speed Port speed in baud, one of (1200,2400,4800,9600,19200,38400)
  \param databits Number of data bits (5, 6, 7, or 8)
  \param stopbits Number of stop bits (1 or 2)
  \param parity Port parity, 0 = noparity, 1 = enable parity  
  \return 0 if successful, -1 if an error.

  Sets the attributes of the open serial port: speed, data bits
  (character size mask), stop bits (1 or 2), and partiy (no parity, or
  parity generation on output/checking on input).  The normal mode for
  most tty ports we use in the data-taking system are set to
  \verbatim 9600 baud, 8 data bits, 1 stop bit, no parity \endverbatim
  corresponding to speed=9600, databits=8, stopbits=2, parity=0.

  The port must have already been opened using OpenSerialPort().

  \sa OpenSerialPort()
*/

int
SetSerialPort(int ttyFD, int speed, int databits, int stopbits, int parity)
{
  struct termios tty;

  // validate the arguments passed

  if (ttyFD < 0) {
    printf("ERROR(SetSerialPort()) - port is not open\n");
    return -1;
  }

  // databits must be 5..8 inclusive, usually 8

  if (databits < 5 || databits > 8) {
    printf("ERROR(SetSerialPort()) - databits is %d - must be 5..8\n",
	   databits);
    return -1;
  }

  // stopbits must be 1 or 2, usually 1

  if (stopbits < 1 || databits > 2) {
    printf("ERROR(SetSerialPort()) - stopbits is %d - must be 1 or 2\n",
	   stopbits);
    return -1;
  }

  // parity must be 0 or 1, usually 0

  if (parity < 0 || parity > 1) {
    printf("ERROR(SetSerialPort()) - parity is %d - must be 0 or 1\n",
	   parity);
    return -1;
  }

  // speed must be one of (1200,2400,4800,9600,19200,38400)

  switch(speed) {
  case 1200:
  case 2400:
  case 4800:
  case 9600:
  case 19200:
  case 38400:
    break;
  default:
    printf("ERROR(SetSerialPort()) - speed is %d - must be one of"
	   " (1200,2400,4800,9600,19200,38400)\n",speed);
    return -1;
    break;
  }

  // we have good parameters, set up the port

  if (tcgetattr(ttyFD,&tty)<0) {
    printf("ERROR(SetSerialPort() - could not get port attributes - %s\n",
	   strerror(errno));
    return -1;
  }

  tty.c_iflag &= ~ISTRIP;     
  tty.c_lflag |= ICANON;
  tty.c_lflag &= ~ECHO;
  switch(databits) {
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
  if (stopbits == 1)
    tty.c_cflag &= ~CSTOPB;   // 1 stop bit
  else
    tty.c_cflag &= CSTOPB;    // 2 stop bits
  if (parity==0)
    tty.c_cflag &= ~PARENB;   // no parity
  else
    tty.c_cflag &= PARENB;    // parity enabled
  tty.c_cc[VMIN] = 1;
  tty.c_cc[VTIME] = 0;

  switch(speed) {
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
  tcflush(ttyFD, TCIFLUSH);
  tcsetattr(ttyFD, TCSAFLUSH, &tty);

  return 0;
}

/*!
  \brief Close an open serial port.

  \param ttyFD file descriptor of an open serial port (see OpenSerialPort())
  
  Closes an open serial port.  Used as a simple wrapper for the close()
  function to provide a logical functional opposite to OpenSerialPort().

*/

void
CloseSerialPort(int ttyFD) 
{
  if (ttyFD > 0)
    close(ttyFD);
}
  
/*!
  \brief Write a message string to a serial port

  \param ttyFD file descriptor of an open serial port (see OpenSerialPort())
  \param msgstr message string to write
  \return Number of bytes written if successful, 0 or -1 if unsuccessful.

  Writes the message string provided to the serial port given by ttyFD.
  This port must have been previously opened and setup using the
  OpenSerialPort() and SetSeriaPort() functions.

  \sa ReadSerialPort()
*/

int  
WriteSerialPort(int ttyFD, char *msgstr)
{
  int nsent = 0;

  nsent = write(ttyFD,msgstr,strlen(msgstr));
  if (nsent < 0) 
    printf("ERROR(WriteSerialPort()) - could not write to serial port - %s\n",
	   strerror(errno));
  return nsent;

}

/*!
  \brief Read data from a serial port

  \param ttyFD file descriptor of an open serial port (see OpenSerialPort())
  \param msgstr message string to carry the input string
  \return Number of bytes read if successful, 0 or -1 if unsuccessful.

  Reads characters from the serial port ttyFD and loads them into
  msgstr.  This port must have been previously opened and setup using
  the OpenSerialPort() and SetSeriaPort() functions.  This function is
  most often used in the context of a select() loop to watch for
  activity on one or more open serial ports, though it may be used
  generally as required.

  \sa WriteSerialPort()
*/

int  
ReadSerialPort(int ttyFD, char *msgstr)
{
  int nread = 0;

  memset(msgstr,0,sizeof(msgstr));
  nread = read(ttyFD,msgstr,sizeof(msgstr));
  if (nread < 0) 
    printf("ERROR(ReadSerialPort()) - could not read serial port - %s\n",
	   strerror(errno));
  return nread;

}

/*!
  \brief Read data from a serial port

  \param ttyFD file descriptor of an open serial port (see OpenSerialPort())
  \param ntimes number of times to read the port for flushing

   Reads the serial port ntimes to attempt to flush any extraneous port
   junk.  This port must have been previously opened and setup using the
   OpenSerialPort() and SetSeriaPort() functions.  Assumes that the
   buffer size is at least the default BUFSIZ defined in stdio.h Whether
   or not this is a good idea remains to be seen.

*/

void 
FlushSerialPort(int ttyFD, int ntimes)
{
  char *junk;
  int i;

  if (ntimes <= 0)
    return;

  if (ttyFD < 0)
    return;

  junk = (char *)malloc(BUFSIZ*sizeof(char));
  for (i=0; i<ntimes; i++) {
    read(ttyFD,junk,sizeof(junk));
    MilliSleep(50);  // sleep 50msec between flushes
  }
  free(junk);

}
