//---------------------------------------------------------------------------
//
// ISISSocket - ISIS client/server socket communications utilities
//
// Contents:
//   InitISISServer()    - Initialize ISIS Server socket address database
//   OpenClientSocket()  - Open/initialize an ISIS client socket
//   ReadClientSocket()  - Read a message from the client socket
//   SendToISISServer()  - Send a message to the ISIS server
//   ReplyToRemHost()    - Send a reply message to a remote host other than an ISIS 
//   CloseClientSocket() - Close a client socket
//
// Function prototypes, required system header files, and global
// scope variables are defined in the isissocket.h header.
//
// Author:
//   R. Pogge, OSU Astronomy Dept
//   pogge@astronomy.ohio-state.edu
//   2003 September 10
//
//

/*!
  \file isissocket.c
  \brief ISIS client socket utilties.

  ISIS clients and servers use Unix Datagram Protocol (UDP) sockets for
  interprocess communications.  This is a set of simple functions for
  using UDP sockets with and ISIS client application.  They encapsulate
  a number of useful basic functions for opening, closing, reading, and
  sending datagrams via sockets, relieving writers of ISIS client
  applications from the pain of getting all the arcane socket(), bind(),
  etc. calls right.

  To make communications with a fixed ISIS server more efficient, we
  store the server's socket address database in the isisclient struct
  using the InitISISServer() function.  This way we do not have to pay
  the overhead of resolving the hostname (e.g., by querying a DNS server
  or translating an IP number) each time we send a datagram to an ISIS
  server.

  Similarly, we also build a sockaddr_in struct inside of isisclient
  when we receive data from a remote non-ISIS client.

  \author R. Pogge, OSU Astronomy Dept. (pogge@astronomy.ohio-state.edu)
  \date 2003 September 10

  Overhauled 2004 July 22
*/

#include "isisclient.h" // ISIS client master header file

/*!
  \brief Initialize the ISIS server address information database.

  \param client pointer to an isis_client struct
  \return 0 if successful, -1 if cannot resolve the server hostname

  Uses the information in client->isisHost and client->isisPort to setup
  the network socket communications database for an ISIS server.

  To communicate with an active ISIS server, a client application needs
  to build a socket address database (sockaddr_in struct) with the
  correct host addressing information for the ISIS server.  This
  information is stored in the isisAddr, a sockaddr_in struct that is a
  member of the isis_client struct.  This routine handles host name
  resolution and builds the database.  We do this once so that we can
  store all data resulting from DNS lookups, so we don't have to rebuild
  this info every time (saving time).

  \sa SendToISISServer()
*/

int
InitISISServer(isisclient_t *client)
{
  struct hostent *host;

  // translate the server hostname into an IP address 

  if (!(host=gethostbyname(client->isisHost))){
    printf("ERROR(InitISISServer): cannot resolve ISIS server hostname %s - %s\n",
	   client->isisHost,hstrerror(h_errno));
    return -1;
  }

  // Setup the server's socket address database 

  client->isisAddr.sin_port = htons(client->isisPort);
  client->isisAddr.sin_family = AF_INET;
  memcpy(&client->isisAddr.sin_addr,host->h_addr, host->h_length);

  return 0;

}

/*!
  \brief Open an ISIS client socket for this client application.

  \param client pointer to an isisclient struct
  \return 0 if success, -1 if an error.

  Open a UDP (datagram) network socket and bind it to a specific port on
  localhost for subsequent communications with an active ISIS server.
  Puts the file descriptor to the open socket in the #FD member of
  the client struct.  ISIS communications uses connectionless datagram
  sockets.

  Once the client socket is opened and bound to its port, the global
  #ISISclient sockaddr_in struct is created with the socket address
  database.  This way we do not need to regenerate the database each
  time we read the socket.

  If port=0 (deprecated), it assigns the client the next free port.
  Technically this is allowed, and the standard ISIS server will
  know how to route messages to this client, but if the client is
  being run in a standalone mode, it will be difficult for the remote
  application to know which port the client's socket is bound to.

  \sa CloseClientSocket()
*/

int
OpenClientSocket(isisclient_t *client)
{
  struct sockaddr_in cls;
  int cls_len;

  // If the socket is already open, close it as a precaution

  CloseClientSocket(client);

  // Open a datagram (UDP) socket 

  if ((client->FD=socket(AF_INET,SOCK_DGRAM,0)) < 0 ) {
    printf("ERROR(OpenClientSocket): Cannot open client socket - %s\n",
	   strerror(errno));
    client->FD = -1;
    return -1;
  }

  // Setup an address info database for our client-side socket.  This is
  // needed only temporarily for socket port binding.
   
  cls.sin_family = AF_INET;
  cls.sin_addr.s_addr = htonl(INADDR_ANY); // generic "any" address = localhost for us
  cls.sin_port = htons(client->Port);      // Client port, 0=any free port

  // Bind our client socket to the specified localhost port

  if (bind(client->FD, (struct sockaddr *) &cls, sizeof(cls)) < 0) {
    printf("ERROR (OpenClientSocket): Cannot bind client socket port - %s\n",
	   strerror(errno));
    CloseClientSocket(client);
    return -1;
  }

  // Get the socket name info 

  if (client->Port == 0) {
     if (getsockname(client->FD, (struct sockaddr *) &cls, 
		     (socklen_t *) &cls_len) < 0) {
       printf("ERROR(OpenClientSocket): Cannot get client socket info - %s\n",
	      strerror(errno));
       CloseClientSocket(client);
       return -1;
     }

     // Let the user know what our port is if using dynamic port allocation 

     if (client->Port == 0) 
       client->Port = ntohs(cls.sin_port);
  }

  // All done, indicate success

  return 0;

}

/*!
  \brief Read a message waiting on the ISIS client socket

  \param client pointer to an isisclient struct
  \param msgstr Message string to load, size \<= #ISIS_MSGSIZE
  \return Number of bytes read if successful, -1 if an error.

  Reads pending input on the client socket into msgstr.  This routine
  does no message format validation, and assumes that msgstr is a
  well-formed and properly terminated IMPv2 message string.

  The hostname and port number of the sending host are extracted and put
  into the hostname and port variables.  This allows the calling
  application to craft replies to a remote host and send them back
  directly if the application is a "standalone" client.  If instead the
  application is an ISIS client, it should send any replies back to the
  ISIS server (see InitISISServer() and SendToISISServer() functions).
  hostname is returned as an unresolved IP address.

  \sa SendToISISServer(), ReplyToRemHost()
*/

int
ReadClientSocket(isisclient_t *client, char *msgstr)
{
  int nread=0;
  int rem_len;
  char inetstr[INET6_ADDRSTRLEN];

  // If we don't have an active file descriptor, we can't do this...

  if (client->FD <= 0) {
    printf("ERROR(ReadClientSocket): No client socket open.\n");
    return -1;
  }

  // Clear the incoming messge string

  memset(msgstr,0,ISIS_MSGSIZE);
  rem_len = sizeof(client->remAddr);

  // Read the socket port with recvfrom()

  nread = recvfrom(client->FD, msgstr, ISIS_MSGSIZE, 0,
		   (struct sockaddr *) &client->remAddr, 
		   (socklen_t *) &rem_len);

  if (nread < 0)
    printf("ERROR(ReadClientSocket): network socket recvfrom() error - %s\n",
	   strerror(errno));

  // Extract the hostname and port of the sender from the sockaddr_in struct

  client->remPort = ntohs(client->remAddr.sin_port);
  strcpy(client->remHost,inet_ntop(client->remAddr.sin_family,
				   &client->remAddr.sin_addr,
				   inetstr,
				   sizeof(inetstr)));
  return nread;

}

/*!
  \brief Send a message to the active ISIS server.

  \param client pointer to an isisclient struct
  \param msgstr message string to transmit, size \<= #ISIS_MSGSIZE
  \return Number of bytes transmitted if successful, 0 if no socket has
  been opened or attempting to send a zero-length string, -1 if an error
  occurred.

  Transmits "msgstr" to the ISIS server via the ISIS client socket.
  This routine does no message format validation, and assumes that
  msgstr is a well-formed and properly terminated IMPv2 protocol message
  string (see ISISMessage()).

  The ISIS server address database must have been initialized by calling
  InitISISServer().  The ISISserver sockaddr_in struct is defined in
  global scope in the isissocket.h header file.

  If sending a message to an ISIS node other than the ISIS server proper,
  use the ReplyToRemHost() function.

  \sa ReplyToRemHost()
*/

int
SendToISISServer(isisclient_t *client, char *msgstr)
{
  int nsent=0;

  if (strlen(msgstr)<0)
    return 0;

  if (client->FD <= 0) {
    printf("ERROR(SendToISISServer): No client socket open.\n");
    return -1;
  }

  nsent = sendto(client->FD,msgstr,strlen(msgstr),0,
		 (struct sockaddr *) &client->isisAddr, 
		 sizeof(client->isisAddr));

  if (nsent < 0)
    printf("ERROR(SendToISISServer): network socket sendto() error - %s\n",
	   strerror(errno));

  return nsent;

}

/*!
  \brief Reply to a remote host that sent us a previous message.

  \param client Pointer to an isisclient struct
  \param msgstr Message string to transmit, size<#ISIS_MSGSIZE
  \return Number of bytes sent if successful, 0 if no socket has been
  opened or attempting to send a zero-length string, -1 if an error
  occurred.

  Transmits "msgstr" to the last remote host (client->remHost:remPort)
  that contacted the application.  This routine presumes that the
  receiving host knows what to make of an ISIS-format message string
  (e.g., generated by ISISMessage()).  It makes no attempt to validate
  the contents of msgstr, assuming that was done upstream.

  If sending a message to the ISIS server proper, use the SendToISISServer()
  function.

  \sa SendToISISServer()
*/

int
ReplyToRemHost(isisclient_t *client, char *msgstr)
{
  int nsent=0;

  if (strlen(msgstr)<0)
    return 0;

  if (client->FD <= 0) {
    printf("ERROR(ReplyToRemHost): No client socket open.\n");
    return -1;
  }

  // We assume that the message is going back to the last remote host we
  // heard from.  Since standalone clients are supposed to be
  // synchronous, this is a valid assumption.

  nsent = sendto(client->FD,msgstr,strlen(msgstr),0,
		 (struct sockaddr *) &client->remAddr, 
		 sizeof(client->remAddr));

  if (nsent < 0)
    printf("ERROR(ReplyToRemHost): could not send to %s:%d - %s\n",
	   client->remHost,client->remPort,strerror(errno));

  return nsent;

}

/*!
  \brief Close the ISIS client socket

  \param client Pointer to an isisclient struct

  Closes the ISIS client socket opened with OpenClientSocket().  While
  currently very simple, in the future it could perform other
  teardown tasks as required.

  \sa OpenClientSocket()
*/

void
CloseClientSocket(isisclient_t *client) 
{
  if (client->FD > 0)
    close(client->FD);
  client->FD = -1;
}
  
