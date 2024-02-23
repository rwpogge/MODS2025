//
// serverlog - routines for server runtime logging
//

/*!
  \file serverlog.c
  \brief Routines for creating a server runtime log.

*/

#include "isisserver.h"  

/*!
  \brief Initialize the ISIS server runtime log for this session.

  Opens the server's runtime log.  Log entries are created by the
  logMessage() function.

*/

void
initLog()
{
  int ierr;
  char logStr[MED_STR_SIZE];
  
  // Query the system clock for the date tag to use.

  getUTCTime();
  switch (isis.logDate) {
  case UTCDATE:
    strcpy(isis.lastDate,isis.dateTag);
    break;
  default:
    strcpy(isis.lastDate,noonDateTag());
    break;
  }

  // Build the log file name from the /path/rootname provided
  // at runtime

  sprintf(isis.logFile,"%s.%s.log",isis.logRoot,isis.lastDate);

  // Open the new runtime log file, append to it if it exists 

  isis.logFD = open(isis.logFile,(O_WRONLY|O_CREAT|O_APPEND));

  if (isis.logFD == -1) {
    printf("ERROR: cannot open runtime log file %s - %s\n",
	   isis.logFile,strerror(errno));
    isis.doLogging == isis_FALSE;
    return;
  }
  chmod(isis.logFile,0666);
  sprintf(logStr,"------------------------------\n");
  ierr = write(isis.logFD,logStr,strlen(logStr));

  memset(logStr,0,sizeof(logStr));
  sprintf(logStr,"%s runtime log (re)started at UTC %s\n",
	  isis.serverID,getDateTime());
  ierr = write(isis.logFD,logStr,strlen(logStr));

  return;

}

/*!
  \brief Append an entry to the server's runtime log with date/time tagging.
  \param message String with the log entry to append to the log.
  \return 0 if successful, <0 if an error occurred.  
  
  Appends the message string given into the server's runtime log, along
  with a date and time tag.  If errors occur, messages are printed on the
  server's command console.  

  If the date tag has changed, start a new logfile for a new observing
  day.

  Returns 0 if the logging has been disabled, making it look the same as
  if the log had been updated.
*/

int
logMessage(char *message) 
{
  int ierr;
  char logStr[BIG_STR_SIZE];
  char dateTag[MED_STR_SIZE];

  // If logging is disabled, return now 

  if (isis.doLogging == isis_FALSE) return(0);
  
  // Did we cross over into the next day?

  getUTCTime();
  switch (isis.logDate) {
  case UTCDATE:
    strcpy(dateTag,isis.dateTag);
    break;
  default:
    strcpy(dateTag,noonDateTag());
    break;
  }

  if (strcasecmp(dateTag,isis.lastDate) != 0) {
    close(isis.logFD);
    initLog();
  }

  // Append the log entry

  if (message[strlen(message)-1]=='\n') message[strlen(message)-1]='\0';

  memset(logStr,0,sizeof(logStr));
  sprintf(logStr,"%s %s\n",getDateTime(),message);
  ierr = write(isis.logFD,logStr,strlen(logStr));

  return(0);

}
