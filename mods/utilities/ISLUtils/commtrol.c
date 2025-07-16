/*!
  \file commtrol.c
  \brief commtrol - commtrol box courier.

  \author Staff, OSU Dept. of Astronomy (rgonzale@astronomy.ohio-state.edu)
  \date 2005 Oct 20 [rdg]

  \par Usage:
  commtrol (controller #, string command);
  Sometime in the future:
  commtrol <name> <string command>

  Send a MicroLYNX command to the Comtrol Box.
*/

#include "system_dep.h"   // utililties function header file
#include "instrutils.h"   // utililties function header file
#include "params.h"       // common parameters and defines
#include "islcommon.h"    // common parameters and defines
#include "isl_shmaddr.h"  // Shared memory attachment.

//----------------------------------------------------------------
int
commtrol(int instrument, char cmd[])
{
  int ierr = 0;
  char *commander;
  // Send command to the Comtrol courier
  
  ierr = sprintf(commander,"Comtrol %d %s",instrument,cmd);
  commander[strlen(commander)]='\0';
  ierr = system(commander);

  return ierr;
}
