#ifndef MODSAGWTC_H
#define MODSAGWTC_H

#include "../include/modscontrol.h"

#define ERROR_UNKNOWN_MODSID   215 //!< User requested an unknown MODS instance (must be MODS1 or MODS2)

#define MODS1 0  // AGw unit ID number for MODS1
#define MODS2 1  // AGw unit ID number for MODS2

//
// MODS AGw stage coordinate transformation code
//
// These routines illustrate the coordinate transformations for the
// MODs AGw system, but are only code stubs meant to show the core
// functions, without following either the strict prototypes or
// including robust error trapping.
//
// R. Pogge, OSU Astronomy Dept.
// Update: 2010 Aug 6
// 2010 Aug 10 [rdg]
//
// Coordinate transform coefficients (defined in the relevant .h file)
// SFP X to GP X transformation
// Default SFPtoAGW transformation coefficients

double sfp_x0[2] = {0.0,0.0};
double sfp_xx[2] = {-1.0,0.0}; // SFP X is a mirror flip of AGw Xgp
double sfp_xy[2] = {0.0,0.0};

// SFP Y to GP Y transformation

double sfp_y0[2] = {0.0,0.0};
double sfp_yx[2] = {0.0,0.0};
double sfp_yy[2] = {1.0,1.0}; // SFP Y is same as AGw Ygp

// GP X to SFP X transformation
// Default AGWtoSFP transformation coefficients

double gp_x0[2] = {0.0,0.0};
double gp_xx[2] = {-1.0,-1.0}; // AGw Xgp is a mirror flip of SFP X
double gp_xy[2] = {0.0,0.0};

// GP Y to SFP Y transformation

double gp_y0[2] = {0.0,0.0};
double gp_yx[2] = {0.0,0.0};
double gp_yy[2] = {0.0,0.0}; // AGw Ygp is the same as SFP Y

// Default CCDtoAGW offset transformation coefficients
//
// Assumptions: guide camera CCD pixel scale: 0.11 arcsec/pixel
//              LBT f/15 image scale: 0.6 mm/arcsec
// CCD dX to AGw GP dX offset transformation

double ccd_xx[2] = {0.066,0.066}; // units: mm/pixel 
double ccd_xy[2] = {0.0,0.0};

// CCD dY to AGw GP dY offset transformation

double ccd_yx[2] = {0.0,0.0};
double ccd_yy[2] = {0.066,0.066}; // units: mm/pixel

int mods_initTrans(char *, int);
int mods_inittrans(char *, int, struct sac_xy *);
int mods_SFPtoAGW(int, double, double, double *,double *);
int mods_AGWtoSFP(int, double, double, double *,double *);
int mods_CCDtoAGW(int, double, double, double *,double *);
int mods_AGW2SFP(int, double, double, tc *xy, int);

#endif // MOSDAGWTC_H

