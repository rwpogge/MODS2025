//
// clients.c - Utilities for creating/maintaining the client host table
//
// Contents:
//   initHostTable() Initialize the server client host table
//   updateHosts()   Update the server client host table 
//   removeHost()    Remove an entry from the client host table
//   isKnownHost()   Verify that a host is in the client host table
//   printHosts()    Print the contents of the client host table on stdout.
//   hostInfo()      Report host information to a remote client
//
// Author:
//   R. Pogge, OSU Astronomy Dept.
//   pogge@astronomy.ohio-state.edu
//   2003 January 8
//
// Modification History:
//   2009 March 18 - updates for v2 [rwp/osu]
//

/*!
  \file clients.c
  \brief Server client host table functions.

  These routines are used by the server to create and manipulate the
  contents of its dynamic client host table.

  Functions:
  \arg initHostTable() Initialize the server client host table
  \arg updateHosts()   Update the server client host table 
  \arg removeHost()    Remove an entry from the client host table
  \arg isKnownHost()   Verify that a host is in the client host table
  \arg printHosts()    Print the contents of the client host table on stdout.
  \arg hostInfo()      Report host information to a remote client
*/

#include "isisserver.h"

/*!
  \brief Initialize the server client host table.
  
  Initializes the contents of the ISIS client table, clearing all host
  entries.  Must be called before filling the client table either at
  server startup or restart.
*/

void 
initHostTable()
{
  int i;
  isis.numClients = 0;
  for (i=0; i<MAXCLIENTS; i++) {
    memset(clientTab[i].ID,0,sizeof(clientTab[i].ID));
    clientTab[i].method = UNASSIGNED;
    clientTab[i].fd     = UNASSIGNED;
    clientTab[i].addr   = UNASSIGNED;
    clientTab[i].port   = UNASSIGNED;
    clientTab[i].tstamp = -1.0;
  }
}

/*!
  \brief Update the client host table with client info.
  \param hostID new host's ID (IMPv2 format)
  \param method new host's transport method (SERIAL, SOCKET, etc.)
  \param fd new host's port file descriptor 
  \param addr new host's 32-bit IP address if method=SOCKET
  \param port new host's port number if method=SOCKET

  \return The host table index assigned to this client if successful, or
  ERR_HOSTS_FULL if the host table is full.

  Searches the Client Table (clientTab struct), and either adds or updates
  the table with the incoming host info.  Because it changes the
  contents of global variables, this particular routine is not very
  thread-safe without lots of work to wrap mutex_ bits around things.

  See isKnownHost() for how to test to see if a recipient is known
  to the system.
*/

int 
updateHosts(char *hostID, int method, int fd, long addr, int port)
{
  int i;
  char testStr[SHORT_STR_SIZE];
  char logStr[MED_STR_SIZE];
  double timeStamp;
  char inetStr[INET6_ADDRSTRLEN];
  struct sockaddr_in client;

  // Copy the hostID into all upper case: this is how we store
  // all hostIDs in the table
  
  strcpy(testStr,hostID);
  upperCase(testStr);

  // Make a numerical timeStamp 

  timeStamp = sysTimeStamp();

  // First Pass:  if this is a known host, update its information 
  // and return immediately if it was found 

  if (isis.numClients > 0) { 
    for (i=0; i<MAXCLIENTS; i++) {
      if (strcmp(testStr,clientTab[i].ID)==0) {
	clientTab[i].method = method;
	clientTab[i].fd     = fd;
	clientTab[i].addr   = addr;
	clientTab[i].port   = port;
	clientTab[i].tstamp = timeStamp;
	return (i);
      }
    }
  }

  // If we got here, either this is the first client, or there were
  // no matches on the first pass.

  if (isis.numClients == MAXCLIENTS) { // the client host table is full 
    return(ERR_HOSTS_FULL);
  
  } 
  else {

    // Make a new client table entry in the first open slot in the table.
    
    for (i=0; i<MAXCLIENTS; i++) {
      if (clientTab[i].method == UNASSIGNED) {
	strcpy(clientTab[i].ID,testStr);
	clientTab[i].method = method;
	clientTab[i].fd     = fd;
	clientTab[i].addr   = addr;
	clientTab[i].port   = port;
	clientTab[i].tstamp = timeStamp;
	isis.numClients++ ;

	// Note this in the log
	
	switch (method) {
	case SERIAL:
	  sprintf(logStr,"Added Serial Client %s on device %s",
		  testStr,ttyTab[port].devName);
	  break;
	  
	case SOCKET:
	  client.sin_family = AF_INET;
	  client.sin_addr.s_addr = htonl(addr) ;
	  client.sin_port = htons(port) ;
	  sprintf(logStr,"Added UDP Client %s on host %s:%d",
		  testStr,
		  inet_ntop(client.sin_family,&client.sin_addr,inetStr,
			    sizeof(inetStr)),
		  port);
	  break;
	}
	logMessage(logStr);
	return(i);
      }
    }
  }

  // If we got here, the host table must be full

  return(ERR_HOSTS_FULL);

}

/*!
  \brief Remove an entry from the client host table
  \param hostID Name of the client host to remove
  
  \return 0 if successful, or ERR_UNKNOWN_HOST if hostID was not in the table.

  Removes a single host by name from the ISIS client table.
  See InitHostTable() for how to clear the entire table all at once.

  Since the number of allowed clients is expected to remain a small
  number, repacking the table to remove gaps does not offer any
  economies at present, but someday if it is decided it is necessary,
  the repacking should go in this routine.
*/

int 
removeHost(char *hostID)
{
  int i;

  if (isis.numClients == 0) return(0);

  // Search for hostID and clear its entries if present 

  for (i=0; i<MAXCLIENTS; i++) {
    if (strcasecmp(clientTab[i].ID,hostID)==0) {
      memset(clientTab[i].ID,0,sizeof(clientTab[i].ID));
      clientTab[i].method = UNASSIGNED;
      clientTab[i].fd     = UNASSIGNED;
      clientTab[i].addr   = UNASSIGNED;
      clientTab[i].port   = UNASSIGNED;
      clientTab[i].tstamp = -1.0;
      isis.numClients-- ;
      return (0);
    }
  } 

  // If we got here, we were asked to remove an unknown host 

  return(ERR_UNKNOWN_HOST);

}

/*!
  \brief Verify that client is a known host (i.e., in the client table)
  \param hostID Client host name to test
  
  \return Host table index of the recipient if valid, or HostUnknown if not.

  Searches the server client table in clientTab struct, and returns the
  index in the host table of the named client.  Called by the various
  message handlers attached to the select() event handler.

*/

int 
isKnownHost(char *hostID)
{
  int i;
  char testStr[SHORT_STR_SIZE];

  // Oops! No clients, so it is by unknown by default

  if (isis.numClients == 0) 
    return(ERR_UNKNOWN_HOST);

  // Is hostID in the table?  Work with an uppercase copy.
  
  strcpy(testStr,hostID);
  upperCase(testStr);

  for (i=0; i<MAXCLIENTS; i++) {
    if (strcmp(testStr,clientTab[i].ID)==0)
      return (i);
  }

  // No matches, unknown host 

  return(ERR_UNKNOWN_HOST);

}

/*!
  \brief Prints the server's client host table on stdout.
  \param hostID Name of the host to print.  If "all", print all hosts.

  Prints a table of the clients currently connected to the server onto
  stdout (i.e., onto the server's console window).

  For output tailored for sending back to remote clients (i.e., output
  as an IMPv2-conformal message string), see hostInfo().
*/

void
printHosts(char *hostID)
{
  int iHost;                      // client host table index of hostID 
  double tstamp0;                 // timeStamp now                     
  double idleTime;                // time host has been idle (seconds) 

  char testStr[SHORT_STR_SIZE];   // strings for network socket info   
  char inetStr[INET6_ADDRSTRLEN];
  struct sockaddr_in client;

  int i;

  // If no clients, report this and return 

  if (isis.numClients == 0) {
    printf("NumClients=0, No clients currently connected\n");
    return;
  }

  // Get system timeStamp now.  Data is seconds since UTC 1970 Jan 01 

  tstamp0 = sysTimeStamp();

  // If hostID = ALL, print the entire host table 

  upperCase(hostID);  // force uppercase for test

  // Info is requested on all clients

  if (strcasecmp(hostID,"ALL")==0) {
    printf("NumClients=%d MaxClients=%d\n",isis.numClients,MAXCLIENTS);
    for (iHost=0; iHost<MAXCLIENTS; iHost++) {
      switch(clientTab[iHost].method) {

      case UNASSIGNED:
	break;

      case SERIAL:
	idleTime = tstamp0 - clientTab[iHost].tstamp;
	printf("  Host%d: ID=%s Method=Serial ttyPort=%d ttyDevice=%s IdleTime=%.3f\n",
	       iHost,clientTab[iHost].ID,
	       clientTab[iHost].port,
	       ttyTab[clientTab[iHost].port].devName,
	       idleTime);
	break;

      case SOCKET:
	idleTime = tstamp0 - clientTab[iHost].tstamp;
	client.sin_family = AF_INET;
	client.sin_addr.s_addr = htonl(clientTab[iHost].addr) ;
	client.sin_port = htons(clientTab[iHost].port) ;
	
	printf("  Host%d: ID=%s Method=UDP udpAddr=%s udpPort=%d IdleTime=%.3f\n",
	       iHost,clientTab[iHost].ID, 
	       inet_ntop(client.sin_family,&client.sin_addr,inetStr,
			 sizeof(inetStr)),
	       clientTab[iHost].port,
	       idleTime);
	break;

      default:
	break;

      } // end of method switch 

    } // end of loop over clients 

    return;


  } 
  else {

    // Caller wants info on a particular client host

    if ((iHost=isKnownHost(hostID))==ERR_UNKNOWN_HOST) {
      printf("ERROR: Unknown Host %s\n",hostID);
      return;
    }

    switch (clientTab[iHost].method) {

    case UNASSIGNED:
      printf("Host=%s (Client=%.2d) is currently unassigned\n",
	     clientTab[iHost].ID,iHost);
      break;

    case SERIAL:
      idleTime = tstamp0 - clientTab[iHost].tstamp;
      printf("Host=%s Method=Serial ttyPort=%d Device=%s IdleTime=%.3f Client=%.2d\n",
	     clientTab[iHost].ID, clientTab[iHost].port,
	     ttyTab[clientTab[iHost].port].devName, idleTime, iHost);
      break;

    case SOCKET:
      idleTime = tstamp0 - clientTab[iHost].tstamp;
      client.sin_family = AF_INET;
      client.sin_addr.s_addr = htonl(clientTab[iHost].addr) ;
      client.sin_port = htons(clientTab[iHost].port) ;
      
      printf("Host=%s Method=UDP udpAddr=%s udpPort=%d IdleTime=%.3f Client=%.2d\n",
	     clientTab[iHost].ID, 
	     inet_ntop(client.sin_family,&client.sin_addr,inetStr,
		       sizeof(inetStr)),
	     clientTab[iHost].port, idleTime, iHost);
      break;

    default:
      break;

    } // end of method switch 
    
  } // end of options 

  return;

}

/*!
  \brief Report host information to a remote client
  \param hostID Name of the host of interest.  If ALL, send a list of all hosts.
  \param reply String to contain the information.

  Generates an IMPv2 message string containing the info the server hass
  on hostID.  If hostID="ALL" it will instead create a string giving a
  concise listing of all known hosts.

  For detailed output tailored for the server's command console window
  (i.e., printing on stdout), see printHosts().
*/

void
hostInfo(char *hostID, char *reply)
{
  int i;
  char inetStr[INET6_ADDRSTRLEN];
  struct sockaddr_in client;
  int iHost;
  double tstamp0, idleTime;

  // clear the reply string 

  memset(reply,0,sizeof(reply));

  if (isis.numClients == 0) {
    sprintf(reply,"DONE: HOST No hosts currently connected to the ISIS server.\n");
    return;
  }

  // If hostID = ALL, print the entire host table 

  upperCase(hostID);  // force to uppercase for tests

  if (strcasecmp(hostID,"ALL")==0) {
    sprintf(reply,"DONE: HOST numHosts=%d maxHosts=%d",isis.numClients,MAXCLIENTS);
    for (i=0; i<MAXCLIENTS; i++) {
      if (clientTab[i].method != UNASSIGNED)
	sprintf(reply,"%s host%d=%s",reply,i,clientTab[i].ID);
    }
  } else {
    if ((iHost=isKnownHost(hostID))==ERR_UNKNOWN_HOST) {
      sprintf(reply,"ERROR: HOST Unknown Host %s",hostID);
      return;
    }

    tstamp0 = sysTimeStamp();

    switch (clientTab[iHost].method) {

    case UNASSIGNED:
      sprintf(reply,"ERROR: HOST Host%d=%s Method%d=None",
	      iHost,clientTab[iHost].ID,iHost);
      break;

    case SERIAL:
      idleTime = tstamp0 - clientTab[iHost].tstamp;
      sprintf(reply,
	      "DONE: HOST Host%d=%s Method%d=Serial ttyPort%d=%d Device%d=%s IdleTime%d=%.3f",
	      iHost, clientTab[iHost].ID, 
	      iHost,
	      iHost, clientTab[iHost].port,
	      iHost, ttyTab[clientTab[iHost].port].devName, 
	      iHost, idleTime);
      break;

    case SOCKET:
      idleTime = tstamp0 - clientTab[iHost].tstamp;
      client.sin_family = AF_INET;
      client.sin_addr.s_addr = htonl(clientTab[iHost].addr) ;
      client.sin_port = htons(clientTab[iHost].port) ;
      
      sprintf(reply,
	      "DONE: HOST Host%d=%s Method%d=UDP udpAddr%d=%s udpPort%d=%d IdleTime%d=%.3f",
	      iHost, clientTab[iHost].ID, 
	      iHost, 
	      iHost, inet_ntop(client.sin_family,&client.sin_addr,inetStr,
			       sizeof(inetStr)),
	      iHost,clientTab[iHost].port, 
	      iHost, idleTime);
      break;

    default:
      sprintf(reply,"ERROR: HOST unsupported transport method %d",
	      clientTab[iHost].method);
      break;

    } // end of method switch 
    
  } // end of options 

  return;

}
