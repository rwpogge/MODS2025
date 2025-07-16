/** \file mmcu.c
 * \brief All purpose client for sending commands via the services 
 * provide by MODS MicroLynx Controller Service (mmcService).
 * Other commands are MicroLynx controller commands. These commands 
 * can be found at address:
 * http://www.astronomy.ohio-state.edu/~rgonzale/README_MLC
 * Returns information in command[] string if needed.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>

#include "instrutils.h"  // Command functions header file
#include "isl_funcs.h"   // ISL Functions
#include "isl_types.h"   // ISL Types
#include "params.h"      // Common parameters
#include "islcommon.h"   // defines for SHRMEM
#include "isl_shmaddr.h" // Shared memory attachment.
#include "islapi.h"         // API for service communication
#include "mmccontrol.h"     // Mechanism command functions

#define BUFFSIZE		512

//extern int reusePort(int);

int mmcu(char *host, char *port, char *who, char str1[])
{
  islcomp comp;             // place holder for host
  islnum app;               // place holder for port
  islconn conn;             // place holder connections
  char send_buff[BUFFSIZE]; // send message buffer
  char recv_buff[BUFFSIZE]; // receive message buffer
  int len;                  // length of message reply

  /* This might be useful for future expansion */
  //int getMechanismID(char [],char []);
  //char dummy[BUFFSIZE];     // receive message buffer
  //int i;                    // counter
  //  setup_ids();
  //i=getMechanismID(&who[0],dummy);
  //if(shm_addr->MODS.busy[i]) {
  //    sprintf(str1,"ERROR: %s %s=BUSY",&who[0],&who[0]);
  //    return -1;
  //  }

  comp = islCnameToComp(host); // convert host argument to binary format
  app = (islnum) atoi(port);   // convert port argument to binary format

  if (comp == -1) {
    (void) fprintf(stderr, "<HOST %s> not available\n", host);
    return -1;
  }

  if (app == -1) {
    (void) fprintf(stderr, "<PORT %s> not available\n", port);
    return -1;
  }

  conn = islMakeContact(comp, app); // form a connection with mmcService
  if (conn < 0) {
    //    if(reusePort(conn) < 0) {
    (void) fprintf(stderr, "connection to <HOST %s> failed\n", host);
      return -1;
  }

  (void) fflush(stdout);

  sprintf(send_buff, "%s %s", who,str1);// form message given by caller
  len=strlen(send_buff)+1;
  send_buff[len]='\0';

  (void) send(conn, send_buff, len, 0); // send message to mmcService
  (void) fflush(stdout);

  len = recv(conn,recv_buff,sizeof(recv_buff),0); // get reply from mmcService

  if (len < 0) {
    (void) fprintf(stderr, "Read from <PORT %s> failed!\n", port);
    islSendEOF(conn);
    return -1;
  }

  recv_buff[len]='\0'; // prevent segmentation faults.

  strcpy(str1,recv_buff); // send reply message back to caller

  (void) fflush(stdout);
  (void) islSendEOF(conn); // iteration ends when EOF found on stdin

  if(strstr(str1,"ERROR: ")) return -1;
   return 0;
}
