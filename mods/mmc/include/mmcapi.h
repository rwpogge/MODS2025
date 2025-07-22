/** \file mmcapi.h 
 * \brief External Library functions
 */

#ifndef MMCISL_H_
#define MMCISL_H_

extern void islAPIInit(void);
extern islnum islAppNameToAppNumb(char *appname);
extern islcomp islCnameToComp(char *cname);
extern islconn islMakeContact(islcomp c, islnum a);
extern int islSendEOF(islconn conn);
extern islconn islWaitContact(islnum a);
extern int recvln(islconn, char *buff, int buffsz);

#endif
