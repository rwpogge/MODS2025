//---------------------------------------------------------------------------
//
// SexStr(x,str) - convert a decimal angular coordinate into sexigesimal
//
// output is coded into sexstr as -dd:mm:ss.sss
//
// R. Pogge, OSU Astronomy Dept.
// 1998 July 10
//
// 2010 Sept 16 - Increased precision on the output seconds to 0.001
//

#include "skyutils.h"

void 
SexStr(double x, char *tstr)
{
  int dd;
  int dm;
  double sec;
  double tmp;
  int isigned;

  if (x == 0.0) {
    strcpy(tstr,"00:00:00.000");
    return;
  }

  if (x < 0.0) {
    isigned = 1;
    x = fabs(x);
  } else {
    isigned = 0;
  }

  dd = (int)(x);
  tmp = (x - (double)(dd)) * 60.0;
  dm = (int)(tmp);
  tmp = (double)(dd) + (double)(dm)/60.0;
  sec = 3600.0*(x-tmp);
  
  // For certain combinations of numbers, we can end up with
  // sec>=60.0.  Fix those rare instances

  if (sec >= 59.999) {
    sec = 0.0;
    dm = dm + 1;
  }

  if (dm >= 60) {
    dm = 0;
    dd = dd + 1;
  }
    
  // Build the output string

  if (isigned == 1) {
    sprintf(tstr,"-%.2i:%.2i:%06.3f",dd,dm,sec);
  } else {
    sprintf(tstr,"%.2i:%.2i:%06.3f",dd,dm,sec);
  }

}

