# Shared Memory Test

loadShm is a program to load a mechanisms.ini file into the shared memory sector (`modsalloc` service)
that may then be read using `vueinfo`. 

It is used to test changes to our MODS shared memory sector.

## Details

loadShm.c is a wrapper for LoadConfig() to load mechanism.ini config file
contents into live shared memory.

Code probably has excessive includes, borrowed from vueinfo.c.

The main program just initializes MODS shared memory access (runs setup_ids()), then runs LoadConfig().

Main is simple and hardwires the config file.  Could be a command-line
argument in a later version
```
   #include "islcommon."    // The MODS shared memory block definition
   #include "isl_shmaddr.h" // creates the shm_addr pointer

   int main(int argc, char *argv[]) {
      setup_ids();  // initializes the shared memory access
      LoadConfig((char*)"mechanism.ini");
   }
```
Verify correct loading of shared memory with "vueinfo ipmap"
Should see the correct IP address and mechanism assignments
