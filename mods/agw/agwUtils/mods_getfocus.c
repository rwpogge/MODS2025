/** \file mods_getfocus.c
 * \brief Client stub for obtaining the actual focus position of the specified
 * AGW Unit
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
mods_getfocus(char *host, int agw_no, double *focus, int options)
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

  ierr=0;
  comp = islCnameToComp(host); // convert argument to binary format comp
  if (comp == -1) {
        ierr=1;
    sprintf(recv_buff,"Network Error, *NO* such host named <%s>",host);
    mods_fatalerrormsg[ierr]=(char *)recv_buff;
    return(ierr);
  }

  if (MODS_1_PORT != 0)
    if(!agw_no)
      app = (islnum) MODS_1_PORT;// convert argument to binary format islnum
    else
      app = (islnum) MODS_2_PORT;// convert argument to binary format islnum
  else
    if ((app = islAppNameToAppNumb("echo")) == -1)
      return(2);

  conn = islMakeContact(comp, app); // form connection with agwService
  if (conn < 0) {
    ierr=3;
    sprintf(recv_buff,"Comm. Error, *NO* Connection formed on <%s>",host);
    mods_fatalerrormsg[ierr]=(char *)recv_buff;
    return ierr;
  }
  (void) fflush(stdout);

  memset(send_buff,0,sizeof(send_buff));
  strcpy(send_buff,"getfocus");
  len=strlen(send_buff)+1;

  (void) send(conn, send_buff, len, 0); // send msg to agwService
  (void) fflush(stdout);
    
  len = recv(conn, recv_buff, sizeof(recv_buff), 0); // read from agwService
  recv_buff[len]='\0'; // prevent segmentation faults.

  if (len < 0) {
    ierr=4;
    sprintf(recv_buff,"Comm. Error: read from <%s> *FAILED*",host);
    mods_fatalerrormsg[ierr]=(char *)recv_buff;

  } else if(strstr(recv_buff,"ERROR:")) { // check for error
    for(bcnt=0;recv_buff[bcnt]!=':';bcnt++); // Search for ':' in string.
    bcnt++;

    strncpy(errchar,&recv_buff[bcnt],3); // get error number.
    ierr=atoi(errchar);

    mods_fatalerrormsg[ierr]=(char *)&recv_buff[bcnt+3];

  } else {
    for(i=0;recv_buff[i]!='=';i++);
    i++;   
    for(;recv_buff[i]!='=';i++);
    i++;   
    *focus=(double)atof(&recv_buff[i]);
    ierr=0;
  }
  
  (void) fflush(stdout);
  (void) islSendEOF(conn); // iteration ends when EOF found
  return ierr;
}
