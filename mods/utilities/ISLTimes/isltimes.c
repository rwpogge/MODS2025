#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <math.h>
#include <ctype.h>
//#include "usrTime.h"
#include "MATHCNST.H"

//char *getTimeStamp(int,char []);
void getUT(int *,int *,int *,int *,int *,int *);
void getvtxUT(int *,int *,int *,int *,int *,int *,float);
void calcLST(double , double , double ,double* , double* );
double dateToJD(int,int,int);
int daySecChange(double *, double *);
void calcGMSTD (double,double,double *,double *);
void secToTime(double ,char *,short);
int checkTime(int *, int *, int *,short);
char *timeFormat(int,int,int,short,char *);
int dateToDay(int,int,int);
int toLeap(int);
int itoa(int ,char []);
void calcAzEl (double,double,double,double,double *,double *);
void calcRaDec (double,double,double,double,double *,double *);
void isl_LST(double *);
void isl_LST_w_vtxTime(double *, double );
void isl_ModifiedJulianDay(long *);
void isl_DayOfYear(long *);
void isl_UTC(double *);

char *months[] = {"Jan","Feb","Mar","Apr","May",
		  "Jun","Jul","Aug","Sep","Oct","Nov","Dec"};

/* Calculate the LST time
GSD:      The current Greenwich Sidreal Day number
GMST:     The current Greenwich Mean Sidreal Time(in secs)
LongSec:  The locations longitude converted to second of 
          time (- for West, + for East)
*OutDay:  Where to leave the LST day number (at 0h LST)
*OutTime: Where to leave the LST in seconds
*/
void
calcLST(double GSD, double GMST, double LongSec,
	double *OutDay, double *OutTime)
{
  
  /* Declarations:
   *   LST....the Local Sidreal Time
   */
  double LST;
  
  
  LST= GMST+LongSec;
  while (LST> 86400.0)
    LST-= 86400.0;
  while (LST< 0.0)
    LST+= 86400.0;
  *OutTime= LST;
  
  /*
   * Figure out whether the station is on the same, or the previous sidreal day
   * as Greenwich and fix the Local Sidreal Day number. (This all assumes the
   * location is West of Greenwich)
   */
  if (LongSec< 0)
    *OutDay= GSD-daySecChange(&LST, &GMST);
  else
    *OutDay= GSD+daySecChange(&GMST, &LST);
}

/*
 * Returns the current UTC time according to the UNIX clock.
 *Y:   returned year, e.g. 1993
 *Mo:  returned month (1-12)
 *D:   returned day (1-31)
 *H:   returned hour (0-23)
 *Mi:  returned minute (0-59)
 *S:   returned hour (0-59)
 */
void getUT (long *Y,long *Mo,long *D,long *H,long *Mi,long *S)
{
  time_t Time;    /* time returned by time() (time_t = long) */
  struct tm *pTM; /* time returned by dmtime() */

  /* get time in seconds since 1970JAN01 00:00:00 GMT */
  time (&Time);
  
  /* use time in seconds to file struct tm */
  pTM = gmtime (&Time);
  
  /* get the stuff from the struct */
  *Y = pTM->tm_year + 1900;
  *Mo = pTM->tm_mon + 1;
  *D = pTM->tm_mday;
  
  *H = pTM->tm_hour;
  *Mi = pTM->tm_min;
  *S = pTM->tm_sec;
}

/*
 * Returns the current UTC time according to the ScramNet secs for time
 * and Unix clock for date.
     int *Y;
     int *Mo;
     int *D;
     int *H;
     int *Mi;
     int *S;
     float ScramNet_secs;
 */
/*
 * Returns the current UTC time according to the ScramNet secs for time
 * UNIX clock.
 *Y:   returned year, e.g. 1993
 *Mo:  returned month (1-12)
 *D:   returned day (1-31)
 *H:   returned hour (0-23)
 *Mi:  returned minute (0-59)
 *S:   returned hour (0-59)
 ScramNet_secs:
 */
void getvtxUT (long *Y,long *Mo,long *D,
	       long *H,long *Mi,long *S,float ScramNet_secs)
{
  time_t Time;        /* time returned by time() (time_t = long) */
  struct tm *pTM;     /* time returned by dmtime() */

  /* get time in seconds since 1970JAN01 00:00:00 GMT */
  time (&Time);
  
  /* use time in seconds to file struct tm */
  pTM = gmtime (&Time);
  
  /* get the stuff from the struct */
  *Y = pTM->tm_year + 1900;
  *Mo = pTM->tm_mon + 1;
  *D = pTM->tm_mday;
  
  ScramNet_secs/= 3600.0;
  *H = (int)ScramNet_secs;
  ScramNet_secs-= (double)*H;

  *H+= 4;
  if(*H >= 24) *H-= 24;
  
  ScramNet_secs*= 60.0;
  *Mi = (int)ScramNet_secs;
  ScramNet_secs-= (double)*Mi;

  ScramNet_secs*= 60.0;
  *S = (int)ScramNet_secs;

  /*
  *H = pTM->tm_hour;
  *Mi = pTM->tm_min;
  *S = pTM->tm_sec;
  */
}

double dateToJD(int Year, int Month, int Day)
{
  double JD;             /* it  */
  int J1;                /* an intermediate value */
  short Calendar= 1;     /* 0=Julian calendar, 1=Gregorian calendar */
  
  
  /* Determine if the date is Gregorian or Julian */
  if (Year< 1583)
    {
      if (Year< 1582)
	Calendar= 0;       else
	  if (Month< 10)
	    Calendar= 0;
	  else
	    if (Month== 10 && Day< 15)
	      Calendar= 0;
    }
  
  /* Here we go! */
  JD= -7*((int)((Month+9)/12)+Year)/4;
  
  /* Do this additional stuff only if the date is Gregorian */
  if (Calendar== 1)
    {
      if ((Month-9)< 0)
	J1= Year+(-abs((Month-9)/7));
      else
	J1= Year+abs((Month-9)/7);
      J1= -(((int)(J1/100)+1)*3/4);
    }
  
  JD+= (int)(275*Month/9)+Day+Calendar*J1;
  JD+= 1721027.0+2.0*Calendar+367.0*Year;
  JD-= .5;
  
  return (JD);
}

/* 
 *   After being passed two times the routine returns a 0 if it thinks the two
 *   times are in the same 24hr day, and 1 if time2 is in the next day.
 *Time1:    the base time
 *Time2:    the time to be compared
 */
int daySecChange(double *Time1, double *Time2)
{
  
  /* Do the comparisons and return the results */
  if (*Time2>= *Time1)
    return(0);
  else
    return(1);
}

/*++*************************************************************************
 Calculates the Greenwich Mean Sidereal Time (GMST) and the Greenwich
 Sidereal Day number (GSD) given the current Julian Date/Time.

JD0:         Julian Date at the 0h UT
UTSec:       number of seconds into the current UT day
*GSD:        where to leave the GSD
*GMST:       where to leave the GMST (in seconds)
*/
void calcGMSTD (double JD0,double UTSec,double *GSD,double *GMST)
{
  /* difference between UT midnight on JD 0, and the LST */
  #define UTLSTOFFSET 0.671104

  /* ratio of the length of a solar day to a sidreal day */
#define SOLSIDRATIO 1.00273790934
  
  double Tu;/* interval of time (in Julian centuries)
	       since or before 20000101.1200 */
  double Tu2;/* Tu squared */
  double Tu3;/* Tu cubed */
  double GMSTp;/* GMST variable for the program */
  double GSDp;/* GSD variable for the program */
  
  Tu = (JD0 - 2451545.0) / 36525;
  Tu2 = Tu * Tu;
  Tu3 = Tu * Tu * Tu;
  
  /* Calculate the Greenwich Mean Sidreal Time at 0000h UT */
  GMSTp = 24110.54841 + 8640184.812866 * Tu
    + 0.093104 * Tu2 - 0.0000062 * Tu3;
  while (GMSTp < 0)
    GMSTp += 86400;
  while (GMSTp > 86400)
    GMSTp -= 86400;
  
  /* Now calculate the Greenwich Mean Sidreal Time at the target UT time */
  GMSTp = GMSTp + (double)(UTSec * SOLSIDRATIO);
  while (GMSTp < 0)
    GMSTp += 86400;
  while (GMSTp > 86400)
    GMSTp -= 86400;
  
  /* Calculate the Greenwich Sidreal Day number at the target time */
  JD0 += UTSec / 86400.0;
  GSDp = JD0 * SOLSIDRATIO + UTLSTOFFSET;
  
  *GMST = GMSTp;
  *GSD = GSDp;
}

/*   Converts an amount of seconds to HH:MM:SS. Rounds to the nearest second.
 *   Don't pass too many seconds with a Format of 1 due to the size limits on
 *   int's.
 Seconds;    the number of seconds
 *OutTime;     where to put the output
 Format;      0=adjust to clock time, 1=don't
 */
void secToTime(double Seconds,char *OutTime,short Format)
{
  int Hours;
  int Mins;
  int Secs;
  
  /* Adjust for a negative number of seconds - not allowed */
  while (Seconds< 0)
    Seconds+= 86400.0;
  
  if (Format== 0)
    {
      while (Seconds> 86400.0)
	Seconds-= 86400.0;
    }
  Seconds/= 3600.0;
  Hours= (int)Seconds;
  Seconds-= (double)Hours;
  
  Seconds*= 60.0;
  Mins= (int)Seconds;
  Seconds-= (double)Mins;
  
  Seconds*= 60.0;
  Seconds+= .5;
  Secs= (int)Seconds;
  
  (void)checkTime(&Hours, &Mins, &Secs, 0);
  (void)timeFormat(Hours, Mins, Secs, 0, OutTime);
}

/*   Accepts H M and S and does clock math (ie 90sec's becomes 1min 30sec...)
 *   and checks for 24:00:00 limit if Limit=0. Returns 0 if everything is OK,
 *   1 if not.
 Limit:    0 = 24hr limit, 1 = no limit
 */
int checkTime(int *Hr, int *Min, int *Sec,short Limit)
{
  
  /* No negative numbers allowed here */
  if (*Hr< 0 || *Min< 0 || *Sec< 0)
    return(1);
  
  /* Distribute the wealth */
  while (*Sec> 59)
    {
      *Min+= 1;
      *Sec-= 60;
    }
  while (*Min> 59)
    {
      *Hr+= 1;
      *Min-= 60;
    }
  if (Limit== 0)
    {
      if (*Hr> 24 || (*Hr== 24 && (*Min> 0 || *Sec> 0)))
	return(1);
    }
  
  return(0);
}

/*
 *   Accepts H, M and S and returns one of the following formats:
 *   0 = HH:MM:SS   1 = HHhMMmSSs   2 = HHMMSS  3 = HH:MMam/pm
 *   4 = HH:MM:SSam/pm   5=HH:MM
 *   OutTime should be large enough to hold the formatted time.
 *   Returns a pointer to OutDate.
 H, M, S;    the incoming time parameter
 Format;   the desired format
 *OutTime;  the finished product
 */
char *timeFormat(int H,int M,int S,short Format,char *OutTime)
{

  if (Format== 0)
    (void)sprintf(OutTime, "%02d:%02d:%02d", H, M, S);
  else if (Format== 1)
    (void)sprintf(OutTime, "%02dh%02dm%02ds", H, M, S);
  else if (Format== 2)
    (void)sprintf(OutTime, "%02d%02d%02d", H, M, S);
  else if (Format== 3)
    {
      if (H== 0)
	(void)sprintf(OutTime, "12:%02dam", M);
      else if (H< 12)
	(void)sprintf(OutTime, "%2d:%02dam", H, M);
      else if (H> 12)
	(void)sprintf(OutTime, "%2d:%02dpm", H-12, M);
      else
	(void)sprintf(OutTime, "12:%02dpm", M);
    }
  else if (Format== 4)
    {
      if (H== 0)
	(void)sprintf(OutTime, "12:%02d:%02dam", M, S);
      else if (H< 12)
	(void)sprintf(OutTime, "%2d:%02d:%02dam", H, M, S);
      else if (H> 12)
	(void)sprintf(OutTime, "%2d:%02d:%02dpm", H-12, M, S);
      else
	(void)sprintf(OutTime, "12:%02d:%02dpm", M, S);
    }
  else if (Format== 5)
    (void)sprintf(OutTime, "%02d:%02d", H, M);
  
  return(OutTime);
}

/*
 *   Given the Year, Month and Day the function returns the day number
 *   (1-366).
 Year,Month,Day..pointers to those things
 Year:
 Month:
 Day:
 */
int
dateToDay(int Year,int Month,int Day)
{
  
  /* Declarations:
   *   Leap..0=not a leap year, 1=is a leap year
   */
  short Leap;

  Leap = toLeap(Year);
  
  switch(Month)
    {
    case 1:
      return (Day);
    case 2:
      return (Day+31);
    case 3:
      return (Day+59+Leap);
    case 4:
      return (Day+90+Leap);
    case 5:
      return (Day+120+Leap);
    case 6:
      return (Day+151+Leap);
    case 7:
      return (Day+181+Leap);
    case 8:
      return (Day+212+Leap);
    case 9:
      return (Day+243+Leap);
    case 10:
      return (Day+273+Leap);
    case 11:
      return (Day+304+Leap);
    case 12:
      return (Day+334+Leap);
    default:
      return (0);
    }
}

/*
 * Accepts the year (Year) and returns 0 if it is not a leap year, 1 if it
 * is.
 */
int
toLeap(int Year)
{
  
  /* It is a leap year if... */
  if ((Year%4== 0 && Year%100!= 0) || Year%400== 0)
    return(1);
  else
    return(0);
}

/* stamp_or_time = 0: return a timestamp (example: Fri Jan 01 00:00:00 1996) */
/* stamp_or_time = 1: return time (example: 00:00:00) */ 
/* stamp_or_time = 2: return date (example: 1996JAN22) */ 
/* stamp_or_time = 3: return utc time (example: 00:00:00) */ 
/* stamp_or_time = 4: return YYMMDD time (example: 960122)
char
*getTimeStamp(int stamp_or_time,char timestamp[])
{
  int i, j, ret;
  char *ctime(long *), year[4], month[3], day[2], utchar[2];
  //  char year[4], month[3], day[2], utchar[2];
  char *today, *return_it, buf[40];
  long thetime;
  time_t time(long);

  thetime = time(NULL);

  * today is returned in this format 'Fri Sep  6 14:22:22 1996' *
  today = ctime(&thetime);

  return_it = "ERROR\0";

  
  for (i = 0; i <= 23; i++)
    timestamp[i] = *today++;
  timestamp[i] = '\0';

  switch(stamp_or_time)
    {
    case 0:
      return_it = "OK\0";
      break;
    case 1:
      for (i = 0; i <= 7; i++)
	{
	  timestamp[i] = timestamp[i + 11];
	}
      timestamp[i] = '\0';
      return_it = "OK\0";
      break;
    case 2:
      * 1996SEP01 *
	* month *
      for (i = 4; i < 7; i++)
	timestamp[i] = toupper(timestamp[i]);
	* day *
      for (i = 7; i < 9; i++)
	{
	  if(timestamp[i+1] == ' ') timestamp[i] = '0';
	  else timestamp[i] = timestamp[i+1];
	}
	* year *
      for (i = 0; i < 4; i++)
	timestamp[i] = timestamp[i+20];
      timestamp[9] = '\0';
      return_it = "OK\0";
      break;
    case 3:
      utchar[0] = timestamp[11];
      utchar[1] = timestamp[12];
      j = atoi(utchar) + 4;
      if(j >= 24) j -= 24;
      itoa(j, utchar);
      timestamp[0] = utchar[0];
      timestamp[1] = utchar[1];
      for (i = 2; i <= 7; i++)
	{
	  timestamp[i] = timestamp[i + 11];
	}
      timestamp[i] = '\0';
      return_it = "OK\0";
      break;
    case 4:
      month[0] = timestamp[4];
      month[1] = timestamp[5];
      month[2] = timestamp[6];
      * 960122 *
	* month *
      for(i = 0; i < 12; i++)
	{
	  for(j=0; month[j] == months[i][j] ;j++)
	    {
	      if(j==2)
		{
		  j = 3;
		  break;
		}
	    }
	  if(j==3) break;
	}
      i++;
      itoa(i,month);
      if(i < 10)
	{
	  timestamp[4] = '0';
	  timestamp[5] = month[0];
	}
      else
	{
	  timestamp[4] = month[0];
	  timestamp[5] = month[1];
	}
	* day *
      if(timestamp[8] == ' ')
	{
	  timestamp[6] = '0';
	  timestamp[7] = timestamp[9];
	}
      else
	{
	  timestamp[6] = timestamp[8];
	  timestamp[7] = timestamp[9];
	}
	* year *
      for (i = 0; i < 4; i++)
	timestamp[i] = timestamp[i+20];
      timestamp[8] = '\0';
      return_it = "OK\0";
      break;
    default:
      break;
    }
  return(return_it);
}
 */ 
/* */
/* convert integer to ascii string */
/* ********** */
int itoa(int n,char s[])
{
  int i, j, c;
  int sign;
  /* this has to be done in reverse order */
  if(((sign=n) < 0)) n = -n;
  i = 0;
  do 
    {
    s[i++] = n % 10 + '0';
  }
  while ((n /= 10) > 0);
  if(sign<0)  s[i++]='-';
  s[i]='\0';
  
  /* correct for reverse order */
  for(i=0, j=strlen(s)-1; i<j; i++, j--)
    { 
      c=s[i];
      s[i]=s[j];
      s[j]=c;
    }
}

/*++**************************************************************************
 * EXAMPLE:

  double ra = 18.539112;
  double dec = 23.219492;
  double lat = 52.00;
  double lst = 0.401389;

  calcAzEl(lst, lat, ra, dec, &az, &el);

LST; local sidreal time in HH.hhh format
LAT; latitude in DD.ddd format
RA;  right ascenion in HH.hhh format
DEC; declination in DD.ddd format
*AZ; where to return azimuth in DD.ddd format
*EL; where to return elevation in DD.ddd format
*/
void calcAzEl (double LST,double LAT,double RA,double DEC,
	       double *AZ,double *EL)
/*
* Calculates the AZ and EL of an object. Uses no global variables.
-*/
{
   double T;	/* results of LST-RA calculations */
   double Z;	/* used in the EL calculations */

   /* Crunch the LST stuff */
   T = LST - RA;
   if (fabs (T) > 12.0)
       {
       if (LST > 12.0)
	   T = ((24.0 - LST) + RA) * (-1);
       else
	   T = (24.0 - RA) + LST;
       }
 
   T = T * 15.0 / DEG_RAD;
   LAT = LAT / DEG_RAD;
   DEC = DEC / DEG_RAD;


   /* Crunch those numbers */
   Z = sin (LAT) * sin (DEC) + cos (LAT) * cos (DEC) * cos (T);
   *EL = atan (Z / sqrt (1.0 - Z * Z)) * DEG_RAD;

   *AZ = atan ((cos (DEC) * sin (T)) / (-cos (LAT) * sin (DEC) + sin (LAT) 
					* cos (DEC) * cos (T))) * DEG_RAD;

   /* Generate the true azimuth */
   if (T < 0)
     {
     if (*AZ < 0)
         *AZ = 180 + *AZ;
     }
   else
       {
       if (*AZ >= 0)
	   *AZ = 180 + *AZ;
       else
	   *AZ = 360 + *AZ;
       }

}

/*++**************************************************************************
 * EXAMPLE:

  double az = 283.271667;
  double el = 19.333889;
  double lat = 52.00;
  double lst = 0.401389;

  calcRaDec(lst, lat, az, el, &ra, &dec);
LST; local sidreal time in HH.hhh format
LAT; latitude in DD.ddd format
RA;  right ascenion in HH.hhh format
DEC; declination in DD.ddd format
*AZ; where to return azimuth in DD.ddd format
*EL; where to return elevation in DD.ddd format
*/
void calcRaDec (double LST,double LAT,double RA,double DEC,
		double *AZ,double *EL)
     /*
      * Calculates the RA and DEC of an object. Uses no global variables.
      -*/
{
  double T;    /* results of LST-RA calculations */
  double Z;    /* used in the EL calculations */

  if(RA >= 360.0) RA = RA - 360.0;

  /* convert to radians */
  T = RA / DEG_RAD;
  LAT = LAT / DEG_RAD;
  DEC = 90.0 - DEC;
  DEC = DEC / DEG_RAD;
  
  
  /* Crunch those numbers */
  Z = sin (LAT) * sin (DEC) + cos (LAT) * cos (DEC) * cos (T);

  /* invert and convert to degrees */
  *EL = atan (Z / sqrt (1.0 - Z * Z)) * DEG_RAD;
  
  *AZ = atan ((cos (DEC) * sin (T)) / (-cos (LAT) * sin (DEC) + sin (LAT)
				       * cos (DEC) * cos (T))) * DEG_RAD;
  
  RA = sin(RA / DEG_RAD);
  
  
  /* Generate the true Hour Angle */
  if (RA < 0) 
    {
      *AZ = *AZ;
    }
  else
    {
      *AZ = 360.0 - *AZ;
    }
  /* Generate RA and convert the Hour Angle to time */

  *AZ = LST - *AZ/15.0;

  if(*AZ < 0)
    *AZ = *AZ + 24.0;
}

void
isl_LST(double *LST)
{
  long yr, mon, day, hrs, mins, secs;
  double jd, lst, longit = 76.8265, utsec, GSD, GMST, lstDay, lstSecs;
  /* double longit = -16020.793 * AO longi */
  
  getUT(&yr, &mon, &day, &hrs, &mins, &secs);
  utsec = hrs * 3600.0 + mins * 60.0 + secs;
  jd = dateToJD(yr,mon,day);
  calcGMSTD (jd, utsec, &GSD, &GMST);
  calcLST(GSD, GMST, longit, &lstDay, &lstSecs);
  *LST = lstSecs/3600.00;
}

void
isl_LST_w_vtxTime(double *LST, double vtx_time)
{
  long yr, mon, day, hrs, mins, secs;
  /* double jd, lst, longit = -16020.793, GSD, GMST, lstDay, lstSecs;*/
  double jd, lst, longit = 76.8265, GSD, GMST, lstDay, lstSecs;
  double utsec;

  getvtxUT(&yr, &mon, &day, &hrs, &mins, &secs, vtx_time);
  utsec = hrs * 3600.0 + mins * 60.0 + secs;

  jd = dateToJD(yr,mon,day);
  calcGMSTD (jd, utsec, &GSD, &GMST);
  calcLST(GSD, GMST, longit, &lstDay, &lstSecs);
  *LST = lstSecs/3600.00;
}

void
isl_UTC(double *UTC)
{

  long yr, mon, day, hrs, mins, secs;

  getUT(&yr, &mon, &day, &hrs, &mins, &secs);
  *UTC = hrs + mins/60.00 + secs/3600.0;
}

void
isl_ModifiedJulianDay(long *MODJULDAY)
{
  long yr, mon, day, hrs, mins, secs, jd_int;
  double jd, utsec;
  
  getUT(&yr, &mon, &day, &hrs, &mins, &secs);
  utsec = hrs * 3600.0 + mins * 60.0 + secs;
  jd = dateToJD(yr,mon,day);
  utsec = jd - 2400000.5;
  *MODJULDAY = (long)(utsec * 1);
}
/*
void
isl_DayOfYear(long *DAYOFYEAR)
{
  long yr, mon, day, day_of_year;
  char calday[20];
  char aodate[10];
  char *timetag;
  
  timetag = getTimeStamp(4, aodate);
  strncpy(calday, aodate, 4);
  yr = atoi(calday);
  strncpy(calday, "    ", 4);
  strncpy(calday, aodate + 4, 2);
  mon = atoi(calday);
  strncpy(calday, "    ", 4);
  strncpy(calday, aodate + 6, 2);
  day = atoi(calday);
  *DAYOFYEAR = dateToDay(yr, mon, day);
}
*/
