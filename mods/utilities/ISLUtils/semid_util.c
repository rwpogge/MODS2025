//
#include <stdio.h>
#include <errno.h>      // error code definition header file
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>    // interprocess communications header file
#include <sys/sem.h>    // shared memory header file

#if defined(__GNU_LIBRARY__) && !defined(_SEM_SEMUN_UNDEFINED)
// union semun is defined by including <sys/sem.h>
#else
// according to X/OPEN we have to define it ourselves
   union semun
   {
     int val;                           // value for SETVAL
     struct semid_ds *buf;              // buffer for IPC_STAT & IPC_SET
     unsigned short int *array;         // array for GETALL & SETALL
     struct seminfo *__buf;             // buffer for IPC_INFO
   };
#endif

//void exit(int);
int semid_get(key_t key,int nsems,int *semid)
{
int     i;
union semun arg;

                                          /* create, new key, permit all */
*semid = semget ( key, nsems, 0666|IPC_CREAT);
if ( *semid == -1 ) {
        perror("semid_get: create failed");
	return ( -1);
}
fprintf ( stdout, "semid_get: key=%d id=%d\n", key, *semid );

return( 0);
}

void semid_att(key_t key,int *semid)
{
   *semid = semget (key, 0, 0 );
   if ( *semid == -1 ) {
        perror("semid_att: translation failed");
        exit(-1);
   }
}

void semid_take(int isem,int semid)
{
   struct sembuf sb;

   sb.sem_num = isem;
   sb.sem_op  = -1;
   sb.sem_flg = SEM_UNDO;
 
   if( -1 == semop(semid,&sb,1)) {
      fprintf( stderr,"semid_take: isem %d ",isem);
      perror("take failed");
      exit(-1);
   }

}

int semid_nb(int isem,int semid)
{
   struct sembuf sb;
   int status;

   sb.sem_num = isem;
   sb.sem_op  = -1;
   sb.sem_flg = SEM_UNDO|IPC_NOWAIT;

   status = semop(semid,&sb,1);

   if( -1 == status && errno != EAGAIN) {
      fprintf( stderr,"semid_nb: isem %d ",isem);
      exit(-1);
   } else if (status == -1)
      return (-1);

   return ( 0);
}

void semid_put(int isem,int semid)
{
   struct sembuf sb;

   sb.sem_num = isem;
   sb.sem_op  = 1;
   sb.sem_flg = SEM_UNDO;
   if( -1 == semop(semid,&sb,1)) {
      fprintf( stderr,"semid_put: semid %d isem %d\n",semid,isem);
      perror("release failed");
      exit(-1);
   }
}

int semid_val(int isem,int semid)
{
   int status;

   if( -1 == ( status = semctl(semid,isem,GETVAL,0)) ) {
      fprintf( stderr,"semid_val: isem %d ",isem);
      perror("value get failed");
      exit(-1);
   }
   return (status);
}
void semid_set(int isem,int val,int semid)
{
   union semun arg;

   arg.val=val;
   if( -1 == semctl( semid, isem,SETVAL,arg.val)) {
      fprintf( stderr,"semid_set: isem %d ",isem);
      perror("value set failed");
      exit(-1);
   }
   return;
}

int semid_rel(key_t key,int *semid)
{
   *semid = semget (key, 0, 0 );
   if ( *semid == -1 ) {
        perror("semid_rel: translation failed");
        return ( -1);
   }

   if( -1 == semctl(*semid,0,IPC_RMID,0) ) {
      perror("semid_rel: release failed");
      return( -1);
   }
   return ( 0);
}
