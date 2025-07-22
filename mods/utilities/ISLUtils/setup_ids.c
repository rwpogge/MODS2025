//
// setup_ids() - setup instrument memory, signal, and runtime attributes
//
// See the 00README.txt file in this directory for an historical overview.
//
// Any main server (e.g., mmcService or agwServer) or auxiliary program
// (e.g., blueIMCS and redIMCS) that use shared memory and other low-level
// instrument mechanism functions for MODS must execute setup_ids()
// at the very start to provide access.
//
// Updated: 2025 July 2 - AlmaLinux 9 port [rwp/osu]
//

#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <signal.h>
#include <unistd.h>

#include "instrutils.h"  // ISL Instrument header
#include "ipckeys.h"
#include "params.h"
#include "isl_types.h"
#include "islcommon.h"

void setup_ids()
{
  void skd_att(key_t);
  void sem_att(key_t), shm_att(key_t), nsem_att(key_t);
  void go_att(key_t),brk_att(key_t), cls_att(key_t);

  setvbuf(stdout, NULL, _IONBF, BUFSIZ);
  setvbuf(stderr, NULL, _IONBF, BUFSIZ);

  if (sizeof(Islcommon) > C_RES ) {
    printf(" setup_ids: Islcommon C structure too large: %d bytes \n",
	   sizeof(Islcommon));
    exit(-1);
  }

  shm_att( SHM_KEY);

  cls_att( CLS_KEY);

  skd_att( SKD_KEY);

  sem_att( SEM_KEY);

  nsem_att( NSEM_KEY);

  brk_att( BRK_KEY);

  go_att( GO_KEY);

  //
  // Ignore all  signals that might abort the application
  // SIGINT, SIGQUIT, and SIGFPE
  //
  
  if (SIG_ERR==signal(SIGINT,SIG_IGN)) {
    perror("setup_ids: ignoring SIGINT");
    exit(-1);
  }

  if (SIG_ERR==signal(SIGQUIT,SIG_IGN)) {
    perror("setup_ids: ignoring SIGQUIT");
    exit(-1);
  }

  if (SIG_ERR==signal(SIGFPE,SIG_IGN)) {
    perror("setup_ids: ignoring SIGFPE");
    exit(-1);
  }
}
