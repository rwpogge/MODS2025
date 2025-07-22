#include <stdio.h>
#include <memory.h>
#include <errno.h>      /* error code definition header file */
#include <sys/types.h>
#include <sys/ipc.h>    /* interprocess communications header file */
#include <sys/shm.h>    /* shared memory header file */

#include "instrutils.h"  // ISL Instrument header
#include "params.h"
#include "isl_types.h"
#include "islcommon.h"

#define BAD_ADDR    (char *)(-1)

static char *begin[MAX_PTS];
static char *start[MAX_PTS];
static int   chars[MAX_PTS];
static int shmid = 0;
struct islcommon *shm_addr = NULL;

int shm_get(key_t key,int size)
{
  struct shmid_ds str_shmid,  // shared memory id struct
                  * buf;      // shared memory id struct pointer
  
  buf = & str_shmid;          // make buf point to str_shmid

  // create, new key, permit all
  shmid = shmget ( key, size, (IPC_CREAT|0666));
  if ( shmid == -1 ) {
    perror("shm_get: allocating segment");
    return ( -1);
  }

  // do a status on the shared memory segment
  if (-1 == shmctl ( shmid, IPC_STAT, buf )) {
    perror("shm_get: checking size");
    return( -1);
  }
  fprintf ( stdout, "shm_get: id=%d, size is %d bytes\n", shmid, buf->shm_segsz);
  
  return( 0);
}

void shm_att(key_t key)
{
  //  char  *shmat(int, const void *, int);
  
  if (C_RES < sizeof(struct islcommon)) {
    printf("shm_map: islcommon C area too large: %d bytes \n",
	   sizeof(struct islcommon));
    exit(-1);
  }
  
  shmid = shmget (key, 0, 0 );
  if ( shmid == -1 ) {
    perror("shm_att: translating key failed");
    exit( -1);
  }
  
  shm_addr = NULL;
  shm_addr = (struct islcommon *) shmat ( shmid, (char *) shm_addr, 0 );
  if ( BAD_ADDR  == (char *) shm_addr ) {
    perror("shm_att: attaching memory segment failed");
    exit( -1);
  }
  
}

int shm_det( )
{
  if(-1==shmdt( (char *) shm_addr)) {
    perror("shm_det: detaching shared memory");
    return( -1);
  }
  return( 0);
}

int shm_rel(key_t key)
{
  struct shmid_ds str_shmid, *buf;
  
  buf = &str_shmid;
  
  shmid = shmget (key, 0, 0 );
  if ( shmid == -1 ) {
    perror("shm_rel: translating key");
    return ( -1);
  }
  
  if ( -1 == shmctl ( shmid, IPC_RMID, buf )) {
    perror("shm_rel: removing id");
    return ( -1);
  }
  return( 0);
  
}

void shm_map(int *b_1,int *e_1,int *b_2,int *e_2)
{
  int   i, total;
  
  begin[0]=NULL;
  begin[1]=(char *) b_1;
  begin[2]=(char *) b_2;
  
  chars[0]=C_RES;   // reserve 4k
  chars[1]=(e_1-b_1+1)*sizeof(int);
  chars[2]=(e_2-b_2+1)*sizeof(int);
  
  start[0]=(char *) shm_addr;
  total=chars[0];

  for (i=1; i<MAX_PTS;i++)  {
    start[i]=start[i-1]+chars[i-1];
    total+=chars[i];
  }
  
  if (total > SHM_SIZE) {
    printf("shm_map: islcommon too large: %d bytes \n",total);
    exit(-1);
  }
}

void shm_read(int *b_read)
{
  int i,ipts;
  char *s1;
  char *s2;
  
  ipts=-1;
  for (i=1;i<MAX_PTS;i++) 
    if (((char *)b_read) == begin[i]) {
      ipts=i; break;}
  if(ipts == -1) {
    perror("shm_read: address lookup failed");
    exit( -1);
  }
  s1=(char *)memcpy((char *)b_read,start[ipts],chars[ipts]);
}

void shm_write(int *b_write)
{
  int i,ipts;
  char *s1;
  
  ipts=-1;
  for (i=1;i<MAX_PTS;i++) 
    if (((char *)b_write) == begin[i]) {
      ipts=i; break;}
  if(ipts == -1) {
    perror("shm_write: address lookup failed");
    exit( -1);
  }
  s1=(char *)memcpy(start[ipts],(char *)b_write,chars[ipts]);
}
