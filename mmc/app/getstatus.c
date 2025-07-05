/** \file getstatus.c
 * \brief Client for obtaining the mechaniam status
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <sys/types.h>
#include <signal.h>

#include "../include/islapi.h"
#include "../include/modscontrol.h"

#define BUFFSIZE		512

int 
getstatus(char *host, char str[], char str1[])
{
  int childpid;
  islcomp comp;
  islnum app;
  islconn conn;
  char send_buff[BUFFSIZE];
  char recv_buff[BUFFSIZE];
  int len;

  // convert the arguments to binary format comp and islnum
  //comp = islCnameToComp(argv[1]);

  comp = islCnameToComp(host);

  if (comp == -1) {
    (void) fprintf(stderr, "no host: %s <computer name>\n", host);
    exit(1);
  }

  // For testing!!!!!!!!!!
  //comp = islCnameToComp(MODS_1_HOST);

  if (MODS_1_PORT != 0)
    app = (islnum) MODS_1_PORT;
  else
    if ((app = islAppNameToAppNumb("echo")) == -1)
      exit(1);
  
  // form a connection with the mmcService
  
  conn = islMakeContact(comp, app);
  if (conn < 0) 
    exit(1);
  
  (void) fflush(stdout);


  strcpy(send_buff,"status"); // form the command
  len=strlen(send_buff)+1;

  (void) send(conn, send_buff, len, 0); // send command to mmcService
  (void) fflush(stdout);
    
  len = recv(conn, recv_buff, sizeof(recv_buff), 0); // get reply

  if (len < 0) {
    islSendEOF(conn);
    return -1;
  }
  recv_buff[len]='\0';  // always protect from Segmentation errors. 

  strncpy(str,recv_buff,len); // always protect from Segmentation errors.

  (void) fflush(stdout);

  (void) islSendEOF(conn); // iterations end when EOF found on stdin
  
  return 0;
}
