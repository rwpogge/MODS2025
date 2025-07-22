/*
 */
#include "../include/ISLSocket.h"  // For Socket and SocketException
#include <iostream>           // For cerr and cout
#include <cstdlib>            // For atoi()
using namespace std;

int mlcOpen(string servAddress, unsigned short servPortID)
{
  try {

    // Establish connection with the echo server
    TCPSocket sock(servAddress, echoServPort);

  } catch(SocketException &e) {
    cerr << e.what() << endl;
    return(1);
  }

  return 0;
}

int mlcWrite(char cmdString[])
{
  int cmdStringLen = strlen(cmdString);   // Determine input length

  cmdString[cmdStringlen]='\r';
  try {

    // Send the string to the echo server
    sock.send(cmdString, cmdStringLen);

  } catch(SocketException &e) {
    cerr << e.what() << endl;
    return(-1);
  }

  return 0;
}

int mlcRead(char replyBuffer[])
{
  int bytesReceived = 0;              // Bytes read on each recv()

  try {
  
    if ((bytesReceived = (sock.recv(replyBuffer, RCVBUFSIZE))) <= 0) {
      cerr << "Unable to read" << endl;
      exit(1);
    }
      replyBuffer[bytesReceived] = '\0';        // Terminate the string!
      rmcrlf(replyBuffer, replyBuffer);

  } catch(SocketException &e) {
    cerr << e.what() << endl;
    return(-1);
  }

  return(0);
}

int mlcWriteRead(char cmdString[], char replyBuffer[])
{
  int cmdStringLen = strlen(cmdString);   // Determine input length

  cmdString[cmdStringlen]='\r';
  try {

    // Send the string to the echo server
    sock.send(cmdString, cmdStringLen);

    if ((bytesReceived = (sock.recv(replyBuffer, RCVBUFSIZE))) <= 0) {
      cerr << "Unable to read" << endl;
      exit(1);
    }
    replyBuffer[bytesReceived] = '\0';        // Terminate the string!
    rmcrlf(replyBuffer, replyBuffer);

  } catch(SocketException &e) {
    cerr << e.what() << endl;
    return(-1);
  }

  return 0;
}
