/** \file calmode
 * \brief All purpose client for sending an ASCII string to
 * Calibration mode. Returns information in str1[] if needed.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>

#include "../include/islapi.h"
#include "../include/modscontrol.h"

#define BUFFSIZE		512

int readln(char *, int);
static char serverinfo[10][40];

int calmode(char *host, int port, char str[], char str1[])
{
  islcomp comp;
  islnum app;
  islconn conn;
  char send_buff[BUFFSIZE];
  char recv_buff[BUFFSIZE];
  int expect, received, len;
  int nextparam, bcnt, scnt;
  mods_errs mods_reply;
  int ierr;
  
  ierr=0;

  // convert the arguments to binary format comp and islnum
  //comp = islCnameToComp(argv[1]);

  comp = islCnameToComp(host);
  app = (islnum) port;

  if (comp == -1) {
    (void) fprintf(stderr, "no host: %s <computer name>\n", host);
    return -1;
  }

  if (app == -1) {
    (void) fprintf(stderr, "no port: %p <PORT>\n", port);
    return -1;
  }

  /*
  if (MODS_1_PORT != 0)
    app = (islnum) MODS_1_PORT;
  else
    if ((app = islAppNameToAppNumb("echo")) == -1)
      exit(1);
  */

  conn = islMakeContact(comp, app);
  if (conn < 0) 
    return -1;

  (void) fflush(stdout);

  sprintf(send_buff, "ml 0 %s", str);
  len=strlen(send_buff)+1;
  send_buff[len]='\0';

  (void) send(conn, send_buff, len, 0);
  (void) fflush(stdout);

  // read and print same no. of bytes from modsIIFServer
      
  len = recv(conn, recv_buff, sizeof(recv_buff), 0);
  if (len < 0) {
    islSendEOF(conn);
    return -1;
  }
  recv_buff[len]='\0';

  strcpy(str1,recv_buff);
  ierr = 0;

  (void) fflush(stdout);
  // iteration ends when EOF found on stdin
  (void) islSendEOF(conn);
  return ierr;
}
