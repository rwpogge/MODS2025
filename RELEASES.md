# MODS2025 Release Notes
Last Release: 2025 Sept 21

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

## Version 1.2.0 - 2025 June
 * Start of AlmaLinux 9 port in earnest
 * Porting MODS agw and mmc (IE) server codes
 * Porting MODS IIF/TCS codes
 * Re-writing IMCS codes to use WAGO-based quad cell readout electronics in the Archon Head Electronics Boxes
 * Replacing proprietary fieldtalk libraries for modbus/tcp interaction with WAGO with open-source libmodbus
 * Major overhaul of code trees and preparing to migrate into GitHub for future revision control and handover to LBTO

## Version 1.1.2
Wednesday, August 14th, 2024

- ModsEnv 3.1.0 -> ModsEnv 3.1.1
    - Added a fallback leap-seconds file.
- IonGuage 1.0.0
    - C and Python code for interacting with the Micro-Ion Plus Vacuum Guage Module.

## Version 1.1.1
Tuesday, July 16th, 2024

- ModsEnv 3.0 -> ModsEnv 3.1.0
    - Enviornmental sensing data can now be logged to Hdf5 files in the LBTO telemetry format.

## Version 1.1.0
Friday, July 8th, 2024

- Qt 5.X -> Qt 6.X
    - imsTool, iTerm, and modsUI should all be compiled using Qt 6.

## Version 1.0.0
Friday, February 23rd, 2024

- Qt 4.X -> Qt 5.X
    - imsTool, iTerm, and modsUI should all be compiled using Qt 5.

## Version 0.0.0
This was the first version committed to the repository.
