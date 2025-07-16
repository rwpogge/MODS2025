/** \file modscontrol.h 
 * \brief Library IIF functions. Although params ard discribed as the values
 * in its units, all are sent as a character string.
 * 
 */

#ifndef MODSCONTROL_H_
#define MODSCONTROL_H_

#include "modscomm.h"

#ifdef __cplusplus 
extern "C" {

/** IIF Control Unit for ISL and/or ISIS
 * @param host: the host the TCP Server is residing on
 * @param str: return ASCII message
 * @return an error value
 */
  int iifcu(char *host, char str[], char who[], char str1[]);

/** Initialize IIF
 * @param host: the host the TCP Server is residing on
 * @param str: return ASCII message
 * @return an error value
 */
  int iifInit(char *host, char str[]);

/** Close IIF
 * @param host: the host the TCP Server is residing on
 * @param str: return ASCII message
 * @return an error value
 */
  int iifClose(char *host, char str[]);

/** iifAuthorize
 * @param host: the host the TCP Server is residing on
 * @param str: return ASCII message
 * @return an error value
 */
  int iifAuthorize(char *host, char str[]);


/** iifDeauthorize
 * @param host: the host the TCP Server is residing on
 * @param str: return ASCII message
 * @return an error value
 */
  int iifDeauthorize(char *host, char str[]);

/** iifCancelCMD
 * @param host: the host the TCP Server is residing on
 * @param str: return ASCII message
 * @return an error value
 */
  int iifCancelCMD(char *host, char str[]);

/** iifDebug
 * @param host: the host the TCP Server is residing on
 * @param str: return ASCII message
 * @return an error value
 */
  int iifDebug(char *host, char str[]);

/** iifGetParam
 * @param host: the host the TCP Server is residing on
 * @param str: request and return ASCII message
 * @return an error value
 */
  int iifGetParam(char *host, char str[]);

/** iifGetPCMDStatus
 * @param host: the host the TCP Server is residing on
 * @param str: request and return ASCII message
 * @return an error value
 */
  int iifGetPCMDStatus(char *host, char str[]);

/** iifGOffset - Offsetting the Telescope Guiding
 * @param host:  The host the TCP Server is residing on
 * @param str:   OFFSET to the current target position.
 * @param str1:  SIDE this command applies to.
 * @param str2:  ROTATOR Angle
 * @return an error value
 */
  int iifGOffset(char *host, char str[], char str1[], char str2[]);

/** iifPOffset - Offsetting the Telescope Pointing
 * @param host:  The host the TCP Server is residing on
 * @param str:   ROTATOR Angle. Also an ASCII message is returned.
 * @param str1:  OFFSET to the current target position.
 * @param str2:  SIDE this command applies to.
 * @return an error value
 */
  int iifPOffset(char *host, char str[], char str1[], char str2[]);

/** iifGStar 
 * @param host: the host the TCP Server is residing on
 * @param str: send COOR1 and return ASCII message
 * @param str1: send COOR2 
 * @param str2: send Mag.
 * @return an error value
 */
  int iifGStar(char *host, char str[], char str1[], char str2[]);

/** iifGuideStars
 * @param host: the host the TCP Server is residing on
 * @param str: send [clear] or [without param] return ASCII message
 * @return an error value
 */
  int iifGuideStars(char *host, char str[]);

/** iifIdle
 * @param host: the host the TCP Server is residing on
 * @param str: send [side - left,right,both] and return ASCII message
 * @return an error value
 */
  int iifIdle(char *host, char str[]);

/** iifLogEvent
 * @param host: the host the TCP Server is residing on
 * @param str: error number - also returns ASCII message
 * @param str1: message
 * @return an error value
 */
  int iifLogEvent(char *host, char str[], char str1[]);

/** iifRotator
 * @param host: the host the TCP Server is residing on
 * @param str: DISTANCE - also returns ASCII message
 * @param str1: ANGLEMAGNITUDE 
 * @param str2: DIRECTION SIDE
 * @return an error value
 */
  int iifRotator(char *host, char str[], char str1[], char str2[]);

/** iifRotate
 * @param host: the host the TCP Server is residing on
 * @param str: DISTANCE - also returns ASCII message
 * @param str1: ANGLEMAGNITUDE 
 * @param str2: DIRECTION SIDE
 * @return an error value
 */
  int iifRotate(char *host, char str[], char str1[], char str2[]);

/** iifMoveXY
 * @param host: the host the TCP Server is residing on
 * @param str:  X - also returns ASCII message
 * @param str1: Y
 * @param str2: SIDE
 * @return an error value
 */
  int iifMoveXY(char *host, char str[], char str1[], char str2[]);

/** iifTipTilt
 * @param host: the host the TCP Server is residing on
 * @param str:  X Rotation - also returns ASCII message
 * @param str1: Y Rotation
 * @param str2: SIDE
 * @return an error value
 */
  int iifTipTilt(char *host, char str[], char str1[], char str2[]);


/** iifMode
 * @param host: the host the TCP Server is residing on
 * @param str:  [passive|guided|active|adaptive|int] also returns ASCII message
 * @return an error value
 */
  int iifMode(char *host, char str[]);

/** iifTrackMode
 * @param host: the host the TCP Server is residing on
 * @param str:  [position|vertical|idle] also returns ASCII message
 * @return an error value
 */
  int iifTrackMode(char *host, char str[]);

/** iifReset
 * @param host: the host the TCP Server is residing on
 * @param str:  returns ASCII message
 * @return an error value
 */
  int iifReset(char *host, char str[]);

/** iifMoveFocus
 * @param host: the host the TCP Server is residing on
 * @param str:  absolute value - also returns ASCII message
 * @param str1: SIDE
 * @return an error value
 */
  int iifMoveFocus(char *host, char str[], char str1[]);

/** iifStepFocus
 * @param host: the host the TCP Server is residing on
 * @param str:  relative value - also returns ASCII message
 * @param str1: SIDE
 * @return an error value
 */
  int iifStepFocus(char *host, char str[], char str1[]);

/** iifPreset
 * @param host: the host the TCP Server is residing on
 * @param str:  SIDE - also returns ASCII message
 * @param str1: MODE
 * @return an error value
 */
  int iifPreset(char *host, char str[], char str1[]);

/** iifSetInst
 * @param host: the host the TCP Server is residing on
 * @param str:  INSTRUMENT ID - also returns ASCII message
 * @param str1: FOCUS
 * @param str2: SIDE
 * @return an error value
 */
  int iifSetInst(char *host, char str[], char str1[], char str2[]);

/** iifTarget
 * @param host: the host the TCP Server is residing on
 * @param str:  c1 - also returns ASCII message
 * @param str1: c2
 * @param str2: system
 * @param str3: equinox
 * @return an error value
 */
  int iifTarget(char *host, char str[], char str1[], 
		char str2[], char char3[], char str4[]);

/** iifGetParam
 * @param host: the host the TCP Server is residing on
 * @param str:  c1 - also returns ASCII message
 * @return an error value
 */
  int iifGetParam(char *host, char str[]);

/** iifSide
 * @param host: the host the TCP Server is residing on
 * @param str:  side - also returns ASCII message
 * @param str1: lev - level
 * @return an error value
 */
  int iifSide(char *host, char str[], char str1[]);

/** iifTCStatus
 * @param host: the host the TCP Server is residing on
 * @param str:  side - also returns ASCII message
 * @return an error value
 */
  int iifTCStatus(char *host, char str[]);

/** iifStandBy
 * @param host: the host the TCP Server is residing on
 * @param str:  side - also returns ASCII message
 * @param str1: lev - level
 * @return an error value
 */
  int iifStandBy(char *host, char str[], char str1[]);

/** iifSendWF
 * @param host: the host the TCP Server is residing on
 * @param str:  WF - also returns ASCII message
 * @return an error value
 */
  int iifSendWF(char *host, char str[]);


/** iifVerbose
 * @param host: the host the TCP Server is residing on
 * @param str:  returns ASCII message
 * @return an error value
 */
  int iifVerbose(char *host, char str[]);


/** log AGW Stage errors
 * @param host: the host the TCP Server is residing on
 * @param agw_no: adresses the AGW
 * @param options: for future use
 * @return an error value
 */
  int geterrormsg(char *host,int agw_no, int options);

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
  int home(char *host,int agw_no,int motors, int options);

/** Obtain the current status of the specified AGW unit and its axis
 * @param host: the host the TCP Server is residing on
 * @param agw_no: adresses the AGW
 * @param agwdata: where the data is written to
 * @param options: for future use
 * @return an error value
 */
  int getstatus(char *host,int agw_no,struct data *agwdata, int options);

/** Writes the current filter number to filter.
 * @param host: the host the TCP Server is residing on
 * @param agw_no: adresses the AGW
 * @param filter: where the filter number is written to
 * @param options: for future use
 * @return an error value
 */
  int getfilter(char *host, int agw_no, int *filter, int options);

/** Writes the current focus position to focus.
 * @param host: the host the TCP Server is residing on
 * @param agw_no: adresses the AGW
 * @param focus: where the position is written to
 * @param options: for future use
 * @return an error value
 */
  int getfocus(char *host, int agw_no, int *focus, int options);

/** Writes the current guide probe position to posxy.
 * @param host: the host the TCP Server is residing on
 * @param agw_no: adresses the AGW
 * @param posxy: the struct where the position is written to
 * @param options: for future use
 * @return an error value
 */
  int getxy(char *host, int agw_no, struct spos_xy *posxy, int options);

/** Writes the current guide probe position to posxy.
 * @param host: the host the TCP Server is residing on
 * @param str:  returned parameters
 * @return an error value
 */
  int getparam(char *host, char str[]);

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
  int setfilter(char *host, int agw_no, int filter_no, double velocity, int options);

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
  int setfocus(char *host, int agw_no, int new_pos, double velocity, int options);

 /** Give the focus the command to move to a new position.
 * @param host: the host the TCP Server is residing on
 * @param agw_no: adresses the AGW
 * @param lul: lock=request=1,unlock=release=0 commands
 * @param options: for future use
 * @return an error value
 */
  int ulock(char *host, int agw_no, int lul, int options);

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
  int drift(char *host, int agw_no, int ssq, int options);

/** Give the driftvec guiding probe the command
 * @param host: the host the TCP Server is residing on
 * @param agw_no: adresses the AGW
 * @param drift_x: x drift vector rates in um/sec.
 * @param drift_y: y drift vector rates in um/sec.
 * @param options: future use.
 * @return an error value
 */
  int driftvec(char *host, int agw_no, int drift_x, int drift_y, int options);

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
  int setxy(char *host, int agw_no, int new_pos_x, int new_pos_y, double velocity, int options);

}
#endif

#endif
