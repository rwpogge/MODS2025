/** \file modscontrol.h 
 * \brief Library functions for controlling the AGW Units of the LBT
 */

#ifndef MODSCONTROL_H_
#define MODSCONTROL_H_

#include "agwcomm.h"

#ifdef __cplusplus 
extern "C" {

/** All purpose AGW Stage client
 * @param host: the host the TCP Server is residing on
 * @param port: adresses the AGW
 * @param command: command string
 * @return an error value
 */
  int agwcu(char *host, int, char [], char []);

/** Initialize AGW Stage
 * @param host: the host the TCP Server is residing on
 * @param agw_no: adresses the AGW
 * @param options: for future use
 * @return an error value
 */
  int mods_init(char *host,int agw_no, int options);

/** abort AGW Stage
 * @param host: the host the TCP Server is residing on
 * @param agw_no: adresses the AGW
 * @param options: for future use
 * @return an error value
 */
  int mods_abort(char *host,int agw_no, int options);

/** Initialize AGW Stage
 * @param host: the host the TCP Server is residing on
 * @param agw_no: adresses the AGW
 * @param options: for future use
 * @return an error value
 */
  int agwinit(char *host,int agw_no, int options);

/** Center the AGW Stage
 * @param host: the host the TCP Server is residing on
 * @param agw_no: adresses the AGW
 * @param options: for future use
 * @return an error value
 */
  int center(char *host,int agw_no, int options);

/** All purpose calibTower client
 * @param host: host the TCP Server is residing on
 * @param name: device holding requested information
 * @param port: port
 * @param send: command
 * @param recv: return information
 */
  int mods_getparam(char *host, int agw_no, char [], int port, char [], char []);

/** log AGW Stage errors
 * @param host: the host the TCP Server is residing on
 * @param agw_no: adresses the AGW
 * @param logfile: logfile name with path
 * @return an error value
 */
  int mods_geterrormsg(char *host,int agw_no, int errnum, char *errmsg);

/** log AGW Stage errors
 * @param host: the host the TCP Server is residing on
 * @param agw_no: adresses the AGW
 * @param logfile: logfile name with path
 * @return an error value
 */
  int startGetErrors(char *host,int agw_no, char *logfile);

/** Homes the addressed Motors. The motors are a bit combination:
 * 0x1 is the Y-motor
 * 0x2 is the X-motor
 * 0x4 is the focus motor
 * 0x10 is the filter wheel
 * The Y-motor doesn't need to be homed, the position is absolute encoded.
 * Homiong this motor will let it drive to position 0
 * @param host: the host the TCP Server is residing on
 * @param agw_no: adresses the AGW
 * @param motors: adresses the axis to home
 * @param options: for future use
 * @return an error value
 */
  int mods_home(char *host, int agw_no, int motors, int options);

/** Obtain the current status of the specified AGW unit and its axis
 * @param host: the host the TCP Server is residing on
 * @param agw_no: adresses the AGW
 * @param agwdata: where the data is written to
 * @param options: for future use
 * @return an error value
 */
//  int mods_getstatus(char *host,int agw_no,struct data *agwdata, int options);
  int mods_getstatus(char *host,int agw_no,struct data *agwdata, int options);

/** Writes the current filter number to filter.
 * @param host: the host the TCP Server is residing on
 * @param agw_no: adresses the AGW
 * @param filter: where the filter number is written to
 * @param options: for future use
 * @return an error value
 */
  int mods_getfilter(char *host, int agw_no, int *filter, int options);

/** Writes the current AG focus position to focus.
 * @param host: the host the TCP Server is residing on
 * @param agw_no: adresses the AGW
 * @param focus: where the position is written to
 * @param options: for future use
 * @return an error value
 */
  int mods_getfocus(char *host, int agw_no, double *focus, int options);

/** Writes the current AG focus position to focus.
 * @param host: the host the TCP Server is residing on
 * @param agw_no: adresses the AGW
 * @param motor_no: 1=X, 2=Y, 4=Focus, 16=Filter adresses the AGW
 * @param ret: where the position is written to
 * @param options: for future use
 * @return an error value
 */
  int mods_getposition(char *host, int agw_no, int motor_no, double *ret, int options);

/** Writes the current guide probe position to posxy.
 * @param host: the host the TCP Server is residing on
 * @param agw_no: adresses the AGW
 * @param posxy: the struct where the position is written to
 * @param options: for future use
 * @return an error value
 */
  int mods_getxy(char *host, int agw_no, struct spos_xy *posxy, int options);

/** Give the filterwheel the command to move to an other filter.
 * @param host: the host the TCP Server is residing on
 * @param agw_no: adresses the AGW
 * @param filter_no: number of the filter to set
 * @param velocity: how fast the filter should be moved (in um/sec) 
 * (INTERNALLY CASTED TO INTEGER, YET!!!)
 * @param options: \n
 * OPT_INC: move incremental
 * @return an error value
 */
  int mods_setfilter(char *host, int agw_no, int filter_no, double velocity, int options);

/** Give the focus the command to move to a new position.
 * @param host: the host the TCP Server is residing on
 * @param agw_no: adresses the AGW
 * @param new_pos: the new position in um from zero
 * @param velocity: how fast the focus should be moved (in um/sec) 
 * (INTERNALLY CASTED TO INTEGER, YET!!!)
 * @param options: \n
 * OPT_INC: move incremental
 * @return an error value
 */
  int mods_setfocus(char *host, int agw_no, double new_pos, double velocity, int options);

/** Give the focus the command to move to a new position.
 * @param host: the host the TCP Server is residing on
 * @param agw_no: adresses the AGW
 * @param motor_no: 1=X, 2=Y, 4=Focus, 16=Filter adresses the AGW
 * @param new_pos: the new position in um from zero
 * @param velocity: how fast the focus should be moved (in um/sec) 
 * (INTERNALLY CASTED TO INTEGER, YET!!!)
 * @param options: \n
 * OPT_INC: move incremental
 * @return an error value
 */
  int mods_setposition(char *host, int agw_no, int motor_no, double new_pos, double velocity, int options);

 /** Give the unlock stage command
 * @param host: the host the TCP Server is residing on
 * @param agw_no: adresses the AGW
 * @param options: for future use
 * @return an error value
 */
  int unlock(char *host, int agw_no, int options);

 /** Give the lock stage command
 * @param host: the host the TCP Server is residing on
 * @param agw_no: adresses the AGW
 * @param options: for future use
 * @return an error value
 */
  int mods_lock(char *host, int agw_no, int options);

 /** Give the autofocus the command
 * @param host: the host the TCP Server is residing on
 * @param agw_no: adresses the AGW
 * @param autof: auto=on=1,auto=off=0 commands
 * @param options: for future use
 * @return an error value
 */
  int autofocus(char *host, int agw_no, int autof, int options);

 /** Give the drift the command
 * @param host: the host the TCP Server is residing on
 * @param agw_no: adresses the AGW
 * @param ssq: start=1,stop=2,query=0  drift motion
 * @param options: for future use
 * @return an error value
 */
  int drift(char *host, int agw_no, int ssq);

/** Give the driftvec guiding probe the command
 * @param host: the host the TCP Server is residing on
 * @param agw_no: adresses the AGW
 * @param drift_x: x drift vector rates in um/sec.
 * @param drift_y: y drift vector rates in um/sec.
 * @param options: future use.
 * @return an error value
 */
  int driftvec(char *host, int agw_no, double drift_x, double drift_y, int options);

/** Give the guiding probe the command to move to a new position.
 * @param host: the host the TCP Server is residing on
 * @param agw_no: adresses the AGW
 * @param new_pos_x: x coordinate of the new position
 * @param new_pos_y: y coordinate of the new position
 * @param velocity: how fast the guiding probe should be moved (in um/sec) 
 * (INTERNALLY CASTED TO INTEGER, YET!!!)
 * @param options: \n
 * OPT_INC: move incremental
 * @return an error value
 */
  int mods_setxy(char *host, int agw_no, double new_pos_x, double new_pos_y, double velocity, int options);

/** Receive the current IP adress from UMAC. This is not really necessary,
 * as the IP adress is specified in the setup file of the oacontrol software.
 * @param[in] host the host the RPC Server is residing on
 * @param[in] agw_no adresses the AGW
 * @param[out] ip int array, where the IP is written to
 * @param[in] options for future use
 * @return an error value
 */
int mods_getip(char *host, int agw_no, unsigned char* ip, int options);

/** Set a new IP adress on UMAC. This should be only neccessary for initial
 * setup purposes and should already be done by AIP. Be aware, that the UMAC
 * needs to reboot for the change to take effect and that the setup file has
 * to be adapted to the new IP, too.
 * @param[in] host the host the RPC Server is residing on
 * @param[in] agw_no adresses the AGW
 * @param[in] ip int array holding the new IP
 * @param[in] options for future use
 * @return an error value
 */
int mods_setip(char *host, int agw_no, unsigned char* ip, int options);

/** Coordinate tranformation from AGW generic coordinates to AGW polar 
 * coordinates.
 * Refer to LBT-CAN-481g221b for details.
 * @param[in] agw_no adresses the AGW
 * @param[in] agwgeneric Gerneric GP coordinates to transform
 * @param[out] gprot GP rotation angle
 * @param[out] r GP radial axis position with origin in point of rotation of 
 * rotational axis
 * @return an error value
 */
int mods_AGWgenericToAGWpolar(char* host, int agw_no, spos_xy agwgeneric, angle *gprot, double *r);

/** Coordinate tranformation from SFP coordinates to AGW generic coordinates.
 * Refer to LBT-CAN-481g221b for details.
 * @param[in] agw_no adresses the AGW
 * @param[in] tcssfp differential GP coordinates
 * @param[in] pa position angle of target
 * @param[out] agwgeneric where the transformed GP coordinates are written to
 * @param[out] gprot GP rotation angle DEPRECDATED!!
 * @return an error value
 */
int mods_SFPtoAGWgeneric(char* host, int agw_no, spos_xy tcssfp, angle pa, spos_xy *agwgeneric);

/** Coordinate tranformation from AGW generic coordinates to TCS SFP
 * coordinates.
 * Refer to LBT-CAN-481g221b for details.
 * @param[in] agw_no adresses the AGW
 * @param[in] agwgeneric generic AGW coordinates
 * @param[in] pa position angle
 * @param[out] tcssfp where the transformed SFP coordinates are written to
 * @return an error value
 */
int mods_AGWgenericToSFP(char* host, int agw_no, spos_xy agwgeneric, angle pa, spos_xy *tcssfp);

/** Coordinate tranformation from SFP coordinates to CCD generic coordinates.
 * Refer to LBT-CAN-481g221b for details.
 * @param[in] agw_no adresses the AGW
 * @param[in] tcssfp offset between reference and actual position
 * @param[in] ccdref reference coordinates on CCD
 * @param[in] pa position angle
 * @param[in] gprot GP rotation angle
 * @param[out] ccdgeneric coordinates on CCD
 * @return an error value
 */
int mods_SFPtoCCDgeneric(char* host, int agw_no, spos_xy tcssfp, spos_xy ccdref, angle pa, angle gprot, spos_xy *ccdgeneric);

/** Coordinate tranformation from CCD generic coordinates to SFP coordinates.
 * Refer to LBT-CAN-481g221b for details.
 * @param[in] agw_no adresses the AGW
 * @param[in] ccdgeneric coordinates on CCD
 * @param[in] ccdref reference coordinates on CCD
 * @param[in] pa position angle
 * @param[in] gprot GP rotation angle
 * @param[out] tcssfp offset between reference and actual position
 * @return an error value
 */
int mods_CCDgenericToSFP(char* host, int agw_no, spos_xy ccdgeneric, spos_xy ccdref, angle pa, angle gprot, spos_xy *tcssfp);

/** Coordinate tranformation from WFS coordinates to SFP coordinates.
 * Refer to LBT-CAN-481g221b for details.
 * @param[in] agw_no adresses the AGW
 * @param[in] wfsgeneric WFS generic coordinates
 * @param[in] ccdref reference WFS coordinates
 * @param[out] tcssfp offset between reference and actual position
 * @return an error value
 */
/*
int WFSgenericToSFP(char* host, int agw_no, spos_xy wfsgeneric, spos_xy ccdref, spos_xy *tcssfp);
*/

/** Writes focus offset to foffset.
 * Refer to LBT-CAN-481g221b for details.
 * @param[in] agw_no adresses the AGW
 * @param[in] agwgeneric GP coordinates
 * @param[in] filterid Filter ID
 * @param[out] foffset offset between on-axis and actual focus
 * @return an error value
 */
int mods_GetFocusOffset(char* host, int agw_no, spos_xy agwgeneric, int filterid, double *foffset);

/** Computes the nominal center of the pupil.
 * Refer to LBT-CAN-481g221b for details.
 * @param[in] agw_no adresses the AGW
 * @param[in] agwgeneric GP coordinates
 * @param[in] gprot GP rotaion angle
 * @param[out] pupil where the pupil center coordinates are written to
 * @return an error value
 */
int mods_GetPupilCenter(char* host, int agw_no, spos_xy agwgeneric, angle gprot, spos_xy *pupil);

/** Computes the the rotation angle of the micro lenslet array.
 * Refer to LBT-CAN-481g221b for details.
 * @param[in] agw_no adresses the AGW
 * @param[in] gprot GP rotation angle
 * @param[in] derot Derotator rotation angle
 * @param[out] mlarot where the micro lenslet rotation angle is written to
 * @return an error value
 */
int mods_GetPupilRotation(char* host, int agw_no, angle gprot, angle derot, angle *mlarot);

}
#endif

#endif
