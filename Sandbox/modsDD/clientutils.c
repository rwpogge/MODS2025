//
// clientutils - client app utilities go here
//

/*!
  \file clientutils.c
  \brief Client application utility functions

  Because they have to go somewhere...

  \author R. Pogge, OSU Astronomy Dept. (pogge.1@osu.edu)
  \date 2025 Sept 19
*/

#include "client.h" // custom client application header 

//---------------------------------------------------------------------------

/*!
  \brief Initialize an LBT information data structure

  \param lbti pointer to an #lbtinfo data structure

  Initializes the LBT telescope information data structure
  with sensible defaults/null values.  This ensures that
  we have some way to avoid data structures with delinquent
  information.

*/

void
initLBTInfo(lbtinfo_t *lbti) 
{
  lbti->Link = 0;    
  lbti->opMode = LABSIM;
  lbti->Turbo = 0; 
  strcpy(lbti->telescope,"LBT-SX");
  strcpy(lbti->iceProps,"config");  // default ICE properties file root name
  strcpy(lbti->proxy,"MODS_DD");
  strcpy(lbti->instID,"MODS1");
  strcpy(lbti->focStation,"directGregorian");
  strcpy(lbti->side,"left");
}

/*!
  \brief Return UTC system time to usec precision in a utcinfo_t struct

  \param utci pointer to a utcinfo_t data structure

  Reads the system's UTC time clock and fills the utcinfo_t struct
  with all the time info we need.

  Based on gf_time() from Stevens, W.R., 1998, Unix Network Programming,
  Vol 2, Prentice Hall, Figure 15.6. 

*/

void
getUTCTime(utcinfo_t *utci)
{
  struct timeval tv;
  struct tm *gmt;
  double utday;
  double mjd;
  int msec;
  int sec;

  gettimeofday(&tv,NULL);
  gmt = gmtime(&tv.tv_sec);

  // UTC date and time components

  utci->Year = 1900 + gmt->tm_year;
  utci->Month = 1 + gmt->tm_mon;
  utci->Day = gmt->tm_mday;
  utci->Hour = gmt->tm_hour;
  utci->Min = gmt->tm_min;
  sec = gmt->tm_sec;
  msec = (int)(tv.tv_usec/1000);
  utci->Sec = (double)(sec) + ((double)(tv.tv_usec)/1000000.0);

  // UTC data and time string representations (ISO8601)

  sprintf(utci->Date,"%.4i-%.2i-%.2i",utci->Year,utci->Month,utci->Day);
  sprintf(utci->Time,"%.2i:%.2i:%06.3f",utci->Hour,utci->Min,utci->Sec);
  sprintf(utci->ISO,"%sT%s",utci->Date,utci->Time);

  // Decimal time values

  utci->dtime = double(utci->Hour) + ((double)(utci->Min)/60.0) + (utci->Sec/3600.0);

  utday = (double)(utci->Day) + (utci->dtime/24.0);

}
