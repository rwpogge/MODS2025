/** \file getposition.c
 * \brief Client for obtaining parameter from AGW Unit via islServer
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

extern int readln(char *, int);
extern int reusePort(int s);

int getposition(char *host,int agw_no,int motor_no,int *position, int options)
{
  islcomp comp;
  islnum app;
  islconn conn;
  char send_buff[BUFFSIZE];
  char recv_buff[BUFFSIZE];
  int len, i;
  mods_errs mods_reply;

  // convert the arguments to binary format comp and islnum

  comp = islCnameToComp(host);
  if (comp == -1)
    exit(1);
  
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

  //reusePort(conn);
  // Which parameter would you like to see.  
  switch (motor_no) {
  case 1:
    strcpy(send_buff,"getposition x");
    break;
  case 2:
    strcpy(send_buff,"getposition y");
    break;
  case 3:
    strcpy(send_buff,"getposition focus");
    break;
  case 4:
    strcpy(send_buff,"getposition filter");
    break;
  default:
    printf("ERROR! motors 1-4 only. 1=x, 2=y, 3=focus, 4=filter\n");
    islSendEOF(conn);
    return -1;
  }
  
  len=strlen(send_buff)+1;
  (void) send(conn, send_buff, len, 0);
  (void) fflush(stdout);
  
  // read and print same no. of bytes from islAGWServer
  len = recv(conn, recv_buff, sizeof(recv_buff), 0);

  if (len < 0) {
    islSendEOF(conn);
    return -1;
  }
  recv_buff[len]='\0';

  // Setup Server Information. 
  i=0;
  if(strstr(recv_buff,"=")) {
    for(i=0;recv_buff[i]!='=';i++);
    // Send requested parameter to caller
    //    if(!strstr(recv_buff,"ERROR:")) {
      *position=atoi(&recv_buff[i+1]);
      //    }
  } else {
    strncpy(mods_reply.err_reply,recv_buff,len);
    return -1;
  }
    
  // iteration ends when EOF found on stdin
  (void) fflush(stdout);
  (void) islSendEOF(conn);
  return 0;
}
