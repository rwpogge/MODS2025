#ifndef AGWUTILS_H
#define AGWUTILS_H

/*!
  \file agwutils.h
  \brief MODS AGW Utilities header

  \author R. Pogge, OSU Astronomy Dept. (pogge@astronomy.ohio-state.edu)
  \date 2008 Aug 19

  \par Modification History:
<pre>
   2008 Aug 19 - based on earlier agwsim, removing the sim parts [rwp/osu]
   2009 Oct 24 - updates based on lab work with the agw stage mechanism [rwp/osu]
</pre>
*/

// System header files 

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/times.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/file.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <termios.h>
#include <fcntl.h>
#include <signal.h>

//---------------------------------------------------------------------------
//
// MODS AGW parameters
//
// Most of these are "fixed" parameters of the system to be defined at
// compile-time, wherease a small number are sensible defaults that can
// be overridden by variables in the runtime configuration file.
//

#define AGW_XYRES        5 //!< XY stage linear resolution in microns
#define AGW_XYSPEED     10 //!< XY stage drive speed in microns/millisec
#define AGW_XSMIN        0 //!< Minimum XS-axis stage travel in mm from home (zero)
#define AGW_XSMAX      185 //!< Maximum XS-axis stage travel in mm from home (zero)
#define AGW_YSMIN        0 //!< Minimum YS-axis stage travel in mm from home (zero)
#define AGW_YSMAX      206 //!< Maximum YS-axis stage travel in mm from home (zero)

#define AGW_FOCRES      10 //!< Focus motion resolution in microns
#define AGW_FOCSPEED     4 //!< Focus motion drive speed in microns/millisec
#define AGW_FOCMIN       0 //!< Minimum AGW Focus actuator travel in mm
#define AGW_FOCMAX      64 //!< Maximum AGW Focus actuator travel in mm
#define AGW_FOC0        42 //!< Default AGW Focus actuator position at the science-field center
#define AGW_DFOCMAX      5 //!< Maximum relative (+/-) focus offset in mm
#define AGW_FRADIUS   1040 //!< Focal surface radius of curvature in millimeters
#define AGW_XMINRATE     1 //!< Minimum absolute X-axis drift rate in mm/second
#define AGW_XMAXRATE    10 //!< Maximum absolute X-axis drift rate in mm/second
#define AGW_YMINRATE     1 //!< Minimum absolute Y-axis drift rate in mm/second
#define AGW_YMAXRATE    10 //!< Maximum absolute Y-axis drift rate in mm/second

#define AGW_XCEN        92 //!< Default XS-axis location of the science-field center in mm
#define AGW_YCEN       202 //!< Default YS-axis location of the science-field center in mm
#define AGW_AGXMIN     -92 //!< Acquisition/Guide (AG) Camera minimum X(AG) position in mm
#define AGW_AGXMAX      92 //!< Acquisition/Guide (AG) Camera maximum X(AG) position in mm
#define AGW_AGYMIN    -204 //!< Acquisition/Guide (AG) Camera minimum Y(AG) position in mm
#define AGW_AGYMAX       0 //!< Acquisition/Guide (AG) Camera maximum Y(AG) position in mm

#define AGW_WFSXOFF      0 //!< Off-axis Wavefront Sensor field center X(AG)-axis offset in mm
#define AGW_WFSYOFF  12.500//!< Off-axis Wavefront Sensor field center Y(AG)-axis offset in mm

#define AGW_NFILTERS     4 //!< Number of AGW filter wheel positions
#define AGW_FILTTIME  1000 //!< Filter select time in milliseconds per position
#define AGW_FILTSIZE    16 //!< Maximum length of filter names in characters

#define TOWER_IN  1           //!< Calibration tower is in the MODS beam (deployed)
#define TOWER_OUT 0           //!< Calibration tower is out of the MODS beam (stowed)

#define AGW_LOCKED    1       //!< AGW command lock held by a host
#define AGW_NOTLOCKED 0       //!< AGW command lock is free

#define MILLIM        1000    //!< Microns per millimeter
#define FMILLIM       2000    //!< 2:1 Gear Ratio for Focus

// AGW default logfile.
#define DEFAULT_AGWLOG "/home/dts/Logs/AGW/agw.log"

//----------------------------------------------------------------
//
// agwpar: MODS AGW mechanism parameter data structure
//

/*!
  \brief MODS AGW Mechanism Parameter Table

  Encapsulates the parameters of an instance of a MODS AGW system (XY
  stage, focus, and filter wheel)

*/

typedef struct agwpar {
  long filter;       //!< Current AG Camera filter wheel position (1..#numFilters)
  char **filterID;   //!< Filter ID table [indexed by (int)(#Filter-1)]
  long numFilters;   //!< Number of AG camera filter positions (default: 4)

  double xStage;     //!< Current AGW stage absolute X-axis position in millimeters from device axis home
  double yStage;     //!< Current AGW stage absolute Y-axis position in millimetersfrom device axis home

  double focus;      //!< Current AGW stage absolute Focus position in millimeters from device axis home
  double foc0;       //!< Focus zero-point (absolute focus position at science field center in millimeters)
  double focOffset;  //!< Focus zero-point offset in millimeters.  Nominal value is 0 millimeters.
  double autoFocus;  //!< AGW AutoFocus flag (0=disabled, 1=enabled, default: enabled)

  double xCen;    //!< XS-axis position of the science-field center in millimeters from stage X home
  double yCen;    //!< YS-axis position of the science-field center in millimeters from stage Y home

  double xAG;     //!< Acquisition/Guide Camera field-center X position in millimeters from #XCen
  double yAG;     //!< Acquisition/Guide Camera field-center Y position in millimeters from #YCen

  double xWFS;    //!< Off-Axis Wave-Front Sensor Camera field-center X position in millimeters from #XCen
  double yWFS;    //!< Off-Axis Wave-Front Sensor Camera field-center Y position in millimeters from #YCen
  double dxWFS;   //!< Off-axis WFS pickoff X offset from the guide probe field center in millimeters
  double dyWFS;   //!< Off-axis WFS pickoff Y offset from the guide probe field center in millimeters
  
  long lock;         //!< AGW probe command lock (0=disabled, 1=asserted)
  char lockHost[12]; //!< Client host ID of the current lock holder
  char cmdHost[12];  //!< Client Host who sent the current command (used for lock testing)

  long calib;        //!< Calibration Tower position (#TOWER_OUT or #TOWER_IN)

  long drift;        //!< AGW probe drive rate flag (0=disabled, 1=enabled)
  long xDrift;       //!< Drift vector X component in mm/second
  long yDrift;       //!< Drift vector Y component in mm/second

  int abort;         //!< Abort flag, normally 0, 1 if abort requested

  int YIP;           //!< Port for Y axis
  int XIP;           //!< Port for X axis
  int FIP;           //!< Port for Z axis(Focus)
  int FWIP;          //!< Port for Filter Wheel

  int MOVING_BITS[4]; //!< mechanism(s) moving bits. [0]=Y,[1]=X,[2]=Focus,[3]=FW

  double LATITUDE;    //!< Latitude
  double LONGITUDE;   //!< longitude

  char agw_reply[4096]; //!< servers error return

} agwpar_t;


// Function Prototypes
int    agwIsLocked(agwpar_t *, char *);
void   agwInitConfig(agwpar_t *);
double agwFocus(agwpar_t *, double, double);
void   gpToAGW(agwpar_t *, double, double, double *, double *, double *);
void   agwToGP(agwpar_t *, double, double, double, double *, double *);
void   agwGetPos(agwpar_t *);
void   AGWSimMove(long movetime);

#endif  // AGWUTILS_H
