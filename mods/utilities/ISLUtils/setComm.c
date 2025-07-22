/*!
  \file setComm.c
  \brief setComm - Utility for ISL Single Microlynx Command

  \author Staff, OSU Dept. of Astronomy (rgonzale@astronomy.ohio-state.edu)
  \date 2006 March 20 (rdg)

  \par Usage:
  int setComm(int controller,"ascii string"); return and error number. 

  NOTE: include/instrutil.h file has more information.

  Command motor(s) to open or close controller

*/
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <signal.h>
#include <unistd.h>

#include "instrutils.h"  // Command action functions header file
#include "ipckeys.h"  // Command action functions header file
#include "isisclient.h"  // ISIS common client library header

#include "isl_funcs.h"
#include "isl_types.h"
#include "params.h"      
#include "islcommon.h"   
#include "isl_shmaddr.h" 

/* ---------------------------------------------------------------- */
int
setComm(int controller, char cmd[])
{
  int err,parm, len;
  char cmdstr[512];
  char nextcmd[20];
  char dummy[512];
  char monit[20];
  void sem_att(key_t), shm_att(key_t), sem_rel(key_t), shm_det(key_t);

  memset(nextcmd,0,sizeof(nextcmd)); // Clear retrive buffer
  memset(cmdstr,0,sizeof(cmdstr)); // Clear return buffer
  memset(dummy,0,sizeof(dummy)); // Clear return buffer

  sprintf(cmdstr,"%s\r",cmd); 
  StrToUpper(cmdstr);

  if ( shm_addr->MODS.busy[controller] == 1) {
    return( -905); // microLYNX controller busy
  } else shm_addr->MODS.busy[controller] = 1;

  // Which mechanism....0-32
  strcpy(shm_addr->MODS.commport[controller].Port,shm_addr->MODS.TTYIP[controller]);
  shm_addr->MODS.modsx=1;
  if ( OpenTTYPort(&shm_addr->MODS.commport[controller]) == -1) { // Open IP:SOCKET
    shm_addr->MODS.busy[controller] = 0;
    return(-12); // cannot make a connection.
  }
  if(!strstr(cmdstr,",")) {
    WriteTTYPort(&shm_addr->MODS.commport[controller],cmdstr);
    rte_sleep(50);
    // read to microLnYNX
    ReadTTYPort(&shm_addr->MODS.commport[controller],dummy,1L);
  } else {
    logitf(cmdstr);
    len=strlen(cmdstr);
    err=0;
    for(parm=0;parm < len-1;parm++) {
      if(cmdstr[parm]!=',') {
	nextcmd[err]=cmdstr[parm];
	err++;
      } else {
	strcat(nextcmd,"\r");
	nextcmd[strlen(nextcmd)]='\0';
	//write to microLYNX
	WriteTTYPort(&shm_addr->MODS.commport[controller],nextcmd);
	rte_sleep(50);
	// read to microLnYNX
	ReadTTYPort(&shm_addr->MODS.commport[controller],dummy,1L);
	rte_sleep(50);
	err=0;
	memset(nextcmd,0,sizeof(nextcmd)); // Clear retrive buffer
      }
    }
  }
  rte_sleep(50);
  CloseTTYPort(&shm_addr->MODS.commport[controller]);  // Close IP:SOCKET
  rte_sleep(100);
  shm_addr->MODS.busy[controller] = 0;
  sprintf(monit,"/home2/isl/bin/smcbusy %d &",controller);
  system(monit);
  //  logitf(monit);
  return(0);
}
