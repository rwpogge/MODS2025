/** \file mmccontrol.h 
 * \brief Library functions for controlling MODS
 */

#ifndef MMCCONTROL_H_
#define MMCCONTROL_H_

#include "agwcomm.h"

#ifdef __cplusplus 
extern "C" {

/** All purpose MODS mechanism client
 * @param host: the host that mmcService(s) reside
 * @param port: Port assigned to the mmcService
 * @param command: command and/or reply message.
 * @return an error value
 */
  int lamp(char *host, char *port, char command[]);

/** All purpose MODS mechanism client
 * @param host: the host that mmcService(s) reside
 * @param port: Port assigned to the mmcService
 * @param who: mechanism to command
 * @param command: command and/or reply message.
 * @return an error value
 */
  int mmcu(char *host,char *port, char *who, char command[]);

/** All purpose MODS calibration tower client
 * @param host: the host that mmcService(s) reside
 * @param port: port assignment to the mmcService
 * @param command: command and/or reply message.
 * @return an error value
 */
  int calib(char *host,char *port, char command[]);

/** All purpose MODS hatch (darkslide) client
 * @param host: the host that mmcService(s) reside
 * @param port: port assignment to the mmcService
 * @param command: command and/or reply message.
 * @return an error value
 */
  int hatch(char *host,char *port, char command[]);

/** All purpose MODS dichroic client
 * @param host: the host that mmcService(s) reside
 * @param port: port assignment to the mmcService
 * @param command: command and/or reply message.
 * @return an error value
 */
  int dichroic(char *host,char *port, char command[]);

/** All purpose MODS mask insert client
 * @param host: the host that mmcService(s) reside
 * @param port: port assignment to the mmcService
 * @param command: command and/or reply message.
 * @return an error value
 */
  int minsert(char *host,char *port, char command[]);

/** All purpose MODS slitmask client
 * @param host: the host that mmcService(s) reside
 * @param port: port assignment to the mmcService
 * @param command: command and/or reply message.
 * @return an error value
 */
  int slitmask(char *host,char *port, char command[]);

/** All purpose MODS mask select client
 * @param host: the host that mmcService(s) reside
 * @param port: port assignment to the mmcService
 * @param command: command and/or reply message.
 * @return an error value
 */
  int mselect(char *host,char *port, char command[]);

/** All purpose MODS mask barcode read client
 * @param host: the host that mmcService(s) reside
 * @param port: port assignment to the mmcService
 * @param command: command and/or reply message.
 * @return an error value
 */
  int readmask(char *host,char *port, char command[]);

/** All purpose MODS AGW X axis client
 * @param host: the host that mmcService(s) reside
 * @param port: port assignment to the mmcService
 * @param command: command and/or reply message.
 * @return an error value
 */
  int agwx(char *host,char *port, char command[]);

/** All purpose MODS AGW Y axis client
 * @param host: the host that mmcService(s) reside
 * @param port: port assignment to the mmcService
 * @param command: command and/or reply message.
 * @return an error value
 */
  int agwy(char *host,char *port, char command[]);

/** All purpose MODS AGW Focus axis client
 * @param host: the host that mmcService(s) reside
 * @param port: port assignment to the mmcService
 * @param command: command and/or reply message.
 * @return an error value
 */
  int agwfoc(char *host,char *port, char command[]);

/** All purpose MODS AGW Filter Wheel client
 * @param host: the host that mmcService(s) reside
 * @param port: port assignment to the mmcService
 * @param command: command and/or reply message.
 * @return an error value
 */
  int agwfilt(char *host,char *port, char command[]);

/** All purpose MODS AGW Guide Probe client
 * @param host: the host that mmcService(s) reside
 * @param port: port assignment to the mmcService
 * @param command: command and/or reply message.
 * @return an error value
 */
  int gprobe(char *host,char *port, char command[]);

/** All purpose MODS AGW Focus Guide Probe client
 * @param host: the host that mmcService(s) reside
 * @param port: port assignment to the mmcService
 * @param command: command and/or reply message.
 * @return an error value
 */
  int gpfocus(char *host,char *port, char command[]);

/** All purpose MODS AGW Guide Probe Offset client
 * @param host: the host that mmcService(s) reside
 * @param port: port assignment to the mmcService
 * @param command: command and/or reply message.
 * @return an error value
 */
  int gpoffset(char *host,char *port, char command[]);

/** All purpose MODS mechanism client
 * @param host: the host that mmcService(s) reside
 * @param port: Port assigned to the mmcService
 * @param command: command and/or reply message.
 * @return an error value
 */
  int lamp(char *host, char *port, char command[]);

/** All purpose MODS mechanism client
 * @param host: the host that mmcService(s) reside
 * @param port: Port assigned to the mmcService
 * @param command: command and/or reply message.
 * @param ret: return string: 
 * @return an error value
 */
  int utilities(char *host, char *port, char command[], char ret[]);

/** All purpose MODS rcolfoc and bcolfoc client
 * @param host: the host that mmcService(s) reside
 * @param port: port assignment to the mmcService
 * @param command: command and/or reply message.
 * @return an error value
 */
  int rcolfoc(char *host,char *port, char command[]);
  int bcolfoc(char *host,char *port, char command[]);

}
#endif

#endif
