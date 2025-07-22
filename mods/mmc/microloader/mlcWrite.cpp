/*
 */
#include "../include/ISLSocket.h"  // For Socket and SocketException
#include <iostream>           // For cerr and cout
#include <cstdlib>            // For atoi()
using namespace std;

int mlcWrite(char cmdString[])
{
  int cmdStringLen = strlen(cmdString);   // Determine input length

  try {
    extern TCPSocket sock;
    // Send the string to the echo server
    sock.send(cmdString, cmdStringLen);

  } catch(SocketException &e) {
    cerr << e.what() << endl;
    return(-1);
  }

  return 0;
}
