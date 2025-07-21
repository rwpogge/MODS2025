# Shared Memory Test

loadShm.c is a wrapper for LoadConfig() to load mechanism.ini config file
contents into live shared memory.

Code probably has excessive includes, borrowed from vueinfo.c.

The main program just initializes MODS shared memory access (runs setup_ids()), then runs LoadConfig().

Main is simple and hardwires the config file.  Could be a command-line
argument in a later version
```
#include "instrutils.h"  // ISL Instrument header
#include "islcommon.h"   // shared memory (Islcommon C data structure) layout
#include "isl_shmaddr.h" // declaration of pointer to islcommon

// prototypes we use

int LoadConfig(char *);
void setup_ids();

// this demo is built on isisclient methods

#include "isisclient.h"
isisclient_t client;

int main(int argc, char *argv[]) {
   setup_ids();  // initializes the shared memory access
   LoadConfig((char*)"mechanism.ini");
}
```
Verify correct loading of shared memory with "vueinfo ipmap"
Should see the correct IP address and mechanism assignments
