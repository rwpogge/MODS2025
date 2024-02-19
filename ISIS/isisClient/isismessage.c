//
// isismessage - ISIS message handling routines
//
// Contents:
//   int SplitMessage()  - decompose/validate a raw ISIS message
//   char *ISISMessage() - create an ISIS message string
// 

#include "isisclient.h"  // master ISIS client header

/*!
  \file isismessage.c
  \brief ISIS client message-handling utilties.

  ISIS clients pass messages to each other using the ICIMACS Messaging
  Protocol version 2 (IMPv2) syntax.  These functions are used by ISIS
  client applications to create a IMPv2-compliant message strings and to
  decompose IMPv2 messages into their component parts for further
  handling.

  IMPv2 messages take the form
  \verbatim FromID>DestID msg_type msg_body\r \endverbatim

  where:
  \arg \c FromID is the ISIS node name of the message sender (sender address)
  \arg \c DestID is the ISIS node name of the recipient (destination address)
  \arg \c msg_type is a message type code identifying the type of message being sent
  \arg \c msg_body is the message body, terminated with \\r (ASCII 13)
               
  msg_body must be one of DONE:, STATUS:, ERROR:, WARNING:, FATAL:,
  EXEC:, REQ:, with REQ: being implicit if no msg_type is given.

  msg_body is a multi-word string that may contain one of the following
  <ul> 
  <li>a command with arguments (requires that the msg_type be REQ:
      or EXEC:) 
  <li>a set ofkey=value pairs returning information in
      response to a command or info request (DONE: or STATUS:)
  <li>human-readable information text (status and error messages).
  </ul>

  For more details on the IMPv2 messaging protocol, see ...

*/

/*!
  \brief Split a raw IMPv2 message string into components.
  
  \param msgstr Raw IMPv2 message string
  \param fromID Address of the sending ISIS node
  \param destID Address of the destination ISIS node
  \param msgtype IMPv2 message type code (see #IMPv2_MsgType)
  \param msgbody Body of the message
  \return 0 if message is a valid message (proper format), -1 if invalid

  This function decomposes a raw message string into its component
  parts.  If the message does not have a valid format, it returns -1.
  Note that the validation stages do not include validating the message
  terminator (\\r = ASCII 13).  Instead we just assume proper termination
  and strip off the terminator character if present, but say nothing if
  absent.

  \sa ISISMessage()
*/

//**************************************************************************
//
// SplitMessage() - split a raw ISIS message string into parts.
//
// Arguments: 
//   char *msgstr     - raw message string
//   char *fromID     - address of the sending ISIS node
//   char *destID     - address of the destination ISIS node
//   MsgType *msgtype - message type code (see isismessage.h)
//   char *msgbody    - body of the message
//
// Returns: 0 if message is a valid message (proper format), -1 if invalid
// The value of msgtype contains the message type code, so the calling
// program must remember to pass a pointer to this function.
//
// Description:
//   ISIS nodes exchange messages in ICIMACS format:
//
//      FromID>DestID msg_ype msg_body
//
//   msg_type is one of (DONE:,STATUS:,ERROR:,WARNING:,FATAL:,EXEC:,REQ:),
//   with REQ: being implicit if absent, and msg_body is the main body of
//   the message. msg_body may be a command with arguments (if the msg_type
//   is REQ: or EXEC:), key=value pairs returning information in response
//   to a command or info request (DONE: or STATUS:), or human-readable
//   information text (status and error messages).
//
//   This routine decomposes a raw message string into its component parts.
//   If the message does not have a valid format, it returns -1.  Note that
//   the validation stages do not include validating the terminator (\r in
//   the standard).  Instead we just assume proper termination and strip
//   off the terminator character if present, but say nothing if absent.
//
//   This routine does not attempt to interpret the messages.  That is
//   left to calling application routines to handle as required.
//
// Author:
//   R. Pogge, OSU Astronomy Dept
//   pogge@astronomy.ohio-state.edu
//   2003 September 14
//
// Modification History:
//   2004 March 23 - major oops, forgot to make msgtype a pointer to
//                   type MsgType.  [rwp/osu]
//
//*************************************************************************

int
SplitMessage(char *msgstr, char *fromID, char *destID, 
	     MsgType *msgtype, char *msgbody)
{

  char addrhdr[32];  // working strings
  char typestr[64];

  int  isValid = 0;  // is the message format valid?

  // clear the parts

  *msgtype = REQ;
  memset(fromID,0,sizeof(fromID));
  memset(destID,0,sizeof(destID));
  memset(msgbody,0,sizeof(msgbody));

  // first token better be an address header with a > embedded

  sscanf(msgstr,"%s %[^\n]",addrhdr,msgbody);

  if (strstr(addrhdr,">")) {
    sscanf(addrhdr,"%[^>]>%s",fromID,destID);

    // A message is only valid if the sender ID, destination ID, and
    // basic message body are not blank.

    if ((strlen(fromID)>0) && (strlen(destID)>0) && (strlen(msgbody)>0)) 
      isValid = 1;
    else
      isValid = 0;

  }

  // We have a valid message, [ull the rest of it apart.

  if (isValid) {

    // remove any terminators from the message string

    if (msgstr[strlen(msgstr)-1]=='\n') msgstr[strlen(msgstr)-1]='\0';
    if (msgstr[strlen(msgstr)-1]=='\r') msgstr[strlen(msgstr)-1]='\0';

    // decompose into address header, message type and message body

    sscanf(msgstr,"%s %s %[^\n]",addrhdr,typestr,msgbody);

    // set the message type - REQ is implicit if not one of the others.

    if (strcasecmp(typestr,"STATUS:")==0)
      *msgtype = STATUS;
    
    else if (strcasecmp(typestr,"DONE:")==0)
      *msgtype = DONE;

    else if (strcasecmp(typestr,"ERROR:")==0)
      *msgtype = ERROR;

    else if (strcasecmp(typestr,"WARNING:")==0)
      *msgtype = WARNING;

    else if (strcasecmp(typestr,"FATAL:")==0)
      *msgtype = FATAL;

    else if (strcasecmp(typestr,"EXEC:")==0)  
      *msgtype = EXEC;

    else if (strcasecmp(typestr,"REQ:")==0)
      *msgtype = REQ;

    else {  // assume and implicit REQ:, re-extract mesgbody
      *msgtype = REQ;
      sscanf(msgstr,"%s %[^\n]",addrhdr,msgbody);
    }

  }
  else {
    return -1;   // invalid message format
  }

  return 0;  // valid message format
}

/*!
  \brief Create an IMPv2 message string.

  \param fromID ISIS node name of the client application
  \param destID ISIS node name of the intended recipient
  \param msgtype IMPv2 message type code (see #IMPv2_MsgType)
  \param msgbody Body of the message to create
  \return a character pointer to the message string.

  Creates a message string in the proper IMPv2 format with
  the correct termination (\\r = ASCII 13).

  \sa SplitMessage()

*/

//**************************************************************************
//
// ISISMessage() - create a valid message from components
//
// Arguments:
//   char *fromID    - ISIS node name of the sending process (us)
//   char *destID    - ISIS node name of the recipient
//   MsgType msgtype - message type code (see isismessage.h)
//   char *msgbody   - body of the message to send
//
// Returns: 
//   Pointer to a string with the message.
//
// Description:
//   Creates an ISIS message string in the proper ICIMACS format with
//   the correct termination (\r = ASCII 13).
// 
// Author:
//   R. Pogge, OSU Astronomy Dept
//   pogge@astronomy.ohio-state.edu
//   2003 September 14
//
// Modification History:
//
//*************************************************************************

char 
*ISISMessage(char *fromID, char *destID, MsgType msgtype, char *msgbody)
{
  static char tmpstr[ISIS_MSGSIZE];  // working string for message

  memset(tmpstr,0,sizeof(tmpstr));  // fill with nulls.

  switch(msgtype) {
  case EXEC:
    sprintf(tmpstr,"%s>%s EXEC: %s\r",fromID,destID,msgbody);
    break;

  case STATUS:
    sprintf(tmpstr,"%s>%s STATUS: %s\r",fromID,destID,msgbody);
    break;

  case DONE:
    sprintf(tmpstr,"%s>%s DONE: %s\r",fromID,destID,msgbody);
    break;

  case ERROR:
    sprintf(tmpstr,"%s>%s ERROR: %s\r",fromID,destID,msgbody);
    break;

  case WARNING:
    sprintf(tmpstr,"%s>%s WARNING: %s\r",fromID,destID,msgbody);
    break;

  case FATAL:
    sprintf(tmpstr,"%s>%s FATAL: %s\r",fromID,destID,msgbody);
    break;

  case REQ: // all REQ:'s are sent as implicit REQ:'s by convention
  default:  
    sprintf(tmpstr,"%s>%s %s\r",fromID,destID,msgbody);
    break;

  }
  return tmpstr;

}

