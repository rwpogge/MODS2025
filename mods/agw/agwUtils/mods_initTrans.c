/** \file mods_initTrans.c
 * \brief Client for obtaining the initializing transformation coefficients
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
extern sac_xy xy;

//-------------------------------------------------------------------------
//
// GetArg() - extract the n-th argument from a command string with
//            arguments separated by spaces
//
// Arguments:
//   char *cmdstr - command string with space-separated arguments
//   int argnum   - number of the argument to extract (1..n)
//   char *argstr - argument string to return
//
// Author:
//   Brian Hartung (OSU undergraduate)
//   from the old Caliban code
//
 
void GetArgs(char *cmdstr, int argnum, char *argstr)
{
  int arglen, i=0, j=0;
  
  arglen=strlen(cmdstr); // Record the length of the overall argument string 

  // Seek to the beginning of the nth argument, where a space delimits args 

  for(i=0;argnum>1;argnum--) {
    while(cmdstr[i]!=' ') {
      i++;
      if(i>=arglen) // If there aren't n arguments, return null 
	break;
    }

    while(cmdstr[i]==' ') {     // Ignore extra spaces between args 
      i++;
      if(i>=arglen) {
	break;
      }
    }
  }
  // Now we should be on the first character of the nth argument So,
  // begin copying it to the return buffer until the next space or end
  // of the arg string 

  for(j=0;(i<arglen) && ((cmdstr[i]!=' ') || (cmdstr[i]=='"')) && 
	(cmdstr[i]!='\n'); i++, j++) {
    // Replace any nulls with a space 
    argstr[j] = (cmdstr[i]=='\0' ? ' ' : cmdstr[i]); 
  }

  argstr[j] = '\0';
}

int
mods_initTrans(char *host, int agw_no, struct sac_xy *sacxy)
{
  islcomp comp;
  islnum app;
  islconn conn;
  char errchar[4];
  char holder[10];
  char send_buff[BUFFSIZE];
  char recv_buff[BUFFSIZE];
  int len;
  int i;
  int ierr;
  int bcnt;

  memset(send_buff,0,sizeof(send_buff));
  memset(recv_buff,0,sizeof(recv_buff));

  sacxy->sfp_x0[agw_no]=0.0000;
  sacxy->sfp_xx[agw_no]=-1.0000;
  sacxy->sfp_xy[agw_no]=0.0000;
  sacxy->sfp_yx[agw_no]=0.0000;
  sacxy->sfp_y0[agw_no]=0.0000;
  sacxy->sfp_yy[agw_no]=1.0000;
  
  sacxy->gp_x0[agw_no]=0.0000;
  sacxy->gp_xx[agw_no]=-1.0000;
  sacxy->gp_xy[agw_no]=0.0000;
  sacxy->gp_yx[agw_no]=0.0000;
  sacxy->gp_y0[agw_no]=0.0000;
  sacxy->gp_yy[agw_no]=1.0000;
  
  sacxy->ccd_xx[agw_no]=0.066;
  sacxy->ccd_xy[agw_no]=0.0000;
  sacxy->ccd_yx[agw_no]=0.0000;
  sacxy->ccd_yy[agw_no]=0.066;
  
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

  sprintf(send_buff,"inittrans %d",agw_no);
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

    GetArgs(recv_buff,3,holder);
    sacxy->sfp_x0[agw_no]=atof(holder);

    GetArgs(recv_buff,4,holder);
    sacxy->sfp_xx[agw_no]=atof(holder);

    GetArgs(recv_buff,5,holder);
    sacxy->sfp_xy[agw_no]=atof(holder);

    GetArgs(recv_buff,6,holder);
    sacxy->sfp_yx[agw_no]=atof(holder);

    GetArgs(recv_buff,7,holder);
    sacxy->sfp_y0[agw_no]=atof(holder);

    GetArgs(recv_buff,8,holder);
    sacxy->sfp_yy[agw_no]=atof(holder);

    GetArgs(recv_buff,9,holder);
    sacxy->gp_x0[agw_no]=atof(holder);

    GetArgs(recv_buff,10,holder);
    sacxy->gp_xx[agw_no]=atof(holder);

    GetArgs(recv_buff,11,holder);
    sacxy->gp_xy[agw_no]=atof(holder);

    GetArgs(recv_buff,12,holder);
    sacxy->gp_yx[agw_no]=atof(holder);

    GetArgs(recv_buff,13,holder);
    sacxy->gp_y0[agw_no]=atof(holder);

    GetArgs(recv_buff,14,holder);
    sacxy->gp_yy[agw_no]=atof(holder);

    GetArgs(recv_buff,15,holder);
    sacxy->ccd_xx[agw_no]=atof(holder);

    GetArgs(recv_buff,17,holder);
    sacxy->ccd_xy[agw_no]=atof(holder);

    GetArgs(recv_buff,18,holder);
    sacxy->ccd_yx[agw_no]=atof(holder);

    GetArgs(recv_buff,19,holder);
    sacxy->ccd_yy[agw_no]=atof(holder);

    ierr=0;
  }

  (void) fflush(stdout);
  (void) islSendEOF(conn);  // iteration ends when EOF
  return ierr;
}
