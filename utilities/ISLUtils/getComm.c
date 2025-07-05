/*!
  \file getComm.c
  \brief getComm - Utility for ISL Single Microlynx Command

  \author Staff, OSU Dept. of Astronomy (rgonzale@astronomy.ohio-state.edu)
  \date 2006 March 20 (rdg)

  \par Usage:
  int getComm(int controller,"ascii string",float); Returns 0 or -error

  \date 2025 June 19 - AlmaLinux 9 port [rwp/osu]

*/
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <signal.h>
#include <unistd.h>

// no paths, use -I in Makefile

#include "instrutils.h"  // Command action functions header file
#include "ipckeys.h"
#include "isisclient.h"  // ISIS common client library header
#include "ics_client.h"  // Custom client application header file
#include "isl_funcs.h"
#include "isl_types.h"
#include "params.h"      // Common parameters and defines
#include "islcommon.h"   // Common parameters and defines
#include "isl_shmaddr.h" // Shared memory attachment.

/* 
 * ----------------------------------------------------------------
 */
int
getComm(int controller, char cmd[], float *val)
{
  char moving[512];
  char strcmd[512];
  int len;
  void sem_att(key_t), shm_att(key_t), shm_det(key_t);

  memset(moving,0,sizeof(moving)); // Clear buffer
  
  sprintf(strcmd,"%s\r",cmd);
  StrToUpper(strcmd);
  if(strstr(cmd,"PRINT"));
  else sprintf(strcmd,"PRINT %s\r",cmd);

  len=strlen(strcmd);

  if ( shm_addr->MODS.busy[controller] == 1) {  // Check for busy
    logit(NULL,-905,"CP");
    return( -905); // microLYNX controller busy.
  } else shm_addr->MODS.busy[controller] = 1;

  /*
   * Which mechanism....0-31 
   */
  strcpy(shm_addr->MODS.commport[controller].Port,shm_addr->MODS.TTYIP[controller]);

  //  shm_addr->MODS.modsx=1;
  if ( OpenTTYPort(&shm_addr->MODS.commport[controller]) == -1) { // Open IP/PORT
    logitf("getComm: Cannot open connection.");
    logit(NULL,-12,"HW");
    shm_addr->MODS.busy[controller] = 0;
    return(-12); // Connection cannot be opened.
  }

  if(strstr(strcmd,"POS")) {
    WriteTTYPort(&shm_addr->MODS.commport[controller],strcmd); // write to microLYNX
    rte_sleep(10);
    ReadTTYPort(&shm_addr->MODS.commport[controller],moving,1L);// read from microLYNX
    rmcrlf(moving,moving);                         // remove CRLF's
    shm_addr->MODS.pos[controller]=atof(&moving[len]);   // load shared memory
    *val = shm_addr->MODS.pos[controller];              // load return value
    shm_addr->MODS.busy[controller] = 0;                // release controller
  } else if(strstr(strcmd,"IO ")) {
    WriteTTYPort(&shm_addr->MODS.commport[controller],strcmd); // write to microLYNX
    rte_sleep(10);
    ReadTTYPort(&shm_addr->MODS.commport[controller],moving,1L);// read from microLYNX
    rmcrlf(moving,moving);                         // remove CRLF's
    sscanf(&moving[11],"%f",val);
  }
  shm_addr->MODS.busy[controller] = 0;                // release controller
  CloseTTYPort(&shm_addr->MODS.commport[controller]); // Close IP:SOCKET

  fprintf(stderr,"getComm: val=%f, moving=%s\n",val,moving);


  return(0); // Success!
}
