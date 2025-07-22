/** \file bgrstep3.c
 * \brief client for sending commands to the Blue Grating Select
 * #3 via the MODS MicroLynx Controller Service (mmcService).
 * Other commands are MicroLynx controller commands. 
 * Can be found at address:
 * http://www.astronomy.ohio-state.edu/~rgonzale/README_MLC
 * Returns information in str1[] if needed.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>

#include "../include/instrutils.h"  // Command functions header file
#include "../include/isl_funcs.h"   // "
#include "../include/isl_types.h"   // "
#include "../include/params.h"      // Common parameters
#include "../include/islcommon.h"   // defines for SHRMEM
#include "../include/isl_shmaddr.h" // Shared memory attachment.
#include "../include/islapi.h"         // API for service communication
//#include "../include/modscontrol.h"    

#define BUFFSIZE		512

int bgrstep3(char *host, char str[], char str1[])
{
  islcomp comp;             // place holder for host
  islnum app;               // place holder for port
  islconn conn;             // place holder connections
  char send_buff[BUFFSIZE]; // message buffer
  int len;                  // length of message reply

  comp = islCnameToComp(host); // convert host argument to binary format
  app = (islnum) atoi(str); // convert port argument to binary format

  if (comp == -1) {
    (void) fprintf(stderr, "<HOST %s> not available\n", host);
    return -1;
  }

  if (app == -1) {
    (void) fprintf(stderr, "<PORT %s> not available\n", str);
    return -1;
  }

  conn = islMakeContact(comp, app); // form a connection with mmcService
  if (conn < 0)
    (void) fprintf(stderr, "connection to <HOST %s> failed\n", host);
    return -1;


  (void) fflush(stdout);

  sprintf(send_buff, "bgrstep3 %s", str1); // Form message given by caller
  len=strlen(send_buff)+1;
  send_buff[len]='\0';


  (void) send(conn, send_buff, len, 0); // Send message to mmcService
  (void) fflush(stdout);

  memset(str1,0,sizeof(str1)); // Clear the return string and ready for reply
      
  len = recv(conn, str1, sizeof(str1), 0); // get reply from mmcService

  if (len < 0) {
    islSendEOF(conn);
    return -1;
  }

  str1[len]='\0'; // prevent segmentation faults.

  (void) fflush(stdout);
  (void) islSendEOF(conn); // iteration ends when EOF found on stdin

  return 0;
}
