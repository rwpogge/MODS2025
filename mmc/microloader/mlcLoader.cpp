/*!
  \file mlcLoader.c
  \brief mlcLoader - microlynx controller Loader

  \author Staff, OSU Dept. of Astronomy (rgonzale@astronomy.ohio-state.edu)
  \date 2008 Dec 10 [rdg]

  \par Usage: Unix style
  mlcLoader <filename>.plc  - without extension.

  mlcLoader will load a PLC mechanism(s) with the proper ladder code.

*/

#include "../include/ISLSocket.h"  // For Socket and SocketException
#include <iostream>           // For cerr and cout
#include <cstdlib>            // For atoi()
#include <sstream>
#include <fstream>
#include <cstring>
using namespace std;

#include "../include/StrConv.h"   // Command action functions header file
#include "../include/params.h"      // Common parameters and defines
#include "../include/islcommon.h"     // Common parameters and defines
#include "../include/isl_shmaddr.h"     // Shared memory attachment.

const int RCVBUFSIZE = 8192;    // (512)Size of receive buffer

int main(int argc, char *argv[]) {
  int i,ierr=0;
  int device;
  char inbuf[128];
  char mechanism[24];
  char plcFile[79];
  FILE *fp;

  if ((argc < 3) || (argc > 4)) {     // Test for correct number of arguments
    cerr << "Usage: " << argv[0] 
         << " <Server> <filename> [<Server Port>]" << endl;
    exit(1);
  }

  strcpy(mechanism,argv[2]);
  sprintf(plcFile,"/home2/mods/plc/%s.plc",mechanism);
  plcFile[strlen(plcFile)]='\0'; // Do not want seg. faults at any time.

  /*
   * Now open the program file, return a -9 if file is in error.
   */
  if (!(fp=fopen(plcFile, "r"))) {
    sprintf(reply,"Could not open file %s",plcFile);
    return -1;
  }

  string servAddress = argv[1]; // First arg: server address
  //  char echoString[64];   // Second arg: string to echo
  //  sprintf(echoString,"%s\r",argv[2]);
  //int echoStringLen = strlen(echoString);   // Determine input length
  unsigned short echoServPort = atoi(argv[3]);

  try {

    device=getMechanismID(mechanism,reply);
    if(device==-1) return -1;

    //if (strstr(&shm_addr->MODS1.commport[device],":")>0) {
    sscanf(port->Port,"%[^:]:%d",servAddress,&servPortID);

    TCPSocket sock(servAddress, servPortID); // Establish connection


    sock.send(echoString, echoStringLen); // Send the string to the echo server
  
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

/*!
  \file islpp.c
  \brief islpp - SNTools ISLProcedureProcessor.

  \author Staff, OSU Dept. of Astronomy (rgonzale@astronomy.ohio-state.edu)
  \date 2005 March 20 [rdg]

  \par Usage: Unix style
  islpp <filename>.prc  - without extension.

  ISLProcedureProcessor will run a procedure file.

*/
#include <iostream>
#include <sstream>
#include <fstream>
#include <cstring>
using namespace std;

#include "../include/StrConv.h"   // Command action functions header file
#include "../include/params.h"      // Common parameters and defines
#include "../include/islcommon.h"     // Common parameters and defines
#include "../include/isl_shmaddr.h"     // Shared memory attachment.

#define NSEMNAME "isl    "
int nsem_test(char []);
//----------------------------------------------------------------
int
mlcLoader(int argc, char *argv[])
{
  int i,ierr=0;
  int device;
  char inbuf[MAXPGMLINE];
  char mechanism[MAXPGMLINE];
  char plcFile[MAXPGMLINE];
  FILE *fp;

  /*
   * First lets play it safe and clear the buffers.
   */
  memset(mechanism,0,sizeof(mechanism)); 
  memset(plcFile,0,sizeof(plcFile)); 
  memset(inbuf,0,sizeof(inbuf));

  strcpy(mechanism,argv[1]);

  sprintf(plcFile,"/home2/mods/plc/%s.plc",mechanism);

  plcFile[strlen(plcFile)]='\0'; // Do not want seg. faults at any time.
  
  device=getMechanismID(mechanism,reply);
  if(device==-1) return CMD_ERR;

  /*
   * Now open the program file, return a -9 if file is in error.
   */
  if (!(fp=fopen(plcFile, "r"))) {
    sprintf(reply,"Could not open file %s",plcFile);
    return CMD_ERR;
  }

  /* ----------------------------------------------------------------
   * Config file parser loop
   * 
   * Read in each line of the config file and process it 
   */
  //cout << "[ML" << device << "]loading: " << plcFile << endl;

  //----------------------------------------------------------------
  // Read in each line of the PLC code and process it 
  //
  shm_addr->MODS1.busy[device]=1; // Hold the IP until finished

  ierr=mlcCmdOnly(device,reply); // Send a Raw command no reply required
  if(ierr<0) {
    shm_addr->MODS1.busy[device]=0;    // Clear the HOST busy bit.
    return CMD_ERR;
  }
  ierr=rawCommandOnly(device,"IP ALL"); // Send a Raw command
  MilliSleep(100);
  ierr=rawCommandOnly(device,"DVF 0,0,0"); // Send a Raw command
  MilliSleep(100);
  ierr=rawCommandOnly(device,"CP 1,1"); // Send a Raw command
  MilliSleep(1000);
  ierr=rawCommandOnly(device,"SAVE"); // Send a Raw command

  while (fgets(inbuf, MAXPGMLINE, fp)) {
    if(strstr(inbuf,"END HEADER")) break;
  }

  while(fgets(inbuf, MAXPGMLINE, fp)) {
    /*
     * Skip comments (#,*) and blank lines
     */
    if (inbuf[0] != '#' && 
	inbuf[0] != '\n' && 
	inbuf[0] != '*' && 
	inbuf[0] != '\'' && 
	inbuf[0] != '\0') {
      
      inbuf[strlen(inbuf)]='\r';
      ierr=rawCommandOnly(device,inbuf); // Send a Raw command
      fprintf(stderr,"%s",inbuf);

    }
    memset(inbuf,0,sizeof(inbuf));
    TTYMSleep(200); // 100ms gives the port server a chance to complete
  }

  ierr=mlcCmdReply(device,reply); // Send a Raw command
  if(ierr<0) {
    shm_addr->MODS1.busy[device]=0;    // Clear the HOST busy bit.
    return CMD_ERR;
  }

  MilliSleep(1000);
  ierr=rawCommandOnly(device,"SAVE"); // Send a Raw command

  shm_addr->MODS1.busy[device]=0;  // Clear the HOST busy bit.
  sprintf(reply,"[ML%d]loaded %s ",device,plcFile);
    
  if (fp!=0)
    fclose(fp);

  exit(0);
}
