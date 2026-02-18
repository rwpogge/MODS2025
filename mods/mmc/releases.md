# MODS Mechanism Control (MMC) Server Release Notes
Original Build: 2009 June 15

Last Build: 2026 Feb 15

## Version 3.2.7: 2026 Feb 8
Seeing seg-faults crashing mmcServer during testing last night. Logs showed that just before the date/time tag
in the mmc log was >30 characters, and had a second, slightly different microsencds part appended onto it, like 
this:
 > 2026-02-18T06:57:58.608389.608392 M1.IE>MC1 DONE: RCAMFOC RCAMFOC=1630

looking at the mmcServer logging code (`mmcServers/mmcLOGGER.c`), the log date/time tag
is created using the `getDateTime()` function, in which the microseconds part is being appended
to the string with a `sprintf()` into the string itself.

This is not thread safe, and if two threads hit the `getDateTime()` function at about the same
time (within 10s of microseconds), you get the extra, intact, microseconds part which sends the
date/time tag string over its declared length of 30 bytes, causing a seg-fault downstream.

Changes needed:
 * increased size of declaration of static char `str` from 30 to 64 to buy some extra room
 * properly initializing str using `memset()`
 * now reads the microseconds part promptly and builds `str` in one `sprintf()` rather than using an append to `str`.  

While this is not perfectly Posix thread safe (besides, it's using BSD threads not pthreads because the core code
is 2006 vintage) it is a lot thread safer than before ahd should avoid this problem.

## Version 3.2.6: 2026 Feb 17
`mmcServers/redIMCS.cpp` - For MODS2 Red it the wires for QC3 and QC4 are
swapped **in the dewar**.  Fixing for now in software, hardware fix at the
HEB level later (summer 2026)?  Means there are now _mods1 and _mods2 versions
of the redIMCS server we need to be careful about!


## Version 3.2.5: 2026 Jan 30
`app/wagoSetGet.c` - increased pause from 10ms to 50ms to break up "Operation now in service" fault due to connecting to the WAGO unit to quickly after
instantiating the new modbus tcp socket (see v3.2.2 below).

Also added a single retry after another 50ms pause. Fault occurs on `modbus_connect()` that follows `modsbus_new_tcp()`.  Most likely culprit for the fault is 
collision with a competing request.  A single retry after a pause has been introduced to see if this calms things.

## Version 3.2.4: 2025 Dec 31
Small changes from live testing at LBTO. Probably more to come.  Took us this long to get the
Archon controllers working properly with the MODS science CCDs
 * `blueIMCS.cpp` and `redIMCS.cpp` - verified IMCS operation earlier in month, got wirign right the first time. Adjusted minimum quad cell signal threshold downward, need to adjust electronic bias when cold later, looks workable for now.

## Version 3.2.3: 2025 Oct 30
Fixed bugs in IEB command that caused crashes with segmentation faults for some IEB options and especially
anything to do with reading or setting the MicroLYNX motor controller (MLC) power status:
 * `mlc.c` in `wagoRW()` - replaced `(short *)atoi(argbuf)` in `wagoSetGet()` calls with `regData` short.
 * `commands.c` in `cmd_ieb()` - replaced `(short *)atoi(argbuf)` in `wagoSetGet()` calls with `regData` short, and cleaned up some of the code and logic. 

Tested on MODS2 live (on cart in high-bay), not crashing, restored MLC engineering-level function

## Version 3.2.2: 2025 Oct 4
Two builds running down bugs
 * Code logic in the `mmcServer` HEB command had bugs that needed fixing. All command combinations confirmed.
 * Needed to insert a 10ms delay between modbus open/connect methods: `mmc/app/wagoSetGet.c`, same issue as with `modsEnv` and `modsHEB`

## Version 3.2.0: 2025 Oct 3
 * Many updates from live testing at LBTO with MODS1 and MODS2
 * Deep changes in the APIs and others, some due to 32-bit to 64-bit migration

Rebranded `mmcService` to `mmcServer`, which sounds trivial but is not, it brings its function into alignment with other 
subsystems called "servers", and it is functionally a server. This also makes a clear distinction between this system
and actual `systemd` *services* that are part of the MODS2025 updates. The `mmcServer` should not (and probably must not)
be run as a system "service".

## Version 3.1.0: 2025 June
 * AlmaLinux 9 port and updates for the MODS2025 Archon CCD controller upgrade started
 * System built on AlmaLinux 9, a few minor changes.
 * Major cleanup of the code base, retiring unused code and removing from the main code tree.  Structure being prepared for revision control using GitHub.

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

## Version 2.5.0-1: [02-01-10] code fix and patch
 * Fixed GRATING reset/initialize problem.
 * Reworked threads, and removed a non-closing FD condition.
 * Major removal of shutdown(FD) and replaced with a close(FD).

## Version 2.4.3: [01-11-10] patch
 * Fixed the istatus VFLAT reporting format error.

## Version 2.4.2: [01-06-10] patch
 * Added patches for the AGWFILT busy.
 * Changed cIMCS information(status) and added commands GAIN, AVERAGE, QCSAMP, QCMIN, OPEN, CLOSE
 * AGw mechanisms move in parallel.

## Version 2.4.1: [12-31-09] patch
 * AGw would not respond to commands ( bug fixed )
 * Added patches for the AGW STEP parameter and range limits.
 * STEP range limits for xCOLFOC, xCOLTTFy. 
 * Increased backlash velocity in PLC code from VI=0.5 to VI=2.0.

## Version 2.4.0: [12-22-09] code fix
 * None beta code. Saved as running system.

## Version 2.3.1: [12-18-09] patch
* VFLAT Old FLAT3, with a parameter(power 1..11).
* QTH1 and QTH2 Old FLAT1 and FLAT2 name change. Mentioned in previous version, but never included as a patch.
* Removing any and all useless code and/or comments. (clean)
* Added the code for checking both limits CW and CCW for agwServer

## Version 2.3.0: [11-01-09] code fix
 * Shared memory modified for the new IMCS (data received from HEB). rimcs.cpp and bimcs.cpp reworked to accept the new shared memory addresses.
 * redIMCS.cpp and blueIMCS.cpp are new tasks to talk to the HEB.
 * ABORT ALL Added for emergency stops.
 * LLB WAGO address changes (515 to 517) 515 is now the VFLAT power parameter, and 517 is the switch for Lamps and Lasers ON/OFF. No WAGO address change on vislaser or irlaser power.
 * VFLAT Old FLAT3, with a parameter(power 1..11).
 * QTH1 and QTH2 Old FLAT1 and FLAT2 name change.

## Version 2.2.0: code fix
 * First non-beta version. [10-01-09]

## Version 2.1.0: [09-10-09] code fix
 * UTIL system with all it's associate tasks.
 * STARTUP and all associate tasks.

## Version 1.0.0: [6-15-2009] BETA code
 * Initial mmc system for MODS1 lab integration and testing.
