/** \file mods_setposition.c
 * \brief Client stub for driving the X stage,Y stage,
 *        Focus, and Filter Wheel to a new position
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>

#include "../include/islapi.h"
#include "../include/modscontrol.h"

#define BUFFSIZE		512
extern char *mods_fatalerrormsg[];

int 
mods_setposition(char *host, int agw_no, int motor_no, double new_pos, double velocity, int options)
{
  islcomp comp;
  islnum app;
  islconn conn;
  char errchar[4];
  char send_buff[BUFFSIZE];
  char recv_buff[BUFFSIZE];
  int ierr;
  int len;
  int bcnt;

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

  if (MODS_1_PORT != 0)
    if(!agw_no)
      app = (islnum) MODS_1_PORT; // convert argument to binary format islnum
    else
      app = (islnum) MODS_2_PORT; // convert argument to binary format islnum
  else
    if ((app = islAppNameToAppNumb("echo")) == -1)
      return(2);
  
  conn = islMakeContact(comp, app); // form connection to the agwService
  if (conn < 0) {
    ierr=3;
    sprintf(recv_buff,"Comm. Error, *NO* Connection formed on <%s>",host);
    mods_fatalerrormsg[ierr]=(char *)recv_buff;
    return ierr;
  }  
  (void) fflush(stdout);
  
  switch (motor_no) { // where should I send the command
  case 1:
    strcpy(send_buff,"setposition x"); // X stage
    break;
  case 2:
    strcpy(send_buff,"setposition y"); // Y stage
    break;
  case 4:
    strcpy(send_buff,"setposition focus"); // Focus
    break;
  case 16:
    strcpy(send_buff,"setposition filter"); // Filter Wheel
    break;
  default:
    ierr=6;
    sprintf(recv_buff,"Unknown parameters <%s> sent to MODS AGW",send_buff);
    mods_fatalerrormsg[ierr]=(char *)recv_buff;
    (void) fflush(stdout);
    islSendEOF(conn);
    return ierr;
  }

  if(motor_no!=16)
    sprintf(&send_buff[strlen(send_buff)]," %f", // form the command
  	    new_pos);
  else sprintf(&send_buff[strlen(send_buff)]," %d", // form the command
	       (int)new_pos);

  len=strlen(send_buff)+1;
  send_buff[len]='\0';

  (void) send(conn, send_buff, len, 0); // send msg to agwService
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
  }
  
  (void) fflush(stdout);
  (void) islSendEOF(conn);  // iteration ends when EOF found
  return ierr;
}
