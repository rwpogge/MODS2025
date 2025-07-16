/* House keeping for WAGOs (shared memory layout) */

struct housekeep_shm {
  /* TIME */
  char timeStamp[80];    // House Keeping timestamp
  int day,               // The date for time
    day_frac;            // The fraction of day for time
  float msec_counter;    // Raw reading on the counter in microsec.

  /* 
   * Instrument Electronic Box(IEB), Lamp Laser Box(LLB) 
   * Utility Box(UB), Head Electronic Box(HEB) WAGO Temperatures
   * and other misc...
   */
  int ieb0_temp[6],      // IEB 0 is a standard temperature array
    ieb1_temp[6],        // Red IEB for MODS1 and MODS2
    ieb2_temp[6],        // Blue IEB for MODS1 and MODS2
    llb1_temp[6],        // LLB for MODS1 and MODS2 array
    utility_temp[6],     // UBB array
    heb_temp[6],         // HEB array

    iebRedCurrent[4],  // Red IEB Current for MODS1 and MODS2
    iebRed65V[4],      // Red IEB Voltage for MODS1 and MODS2
    iebBlueCurrent[4], // Blue IEB Current for MODS1 and MODS2
    iebBlue65V[4];     // Blue IEB Voltage for MODS1 and MODS2
    
  int check;           // read WAGO every n secs into SharedMemory
  /* */
  char hk_file[80];      // House Keeping file

};

