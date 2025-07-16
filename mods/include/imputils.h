#ifndef IMPUTILS_H
#define IMPUTILS_H

/*!
  \file imputils.h
  \brief IMPv2 Message Utilties header file.

  ...

*/

// System header files

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>
#include <sys/file.h>
#include <sys/types.h> 
#include <sys/stat.h>
#include <sys/types.h> 

// IMPv2 message size parameters

#define IMPv2_MSGSIZE 2048   //!< Maximum size of an IMPv2 message string
#define IMPv2_NODESIZE   9   //!< Maximum size of an IMPv2 host node name

//----------------------------------------------------------------
//
// IMPv2 Message Types
//

/*!
  \enum impv2_msgtype
  
  This enum is used to define the IMPv2 message type codes.
*/

/*!
  \var typedef enum impv2_msgtype impv2_msgtype_t

  impv2_msgtype_t is a global typedef used by imputils and any user
  routines that use IMPv2 message-type codes.
*/

// Message Types:

typedef enum impv2_msgtype {
  REQ,     //!< Command Request
  EXEC,    //!< Executive Command Request
  STATUS,  //!< Informational Status message
  DONE,    //!< Command completion acknowledgment
  ERROR,   //!< Error message
  WARNING, //!< Warning message
  FATAL    //!< Fatal error condition message
} impv2_msgtype_t;  

//----------------------------------------------------------------
//
// IMPv2 Message Struct
//

/*!
  \brief IMPv2 message struct

  IMPv2 messages have the format:

  \verbatim srcID>destID MsgType MsgBody \endverbatim

  where MsgType is one of DONE:, STATUS:, ERROR:, WARNING:, FATAL:,
  EXEC:, or REQ:, with REQ: being implicit if absent, and MsgBody is
  the main body of the message.  MsgBody may be a command with
  arguments (if MsgType is REQ: or EXEC:), key=value pairs returning
  information in response to a command or info request (DONE: or
  STATUS:), or human-readable information text (status and error
  messages).

*/

typedef struct impv2_msg {
  char srcID[IMPv2_NODESIZE];  //!< IMPv2 node name of the sender (source)
  char destID[IMPv2_NODESIZE]; //!< IMPv2 node name of the intended recipient (destination)
  impv2_msgtype_t MsgType;     //!< IMPv2 message type code
  char MsgBody[IMPv2_MSGSIZE]; //!< Body of the message
} impv2_msg_t;

// Function prototypes

char *IMPv2CreateMessage(impv2_msg_t *impmsg);
int IMPv2SplitMessage(char *msgstr, impv2_msg_t *impmsg);
char *IMPv2GetInfo();

#endif  // IMPUTILS_H


