/** \file init.c
 * \brief Client for obtaining initializing MODS
 * mirror of the specified MODS AGW Unit.
 */
/*
/*-----------------------------------------------------------------------
 *
 * Program: init
 * Purpose: contact modsAGWServer, send user input a reset command.
 *
 *-----------------------------------------------------------------------
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>

#include "../include/islapi.h"
#include "../include/modscontrol.h"

#define BUFFSIZE	128

int readln(char *, int);
static char serverinfo[10][40];

int init(char *host, int agw_no, int options)
{
  islcomp comp;
  islnum app;
  islconn conn;
  char send_buff[BUFFSIZE];
  char recv_buff[BUFFSIZE];
  int nextparam, bcnt, scnt, len;
  mods_errs mods_reply;

  // convert the arguments to binary format comp and islnum

  //TEST
  printf("Here for a test on %s\n",host),
  comp = islCnameToComp(host); 
  if (comp == -1) {
    (void) fprintf(stderr, "no host: %s <computer name>\n", host);
    exit(1);
  }
  
  //  if (agw_no < 4 || agw_no > 7) {
  //    (void) fprintf(stderr, "invalid agw_no %d\n", agw_no);
  //    exit(-1);
  //  }

  if (MODS_1_PORT != 0)
    //   for testing app = (islnum) atoi(argv[2]);
    app = (islnum) MODS_1_PORT;
  else
    if ((app = islAppNameToAppNumb("echo")) == -1)
      exit(1);
  
  // form a connection with the islServer
  
  conn = islMakeContact(comp, app);
  if (conn < 0) 
    exit(1);
  
  //  if(motors==0 || motors>7) return -1;
  strcpy(send_buff,"reset");
  len=strlen(send_buff)+1;
  (void) send(conn, send_buff, len+1, 0);
  (void) fflush(stdout);
  
  // Repeatedly read data from socket and save for caller.
  len = recv(conn, recv_buff, sizeof(recv_buff), 0);
  if (len < 0) {
    islSendEOF(conn);
    return -1;
  }
  recv_buff[len]='\0';

  // Setup Server Information. Data starts after first space
  // then Data will be broken up at every space.
  nextparam=0;
  for(bcnt=scnt=0;recv_buff[bcnt]!=NULL;bcnt++) {
    if(recv_buff[bcnt]!=' ') {
      serverinfo[nextparam][scnt++]=recv_buff[bcnt];
    } else {
      nextparam++; 
      scnt=0; 
    }
  }

  // Send X and Y values to caller
  if(strstr(recv_buff,"ERROR:")) {  
    strncpy(mods_reply.err_reply,recv_buff,len);
    printf("errorcode: -1\n");
    return -1;
  } else {
    printf("***********************************\n");
    printf("Received the following values:\n");
    printf("agw_no: %d\n",agw_no);
    printf("Will send back the following values:\n");
    printf("errorcode: 0\n");
  }

  // iteration ends when EOF found on stdin

  (void) fflush(stdout);
  (void) islSendEOF(conn);

  return 0;
}
