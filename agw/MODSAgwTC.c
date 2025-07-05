//
// MODS AGw stage coordinate transformation code stubs
//
// These routines illustrate the coordinate transformations for the
// MODs AGw system, but are only code stubs meant to show the core
// functions, without following either the strict prototypes or
// including robust error trapping.
//
// R. Pogge, OSU Astronomy Dept.
// dates: 2010 Aug 6 [rwp/rdg] First instance was sudo code
// dates: 2010 Aug 10 [rdg] added sudo code to app library in C/C++
// dates: 2010 Sept 10 [rwp] added sudo code for CCDToSFP
// dates: 2010 Sept 13 [rdg] added CCDToSFP sudo code to app library in C/C++
//
#include <stdio.h>
#include <stdlib.h>
#include "../include/modscontrol.h"

#define MODS1 0
#define MODS2 1

sac_xy xy;

int mods_initTrans(char *host, int agw_no)
{
  int ierr;
  ierr=initTrans(host,agw_no, &xy);
  return ierr;
}
//----------------------------------------------------------------
//
// SFPtoAGW() - convert SFP (x,y) to AGw Stage guide probe (x,y)
//
// Given standard focal plane coordinates (xsfp,ysfp) return AGw stage
// guide probe coordinates (xgp,ygp)
//
int
mods_SFPToAGW(int agw_no, double xsfp, double ysfp, double *xgp, double *ygp, int options)
{
  if (agw_no != MODS1 && agw_no != MODS2) {
    return (115);  // No such instrument available
  }

  //  printf("SFP to AGW:\n%0.4f,%0.4f,%0.4f\n%0.4f,%0.4f,%0.4f\n",
  //	 xy.sfp_x0[agw_no], xy.sfp_xx[agw_no], xy.sfp_xy[agw_no],
  //	 xy.sfp_y0[agw_no], xy.sfp_yx[agw_no], xy.sfp_yy[agw_no]);

  *xgp = xy.sfp_x0[agw_no] + xy.sfp_xx[agw_no]*xsfp + xy.sfp_xy[agw_no]*ysfp;
  *ygp = xy.sfp_y0[agw_no] + xy.sfp_yx[agw_no]*xsfp + xy.sfp_yy[agw_no]*ysfp;

  return 0;
}


//----------------------------------------------------------------
//
// AGWtoSFP() - convert AGw Stage guide probe (x,y) to SFP (x,y)
//
// Given AGw stage guide probe coordinates (xgp,ygp) return standard
// focal plane coordinates (xsfp,ysfp)
//

int 
mods_AGWToSFP(int agw_no, double xgp, double ygp, double *xsfp, double *ysfp, int options)
{
  if (agw_no != MODS1 && agw_no != MODS2) {
    return (115);  // No such instrument available
  }
  *xsfp = xy.gp_x0[agw_no] + xy.gp_xx[agw_no]*xgp + xy.gp_xy[agw_no]*ygp;
  *ysfp = xy.gp_y0[agw_no] + xy.gp_yx[agw_no]*xgp + xy.gp_yy[agw_no]*ygp;

  return 0;

}

//----------------------------------------------------------------
//
// CCDtoAGW() - convert CCD offset (dx,dy) to AGw stage guide
//                   probe offset
//
// Given a pixel offset (dx,dy)_CCD on the guide camera CCD frame,
// compute the corresponding AGw stage (dx,dy)_GP offset to apply to
// move an image on the guide camera by that amount.
//
// Because this is a *differential* rather than absolute calculation,
// the CCDtoAGW transformation only has 4 coefficients instead of 6
// (the translation offset between the CCD and AGw GP coordinate
// systems always subtracts out).
//

int
mods_CCDToAGW(int agw_no, double dxccd,double dyccd,double *dxagw, double *dyagw, int options)
{
  if (agw_no != MODS1 && agw_no != MODS2) {
    return(115);  // No such instrument available
  }
  *dxagw = xy.ccd_xx[agw_no]*dxccd + xy.ccd_xy[agw_no]*dyccd;
  *dyagw = xy.ccd_yx[agw_no]*dxccd + xy.ccd_yy[agw_no]*dyccd;

  return 0;
}

//-----------------------------------------------------------------------------
//
// CCDtoSFP() - convert CCD (x,y) relative to a reference (x,y) pixel
//              into an SFP offset.
//
// Given a pixel location (x,y)_CCD of an object in the guide camera CCD,
// and the pixel location of a reference location (x,y)_ref, compute the
// corresponding SFP coordinate offset (dx,dy)_SFP.
//
// This transformation computes the CCD pixel offset between the reference
// pixel and the object:
//    dx = xref - xccd;
//    dy = yref - yccd;
// Then algebraically it uses the calibration of the CCD-to-AGW transformation
// to compute the corresponding guide probe offset, then this is converted
// into the SFP offset using the AGW-to-SFP transformation.  Because this
// is a relative rather than absolute transform, we work it out algebraically
// here using the coefficients of the two relevant transforms.
//
// The detailed derivation is described in a separate document [citation TBD]
//

int
mods_CCDToSFP(int agw_no, double xccd, double yccd, double xref, double yref,
              double *dxsfp, double *dysfp, int options)
{
  if (agw_no != MODS1 && agw_no != MODS2) {
    return(115);  // No such instrument available
  }

  // Compute the offset between the reference pixel and object pixel coords

  double dx = xref - xccd;
  double dy = yref - yccd;

  // Convert pixel offset (dx,dy) to SFP offset in millimeters

  *dxsfp = (xy.gp_xx[agw_no]*xy.ccd_xx[agw_no] + xy.gp_xy[agw_no]*xy.ccd_yx[agw_no])*dx + (xy.gp_xx[agw_no]*xy.ccd_xy[agw_no] + xy.gp_xy[agw_no]*xy.ccd_yy[agw_no])*dy;

  *dysfp = (xy.gp_yx[agw_no]*xy.ccd_xx[agw_no] + xy.gp_yy[agw_no]*xy.ccd_yx[agw_no])*dx + (xy.gp_yx[agw_no]*xy.ccd_xy[agw_no] + xy.gp_yy[agw_no]*xy.ccd_yy[agw_no])*dy;

  return 0;
}
