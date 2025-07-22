#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
using namespace std;
 

#include <sys/sendfile.h>

// avoid paths where you can (do the include path in Makefile.build)
//
// i.e., don't do #include "../../include/isisclient.h"  

#include "isisclient.h"  
#include "instrutils.h"  
#include "isl_funcs.h"  
#include "isl_types.h"  
#include "params.h"      // Common parameters and defines
#include "islcommon.h"   // Common parameters and defines
#include "isl_shmaddr.h" // Shared memory attachment.

int getCurrentTime(char* , char* );

/*!
  \brief Return the UTC date and time to microsecond precision as
         a system timestamp

  Reads the system's UTC time clock and returns a pointer to a
  string with the fine-grained UTC time in \c hh:mm:ss.ssssss format.

  Based on gf_time() from Stevens, W.R., 1998, Unix Network Programming,
  Vol 2, Prentice Hall, Figure 15.6, but I make a string with an ISO8601
  compliant format.
  
  Of course, while it may return microsecond precision, microsecond accuracy
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
  int monthNum;
  int ccyy;

  // First get the UTC time 

  t = time(NULL);
  gmt = gmtime(&t);
  monthNum = (gmt->tm_mon)+1;

  // ISO 8601 Date & time format: ccyy-mm-ddThh:mm:ss 

  ccyy = gmt->tm_year + 1900;
  sprintf(str,"%.4i-%.2i-%.2iT%.2i:%.2i:%.2i",ccyy,monthNum,
          gmt->tm_mday,gmt->tm_hour,gmt->tm_min,gmt->tm_sec);

  // Now get the usec part. If we're off a couple of usec no big deal.

  gettimeofday(&tv,NULL);
  ptr = ctime(&tv.tv_sec);

  // Append it to the ISO8601 date+time string created above

  sprintf(str,"%s.%06ld",str,tv.tv_usec);

  return(str);

}

/*!
  \brief mmcService Logger

  Reads system's UTC time with getDataTime() function and logs 
  Service infomation to mmcLogging.log
  
*/

int
mmcLOGGER(char path[79], char msg[512])
{ 
  int len;
  int result;
  char timestr[80];
  char newfile[80];

  // Check for change of UT Date

  if(strlen(shm_addr->MODS.mmcUpdate1)<=0) {
    sprintf(shm_addr->MODS.mmcUpdate1,"%s",UTCDateTag());
  }

  if(strcasecmp(shm_addr->MODS.mmcUpdate1,UTCDateTag())) {
    len=strlen(path);
    strncpy(newfile,path,len-4); // Remove the .log extention from filename
    len=strlen(newfile);
    sprintf(&newfile[len],".%s.log",shm_addr->MODS.mmcUpdate1); // Append date
    
    result=rename(path,newfile); // rename the logfile to logfile.date.log

    sprintf(shm_addr->MODS.mmcUpdate1,"%s",UTCDateTag()); // recap DateTag
    chmod(path,0666);  // Give permissions to default logging file
    chmod(newfile,0666); // Give permissions for dated logfile

  }

  ofstream mmclog;
  mmclog.open(path, ios::app);
  mmclog << getDateTime() << " " << msg << endl;
  mmclog.close();
  return 0;
}

/*!
  \brief mmcSaveConfig save MODS configuration

  Reads system's UTC time with getDataTime() function and logs 
  Service infomation to mmcSaveConfig.dat
  
*/

int
mmcSaveConfig(char path[79], char mmcData[])
{ 
  return 0;
}

/*!
  \brief mmcGetConfig get the last MODS configuration

  Reads system's UTC time with getDataTime() function and logs 
  Service infomation to modsConfig_1.dat
  
*/

int
mmcGetConfig(char path[79], char mmcData[])
{ 
  string line;

  ifstream mmcget (path);
  if( mmcget.is_open()) {

    getline(mmcget,line);
    strcpy(mmcData,&line[0]);

    mmcget.close();

  } else cout << "Unable to open file" << endl;

  return 0;
}
