/*!
  \mainpage mmcServer - MODS Mechanism Control server
 
  \authors Ray Gonzalez (rgonzale@astronomy.ohio-state.edu, retired), R. Pogge (pogge.1@osu.edu)
  \date 2006 March 16
 
  \section Usage
 
  Purpose: Starts MODS get/set specific threads and then execute the following:
            (1) thread (x) waits for connection from a MODS mechanism client
            (2) thread (x) sends out command request to instrutils
            (3) instrutils gets/sets information/commmand
            (4) thread (x) sends MicroLynx requested information or ERROR to client.
            (5) MODS mechanism client closes the connection.
            (6) go back to step (1)

  Usage: 
  mmcServer &  - Default Port = 10435
  mmcServer [PortNumber] &

  \section Introduction
 
  mmcServer is a server used by the MODS mechanism clients It accepts
  commands from client(s) and moves mechanisms.  This is for the MODS
  mechanism interface library.

  \section Commands

  Accepts commands from a MODS mechanism client and returns
  information or errors.

  \section Notes

  This application uses the RTE library (link).

  \section Mods Modification History
  <pre>
  2006 February 6 - New application
  2006 March 16 - rework for mechanism requirements doc and new MODS
  ICD API architecture [rdg]
  2009 Sept 17 - rework of the listen(), and accept() protocols. accept(fd) 
		re-transmit didn't seem to work with threads [rdg]
  </pre>

  <pre>
  2025 Aug 8 - Port to AlmaLinux 9.x [rwp/osu]
  2026 Feb 18 - need to add mutex locks to avoid resource conflicts, esp
                when logging [rwp/osu]
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
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>

#include "mmc_client.h"  // Custom client application header

#include "instrutils.h"  // Instrument common
#include "isl_funcs.h"   // Instrument types
#include "isl_types.h"   // Instrument types
#include "params.h"      // Common parameters and defines
#include "islcommon.h"   // Common parameters and defines
#include "isl_shmaddr.h" // Shared memory attachment.

struct islcommon *mechs;

extern int wagoSetGet(int,char [],int,int,short int*,int);
extern int getWagoID(char [],char []); // Get WAGO ID
extern int isisStatusMsg(char []);
// extern int MSOpenPort(char *);

#define MAX_CLIENT_PER_THREAD 64
#define MAX_THREAD 100
#define PORT 10435
#define PROMPT "M1%"

char buf[ISIS_MSGSIZE]; // command/message buffer
char tempbuf[80]; // temporary buffer for saving last command before failiar

isisclient_t appClient; // ISIS AGW Client data structure
isisclient_t client; // ISIS Client data structure

struct sockaddr_in srv, cli;
                                                                               
void sig_chld(int);

char timestr[80];
int cmdCounter;
int listenfd;
int clifd;
fd_set read_fd;
int charReceived;
int readFromListen_port;
char getset_reply[ISIS_MSGSIZE]; // This is for logging purposes
int mmcLOGGER(char [],char []);
int allPower;

typedef struct {
  pthread_t tid;
  int client_count;
  int clients[MAX_CLIENT_PER_THREAD];
} Thread;

typedef struct {
  char errors[25][128];
  char err_cnt;
} agw_errors;

agw_errors errmsgs;

// Thread declarations

void *thread_init_func(void *);
pthread_cond_t new_connection_cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t new_connection_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock; // generic mutex lock

Thread threads[MAX_THREAD];

//------------------------------------------------

// nonblock() function (currently unused)

void
nonblock(int sockfd)
{
  int opts;
  opts = fcntl(sockfd, F_GETFL);
  if (opts < 0) {
    mmcLOGGER(shm_addr->MODS.LLOG,(char*)"mmcServer: fcntl(F_GETFL) failed");
    perror("fcntl(F_GETFL)\n");
    exit(1);
  }
  opts = (opts | O_NONBLOCK);
  if (fcntl(sockfd, F_SETFL, opts) < 0) {
    mmcLOGGER(shm_addr->MODS.LLOG,(char*)"mmcServer: fcntl(F_SETFL) failed");
    perror("fcntl(F_SETFL)\n");
    exit(1);
  }
}

extern char *getDateTime(void);

//------------------------------------------------
//
// Thread function: thread_init_func()
//

void
*thread_init_func(void *arg)
{
  int tid = (long long) arg;

  int readsocks;
  int isis_read;
  int fromlen;
  int i;
  int ierr;
  int len;
  char srcID[ISIS_NODESIZE];    // ISIS message sending node ID
  char cmdName[BIG_STR_SIZE];
  //char timestr[80];
  char dummy[BIG_STR_SIZE];
  char msg[ISIS_MSGSIZE];
  char destID[ISIS_NODESIZE];   // ISIS message destination node ID
  char c;
  int n;
  int m;
  int unit;
  int ieb1_state=0;  // ON=1, OFF=0 for IEB1
  int ieb2_state=0;  // ON=1, OFF=0 for IEB2
  time_t ieb_timer;
  int time_30sec=0;
  int getMechanismID(char[],char[]);
  int rawCommand(int,char[],char[]);

  time(&ieb_timer); // IEB(s) timer

  memset((int *) &threads[tid].clients,0,sizeof(threads[tid].clients));
  memset(buf,0,sizeof(buf));
  memset(tempbuf,0,sizeof(tempbuf));
  shm_addr->MODS.mmcServerCounter=0;

  while(1) {

    MilliSleep(1);

    for (i = 0; i < MAX_CLIENT_PER_THREAD; i++) {

      if (threads[tid].clients[i] != 0) {

	if (readFromListen_port==1) { // Bypass the ISIS server
	  memset(buf,0,sizeof(buf));
	  memset(tempbuf,0,sizeof(tempbuf));
	  n = recv(threads[tid].clients[i], buf, ISIS_MSGSIZE, 0);

	}
	else if(readFromListen_port==2) { // Bypass the Listen server
	  n=charReceived; // Check for ISIS reads
	}
	else
	  n=0;

	if (n == 0 ) {
	  threads[tid].clients[i] = 0;
	  threads[tid].client_count--;

	}
	else if (n < 0) {
	  if (errno == EAGAIN) {
	    threads[tid].clients[i] = 0;
	    threads[tid].client_count--;
	  }
	  else {
	    threads[tid].clients[i] = 0;
	    threads[tid].client_count--;
	  }

	}
	else {
	  if (strstr(buf,"geterrormsg")) {
	    readFromListen_port=0;  // reset client.FD listen
	    for (ierr=0; ierr<errmsgs.err_cnt; ierr++) {
	      send(threads[tid].clients[i], 
		   errmsgs.errors[ierr], 
                   strlen(errmsgs.errors[ierr]), 0); 
	    }
	    memset(buf,0,sizeof(buf));
	    if (threads[tid].clients[i]>0)
	      close(threads[tid].clients[i]);
	    
	  }
	  else {

	    if (readFromListen_port==1) {  // This bypasses ISIS

	      readFromListen_port=0;  // reset client.FD listen

	      // Save time and who sent command in Shared Memory
	      
	      ierr=shm_addr->MODS.mmcServerCounter;

	      mmcLOGGER(shm_addr->MODS.LLOG,buf);

	      strncpy(tempbuf,buf,79);
	      tempbuf[79]='\0';
	      sprintf(shm_addr->MODS.ieb_msg[ierr],"[%d] %s,t%d,fd%d,%d %s",ierr,getDateTime(),tid, threads[tid].clients[i],clifd,tempbuf);

	      KeyCommand(buf,getset_reply);

	      send(threads[tid].clients[i], getset_reply, strlen(getset_reply), 0);
	      mmcLOGGER(shm_addr->MODS.LLOG,getset_reply);
	      
	      if (threads[tid].clients[i]>0)
		close(threads[tid].clients[i]);

	      memset(getset_reply,0,sizeof(getset_reply));
	      memset(buf,0,sizeof(buf));
	      memset(tempbuf,0,sizeof(tempbuf));

	    }

	    // This goes out to the isis server
	    
	    if (readFromListen_port==2) {

	      readFromListen_port=0;  // reset client.FD listen

	      // Save time and who sent command in Shared Memory
	      
	      ierr=shm_addr->MODS.mmcServerCounter;

	      mmcLOGGER(shm_addr->MODS.LLOG,buf);

	      strncpy(tempbuf,buf,79);
	      tempbuf[79]='\0';
	      sprintf(shm_addr->MODS.ieb_msg[ierr],"[%d] %s,t%d,fd%d %s",ierr,getDateTime(),tid, threads[tid].clients[i],tempbuf);

	      SocketCommand(buf);
	      memset(buf,0,sizeof(buf));
	      memset(tempbuf,0,sizeof(tempbuf));
	    }

	    //----------------------------------------------------------------
	    //
	    // This is a way of terminating the mmcServers via a client
	    //   * close all MicroLynx Controller(MLC) communications(COMM)
	    //   * exit the mmcServer
	    //
	    
	    // Close all Host to MicroLynx communications and stop the
	    // mmcServers except the AGW

	    if (client.KeepGoing==0) { // Close all MLC COMM. except AGW
	      for (unit=0;unit<MAX_ML-1;unit++) {
		if (shm_addr->MODS.host[unit]==0);
		else if (shm_addr->MODS.host[unit]==1 &&
			!strncasecmp(shm_addr->MODS.who[unit],"agw",3));
		else
		  if (unit!=MAX_ML-1)
		    CloseTTYPort(&shm_addr->MODS.commport[unit]);
	      }
	      isisStatusMsg((char*)"mmcServer communication closed and mmcServer halted");
	      mmcLOGGER(shm_addr->MODS.LLOG,(char*)"mmcServer communication closed and mmcServer halted");
	      memset(buf,0,sizeof(buf));
	      exit(0); // exit mmcServer
	    }

	    // Close all Host to MicroLynx communications except the AGW
	    
	    if (client.KeepGoing==2) {
	      for (unit=0;unit<MAX_ML-1;unit++) {
		if (shm_addr->MODS.host[unit]==0);
		else if (shm_addr->MODS.host[unit]==1 &&
			!strncasecmp(shm_addr->MODS.who[unit],"agw",3));
		else
		  CloseTTYPort(&shm_addr->MODS.commport[unit]);
	      }
	      isisStatusMsg((char*)"mmcServer communication service to mechanisms closed");
	      mmcLOGGER(shm_addr->MODS.LLOG,(char*)"agwService communication service to mechanisms closed");
	      memset(buf,0,sizeof(buf));
	      appClient.KeepGoing=2; // close AGW comm.

	      client.KeepGoing=1;
	    }

	    // Open mmcServers restoring communications from Host to MicroLynx, except the AGW 

	    if (client.KeepGoing==3) {

	      // Make MicroLynx inactive or active depending on config file
	      
	      for (unit=0;unit<MAX_ML-1;unit++) {
		if (strncasecmp(shm_addr->MODS.commport[unit].Port,"NONE",4)) {
		  shm_addr->MODS.host[unit]=1;
		  shm_addr->MODS.busy[unit]=0; // initialize
		}
	      }

	      for (unit=0;unit<MAX_ML-1;unit++) {
		if (shm_addr->MODS.host[unit]==0);
		else if (shm_addr->MODS.host[unit]==1 &&
			!strncasecmp(shm_addr->MODS.who[unit],"agw",3) ||
			!strncasecmp(shm_addr->MODS.who[unit],"rimcs",5) ||
			!strncasecmp(shm_addr->MODS.who[unit],"bimcs",5));
		else
		  if (unit!=MAX_ML-1) {
		    OpenTTYPort(&shm_addr->MODS.commport[unit]);
		  }
		sprintf(dummy,"%s mechanism opened",shm_addr->MODS.who[unit]);
		isisStatusMsg(dummy);
	      }
	      isisStatusMsg((char*)"mmcServer communication service to mechanisms established");
	      mmcLOGGER(shm_addr->MODS.LLOG,(char*)"mmcServer communication service to mechanisms established");
	      memset(buf,0,sizeof(buf));
	      appClient.KeepGoing=3; // open AGW comm.

	      client.KeepGoing=1;

	    }

	    // Reconfigure mmcServers and restore communications except the AGW 

	    if (client.KeepGoing==4) {

	      // Make MicroLynx inactive or active depending on config file
	      
	      for (unit=0;unit<MAX_ML-1;unit++) {
		if (strncasecmp(shm_addr->MODS.commport[unit].Port,"NONE",4)) {
		  shm_addr->MODS.host[unit]=1;
		  shm_addr->MODS.busy[unit]=0; // initialize
		}
	      }

	      for (unit=0;unit<MAX_ML-1;unit++) {
		if (shm_addr->MODS.host[unit]==0);
		else if (shm_addr->MODS.host[unit]==1 &&
			!strncasecmp(shm_addr->MODS.who[unit],"agw",3));
		else
		  if (unit!=MAX_ML-1) {
		    CloseTTYPort(&shm_addr->MODS.commport[unit]);
		  }
	      }
	      
	      LoadConfig(DEFAULT_RCFILE);
	      
	      for (unit=0;unit<MAX_ML-1;unit++) {
		if (shm_addr->MODS.host[unit]==0);
		else if (shm_addr->MODS.host[unit]==1 &&
			!strncasecmp(shm_addr->MODS.who[unit],"rimcs",5) ||
			!strncasecmp(shm_addr->MODS.who[unit],"bimcs",5) ||
			!strncasecmp(shm_addr->MODS.who[unit],"agw",3));
		else
		  if (unit!=MAX_ML-1) {
		    CloseTTYPort(&shm_addr->MODS.commport[unit]);
		    OpenTTYPort(&shm_addr->MODS.commport[unit]);
		  }
	      }

	      isisStatusMsg((char*)"MODS instance has been reconfigured and communication to mechanisms have been reestablished");

	      mmcLOGGER(shm_addr->MODS.LLOG,(char*)"MODS instance has been reconfigured and communication to mechanisms have been reestablished");
	      memset(buf,0,sizeof(buf));
	      appClient.KeepGoing=4; // Reconfigure AGW and open comm.

	      client.KeepGoing=1;
	    }
	  }
	
	  if (strstr(getset_reply,"ERROR")) {
	    
	    if (errmsgs.err_cnt > 24) errmsgs.err_cnt=0;
	    else errmsgs.err_cnt++;
	    
	    strcpy (errmsgs.errors[errmsgs.err_cnt],getset_reply);
	    strcat (errmsgs.errors[errmsgs.err_cnt],"\n");
	    errmsgs.errors[errmsgs.err_cnt]\
	      [strlen(errmsgs.errors[errmsgs.err_cnt])]=\
	      '\0';
	  }

	  readFromListen_port=0; // reset listenfd listen
	  memset(getset_reply, 0, sizeof(getset_reply));
	  memset(buf,0,sizeof(buf));
	}
      }
    }
  }
}

// Thread chooser

int
choose_thread()
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

//
// Main Program
//

int
main(int argc, char *argv[])
{
  char c;
  struct sockaddr_in srv, cli;
  int tid;
  int i;
  int n;
  int unit;
  int utilID;
  int llbID;
  int ieb1ID;
  int ieb2ID;
  int iebRedOnOff;
  int iebBlueOnOff;
  int choosen;
  int n_ready;
  short onoff[1];
  char temp[PAGE_SIZE];
  char HALTKey[10];
  static int sel_wid; 

  // Basic initializations

  setup_ids();
  sel_wid = getdtablesize();
  
  signal(SIGCHLD,sig_chld); // Avoid "zombie" process generation.
  signal(SIGINT,HandleInt);    // Ctrl+C sends a move abort to controller
  signal(SIGPIPE,SIG_IGN);     // ignore broken pipes

  // Allocate the strings for the A/G Camera Filter ID Table

  errmsgs.err_cnt=0;

  cmdCounter=0; // initialize command counter.
  shm_addr->MODS.mmcServerCounter=0;
    
  for (unit=0;unit<MAX_ML-1;unit++) { // Make MicroLynx HOST inactive or active!
    sprintf(shm_addr->MODS.commport[unit].Port,"NONE");
    shm_addr->MODS.host[unit]=0;
    shm_addr->MODS.busy[unit]=0; // initialize
    sprintf(shm_addr->MODS.who[unit],"MLC%d",unit+1); // initialize
  }

  if (argc==2) {
    n = LoadConfig(argv[1]);
    fprintf(stderr,"Loading config file: %s\n",argv[1]);
  }
  else n = LoadConfig(DEFAULT_RCFILE);

  memset(shm_addr->MODS.LLOG,0,79); // Setup logging file
  sprintf(shm_addr->MODS.LLOG,client.logFile);
  chmod(shm_addr->MODS.LLOG,0666);  // Give permissions to default logging file

  mmcLOGGER(shm_addr->MODS.LLOG,(char*)"######");
  mmcLOGGER(shm_addr->MODS.LLOG,(char*)"Mechanism controller services have been (re)started");
  mmcLOGGER(shm_addr->MODS.LLOG,(char*)"(mmcServer and agwServer)"); // now lists services we use at LBTO
  mmcLOGGER(shm_addr->MODS.LLOG,(char*)"######");

  for (unit=0;unit<MAX_ML-1;unit++) { // Make MicroLynx HOST inactive or active!
    if (!strncasecmp(shm_addr->MODS.commport[unit].Port,"NONE",4)) {
      shm_addr->MODS.host[unit]=0;
      shm_addr->MODS.busy[unit]=0; // initialize
      sprintf(shm_addr->MODS.who[unit],"MLC%d",unit+1); // initialize
    }
    else {
      shm_addr->MODS.host[unit]=1;
      shm_addr->MODS.busy[unit]=0; // initialize
    }
  }

  // In keeping with various other instruments provide by OSU we also
  // need to 'listen()'

  readFromListen_port=0;
  charReceived=0;

  for(i = 0; i < MAX_ML-1; i++) {
    sprintf(shm_addr->MODS.ieb_msg[i],"Empty");
  }
  system("/usr/local/bin/mlcRecover agwfilt");
  system("/usr/local/bin/mlcRecover rfilter");
  system("/usr/local/bin/mlcRecover bfilter");
  system("/usr/local/bin/mlcRecover rgrating");
  system("/usr/local/bin/mlcRecover bgrating");
  system("/usr/local/bin/mlcRecover dichroic");
  system("/usr/local/bin/mlcRecover slitmask");

  if (client.useISIS) {
    if (InitISISServer(&client)<0) {
      cout << "ISIS server connection initialization failed" << endl;
    }
  }

  // Open the client network socket port for interprocess communications 

  if (OpenClientSocket(&client)<0) {
    cout << "Client socket initialization failed" << endl;
  }
  
  if (client.useISIS) {
    cout << "Started mmcServer as\nISIS client node " << client.ID 
	 << " on " << client.Host << " port " << client.Port << " Use:" 
	 << client.useISIS << endl;
  }
  else {
    cout << "Started mmcServer as standalone " << client.ID 
	 << " on "  << client.Host << " port " << client.Port  << " Use:" 
	 << client.useISIS << endl;
  }

  mmcLOGGER(shm_addr->MODS.LLOG,(char*)"started ISIS client instance");
 
  // Broadcast a PING to the ISIS server, if enabled.  If it fails,
  // we'll have to do the ping by hand after the comm loop starts.
  
  if (client.useISIS) {
    memset(buf,0,sizeof(buf));
    sprintf(buf,"%s>AL ping\r",client.ID); 
    if (SendToISISServer(&client,buf)<0) 
      cout << "Failed to PING the ISIS server..." << strerror(errno) << endl;
    if (client.isVerbose)
      cout << "OUT: " << buf << endl;
  }

  memset(buf,0,sizeof(buf));
  memset(temp,0,sizeof(temp));

  utilID=getWagoID((char*)"util",temp); // Get Utility Box ID
  if (utilID==-1) {
    mmcLOGGER(shm_addr->MODS.LLOG,(char*)"mmcServer: Utility Box (UTIL) Identification not found");
  }

  allPower = wagoSetGet(0,shm_addr->MODS.WAGOIP[utilID],1,513,onoff,1);

  if (allPower<0) {
    mmcLOGGER(shm_addr->MODS.LLOG,(char*)"UTIL UTIL=OFF Utility Box OFF");
    isisStatusMsg((char*)"UTIL UTIL=OFF Utility Box OFF");

  }
  else {  
    mmcLOGGER(shm_addr->MODS.LLOG,(char*)"UTIL UTIL=ON Utility Box ON");
    isisStatusMsg((char*)"UTIL UTIL=ON Utility Box is ON");

  }

  llbID=getWagoID((char*)"llb",temp); // Get LLB box ID
  if (llbID==-1) {
    mmcLOGGER(shm_addr->MODS.LLOG,(char*)"mmcServer: Lamp/Laser Box (LLB) Identification not found");
  }

  allPower = wagoSetGet(0,shm_addr->MODS.WAGOIP[llbID],1,515,onoff,1);

  if (allPower<0) {
    mmcLOGGER(shm_addr->MODS.LLOG,(char*)"LLB LLB=OFF Lamp Laser Box OFF");
    isisStatusMsg((char*)"LLB LLB=OFF Lamp Laser Box OFF"); 
    
  }
  else { 
    mmcLOGGER(shm_addr->MODS.LLOG,(char*)"LLB LLB=ON Lamp Laser Box is ON");
    isisStatusMsg((char*)"LLB LLB=ON Lamp Laser Box is ON"); 
      
  }

  // Blue Instrument Electronics Box (ieb2)
  
  ieb1ID=getWagoID((char*)"ieb2",temp); // Get IEB ID

  if (ieb1ID==-1) {
    mmcLOGGER(shm_addr->MODS.LLOG,(char*)"mmcServer: Blue Instrument Electronics Box (IEB_B) Identification not found");
    return -1;
  }

  allPower = wagoSetGet(0,shm_addr->MODS.WAGOIP[ieb1ID],1,514,onoff,1);
  if (allPower<0) {
    mmcLOGGER(shm_addr->MODS.LLOG,(char*)"IEB IEB_B=OFF Blue IEB is OFF");
    isisStatusMsg((char*)"IEB IEB_B=OFF Blue IEB Box is OFF");
    iebBlueOnOff=0;
  }
  else { 
    mmcLOGGER(shm_addr->MODS.LLOG,(char*)"IEB IEB_B=ON Blue IEB is ON");
    isisStatusMsg((char*)"IEB IEB_B=ON Blue IEB Box is ON");
    iebBlueOnOff=2; // BLUE IEB mechanisms are in IEBox 2
  }
  iebBlueOnOff=2; 

  // Red Instrument Electronics Box (ieb1)
  
  ieb2ID=getWagoID((char*)"ieb1",temp); // Get IEB ID
  if (ieb2ID==-1) {
    mmcLOGGER(shm_addr->MODS.LLOG,(char*)"mmcServer: Red Instrument Electronics Box (IEB_R) Identification not found");
  }

  allPower = wagoSetGet(0,shm_addr->MODS.WAGOIP[ieb2ID],1,514,onoff,1);
  if (allPower<0) {
    mmcLOGGER(shm_addr->MODS.LLOG,(char*)"IEB IEB_R=OFF Red IEB is OFF");
    isisStatusMsg((char*)"IEB IEB_R=OFF Red IEB Box is OFF");
    iebRedOnOff=0;
  }
  else { 
    mmcLOGGER(shm_addr->MODS.LLOG,(char*)"IEB IEB_R=ON Red IEB is ON");
    isisStatusMsg((char*)"IEB IEB_R=ON Red IEB Box is ON");
    iebRedOnOff=1; // Red IEB mechanisms are in IEBox 1
  }
  iebRedOnOff=1; // Red IEB mechanisms are in IEBox 1

  ostringstream os;
  os << "\n";
  os << "AGWY, AGWX, AGWFOCUS, AGWFILTER - Handled by agwServer\n";

  for (unit=0;unit<MAX_ML-1;unit++) {
    if (shm_addr->MODS.host[unit]==0) { ;
    }
    else if(shm_addr->MODS.host[unit]==1 &&
	    !strncasecmp(shm_addr->MODS.who[unit],"agw",3) ||
	    !strncasecmp(shm_addr->MODS.who[unit],"rimcs",5) ||
	    !strncasecmp(shm_addr->MODS.who[unit],"bimcs",5) ||
	    !strncasecmp(shm_addr->MODS.who[unit],"red",3) ||
	    !strncasecmp(shm_addr->MODS.who[unit],"blue",4)) {
      os << "MODS MLC" << unit+1 << ", "  << shm_addr->MODS.who[unit]
	 << " , " << shm_addr->MODS.commport[unit].Port
	 << " ,Handled by another service " << endl;
    }
    else {
      if (unit!=MAX_ML-1) {
	if ((shm_addr->MODS.ieb_i[unit]==iebRedOnOff) ||
	    (shm_addr->MODS.ieb_i[unit]==iebBlueOnOff)) {
	  CloseTTYPort(&shm_addr->MODS.commport[unit]); // Close before opening
	  if (OpenTTYPort(&shm_addr->MODS.commport[unit]) < 0) {
	    os << "ERROR: MODS MLC" << unit+1 << " , " 
	       << shm_addr->MODS.who[unit]
	       << ", " << shm_addr->MODS.commport[unit].Port
	       << "\nCannot Open communications, Reason: "
	       << strerror(errno)
	       << "\nCheck mechanism.ini file" << endl;
	  }
	  else
	    os << "MODS MLC" << unit+1 << " , " << shm_addr->MODS.who[unit]
	       << " , " << shm_addr->MODS.commport[unit].Port << " OPENED"
	       << endl;
	}
      }
    }
  }
  string strtemp=os.str();
  sprintf(temp,"%s",&strtemp[0]);
  mmcLOGGER(shm_addr->MODS.LLOG,temp);

  memset(buf,0,sizeof(buf));
  if( (listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    sprintf(buf,"mmcServer: listenfd - Socket failed %s",strerror(errno));
    mmcLOGGER(shm_addr->MODS.LLOG,buf);
    perror("sockfd\n");
    exit(1);
  }

  shm_addr->MODS.modsPorts[2]=PORT; // Save the Port number in shared memory

  bzero(&srv, sizeof(srv));
  srv.sin_family = AF_INET;
  srv.sin_addr.s_addr = INADDR_ANY;
  srv.sin_port = htons(PORT);

  if( bind(listenfd, (struct sockaddr *) &srv, sizeof(srv)) < 0) {
    sprintf(buf,"mmcServer: Socket binding failed %s",strerror(errno));
    mmcLOGGER(shm_addr->MODS.LLOG,buf);
    mmcLOGGER(shm_addr->MODS.LLOG,(char*)"mmcServer: Socket binding failed");
    perror("bind\n");
    exit(1);
  }

  listen(listenfd, 1024);

  //nonblock(client.FD); // Make ISIS client non-blocking

  // Initialize the mutex lock
  
  // pthread_mutex_init(&lock,NULL);

  // Create the threads
  
  for(i = 0; i < MAX_THREAD; i++) {
    //pthread_create(&threads[i].tid, NULL, &thread_init_func, (void *)i);
    pthread_create(&threads[i].tid, NULL, &thread_init_func, (void *)(long long)(i));
    threads[i].client_count = 0;
  }

  client.KeepGoing = 1;

  while (client.KeepGoing) {

    MilliSleep(1);

    // clear the table of active file descriptors
    FD_ZERO(&read_fd);

    // we always listen to the listen IP and Socket
    if (listenfd > 0) FD_SET(listenfd, &read_fd);

    // if enabled, listen to this app's ISIS socket
    if (client.FD > 0) FD_SET(client.FD, &read_fd);

    n_ready = 0; // Do the select() call and wait for activity from anyone
    n_ready = select(sel_wid, &read_fd, NULL, NULL, NULL);

    if (n_ready == 0) { // would be a timeout if enabled, do nothing...
      cout << "select() interrupted by Ctrl+C..." << endl;
      continue;

    } else if (n_ready < 0) {
      if (errno == EINTR) { // caught Ctrl+C, hopefully sigint handler caught it
	if (client.isVerbose)
	  cout << "select() interrupted by Ctrl+C...\n";

      } else { // something else bad happened, let us know
	cout << "Warning: select() failed - " << strerror(errno) 
	     << " - pressing on anyway...\n";
      }
      continue;

    } else { // somebody wants something, figure out who...

      if (FD_ISSET(client.FD, &read_fd)) {  // Client socket input
	memset(buf,0,sizeof(buf));
	if (charReceived=ReadClientSocket(&client,buf)>0) {
	  if (client.isVerbose) cout << "mmcServer: " << buf << endl;
	  readFromListen_port=2;
	  if(shm_addr->MODS.mmcServerCounter >= MAX_ML-2) {
	    shm_addr->MODS.mmcServerCounter=0;
	    cmdCounter++;
	  } else {
	    shm_addr->MODS.mmcServerCounter++;
	    cmdCounter++;
	  }
	  if (client.isVerbose)
	    cout << " ISIS_port=" << readFromListen_port << ", client.FD=" << client.FD << endl;
	  
	}
      }

      if (FD_ISSET(listenfd, &read_fd)) {  // Client socket input
	memset(buf,0,sizeof(buf));
	readFromListen_port=1;
	if(shm_addr->MODS.mmcServerCounter >= MAX_ML-2) {
	  shm_addr->MODS.mmcServerCounter=0;
	  cmdCounter++;
	} else {
	  shm_addr->MODS.mmcServerCounter++;
	  cmdCounter++;
	}
      	if (client.isVerbose)
      	  cout << " Listen_port=" << readFromListen_port << ", clifd=" << clifd << endl;
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
	  } else if(readFromListen_port==2) { // for ISIS protocol
	    threads[choosen].clients[i] = client.FD;
	    threads[choosen].client_count++;
	    break;
	  }
	}	
      }
    }

    for(i = 0; i < MAX_THREAD; i++) {
      if (client.isVerbose)
	cout << "threads[" << i << "].client_count: "
	     << threads[i].client_count
	     << "thread_id:" << threads[i].tid 
	     << endl;
    }
    
    pthread_mutex_unlock(&new_connection_mutex);

  } // end of select() I/O handling checking

  if(errno) {
    cout << "errno: " << errno << endl;
  }

  // pthread_mutex_destroy(&lock);
  
  return 0;
}

// Here for standalone input system to stop motion.

void
HandleInt(int signalValue) 
{
  int i;
  int unit;
  char dummy[79];

  if (client.Debug)
    cout << "Caught Ctrl+C (Signal " << signalValue << ")..." << endl;

  for(unit=0;unit<MAX_ML-1;unit++) {
    if(shm_addr->MODS.host[unit]==0);
    else if(shm_addr->MODS.host[unit]==1 &&
	    !strncasecmp(shm_addr->MODS.who[unit],"agw",3));
    else
      if(unit!=MAX_ML-1) {
	CloseTTYPort(&shm_addr->MODS.commport[unit]);
	sprintf(dummy,"mmcServer %s connection CLOSED",
		shm_addr->MODS.who[unit]);
	mmcLOGGER(shm_addr->MODS.LLOG,dummy);
	cout << "mmcServer " << shm_addr->MODS.who[unit] 
	     << " CLOSED" << endl;
      }
    MilliSleep(10);
  }
  mmcLOGGER(shm_addr->MODS.LLOG,(char*)"mmcServers have been halted by a killall");
  cout << "mmcServers have been halted by a 'killall -s SIGINT mmcServer'"
       << endl;

  memset(buf,0,sizeof(buf));
  for(i = 0; i < MAX_THREAD; i++) {
    threads[i].clients[i] = 0;
    threads[i].client_count = 0;
  }
  if(clifd>0) {
    close(clifd);
  }

  client.KeepGoing = 0;

  exit(0); // exit mmcServer
}

// The use of this functions avoids the generation of "zombie" processes.

void sig_chld( int signo )
{
  pid_t pid;
  int stat;

  while ( ( pid = waitpid( -1, &stat, WNOHANG ) ) > 0 ) {
    cout << "Child " << pid << " terminated" << endl;
  }
}
