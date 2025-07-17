/* 
  loadShm - load mechanisms.ini info to shared memory

  R. Pogge, OSU Astronomy
  pogge.1@osu.edu

  Modification:
    2025 July 1 - first version

*/
 
#include <iostream>
using namespace std;

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>

#include "system_dep.h"     // OS dependent headers
// #include "dpi.h"
#include "MATHCNST.H"
#include "instrutils.h"     // ISL Instrument header
#include "isl_funcs.h"      // function defines for all isl
#include "isl_types.h"      // general header file for all isl data structure definitions
#include "params.h"         // general isl parameter header
#include "islcommon.h"      // shared memory (Islcommon C data structure) layout
#include "isl_shmaddr.h"    // declaration of pointer to islcommon
#include "isisclient.h"

#include "agwcomm.h"     // AGW functions header file
#include "modscontrol.h" // AGW functions header file
#include "mmccontrol.h"  // MMC (IE) functions header file
#include "mmc_client.h"  // loadconfig prototype

// dummy client instance for this test

isisclient_t client;

int
main(int argc, char *argv[])
{
  setup_ids();
  if (argc == 1)
    LoadConfig((char*)"mechanisms.ini");
  else
    LoadConfig(argv[1]);
  
}
