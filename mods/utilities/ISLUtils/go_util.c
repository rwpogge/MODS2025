#include <stdio.h>
#include <memory.h>  // data type definition header file
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#include "instrutils.h"  // ISL Instrument header
#include "params.h"
#include "isl_types.h"
#include "islcommon.h"

extern struct islcommon *shm_addr;

static int semid = 0;

int semid_set(int ,int ,int);
int semid_get(key_t ,int ,int *);
void semid_att(key_t, int *);
int semid_rel(key_t, int *);

int go_rel(key_t);
int go_test(char []);
int go_get(key_t, int);
void go_put(char []);
static int go_find(char []);
int go_take(char [],int);
void go_att(key_t);

int go_get(key_t key,int nsems)
{
  void sem_take(key_t),sem_put(key_t);

  int iret, i;

  sem_take( SEM_GO);

  shm_addr->sem.allocated=0;

  iret=semid_get( key, nsems, &semid);

  if(iret != -1)
    for (i=0;i<nsems;i++)
      semid_set( i, 0, semid);

  sem_put( SEM_GO);

  return(iret);
}
void go_att(key_t key)
{
  semid_att(key,&semid);
}

int go_take(char name[7],int flags)
{
    int isem, semid_nb(int, int);
    void semid_take(int, int);
    int semid_val(int, int);

/*    printf("go_take enter, name %7.7s flags %d \n",name,flags); */
    isem=go_find(name);
/*    printf("isem %d val %d\n",isem, semid_val(isem, semid));*/


    if(semid_val( isem, semid) > 0) semid_set( isem, 0, semid);
    if( flags == 0) {
       semid_take(isem, semid);
       return( 0);
    } else {
       int iret;
       iret=semid_nb( isem, semid)==0 ? 0: 1;
       return iret;
     }
}

void go_put(char name[7])
{
    int  isem;
    void semid_put(int, int);

     isem=go_find(name);

    semid_put( isem, semid);
    return;
}    
int go_test(char name[7])
{
    int isem, iret;
    int semid_val(int, int);


/*    printf(" name %7.7s\n",name);*/

    isem=go_find(name);

/*    printf(" isem %d\n",isem); */

    iret=semid_val(isem, semid)==0? 1:0;

    return (iret);
}    
static int go_find(char name[7])
{
    int semid_val(int, int);
    void sem_take(key_t),sem_put(key_t);
    int i, isem;

    sem_take( SEM_GO);

/*  printf(" name %7.7s allocated %d\n",name,shm_addr->sem.allocated);*/
    isem = -1;

    for (i=0; i<shm_addr->sem.allocated && isem == -1; i++) {
/*     printf(" i %d sem.name %7.7s\n",i,shm_addr->sem.name[i]);*/
       if(0==memcmp(shm_addr->sem.name[i],name,7)) {
          isem=i;
       }
    }

/*  printf(" isem %d\n",isem); */
    if( isem == -1 )
      if (shm_addr->sem.allocated<SEM_NUM ) {
         isem=(shm_addr->sem.allocated)++;
         (void) memcpy(shm_addr->sem.name[isem],name,7);
      } else {
         fprintf( stderr," not enough semaphores for %7.7s\n",name);
         exit( -1);
      }

/*   printf(" ending isem %d allocated %d name %7.7s\n",
            isem,shm_addr->sem.allocated,shm_addr->sem.name[isem]);*/
    sem_put( SEM_GO);

    return( isem);
}

int go_rel(key_t key)
{
  return(semid_rel( key, &semid));
}

