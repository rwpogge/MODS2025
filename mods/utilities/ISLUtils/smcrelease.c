/*!
  \file smcrelease.c
  \brief smcrelease - ISLSN Release Controller.

  \author Staff, OSU Astronomy Dept. (staff@astronomy.ohio-state.edu)
  \date 2005 March 20 (Astronomy Staff)

  \par Usage: Unix style
  smcrelease(int controller);

  Release or disconnect TCP/IP contoller connection.

*/
#include <iostream>
#include <sstream>
#include <cstring>
using namespace std;

#include "instrutils.h"   // Command action functions header file
#include "isl_funcs.h"
#include "isl_types.h"
#include "params.h"      // Common parameters and defines
#include "islcommon.h"     // Common parameters and defines
#include "isl_shmaddr.h"     // Shared memory attachment.

//----------------------------------------------------------------
int
smcrelease(int release)
{
  if(release<0) release=0;
  shm_addr->MODS.busy[release] = 0;  
  return 0;
}
