/*
 */
#include "../include/ISLSocket.h"  // For Socket and SocketException
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <cstdlib>            // For atoi()

using namespace std;
 
#include <sys/sendfile.h>
#include "../include/isisclient.h"  
#include "../include/instrutils.h"  

#include "../include/isl_funcs.h"  
#include "../include/isl_types.h"  
#include "../include/params.h"      // Common parameters and defines
#include "../include/islcommon.h"   // Common parameters and defines
#include "../include/isl_shmaddr.h" // Shared memory attachment.

#include <time.h>
const int RCVBUFSIZE = 8192;    // Size of receive buffer

/*
 * rmcrlf() - remove CRLF
 */
  
/*!
  \brief Remove CRLFs and replace them with blanks
  
  \param s  String to be returned
         s1 String to be operated on
  
  Helper utility to change CRLF to blanks in the return string
  
*/
int getCurrentTime(char* , char* );
char *getDateTime(void);
void display_it(int Row, int Col, char *Attrib, char *InLine);
/*
 * Remove CR's LF's 
 */
#include <stdio.h>
int rmcrlf(char s[], char s1[]) {
  int i,j;

  for(i=0,j=0;s[i]!='\0';i++) {
    if (s[i]=='\r' || s[i]=='\n' || s[i]=='>')
      s1[j++]=' ';
    else 
      s1[j++]=s[i];
  }
  /*
   * Blanks were produced by \r \n and > at the end of the return string
   * from the MicroLynx. This will remove the tail-end blanks.
   */
  j--;
  i--;
  for(;s1[j]==' ';--i,--j);
  s1[j+1]='\0';
  s[i+1]='\0';
  return 0;
}

void HandleInt(int signalValue) 
{
  fprintf( stderr,"Stop vueMaps\n");
  exit(0);
}

int main(int argc, char *argv[]) {
  int i,j,k,icol,irow;

  if (argc <= 1){
    printf("Usage: vueMaps [pos|slitmask]\n");
    exit(0);
  }
  
  //  signal(SIGPIPE,SIG_IGN);  // ignore broken pipes

  setup_ids();

  printf("%c[%d;%dH",27,0,0);
  printf("%c[2J",27);

  signal(SIGINT,HandleInt); // Ctrl+C sends a move abort to controller

  icol=0;
  irow=0;
  /* position on the screen. */
  //  irow=1;
  display_it(irow,icol,(char*)"0",(char*)" ");
  display_it(irow,icol,(char*)"",getDateTime());
  printf(" MODS ADDRESS MAP\n");
  irow=1;
  
  if(!strcasecmp(argv[1],"pos")) {
    printf("    RED IEB                                   BLUE IEB\n");
    for(i=0;i<18;i++) {
      printf("%s[%s] = %0.1f\n",shm_addr->MODS.commport[i].Port,shm_addr->MODS.who[i],shm_addr->MODS.pos[i]);
    }

    printf("\n  WAGO(s)\n");
    for(j=0;j<6;j++) {
      printf("%s[%s]\n",shm_addr->MODS.WAGOWHO[j],shm_addr->MODS.WAGOIP[j]);
    }

    icol=40;
    irow=1;
    for(;i<MAX_ML-1;i++,irow++) {
      display_it(irow,icol,(char*)"0",(char*)" ");
      printf("%s[%s] = %0.1f\n",shm_addr->MODS.commport[i].Port,shm_addr->MODS.who[i],shm_addr->MODS.pos[i]);
    }

    display_it(irow++,icol,(char*)"0",(char*)" ");
    display_it(irow++,icol,(char*)"0",(char*)" ");
    printf(" IIFServer Port=%d, ",shm_addr->MODS.modsPorts[0]);
    display_it(irow++,icol,(char*)"0",(char*)" ");
    printf(" agwServer Port=%d, ",shm_addr->MODS.modsPorts[1]);
    display_it(irow++,icol,(char*)"0",(char*)" ");
    printf("mmcService Port=%d",shm_addr->MODS.modsPorts[2]);
    display_it(irow++,icol,(char*)"0",(char*)" ");
    printf("\n");
    
  } else if(!strcasecmp(argv[1],"slitmask")) {
    for(i=0;i<13;i++) {
      if(i==0) { printf("TIME: %s\n",shm_addr->MODS.slitmaskName[i]); i++;}
      if(shm_addr->MODS.active_smask==i)
      	printf("@>MASK %d:%s\n",i,shm_addr->MODS.slitmaskName[i]);
      else
	printf("MASK %d:%s\n",i,shm_addr->MODS.slitmaskName[i]);
    }
    if(shm_addr->MODS.active_smask==-1)
      printf("@>=======:BRACE\n");
    else
      printf("=======:BRACE\n");

    for(;i<25;i++) {
      if(shm_addr->MODS.active_smask==i)
      	printf("@>MASK %d:%s\n",i,shm_addr->MODS.slitmaskName[i]);
      else
	printf("MASK %d:%s\n",i,shm_addr->MODS.slitmaskName[i]);
    }
  }
}

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
  \brief mmcSaveConfig save MODS configuration

  Reads system's UTC time with getDataTime() function and logs 
  Service infomation to mmcSaveConfig.dat
  
*/

int
mmcSaveConfig(char path[79], char mmcData[])
{ 
  ofstream mmcsave (path);

  if( mmcsave.is_open()) {
    mmcsave << getDateTime() << mmcData << endl;
    mmcsave.close();
  } else cout << "Unable to open file" << endl;

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

/*
void display_it();
display_it(irow,icol,"M",buffer);
printf("\n");
display_it(irow,icol,"R",buffer);
printf("\n");
display_it(irow,icol,"Y",buffer);
printf("\n");
display_it(irow,icol,"B",buffer);
*/
/************************************************************************
examples:
display_it(0, 4, "Y5", string);
display_it(1, 4, "R5", string);
display_it(2, 4, "C", STRING);
display_it(2, 4, "Y", STING);
display_it(2, 40, "", string);
display_it(2, 105, "Y", "BAD ");
display_it(2,105,"", "GOOD");
display_it(2, 4, "B", "VAC  MKIII     STOP  ????????");
************************************************************************/
/* Function declarations:
 *   Row - Screen row position
 *   Col - Screen column position
 *   Attrib - Modifiying attributes (blink, underline, red, blue...)
 *   InLine - The text to print
 */
void
display_it(int Row, int Col, char *Attrib, char *InLine)
{
  //int i;

  /* Check for attribute modifiers */
  if (strstr(Attrib, "R")){ // RED
    (void)printf("%c[1;31m", 27);
  }
  else if (strstr(Attrib, "G"))  // GREEN
    (void)printf("%c[1;32m", 27);
  else if (strstr(Attrib, "Y"))  // YELLOW
    (void)printf("%c[1;33m", 27);
  else if (strstr(Attrib, "B")) { // BLUE
    (void)printf("%c[1;34m", 27);
  }
  else if (strstr(Attrib, "M")) //
    (void)printf("%c[1;35m", 27);
  else if (strstr(Attrib, "C")) //
    (void)printf("%c[1;36m", 27);
  else if (strstr(Attrib, "W")) // WHITE
    (void)printf("%c[1;37m", 27);
  else if (strstr(Attrib, "X")) // WHITE
    (void)printf("%c[1;30m", 27);
  
  if (strstr(Attrib, "7"))
    (void)printf("%c[7m", 27);
  if (strstr(Attrib, "5"))
    (void)printf("%c[5m", 27);
  if (strstr(Attrib, "4"))
    (void)printf("%c[4m", 27);
  if (strstr(Attrib, "1"))
    (void)printf("%c[1m", 27);
  if (strstr(Attrib, "0"))
    (void)printf("%c[0m", 27);
  
/* Position the cursor and print the text. If Row and Col are 0, just print */
  if (Row== 0 && Col== 0)
    (void)printf("%s", InLine);
  else
    (void)printf("%c[%d;%dH%s", 27, Row, Col, InLine);
  
/* Reset the video attributes. Don't send anything if we don't have to */
  if (*Attrib!= '\0')
    (void)printf("%c[0m", 27);
  
}
