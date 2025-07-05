/** \file islcontrol.h 
 * \brief Library functions for controlling the AGW Units of the LBT
 */

#ifndef ISLCONTROL_H_
#define ISLSCONTROL_H_

#ifdef __cplusplus 
extern "C" {

/** All purpose AGW Stage client
 * @param host: the host the TCP Server is residing on
 * @param port: adresses the AGW
 * @param command: command string
 * @return an error value
 */
  int agwcu(char *host,char [], char [], char []);

/** All purpose Mechanism controller
 * @param host: the host the TCP Server is residing on
 * @param port: adresses the AGW
 * @param command: command string
 * @return an error value
 */

  int mmcu(char *host,char [], char [], char []);

/** All purpose IIF TCS controller
 * @param host: the host the TCP Server is residing on
 * @param port: adresses the AGW
 * @param command: command string
 * @return an error value
 */
  int iifcu(char *host,char [], char [], char []);

#endif

#endif
