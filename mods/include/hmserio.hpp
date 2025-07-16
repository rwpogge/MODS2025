/**
 * @internal
 * @file hmserio.hpp
 *
 * Serial port I/O header file
 *
 * @if NOTICE
 *
 * $Id: hmserio.hpp,v 1.13 2004/05/26 01:21:24 henrik Exp $
 *
 * Copyright (c) 2002-2003 FOCUS Software Engineering Pty Ltd, Australia.
 * All rights reserved. <www.focus-sw.com>
 *
 * USE OF THIS SOFTWARE IS GOVERNED BY THE TERMS AND CONDITIONS OF A
 * SEPARATE LICENSE STATEMENT AND LIMITED WARRANTY.
 *
 * IN PARTICULAR, YOU WILL INDEMNIFY AND HOLD FOCUS SOFTWARE ENGINEERING,
 * ITS RELATED COMPANIES AND ITS SUPPLIERS, HARMLESS FROM AND AGAINST ANY
 * CLAIMS OR LIABILITIES ARISING OUT OF THE USE, REPRODUCTION, OR
 * DISTRIBUTION OF YOUR PROGRAMS, INCLUDING ANY CLAIMS OR LIABILITIES
 * ARISING OUT OF OR RESULTING FROM THE USE, MODIFICATION, OR DISTRIBUTION
 * OF PROGRAMS OR FILES CREATED FROM, BASED ON, AND/OR DERIVED FROM THIS
 * SOURCE CODE FILE.
 *
 * @endif
 */


#ifndef _HMSERIO_H_INCLUDED
#define _HMSERIO_H_INCLUDED

#ifndef __cplusplus
#  error Must use C++ to compile this module!
#endif

// HM platform detection
#include "hmplatf.h"

// Platform header
#if defined(__WIN32__)
#  include <windows.h>
#elif defined(__VXWORKS__)
#  include <ioLib.h>
#  include <selectLib.h>
#elif defined(__PALMOS__)
#  include <PalmOS.h>
#  include <SerialMgrOld.h>
#  undef send
#  if !defined(__size_t__)
#     define __size_t__
      typedef UInt32 size_t;
#  endif
#elif defined(__MACOS__) && !defined(__MACOSX__)
#  error Mac not yet supported!
#elif defined(__UNIX__)
#  include <sys/select.h> // Needed by QNX4
#  include <termios.h>
#else
#  error "Platform not yet supported. Please add a new condition to this #if block or update hmplatf.h"
#endif


/*****************************************************************************
 * Serial Port I/O interface
 *****************************************************************************/

/**
 * Serial Port class. This class implements a platform independant access
 * to serial ports.
 *
 * @version 1.1
 */
class
#if defined (_WINDLL) || defined(__DLL__)
   __declspec(dllexport)
#endif
SerialPort
{
  public:

   enum
   {
      SER_SUCCESS = 0,       ///< No error
      SER_ERR_CLASS = 0x1100,///< Serial error class
      SER_INVALID_PARAMETER, ///< Parameter invalid or not supported
      SER_API_ERROR,         ///< Underlaying API returns an error
      SER_PORT_NOT_FOUND,    ///< Serial port not found
      SER_ALREADY_OPEN,      ///< Serial port already open
      SER_NOT_OPEN,          ///< Port not opened
      SER_TIME_OUT,          ///< Operation timed out
      SER_LINE_ERROR,        ///< RS232 line error
      /// Operation not supported in current configuration
      SER_UNSUPPORTED_OPERATION,
      SER_PORT_NO_ACCESS     ///< No access rights for port (Try as root)
   };

   enum
   {
      SER_DATABITS_7 = 7, ///< 7 data bits
      SER_DATABITS_8 = 8  ///< 8 data bits
   };

   enum
   {
      SER_STOPBITS_1 = 1, ///< 1 stop bit
      SER_STOPBITS_2 = 2  ///< 2 stop bits
   };

   enum
   {
      SER_PARITY_NONE = 0, ///< No parity
      SER_PARITY_ODD  = 1, ///< Odd parity
      SER_PARITY_EVEN = 2  ///< Even parity
   };

   enum
   {
      SER_HANDSHAKE_NONE    = 0, ///< No handshake
      SER_HANDSHAKE_RTS_CTS = 1  ///< RTS/CTS hardware handshake
   };


   SerialPort();

   ~SerialPort();

   int openPort(const char * const name);

   void closePort();

   int config(long baudRate, int dataBits,
              int stopBits, int parity, int flowControl);

   int send(const char *bufPtr, size_t len, long timeOut);

   int receive(const char *bufPtr, size_t len, long timeOut);

   int receiveByte(const char *charBuf, unsigned long microTimeOut);

   int flush();

   int drain();

   int setRts();

   int clearRts();

   int isOpen();


  protected:

   int baudRate;
   int flowControl;
#if defined (__WIN32__)
   HANDLE port;
   COMMTIMEOUTS commTimeOutBlock;
   DCB savedDevCtrlBlock;
#elif defined(__VXWORKS__)
   int port;
   int channel;
#elif defined(__PALMOS__)
   Err err;
   UInt16 port;
   SerSettingsType savedSerialSettings;
#else
   int port;
   struct termios savedTermioAttr;
#endif


  private:

   // Disable default operator and copy constructor
   SerialPort &operator=(SerialPort &);
   SerialPort(const SerialPort &);

};


#endif // ifdef ..._H_INCLUDED
