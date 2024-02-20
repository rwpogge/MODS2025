# modsenv - MODS instrument environmental monitor

Version: 3.0

Date: 2024 Feb 20

Author: R. Pogge, OSU Astronomy (pogge.1@osu.edu)

## Overview

modsEnv is collects and logs MODS instrument enviromental and power-status data
as an autonomous background process. Environmental sensors and power-status data
are obtained from WAGO Modbus/TCP fieldbus controllers in the MODS instrument
Utility and Instrument Electronics boxes (IUB and IEBs).  One instance of the modsEnv
agent must be run for each MODS instrument.

Basic operation is that modsEnv for a MODS instance runs in the background started
by the MODS user on the MODS instrument's data machine (mods1data or mods2data).
The modsEnv agent has two modes
 * If enviromental data is requested by another data-taking system process via the ISIS server, it queries the instrument and returns status info and logs the requested info.
 * If no external requests are pending, the agent will automatically query and log environmental sensor and power status on a fixed cadence (default is 5s, but set in the runtime initialization file), then wait for the next query time or request from another data-taking system agent.

Logs are kept on the local MODSn server machine.

## Dependencies

 * `isisClient` - ISIS client library (`libisis`) on the same machine
 * `libmodbus` - open source libmodbus libraries (http://libmodbus.org) - verified with v3.1.7 (needs `libmodbus` and `libmodbus-devel`).

## Release Notes

Version 2.0 was written in 2013 to support the commissioning of MODS2 and initiation
of binocular operation at the LBT.  At that time we experimentally implemented
sharing the environmental data via the MMS shared memory (shmem) sector. This worked
but conveyed no particular utility to sustain it. V2 will be retired along with the
old server hardware in ~2025 as we can no longer support using the proprietary FieldTalk
libraries for WAGO Modbus interaction.

Version 3.0 was written in 2024 and retired the proprietary FieldTalk libraries in favor
of the open source libmodbus libraries (http://libmodbus.org) for Modbus/TCP communication
with the WAGO fieldbus controllers. This allowed us to port the modsEnv code to AlmaLinux 9.3 which 
became the new OS standard at LBT for all instrument.
