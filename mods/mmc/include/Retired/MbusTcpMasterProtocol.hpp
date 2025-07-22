/**
 * @internal
 * @file MbusTcpMasterProtocol.hpp
 *
 * @if NOTICE
 *
 * $Id: MbusTcpMasterProtocol.hpp,v 1.15 2004/01/23 22:53:54 henrik Exp $
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


#ifndef _MBUSTCPMASTERPROTOCOL_H_INCLUDED
#define _MBUSTCPMASTERPROTOCOL_H_INCLUDED

#ifndef __cplusplus
#  error Must use C++ to compile this module!
#endif


// Package header
#include "hmtcpip.h"
#include "MbusMasterFunctions.hpp"


/*****************************************************************************
 * MbusTcpMasterProtocol class declaration
 *****************************************************************************/

/**
 * MODBUS/TCP Master Protocol class
 *
 * This class realises the MODBUS/TCP master protocol. It provides
 * functions to establish and to close a TCP/IP connection to the slave as
 * well as data and control functions which can be used after a connection
 * to a slave device has been established successfully. The data and
 * control functions are organized different conformance classes. For a
 * more detailed description of the data and control functions see section
 * @ref mbusmaster.
 *
 * It is also possible to instantiate multiple instances of this class for
 * establishing multiple connections to either the same or different hosts.
 *
 * @ingroup mbusmastertcp
 * @version 1.1
 * @see mbusmaster
 * @see MbusMasterFunctions
 */
class
#if defined (_WINDLL) || defined(__DLL__)
   __declspec(dllexport)
#endif
MbusTcpMasterProtocol: public MbusMasterFunctions
{
  private:

   enum
   {
      // TCP prefix
      PREFIX_LEN = 6,
      // Header is TCP prefix + 1 byte function + 1 byte address
      HDR_LEN = PREFIX_LEN + 2,
      // Frame is like header for this protocol
      FRAME_LEN = HDR_LEN,
      // TCP exception message length is frame + 1 byte exception code
      EXC_MSG_LEN = FRAME_LEN + 1,
      // TCP max. message size is data size + frame length:
      MAX_MSG_SIZE = MAX_DATA_SIZE + FRAME_LEN
   };

   char bufferArr[MAX_MSG_SIZE];


  protected:

   SOCKET tcpSocket;
   unsigned short portNo; // TCP port number
   struct sockaddr_in hostAddress;


  public:

   MbusTcpMasterProtocol();

   /**
    * @name TCP/IP Connection Management Functions
    */
   //@{

   int openProtocol(const char * const hostName);

   virtual void closeProtocol();

   virtual int isOpen();

   int setPort(unsigned short portNo);

   /**
    * Returns the TCP port number used by the protocol.
    *
    * @return Port number used by the protocol
    */
   unsigned short getPort() { return (portNo); }

   //@}


   /**
    * @name Advantec ADAM 5000/6000 Series Commands
    * @ingroup devicespecific
    */
   //@{

   int adamSendReceiveAsciiCmd(const char * const commandSz, char* responseSz);

   //@}

   /**
    * @name MicroLYNX Controller Series Commands
    * @ingroup devicespecific
    */
   //@{

   int mlcSendReceiveAsciiCmd(const char * const commandSz, char* responseSz);

   //@}



  protected:

   int openProtocol();

   virtual int deliverMessage(int address, int function,
                              char sendDataArr[], int sendDataLen,
                              char rcvDataArr[], int rcvDataLen);

   int refreshConnection();


  private:

   virtual int transceiveMessage(int address, int function,
                                 char sendDataArr[], int sendDataLen,
                                 char rcvDataArr[], int rcvDataLen);


  private:

   // Disable default operator and copy constructor
   MbusTcpMasterProtocol &operator= (MbusTcpMasterProtocol &);
   MbusTcpMasterProtocol (const MbusTcpMasterProtocol &);

};


#endif // ifdef ..._H_INCLUDED
