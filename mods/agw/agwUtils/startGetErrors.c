/** \file startGetErrors.c
 * \brief This module implements the function to be called by the agwServer
 * thread managing routine to start the periodically check on all AGW units
 * if an error is generated. If so, the description of that error is 
 * written to the fatalerrormsg[index] found in startGetErrors.h.
 */
#include <iostream>
#include <fstream>
using namespace std;

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <sys/types.h>
#include <signal.h>

#include "../include/islapi.h"
#include "../include/modscontrol.h"
#include "../include/mods_timefuncts.h"

#define BUFFSIZE 512
extern char *mods_fatalerrormsg[];

int 
startGetErrors(char *host, int agw_no, char *logfile)
{
  islcomp comp;
  islnum app;
  islconn conn;
  char errchar[4];
  char send_buff[BUFFSIZE];
  char recv_buff[BUFFSIZE];
  int len;
  int bcnt;
  int i;
  int ierr;
  char timestr[24];

  memset(send_buff,0,sizeof(send_buff));
  memset(recv_buff,0,sizeof(recv_buff));
  ierr=0;

  ofstream errlog (logfile, ios::app);

  comp = islCnameToComp(host); // convert arguments to binary format comp
  if (comp == -1) {
    ierr=1;
    sprintf(recv_buff,"Network Error, *NO* such host named <%s>",host);
    mods_fatalerrormsg[ierr]=(char *)recv_buff;
    getCurrentTime(timestr,"%Y %j %H %M %S");
    errlog << timestr << " " << (char *)recv_buff << endl; // log error;
    errlog.close();
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
    getCurrentTime(timestr,"%Y %j %H %M %S");
    errlog << timestr << " " << (char *)recv_buff << endl; // log error;
    errlog.close();
    return ierr;
  }  

  errlog.close();

  while(1) {
    ofstream errlog (logfile, ios::app);
    (void) fflush(stdout);
    
    //    strcpy(send_buff,"startGetErrors");
    strcpy(send_buff,"geterrormsg");
    len=strlen(send_buff);
    
    (void) send(conn, send_buff, len, 0); // send msg to agwService  
    (void) fflush(stdout);
    
    len = recv(conn, recv_buff, sizeof(recv_buff), 0); // read from agwService
    recv_buff[len]='\0'; // prevent segmentation faults.
    
    if (len < 0) {
      ierr=4;
      sprintf(recv_buff,"Comm. Error: read from <%s> *FAILED*",host);
      mods_fatalerrormsg[ierr]=(char *)recv_buff;
      getCurrentTime(timestr,"%Y %j %H %M %S");
      errlog << timestr << " " << (char *)recv_buff << endl; // log error;
      errlog.close();
      break;
    } else {
      getCurrentTime(timestr,"%Y %j %H %M %S");
      errlog << timestr << " " << (char *)recv_buff << endl; // log error;
      printf("%s",recv_buff);    
      ierr=0;
      memset(recv_buff,0,sizeof(recv_buff));
    }
    errlog.close();
    sleep(2);
 }
  
  errlog.close();
  (void) fflush(stdout);
  (void) islSendEOF(conn); // iteration ends when EOF
  return ierr;
}
