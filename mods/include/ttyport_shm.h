#ifndef TTYPORT_SHM_H
#define TTYPORT_SHM_H

//
// ttyport_shm.h - IP:SOCKET to comtrol (RJ45/RS232) Serial Port
// Parameter Table Header.
//

/*!
  \file ttyport_shm.h
  \brief Serial (comtrol box) Port I/O Utilties header

  \author Staff, OSU Astronomy Dept. (rgonzale@astronomy.ohio-state.edu)
  \date 2004 Mar 13

  \par Modification History:
<pre>
</pre>
*/

//----------------------------------------------------------------
//
// ttyport: comtrol ip:socket serial port configuration struct
//


/*!
  \brief Serial (comtrol) Port Configuration Table

  Contains the configuration of a comtrol ip:socket to serial port.

*/

typedef struct ttyport_shm {
  char   Port[64];    //!< Port name (e.g., /dev/tty1 or host:port)
  char   ID[40];      //!< Port ID (Indentity of the module using this port)
  int    Interface;   //!< Interface Type: one of TTY_SERIAL/TTY_NETWORK/TTY_UNKNOWN
  int    FD;          //!< File descriptor of the open port, 0 = closed
  int    Speed;       //!< Port Speed if Interface=TTY_SERIAL, one of (1200,2400,4800,9600,19200,38400)
  int    DataBits;    //!< Port data bits if Interface=TTY_SERIAL, range: 5..8
  int    StopBits;    //!< Port stop bits if Interface=TTY_SERIAL, values: 1 or 2
  int    Parity;      //!< Enable parity generation/checking if Interface=TTY_SERIAL, values: 0 or 1
} *ttyport_shm_t[MAX_ML];

// Serial Port Parameters

#define TTY_UNKNOWN 0 //!< Port interface unknown (should be starting default)
#define TTY_SERIAL  1 //!< Port is a direct serial device on the host computer
#define TTY_NETWORK 2 //!< Port is a serial port connected through a TCP network port server

// Ttyport_Shm Function Prototypes

  int OpenTTYPort( ttyport_shm_t []);
  int SetTTYPort(ttyport_shm_t []);
  void CloseTTYPort(ttyport_shm_t []);
  int WriteTTYPort(ttyport_shm_t [], char *);
  int ReadTTYPort(ttyport_shm_t [], char *, long);
  void FlushTTYPort(ttyport_shm_t [], int);
  int TTYMSleep(long);

#endif  // TTYPORT_SHM_H
