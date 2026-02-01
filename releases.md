# MODS2025 Release Notes

**Last Release: 2026 Feb 01**

## Version 1.6.6 - 2026 Feb 01
Changes after live testing at LBTO during the Jan 31/Feb 1 ECD night
 * `modsCCD` changed to slow the cadence of readout reporting from every 200ms to roughly every second. This keeps from flooding comms with rapid-fire updates. A fast internal monitor cadence is required to be able to properly detect CCD control state changes, but that is too fast and unnecessary for downstream clients. Implemented in `main.c`, tested, verified, and rolled out across the 4 Archon servers.
 * `modsUI` made 2 changes in the default `modsUI.ini` configuration files:
   - decreased the message dispatcher interval from 500ms to 250ms as modern computers can handle faster speeds
   - disabled the housekeeping dashboard as redundant given all MODS environmental and power state telemetry is accessible to the general observatory telemetry dashboard (DMS), quasi-real-time in the IIF DD, and long term archive in the HDF5 telemetry stream.  A separate housekeeping dashboard was loading the GUI and logging (not helping given 5x faster Archon controller speeds).
 * `mkMaskSnap.pl` - resurrected and added to `modsPerl`.  Used by support astronomers to make scripts to take through-mask images. Tested, committed, and installed in the common executable path.


## Version 1.6.5 - 2026 Jan 27
Multiple updates from live testing at LBTO
 * Cleaned up the filename logic in the `modsUI` setup dashboard. Found and removed misbehavior in the `azcam-mods` routine `set_filename()` that was causing problems. Auto-naming is now more robust, but watch users break it during upcoming live tests.
 * Introduced `binoMODS` script to be used running binocular scripts in a `modsTerm` allows users to send "return" or "abort" simultaneously to both MODS script engines (or just one at need). Will help efficiency not having to click and type between panes.
 * Caught up on updating documentation. New `readme.md` that goes in `/lbt/lbto/mods/bin` to get captured by the LBTO GitHub so we have a rolling quick-look summary of the scripts in that public folder.

## Version 1.6.4 - 2026 Jan 24
 * Overhauled the `modsUI` instrument setup dashboard. The obsDate is now provided by the `azcam-mods` server using an algorithm that uses the UTC date tag (CCYYMMDD) for images, but between local noon and 5pm MST, uses the UTC date of the coming night so that afternoon (pre-0UTC) calibrations have the same CCYYMMDD as the coming night. SciOps was doing this by hand, now it does it for you.
 *  Many changes to the `modsUI` instrument dashboard to get all the reporting and display functions working normally. All of the open issues identified on the ECD night (2026 Jan 19 - see v1.6.2 below) have been addressed and tested.
   
## Version 1.6.3 - 2026 Jan 22
 * Fixed a nasty segmentation fault bug in the `modsCCD` agent that would crash out if a string was too long for common commands like object et al.  Rebuilt `modsCCD` on all servers
 * Put in extra filtering of "=" in strings intended for the azcam server coming from scripts by filtering at the source in `acqMODS` and `execMODS`. Kind of insidious, but so deep in the structure of the `azcam` server remote interface we have to stop it from getting to the socket.

## Version 1.6.2 - 2026 Jan 19
Updates resulting from an ECD night on-sky with LBT in great seeing(!).  Ran up the full system for the first time and found all
kinds of new bugs and issues.

### `modsUI`
 * misconfiguration in `modsUI.ini` for MODS1 for the TCS, was tricky - updated config files for both
 * dashboard not showing `LASTFILE` info. Turns out that was a push from the old caliban data-transfer agents, so put in explicit requests to the `modsCCD` agents at the right places.
 * dashboard also not showing `Image: 1 of 3` status during exposures, reason was that the logic to catch the `SETUP` state was not fast enough for the Archons.  Solution was to modify `doExposure()` in `modsCCD/clientutils.c` to send an explicit notification to remote clients with the required `EXPSTATUS=INITIALIZING` flag. With the old DOS system this triggered the exposure state init in the GUI, including starting the Image counter.  Also some other stuff that was behind some benign but odd behaviors during exposures.
 * `COMMENT` field in setup has no equivalent in the Archon system.  Until I can work out a replacement, the right solution is to keep it in the setup info, but make it a benign "no-op" in the `modsCCD` agent for now.  It doesn't put info in the headers yet.  Later.
 * `monitor/envpanel.cpp` fixed location of the dewar temperature displays to make blue like red (typo in code).  Still not populating the `ArchonTemp` field, but we should be able to do that by adding it to the `modsCCD` agent `status` call.
 * Object names with "=" anywhere in them confused the `azcam` remote command interface. We filter these at the `modsUI`, but may have to implement filters elsewhere.
 * Updated `modsUI` config files with the new subframe region-of-interst modes for the Archon (`mods/Config/MODSn/ccdroi.tab`).
 * Modified `modsUI` dashboard to limit binning factors to 1 and 2 only (bin factors of 4 and 8 supported by old MODS are not supported by the Archon controllers).
 
### `modsCCD`
 * made `COMMENT` and `SAVECONFIG` benign no-ops until we can design suitable replacements.
 * added `notifyClient()` call to `doExposure()` in `clientutils.c` to send `EXPSTATUS=INITIALIZING` back to the client initiating a `GO` on exposure setup.  The original timing-loop method is not going to work for the Archons as the state transition is too fast to catch. Tested and released.
 * Modified `commands.c` binning commands to limit binning factors to 1 and 2 only (bin factors of 4 and 8 supported by old MODS are not supported by the Archon controllers).


## Version 1.6.0 - 2026 Jan 02
Critical updates:
 * `azcam-mods`: Updated Archon config files for correct readout (50 pre-scan instead of 48)
 * `dataMan`: Updated dataMan agent with an overhauled merging code (correct mapping of Archon ADC lines to MODS quadrants)
 * minor fixes to `azcam_mods/mods.py` to fix `obsDate()` bug and update `set_roiByName()` coords for new Archon readout.

## Version 1.5.0 - 2025 Dec 31
Beta release of the `dataMan` for use across the servers, plus support scripts for starting/stopping the `azcam` server and `modsCCD` agent in a `ccdTerm`.

## Version 1.4.2 - 2025 Dec 26
Live testing with `dataMan` and `azcam` on mountain, dataMan still alpha but advancing rapidly.
 * Tested and verified method to invoke `dataMan` from `azcam` proper that streamlines the data flow after end of exposure. See release notes for `azcam-mods` for details. Advanced `azcam-mods` to version 1.1.8

## Version 1.4.1 - 2025 Dec 20
First live tests on summit observing workstations with the `lbto` account by Olga Kuhn on the `robs1` machine, 
with additional checks from user `osurc` on `obs1` and one of the other member user accounts.  Found and fixed a
number of issues small and large
 * `agwServer` - mods2 `agwServer` had been recompiled but not installed in path, found when checking `gprobe` for MODS2. Fixed and verified
 * User accounts given `PATH` updates to put the standard executables in their default path
 * Found setup issues with the `observer` account on two of the Archon servers, fixed and verified
 * Tested and updated the public `mods1`, `mods2`, and `imcsTools` scripts visible from the `robs/obs` machines.
 * Captured the `.bashrc` file template on the GitHub for future reference.

## Version 1.4.0 - 2025 Dec 16
Updates after significant work by Mike Lesser getting the MODS CCD detectors working with the Archon controllers.
 * `azcam` - added `azcam-console` and updated all of the runtime and Archon configuration files in `azcam-mods`.  
 * `azcam-mods` - multiple changes for the Archon controllers. See notes there.
 * `Sandbox/dataMan` - update for the azcam-generated FITS headers, and a testing Jupyter notebook.

## Version 1.3.3 - 2025 Oct 24
Live testing with MODS1 and MODS2 on LBT, ran with temperature control on all 4 dewars.  Bug fixes applied, tools built
to help monitor progress, and new problems emerged. 
 * We stood up all major instrument systems on both and have good logging and comms on all 6 computers.
 * Ran dewars cold, learning about temperature control, found ways to crash `azcam` and identified ways to stop it.
 * FITS headers now looking good, nearly converged, many bugs found and fixed
 * More work to go...

## Version 1.3.2 - 2025 Oct 12
 * Updated MODS azcam to v1.1.1
   * Added Archon config for the CCD heater controls with help from Greg Bredthauer
   * Added methods for reading heater PID info for engineering
   * Fixed bugs in the `MODSInstrument()` class
   * Changed method used by the `MODS()` class to read Archon status info (found internal azcam method that simplified parsing)

## Version 1.3.1 - 2025 Oct 2
 * fixed bugs in `agw/API` (also in `mmc/API`) that were preventing the `mmcServer` from communicating with the `agwServer` via the direct TCP port interface.  Traced to a problem with 32-bit vs 64-bit data in the way a key connection init routine (`islCnameToCComp.c`) was implemented.  Fixed and verified AGW comms.
 * fixed critical bug in the `agwServer` that caused the server to crash with a seg-fault.  Replaced suspect code with a more robust method, verified with both MODS.
 * New MODS admin interface scripts to replace `mods1` and `mods2` for the new system advanced to beta release.  We have adopted the `tmux` terminal multiplexer to handle terminal-based persistent session attach/detach interactions that is much more efficient over ethernet and does not require desktop sharing with all its attendant problems.  Alpha versions in `Sandbox/TMux`, beta versions migrating into `Scripts`.  Established dependencies on perl `Curses` lib.
 * Released parts of the `modsPerl` operational scripts (starting with `isisCmd`) into use for instrument testing. More on the way
 * Iterated with LBTO on environmental telemetry, general DD telemetry, and alarm handling.  Multi-week live tests used to identify and fix remaining issues in `modsEnv`, now getting reliable MODS telemetry into the DD and hdf5 streams.
 * First cut at `azcam-mods` code `instrument_mods.py` that implements an azcam `Instrument()` class instance tailored for MODS.  Will be used to create the instrument configuration FITS headers by pulling instrument config information from the DD using the IIF interface.  DD is populated by the `modsDD` agent now running as a systemd service on both machines.

## Version 1.3.0 - 2025 Sept 21
 * Beta release of most subsystems on LBTO machines, start of live testing with MODS1 and MODS2 on the telescope
 * New and updated subsystems:
   * `modsHEB` - New ISIS client agent for Archon head electronics box control (engineering)
   * `modsCCD` - New ISIS client agent that replaces the DOS IC programs for Archon CCD control and data aquisition and interfaces the MODS data-taking system with a custom `azcam` server operating the Archon CCD controller hardware.
   * `modsEnv` - Major update for systems associated with the Archons and removing systems retired with the old controllers, and now outputting most MODS environmental sensor data (including dewar temperature and vacuum pressure) as LBTO telemetry streams in HDF5 format, formally integrating our sensor telemetry with the observatory telemetry system. Also pushes the most recent measurements into the MODS server shared memory segment, exposing the data to other systems transparently. Run as a `systemd` service for full-time telemetry and auto restart.
   * `lbttcs` - Update for the Archon controllers and TCS/IIF Build 2025A, setup to run as a background `systemd` service.
   * `isis` - Port of the ISIS message-passing server to AlmaLinux, and implemented as a `systemd` service for full-time background operation and auto restart.
   * `modsDD` - New standalone agent that regularly updates the observatory Data Dictionary (DD) with current instrument state and environmental sensor data, exposing real-time status for third-party observatory apps like dashboards and alarm state monitors.
   * `modsPerl` - Port of the perl code for observing script execution (`acqMODS` and `execMODS`) and related engineering scripts.
   * GUIs (modsUI and iTerm) ported to Qt6.
  
The python `azcam` and `azcam-mods` modules to communicate with the Archon controllers and handle realtime data acquisition is in beta release and ready for live testing with the science CCDs on the telescope.  

On the to-do list is a replacement for the Caliban data-transfer agents (alpha testing in progress), new instrument startup scripts.

## Version 1.2.2 - 2025 Aug

Incorporated many changes following live tests at LBTO during the Archon+HEB installation and MODS modifications. Too many changes to recount, most data-taking sub 
systems advanced to release versions, but still many to-do items coming out of the engineering run

## Version 1.2.1 - 2025 July

A mix of tested beta code and developmental alpha code, including a version of the `azcam-mods` module ready for lab testing in Tucson when we go out
end of the month for Archon installation.  
 * First builds on the new mods flight Linux workstations running AlmaLinux 9.5, pre-shipped with the last build
 * Tested lbttcs against the TCS simulator provided by LBTO
 * Final assembly and pre-ship tests all of the Archon HEB boxes to be sent to LBTO using both the Jupyter notebook and the beta `modsHEB` code on the lab LBT MODS VLAN.
 * All ported code ready for live testing with MODS1 and MODS2 on their carts in the highbay when the team arrives at LBTO later this month

## Version 1.2.0 - 2025 June
 * Start of AlmaLinux 9 port in earnest
 * Porting MODS agw and mmc (IE) server codes
 * Porting MODS IIF/TCS codes
 * Re-writing IMCS codes to use WAGO-based quad cell readout electronics in the Archon Head Electronics Boxes
 * Replacing proprietary fieldtalk libraries for modbus/tcp interaction with WAGO with open-source libmodbus
 * Major overhaul of code trees and preparing to migrate into GitHub for future revision control and handover to LBTO

## Version 1.1.2 - 2024 Aug 14

- modsEnv 3.1.0 -> ModsEnv 3.1.1
    - Added a fallback leap-seconds file.
- test Ion Gauge I/O through Comtrol
    - C and Python code for interacting with the Micro-Ion Plus Vacuum Guage Module.

## Version 1.1.1 - 2024 Jul 16

- modsEnv 3.0 -> modsEnv 3.1.0
    - Environmental sensing data can now be logged to HDF5 files in the LBTO telemetry format.

## Version 1.1.0 - 2024 Jul 8

- Qt 5.X -> Qt 6.X
    - imsTool, iTerm, and modsUI should all be compiled using Qt 6.

## Version 1.0.0 - 2024 Feb 23

- Qt 4.X -> Qt 5.X
    - imsTool, iTerm, and modsUI should all be compiled using Qt 5.

## Version 0.0.0

This was the first version committed to the repository.
