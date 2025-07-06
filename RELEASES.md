# MODS2025 Release Notes
Last Release: 2024 Aug 14

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
