/*!
  \mainpage err_report - Utility used for sending infomation to the  
  \                      isl user.

  \OSU Astronomy Dept.
  \date 2005 May 04

  \section Usage

  Usage: err_report(char *str,char *file, int flag,int ierr)

  Where: \c char *str the error message
         \c char *file filensme
         \c int flag 
         \c string4 directory where you want the log file.

  err_report sends information to the /tmp directory. and will send
  an email to whom ever will be responsible for the ISL software.

  \section Introduction

  ...

  err_report sends information to the /tmp directory
  file names will be generated in this format - whoserror.log
  "whoserror" is the name of the function the error came from.
  ".log" 

<pre>
2005 May 04 - new application [osu/isl]
</pre>

\todo 
<ul>
<li> Will not be static. As the isl software matures, changes are
<li> likely to be made to err_report.
</ul>
*/

/*!
  \file err_report.c
  \brief err_report main program
*/

#include <stdio.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include <syslog.h>

#define MAX_BUF 80

void err_report(char *str,char *file, int flag,int ierr)
{
  time_t t;
  static time_t old=-1;
  FILE *fildes;
  char buff[MAX_BUF];

  if(old==-1)
#if 1
    openlog("logisl",LOG_CONS|LOG_PID,LOG_DAEMON);
#else
    openlog("logisl",LOG_CONS|LOG_PID|LOG_PERROR,LOG_DAEMON);
#endif

  buff[0]=0;
  if(flag) {
    snprintf(buff+strlen(buff),-1+sizeof(buff)-strlen(buff),"%s: ",str);
    snprintf(buff+strlen(buff),-1+sizeof(buff)-strlen(buff),"%s",strerror(errno));
  } else
    snprintf(buff+strlen(buff),-1+sizeof(buff)-strlen(buff),"%s",str);
  if(file!=NULL)
    snprintf(buff+strlen(buff),-1+sizeof(buff)-strlen(buff),", %s",file);
  if(ierr!=0)
    snprintf(buff+strlen(buff),-1+sizeof(buff)-strlen(buff),", Error =%d",
	     ierr);

  syslog(LOG_DAEMON|LOG_ERR,"%s",buff);

  t=time(NULL);
  if(old==-1||t-old > 3600) {
    old=t;

    fildes=fopen("/tmp/logisl.error","w+");

    fprintf(fildes,"%s\n",buff);

    fclose(fildes);

    //    system("/bin/cat /tmp/ttytool.error | /usr/bin/mail -s 'metclient ERROR' rgonzale");

    return;
  }
}
