/** \file setfocus.c
 * \brief Client stub for driving the Focus to a new position
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

int 
setfocus(char *host, int agw_no, int new_pos, double velocity, int options)
{
  islcomp comp;
  islnum app;
  islconn conn;
  char send_buff[BUFFSIZE];
  char recv_buff[BUFFSIZE];
  int len, i;
  int nextparam, bcnt, scnt;
  mods_errs mods_reply;

  // convert the arguments to binary format comp and islnum
  //comp = islCnameToComp(argv[1]);

  comp = islCnameToComp(host);
  if (comp == -1) {
    (void) fprintf(stderr, "no host: %s <computer name>\n", host);
    exit(1);
  }

  //  if (agw_no < 4 || agw_no > 7) {
  //    (void) fprintf(stderr, "invalid agw_no %d\n", agw_no);
  //    exit(-1);
  //  }
  
  // For testing!!!!!!!!!!
  //comp = islCnameToComp(MODS_1_HOST);

  if (MODS_1_PORT != 0)
    app = (islnum) MODS_1_PORT;
  else
    if ((app = islAppNameToAppNumb("echo")) == -1)
      exit(1);
  
  // form a connection with the islServer

  conn = islMakeContact(comp, app);
  if (conn < 0) 
    exit(1);
  
  (void) fflush(stdout);

  sprintf(send_buff,"setfocus %d",new_pos);
  len=strlen(send_buff)+1;
  send_buff[len]='\0';

  (void) send(conn, send_buff, len, 0);
  (void) fflush(stdout);
    
    // read and print same no. of bytes from islServer
      
  len = recv(conn, recv_buff, sizeof(recv_buff), 0);
  if (len < 0) {
    islSendEOF(conn);
    return -1;
  }

  recv_buff[len]='\0';

  // Setup Server Information.

  nextparam=0;
  for(bcnt=scnt=0;recv_buff[bcnt]!=NULL;bcnt++) {
    if(recv_buff[bcnt]!=' ') {
      serverinfo[nextparam][scnt++]=recv_buff[bcnt];
    } else {
      nextparam++;
      scnt=0;
    }
  }

  // Send Focus to caller
  if(!strstr(recv_buff,"ERROR")) {  
    printf("***********************************\n");
    printf("Received the following values:\n");
    printf("agw_no: %d\n",agw_no);
    printf("new_pos: %d\n",new_pos);
    printf("velocity: %d\n",velocity);
    printf("options: %d\n",options);
    printf("Will send back the following values:\n");
    printf("errorcode: %d\n",0);
  } else {
    strncpy(mods_reply.err_reply,recv_buff,len-1);
    printf("errorcode: %d\n",-1);
    return -1;
  }
  
  (void) fflush(stdout);
  // iteration ends when EOF found on stdin
  (void) islSendEOF(conn);
  return 0;
}
