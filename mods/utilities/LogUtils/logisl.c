/*!
  \mainpage logger - Utility used for logging information for all isl functions.
 
  \OSU Astronomy Dept.
  \date 2005 May 04
 
  \section Usage
 
  Usage: logisl(string1, string2, string3, string4);
 
  Where: \c string1 is the error message if return is ?
         \c string2 the logging message.
         \c string3 which microLYNX controllor of device.
         \c string4 directory where you want the log file.
 
  By default, logisl uses log name default.log
  #DEFAULT_RCFILE in the client.h header.
 
  \section Introduction
 
  ...
 
  logisl will be used by all ISL software to log infomation,
  file names will be generated this format - islMMDDD#MC.log
  "isl" Multi-Object Double Spectrograph
  "MMDDD" Month and Day number. dates will by honored
  "#MC" number of microLYNX controllor (1-32)
  ".log"
  When a new day begins at UT midnight, a new logfile name will
  be generated.
                                                                                
  log messages in the log file will have this format:
  YYYY-MM-DD.HH:MM:SS;message
  "YYYY" Year
  "-" dilimiter
  "HH" hour
  "MM" minutes
  "SS" seconds
  "message" What will be received from the function.
                                                                                
<pre>
2005 May 04 - new application [osu/isl]
2025 Jun 19 - AlmaLinux 9 port and cleanup [rwp/osu]
</pre>
                                                                                
\todo
<ul>
<li> Will not be static. As the isl software matures, changes are
<li> likely to be made to logisl.
</ul>
*/
                                                                                
/*!
  \file logisl.c
  \brief logisl main program
*/

#include <stdio.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include <syslog.h>
#include <sys/stat.h>

#include "instrutils.h"  // ISL Instrument header
#include "isl_types.h"
#include "params.h"
#include "islcommon.h"
#include "isl_shmaddr.h"      // shared memory pointer
 
#define MAX_BUF 4086

char *UTCDate(void);
char *UTCTime(void);
void err_report(char *,char *, int, int);


void logisl(
      char mlloc_str[],
      char msg_str[],
      char id[],
      char logfile[])
{
  char file[MAX_BUF];
  char new1[sizeof(file)];
  char loc_stamp[MAX_BUF];
  int len,error,total,ncopy,i,k;
  struct tm *ptr;
  time_t t;
  char buffer[MAX_BUF];
  char ch;
  static FILE *fildes= (FILE *) NULL;
  long offset;

  // get Year, Day, and UT time
  // example1: 2005.005.15:25:15/FW/print pos
  // example2: 2005.005.15:25:15/FW/ERROR print pos
  //
  //  strcpy(loc_stamp,UTCdate());
  //  strcat(loc_stamp,".");
  //  strcat(loc_stamp,UTCTime());
  //  strcat(loc_stamp,"/");
  if (strstr(msg_str,"?")) {
    strcat(loc_stamp,"ERROR ");
    strcat(loc_stamp,msg_str);
  } else {
    strcat(loc_stamp,msg_str);
  }
  msg_str[strlen(msg_str)]='\0';
  sprintf(msg_str,"%s",loc_stamp);

  t=time(NULL);
  if(((time_t) -1) == t) {
    err_report("Error getting time in logisl",NULL,1,0);
    return;
  }

  // Setup new logfile.
  ptr=gmtime(&t);
  strcpy(new1,logfile);
  strcat(new1,"/");
  strcat(new1,mlloc_str);
  strftime(new1+strlen(new1),sizeof(new1),"%y%j",ptr);
  if(-1==snprintf(new1+strlen(new1),sizeof(new1)-strlen(new1),
		  "%c%c.log",id[0],id[1])) {
    err_report("Error formatting entry for log in logisl",new1,1,0);
    return;
  }
  if(strcmp(new1,file)!=0) {
    if(fildes != (FILE *) NULL) {
      if(EOF == fclose(fildes)) {
	err_report("Closing old log in logisl",file,1,0);
	return;
      }
    }

    fildes=(fopen(new1,"a+"));
    if(fildes == (FILE *) NULL) {
      err_report("Opening new1 log in logisl",new1,1,0);
      return;
    }
    if(0!=chmod(new1,0666)) {
      err_report("Setting permissions in logisl",new1,1,0);
      return;
    }

    strncpy(file,new1,sizeof(file));
    offset=ftell(fildes);
    if(offset==(long)-1) {
      err_report("Opening checking log position in logisl",new1,1,0);
      return;
    }
    if(offset!=(long)0){
      if(EOF==fseek(fildes, (long) -1,SEEK_END)) {
	err_report("Error positioning log in logisl",new1,1,0);
	return;
      }
      if(EOF==fread(&ch,1,1,fildes)) {
	err_report("Error reading log in logisl",new1,1,0);
	return;
      }
      if(ch!='\n') {
	ch='\n';
	if(1!=fwrite(&ch,1,1,fildes)) {
	  err_report("Error adding newline to log in logisl",new1,1,0);
	  return;
	}
      }
    }
  }

  //  if(ftell(fildes)==0) {
  //    if(strlen(mlloc_str)+1!=fprintf(fildes,"%s\n",mlloc_str)) {
  //       err_report("Error writing entry to log in logisl",new1,1,0);
  //       return;
  //     }
  //   }

  if(-1==snprintf(buffer,sizeof(buffer)-strlen(buffer),"%s",msg_str)){
    err_report("Error formatting entry for log in logisl",new1,1,0);
    return;
  }
      
  if(strlen(buffer)+1!=fprintf(fildes,"%s\n",buffer)) {
    err_report("Error writing entry to log in logisl",new1,1,0);
    return;
  }

  if(EOF == fflush(fildes)) {
    err_report("Error flushing log stream in logisl",new1,1,0);
    return;
  }
}    
