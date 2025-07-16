#ifndef AGWSIM_H
#define AGWSIM_H

/*!
  \file agwsim.h
  \brief MODS AGW Simulator API header

  \author R. Pogge, OSU Astronomy Dept. (pogge@astronomy.ohio-state.edu)
  \date 2006 Jan 11

  \par Modification History:
<pre>
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
// MODS AGW simulator parameters
//

#define AGWSIM_XYRES        5 //!< XY stage linear resolution in microns
#define AGWSIM_XYSPEED     10 //!< XY stage drive speed in microns/millisec
#define AGWSIM_XSMAX   180000 //!< Maximum XS-axis stage travel in microns from home (zero)
#define AGWSIM_YSMAX   200000 //!< Maximum YS-axis stage travel in microns from home (zero)

#define AGWSIM_FOCRES      10 //!< Focus motion resolution in microns
#define AGWSIM_FOCSPEED     4 //!< Focus motion drive speed in microns/millisec
#define AGWSIM_FMAX     20000 //!< Maximum Focus travel in microns (in +YS axis direction)

#define AGWSIM_FRADIUS   1040 //!< Focal surface radius of curvature in millimeters

#define AGWSIM_XMINRATE     1 //!< Minimum absolute X-axis drift rate in microns/second
#define AGWSIM_XMAXRATE 10000 //!< Maximum absolute X-axis drift rate in microns/second
#define AGWSIM_YMINRATE     1 //!< Minimum absolute Y-axis drift rate in microns/second
#define AGWSIM_YMAXRATE 10000 //!< Maximum absolute Y-axis drift rate in microns/second

#define AGWSIM_XCEN     90000 //!< XS-axis location of the science-field center in microns
#define AGWSIM_YCEN    200000 //!< YS-axis location of the science-field center in microns
#define AGWSIM_AGXMIN  -90000 //!< Acquisition/Guide (AG) Camera minimum X(AG) position in microns
#define AGWSIM_AGXMAX   90000 //!< Acquisition/Guide (AG) Camera maximum X(AG) position in microns
#define AGWSIM_AGYMIN       0 //!< Acquisition/Guide (AG) Camera minimum Y(AG) position in microns
#define AGWSIM_AGYMAX  180500 //!< Acquisition/Guide (AG) Camera maximum Y(AG) position in microns

#define AGWSIM_WFSXOFF      0 //!< Off-axis Wavefront Sensor field center X(AG)-axis offset in microns
#define AGWSIM_WFSYOFF -12500 //!< Off-axis Wavefront Sensor field center Y(AG)-axis offset in microns

#define AGWSIM_NFILTERS     4 //!< Number of AGW filter wheel positions
#define AGWSIM_FILTTIME  1000 //!< Filter select time in milliseconds per position
#define AGWSIM_FILTSIZE    16 //!< Maximum length of filter names in characters

#define TOWER_IN  1           //!< Calibration tower is in the MODS beam (deployed)
#define TOWER_OUT 0           //!< Calibration tower is out of the MODS beam (stowed)

#define AGW_LOCKED    1       //!< AGW command lock held by a host
#define AGW_NOTLOCKED 0       //!< AGW command lock is free
#define MILLIM           1000 //!< Convert microns to millimeters mechanism position
#define FOCUS_MM            2 //!< focus 1/2 the size of XY mechanisms.

//----------------------------------------------------------------
//
// agw_sim: MODS AGW simulator configuration data structure
//

/*!
  \brief MODS AGW Simulator Configuration Table

  Contains data on the configuration of a MODS AGW simulator.

*/

typedef struct agw_sim {
  long Filter;       //!< Current A/G Camera filter wheel position (1..#NFiltPos)
  char **FilterID;   //!< Filter ID table [indexed by (int)(#Filter-1)]
  long NFiltPos;     //!< Number of A/G camera filter positions (default: 4)

  long XS;           //!< Current AGW stage absolute X-axis position in microns from device axis home
  long YS;           //!< Current AGW stage absolute Y-axis position in microns from device axis home

  long Focus;        //!< Current AGW stage absolute Focus position in microns from device axis home
  long F0;           //!< Focus zero-point (absolute focus position at science field center in microns)
  long AutoFocus;    //!< AGW AutoFocus flag (0=disabled, 1=enabled, default: enabled)

  long XCen;         //!< XS-axis position of the science field center in microns from stage X home
  long YCen;         //!< YS-axis position of the science field center in microns from stage Y home

  long XAG;          //!< Acquisition/Guide Camera field-center X position in microns from #XCen
  long YAG;          //!< Acquisition/Guide Camera field-center Y position in microns from #YCen

  long XWFS;         //!< Off-Axis Wave-Front Sensor Camera field-center X position in microns from #XCen
  long YWFS;         //!< Off-Axis Wave-Front Sensor Camera field-center Y position in microns from #YCen

  long Lock;         //!< AGW probe command lock (0=disabled, 1=asserted)
  char LockHost[12]; //!< Client host ID of the current lock holder
  char CmdHost[12];  //!< Client Host who sent the current command (used for lock testing)

  long Calib;        //!< Calibration Tower position (#TOWER_OUT or #TOWER_IN)

  long Drift;        //!< AGW probe drive rate flag (0=disabled, 1=enabled)
  long Xrate;        //!< Drift vector X component in microns/second
  long Yrate;        //!< Drift vector Y component in microns/second

  int Abort;         //!< Abort flag, normally 0, 1 if abort requested

  int CALIP;         //!< Port for Calibration Tower
  int YIP;           //!< Port for Y axis
  int XIP;           //!< Port for X axis
  int FIP;           //!< Port for Z axis(Focus)
  int FWIP;          //!< Port for Filter Wheel

  double LATITUDE;   //!< Latitude
  double LONGITUDE;  //!< longitude

  char agw_reply[4096]; //!< servers error return

} agw_sim_t;

// Function Prototypes

int  AGWIsLocked(char *reply);
void AGWInitConfig(agw_sim_t *agw);
long AGWFocus(agw_sim_t *agw, long Xt, long Yt);
void DeviceToAGW(agw_sim_t *agw);
int  AGToDevice(agw_sim_t *agw);
int  WFSToDevice(agw_sim_t *agw);
void AGWSimMove(long movetime);

#endif  // AGWSIM_H
