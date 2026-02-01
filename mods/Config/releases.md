# MODS Runtime Configuration File Release Notes

Latest Release: 2026 Jan 19

## Version 2.3 - 2026 Feb 01

Updated after live testing on-sy during the Jan 31/Feb 1 ECD night, made changes to the `modsUI.ini` file for MODS1 and MODS2:
 * Reduced the modsUI dispatcher cadence from 500ms to 250ms.  The slower cadence is a throwback to older, slower machines
 * Disabled the modsUI housekeeping dashboard as all info is now available in the LBTO observatory-wide telementry tool (DMS) and in the IIF Data Dictionary, as well as in the general LBTO HDF5 archival telemetry stream.

## Version 2.2 - 2026 Jan 19

Updated after live testing on-sky with the full end-to-end system
 * Errors fixed in the `modsUI.ini` files for MODS1 and MODS2 related to the TCS interface for MODS1
 * Updated `ccdroi.tab` for the new correction Archon readout ROIs (50 pixels of prescan)

## Version 2.1 - 2025 Dec 17
 * Updates after Archon controller configuration and tune-up. Found errors in the MODS2 `rconfig.tab` file that confused the MODS2R tune-up until corrected.  

## Version 2.0 - 2025 July 
 * MODS2025 controller update including new mechanism.ini files for both MODS

## Version 1.2 - 2012 Sept
 * Updates for MODS2 during OSU lab integration testing
 * Added additional information for MODS2 currents, lasers, and imcs

## Version 1.1 - 2010 Aug
 * MODS1 installation on the LBT
 * Updates include information for MODS1 currents,lamps, lasers, and imcs

## Version 1.0 - 2009 June
 * MODS1 lab integration and testing at OSU
 * First release version
 * Mechanism files consolidated into a single mechanisms.ini file

## Version 0.1 - 2005 May
 * First alpha release as part of the isl-1.0.0 development
 * Each mechanism has its own configuration file (darkslide.ini, agwx.ini, etc...)
