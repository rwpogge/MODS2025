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

// headers we need

#include "instrutils.h"  // ISL Instrument header
#include "islcommon.h"   // shared memory (Islcommon C data structure) layout
#include "isl_shmaddr.h" // declaration of pointer to islcommon

int LoadConfig(char *);
void setup_ids();

// uses isisclient mechanisms

#include "isisclient.h"
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
