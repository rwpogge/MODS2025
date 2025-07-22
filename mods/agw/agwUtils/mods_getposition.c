/** \file mods_getposition.c
 * \brief Client for obtaining parameter from AGW Unit via agwServer
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

int mods_getposition(char *host,int agw_no,int motor_no,double *position, int options)
{
  islcomp comp;
  islnum app;
  islconn conn;
  char send_buff[BUFFSIZE];
  char recv_buff[BUFFSIZE];
  char errchar[4];
  int i;
  int ierr;
  int len;
  int bcnt;

  comp = islCnameToComp(host); // convert the argument host to binary format
  if (comp == -1) {
    ierr=1;
    sprintf(recv_buff,"Network Error, *NO* such host named <%s>",host);
    mods_fatalerrormsg[ierr]=(char *)recv_buff;
    return ierr;
  }
  
  if (MODS_1_PORT != 0)
    if(!agw_no)
      app = (islnum) MODS_1_PORT; // convert the argument port to binary format
    else
      app = (islnum) MODS_2_PORT; // convert the argument port to binary format
  else
    if ((app = islAppNameToAppNumb("echo")) == -1)
      return(2);
  
  conn = islMakeContact(comp, app); // connect to agwServer
  if (conn < 0) {
    ierr=3;
    sprintf(recv_buff,"Comm. Error, *NO* Connection formed on <%s>",host);
    mods_fatalerrormsg[ierr]=(char *)recv_buff;
    return ierr;
  }  
  (void) fflush(stdout);

  switch (motor_no) { // who gets the command  
  case 1:
    strcpy(send_buff,"getposition x"); // X stage
    break;
  case 2:
    strcpy(send_buff,"getposition y"); // Y stage
    break;
  case 4:
    strcpy(send_buff,"getposition focus"); // Focus
    break;
  case 16:
    strcpy(send_buff,"getposition filter"); // Filter Wheel
    break;
  default:
    ierr=6;
    sprintf(recv_buff,"Unknown MODS AGW mechanism <%d>. Valid IDs 1=X,2=Y,4=Focus, 16=FilterWheel",motor_no);
    mods_fatalerrormsg[ierr]=(char *)recv_buff;
    (void) fflush(stdout);
    islSendEOF(conn);
    return ierr;
  }
  
  len=strlen(send_buff)+1;
  (void) send(conn, send_buff, len, 0); // send to agwServer
  (void) fflush(stdout);
  
  len = recv(conn, recv_buff, sizeof(recv_buff), 0); // read from agwServer
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
  } else if(strstr(recv_buff,"=")) {   // parse received message
    for(i=0;recv_buff[i]!='=';i++);
    i++;
    *position=atof(&recv_buff[i]);
    ierr=0;
  }

  (void) fflush(stdout);
  (void) islSendEOF(conn); // iteration ends when EOF found

  return ierr;
}
 
