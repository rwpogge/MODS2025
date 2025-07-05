/** \file calib.c
 * \brief Client stub for setting calibration tower bit.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>

#include "../include/islapi.h"
#include "../include/modscontrol.h"

#define BUFFSIZE		512
extern char *mods_fatalerrormsg[];

int calib(char *host, char *port, char str1[])
{
  islcomp comp;             // place holder for host
  islnum app;               // place holder for port
  islconn conn;             // place holder connections
  char send_buff[BUFFSIZE]; // send message buffer
  char recv_buff[BUFFSIZE]; // reply message buffer
  int len;                  // length of message reply
  int ierr;                 // Error code
  int agw_no;               // Error code

  memset(send_buff,0,sizeof(send_buff));
  memset(recv_buff,0,sizeof(recv_buff));

  ierr=0;

  comp = islCnameToComp(host); // convert arguments to binary format comp
  if (comp == -1) {
    ierr=1;
    sprintf(recv_buff,"Network Error, *NO* such host named <%s>",host);
    mods_fatalerrormsg[ierr]=(char *)recv_buff;
    return(ierr);
  }
  agw_no=atoi(port); // convert port argument to binary format

  if (MODS_1_PORT != 0)
    if(!agw_no)
      app = (islnum) MODS_1_PORT+2; // convert argument to binary format comp
    else
      app = (islnum) MODS_2_PORT+2; // convert argument to binary format comp
  else
    if ((app = islAppNameToAppNumb("echo")) == -1)
      return (2);

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
    ierr=3;
    sprintf(recv_buff,"Comm. Error, *NO* Connection formed on <%s>",host);
    mods_fatalerrormsg[ierr]=(char *)recv_buff;
    return ierr;
  }  
  //  if (conn < 0)
  //    if(reusePort(conn) < 0) {
  //      (void) fprintf(stderr, "connection to <HOST %s> failed\n", host);
  //      return -1;
  //    }

  (void) fflush(stdout);

  sprintf(send_buff, "calib %s", str1); // Form message given by caller
  len=strlen(send_buff)+1;
  send_buff[len]='\0';


  (void) send(conn, send_buff, len, 0); // Send message to mmcService
  (void) fflush(stdout);

  len = recv(conn,recv_buff,sizeof(recv_buff),0); // get reply from mmcService

  if (len < 0) {
    islSendEOF(conn);
    return -1;
  }

  recv_buff[len]='\0'; // prevent segmentation faults.
  strcpy(str1,recv_buff); // send reply message back to caller

  (void) fflush(stdout);
  (void) islSendEOF(conn); // iteration ends when EOF found on stdin

  return 0;
}
