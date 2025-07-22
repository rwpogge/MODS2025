/*!
  \file agwutils.c 
  \brief MODS AGW Stage Utility Functions

  This module contains utility functions used by the MODS AGW stage
  control system.

  \author R. Pogge, OSU Astronomy Dept. (pogge@astronomy.ohio-state.edu)
  \date 2006 Jan 12

  \par Modification History:
<pre>
   2008 Jan 27 - various changes for the actual AGW stage system 
                 and the simulation modes [rwp/osu]
   2008 Aug 19 - overhauled to update transformations and remove 
                 the simulation mode [rwp/osu]
   2009 Oct 24 - modifications while testing the LBT GCS interface [rdg & rwp/osu]
   2010 Mar 10 - replace sendCommand(); with mlcBase10Bits(); for filterwheel[rdg]
   2011 Mar 19 - modify agwFocus() to compute focus at the location of the WFS
                 hotspot [rwp/osu]
</pre>
*/

// in ../include specific to the AGW codes

#include "../include/client.h"     // Custom client application header file
#include "../include/agwutils.h"   // AGW mechanism header

extern int mlcBitsBase10(int, char [],char []);

//---------------------------------------------------------------------------
//
// agwIsLocked() - check the lock status
//

/*!  
  \brief Check the AGW lock status
  \param agw pointer to an agwpar_t struct for this AGW instance
  \param reply string to contain any command return reply
  \return 1 if the AGW is locked against this host, 0 if the AGW
  is either (a) not locked or (b) host is the lock holder. The
  reply string contains an error message if the host is locked out.

  Tests the status of the AGW command lock.  If the lock is not set, it
  returns 0, allowing the action function (usually in command.c) to
  proceed.  If the lock is asserted, it tests the cmdHost against
  lockHost.  If cmdHost is the lockHost it returns 0 ("OK to proceed"),
  otherwise it returns 1 ("device is locked out").

  Allowance for executive override must be tested in the calling
  routine.

*/

int
agwIsLocked(agwpar_t *agw, char *reply)
{

  if (agw->lock == 0) // If no lock enabled, return AGW_NOTLOCKED in all cases
    return AGW_NOTLOCKED;

  if (strcasecmp(agw->cmdHost,agw->lockHost)==0) // If command host holds the lock, return AGW_NOTLOCKED
    return AGW_NOTLOCKED;

  // Command is locked and host is not the lock holder

  sprintf(reply,"Cannot execute command, client is locked by %s",agw->lockHost);
  return AGW_LOCKED;

}

/*!  
  \brief Initialize the AGW stage runtime parameters
  \param agw pointer to the agwpar_t struct to initialize

  Initializes the AGW stage configuration data structure to sensible
  default values.

*/

void
agwInitConfig(agwpar_t *agw) 
{
  int i;

  agw->filter = 1;                // Assume filter position 1 before query
  agw->numFilters = AGW_NFILTERS; // Number of filter wheel positions

  agw->xStage = 0;     // Assume home position for all 3 axes before query
  agw->yStage = 0;      
  agw->focus = 0;                  
  agw->focOffset = 0;   // Focus offset is 0 mm from nominal
  agw->autoFocus = 1;   // autoFocus is ENABLED by default
  agw->xCen = AGW_XCEN; // Nominal XS position of the science field center, set by ini file
  agw->yCen = AGW_YCEN; // Nominal YS position of the science field center, set by ini file
  agw->foc0 = AGW_FOC0; // Nominal focus at the science field center, set by ini file

  /*
  // Compute the guide probe field center coordinates
  */
  agwToGP(agw,agw->xStage,agw->yStage,agw->focus,&agw->xAG,&agw->yAG); 

  /*
  // WFS pickoff mirror position in field-center coordinates
  */
  agw->dxWFS = AGW_WFSXOFF;
  agw->dyWFS = AGW_WFSYOFF;
  agw->xWFS = agw->xAG + agw->dxWFS;
  agw->yWFS = agw->yAG + agw->dyWFS;

  agw->lock = 0;                   // Lock is disabled on init
  strcpy(agw->lockHost,"none");    // and we clear the lock host

  agw->calib = TOWER_OUT;          // Calibration tower is stowed (out of the beam) until query

  agw->drift = 0;                  // Drift disabled
  agw->xDrift = 0;                 // No drift vector
  agw->yDrift = 0;

  /*
  // Load place holders into the FilterID table, it gets loaded from the
  // runtime config file later.  The placeholder is *i*, where "i" is
  // the slot number (1..AGW_NFILTERS)
  */
  for (i=0; i<AGW_NFILTERS; i++)
    sprintf((agw->filterID)[i],"*%d*",i+1);

  agw->abort = 0;   // Finally, reset the Abort flag

}

//---------------------------------------------------------------------------
// 
// agwFocus() - compute the focus offset relative to the science field
//              center at any given guide probe position
// 

/*!
  \brief Compute the focus offset at any given guide probe field position
  \param xGP X-axis location of the guide probe center in millimeters
  \param yGP Y-axis location of the guide probe center in millimeters
  \return The focus value in millimeters from the focus home position

  The LBT f/15 Direct Gregorian focus has a convex spherical focal
  surface with the tangent point located at the center of the MODS
  science field.  The radius of curvature of this surface is defined by
  #AGW_FRADIUS in millimeters.

  For any given guide probe position relative to the science field
  center, this routine computes the nominal focus offset.

  Note that this routine just does the formal calculation of the focus
  surface, it does not validate the computed offset against the
  permitted range of motion of the focus mechanism.  That is the
  ultimate responsibility of the calling routine.

  Because the WFS pickoff is not located at the center of the guider
  field-of-view, we need to compute the focus at the location of
  the WFS "hotspot", which is defined in the .ini file by the
  WFSXOffset and WFSYOffset parameters.  This offset must be measured
  for each AGw unit, and can change when the AGw units are serviced.

  The telescope active optics is sensitive enough to see the focus
  difference.  For example, for a WFS Y-offset of 13.5mm relative to
  the center of the guide camera, at Y=4-arcmin from the science
  field center the focus shift is -1.8mm, corresponding to -298nm of
  Z4 shift (focus Zernike term).  This is readily measurable
  by the LBT active optics system.
 
  This change was instituted on 2011 March 19 [rwp/osu]

*/

double
agwFocus(double xGP, double yGP)
{
  double dx, dy;
  double rcyl2;
  double rc;
  double dfoc;

  rc = AGW_FRADIUS;  // LBT f/15 Direct Gregorian focal surface radius of curvature in millimeters

  // These are the locations used for computing focus at the guide camera field center
  //
  //dx = (double)(xGP);
  //dy = (double)(yGP);  

  // Compute the focus at the WFS pickoff location

  dx = (double)(xGP + appAGW.dxWFS);  
  dy = (double)(yGP + appAGW.dyWFS);  

  rcyl2 = dx*dx + dy*dy;  // Cylindrical radius of the current position

  dfoc = (rc - sqrt(rc*rc - rcyl2));  // Focus offset relative to field center

  return dfoc;
}

//---------------------------------------------------------------------------
// 
// Guide Probe to Stage Coordinate Transformations
//
// gpToAGW() - Guide Probe (X,Y) to AGW stage (X,Y,Focus)
// agwToGP() - AGW stage (X,Y,Focus) to Guide Probe (X,Y)
//

/*!
  \brief Transform Guide Probe (xGP,yGP) to AGW stage (Xs,Ys,Focus)
  
  \param agw Pointer to an AGW stage data structure
  \param xgp Guide Probe X coordinate relative to science field center [mm]
  \param ygp Guide Probe Y coordinate relative to science field center [mm]
  \param xs AGW stage X position relative to home [mm]
  \param ys AGW stage Y position relative to home [mm]
  \param foc AGW stage focus probe position relative to home [mm]

  For the AGW stage, home is defined as the mechanism driven to its
  mechanical home position sensor.

  The X stage position is just the requested X probe position relative
  to the calibrated X stage position of the field center.

  The Y stage position transformation includes the calibrated Y stage
  position of the field center and the position of the focus actuator,
  which is colinear with the Y stage motion.  The focal surface of the
  LBT f/15 direct Gregorian focus is spherical, and computed using
  agwFocus().  At each Y position, we compute the focus offset
  associated with that position, and then offset the Y stage that amount
  to put the guide probe field center at that position, with that focus.

  If autoFocus is disabled, it uses the current focus position.  In
  normal operation we expect autoFocus to always be enabled.

  Complicated, but it works.

  For the reverse transformation, see agwToGP()

*/

void
gpToAGW(agwpar_t *agw, double xgp, double ygp, 
	double *xs, double *ys, double *foc)
{
  double dfoc;

  if (agw->autoFocus){
    dfoc = agwFocus(xgp,ygp);

  }else
    dfoc = (agw->focus - (agw->foc0 + agw->focOffset));

  *xs  = xgp + agw->xCen;
  *ys  = ygp + agw->yCen - dfoc - agw->focOffset;
  *foc = agw->foc0 + dfoc + agw->focOffset;
}

/*!
  \brief Transform AGW stage (Xs,Ys,Focus) to Guide Probe (xGP,yGP)
  
  \param agw Pointer to an AGW stage data structure
  \param xs  AGW stage X position relative to home [mm]
  \param ys  AGW stage Y position relative to home [mm]
  \param foc AGW stage focus probe position relative to home [mm]
  \param xgp Guide Probe X coordinate relative to science field center [mm]
  \param ygp Guide Probe Y coordinate relative to science field center [mm]

  Transforms the AGW stage position defined by the triplet (Xs,Ys,Focus)
  into the location of the guide probe field center (xGP,yGP) relative
  to the defined center of the science field.

  For the reverse transformation, see gpToAGW()

*/

void
agwToGP(agwpar_t *agw, double xs, double ys, double foc, 
	double *xgp, double *ygp)
{
  double dfoc;

  dfoc = foc - agw->foc0 - agw->focOffset;

  *xgp  = xs - agw->xCen;
  *ygp  = ys - agw->yCen + dfoc + agw->focOffset;

}

/*!
  \brief Query the mechanisms and update the device position data members
  
  \param agw Pointer to an AGW stage data structure

  Queries the AGW stage mechanisms and updates the current coordinates
  of the stage, guide probe center, and WFS pickoff center.  The origin
  of the guide probe and WFS pickoff fields is the nominal center of the
  MODS science field of view.

  Queries the individual mechanism controllers for the current position.
  All positions are stored internally in units of millimeters.

*/

void
agwGetPos(agwpar_t *agw)
{
  double xGP,yGP;
  char dummy[512];
  int mbits;

  /*
  // Query the controllers for the current positions (updates shared
  // memory) We have to correct for the -0 problem here when we
  // rectify the sign of the X and Y stage positions becaues the
  // drives for both X and Y stages are geared such that CCW
  // (negative) stepper motor rotation corresponds to positive
  // translation away from the axes home limit switches (the CW
  // limit).
  */

  agw->xStage = -positionToShrMem(agw->XIP,dummy);

  if (agw->xStage <= 0.0) agw->xStage = 0.0;

  agw->yStage = -positionToShrMem(agw->YIP,dummy);

  if (agw->yStage <= 0.0) agw->yStage = 0.0;

  agw->focus = positionToShrMem(agw->FIP,dummy)*2.0;

#ifdef DEBUG
  printf ("AGW(x,y,f)=(%0.3f,%0.3f,%0.3f\n",
	  agw->xStage,agw->yStage,agw->focus);
#endif

  //agw->filter = mlcBitsBase10(appAGW.FWIP, "PRINT IO 22,IO 21",dummy)+1;

  /*
  // Convert the raw stage positions into the derived positions of the
  // centers of the guide probe and WFS pickoffs.
  */

  agwToGP(agw,agw->xStage,agw->yStage,agw->focus,&xGP,&yGP);
  agw->xAG = xGP;
  agw->yAG = yGP;
  agw->xWFS = agw->xAG + agw->dxWFS;
  agw->yWFS = agw->yAG + agw->dyWFS;
}
