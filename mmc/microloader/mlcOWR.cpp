/*
 */
#include "../include/ISLSocket.h"  // For Socket and SocketException
#include <iostream>           // For cerr and cout
#include <cstdlib>            // For atoi()
using namespace std;

const int RCVBUFSIZE = 512;    // Size of receive buffer

int mlcOWR(char cmdString[], char replyBuffer[])
{
  int bytesReceived = 0;              // Bytes read on each recv()
  int cmdStringLen = strlen(cmdString);   // Determine input length
  string servAddress="localhost";
  unsigned int servPortID=10435;

  cmdString[cmdStringLen]='\r';
  try {
    
    // Establish connection with the echo server
    TCPSocket sock(servAddress, servPortID);

    // Send the string to the echo server
    sock.send(cmdString, cmdStringLen);

    if ((bytesReceived = (sock.recv(replyBuffer, RCVBUFSIZE))) <= 0) {
      cerr << "Unable to read" << endl;
      exit(1);
    }
    replyBuffer[bytesReceived] = '\0';        // Terminate the string!

  } catch(SocketException &e) {
    cerr << e.what() << endl;
    return(-1);
  }

  return 0;
}
