/*!
  \mainpage mlx - MicroLynX controller Interactive User Input Port Client

  \author Staff, OSU Dept. of Astronomy (rgonzale@astronomy.ohio-state.edu)
  \date 2005 May 01

  \section Usage

  Usage: mlx [rcfile]

  Where: \c rcfile is an optional runtime config file to load.  

  By default, mlx uses the runtime config file defined by
  #DEFAULT_RCFILE in the ml1_client.h header.

  \section Introduction

  ...

  mlx can be run as either a standalone interactive program with a
  command-line interface and backdoor socket interface, or as a client
  in an ISIS system.

  \section Commands

  These are the interactive commands for mlx:
<pre>
Client Commands:
  info           - report client information
  version        - report mlx version & compile info
  reset          - reset runtime & controller parameters
  verbose        - toggle verbose output mode
  debug          - toggle debugging output
  quit           - quit mlx
  history        - show command history
  !!             - repeat last command
  !cmd           - repeat last command matching 'cmd'
  help or ?      - view this list
</pre>
  Note that all commands are <em>case-insensitive</em>.

  \section Config Runtime Configuration File

  This is a typical runtime config file for the mlx agent:

  \verbinclude ml(x).ini

  Note that all parameter names are \e case-insensitive.

  \section Notes

  This application uses the ISISclient library (link).

  \section Mods Modification History

<pre>
2005 May 01 - new application [rwp/osu]
2005 May 30 - new application [rdg/osu]
</pre>

\todo 
<ul>
</ul>
*/

/*!
  \file mlx.c
  \brief mlx program and I/O event handler.
*/
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
using namespace std;
 
#include <sys/sendfile.h>
#include "../../include/instrutils.h"  
#include "../../include/isisclient.h"  // ISIS common client library header
#include "../../include/siutils.h"     // ISIS common client library header
#include "../../include/ml_client.h"   // Custom client application header

#include "../../include/isl_funcs.h"  
#include "../../include/isl_types.h"  
#include "../../include/params.h"      // Common parameters and defines
#include "../../include/islcommon.h"   // Common parameters and defines
#include "../../include/isl_shmaddr.h" // Shared memory attachment.

/* 
 * The client cli uses the GNU readline and history utilities
 */

#include <readline/readline.h>
#include <readline/history.h>
 
#define MAXPGMLINE 80

/*
 * ISIS Client data structure
 */

isisclient_t client;

//---------------------------------------------------------------------------
// StrUpper() - convert all characters in a string to uppercase
//
 
/*!
  \brief Convert all characters in a string to uppercase
 
  \param str String to be converted to uppercase. Changed by this function.
 
  Helper utility to convert all characters in a string to uppercase.
  All commands are required to be in uppercase.
 
*/
 
void
StrUpper(char *str)
{
  int i;
  for(i=0;i<strlen(str);i++)
    if(str[i] >= 'a' && str[i] <= 'z')
      str[i] = toupper(str[i]);
}

//---------------------------------------------------------------------------
//
// getMechanismID(dummy,reply) - Get the mechanism ID from Shared Memory
//

/*!
  \brief Get the mechanism ID from Shared Memory

  \param dummy mechanism
  \param reply reply message
  \return mechanismID  or error

  Get Mechanism ID

  <pre>
  </pre>

*/

int 
getMechanismID(char mechanism_name[], char dummy[])
{
  int dev;
  int len;
  char dummy2[PAGE_SIZE];

  memset(dummy2,0,sizeof(dummy2));
  
  for(dev=0;
      !strstr(mechanism_name,shm_addr->MODS1.who[dev]) && dev<=MAX_ML;
      dev++);

  //  if(!shm_addr->MODS1.host[dev]) {
  //    sprintf(dummy,"MLCERR=NOCOMM No IP:SOCKET assigned");
  //    return CMD_ERR;
  //  }

  StrUpper(mechanism_name);
  if(dev<0 || dev>=MAX_ML) {
    sprintf(dummy,"%s=FAULT No mechanism name available",mechanism_name);
    return -1;
  }

  return dev;
}
/* ----------------------------------------------------------------
 * The main event...
 */
int
main(int argc, char *argv[]) 
{
  int n;
  int i=0,j,k;
  int nread;
  int icontroller;  
  int nopen=0;            // number of open, addressable RTS server ports
  
  char buf[ISIS_MSGSIZE]; // command/message buffer
  char temp[PAGE_SIZE];   // temp. message buffer
  char temp2[PAGE_SIZE];  // temp. message buffer
  char ttyData[PAGE_SIZE];// raw scl serial port data string (oversized)
  char closeMech[40];// raw scl serial port data string (oversized)

  int lastchar;
  
  /*
   * readline & history handling stuff
   */
  
  char cliPrompt[ISIS_NODESIZE+2]; // the console prompt is our ISIS node name
  
  /*
   * select() event handler parameters
   */
  
  fd_set read_fd;
  int kbdFD;
  int n_ready;
  time_t t;
  struct tm *ptr;
  struct timeval timeout;
  static int sel_wid;
  long ip[5];
  string messages;
  string who;
  string pathstr("/home2/mods/Config/ml");
  /* 
   * Basic initializations
   */

  setup_ids();

  sel_wid = getdtablesize();

  if (n!=0) {
    cout << "Unable to load the runtime config file...mlx aborting" << endl;
    exit(1);
  }
  /*
   * If required, initialize the socket connection to the ISIS server
   * We can disable ISIS interaction by specifying "ServerID None" in
   * the runtime config file
   */
  if (client.useISIS) {
    if (InitISISServer(&client)<0) {
      cout << "ISIS server connection initialization failed" << endl;
    }
  }

  /* 
   * Open the client network socket port for interprocess communications 
   */
  if (OpenClientSocket(&client)<0) {
    cout << "Client socket initialization failed" << endl;
  }
  
  /* 
   * Now, open connections to all of the specified RTS server serial ports 
   */
  memset(temp,0,ISIS_MSGSIZE);

  /*
   * Set the SIGINT signal trap 
   */
  signal(SIGINT,HandleInt); // Ctrl+C sends a move abort to controller
  signal(SIGPIPE,SIG_IGN);  // ignore broken pipes

  /* 
   * Start the I/O event handling loop 
   */
  client.KeepGoing = 1;

  while (client.KeepGoing) {

    FD_ZERO(&read_fd); // clear the table of active file descriptors

    /*
     * if enabled, listen to this application's UDP socket
     */
    if (client.FD > 0) FD_SET(client.FD, &read_fd);

    /*
     * Do the select() call and wait for activity on any of our comm
     * ports or the console keyboard
     */
    n_ready = 0;
    n_ready = select(sel_wid, &read_fd, NULL, NULL, NULL);
    
    if (n_ready == 0) { // would be a timeout if enabled, do nothing...
      continue;

    } else if (n_ready < 0) {
      if (errno == EINTR) {// caught Ctrl+C, hopefully sigint handler caught it
	if (client.Debug)
	 cout << "select() interrupted by Ctrl+C...continuing" << endl;
      }
      else { // something else bad happened, let us know
	cout << "Warning: select() failed - %s - pressing on anyway..." 
	     <<	strerror(errno) << endl;
      }
      rl_refresh_line(0,0);
      continue;
      
    } else { // somebody wants something, figure out who...
      
      if (FD_ISSET(client.FD, &read_fd)) { // Client socket input
	memset(buf,0,ISIS_MSGSIZE);
	if (ReadClientSocket(&client,buf)>0) {
	  if (client.isVerbose) cout << "IN: " << buf << endl;
	  SocketCommand(buf);
	  rl_refresh_line(0,0);
	}
      }
    } // end of select() I/O handling checking
  } // bottom of the while(client.KeepGoing) loop

  /* ------------------------------------------------------
   * If we got here, the client was instructed to shut down
   */

  cout << "isisMlcShm client shutting down..." << endl;

  CloseClientSocket(&client); // Tear down the client socket

  cout << "bye" << endl; // all done, say goodbye...
  
  exit(0);

}

/*!
  \brief Service Ctrl+C Interrupts (SIGINT signals)

  SIGINT signal trap for trapping Ctrl+C interrupts.  Calls
  abortall() to immediately abort all moves in progress.

  \sa abortall()
*/

void
HandleInt(int signalValue)
{
  if (client.Debug)
    cout << "Caught Ctrl+C (Signal " << signalValue << "...." << endl;
  
  cout << "Ctrl+C Abort requested - aborting moves now - check state after abort!\n" << endl;
  abortall(); 
}

/*
 * abortall() - Abort all pending moves
 */
/*!
  \brief Abort all pending moves

  Close ISIS client.

  \sa HandleInt()
*/

void
 abortall(void) 
 { 

  CloseClientSocket(&client); // Tear down the client socket

  exit(0);
 } 
