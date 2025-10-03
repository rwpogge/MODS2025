/*!
  \mainpage islmlynxShm - ISLMicroLYNX All-purpose client for MODS 

  \author Ray Gonzalez, OSU Dept. of Astronomy (rgonzale@astronomy.ohio-state.edu - retired)
  \date 2008 May 01
  \section Usage

  Usage: 
  islmlynxShm ?[IP addr]? ?[Port]? [command|help]
 \c command - is the command used by the Server mmcServer to Query/Set
 
  Where: \c IP addr - is mmcServer IP address number
         \c Port    - is mmcServer Port number
         \c command - is the command used by the Server mmcServer to Query/Set
                      wago/microLynx mechanisms
	 \c help    - give available commands that can be called.

  \section Introduction
   * islmlynx is a system call for communicating with the mmcServer.
   * mmcServer communicates with all MODS mechanisms and WAGOs.
    
   * islmlynxShm -> mmcServer -> microLynx controller -> mechanism motor
   * islmlynxShm -> mmcServer -> wago -> WAGO parameters

   * user and/or task hands a command to islmlynx
   * islmlynxShm passes command to the mmcServers.
   * mmcServer commands the wago/microLynx controller. 
   * wago/microLynx controller Sets wago parameter/mechanism motor.
   * wago/microLynx controller returns information to mmcServer.

<pre>
2008 May 01 - new application [rdg]
2025 Oct 03 - legacy application, deprecated [rwp/osu]
</pre>

\todo

<ul>
</ul>
*/

/*!
  \file islmlynxShm.c
  \brief islmlynx is an all purpose client that works with mmcServer
*/

#include "ISLSocket.h"        // For Socket and SocketException
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <vector>
#include <cstdlib>            // For atoi()
#include <unistd.h>           // For fork()

#include "timer.h" // Timer

using namespace std;

void helper();
timer ticker; // timer

int main(int argc, char *argv[]) {
  string servAddress;
  unsigned short servPortID;
  char *cmdString;
  int cmdStringLen;

  int childpid; // Child id

  /* the First Argument is always the command 'islmlynx' */
  if ((argc < 2) || (argc > 4)) {     // Test for correct number of arguments
    cerr << "Usage: " << argv[0] 
	 << " ['command String'|help]\n"
	 << " or " << argv[0] << " [Server 'command String']\n"
	 << " or " << argv[0] << " [Server Port 'command String']\n"
	 << "\nexample: islmlynx 'hatch close' "<< endl;
    exit(1);

  } else if(strstr(argv[1],"help") || strstr(argv[1],"HELP")) {
    helper();
    exit(0);

  } else if ( argc==2 ) { // default parameter with command string
    servAddress = "localhost";
    servPortID = 10435;
    cmdString = argv[1];                 // Second arg: Command string
    cmdStringLen = strlen(cmdString);   // Command input length

  } else if( argc==3 ) { // default parameter
    servAddress = "localhost";
    servPortID = atoi(argv[1]);         // Second arg: Server port
    cmdString = argv[2];                 // Third arg: Command string
    cmdStringLen = strlen(cmdString);   // Command input length


  } else if( argc==4 ) {
    servAddress = argv[1];                // Second arg: Server address
    servPortID = (argc == 4) ? atoi(argv[2]) : 7;  // Third arg: Server port
    cmdString = argv[3];                 // Fourth arg: Command string
    cmdStringLen = strlen(cmdString);   // Command input length

  }    

  childpid=fork();
  if ( childpid == 0) {
  try {

    TCPSocket sock(servAddress, servPortID); // Establish connection
    sock.send(cmdString, cmdStringLen);      // Sender

    char replyBuffer[4096];             // Buffer for reply string + \0
    int bytesReceived = 0;                   // Bytes read on each recv()
    
    memset(replyBuffer,0,sizeof(replyBuffer)); // Clear receive buffer
    
    if((bytesReceived=(sock.recv(replyBuffer,4095)))<= 0) { // Receive
      cerr << "islmlynxShm " << cmdString 
	   << " Unable to read via mmcServer" << endl;
      exit(1);
    }

    replyBuffer[bytesReceived] = '\0';   // Terminate the string!

    memset(replyBuffer,0,sizeof(replyBuffer));

   } catch(SocketException &e) {         // Destructor closes the socket
    cerr << e.what() << endl;
    exit(1);
  }
  }
  exit(0);
}

void helper() {
  system("xterm -fn 10x20 -geometry 80x24+0+0 -e less /home2/mods/help/helper.___ &");
}



