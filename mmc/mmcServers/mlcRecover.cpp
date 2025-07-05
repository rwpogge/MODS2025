/*!
  \file mlcRecover.cpp
  \brief mlcRecover - Recovers all lost data due to a reboot.

  \author Staff, OSU Astronomy Dept. (rgonzale@astronomy.ohio-state.edu)
  \date 2009 Mar 12 (rdg)

  \par Usage: Unix style.
  mlcRecover(); 

  Shared Memory recover function, recovers the MicroLynx Controller state,
  agwFilter names, camera filter names, and anything else we can think of
  Information is place computers MODS shared memory segments.

  NOTE: These files are kept in /home2/mods/data
  shrmem.tab, agwfilters.tab, camfilters.tab misc.tab

  2025 June 20 - AlmaLinux 9 port [rwp/osu]

*/

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <sys/sendfile.h>

using namespace std;

#include "instrutils.h"  
#include "isl_funcs.h"  
#include "isl_types.h"  
#include "params.h"      // Common parameters and defines
#include "islcommon.h"   // Common parameters and defines
#include "isl_shmaddr.h" // Shared memory attachment.

void
StrUpper(char *str)
{
  int i;
  for(i=0;i<strlen(str);i++)
    if(str[i] >= 'a' && str[i] <= 'z')
      str[i] = toupper(str[i]);
}
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

//----------------------------------------------------------------

#define MAX_TABLINES 80  //!< maximum number of lines to read from a mechanism table

int
main(int argc, char *argv[])
{
  int i,j;
  int len;
  int itemNumber;
  int dFocNum;
  char num[4], itemName[24];
  char strcopy[128];
  char temp[79];
  char itemInfo[128];
  string state_copy;

  setup_ids();

  // use MODS_CONFIG and define symlinks to define the instance

  sprintf(temp,"%s/%s.tab",MODS_CONFIG,argv[1]);
  StrUpper(argv[1]);

  if(!strcasecmp(argv[1],"SLITMASK")) {
    sprintf(shm_addr->MODS.slitmaskName[0],"%s",getDateTime()); 
    ifstream files_data (temp);
    for(int i=0;i<MAX_TABLINES && !files_data.eof();i++) {
      getline(files_data, state_copy);
      len=strlen(&state_copy[0]);
      strcpy(strcopy,&state_copy[0]);
      if(strcopy[0]=='#')
	;
      else {
	sscanf(&strcopy[0],"%d %s %[^\n]",&itemNumber,itemName,itemInfo);
	if(itemNumber!=0) {
	  sprintf(shm_addr->MODS.slitmaskName[itemNumber],"%s",itemName); 
	  sprintf(shm_addr->MODS.slitmaskInfo[itemNumber],"%s",itemInfo); 
	}
      }
    }
    files_data.close();
    MilliSleep(100);

  } else if(!strcasecmp(argv[1],"RGRATING")) {
    sprintf(shm_addr->MODS.rgrating[0],"%s",getDateTime()); 
    ifstream files_data (temp);
    for(int i=0;i<MAX_TABLINES && !files_data.eof();i++) {
      getline(files_data, state_copy);
      len=strlen(&state_copy[0]);
      strcpy(strcopy,&state_copy[0]);
      if(strcopy[0]=='#')
	;
      else {
	sscanf(&strcopy[0],"%d %s %[^\n]",&itemNumber,itemName,itemInfo);
	if(itemNumber!=0) {
	  sprintf(shm_addr->MODS.rgrating[itemNumber],"%s",itemName); 
	  sprintf(shm_addr->MODS.rgratInfo[itemNumber],"%s",itemInfo); 
	}
	//	printf("[%d]%s\n",itemNumber,itemName);
      }
    }
    files_data.close();
    MilliSleep(100);

  } else if(!strcasecmp(argv[1],"BGRATING")) {
    sprintf(shm_addr->MODS.bgrating[0],"%s",getDateTime()); 
    ifstream files_data (temp);
    for(int i=0;i<MAX_TABLINES && !files_data.eof();i++) {
      getline(files_data, state_copy);
      len=strlen(&state_copy[0]);
      strcpy(strcopy,&state_copy[0]);
      if(strcopy[0]=='#')
	;
      else {
	sscanf(&strcopy[0],"%d %s %[^\n]",&itemNumber,itemName,itemInfo);
	if(itemNumber!=0) {
	  sprintf(shm_addr->MODS.bgrating[itemNumber],"%s",itemName); 
	  sprintf(shm_addr->MODS.bgratInfo[itemNumber],"%s",itemInfo); 
	}
	//	printf("[%d]%s\n",itemNumber,itemName);
      }
    }
    files_data.close();
    MilliSleep(100);

  } else if(!strcasecmp(argv[1],"BFILTER")) {
    sprintf(shm_addr->MODS.bcamfilters[0],"%s",getDateTime()); 
    ifstream files_data (temp);
    for(int i=0;i<MAX_TABLINES && !files_data.eof();i++) {
      getline(files_data, state_copy);
      len=strlen(&state_copy[0]);
      strcpy(strcopy,&state_copy[0]);
      if(strcopy[0]=='#')
	;
      else {
	sscanf(&strcopy[0],"%d %s %d %[^\n]",&itemNumber,itemName,&dFocNum,itemInfo);
	if(itemNumber!=0) {
	  sprintf(shm_addr->MODS.bcamfilters[itemNumber],"%s",itemName); 
	  sprintf(shm_addr->MODS.bcamfiltInfo[itemNumber],"%s",itemInfo); 
	}
	//	printf("[%d]%s\n",itemNumber,itemName);
      }
    }
    files_data.close();
    MilliSleep(100);

  } else if(!strcasecmp(argv[1],"RFILTER")) {
    sprintf(shm_addr->MODS.rcamfilters[0],"%s",getDateTime()); 
    ifstream files_data (temp);
    for(int i=0;i<MAX_TABLINES && !files_data.eof();i++) {
      getline(files_data, state_copy);
      len=strlen(&state_copy[0]);
      strcpy(strcopy,&state_copy[0]);
      if(strcopy[0]=='#')
	;
      else {
	sscanf(&strcopy[0],"%d %s %d %[^\n]",&itemNumber,itemName,&dFocNum,itemInfo);
	if(itemNumber!=0) {
	  sprintf(shm_addr->MODS.rcamfilters[itemNumber],"%s",itemName); 
	  sprintf(shm_addr->MODS.rcamfiltInfo[itemNumber],"%s",itemInfo); 
	}
      }
    }
    files_data.close();
    MilliSleep(100);

  } else if(!strcasecmp(argv[1],"AGWFILT")) {
    sprintf(shm_addr->MODS.agwfilters[0],"%s",getDateTime()); 
    ifstream files_data (temp);
    for(int i=0;i<MAX_TABLINES && !files_data.eof();i++) {
      getline(files_data, state_copy);
      len=strlen(&state_copy[0]);
      strcpy(strcopy,&state_copy[0]);
      if(strcopy[0]=='#')
	;
      else {
	sscanf(&strcopy[0],"%d %s %[^\n]",&itemNumber,itemName,itemInfo);
	if(itemNumber!=0) {
	  sprintf(shm_addr->MODS.agwfilters[itemNumber],"%s",itemName); 
	  sprintf(shm_addr->MODS.agwfiltInfo[itemNumber],"%s",itemInfo); 
	}
      }
    }
    files_data.close();
    MilliSleep(100);

  } else if(!strcasecmp(argv[1],"DICHROIC")) {
    sprintf(shm_addr->MODS.dichroicName[0],"%s",getDateTime()); 
    ifstream files_data (temp);
    for(int i=0;i<MAX_TABLINES && !files_data.eof();i++) {
      getline(files_data, state_copy);
      len=strlen(&state_copy[0]);
      strcpy(strcopy,&state_copy[0]);
      if(strcopy[0]=='#')
	;
      else {
	sscanf(&strcopy[0],"%d %s %d %[^\n]",&itemNumber,itemName,&dFocNum,itemInfo);
	if(itemNumber!=0) {
	  sprintf(shm_addr->MODS.dichroicName[itemNumber],"%s",itemName); 
	  sprintf(shm_addr->MODS.dichroicInfo[itemNumber],"%s",itemInfo); 
	}
      }
    }
    files_data.close();
    MilliSleep(100);

  }
  return 0;
}
