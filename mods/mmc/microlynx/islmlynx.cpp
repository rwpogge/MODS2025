/*!
  \mainpage islmlynx - ISLMicroLYNX All purpose client for MODS 

  \author Ray Gonzalez, OSU Dept. of Astronomy (rgonzale@astronomy.ohio-state.edu), R. Pogge (pogge.1@osu.edu)
  \date 2008 May 01
  \section Usage

  Usage: 
  islmlynx ?[IP addr]? ?[Port]? [command|help]
 \c command - is the command used by the Server mmcServer to Query/Set
 
  Where: \c IP addr - is the Host IP that the mmcServers run
         \c Port    - is the Port Number that the mmcServers run
         \c command - is the command used by the mmcServer to Query/Set
                      wago/microLynx mechanisms
	 \c help    - give available commands that can be called.

  \section Introduction
  * islmlynx is a system call for communicating with the mmcServer.
  * mmcServer communicates with all MODS mechanisms and WAGOs.
    
  * islmlynx <-> mmcServer <-> microLynx controller <-> mechanism motor
  * islmlynx <-> mmcServer <-> wago <-> WAGO parameters

  * user and/or script hands a command to islmlynx
  * islmlynx passes command to the mmcServer.
  * mmcServer commands the wago/microLynx controller. 
  * wago/microLynx controller Query/Set wago parameter/mechanism motor.
  * wago/microLynx controller returns information to the mmcServer.
  * mmcServer returns information to islmlynx.
  * islmlynx returns information to user and/or script screen.


<pre>
2008 May 01 - new application [rdg]
2025 Oct 03 - legacy application, deprecated [rwp/osu]
</pre>

\todo

<ul>
</ul>
*/

/*!
  \file islmlynx.c
  \brief islmlynx is an all purpose client that works with mmcServer
*/

#include "ISLSocket.h"        // For Socket and SocketException
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <vector>
#include <cstring>            // for memset()
#include <cstdlib>            // For atoi()

using namespace std;

#include "params.h"

void helper();

int main(int argc, char *argv[]) {
  string servAddress;
  unsigned short servPortID;
  char *cmdString;
  int cmdStringLen;

  /* In case we want to fork this bad boy
   *  int childpid; // Child id
   */

  // memset(cmdString,0,sizeof(cmdString));

  /* the First Argument is not always the command */
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

  /* In case we want to fork this bad boy
   * childpid=fork();
   * if ( childpid == 0) {
   */
  try {

    TCPSocket sock(servAddress, servPortID); // Establish connection
    sock.send(cmdString, cmdStringLen);      // Sender

    char replyBuffer[4096];         // Buffer for reply string + \0
    int bytesReceived = 0;                   // Bytes read on each recv()
    
    memset(replyBuffer,0,sizeof(replyBuffer)); // Clear receive buffer
    
    if((bytesReceived=(sock.recv(replyBuffer,4096)))<= 0) { // Receive
      cerr << "[" << bytesReceived 
	   << "]islmlynx, Segmentation Fault from mmcServer at "
	   << cmdString
	   << endl;
      replyBuffer[bytesReceived] = '\0';   // Terminate the string!
      exit(1);
    }

    replyBuffer[bytesReceived] = '\0';   // Terminate the string!
    cout << replyBuffer << endl;         // echo replyBuffer
    
   } catch(SocketException &e) {         // Destructor closes the socket
    cerr << e.what() << endl;
    exit(1);
  }
  /* End of forking if used
   *  }
   */
  exit(0);
}

/* helper
   helper is just a text file that is in the same directory
   started with system(xterm) start.
 */
void helper() {
  system("xterm -fn 10x20 -geometry 80x24+0+0 -e less /home2/mods/help/helper.___ &");
}
