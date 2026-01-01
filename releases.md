# MODS2025 Release Notes

**Last Release: 2025 Dec 31**

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
