#include <stdio.h>
#include <memory.h>
#include <errno.h>      /* error code definition header file */
#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>    /* interprocess communications header file */
#include <sys/shm.h>    /* shared memory header file */
#include <sys/sem.h>    // shared memory header file

#include "../include/instrutils.h"
#include "../include/params.h"
#include "../include/isl_types.h"
#include "../include/islcommon.h"
#include "../include/ipckeys.h"
#include "../include/isl_shmaddr.h"

int nsem_get(key_t,int);
void nsem_att(key_t);
int nsem_rel(key_t);
int go_rel(key_t);
int brk_get(key_t,int);
int brk_ini(int);
int brk_rel(key_t);
int skd_rel(key_t);
int skd_get(key_t,int);
int skd_ini(key_t);
int sem_get(key_t,int);
int shm_get(key_t,int);
int sem_rel(key_t);
int shm_rel(key_t);
void sem_att(key_t);
int shm_det();
void go_att(key_t);
int cls_get(key_t,int);
int cls_rel(key_t);
void cls_ini(key_t);

int main()
{
  int size, nsems, shm_id, sem_id, cls_id, skd_id, brk_id;
  key_t key;
  void shm_att(key_t);
  // long rte_secs();
  long rte_secs;
  
  key = SHM_KEY;
  size = SHM_SIZE;
  
  if( (shm_id = shm_get( key, size)) == -1) {
    fprintf( stderr, " shm_get failed \n");
    exit( -1);
  }
  shm_att(key);
  shm_addr->time.index = 0;
  shm_addr->time.offset[0] = 0;
  shm_addr->time.offset[1] = 0;
  shm_addr->time.epoch[0] = 0;
  shm_addr->time.epoch[1] = 0;
  shm_addr->time.icomputer[0]=0;
  shm_addr->time.icomputer[1]=0;
  shm_addr->time.secs_off = rte_secs; // ();
  
  key = SEM_KEY;
  nsems = SEM_NUM;
  if( (sem_id = sem_get( key, nsems)) == -1) {
    fprintf( stderr," sem_get failed\n");
    goto cleanup2;
  }
  sem_att(key);
  
  key = NSEM_KEY;
  nsems = SEM_NUM;
  if( (sem_id = nsem_get( key, nsems)) == -1) {
    fprintf( stderr," nsem_get failed\n");
    goto cleanup3;
  }
  nsem_att(key);
  
  key = CLS_KEY;
  size = CLS_SIZE;
  if( (cls_id = cls_get( key, size)) == -1) {
    fprintf( stderr," cls_get failed\n");
    goto cleanup4;
  }
  cls_ini( key);
  
  key = SKD_KEY;
  size = SKD_SIZE;
  if( (skd_id =  skd_get( key, size)) == -1) {
    fprintf( stderr," skd_get failed\n");
    goto cleanup5;
  }
  skd_ini( key);
  
  key = BRK_KEY;
  size = BRK_SIZE;
  if( (brk_id =  brk_get( key, size)) == -1) {
    fprintf( stderr," brk_get failed\n");
    goto cleanup6;
  }
  brk_ini( key);
  
  key = GO_KEY;
  nsems = SEM_NUM;
  if( (sem_id = nsem_get( key, nsems)) == -1) {
    fprintf( stderr," nsem_get failed\n");
    goto cleanup7;
  }
  go_att(key);
  
  
  exit( 0);
  
 cleanup7:
  key = NSEM_KEY;
  if( -1 == go_rel( key)) {
    fprintf( stderr," go_rel failed\n");
  }
 cleanup6:
  key = SKD_KEY;
  if( -1 == skd_rel( key)) {
    fprintf( stderr," skd_rel failed\n");
  }
 cleanup5:
  key = CLS_KEY;
  if( -1 == cls_rel( key)) {
    fprintf( stderr," cls_rel failed\n");
  }
 cleanup4:
  key = SEM_KEY;
  if( -1 == sem_rel( key)) {
    fprintf( stderr," sem_rel failed\n");
  }
 cleanup3:
  key = NSEM_KEY;
  if( -1 == nsem_rel( key)) {
    fprintf( stderr," nsem_rel failed\n");
  }
 cleanup2:
  if( shm_det( ) == -1) {
    fprintf( stderr, " shm_det failed \n");
  }
 cleanup1:
  key = SHM_KEY;
  if( shm_rel( key) == -1) {
    fprintf( stderr, " shm_rel failed \n");
  }
  exit( -1);
}
