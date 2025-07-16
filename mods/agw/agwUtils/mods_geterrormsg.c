 /** \file mods_geterrormsg.c
 * \brief This module implements the function to be called by the agwServer
 * thread managing routine to start the periodically check on all AGW units
 * if an error is generated. If so, the description of that error is 
 * written to the fatalerrormsg[index] found in getErrorMsg.h.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <sys/types.h>
#include <signal.h>

#include "../include/islapi.h"
#include "../include/modscontrol.h"
//#include "../include/getErrorMsg.h"
extern char *mods_fatalerrormsg[];
int 
mods_geterrormsg(char *host, int agw_no, int errnum, char *errmsg)
{
  strcpy(errmsg,mods_fatalerrormsg[errnum]);
  return errnum;
}
