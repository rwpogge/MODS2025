//
// messages.c - message handling functions
//
// Contents:
//   handleMessage()
//   broadcastMessage()
//   sendMessage()
//
// Author:
//   R. Pogge, OSU Astronomy Dept.
//   pogge@astronomy.ohio-state.edu
//   2003 January 6
//
// Modification History:
//   2003 Jan 09 - fixed incorrect IMPv2 termination [rwp/osu]
//   2004 May 11 - added Doxygen documentation hooks [rwp/osu]
//   2009 Mar 18 - v2 updates [rwp/osu]
//

#include "isisserver.h"  

/*!
  \file messages.c
  \brief Server message handling functions.

  The ISIS server passes messages between clients using the ICIMACS
  Messaging Protocol version 2 (IMPv2).

  ... more to come ...

*/

//---------------------------------------------------------------------------
//
// handleMessage()
//

/*!
  \brief Handle messages received by the ISIS server
  \param sendHost Client table index of the sending client host
  \param destHost Client table index of the destination host
  \param msgBody Body of the message

  Figures out what to do with a message received and already
  validated by the calling procedure.  There are three possibilities:
  <ol>
  <li>It is an ISIS server command request to be passed to isisCommand()
      for processing.
  <li>It is a broadcast message to be copied out to all known ISIS client 
      hosts (passed to broadcastMessage()), as well as being processed
      by the server proper (passed to isisCommand()).
  <li>It is a message to be routed to another client host using sendMessage().
  </ol>
  This routine is designed to be safe for future multithreading.

  IMPv2 messages are terminated with return (\\r = ASCII 13), not with a
  newline (\\n = ASCII 10).  Messages prepared for further routing by
  sendMessage() must be correctly terminated.  Those printed to stdout
  do not require termination, as the printing routine will strip any
  termination and provide a \\r as required.

  BEWARE: No terminination checking is done by sendMessage()!
*/

void
handleMessage(int sendHost, int destHost, char *msgBody) 
{

  // Pointers for character strings 

  char *argStr;
  char *reply;

  // Reply handling flag (MSG_REPLY,MSG_FORWARD,MSG_ECHO,MSG_NOOP) 

  int doReply; 

  // Allocate memory for the working strings 

  reply  = (char *)malloc((ISIS_MSGSIZE)*sizeof(char));
  memset(reply,0,ISIS_MSGSIZE);

  doReply = MSG_NOOP;

  // Remove any extraneous termination characters on msgBody 

  if (msgBody[strlen(msgBody)-1]=='\n') msgBody[strlen(msgBody)-1]='\0';
  if (msgBody[strlen(msgBody)-1]=='\r') msgBody[strlen(msgBody)-1]='\0';

  // Action depends on the message recipient

  switch (destHost) {
  
  case ISIS_SERVER:

    // Message is a server command, pass to isisCommand() for processing 

    argStr = (char *)malloc((BIG_STR_SIZE)*sizeof(char));
    doReply = isisCommand(msgBody,argStr);

    switch (doReply) {

    case MSG_REPLY:
      // Return ISIS reply to sending host 
      sprintf(reply,"%s>%s %s\r",isis.serverID,clientTab[sendHost].ID,argStr);
      free(argStr);
      break;

    case MSG_ECHO:
      // Echo message body on the server console as-is (no termination) 
      sprintf(reply,"%s> %s",clientTab[sendHost].ID,msgBody);
      free(argStr);
      break;

    default:
      // No action required, return now 
      free(argStr);
      free(reply);
      return;
      break;


    } // end of doReply switch for ISIS commands 

    break;

  case BROADCAST:

    // The message is to be broadcast to all known clients except the
    // sender.  Pass the message to BroadcastMesssage() for handling.
  
    broadcastMessage(sendHost,msgBody); 
    doReply = MSG_NOOP;
    break;

  default:

    // The message is to be routed to a known client host.  We have
    // already validated the destination host in the calling program, so
    // rebuild the message and set for forwarding.
    //
    // Make sure the message is correctly terminated.
  
    sprintf(reply,"%s>%s %s\r",clientTab[sendHost].ID,clientTab[destHost].ID,msgBody);
    doReply = MSG_FORWARD;
    break;

  } // end of message handling 

  // Reply handling 

  switch (doReply) {

  case MSG_REPLY:
    sendMessage(sendHost,reply);  // reply back to sending host 
    if (reply[strlen(reply)-1]=='\r') reply[strlen(reply)-1]='\0';
    if (isis.isVerbose && isis.useCLI) printf("%s\n",reply);
    logMessage(reply);
    break;

  case MSG_FORWARD:
    sendMessage(destHost,reply);  // reply forward to destination host 
    break;

  case MSG_ECHO:
    if (isis.useCLI) printf("\n%s\n",reply);      // echo to the server console 
    break;

  default:
    break;

  } // end of doReply switch 

  // Free the reply string memory and return 

  free(reply);

  return;

}

//---------------------------------------------------------------------------
//
// broadcastMessage()
//

/*!
  \brief Broadcast a message to all clients.
  \param sendHost Client table index of the sending host
  \param msgBody Body of the broadcast message.

  Sends a broadcast message out to all known clients on the
  system.  Broadcast messages take the form of
  \verbatim XX>AL message_text\r \endverbatim
  If XX is the local host, we pass it to all ports in this
  order:
  <ol>
  <li>to each active serial port
  <li>to each active network client
  </ol>
  If XX is one of our known clients, we pass the message along
  only to ports other than the one the sending client is using.

  This is a key distinction: broadcasts are blind-sent to \em ports not
  hosts.  We have to be careful not to echo a broadcast back to the
  sender, which risks starting infinite regressions.

  Designed to be lightweight and thread safe.  All routines called by
  this (and the routine itself) must not alter global variables.  this
  means that logging is the responsibility of calling programs upstream.
*/

void
broadcastMessage(int sendHost, char *msgBody) 
{
  struct sockaddr_in client;  // network client socket info   
  int clientLen;       
  int serialFD;               // serial port file descriptor 
  int senderFD;               // sender file descriptor      
  int numSent;                // number of bytes sent        
  int i;
  char errStr[256];           // error string
  char *message;              // full mesage string

  // Which host is sending the broadcast message?

  message  = (char *)malloc((ISIS_MSGSIZE)*sizeof(char));
  memset(message,0,ISIS_MSGSIZE);

  // The sending host is the ISIS server... 

  if (sendHost == ISIS_SERVER) {
    
    // Build the properly terminated IMPv2 broadcast message string 

    sprintf(message,"%s>AL %s\r",isis.serverID,msgBody);

    // First, send to all serial ports 

    if (isis.numSerial > 0) {
      for (i=0; i<isis.numSerial; i++) {
	serialFD = ttyTab[i].fd;
	if (serialFD > 0) {
	  numSent = write(serialFD,message,strlen(message));
	  if (numSent < 0) {
	    sprintf(errStr,"ERROR: Serial Port write() Error on port %s - %s",
		    ttyTab[i].devName,strerror(errno));
	    if (isis.useCLI)
	      printf("%s\n",errStr);
	    else
	      logMessage(errStr);
	  }
	}
      }
    }

    // Now, send it out to each active network client, skipping serial ports.

    if (isis.numClients > 0) {
      for (i=0; i<MAXCLIENTS; i++) {
	if (clientTab[i].method == SOCKET) {
	  client.sin_family = AF_INET;
	  client.sin_addr.s_addr = htonl(clientTab[i].addr) ;
	  client.sin_port = htons(clientTab[i].port) ;
	  clientLen = sizeof(client);
	  numSent=sendto(isis.sockFD,message,strlen(message),0, 
		       (struct sockaddr *) &client, clientLen);
	  if (numSent < strlen(message)) {
	    sprintf(errStr,"ERROR: (broadcastMessage): Network Socket sendto() Error"
		    ", Client %d (%s) - %s",
		    i,clientTab[i].ID,strerror(errno));
	    if (isis.useCLI)
	      printf("%s\n",errStr);
	    else
	      logMessage(errStr);
	  }
	}
      }
    }

    // all done 

    free(message);
    return;

  }

  // The sending host is another client, take care not to reflect
  // it back to their port

  // First, ISIS must process the broadcast message itself 

  handleMessage(sendHost,ISIS_SERVER,msgBody);

  // Second, it must pass along the message to all known hosts
  // EXCEPT the sending host and all clients that share
  // the same port as the sending host. 
   
  sprintf(message,"%s>AL %s\r",clientTab[sendHost].ID,msgBody);

  // Subsequent actions depend on the client's transport method 

  switch (clientTab[sendHost].method) {

  case SOCKET:

    // Broadcast was from a network client, so send it to all active
    // serial ports 

    if (isis.numSerial > 0) {
      for (i=0; i<isis.numSerial; i++) {
	serialFD = ttyTab[i].fd;
	if (serialFD > 0) {
	  numSent = write(serialFD,message,strlen(message));
	  if (numSent < 0) {
	    sprintf(errStr,"ERROR: Serial Port write() Error on port %s - %s",
		    ttyTab[i].devName,strerror(errno));
	    if (isis.useCLI)
	      printf("%s\n",errStr);
	    else
	      logMessage(errStr);
	  }
	}
      }
    }
    
    // Now, send it out to each active network client, except itself,
    // and skipping serial ports

    if (isis.numClients > 0) {
      for (i=0; i<MAXCLIENTS; i++) {
	if (clientTab[i].method == SOCKET) {
	  if (i != sendHost) {
	    client.sin_family = AF_INET;
	    client.sin_addr.s_addr = htonl(clientTab[i].addr) ;
	    client.sin_port = htons(clientTab[i].port) ;
	    clientLen = sizeof(client);
	    numSent=sendto(isis.sockFD,message,strlen(message),0, 
			 (struct sockaddr *) &client, clientLen);
	    if (numSent < strlen(message)) {
	      sprintf(errStr,"ERROR: Network Socket sendto() Error, Client %d (%s) - %s",
		      i,clientTab[i].ID,strerror(errno));
	      if (isis.useCLI)
		printf("%s\n",errStr);
	      else
		logMessage(errStr);
	    }
	  }
	}
      }
    }
    break;

  case SERIAL:
    // sending host is on the serial port, first pass the
    // broadcast to all other serial ports (if only one
    // port active, skip serial passing entirely)
     
    if (isis.numSerial > 1) {
      senderFD = ttyTab[clientTab[sendHost].port].fd;
      for (i=0; i<isis.numSerial; i++) {
	serialFD = ttyTab[i].fd;
	if (serialFD != senderFD) {
	  numSent = write(serialFD,message,strlen(message));
	  if (numSent < 0) {
	    sprintf(errStr,"ERROR: Serial Port write() Error on port %s - %s",
		    ttyTab[i].devName,strerror(errno));
	    if (isis.useCLI)
	      printf("%s\n",errStr);
	    else
	      logMessage(errStr);
	  }
	}
      }
    }
    
    // Now, send it out to each active network client, skipping serial
    // ports
    
    if (isis.numClients > 0) {
      for (i=0; i<MAXCLIENTS; i++) {
	if (clientTab[i].method == SOCKET) {
	  client.sin_family = AF_INET;
	  client.sin_addr.s_addr = htonl(clientTab[i].addr) ;
	  client.sin_port = htons(clientTab[i].port) ;
	  clientLen = sizeof(client);
	  numSent=sendto(isis.sockFD,message,strlen(message),0, 
			 (struct sockaddr *) &client, clientLen);
	  if (numSent < strlen(message)) {
	    sprintf(errStr,"ERROR: Network Socket sendto() Error, Client %d (%s) - %s",
		    i,clientTab[i].ID,strerror(errno));
	    if (isis.useCLI)
	      printf("%s\n",errStr);
	    else
	      logMessage(errStr);
	  }
	}
      }
    }
    break;

  default:
    sprintf(errStr,"ERROR: unsupported transport method %d for client %d (%s)",
	    clientTab[sendHost].method,sendHost,clientTab[sendHost].ID);
    if (isis.useCLI)
      printf("%s\n",errStr);
    else
      logMessage(errStr);
    return;
    break;

  } // end of method switch 

  return;

}

//---------------------------------------------------------------------------
//
// sendMessage() 
//

/*!
  \brief Send a message string to a client.
  \param destHost Index of the destination client host in the server's client table
  \param message The message to send

  Sends the message to a client.  The message must by this point be in
  the form of a valid IMPv2 protocol message:
  \verbatim XX>YY message_text\r \endverbatim
  for passing.  The transport mechanism is based on the entries for
  destHost in the client host table.  This presumes that the calling
  routine has already validated the destination host as valid.

  The message must be properly terminated with \\r (ASCII 13) as per the
  IMPv2 message format specification.

  Designed to be lightweight and thread safe.  All routines called by
  this (and the routine itself) must not alter global variables.  this
  means that logging is the responsibility of calling programs upstream.
*/

void
sendMessage(int destHost, char *message) 
{
  struct sockaddr_in client;  // network client socket info   
  int clientLen;       
  int serialFD;               // serial port file descriptor 
  int numSent;                // number of bytes sent        
  char errStr[256];           // error string
  char clientAddr[INET6_ADDRSTRLEN];
  char inetStr[INET6_ADDRSTRLEN];

  switch (clientTab[destHost].method) {

  case SOCKET:
    client.sin_family = AF_INET;
    client.sin_addr.s_addr = htonl(clientTab[destHost].addr) ;
    client.sin_port = htons(clientTab[destHost].port) ;
    clientLen = sizeof(client);
    strcpy(clientAddr,inet_ntop(client.sin_family,&client.sin_addr,
				inetStr,sizeof(inetStr)));
    numSent=sendto(isis.sockFD,message,strlen(message),0, 
		 (struct sockaddr *) &client, clientLen);
    if (numSent < strlen(message)) {
      sprintf(errStr,"ERROR: Network Socket sendto() Error, Client %d (%s) - %s",
	      destHost,clientTab[destHost].ID,strerror(errno));
      if (isis.useCLI)
	printf("%s\n",errStr);
      else
	logMessage(errStr);
      return;
    }
    break;

  case SERIAL:
    serialFD = ttyTab[clientTab[destHost].port].fd;
    numSent = write(serialFD,message,strlen(message));
    if (numSent < 0) {
      sprintf(errStr,"ERROR: Serial Port write() Error on %s, Client %d (%s) - %s",
	      ttyTab[clientTab[destHost].port].devName,destHost,
	      clientTab[destHost].ID,strerror(errno));
      if (isis.useCLI)
	printf("%s\n",errStr);
      else
	logMessage(errStr);
      return;
    }
    break;

  default:
    sprintf(errStr,"ERROR: unsupported transport method %d for client %d (%s)",
	    clientTab[destHost].method,destHost,clientTab[destHost].ID);
    if (isis.useCLI)
      printf("%s\n",errStr);
    else
      logMessage(errStr);
    return;
    break;

  } // end of method switch 

  return;

}
