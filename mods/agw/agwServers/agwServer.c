/*!
  \mainpage agwServer - Server for MODS AGW Stage.
 
  \author Staff, OSU Astronomy Dept. (rgonzale@astronomy.ohio-state.edu)
  \date 2006 March 16
 
  \section Usage
 
  Purpose:  Starts GCS get/set specific threads and then execute the following:
            (1) thread (x) waits for connection from a GCS get/set client
            (2) thread (x) sends out command request to islmgr
            (3) islmgr gets/sets information/commmand
            (4) thread (x) sends AGW Stage requested information or ERROR
	        to client.
            (5) GCS client closes the connection.
            (6) go back to step (1)

  Usage: 
  agwServer &  - Default Port = 10433
  agwServer [PortNumber] &

  \section Introduction
 
  agwServer is a server used by the MODS AGW stage and/or TCS/IIF 
  Actual.  It accepts commands from a Client and moves mechanisms. 
  This is for GCS interface library.

  \section Commands

  Accepts commands from a GCS client and returns information or errors.

  \section Notes

  This application uses the RTE library (link).

  \section Mods Modification History
  <pre>
  2006 February 6 - New application
  2006 March 16 - rework for GCS requirements doc and new MODS
  ICD API architecture [rdg]
  </pre>

*/
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
using namespace std;

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/timeb.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>

// Specific headers in ../include/ for the AGW code

#include "../include/client.h"      // Custom client application header
#include "../include/mods_timefuncts.h"      // Custom client application header

/* 
 * ISL general includes
 */
#include "isisclient.h"  // ISIS common client library header
#include "instrutils.h"  // Instrument common
#include "isl_funcs.h"   // Instrument types
#include "isl_types.h"   // Instrument types
#include "params.h"      // Common parameters and defines
#include "islcommon.h"   // Common parameters and defines
#include "isl_shmaddr.h" // Shared memory attachment.

struct islcommon *agws;

#define MAX_CLIENT_PER_THREAD 12
#define MAX_THREAD 12
#define PORT 10433
#define PROMPT "AGW>"

char buf[ISIS_MSGSIZE]; // command/message buffer

//#define DEBUG 1

// Global ISIS Client data structure for this app
                                                                              
isisclient_t appClient;

// Services

struct sockaddr_in srv, cli;
                                                                               
// Global AGW mechanism data structure for this app

agwpar_t appAGW;

void sig_chld(int);

int listenfd;
int clifd;
fd_set read_fd;
int charReceived;
int readFromListen_port;
char buffer[ISIS_MSGSIZE];

extern int mmcLOGGER(char [], char []);

typedef struct {
  pthread_t tid;
  int client_count;
  int clients[MAX_CLIENT_PER_THREAD];
} Thread;

typedef struct {
  char errors[512];
  int err_cnt;
} agw_errors;

agw_errors errmsgs;

extern char* makeUpper(char []);
void *thread_init_func(void *);
pthread_cond_t new_connection_cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t new_connection_mutex = PTHREAD_MUTEX_INITIALIZER;


Thread threads[MAX_THREAD];

void nonblock(int sockfd)
{
  int opts;
  opts = fcntl(sockfd, F_GETFL);
  if(opts < 0)
    {
      perror("fcntl(F_GETFL)\n");
      exit(1);
    }
  opts = (opts | O_NONBLOCK);
  if(fcntl(sockfd, F_SETFL, opts) < 0)
    {
      perror("fcntl(F_SETFL)\n");
      exit(1);
    }
}

void *thread_init_func(void *arg)
{
  // int tid = (int) arg;
  int tid = *((int*)arg);

  int readsocks;
  int fromlen;
  int unit;
  int i, ierr;
  //  char buffer[ISIS_MSGSIZE];
  char getset_reply[ISIS_MSGSIZE];
  char c;
  int n;
  char timestring[256];

  memset((int *) &threads[tid].clients, 0,
	 sizeof(threads[tid].clients));
  memset(buffer, 0, sizeof(buffer));

  while(1) {

    MilliSleep(10);
    
    for(i = 0; i < MAX_CLIENT_PER_THREAD; i++) {
      if(threads[tid].clients[i] != 0) {
	if(readFromListen_port==1) { // read from port
	  n = recv(threads[tid].clients[i], buffer, ISIS_MSGSIZE, 0);
	} else n=0;

	if(n == 0) {

	  readFromListen_port=0; // reset listenfd listen
	  threads[tid].clients[i] = 0;
	  threads[tid].client_count--;
	  memset(buffer, 0, sizeof(buffer));

	} else if (n < 0) {

	  
	  readFromListen_port=0; // reset listenfd listen
	  memset(buffer, 0, sizeof(buffer));

	  if(errno == EAGAIN) {

#ifdef DEBUG
	    printf("errno: EAGAIN on %d\n",threads[tid].clients[i]);
#endif
	  } else {
	    
	    threads[tid].clients[i] = 0;
	    threads[tid].client_count--;

	  }
	} else {

	  if (strstr(buffer,"geterrormsg")) {
	    readFromListen_port=0;  // reset client.FD listen
	    send(threads[tid].clients[i], errmsgs.errors,
		 strlen(errmsgs.errors), 0);

	    memset(buffer, 0, sizeof(buffer));
	    if(threads[tid].clients[i]>0) close(threads[tid].clients[i]);


	  } else {

	    if(readFromListen_port==1) {  // Got something on the line

	      mmcLOGGER((char*)DEFAULT_AGWLOG,buffer);
	      readFromListen_port=0;

	      GCSCommand(buffer,getset_reply);

	      send(threads[tid].clients[i], getset_reply,
		   strlen(getset_reply), 0);

	      mmcLOGGER((char*)DEFAULT_AGWLOG,getset_reply);

	      memset(buffer, 0, sizeof(buffer));
	      if(threads[tid].clients[i]>0) close(threads[tid].clients[i]);

	    }
	    /* 
	       This is a way of terminating the agwServer via a client.
	       o close all MicroLynx Controller(MLC) communications(COMM)
	       o exit the agwServer
	    */
	    if(appClient.KeepGoing==0) { // Close all AGW comport connections
	      for(unit=0;unit<MAX_ML-1;unit++) {
		if(shm_addr->MODS.host[unit]==0);
		if(shm_addr->MODS.host[unit]==1 &&
		   !strncasecmp(shm_addr->MODS.who[unit],"agw",3))
		  CloseTTYPort(&shm_addr->MODS.commport[unit]);
	      }
	      fprintf(stderr,"MODS AGW service has been halted");
	      memset(buffer, 0, sizeof(buffer));
	      exit(0); // exit agwServer
	    }

	    if(appClient.KeepGoing==2) { // Close all AGW comport connections
	      for(unit=0;unit<MAX_ML-1;unit++) {
		if(shm_addr->MODS.host[unit]==0);
		if(shm_addr->MODS.host[unit]==1 &&
		   !strncasecmp(shm_addr->MODS.who[unit],"agw",3))
		  CloseTTYPort(&shm_addr->MODS.commport[unit]);
	      }
	      fprintf(stderr,"MODS AGW communication service to mechanisms closed");
	      memset(buffer, 0, sizeof(buffer));
	      appClient.KeepGoing=1;
	    }

	    if(appClient.KeepGoing==3) { // Reset all AGW comport connections
	      for(unit=0;unit<MAX_ML-1;unit++) {
		if(shm_addr->MODS.host[unit]==0);
		else if(shm_addr->MODS.host[unit]==1 &&
			!strncasecmp(shm_addr->MODS.who[unit],"agw",3))
		  CloseTTYPort(&shm_addr->MODS.commport[unit]);
	      }

	      for(unit=0;unit<MAX_ML-1;unit++) {
		if(shm_addr->MODS.host[unit]==0);
		else if(shm_addr->MODS.host[unit]==1 &&
			!strncasecmp(shm_addr->MODS.who[unit],"agw",3))
		  OpenTTYPort(&shm_addr->MODS.commport[unit]);
	      }

	      fprintf(stderr,"MODS AGW communication service to mechanisms established");
	      memset(buffer, 0, sizeof(buffer));
	      appClient.KeepGoing=1;
	    }

	    if(appClient.KeepGoing==4) { // Reset all AGW comport connections
	      for(unit=0;unit<MAX_ML-1;unit++) {
		if(shm_addr->MODS.host[unit]==0);
		else if(shm_addr->MODS.host[unit]==1 &&
			!strncasecmp(shm_addr->MODS.who[unit],"agw",3))
		  CloseTTYPort(&shm_addr->MODS.commport[unit]);
	      }

	      LoadConfig(&appAGW,&appClient,(char*)APP_CFGFILE); // reconfig

	      for(unit=0;unit<MAX_ML-1;unit++) {
		if(shm_addr->MODS.host[unit]==0);
		else if(shm_addr->MODS.host[unit]==1 &&
			!strncasecmp(shm_addr->MODS.who[unit],"agw",3))
		  OpenTTYPort(&shm_addr->MODS.commport[unit]);
	      }

	      fprintf(stderr,"MODS AGW has been reconfigured and communication to mechanisms have been reestablished");
	      memset(buffer, 0, sizeof(buffer));
	      appClient.KeepGoing=1;
	    }

	    if (strstr(getset_reply,"ERROR:")) { // check for error
	      memset(errmsgs.errors,0,sizeof(errmsgs.errors));
	      getCurrentTime(timestring,(char*)"%Y %j %H %M %S");
	      sprintf(errmsgs.errors,"%s%s",timestring,
		      &getset_reply[9]);
	      strcat(errmsgs.errors,"\n");
	      errmsgs.errors[strlen(errmsgs.errors)]='\0';
	    }
 
	  }
	  memset(getset_reply, 0, sizeof(getset_reply));
	  memset(buffer, 0, sizeof(buffer));
	}
      }
    }
  }
}

int choose_thread()
{
  int i=MAX_THREAD-1;
  int min = 0;
  while(i > -1) {
    if(threads[i].client_count < threads[i-1].client_count) {
      min = i;
      break;
    }
    i--;
  }
  return min;
}

int 
main(int argc, char *argv[])
{
  char c;
  struct sockaddr_in srv, cli;
  int tid;
  int i,unit;
  int choosen;
  int n_ready;
  char temp[PAGE_SIZE];
  static int sel_wid; 

  setup_ids();
  sel_wid = getdtablesize();

  signal(SIGCHLD,sig_chld);  // Avoid "zombie" process generation.
  signal(SIGINT,HandleInt);  // Ctrl+C sends a move abort to controller
  signal(SIGPIPE,SIG_IGN);    // Ignore broken pipes

  // Allocate the strings for the AG Camera Filter ID Table

  errmsgs.err_cnt=0;
  errmsgs.errors[0]='\0';

  appAGW.filterID = (char **)malloc(AGW_NFILTERS*sizeof(char *));
  for (i=0; i<AGW_NFILTERS; i++) {
    (appAGW.filterID)[i] = (char *)calloc(AGW_FILTSIZE,sizeof(char));
  }

  if(LoadConfig(&appAGW,&appClient,(char*)APP_CFGFILE)!=0)
    printf("Could not load the runtime config file...agwServer aborting\n");

  if( (listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("sockfd\n");
    exit(1);
  }

  for (i=0; i<AGW_NFILTERS; i++) {
    sprintf(shm_addr->MODS.agwfilters[i],"%s",(appAGW.filterID)[i]);
  }

  /* 
   * IPs are as follows appAGW.YIP, appAGW.XIP, appAGW.FIP, appAGW.FWIP
   * These are set in the appAGW.ini file.
   */
  ostringstream os;
  os << "\n";
  os << "MODS agwServer started\n";

  for(unit=0;unit<MAX_ML-1;unit++) {
    if(!strncasecmp(shm_addr->MODS.who[unit],"agw",3)) {
      if(unit!=MAX_ML-1) {
	CloseTTYPort(&shm_addr->MODS.commport[unit]); // Close before opening
	if(OpenTTYPort(&shm_addr->MODS.commport[unit]) < 0) {
	  os << "ERROR: MODS MLC" << unit+1 << " , " 
	     << shm_addr->MODS.who[unit]
	     << " , " << shm_addr->MODS.commport[unit].Port
	     << "\nCannot Open communications: Reason: " 
	     << strerror(errno)
	     << "\nCheck mechanism.ini config file.\n";
	} else {
	  os << "MODS MLC" << unit+1 << " , "
	     << shm_addr->MODS.who[unit]
	     << " , " << shm_addr->MODS.commport[unit].Port
	     << " OPENED\n";
	}
	if(!strncasecmp(shm_addr->MODS.who[unit],"agwy",4)) {
	  appAGW.YIP=unit;
	} else if(!strncasecmp(shm_addr->MODS.who[unit],"agwx",4)) {
	  appAGW.XIP=unit;
	} else if(!strncasecmp(shm_addr->MODS.who[unit],"agwfoc",6)) {
	  appAGW.FIP=unit;
	} else if(!strncasecmp(shm_addr->MODS.who[unit],"agwfilt",7)) {
	  appAGW.FWIP=unit;
	}
      }
    } // else os << cin << "MLC[" << unit+1 << "] Inactive\n";
  }

  string strtemp=os.str();
  sprintf(temp,"%s",&strtemp[0]);
  mmcLOGGER((char*)DEFAULT_AGWLOG,temp);
  chmod((char*)DEFAULT_AGWLOG,0666);

  shm_addr->MODS.modsPorts[1]=PORT;

  bzero(&srv, sizeof(srv));
  srv.sin_family = AF_INET;
  srv.sin_addr.s_addr = INADDR_ANY;
  srv.sin_port = htons(PORT);
  
  if( bind(listenfd, (struct sockaddr *) &srv, sizeof(srv)) < 0) {
    sprintf(buf,"agwServer: Socket binding failed %s",strerror(errno));
    mmcLOGGER(shm_addr->MODS.LLOG,buf);
    mmcLOGGER(shm_addr->MODS.LLOG,(char*)"agwServer: Socket binding failed");
    exit(1);
  }

  listen(listenfd, 1024);

  // Create threads

  for(i = 0; i < MAX_THREAD; i++) {
    // pthread_create(&threads[i].tid, NULL, &thread_init_func, (void *)i);
    pthread_create(&threads[i].tid, NULL, &thread_init_func, (void *)(unsigned long long)(i));
    threads[i].client_count = 0;
  }

  appClient.KeepGoing = 1;

  while(appClient.KeepGoing) {
    
    // clear the table of active file descriptors
    FD_ZERO(&read_fd);

    // we always listen to the listen IP and Socket
    if (listenfd > 0) FD_SET(listenfd, &read_fd);

    n_ready = 0; // Do the select() call and wait for activity from anyone
    n_ready = select(sel_wid, &read_fd, NULL, NULL, NULL);

    if (n_ready == 0) { // would be a timeout if enabled, do nothing...
      cout << "select() interrupted by Ctrl+C..." << endl;
      continue;

    } else if (n_ready < 0) {
      if (errno == EINTR) { // caught Ctrl+C, hopefully sigint handler caught it
	//if (appClient.isVerbose)
	cout << "select() interrupted by Ctrl+C...\n";

      } else { // something else bad happened, let us know
	cout << "Warning: select() failed - " << strerror(errno) 
	     << " - pressing on anyway...\n";
      }
      continue;

    } else { // somebody wants something, figure out who...

      if (FD_ISSET(listenfd, &read_fd)) {  // Client socket input
	memset(buffer,0,sizeof(buffer));
	readFromListen_port=1;
      }

      pthread_mutex_lock(&new_connection_mutex);

      choosen = choose_thread();

      for(i = 0; i < MAX_CLIENT_PER_THREAD; i++) {
	if(threads[choosen].clients[i] == 0) {
	  if(readFromListen_port==1) { // for listen protocol
	    clifd = accept(listenfd, NULL, NULL);
	    //nonblock(clifd);
	    threads[choosen].clients[i] = clifd;
	    threads[choosen].client_count++;
	    break;
	  }
	}
      }
    }
    pthread_mutex_unlock(&new_connection_mutex);
  }
  return 0;
}

// Here for standalone input system to stop motion.

void
HandleInt(int signalValue) 
{
  int i;
  int unit;
  char dummy[79];

  if (appClient.Debug)
    cout << "Caught Ctrl+C (Signal " << signalValue << ")..." << endl;
   
  for(unit=0;unit<MAX_ML-1;unit++) {
    if(shm_addr->MODS.host[unit]==1 &&
       !strncasecmp(shm_addr->MODS.who[unit],"agw",3)) {
      CloseTTYPort(&shm_addr->MODS.commport[unit]);
      sprintf(dummy,"agwServer %s connection CLOSED",
	      shm_addr->MODS.who[unit]);
      mmcLOGGER(shm_addr->MODS.LLOG,dummy);
      cout << "agwServer " << shm_addr->MODS.who[unit] << " CLOSED" << endl;
    }
    MilliSleep(100);
  
}  
  mmcLOGGER(shm_addr->MODS.LLOG,(char*)"agwServer has been halted by a killall");
  cout << "agwServer has been halted by a 'killall -s SIGINT agwServer'" << endl;

  memset(buf,0,sizeof(buf));
  for(i = 0; i < MAX_THREAD; i++) {
    threads[i].clients[i] = 0;
    threads[i].client_count = 0;
  }
  close(clifd);

  appClient.KeepGoing = 0;

  exit(0); // exit agwService

}

// The use of this function avoids the generation of "zombie" processes.

void sig_chld( int signo )
{
  pid_t pid;
  int stat;

  while ( ( pid = waitpid( -1, &stat, WNOHANG ) ) > 0 ) {
    //    printf( "Child %d terminated.\n", pid );
  }
}
