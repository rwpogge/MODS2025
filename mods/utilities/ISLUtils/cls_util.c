#include <stdio.h>	// standard I/O header file
#include <limits.h>     // limits header file
#include <errno.h>	// error number definitions
#include <memory.h>     // shared memory IPC header file
#include <sys/types.h>
#include <sys/ipc.h>	// interprocess communications (IPC) header file
#include <sys/msg.h>	// message IPC header file

#include "instrutils.h"
#include "params.h"
#include "isl_types.h"
#include "islcommon.h"
#include "isl_shmaddr.h"

#define MAX_TEXT  4096

struct  cls_buf {
	long mtype;
        struct {
          int parm[2];
          unsigned char mtext[ MAX_TEXT];
        } messg;
};

static int msqid;

int cls_get(key_t,int);
void cls_ini(key_t);
void cls_att(key_t);
void cls_snd(long*,char*,int,int,int);
long cls_alc();
static long cls_alc_s();
int cls_rcv(long ,char*,int,int*,int*,int,int);
void cls_clr(long);
int cls_rel(key_t);
static int cls_chk(long,int,int);

int cls_get(key_t key,int size)
{
  struct msqid_ds str_msqid;  // message queue id struct

  msqid = msgget( key, ( IPC_CREAT | 0666 ));
  if ( msqid == -1 ) {
    perror("cls_get: creating cls queue");
    return( -1);
  }
  
  // set the queue size
  if( -1 == msgctl( msqid, IPC_STAT, &str_msqid )) {
    perror("cls_get: getting status");
    return( -1);
  }
  str_msqid.msg_qbytes = size;
  if( -1 == msgctl( msqid, IPC_SET, &str_msqid )) {
    perror("cls_get: setting status");
    return( -1);
  }
  
  fprintf( stdout,"cls_get: id %d\n",msqid);
  
  return( 0);
}

void cls_ini(key_t key)
{
  int	status, i;
  long    msgtype;
  void    sem_take(key_t),sem_put(int);
  struct  cls_buf msg;
  
  msqid = msgget( key, 0);
  if ( msqid == -1 ) {
    perror("cls_ini: translating key");
    exit( -1);
  }
  
  sem_take( SEM_CLS);
  
  while ( -1 !=
	  msgrcv( msqid,(struct msgbuf *)&msg, sizeof( msg.messg ), -LONG_MAX,IPC_NOWAIT|MSG_NOERROR));
  if( errno != ENOMSG){
    perror("cls_ini: error cleaning mclass numbers");
    exit(-1);
  }
  for (i=0;i<MAX_CLS;i++)
    shm_addr->nums[i]=0;
  shm_addr->mclass_count=0;
  
  sem_put( SEM_CLS);
}

void cls_att(key_t key)
{
  msqid = msgget(key, 0);
  if ( msqid == -1 ) {
    perror("cls_ini: translating key");
    exit( -1);
  }
}

long cls_alc()
{
  long mclass;
  void sem_take(key_t), sem_put(int);
  
  sem_take( SEM_CLS);
  mclass=cls_alc_s();
  sem_put( SEM_CLS);
  return(mclass);
}

static long cls_alc_s()
{
  int	i;
  long    mclass;
  int    imod;
  
  mclass=0;
  for (i=0;i<MAX_CLS;i++) {
    imod=(shm_addr->mclass_count+i)%MAX_CLS;
    if (shm_addr->nums[imod] == 0) {
      mclass=imod+1;
      shm_addr->nums[imod]=1;
      shm_addr->mclass_count=(imod+1)%MAX_CLS;
      break;
    }
  }
  
  if(mclass == 0) {
    fprintf( stderr,"cls_alc_s: out of mclass numbers\n");
    return -1;
  }
  return( mclass);
}

//mclass: message queue id in which to place buffer
//buffer: contains message for process
//length: length of buffer in bytes
//int parm3: use to be now char
//int parm4: use to be now char
void cls_snd( long *mclass,
	      char *buffer,
	      int length,
	      int parm3,
	      int parm4)
{
  int	status, i;
  size_t  nchars;
  struct  cls_buf msg;
  long    msgtype;
  char    *s1;
  void sem_take(key_t), sem_put(int);
  
  if(length > MAX_TEXT) {
    length=MAX_TEXT;
  } else if(length < 0)
    length=0;

  *mclass &= ~ 0160000;
  sem_take( SEM_CLS);
  if(*mclass == 0)
    if( 0 > (*mclass = cls_alc_s())){
      fprintf(stderr,"cls_alc_s error in cls_snd()\n");
      exit (-1);
    }
  if(cls_chk( *mclass, 1,0)) {
    fprintf(stderr,"cls_chk:mclass error in cls_snd()\n");
    exit(-1);
  }
  sem_put( SEM_CLS);
  
  msg.mtype = *mclass;
  msg.messg.parm[0]=parm3;
  msg.messg.parm[1]=parm4;
  s1=(char *)memcpy( msg.messg.mtext, buffer, length);
  status = msgsnd( msqid, (struct msgbuf *)&msg,
		   length+sizeof( msg.messg.parm), 0);
  if ( status == -1 ) {
    perror("cls_snd: sending message");
    exit(-1);
  }
}

int cls_rcv(long mclass,
	    char *buffer,
	    int length,
	    int *rtn1,
	    int *rtn2,
	    int msgflg,
	    int save)
{
  int     nchars, sb, sc, nw;
  struct  cls_buf msg;
  char *s1;
  void sem_take(key_t), sem_put(int);

  sb= 0040000 & mclass;
  sc= 0020000 & mclass;
  nw= 0100000 & mclass;
  mclass &= ~0160000;
  if(msgflg != 0 || nw) msgflg=IPC_NOWAIT;
  if(save   != 0 || sc) save = 1;

  sem_take( SEM_CLS);
  nchars = msgrcv( msqid, (struct msgbuf *)&msg, sizeof( msg.messg),
		   mclass+MAX_CLS,IPC_NOWAIT);

  if ( (nchars == -1) && (errno != ENOMSG)) {
    fprintf( stderr,"cls_rcv: msqid %d mclass %d,",msqid,mclass);
    perror("getting saved mclass");
    exit(-1);
  } else if (nchars != -1) {
    goto copy;
  }

  if(msgflg==0) {
    if(cls_chk( mclass, 1, 0)) {
      fprintf(stderr,"cls_chk error 1 in cls_rcv()\n");
      exit(-1);
    }
    sem_put(SEM_CLS);
  }
  
  nchars = msgrcv( msqid, (struct msgbuf *)&msg, sizeof( msg.messg ),
		   mclass, msgflg);
  if ( (nchars == -1) && (errno != ENOMSG)) {
    fprintf( stderr,"cls_rcv: msqid %d mclass %d,",msqid,mclass);
    perror("getting mclass");
    exit(-1);
  } else if (nchars == -1) {
    sem_put( SEM_CLS);
    return( -1);
  }

  if(msgflg==0) {
    sem_take( SEM_CLS);
    if(cls_chk( mclass, -1, save)) {
      fprintf(stderr,"cls_chk error 2 in cls_rcv()\n");
      exit(-1);
    }
  }
 copy:
  if( sb ) {
    msg.mtype = mclass + MAX_CLS;
    if ( -1 == msgsnd( msqid, (struct msgbuf *)&msg, nchars, IPC_NOWAIT)) {
      perror("cls_rcv: sending saved message");
      exit(-1);
    }
  } else 
    if( cls_chk( mclass, -1, save)) {
      fprintf(stderr,"cls_chk error 3 in cls_rcv()\n");
      exit(-1);
    }
  
  sem_put( SEM_CLS);

  *rtn1=msg.messg.parm[0];
  *rtn2=msg.messg.parm[1];
  
  nchars=nchars-sizeof(msg.messg.parm);
  nchars=(nchars > length) ? length:nchars;
  s1=(char *)memcpy(buffer, msg.messg.mtext, nchars);
  return(nchars);
}

void cls_clr(long mclass)
{
  struct  cls_buf msg;
  void sem_take(key_t), sem_put(int);
  
  mclass &= ~0160000;
  if(mclass==0) return;
  
  sem_take( SEM_CLS);
  
  while ( -1 !=
	  msgrcv( msqid, (struct msgbuf *)&msg, sizeof( msg.messg ), mclass,
		  IPC_NOWAIT|MSG_NOERROR));
  if( errno != ENOMSG){
    perror("cls_clr: error clearing mclass buffers");
    exit(-1);
  }
  
  while ( -1 !=
	  msgrcv(msqid,(struct msgbuf *)&msg,sizeof( msg.messg), mclass+MAX_CLS, IPC_NOWAIT|MSG_NOERROR));
  if( errno != ENOMSG){
    perror("cls_clr: error clearing saved mclass buffers");
    exit(-1);
  }

  shm_addr->nums[mclass-1]=0;
  
  sem_put( SEM_CLS);
}

int cls_rel(key_t key)
{
  if(-1==msgctl( msqid, IPC_RMID, NULL )) {
    perror("cls_rel: release cls queue");
    return(-1);
  }
  return (0);
}

static int cls_chk(long mclass,int action,int save)
{
  if (mclass <1 || mclass > MAX_CLS) {
    //
    int i;
      for(i=0;i<MAX_CLS;i++)
         fprintf( stderr, "::mclass %d num %x\n",i, shm_addr->nums[i]);
    fprintf( stderr,"cls_chk: illegal mclass number %d pid %d\n",mclass,getpid());
    return -1;
  }
  
  if (shm_addr->nums[mclass-1] == 0) {
#if 0
    // For debugging
    int i;
    for(i=0;i<MAX_CLS;i++)
      fprintf( stderr, "mclass %d num %x\n",i, shm_addr->nums[i]);
#endif
    fprintf( stderr,"cls_chk: mclass %d not allocated\n",mclass);
    return -1;
  }
  shm_addr->nums[mclass-1]+= action;
  if(shm_addr->nums[mclass-1] < 1) {
#if 0
    // For debugging
   int i;
   for(i=0;i<MAX_CLS;i++)
     fprintf( stderr, "mclass %d num %x\n",i, shm_addr->nums[i]);
#endif
    fprintf( stderr,"cls_chk: mclass %d decremented too far\n",mclass);
    return -1;
  } else if ( shm_addr->nums[mclass-1] == 1 && save == 0) {
    shm_addr->nums[mclass-1]=0;
  }
  return 0;
}
