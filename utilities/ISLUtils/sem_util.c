#include <sys/types.h>
#include <sys/ipc.h>    // interprocess communications header file
#include <sys/sem.h>    // shared memory header file
#include <sys/sem.h>    // shared memory header file

int semid_set(int ,int ,int);
int semid_get(key_t ,int ,int *);
void semid_att(key_t, int *);
void semid_take(int, int);
int semid_nb(int, int);
void semid_put(int, int);
int semid_val(int, int);
int semid_rel(key_t, int *);

static int semid = 0;

int sem_get(key_t key, int nsems)
{
  int iret, i;

  iret=semid_get( key, nsems, &semid);

  if(iret != -1)
    for (i=0;i <nsems; i++)
      semid_set(i,1,semid);

  return iret;
}

void sem_att( key_t key)
{
  semid_att(key,&semid);
}

void sem_take(int isem)
{
  semid_take( isem, semid);
}

int sem_nb(int isem)
{
  return (semid_nb( isem, semid));
}

void sem_put(int isem)
{
  semid_put( isem, semid);
}

int sem_val(int isem)
{
  return(semid_val( isem, semid));
}

void sem_set(int isem, int val)
{
  semid_set( isem, val, semid);
}

int sem_rel(key_t key)
{
  return(semid_rel( key, &semid));
}
