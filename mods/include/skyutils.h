#ifndef SKYUTILS_H
#define SKYUTILS_H

//**************************************************************************
//
// skyutils.h - various bits for doing astronomical time calculations
//
// derived from a number or sources, especially Duffet-Smith and
// various and sundry bits of software gleaned from many sources
// over the years of very scrambled pedigree.
//
//**************************************************************************/

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

// We need PI, make sure it is defined

#ifndef PI
#define PI              3.141592653589793
#endif

// conversions among hours (of ra), degrees and radians.

#define degrad(x)       ((x)*PI/180.)
#define raddeg(x)       ((x)*180./PI)
#define hrdeg(x)        ((x)*15.)
#define deghr(x)        ((x)/15.)
#define hrrad(x)        degrad(hrdeg(x))
#define radhr(x)        deghr(raddeg(x))

// convenient forms of trig functions for degree arguments

#define	DCOS(x)		cos(degrad(x))
#define	DSIN(x)		sin(degrad(x))
#define	DASIN(x)	raddeg(asin(x))
#define	DATAN2(y,x)	raddeg(atan2((y),(x)))

// starting point for MJD calculations

#define MJD0  2415020.0               // 1900 Jan 0.5 (noon)  
#define J2000 (2451545.0 - MJD0)      // let compiler optimise
#define MJDF0 2400000.5               // Recommended FITS/OGIP zero

// other stuff

#define SIDRATE 0.9972695677    // ratio of synodic to sidereal rate
#define SPD     (24.0*3600.0)   // seconds per day

// Function Prototypes

void CalToMJD (int mn, double dy, int yr, double *mjd);
void MJDToCal (double mjd, int *mn, double *dy, int *yr);
void MJDToYear (double mjd, double *yr);
void YearToMJD (double yr, double *mjd);
void range (double *v, double r);
void UTCtoGST(double mjd0, double utc, double *gmst);
void Zenith (double ha, double dec, double lat, double *zd, double *secz);
void Precess (double mjd1, double mjd2, double *ra, double *dec);
void Range (double *v, double r);
double JDtoHJD (double jd, double dra, double ddec);
void SexStr(double x, char *tstr);
void AzEl(double, double, double, double *, double *, double *);

#endif // SKYUTILS_H
