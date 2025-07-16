/*!
  \file getSensor.c
  \brief getSensor - Utility for ISL Single Microlynx Command

  \author Staff, OSU Dept. of Astronomy (rgonzale@astronomy.ohio-state.edu)
  \date 2006 March 20 (rdg)

  \par Usage:
  int getSensor(int controller,"ascii string",float); Returns 0 or -error

  \date 2025 Jun 19 - AlmaLinux 9 port [rwp/osu]

*/
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <signal.h>
#include <unistd.h>

// No paths!  Use -I in Makefile
//
#include "instrutils.h"   // Command action functions header file
#include "ipckeys.h"
#include "isisclient.h" // ISIS common client library header
#include "ics_client.h"     // Custom client application header file
#include "isl_funcs.h"
#include "isl_types.h"
#include "params.h"      // Common parameters and defines
#include "islcommon.h"     // Common parameters and defines
#include "isl_shmaddr.h"     // Shared memory attachment.

/* 
 * ----------------------------------------------------------------
 */
char *
getSensor(int controller, int cmd)
{
  char moving[512];
  int len;

  memset(moving,0,sizeof(moving)); // Clear buffer
  
  //  shm_addr->MODS.modsx=1;
  if ( OpenTTYPort(&shm_addr->MODS.commport[controller]) == -1) { // Open IP/PORT
    logitf("getSensor: Cannot open connection.");
    logit(NULL,-12,"HW");
    return("getSensor: Cannot open connection.");
  }

  if(cmd==20) {
    WriteTTYPort(&shm_addr->MODS.commport[controller],
		 "PRINT IO 26,IO 25, IO 24, IO 23, IO 22, IO 21\r");
    rte_sleep(10);  // Wait for about 10 centsec.
    ReadTTYPort(&shm_addr->MODS.commport[controller],moving,1L);
    rmcrlf(moving,moving);     // remove CRLF's
    moving[strlen(moving)]='\0';

  } else if(cmd==30) {
    WriteTTYPort(&shm_addr->MODS.commport[controller],
		 "PRINT IO 36,IO 35, IO 34, IO 33, IO 32, IO 31\r");
    rte_sleep(10);  // Wait for about 10 centsec.
    ReadTTYPort(&shm_addr->MODS.commport[controller],moving,1L);
    rmcrlf(moving,moving);     // remove CRLF's
    moving[strlen(moving)]='\0';
  }
  CloseTTYPort(&shm_addr->MODS.commport[controller]); // Close IP:SOCKET
  return(&moving[47]);
}
