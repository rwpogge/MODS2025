//
// utils.c - ISIS server utilities
//
// Contents:
//   getArg()  - get an argument by position out of a message string
//   upperCase() - convert to all uppercase
//
// Time Utilities: 
// --------------
//   getUTCTime()  - read the system's UTC time clock
//   getFineTime() - get fine-grained system UTC time with usec precision
//   sysTimeStamp() - return the fine-grained system time in with usec
//                    precision as seconds elapsed since UTC 1970 January 1.
//   getDateTime() - return a UTC date/time string in ISO-8601 format
//   noonDateTag() - return a noon-to-noon local date tag (CCYYMMDD format)
//

/*!
  \file utils.c
  \brief Server utility routines

  A suite of basic utility functions, primarily for string and time handling,
  used by the ISIS server application.  

  Some of these are not used - we'll either ignore that or clean up in
  the future.

*/

#include "isisserver.h"

/*!
  \brief Read the UTC time from the system clock.

  Reads the system's UTC time clock and puts date/time information
  into the system table.
  
  Time info is stored as follows:
  \arg isis.dateTag has a date tag in \c ccyymmdd format
  \arg isis.utcDate has the UTC date in ISO8601-compliant \c CCYY-MM-DD format
  \arg isis.utcTime has the UTC time in \c hh:mm:ss format

*/

void
getUTCTime(void)
{
  struct tm *gmt;
  time_t t;
  int monthNumber;
  int ccyy;

  t = time(NULL);
  gmt = gmtime(&t);
  monthNumber = (gmt->tm_mon)+1;

  // ISO 8601 Date & time format: ccyy-mm-dd, hh:mm:ss 

  ccyy = gmt->tm_year + 1900;
  sprintf(isis.dateTag,"%.4i%.2i%.2i",ccyy,monthNumber,gmt->tm_mday);
  sprintf(isis.utcDate,"%.4i-%.2i-%.2i",ccyy,monthNumber,gmt->tm_mday);
  sprintf(isis.utcTime,"%.2i:%.2i:%.2i",gmt->tm_hour,gmt->tm_min,
	  gmt->tm_sec);

}

/*!
  \brief Return the UTC time to microsecond precision.

  Reads the system's UTC time clock and returns a pointer to a
  string with the fine-grained UTC time in \c hh:mm:ss.ssssss format.

  Based on gf_time() from Stevens, W.R., 1998, Unix Network Programming,
  Vol 2, Prentice Hall, Figure 15.6, but I make a string, and restrict
  the output of seconds to msec rather than usec.
  
  Of course, it may return microsecond precision, microsecond accuracy
  is quite another thing...
*/

char *
getFineTime(void)
{
  struct timeval tv;
  static char str[30];
  char *ptr;
  struct tm *gmt;
  time_t t;

  // first get the UTC time 

  t = time(NULL);
  gmt = gmtime(&t);

  sprintf(str,"%.2i:%.2i:%.2i",gmt->tm_hour,gmt->tm_min,gmt->tm_sec);

  // Then get the usec part, If we're off a couple of usec no big deal 

  if (gettimeofday(&tv,NULL)<0) {
    if (isis.useCLI)
      printf("getFineTime() gettimeofday error\n");
    else
      logMessage("ERROR getFineTime() gettimeofday fault");
  }
  ptr = ctime(&tv.tv_sec);

  sprintf(str,"%s.%06ld",str,tv.tv_usec);

  return(str);

}

/*!
  \brief Return the UTC date and time to microsecond precision as
         a system timestamp

  Reads the system's UTC time clock and returns a pointer to a
  string with the fine-grained UTC time in \c hh:mm:ss.ssssss format.

  Based on gf_time() from Stevens, W.R., 1998, Unix Network Programming,
  Vol 2, Prentice Hall, Figure 15.6, but I make a string, and restrict
  the output of seconds to msec rather than usec.
  
  Of course, it may return microsecond precision, microsecond accuracy
  is quite another thing...
*/

char *
getDateTime(void)
{
  struct timeval tv;
  static char str[30];
  char *ptr;
  struct tm *gmt;
  time_t t;
  int monthNumber;
  int ccyy;

  // First get the UTC time 

  t = time(NULL);
  gmt = gmtime(&t);
  monthNumber = (gmt->tm_mon)+1;

  // ISO 8601 Date & time format: ccyy-mm-dd, hh:mm:ss 

  ccyy = gmt->tm_year + 1900;
  sprintf(str,"%.4i-%.2i-%.2iT%.2i:%.2i:%.2i",ccyy,monthNumber,
	  gmt->tm_mday,gmt->tm_hour,gmt->tm_min,gmt->tm_sec);

  // Then get the usec part, If we're off a couple of usec no big deal 

  if (gettimeofday(&tv,NULL)<0) {
    if (isis.useCLI)
      printf("getFineTime() gettimeofday error\n");
    else
      logMessage("ERROR getFineTime() gettimeofday fault");
  }
  ptr = ctime(&tv.tv_sec);

  sprintf(str,"%s.%06ld",str,tv.tv_usec);

  return(str);

}

/*!
  \brief Return the elapsed time in sec since UTC 1970-01-01 with microsec precision.

  Reads the system's time clock and returns a double-precision value
  with the time in seconds and microseconds since UTC 1970-01-01.  This
  provides us with a fine-grained numerical timestamp for the system.

  Of course, it may return microsecond precision, microsecond accuracy
  is quite another thing. The relative time should be as stable as PC
  system clocks ever are.
*/

double 
sysTimeStamp(void)
{
  struct timeval tv;
  static char str[30];
  char *ptr;

  if (gettimeofday(&tv,NULL)<0) {
    if (isis.useCLI)
      printf("sysTimeStamp() gettimeofday error\n");
    else
      logMessage("ERROR sysTimeStamp() gettimeofday fault");
  }
  ptr = ctime(&tv.tv_sec);

  sprintf(str,"%ld.%06ld",tv.tv_sec,tv.tv_usec);
  
  return((double)(atof(str)));

}

/*!
  \brief Local noon-to-noon date tag in CCYYMMDD format

  Convenience function for creating noon-to-noon date tags in the
  local time zone.  If the time is after noon, it uses the current
  local date. If before noon, it uses *yesterday's* local date.

  This convention is used for "observing day", to avoid the problem
  with UTC that afternoon calibrations end up in the previous night's
  logs, or with using local midnight where logs of a night's observing
  are divided between two "civil day" logs.

*/

char *
noonDateTag(void)
{
  struct tm *loct;
  time_t t;
  int localYear, localMonth, localDay, dayNumber;
  int localHour;
  int tday, tmonth, tyear;
  int leap;
  static char str[SHORT_STR_SIZE];
   
  // Last day of each month, 1=Jan ... 12=Dec

  int lastDay[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

  // Read the local clock time

  t = time(NULL);
  loct = localtime(&t);
  localYear = 1900 + (loct->tm_year);
  localDay = loct->tm_mday;
  localMonth = 1 + (loct->tm_mon);   // Unix clock sets 0=Jan
  dayNumber = loct->tm_yday;

  localHour = loct->tm_hour;

  // If it is after local noon, use today's date

  if (localHour >= 12.0) {
    sprintf(str,"%.4i%.2i%.2i",localYear,localMonth,localDay);
    return str;
  }

  // It is before local noon so use yesterday's local date.  We have
  // to worry about what to do if we are at the start of a month
  // (yesterday was last month), with particular attention to leap
  // years if it is March, or if today is Jan 1 (yesterday was last
  // year).

  // If today is March 1, is it a leap year?  The Gregorian Calendar
  // formula for a leap year (Lilius' Algorithm) is that the year must
  // be divisible by 4, except for century years unless divisible by
  // 400.

  if (localMonth==3 && localDay==1) {
    leap = 0;
    if (localYear % 4 == 0) {  
      leap = 1;
      if ((localYear % 100)==0 && (localYear % 400) != 0) leap=0;
    }
    if (leap) lastDay[1] = 29;
  }

  // Check the day of the year.  If 0, yesterday was Dec 31 in the
  // previous year.  Otherwise, subtract one from the day of the
  // month.  If zero, yesterday was in the previous month
  
  tyear = localYear;
  tmonth = localMonth;
  tday = localDay - 1;

  if (dayNumber == 0) {
    tyear = localYear - 1;
    tmonth = 12;
    tday = lastDay[11];
  } 
  else {
    if (tday == 0) {
      tmonth = localMonth - 1;
      tday = lastDay[tmonth-1];
    }
  }

  sprintf(str,"%.4i%.2i%.2i",tyear,tmonth,tday);
  return str;
}

/*!
  \brief Convert a string to all uppercase
  \param str String to convert to uppercase

  BEWARE: Changes the string being converted!

*/

void 
upperCase(char *str)
{
  int i;

  for (i=0;i<strlen(str);i++)
    if (str[i] >= 'a' && str[i] <= 'z')
      str[i] = toupper(str[i]);
}

/*!
  \brief Extract an argument from the command line
  \param argstr string containing the argument list
  \param argnum number of the argument to extract
  \param retarg string to contain the extracted argnum-th argument

  Simple command-line argument parser.  Works OK, but will probably
  replace someday by something fancier of the need arises.

  Returns a null (\\0) in regarg if the argnum-th argument doesn't exist.

  If any stray nulls are found in the string, it replaces them with
  spaces.

*/

void 
getArg(char *argStr, int argNum, char *returnStr)
{
  int argLen, i=0, j=0;
  
  argLen=strlen(argStr);  // Record the length of the overall argument string

  // Seek to the beginning of the nth argument, where a space delimits args

  for(i=0;argNum>1;argNum--) {
    while(argStr[i]!=' ') {
      i++;
      if(i>=argLen) // If there aren't n arguments, return null
	break;
    }

    while(argStr[i]==' ') {     // Ignore extra spaces between args
      i++;
      if(i>=argLen) {
	break;
      }
    }
  }
  
  // Now we should be on the first character of the nth argument So,
  // begin copying it to the return buffer until the next space or end
  // of the arg string

  for(j=0;(i<argLen) && ((argStr[i]!=' ') || (argStr[i]=='"')) && 
	(argStr[i]!='\n'); i++, j++) {
    // Replace any nulls with a space
    returnStr[j] = (argStr[i]==NUL ? ' ' : argStr[i]); 
  }

  returnStr[j] = NUL;
}

