/** \file drift.c
 * \brief Client for obtaining the cartesian coordinates of the guiding
 * mirror of the specified MODS AGW Unit via the agwService.
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
extern char *mods_fatalerrormsg[];

int 
drift(char *host, int agw_no, int d_onoff)
{
  islcomp comp;
  islnum app;
  islconn conn;
  char errchar[4];
  char send_buff[BUFFSIZE];
  char recv_buff[BUFFSIZE];
  int i;
  int ierr;
  int len;
  int bcnt;

  comp = islCnameToComp(host); // convert argument to binary format comp
  if (comp == -1) {
    ierr=1;
    sprintf(recv_buff,"Network Error, *NO* such host named <%s>",host);
    mods_fatalerrormsg[ierr]=(char *)recv_buff;
    return(ierr);
  }

  if (MODS_1_PORT != 0)
    if(!agw_no)
      app = (islnum) MODS_1_PORT; // convert argument to binary format comp
    else
      app = (islnum) MODS_2_PORT; // convert argument to binary format comp
  else
    if ((app = islAppNameToAppNumb("echo")) == -1)
      return (2);
  
  conn = islMakeContact(comp, app);  // form connection with agwService
  if (conn < 0) {
    ierr=3;
    sprintf(recv_buff,"Comm. Error, *NO* Connection formed on <%s>",host);
    mods_fatalerrormsg[ierr]=(char *)recv_buff;
    return ierr;
  }   
  (void) fflush(stdout);

  sprintf(send_buff,"drift %d", d_onoff);
  len=strlen(send_buff)+1;

  (void) send(conn, send_buff, len, 0); // send msg to agwService
  (void) fflush(stdout);
    
  len = recv(conn, recv_buff, sizeof(recv_buff), 0); // read from agwService
  recv_buff[len]='\0'; // prevent seg. faults

  if (len < 0) {
    ierr=4;
    sprintf(recv_buff,"Comm. Error: read from <%s> *FAILED*",host);
    mods_fatalerrormsg[ierr]=(char *)recv_buff;

  } else if(strstr(recv_buff,"ERROR:")) {
    for(bcnt=0;recv_buff[bcnt]!=':';bcnt++); // Search for ':' in error string
    bcnt++;
 
    strncpy(errchar,&recv_buff[bcnt],3); // get the error number.
    ierr = atoi(errchar);

    mods_fatalerrormsg[ierr]=(char *)&recv_buff[bcnt+3];
    return ierr; // return error

  }

  (void) fflush(stdout);
  (void) islSendEOF(conn);  // iteration ends when EOF found
  return 0;
}
