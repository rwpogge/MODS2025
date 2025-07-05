/*!
  \file smcbusy.c
  \brief smcbusy - ISLSN SMC Controller for busy status

  \author Staff, OSU Astronomy Dept. (staff@astronomy.ohio-state.edu)
  \date 2005 March 20 (Astronomy Staff)

  \par Usage: Unix style.
  smcbusy (int controller);

   Monitors motor(s) and loads Shared Memory with position.

*/

#include "instrutils.h"   // Command action functions header file
#include "isisclient.h" // ISIS common client library header
#include "ics_client.h"     // Custom client application header file
#include "isl_funcs.h"
#include "isl_types.h"
#include "params.h"      // Common parameters and defines
#include "islcommon.h"     // Common parameters and defines
#include "isl_shmaddr.h"     // Shared memory attachment.

#define TRUE_FALSE_BIT "PRINT MVG," ",POS\r"
#define POSITION_CHK "PRINT POS\r"
#define IO_CHK "PRINT IO 21,IO 22\r"
#define BREAK_CHK "PRINT IO 26\r"

extern int rmcrlf(char [], char []);
//----------------------------------------------------------------
int
smcbusy(int controller)
{
  int busy, ierr;
  char moving[512];
  char logpos[80];
  char monit[20];
  int iolimit;
  int childpid;

  memset(monit,0,sizeof(monit));
  if ( shm_addr->MODS.busy[controller] == 1) {
    ierr = -1;
    return ierr;
  } else {
    shm_addr->MODS.busy[controller]=1;
  }
  strcpy(shm_addr->MODS.commport[controller].Port,shm_addr->MODS.TTYIP[controller]);
  rte_sleep(200);
  if ( OpenTTYPort(&shm_addr->MODS.commport[controller]) == -1) {
    shm_addr->MODS.busy[controller]=0;
    ierr = -11;
    return ierr;
  }
  while(1) {
    memset(moving,0,sizeof(moving)); 
    rte_sleep(10); 
    WriteTTYPort(&shm_addr->MODS.commport[controller],TRUE_FALSE_BIT);
    rte_sleep(10); 
    ReadTTYPort(&shm_addr->MODS.commport[controller],moving,1L);
    rte_sleep(10); 
    if (strstr(moving,"FALSE") || shm_addr->MODS.busy[controller] == 0) {
      break;
    }
    rmcrlf(moving,moving);
    shm_addr->MODS.pos[controller]=atof(&moving[18]);
  } // while motor(s) are moving do not disturb.

  memset(moving,0,sizeof(moving)); 
  rte_sleep(20); 
  WriteTTYPort(&shm_addr->MODS.commport[controller],POSITION_CHK);
  rte_sleep(20); 
  ReadTTYPort(&shm_addr->MODS.commport[controller],moving,2L);

  rte_sleep(20);   // Careful on the last update. you might get junk so give
  // an extra 200ms.
  rmcrlf(moving,moving);
  shm_addr->MODS.pos[controller]=atof(&moving[9]);
  
  memset(moving,0,sizeof(moving)); 
  WriteTTYPort(&shm_addr->MODS.commport[controller],IO_CHK);

  ReadTTYPort(&shm_addr->MODS.commport[controller],moving,2L);
  rte_sleep(20); 
  shm_addr->MODS.qued[controller]=atoi(&moving[19]); // Check limit switches
  
  CloseTTYPort(&shm_addr->MODS.commport[controller]); // Clean up and Close the socket
  shm_addr->MODS.busy[controller]=0;
  ierr = 0;
  return (ierr);
}

