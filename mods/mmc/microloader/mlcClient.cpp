/*
 */

#include "../include/ISLSocket.h"  // For Socket and SocketException
#include <iostream>           // For cerr and cout
#include <cstdlib>            // For atoi()
using namespace std;

//const int RCVBUFSIZE = 512;    // Size of receive buffer
const int RCVBUFSIZE = 8192;    // Size of receive buffer

int main(int argc, char *argv[]) {
  if ((argc < 3) || (argc > 4)) {     // Test for correct number of arguments
    cerr << "Usage: " << argv[0] 
         << " <Server> <Echo String> [<Server Port>]" << endl;
    exit(1);
  }

  string servAddress = argv[1]; // First arg: server address
  char echoString[64];   // Second arg: string to echo
  sprintf(echoString,"%s\r",argv[2]);
  int echoStringLen = strlen(echoString);   // Determine input length
  unsigned short echoServPort = atoi(argv[3]);

  try {

    // Establish connection with the echo server
    TCPSocket sock(servAddress, echoServPort);
    
    // Send the string to the echo server

    sock.send(echoString, echoStringLen);
  
    char echoBuffer[RCVBUFSIZE];    // Buffer for echo string + \0
    int bytesReceived = 0;              // Bytes read on each recv()
    int totalBytesReceived = 0;         // Total bytes read
    // Receive the same string back from the server
    cout << "Received: ";               // Setup to print the echoed string
    while (totalBytesReceived < echoStringLen) {
      // Receive up to the buffer size bytes from the sender
      if ((bytesReceived = (sock.recv(echoBuffer, RCVBUFSIZE))) <= 0) {
        cerr << "Unable to read" << endl;
        exit(1);
      }
      totalBytesReceived += bytesReceived;     // Keep tally of total bytes
      echoBuffer[bytesReceived] = '\0';        // Terminate the string!
      //      rmcrlf(echoBuffer, echoBuffer);
      cout << echoBuffer;                      // Print the echo buffer
      memset(echoBuffer,0,sizeof(echoBuffer));
    }
    cout << endl;

    // Destructor closes the socket

  } catch(SocketException &e) {
    cerr << e.what() << endl;
    exit(1);
  }

  return 0;
}
