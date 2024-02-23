// 
// ISIS Utility Library                                                
//
// Contents:
//   LeftStr()      - return n left-most characters from a string
//   RightStr()     - return n right-most characters from a string
//   MidStr()       - return n characters from middle of a string
//   SubStr()       - extract a substring from a larger string
//   UpperCase()    - convert string to all uppercase
//   GetArg()       - retrieve an argument from a command string 
//   UTCDate()      - read system UTC clock and return ISO8601 date 
//   UTCTime()      - read system UTC time clock and return hh:mm:ss
//   ISODate()      - return UTC date/time in full ISO8601 "T" format
//   UTCDateTag()   - read system UTC clock and return CCYYMMDD date tag
//   GetFineTime()  - get fine-grained system UTC time with usec precision
//   SysTimestamp() - return the fine-grained system time in with usec
//                    precision as seconds elapsed since UTC 1970 January 1.
//   MilliSleep()   - pause execution for a certain number of msec
//
// Prototypes in the isisutils.h header file
//
 
/*!
  \file isisutils.c
  \brief ISIS client utility functions.

  A suite of utility functions that most ISIS client applications will
  find useful.  These include functions for string handling (something
  stock C is not well-equipped with), an dfunctions for getting time and
  date information, including date/time in ISO8601 format.
*/

#include "isisclient.h"  // ISIS client routines header

/*!
  \brief Extract the n left-most characters from a given string.

  \param substr Substring to extract
  \param str Original string
  \param nchar Number of characters to extract

*/

//---------------------------------------------------------------------------
//
// LeftStr() - return n left-most characters from a given string
//
// Arguments:
//   char *substr - substring to extract
//   char *str    - original string
//   int nchar    - number of characters to extract
//
// Author:
//   Brian Hartung (OSU undergraduate)
//   from the old Caliban code
//

void 
LeftStr(char *substr, char *str, int nchar)
{
  int i;

  for(i=0;i<nchar && i<strlen(str);i++) {
    substr[i] = str[i];
  }
  //substr[i] = '\0';
  *(substr+i) = '\0';
}

/*!
  \brief Extract the n right-most characters from a given string

  \param substr Substring to extract
  \param str Original string
  \param nchar Number of characters to extract
*/

//---------------------------------------------------------------------------
//
// RightStr() - return n right-most characters from a given string
//
// Arguments:
//   char *substr - substring to extract
//   char *str    - original string
//   int nchar    - number of characters to extract
//
// Author:
//   Brian Hartung (OSU undergraduate)
//   from the old Caliban code
//

void 
RightStr(char *substr, char *str, int nchar)
{
  int i,j;
  int len_str;

  if(nchar>(len_str=strlen(str)))
    strcpy(substr, str);
  else
    {
      for(i=0,j=len_str-nchar;i<nchar;i++,j++)
	substr[i] = str[j];
      *(substr+i) = '\0';
    }
}

/*!
  \brief Extract n characters starting in the middle of a string

  \param substr Substring to extract
  \param str Original string
  \param start Starting character in str
  \param nchar Number of characters to extract

  \sa SubStr()
*/

//-------------------------------------------------------------------------
//
// MidStr() returns n characters starting in the middle of a string
//
// Arguments:
//   char *substr - substring to extract
//   char *str    - original string
//   char start   - starting character of the new substring
//   char nchar   - number of characters to extract
//
// Author:
//   Brian Hartung (OSU undergraduate)
//   from the old Caliban code
//

void 
MidStr(char *substr, char *str, int start, int nchar)
{
  int i;

  for(i=0,start--;i<nchar && start<strlen(str);i++,start++)
    substr[i]=str[start];
}

/*!
  \brief Extracts a substring from inside a larger string

  \param substr Substring to extract
  \param str Original string
  \param first First character in str to extract
  \param last Last character in str to extract

  \sa MidStr()
*/

//-------------------------------------------------------------------------
//
// SubStr() - extracts a substring from a larger string
//
// Arguments:
//   char *substr - substring to extract
//   char *str    - original string
//   char first   - starting character of the substring to extract
//   char last    - last character in the string to extract
//
// Returns null if the first/last character indexes are out of bounds or
// ridiculous (last<first).  last=first returns one character.  All
// character positions in the main string are 0-relative in C fashion.
//
// Author:
//   R. Pogge, OSU Astronomy
// 

void 
SubStr(char *substr, char *str, int first, int last)
{
  int i,nchar;
  if (first < 0 || last > strlen(str)-1 || last<first) {
    *(substr) = '\0';
  }
  else {
    nchar = last-first+1;
    for(i=0;i<nchar;i++)
      substr[i]=str[i+first];
  }
}

/*!
  \brief Convert all characters in a string up uppercase

  \param str String to be converted to uppercase
*/

//-------------------------------------------------------------------------
//
// UpperCase() - convert all characters in a string up uppercase
//
// Arguments:
//   char *str - string to be converted to uppercase.  Changed by
//               this function.
//
// Author:
//   Brian Hartung (OSU undergraduate)
//   from the old Caliban code
//

void 
UpperCase(char *str)
{
  int i;

  for(i=0;i<strlen(str);i++)
    if(str[i] >= 'a' && str[i] <= 'z')
      str[i] = toupper(str[i]);
}

/*!
  \brief Extract the n-th argument from a command string with arguments separated by spaces

  \param cmdstr Command string with space-separated arguments
  \param argnum Number of the argument to extract (1..n)
  \param argstr Argument string to return

  Returns the n-th argument in a command string that contains a series
  of command-line arguments separated by spaces.  Provides a very simple
  command parsing capability.  The command looks like

  \verbatim cmd arg1 arg2 .. argN \endverbatim

  With the command verb itself being considerd argument "0".  

  Returns a null string in argstr if argnum is out of range.
*/

//-------------------------------------------------------------------------
//
// GetArg() - extract the n-th argument from a command string with
//            arguments separated by spaces
//
// Arguments:
//   char *cmdstr - command string with space-separated arguments
//   int argnum   - number of the argument to extract (1..n)
//   char *argstr - argument string to return
//
// Author:
//   Brian Hartung (OSU undergraduate)
//   from the old Caliban code
//
 
void 
GetArg(char *cmdstr, int argnum, char *argstr)
{
  int arglen, i=0, j=0;
  
  arglen=strlen(cmdstr); // Record the length of the overall argument string 

  // Seek to the beginning of the nth argument, where a space delimits args 

  for(i=0;argnum>1;argnum--) {
    while(cmdstr[i]!=' ') {
      i++;
      if(i>=arglen) // If there aren't n arguments, return null 
	break;
    }

    while(cmdstr[i]==' ') {     // Ignore extra spaces between args 
      i++;
      if(i>=arglen) {
	break;
      }
    }
  }
  
  // Now we should be on the first character of the nth argument So,
  // begin copying it to the return buffer until the next space or end
  // of the arg string 

  for(j=0;(i<arglen) && ((cmdstr[i]!=' ') || (cmdstr[i]=='"')) && 
	(cmdstr[i]!='\n'); i++, j++) {
    // Replace any nulls with a space 
    argstr[j] = (cmdstr[i]=='\0' ? ' ' : cmdstr[i]); 
  }

  *(argstr+j) = '\0';
  //argstr[j] = '\0';
}

//-------------------------------------------------------------------------
//
// UTC Time Functions - return strings with different time components
//
// Arguments: none
//
// Descriptions: 
//
//   char *UTCDate() - returns ISO8601 format UTC date: CCYY-MM-DD
//   char *UTCTime() - returns ISO8601 format UTC time hh:mm:ss to nearest sec
//   char *UTCDateTag() - returns CCYYMMDD date tag as a single string
//   char *ISODate() - return full ISO8601 format UTC dateTtime string
//
//   Reads the system's UTC time clock, which it assumes is right.
//
// Author:
//   R. Pogge, OSU Astronomy Dept.
//   pogge@astronomy.ohio-state.edu
//   2003 September 10
//
// Modification History:
//   2004 Feb 29: added ISODate() function [rwp/osu]
//
//-------------------------------------------------------------------------

/*!
  \brief Create a CCYYMMDD date tag string.

  Reads the system timeclock and returns the UTC date in \c CCYYMMDD
  format.  This is a hyphen-free variant on the standard ISO8601 format
  date specification.

  \sa UTCDate(), ISODate()
*/


char
*UTCDateTag(void)
{
  struct tm *gmt;
  time_t t;
  int monthnum;
  int ccyy;
  static char str[9];

  t = time(NULL);
  gmt = gmtime(&t);
  monthnum = (gmt->tm_mon)+1;
  ccyy = gmt->tm_year + 1900;

  // ISO 8601-style date tag format: ccyymmdd 

  memset(str,0,sizeof(str));
  sprintf(str,"%.4i%.2i%.2i",ccyy,monthnum,gmt->tm_mday);
  return str;
}

/*!
  \brief Return the UTC date in ISO8601 format

  Reads the system clock and returns the UTC date in ISO8601-compliant
  \c CCYY-MM-DD format, where CCYY is the year (century and year), MM is
  the month number (01..12), and DD is the day of the month
  (01..28/29/30/31).
  
  \sa UTCDateTag(), ISODate()
*/

char
*UTCDate(void)
{
  struct tm *gmt;
  time_t t;
  int monthnum;
  int ccyy;
  static char str[11];

  t = time(NULL);
  gmt = gmtime(&t);
  monthnum = (gmt->tm_mon)+1;
  ccyy = gmt->tm_year + 1900;

  // ISO 8601 Date Format: ccyy-mm-dd 

  memset(str,0,sizeof(str));
  sprintf(str,"%.4i-%.2i-%.2i",ccyy,monthnum,gmt->tm_mday);
  return str;

}

/*!
  \brief Return the UTC Time in ISO8601 format

  Reads the system clock and returns the UTC time in ISO8601-compliant
  time to the nearest second in \c hh:mm:ss format.

  \sa GetFineTime(), ISODate()
*/

char
*UTCTime(void)
{
  struct tm *gmt;
  time_t t;
  int monthnum;
  int ccyy;
  static char str[9];

  t = time(NULL);
  gmt = gmtime(&t);

  // ISO 8601 Time Format: hh:mm:ss 

  memset(str,0,sizeof(str));
  sprintf(str,"%.2i:%.2i:%.2i",gmt->tm_hour,gmt->tm_min,gmt->tm_sec);
  return str;

}

/*!
  \brief Return the UTC date and time in ISO8601 format

  Reads the system clock and returns the UTC date and time
  in full ISO8601-compliant date/time format:

  \verbatim CCYY-MM-DDThh:mm:ss \endverbatim

  where:
  \arg \c CCYY-MM-DD is the UTC date
  \arg \c hh:mm:ss is the UTC time to the nearest second

  We omit the zone specification described by ISO8601 since this
  only reads UTC time.

  \sa UTCDate(), UTCTime(), UTCDateTag()
*/

char
*ISODate(void)
{
  struct tm *gmt;
  time_t t;
  int monthnum;
  int ccyy;
  static char str[11];

  t = time(NULL);
  gmt = gmtime(&t);
  monthnum = (gmt->tm_mon)+1;
  ccyy = gmt->tm_year + 1900;

  // Full ISO 8601 Date/Time Format: ccyy-mm-ddThh:mm:ss

  memset(str,0,sizeof(str));
  sprintf(str,"%.4i-%.2i-%.2iT%.2i:%.2i:%.2i",ccyy,monthnum,
	  gmt->tm_mday,gmt->tm_hour,gmt->tm_min,gmt->tm_sec);
  return str;

}

/*!
  \brief Return UTC time with microsecond precision.

  Reads the system's UTC time clock and returns a pointer to a string
  with the fine-grained UTC time in \c hh:mm:ss.ssssss format.
 
  Based on gf_time() from Stevens, W.R., 1998, <i>Unix Network
  Programming</i>, Vol 2, Prentice Hall, Figure 15.6, but I make a
  string, and extract time to microsec instead of msec.

  Note: It returns microsecond precision, accuracy is quite another
  matter...

  \sa UTCTime(), ISODate()
*/

//-------------------------------------------------------------------------
//
// GetFineTime() - read the system's UTC time clock and return the
//                 fine-grained time, with seconds to usec precision
//
// Arguments: none
//
// Description: 
//   Reads the system's UTC time clock and returns a pointer to a
//   string with the fine-grained UTC time in the format
//
//      hh:mm:ss.ssssss
// 
//   Based on gf_time() from Stevens, W.R., 1998, Unix Network Programming,
//   Vol 2, Prentice Hall, Figure 15.6, but I make a string, and extract time
//   to microsec instead of msec.
//
// Author:
//   R. Pogge, OSU Astronomy Dept.
//   pogge@astronomy.ohio-state.edu
//   2003 January 7
//
// Modification History:
//
//-------------------------------------------------------------------------

char 
*GetFineTime(void)
{
  struct timeval tv;
  static char str[30];
  char *ptr;
  struct tm *gmt;
  time_t t;

  // first get the UTC time 

  t = time(NULL);
  gmt = gmtime(&t);

  // then get the usec part, If we're off a couple of usec no big deal 

  if (gettimeofday(&tv,NULL)<0)
    printf("gettimeofday error\n");
  ptr = ctime(&tv.tv_sec);

  sprintf(str,"%.2i:%.2i:%.2i.%06ld",gmt->tm_hour,gmt->tm_min,
	  gmt->tm_sec,tv.tv_usec);

  return(str);

}

/*!
  \brief Return elapsed time since UTC 1970 Jan 1 with microsecond precision.

  Reads the system's time clock and returns a double-precision value
  with the time in seconds and microseconds since UTC 1970 January 1.
  The primary use of this function is to create a fine-grained numerical
  timestamp for the system, but other uses are possible.

  Note: It returns microsecond precision, accuracy is quite another matter...

  \sa GetFineTime()
*/

//-------------------------------------------------------------------------
//
// SysTimestamp() - read the system's time clock and return the
//                  elapsed time in sec since UTC 1970 Jan 1 with
//                  usec precision.
//
// Arguments: none
//
// Description: 
//   Reads the system's time clock and returns a double-precision
//   value with the time in seconds and microseconds since UTC
//   1970 January 1.  This provides us with a fine-grained numerical
//   timestamp for the system.
//
// Author:
//   R. Pogge, OSU Astronomy Dept.
//   pogge@astronomy.ohio-state.edu
//   2003 January 7
//
// Modification History:
//
//-------------------------------------------------------------------------

double
SysTimestamp(void)
{
  struct timeval tv;
  static char str[30];
  char *ptr;
  double ttag;

  if (gettimeofday(&tv,NULL)<0)
    printf("gettimeofday error\n");
  ptr = ctime(&tv.tv_sec);

  sprintf(str,"%ld.%06ld",tv.tv_sec,tv.tv_usec);
  ttag=atof(str);
  // printf("ttag=%s (%f)\n",str,ttag);

  return(ttag);

}

/*!
  \brief Pause execution for a certain number of msec.

  \param msec - time to sleep in milliseconds
  \return 0 if successful, -1 if there were problems.  errno says
  what is bothering it.

  Acts as a convenience function for using nanosleep(), the POSIX.1b
  conformal way of suspending a process for a certain time.  nanosleep()
  is a replacement for the obsolete usleep() when a sleep interval more
  fine-grained than sleep() is required. sleep() only allows intervals
  in integer seconds, which is not short enough for some applications.
  For most of what we do, msec are the appropriate interval, and
  nanosleep() can be somewhat involved to setup.  This function encapsulates
  the nasty setup for calling nanosleep() as a convenience for users.

  See "man 2 nanosleep" for details on the nanosleep() function.
*/

//-------------------------------------------------------------------------
//
// MilliSleep() - pause execution for a certain number of msec
//
// Arguments:
//   long msec - time to sleep in milliseconds
//
// Description: 
//   Acts as a convenient function for using nanosleep(), the POSIX.1b
//   conformal way of suspending a process for a certain time.
//   nanosleep() is a replacement for the obsolete usleep() when a sleep
//   interval more fine-grained than sleep() is required. sleep() only
//   allows intervals in integer seconds, which is not short enough for
//   some applications.  For most of what we do, msec are the
//   appropriate interval, and nanosleep() can be somewhat involved to
//   setup, as the code below should reveal.
//
//   See "man 2 nanosleep" for details on the nanosleep() function.
//
// Returns, 0 if successful, -1 if there were problems.  errno says
// what is bothering it.
//
// Author:
//   R. Pogge, OSU Astronomy Dept.
//   pogge@astronomy.ohio-state.edu
//   2004 March 18
//
// Modification History:
//
//-------------------------------------------------------------------------

int
MilliSleep(long msec)
{
  struct timespec treq, trem;
  long tsec, nsec;

  if (msec < 0)
    return -1;

  if (msec >= 1000) {
    tsec = (long)(floor((double)(msec)/1000.0));  // seconds part
    nsec = (long)(1000000000*(((double)(msec)/1000.0) - (double)(tsec)));  // ns part
  }
  else {
    tsec = 0;
    nsec = msec*1000000;   // msec -> nsec
  }

  treq.tv_sec = (time_t)(tsec);
  treq.tv_nsec = nsec;

  return nanosleep(&treq,&trem);

}
