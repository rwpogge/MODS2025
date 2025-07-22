/*!
  \file islSysTask.c
  \brief islSysTask.c - Utility for ISL to start 'system' type tasks. 

  \author Staff, OSU Dept. of Astronomy (rgonzale@astronomy.ohio-state.edu)
  \date 2007 Feb 20 (rdg)

  \par Usage:
  int islSysTask(FILE *,char []); return and error number. 

  NOTE: /home2/isl/include/instrutil.h file has more information.

  Run a isl 'system' style task.

*/

#include "system_dep.h"  // Command action functions header file
#include "instrutils.h"  // Command action functions header file
#include "isisclient.h"  // ISIS common client library header

#include "isl_funcs.h"
#include "isl_types.h"
#include "params.h"      
#include "islcommon.h"   
#include "isl_shmaddr.h" 

#define MAXSIZE 1024
/* ---------------------------------------------------------------- */
int
islSysTask(char cmd[])
{
  int ierr;
  char buffer[MAXSIZE];
  FILE *ptr;
  
  if ( (ptr = popen(cmd, "r")) != NULL) {
    while(fgets(buffer,MAXSIZE-1,ptr) != 0)
      printf("%s",buffer);
    ierr=0;
  } else ierr=-1;

  fflush(ptr);
  pclose(ptr);
  return(ierr);
}
