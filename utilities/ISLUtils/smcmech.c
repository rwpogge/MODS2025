/*!
  \file smcmech.c
  \brief smcmech - communicate with the without ISL.

  \author Staff, OSU Dept. of Astronomy (rgonzale@astronomy.ohio-state.edu)
  \date 2006 July 20 [rdg]

  \par Usage:
  smcmech (mechanism_name, MicroLYNX_command, value);

  Send the command to the ISL Comtrol system call for service.
*/

#include "system_dep.h"   // utililties function header file
#include "instrutils.h"   // utililties function header file
#include "params.h"       // common parameters and defines
#include "islcommon.h"    // common parameters and defines
#include "isl_shmaddr.h"  // Shared memory attachment.

static char commands[MAX_ML-1][10] = {
  {"darkslide"}, {"calib"}, {"maskI"}, {"maskS"}, 
  {"agwY"}, {"agwX"}, {"agwPF"}, {"agwFW"}, {"dich"},
  {"BttfA"}, {"BttfB"}, {"BttfC"}, {"Bgt1"}, {"Bgt2"}, 
  {"Bgs"}, {"RttfA"}, {"RttfB"}, {"RttfC"}, 
  {"Rgt1"}, {"Rgt2"}, {"Rgs"}, {"Bshutter"}, 
  {"Rshutter"}, {"BcamPF"}, {"RcamPF"}, 
  {"BcamFW"}, {"RcamFW"} /* 0 - 27 */
};

//----------------------------------------------------------------
int
smcmech(char instrument[], char cmd[], float fval)
{
  int ierr = 0;
  int controller;
  char commander[40]; 

  // Send command to the Comtrol courier

  for (controller=0; controller<MAX_ML-1; controller++) {
    if (strcasecmp(instrument,commands[controller])==0) break;
  }

  // add one for those who seem to think that counting starts at one (1)
  controller++;

  if (cmd == NULL ) {
    ierr = sprintf(commander,"smcm %d print stats\n",controller);
    commander[strlen(commander)]='\0';
    ierr = system(commander);
  } else {
    ierr = sprintf(commander,"smcm %d %s %f",controller,cmd,fval);
    commander[strlen(commander)]='\0';
    ierr = system(commander);
  }

  return ierr;
}
