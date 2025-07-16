#include <stdio.h>	// standard I/O header file
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>	// error number definitions
#include <termios.h>   
#include <fcntl.h>   
#include <limits.h>
#include <signal.h>
#include <sys/types.h>	// standard data types definitions
#include <sys/ipc.h>	// interprocess communications (IPC) header file
#include <sys/msg.h>	// message IPC header file
#include <sys/shm.h>    // shared memory IPC header file
#include <memory.h>    // shared memory IPC header file
#include <sys/file.h>

//define  MAX_BUF       4096
#define  MAX_BUF       256

struct skd_buf {
	long	mtype;
        struct {
	    long    ip[5];
	    long    rtype;
	    int     dad;
	    int timed_out;
	    char arg[MAX_BUF+1];
        } messg;

} ;

static int msqid;
static long rtype=0;
static int dad=0;
static char prog_name[7];
static long save_ip[5];
static char arg[MAX_BUF+1];
static char *argv[(MAX_BUF+1)/2];
static int  argc = -1;
static long wait_rtype=0;
static long ipr[5] = { 0, 0, 0, 0, 0};

int skd_chk(char [],long []);
void skd_end(long []);
static long mtype(char []);
static void nullfcn(int);
static void skd_end_to(long *, long *,int);
void rte_alarm(unsigned);
void nsem_put(char []);
static int skd_run_arg_cls_to(char *name, char w, long ip[5], char *arg,
			      char nsem[6],unsigned to);

int skd_get( key_t key,int size)
{
  struct msqid_ds str_msqid;  // message queue id struct

  // create a message queue
  msqid = msgget( key, ( IPC_CREAT | 0666 ));
  if ( msqid == -1 ) {
    perror("skd_get: getting queue");
    return( -1);
  }

  // set the queue size
  if (-1== msgctl( msqid, IPC_STAT, &str_msqid )) {
    perror("skd_get: getting status");
    return( -1);
  }

  str_msqid.msg_qbytes = size;
  if(-1 == msgctl( msqid, IPC_SET, &str_msqid )) {
    perror("skd_get: setting size");
    return( -1);
  }
  fprintf( stdout,"skd_get: id %d\n",msqid);

  return( 0);
}

void skd_ini(key_t key)
{
struct skd_buf sched;
int status;

msqid = msgget( key, 0);

if ( msqid == -1 ) {
    perror("skd_ini: translating key");
    exit( -1);
}

waitr:
status = msgrcv( msqid, (struct msgbuf *) &sched, sizeof( sched.messg),
                -LONG_MAX,IPC_NOWAIT|MSG_NOERROR);

 if (status != -1)
   goto waitr;
 else {
   if (errno == EINTR)
     goto waitr;
   else if (errno != ENOMSG) {
     perror("skd_ini: error cleaning skd queue\n");
     exit( -1);
   }
 }
}

void skd_att(key_t key)
{
    msqid = msgget( key, 0);
    if ( msqid == -1 ) {
        perror("skd_att: translating key");
        exit( -1);
    }
}

// buffer - contains message for process
// length - length of buffer in bytes
void skd_islmgr_inject_w(long *iclass, char *buffer,int length)
{
  char insnp[]="insnp  ";
  void nsem_take(char[], int);
  void cls_snd(long*,char*,int,int,int);

  if(*iclass==0)
    return;

  nsem_take(insnp,0);

  // Execute this MLC command via "isl"
  cls_snd( iclass, buffer, length, 0, 1);
  skd_run_arg_cls_to("islerr ",'w',ipr,(char *) NULL,insnp,(unsigned) 0);


}
void skd_run(char name[7],char w,long ip[5])
{
  skd_run_arg_cls_to( name, w, ip, (char *) NULL, (char *) NULL, (unsigned) 0);
}

void skd_run_arg(char name[7],char w,long ip[5],char *arg)
{
  skd_run_arg_cls_to( name, w, ip, arg, NULL,(unsigned) 0);
}

int skd_run_to(char name[7],char w,long ip[5],unsigned to)
{  
  return skd_run_arg_cls_to(name, w, ip, (char *) NULL, (char *) NULL, to);
}

static int skd_run_arg_cls_to(char name[7],
			  char w,
			  long ip[5],
			  char *arg,
			  char nsem[6],
			  unsigned to)
{
int	status, i, n;
struct skd_buf sched;

 if( w == 'w') sched.messg.rtype=(1<<30)|getpid();
 else          sched.messg.rtype=0;
 
if(name!=NULL) {
   sched.mtype=mtype(name);
   
   for (i=0;i<5;i++) {
     sched.messg.ip[i]=ip[i];
   }
   
   sched.messg.dad=getpid();

   if(arg==NULL)
     arg="";

   n=strlen(arg)+1;
   n = n > MAX_BUF + 1 ? MAX_BUF + 1: n;
   strncpy(sched.messg.arg,arg,n);
   
 waits:
   status = msgsnd(msqid, (struct msgbuf *) &sched,
		  sizeof(sched.messg)+strlen(sched.messg.arg)-(MAX_BUF+1), 0 );

   if (status == -1) {
     if(errno == EINTR)
       goto waits;
     else {
       fprintf( stderr,"skd_run: msqid %d,",msqid);
       perror(" sending schedule message");
       exit( -1);
     }
   }
 }

if(nsem!=NULL && nsem[0]!=0)
  nsem_put(nsem);

 if(w != 'w') return(-1);

 if(to !=0) {
   if(signal(SIGALRM,nullfcn) == SIG_ERR){
     fprintf( stderr,"skd_wait: setting up signals\n");
     exit(-1);
   }
   wait_rtype=sched.messg.rtype;
   rte_alarm( to);
 }

waitr:
status = msgrcv( msqid, (struct msgbuf *) &sched, sizeof(sched.messg),
		 sched.messg.rtype, 0);

if(status == -1) {
  if(errno == EINTR)
    goto waitr;
  else {
    perror("skd_run: receiving return message");
    exit( -1);
  }
}

 if (to !=0) {
   rte_alarm((unsigned) 0);
   if(signal(SIGALRM,SIG_DFL) == SIG_ERR){
     fprintf( stderr,"skd_wait: setting default signals\n");
     exit(-1);
   }
   to=0;
   }

 for (i=0;i<5;i++)
   save_ip[i]=sched.messg.ip[i];

 return sched.messg.timed_out;
 
}
void skd_par(long ip[5])
{
int i;

for (i=0;i<5;i++)
    ip[i]=save_ip[i];

}
void skd_arg(int n,char *buff,int len)
{
  char *ptr;
  int n1;

  if (argc < 0) {
    argc = 0;
    ptr = strtok(arg," ");
    while ( NULL != (argv[argc++] = ptr) ) {
      ptr = strtok(NULL," ");
    }
  }

  if (n < argc && argv[n] != NULL) {
    n1=strlen(argv[n])+1;
    n1 = n1 > len? len: n1;
    strncpy(buff,argv[n],n1);
    if(n1==n && len > 0)
      buff[n-1]='\0';
  } else if (len > 0)
    buff[0]='\0';

}

int skd_chk(char name[7],long ip[5])
{
int	status,i;
struct skd_buf	sched;
long    type;
char *s1;


 skd_end(ip);

type=mtype(name);
 s1=(char *)memcpy(prog_name,name,7);

waitr:
status = msgrcv( msqid, (struct msgbuf *) &sched, sizeof( sched.messg),
		type, IPC_NOWAIT);

if(status == -1) {
  if(errno == EINTR)
    goto waitr;
  else if (errno == ENOMSG)
    return 0;
  else {
    perror("skd_chk: receiving message");
    exit( -1);
  }
}

for (i=0;i<5;i++)
    ip[i]=sched.messg.ip[i];

rtype=sched.messg.rtype;

if (getpid() == sched.messg.dad)
  dad=0;
else
  dad=sched.messg.dad;

strcpy(arg,sched.messg.arg);
argc=-1;

return 1;
}
int skd_end_inject_qc(char name[7],long ip[5])
{
int	status,i;
struct skd_buf	sched;
long    type;
char *s1;

 if(strncmp("islmgr ",name,7)==0) {
   type=mtype("islmgrx");

 waitrx:
   status = msgrcv( msqid, (struct msgbuf *) &sched, sizeof( sched.messg),
		type, IPC_NOWAIT);

   if(status == -1) {
     if(errno == EINTR)
       goto waitrx;
     else if (errno == ENOMSG) {
       goto next;
     } else {
       perror("skd_end_inject_qc: receiving message 1");
       exit( -1);
     }
   }

   goto process;
 }

 next:
type=mtype(name);
 s1=(char *)memcpy(prog_name,name,7);

waitr:
status = msgrcv( msqid, (struct msgbuf *) &sched, sizeof( sched.messg),
		type, IPC_NOWAIT);

if(status == -1) {
  if(errno == EINTR)
    goto waitr;
  else if (errno == ENOMSG)
    return -1;
  else {
    perror("skd_end_inject_qc: receiving message 2");
    exit( -1);
  }
}

 process:
if(sched.messg.rtype == 0)
  goto waitr;

rtype=sched.messg.rtype;

if (getpid() == sched.messg.dad)
  dad=0;
else
  dad=sched.messg.dad;

strcpy(arg,sched.messg.arg);
argc=-1;

 skd_end(ip);

  return 0;

}

void skd_wait(char name[7],long ip[5],unsigned centisec)
{
int	status,i;
struct skd_buf	sched;
long    type;
char *s1;


 skd_end(ip);

 type=mtype(name);
 s1=(char *)memcpy(prog_name,name,7);
 if(centisec !=0) {
   if(signal(SIGALRM,nullfcn) == SIG_ERR){
     fprintf( stderr,"skd_wait: setting up signals\n");
     exit(-1);
   }
  rte_alarm( centisec);
 }

waitr:
 status = msgrcv( msqid, (struct msgbuf *) &sched, sizeof( sched.messg),
		  type, 0);
 if (centisec !=0) {
   rte_alarm((unsigned) 0);
   if(signal(SIGALRM,SIG_DFL) == SIG_ERR){
     fprintf( stderr,"skd_wait: setting default signals\n");
     exit(-1);
   }
   centisec=0;
 }

 if (status == -1) {
   if(errno == EINTR)
     goto waitr;
   else {
     perror("skd_wait: receiving message");
     exit( -1);
   }
 }

 if(strncmp("isl    ",name,7)==0 && sched.messg.rtype!=0) {
   sched.mtype=mtype("islx   ");
 waits:
   status = msgsnd(msqid, (struct msgbuf *) &sched,
		   sizeof(sched.messg)+strlen(sched.messg.arg)-(MAX_BUF+1), 0);
   
   if (status == -1) {
     if(errno == EINTR)
       goto waits;
     else {
       fprintf( stderr,"skd_run: msqid %d,",msqid);
       perror(" sending schedule message");
       exit( -1);
     }
   }
   sched.messg.rtype=0;
 }
   
for (i=0;i<5;i++)
    ip[i]=sched.messg.ip[i];

rtype=sched.messg.rtype;

if (getpid() == sched.messg.dad)
  dad=0;
else
  dad=sched.messg.dad;

 strcpy(arg,sched.messg.arg);
 argc=-1;

}

void skd_end(long ip[5])
{
  skd_end_to(ip,&rtype,0);
}

static void skd_end_to(long ip[5],long *rtype_in,int timed_out)
{
  int i, status;
  struct skd_buf sched;

  if( *rtype_in != 0) {
    for (i=0;i<7;i++) {
      sched.messg.ip[i]=ip[i];
    }
    sched.messg.timed_out=timed_out;
    sched.mtype=*rtype_in;
  waits:
    status = msgsnd( msqid, (struct msgbuf *) &sched, sizeof( sched.messg),
		      0 );
    if (status == -1) {
      if(errno == EINTR)
	goto waits;
      else {
	perror("skd_end_to: sending termination message");
	exit( -1);
      }
    }
    *rtype_in = 0;
  }
}
void skd_clr(char name[7])
{
int	status;
struct skd_buf	sched;
long    type;
char *s1;

type=mtype(name);

waitr:
status = msgrcv( msqid, (struct msgbuf *) &sched, sizeof( sched.messg),
		type, IPC_NOWAIT);

if(status!=-1)
  goto waitr;
else if(errno == EINTR)
  goto waitr;
else if (errno != ENOMSG) {
  perror("skd_clr: receiving message");
  exit( -1);
}
}

int skd_rel(key_t key)
{
int status;
/* release specified shared memory segment */
if(-1==msgctl( msqid, IPC_RMID, 0 )) {
   perror("skd_rel: releasing skd id");
   return( -1);
}
return( 0);
}

static long mtype(char name[7])
{
    int i;
    long val;

    val=0;
    for (i=0;i<7;i++) {
       if(name[i] != ' ' && name[i] != 0 ) {
           val+=(tolower(name[i])-'a')<<(5*i);
       }
    }
    return(val);
}
static void nullfcn(int sig)
{
    int i;
    void skd_run(char [],char,long []);

    if(signal(sig,SIG_IGN) == SIG_ERR ) {
      perror("nullfcn: error ignoring signal");
      exit(-1);
    }

    if(wait_rtype==0)
      skd_run(prog_name,'n',ipr);
    else
      skd_end_to(ipr,&wait_rtype,1);

    return;
}
int dad_pid()
{
return(dad);
}
