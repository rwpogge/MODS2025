# modsHeb Release Notes
Last Build: 2024 Aug 14

## Version 3.1.1
Wednesday, August 14th, 2024

Added a fallback leap-seconds file. The fallback file ensures that if a leap-seconds file is not found, the program will not crash.

## Version 3.1.0
Monday, July 15th, 2024

Enviornmental sensing data can now be logged to Hdf5 files in the LBTO telemetry format.

## Version 3.0
Monday, Feb 20th, 2024

Version 3.0 was written in 2024 to retire use of the proprietary FieldTalk libraries in favor of the open source libmodbus libraries (http://libmodbus.org) for Modbus/TCP communication with the WAGO fieldbus controllers. This allowed us to port the modsEnv code to AlmaLinux 9.3 which is the new OS standard at LBT for all instruments and subsystems.  **Version 3 replaces all Version 2.x**

- FieldTalk -> libmodbus

## Version 2.0
2013

Version 2.0 was written in 2013 to support the commissioning of MODS2 and initiation of binocular operation at the LBT.  At that time we experimentally implemented sharing the environmental data via the MMS shared memory (shmem) sector. This worked but conveyed no particular utility to sustain it. V2 will be retired along with the old server hardware in ~2025 as we can no longer support using the proprietary FieldTalk libraries for WAGO Modbus interaction.

## Version 1.X
Wednesday Dec 15th, 2010

Introduced UseTTY for non-interactive (background) operation.

## Version 1.X
Monday Jun 21st, 2010

New client application.