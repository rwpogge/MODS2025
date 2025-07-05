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
    TCPSocket sock(servAddress, servPortID);

  } catch(SocketException &e) {
    cerr << e.what() << endl;
    exit(1);
  }

  return 0;
}
