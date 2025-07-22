/** \file geterrors.c
 * \brief This module implements the function to be called by the islServer
 * thread managing routine to start the periodically check on all AGW units
 * if there has occured an error. If so, the description of that error is 
 * written to an log file.
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

int readln(char *, int);
char serverinfo[20][128];

int 
geterrormsg(char *host, int agw_no, int options)
{
  islcomp comp;
  islnum app;
  islconn conn;
  char send_buff[BUFFSIZE];
  char recv_buff[BUFFSIZE];
  char save_buff[BUFFSIZE];
  int len;
  int nextparam, bcnt, scnt;

  // convert the arguments to binary format comp and islnum
  //comp = islCnameToComp(argv[1]);


  comp = islCnameToComp(host);
  if (comp == -1) {
    (void) fprintf(stderr, "no host: %s <computer name>\n", host);
    exit(1);
  }

  //  if (agw_no == 1 || agw_no == 2) {
  //    (void) fprintf(stderr, "invalid agw_no %d\n", agw_no);
  //    exit(-1);
  //  }

  // For testing!!!!!!!!!!
  //comp = islCnameToComp(MODS_1_HOST);

  // Clear message buffers
  memset(save_buff,0,sizeof(save_buff));
  memset(recv_buff,0,sizeof(recv_buff));

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

  printf("University of Arizona  Large Binocular Telescope (LBT%d)\n",agw_no);
  printf("Instrument: OSU/ISL MODS(%d)\n",agw_no);
  
  strcpy(send_buff,"status");
  len=strlen(send_buff)+1;
  (void) send(conn, send_buff, len, 0);
  (void) fflush(stdout);

  // get and print errors from islAGWServer via error thread #6
  len = recv(conn, recv_buff, sizeof(recv_buff), 0);
  if (len < 0) {
    islSendEOF(conn);
    return -1;
  }
  recv_buff[len]='\0';
  
  // examine Server Information.
  nextparam=0;
  for(bcnt=scnt=0;recv_buff[bcnt]!=NULL;bcnt++) {
    if(recv_buff[bcnt]!=' ') {
      serverinfo[nextparam][scnt++]=recv_buff[bcnt];
    } else {
      nextparam++;
      scnt=0;
    }
  }

  //XAG:YAG XWFS:YWFS FOCUS FILTER

  printf("AG Settings: %s:%s\nWFS Settings: %s:%s\nFocus:%s - Filter:%s\n",
	 serverinfo[1], serverinfo[2], serverinfo[3],
	 serverinfo[4], serverinfo[7], serverinfo[11]);

  //Xs:Ys F0:AutoFocus CalibrationTower

  printf("Raw Settings %s:%s\n%s:%s\n%s\n",
	 serverinfo[5], serverinfo[6], serverinfo[8],
	 serverinfo[9], serverinfo[17]);
  
  while(1) {
    strcpy(send_buff,"status");
    len=strlen(send_buff)+1;
    (void) send(conn, send_buff, len, 0);
    (void) fflush(stdout);

  // get and print errors from islAGWServer via error thread #6
    len = recv(conn, recv_buff, sizeof(recv_buff), 0);
    if (len < 0) {
      islSendEOF(conn);
      return -1;
    }
  
    printf("%s\n",recv_buff);    
    sleep(options);
  }

  (void) fflush(stdout);
  // iteration ends when EOF found on stdin
  (void) islSendEOF(conn);
  
  return 0;
}
