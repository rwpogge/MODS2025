#ifndef INSTRUTILS_H
#define INSTRUTILS_H

//
// instrutils.h - IP:SOCKET to comtrol (RJ45/RS232) Serial Port
// Parameter Table Header, with Defined MicroLynx commands
//

/*!
  \file instrutils.h
  \brief Serial (comtrol box) Port I/O Utilties header

  \author Staff, OSU Dept. of Astronomy (rgonzale@astronomy.ohio-state.edu)
  \date 2004 Mar 13 [rdg]

  \par Modification History:
<pre>
</pre>
*/

#include "params.h"
//----------------------------------------------------------------
//
// ttyport: comtrol ip:socket serial port configuration struct
//
/*!
  \brief Serial (comtrol) Port Configuration Table

  Contains the configuration of a comtrol ip:socket to serial port.

*/
typedef struct ttyport {
  char   Port[64];    //!< Port name (e.g., /dev/tty1 or host:port)
  char   ID[40];      //!< Port ID (Indentity of the module using this port)
  int    Interface;   //!< Interface Type: one of TTY_SERIAL/TTY_NETWORK/TTY_UNKNOWN
  int    FD;          //!< File descriptor of the open port, 0 = closed
  int    Speed;       //!< Port Speed if Interface=TTY_SERIAL, one of (1200,2400,4800,9600,19200,38400)
  int    DataBits;    //!< Port data bits if Interface=TTY_SERIAL, range: 5..8
  int    StopBits;    //!< Port stop bits if Interface=TTY_SERIAL, values: 1 or 2
  int    Parity;      //!< Enable parity generation/checking if Interface=TTY_SERIAL, values: 0 or 1
} ttyport_t;

// Controllers PLC Commands
#define HOME "HOME"
#define CENTER "CENTER"
#define INIT "INITIAL"
#define MOVA "mova"
#define MOVR "movr"
#define SLEW "slew"
#define STEP "step"
#define STOPPLC "sstp 1"
#define STOP "sstp 0"
#define PSTATUS "print stat"
#define PRINT "print stat"

// Controllers
#define DARKSLIDE 0
#define CALIBT 1
#define MASKI 2
#define MASKS 3
#define AGWX 5
#define AGWY 4
#define AGWPF 6
#define AGWFW 7
#define DICHROIC 8 
// Blue
#define B_TIP 9 
#define B_TILT 10
#define B_FOCUS 11
#define B_GT1 12
#define B_GT2 13
#define B_GS 14
#define B_SHUTTER 15
#define B_CAMERAPF 16
#define B_CAMERAFW 17
// Red
#define R_TIP 18 
#define R_TILT 19
#define R_FOCUS 20
#define R_GT1 21
#define R_GT2 22
#define R_GS 23
#define R_SHUTTER 24
#define R_CAMERAPF 25
#define R_CAMERAFW 26

#endif  // INSTRUTILS_H
