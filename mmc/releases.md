# MODS Mechanism Control (MMC) Server Release Notes
Original Build: 2009 June 15

Last Build: 2025 July 5

## Version 3.0.5: [2016-01-24]
 * race condition in cmd_slitmask().  Added a 100ms sleep between the select and position query, like in cmd_mselect(), to fix [rwp]

## Version 3.0.4: [2013-10-03]
 * Pre-shipment version for MODS2.  Mostly minor tweaks deep inside for 2-MODS operation, including using a MODS symlink in the Config directories to avoid any hardwired paths with MODS1 or MODS2
 * Minor bug fixes related to detailed message handling, low-level parameter return inconsistencies that didn't matter for MODS1 but will confuse later when running MODS1+2, etc.
 * Patch level not advanced from 3-4 mostly for expedience.

## Version 3.0.3: [2011-05-10]
 * Numerous low-level tweaks for MODS1 learned during on-sky work Had to fix logic in the circuit breaker tests, as the code did not test for the breaker-tripped (xxx_BRK=FAULT) condition. Implemented and tested 2011-10-31 at LBT on MODS1.
 * Also fixed bug in "util llb" command that was not correctly testing the AC power on/off condition [rwp]

## Version 3.0.2: [2011-01-10]
 * Added position names to DICHROIC, [R|B]FILTER, [R|B]GRATING
 * "Fixed" config files mechanisms.ini. File was never changed to conform with the new logging and initialize paths.
 * CALMODE and OBSMODE functions have been added to commands.c
   * CALMODE Configures the MODS instrument to acquire internal calibrations.
   * OBSMODE Configures MODS intrument for on-sky observing.

## Version 3.0.1: [2010-10-20]
 * Synch of code from LBT after the MODS1 September AGw commissioning run.  Changes mostly transparent [rwp]

## Version 3.0.0: [04-01-10]
**Major code revision with a number of patch and code changes**
 * Fixed thread leak. Major change in thread_clients.
 * AGWFOC changed to AGWFS
 * GPFOCUS reply message has been changed from:
   <pre>
   DONE: GPFOCUS AGWFOCUS=0.000
     to
   DONE: GPFOCUS AGWFOCUS=0.000 AGWFS=0.000 AGWFS0=38.500 AGWYS=0.000
   </pre>
 * BARCODE block of code is commented out so inserting mask was not sticking out. isisStatus code for reporting this has been moved outside of BARCODE block.
 * mmcLOGGER rework for timestamp logging.
 * All Temperature sensors except for HEB's have names:
   * IEB_R: IEBTEMPR, IEBGRT_R, TAIRTOP, TAIRBOT
   * IEB_B: IEBTEMPB, IEBGRT_B, TCOLLTOP, TCOLLBOT Have been added to all calling verbs including the ISTATUS verb and the mlc.c function
   * IUB: GSPRES, GSTEMP, GRPRES, GRTEMP, IUBTAIR, AMBTEMP, AGHSTEMPand they have been added to all calling verbs and the ISTATUS module in the commands.c function.
 * The ESTATUS verb has been added the RESET module in the commands.c function.
 * Rick Pogge made some changes to commands.c at LBT which were copied to OSU mods1 and mods4 computers
 * Added xxxxINFO=string to ISTATUS [R|B] query command.
 * Change mlcRecover.cpp and params.h to redirect path for tables to /home/mods/Config/MODS1/*.tab
 * Corrected typo from NOCONN to NOCOMM
 * Corrected an error in commands.c when inserting a mask. It was giving message "Inserting mask 0" which is incorrect. It should give mask that was selected. "Inserting Mask 5" for example. 

## Version 

## Version 




mmc_1.0.0: [6-15-2009] BETA code
        * Initial mods system.

mmc_2.1.0: [09-10-09] code fix
        * UTIL system with all it's associate tasks.
        * STARTUP and all associate tasks.

mmc_2.2.0: code fix
        * First non-beta version. [10-01-09]

mmc_2.3.0: [11-01-09] code fix
        * Shared memory modified for the new IMCS (data received from HEB).
          rimcs.cpp and bimcs.cpp reworked to accept the new shared memory 
          addresses.
        * redIMCS.cpp and blueIMCS.cpp are new tasks to talk to the HEB.
        * ABORT ALL Added for emergency stops.
        * LLB WAGO address changes (515 to 517) 515 is now the VFLAT power 
          parameter, and 517 is the switch for Lamps and Lasers ON/OFF.
          No WAGO address change on vislaser or irlaser power.
        * VFLAT Old FLAT3, with a parameter(power 1..11).
        * QTH1 and QTH2 Old FLAT1 and FLAT2 name change.

mmc_2.3.1: [12-18-09] patch
        * VFLAT Old FLAT3, with a parameter(power 1..11).
        * QTH1 and QTH2 Old FLAT1 and FLAT2 name change.
          mention in previous version, but never included as a patch.
        * Removing any and all useless code and/or comments. (clean)
        * Added the code for checking both limits CW and CCW for agwServer

mmc_2.4.0: [12-22-09] code fix
        * None beta code. Saved as running system.

mmc_2.4.1: [12-31-09] patch
        * AGw would not respond to commands ( bug fixed )
        * Added patches for the AGW STEP parameter and range limits.
        * STEP range limits for xCOLFOC, xCOLTTFy. 
        * Increased backlash velocity in PLC code from VI=0.5 to VI=2.0.
mmc_2.4.2: [01-06-10] patch
        * Added patches for the AGWFILT busy.
        * Changed cIMCS information(status) and added commands GAIN,AVERAGE,
          QCSAMP,QCMIN,OPEN,CLOSE
        * AGw mechanisms move in parallel.
mmc_2.4.3: [01-11-10] patch
        * Fixed the istatus VFLAT reporting format error.
mmc_2.5.0-1: [02-01-10] code fix and patch
        * Fixed GRATING reset/initialize problem.
        * Reworked threads, and removed a non-closing FD condition.
        * Major removal of shutdown(FD) and replaced with a close(FD).






