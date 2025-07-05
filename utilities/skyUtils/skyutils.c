//**************************************************************************
//
// Routines for working with the Modified Julian Date
//
// MJD is the number of days elapsed since 1900 January 0.5
//
// To convert to standard Julian Date: jd = mjd + MDJ0
// where MJD0 = 2415020.0 = JD at 1900 Jan 0.5 (noon)
//
// To convert MJD to the standard form MJD(std) = JD - 2450000.5, you need
// to use
//        MJD(std) = mjd + MDJ0 - MJDF0
//
// MJD0 and MJD0F are defined in sky.h
//
// Routines:
//   CalToMJD() - convert UT calendar date to MJD
//   MJDToCal() - convert MJD to UT calendar date 
//   MJDToYear() - convert MJD to decimal UT year
//   YearToMJD() - convert decimal UT year to MJD
//   
//**************************************************************************/

#include <stdio.h>
#include <math.h>

#include "skyutils.h"   // sky time calculation stuff

//**************************************************************************
// 
// CalToMJD - convert UT calendar date to modified Julian Date
//
// given a UT date in months, days, & years, return the modified Julian date 
//
// calls no external routines
//

void 
CalToMJD (int mn, double dy, int yr, double *mjd)
{
  static double last_mjd, last_dy;
  static int last_mn, last_yr;
  int b, d, m, y;
  long c;
  
  if (mn == last_mn && yr == last_yr && dy == last_dy) {
    *mjd = last_mjd;
    return;
  }

  m = mn;
  y = (yr < 0) ? yr + 1 : yr;
  if (mn < 3) {
    m += 12;
    y -= 1;
  }

  if (yr < 1582 || (yr == 1582 && (mn < 10 || (mn == 10 && dy < 15))))
    b = 0;
  else {
    int a;
    a = y/100;
    b = 2 - a + a/4;
  }

  if (y < 0)
    c = (long)((365.25*y) - 0.75) - 694025L;
  else
    c = (long)(365.25*y) - 694025L;
  
  d = 30.6001*(m+1);

  *mjd = b + c + d + dy - 0.5;

  last_mn = mn;
  last_dy = dy;
  last_yr = yr;
  last_mjd = *mjd;
}

//**************************************************************************
//
// MJDToCal() - convert modified Julian Date to Calendar Date
// 
// given the modified Julian date, return the calendar date in months, days,
// and year
//

void 
MJDToCal (double mjd, int *mn, double *dy, int *yr)
{
  static double last_mjd, last_dy;
  static int last_mn, last_yr;
  double d, f;
  double i, a, b, ce, g;

  // MJD 0 = noon the last day of 1899.

  if (mjd == 0.0) {
    *mn = 12;
    *dy = 31.5;
    *yr = 1899;
    return;
  }
  
  if (mjd == last_mjd) {
    *mn = last_mn;
    *yr = last_yr;
    *dy = last_dy;
    return;
  }
  
  d = mjd + 0.5;
  i = floor(d);
  f = d-i;
  if (f == 1) {
    f = 0;
    i += 1;
  }

  if (i > -115860.0) {
    a = floor((i/36524.25)+.99835726)+14;
    i += 1 + a - floor(a/4.0);
  }

  b = floor((i/365.25)+.802601);
  ce = i - floor((365.25*b)+.750001)+416;
  g = floor(ce/30.6001);
  *mn = g - 1;
  *dy = ce - floor(30.6001*g)+f;
  *yr = b + 1899;

  if (g > 13.5)
    *mn = g - 13;
  if (*mn < 2.5)
    *yr = b + 1900;
  if (*yr < 1)
    *yr -= 1;

  last_mn = *mn;
  last_dy = *dy;
  last_yr = *yr;
  last_mjd = mjd;
}

//**************************************************************************
//
// MJDToYear - convert MJD to decimal UT year
//
// Calls MJDToCal() & CalToMJD() to do the dirty work
//

void 
MJDToYear (double mjd, double *yr)
{
  static double last_mjd, last_yr;
  int m, y;
  double d;
  double e0, e1;	// mjd of start of this year, start of next year

  if (mjd == last_mjd) {
    *yr = last_yr;
    return;
  }

  MJDToCal (mjd, &m, &d, &y);
  if (y == -1) y = -2;
  CalToMJD (1, 1.0, y, &e0);
  CalToMJD (1, 1.0, y+1, &e1);
  *yr = y + (mjd - e0)/(e1 - e0);
  
  last_mjd = mjd;
  last_yr = *yr;
}

//**************************************************************************
//
// YearToMJD() - Converts decimal UT year to MJD
//
// calls CalToMJD() to do the dirty work
// 

void 
YearToMJD (double y, double *mjd)
{
  double e0, e1;	// mjd of start of this year, start of next year
  int yf = floor (y);
  if (yf == -1) yf = -2;

  CalToMJD (1, 1.0, yf, &e0);
  CalToMJD (1, 1.0, yf+1, &e1);
  *mjd = e0 + (y - yf)*(e1-e0);
}

//**************************************************************************
// 
// JDtoHJD - return the Heliocentric Julian Date given geocentric JD and
//           object RA,Dec (J2000)
//
// Arguments:
//   jd (double) - geocentric julian date
//   dra (double) - object RA in decimal hours, equinox J2000
//   ddec (double) - object DEC in decimal degrees, equinox J2000
//
// HJD computes the difference in light travel time between the Earth
// and the Sun.  The correction (deltajd) is subtracted from the
// geocentric jd to give the heliocentric jd
//
// Based largely on algorithms from Duffet-Smith and the usual run
// of code that has been circulating around for years.  This one was
// attributed to "RLM", whoever that is.
//
// calls Precess()
//
//**************************************************************************/

double 
JDtoHJD (double jd, double dra, double ddec)
{
  double e;	       // obliquity of ecliptic
  double n;	       // day number
  double g;	       // solar mean anomaly
  double L;	       // solar ecliptic longitude
  double l;	       // mean solar ecliptic longitude
  double R;	       // sun distance, AU
  double X, Y;	       // equatorial rectangular solar coords
  double cdec, sdec;
  double cra, sra;
  double deltajd;      // HJD = JD - deltajd

  double ra, dec;    // RA, Dec in radians

  // convert RA and Dec to radians

  ra = hrrad(dra);
  dec = degrad(ddec);

  // precess from J2000 (input equinox) to the observation equinox,
  // where times are given in MJD

  Precess(J2000, jd - MJD0, &ra, &dec);

  // do it to it

  cdec = cos(dec);
  sdec = sin(dec);
  cra = cos(ra);
  sra = sin(ra);

  n = jd - 2451545.0;	// use epoch 2000
  e = degrad(23.439 - 0.0000004*n);
  g = degrad(357.528) + degrad(0.9856003)*n;
  L = degrad(280.461) + degrad(0.9856474)*n;
  l = L + degrad(1.915)*sin(g) + degrad(0.02)*sin(2.0*g);
  R = 1.00014 - 0.01671*cos(g) - 0.00014*cos(2.0*g);
  X = R*cos(l);
  Y = R*cos(e)*sin(l);

#if __DEBUGGER
  printf ("n=%g g=%g L=%g l=%g R=%g X=%g Y=%g\n",
	  n, raddeg(g), raddeg(L), raddeg(l), R, X, Y);
#endif
  
  deltajd = 0.0057755 * (cdec*cra*X + (cdec*sra + tan(e)*sdec)*Y);

  return(jd-deltajd);

}

//**************************************************************************
//
// precess() - precess coordinates between two MJD equinoxes 
//
// Based on hiprec_precess(), which is Copyright (c) 1990 by Craig 
// Counterman. All rights reserved.
//
// This software may be redistributed freely, not sold.
// This copyright notice and disclaimer of warranty must remain
//    unchanged. 
//
// No representation is made about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty, to the extent permitted by applicable law.
//
// Rigorous precession. From Astronomical Ephemeris 1989, p. B18
//
// 96-06-20 Hayo Hase <hase@wettzell.ifag.de>: theta_a corrected
//
// corrects ra and dec, both in radians, for precession from epoch 1 to
// epoch 2.  the epochs are given by their modified JDs, mjd1 and mjd2,
// respectively.  N.B. ra and dec are modifed IN PLACE. 
//
// Calls Range(), MJDToYear(), 


void 
Precess (double mjd1, double mjd2, double *ra, double *dec)
{
  static double last_mjd1 = -213.432, last_from;
  static double last_mjd2 = -213.432, last_to;
  double zeta_A, z_A, theta_A;
  double T;
  double A, B, C;
  double alpha, delta;
  double alpha_in, delta_in;
  double from_equinox, to_equinox;
  double alpha2000, delta2000;

  // convert mjds to years

  if (last_mjd1 == mjd1)
    from_equinox = last_from;
  else {
    MJDToYear(mjd1, &from_equinox);
    last_mjd1 = mjd1;
    last_from = from_equinox;
  }

  if (last_mjd2 == mjd2)
    to_equinox = last_to;
  else {
    MJDToYear(mjd2, &to_equinox);
    last_mjd2 = mjd2;
    last_to = to_equinox;
  }

  // convert coords in rads to degs

  alpha_in = raddeg(*ra);
  delta_in = raddeg(*dec);

  // precession progresses about 1 arc second in .047 years
  // From from_equinox to 2000.0

  if (fabs (from_equinox-2000.0) > .04) {

    T = (from_equinox - 2000.0)/100.0;
    zeta_A  = 0.6406161* T + 0.0000839* T*T + 0.0000050* T*T*T;
    z_A     = 0.6406161* T + 0.0003041* T*T + 0.0000051* T*T*T;
    theta_A = 0.5567530* T - 0.0001185* T*T - 0.0000116* T*T*T;
    
    A = DSIN(alpha_in - z_A) * DCOS(delta_in);
    B = DCOS(alpha_in - z_A) * DCOS(theta_A) * DCOS(delta_in)
                             + DSIN(theta_A) * DSIN(delta_in);
    C = -DCOS(alpha_in - z_A) * DSIN(theta_A) * DCOS(delta_in)
                              + DCOS(theta_A) * DSIN(delta_in);
    
    alpha2000 = DATAN2(A,B) - zeta_A;
    Range(&alpha2000, 360.0);
    delta2000 = DASIN(C);

  } 
  else {

    alpha2000 = alpha_in;
    delta2000 = delta_in;

  };

  // From 2000.0 to to_equinox

  if (fabs (to_equinox - 2000.0) > .04) {

    T = (to_equinox - 2000.0)/100.0;
    zeta_A  = 0.6406161* T + 0.0000839* T*T + 0.0000050* T*T*T;
    z_A     = 0.6406161* T + 0.0003041* T*T + 0.0000051* T*T*T;
    theta_A = 0.5567530* T - 0.0001185* T*T - 0.0000116* T*T*T;
    
    A = DSIN(alpha2000 + zeta_A) * DCOS(delta2000);
    B = DCOS(alpha2000 + zeta_A) * DCOS(theta_A) * DCOS(delta2000)
      - DSIN(theta_A) * DSIN(delta2000);
    C = DCOS(alpha2000 + zeta_A) * DSIN(theta_A) * DCOS(delta2000)
      + DCOS(theta_A) * DSIN(delta2000);
    
    alpha = DATAN2(A,B) + z_A;
    Range(&alpha, 360.0);
    delta = DASIN(C);

  } 
  else {

    alpha = alpha2000;
    delta = delta2000;

  };
  
  *ra = degrad(alpha);
  *dec = degrad(delta);

}

//**************************************************************************
//
// Range(v,r) - ensures that v is in the range 0 <= v < r.
//

void 
Range (double *v, double r)
{
  *v -= r*floor(*v/r);
}

//**************************************************************************
//
// Routines for working with Sideral Times
//
// Routines:
//   UTCtoGST() - convert UTC into Greenwich Mean Sidereal Time
//   Zenith() - compute zenith distance and sec(zd)
//
//**************************************************************************/

//**************************************************************************
//
// UTCtoGST() - Given the modified julian date @ 0h UTC and the UTC time,
//             returns the greenwich mean sidereal time.
//
// arguments: 
//   mjd0 (input, double): MJD at 0h UTC [decimal days]
//   utc (input, double): UTC time [decimal hours]
//   gmst (output, double): Greenwich Mean Sidereal Time [decimal hours]
//

void 
UTCtoGST (double mjd0, double utc, double *gmst)
{
  double T, gmst0;

  T = ((int)(mjd0 - 0.5) + 0.5 - J2000)/36525.0;
  gmst0 = 24110.54841 + (8640184.812866 + (0.093104 - 6.2e-6*T)*T)*T ;
  gmst0 /= 3600.0;
  Range(&gmst0, 24.0);

  *gmst = (1.0/SIDRATE)*utc + gmst0;
  Range(gmst, 24.0);
}

//**************************************************************************
//
// Zenith() - given the HA and Declination of an object for a given
//            latitude, returns the zenith distance and secz
//
// Arguments:
//    ha (input, double): Hour Angle [decimal hours]
//   dec (input, double): Declination [decimal degrees]
//   lat (input, double): Latitude [decimal degrees]
//    zd (output, double): Zenith Distance = 90-Altitude [decimal degrees]
//  secz (output, double): Secant(ZD) (dimensionless)
//
// R. Pogge, OSU Astronomy Dept
// pogge@astronomy.ohio-state.edu
// 2000 Jan 29
//

void 
Zenith(double ha, double dec, double lat, double *zd, double *secz)
{
  double coszd;
  
  coszd = DSIN(lat)*DSIN(dec) + DCOS(lat)*DCOS(dec)*DCOS(hrdeg(ha));

  // if coszd is <= 0, set airmass to 99.9 to signal

  if (coszd <= 0.0) {
    *secz = 99.9;
    *zd = raddeg(acos(coszd)) ;
  } else {
    *secz = 1.0/coszd ;
    *zd = raddeg(acos(coszd)) ;
  }

}

/*!  

  \brief Compute Azimuth and Elevation (horizontal coordinates) given
  celestial coordinates HA and Dec and the site latitude.
  
  \param lat site latitude in decimal degrees
  \param ha target hour angle in decimal hours
  \param dec target celestial declination in decimal degrees
  \param az target azimuth in decimal degrees (-999.9 if at zenith)
  \param el target elevation in decimal degrees
  \param par parallactic angle in decimal degrees

  Azimuth is defined in the traditional Astronomical way such that
  azimuth increases as objects rise in the East and set in the West
  (i.e., clockwise from North through East).

  Also returns the elevation (sometimes called "altitude") and the
  parallactic angle.

*/

void
AzEl(double lat, double ha, double dec,
     double *az, double *el, double *par)
{
  double cosaz;
  double sinel;
  double sinpar;

  sinel = DSIN(lat)*DSIN(dec) + DCOS(lat)*DCOS(dec)*DCOS(hrdeg(ha));
  *el = raddeg(asin(sinel));

  if (*el == 90 || *el == -90) {
    *az = -999.99;  // undefined at zenith
    *par = -999.99; 
  }
  else {
    cosaz = DCOS(lat)*DSIN(dec) - DSIN(lat)*DCOS(dec)*DCOS(hrdeg(ha));
    cosaz = cosaz/DCOS(*el);
    *az = raddeg(acos(cosaz));
    if (ha>0) 
      *az = 360 - *az;  // ensure proper 0-360 value given acos() limits

    sinpar = DCOS(lat)*DSIN(hrdeg(ha))/DCOS(*el);
    *par = raddeg(asin(sinpar));
  }
  
}
