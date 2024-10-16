# modsenv - MODS environmental sensor monitor agent
Version: 3.1.1

Author: R. Pogge, OSU Astronomy (pogge.1@osu.edu)

## Overview
modsEnv collects and logs MODS instrument environmental and power-status data as an autonomous background process. Environmental sensors and power-status data are obtained from WAGO Modbus/TCP fieldbus controllers in the MODS instrument
Utility and Instrument Electronics boxes (IUB and IEBs). One instance of the modsEnv agent must be run for each MODS instrument.

Basic operation is that modsEnv for a MODS instance runs in the background started by the MODS user on the MODS instrument's data machine (mods1data or mods2data).

The modsEnv agent has two modes
 * If environmental data is requested by another data-taking system process via the ISIS server, it queries the instrument and returns status info and logs the requested info.
 * If no external requests are pending, the agent will automatically query and log environmental sensor and power status on a fixed cadence (default is 5s, but set in the runtime initialization file), then wait for the next query time or request from another data-taking system agent.

Logs are kept on the local MODSn server machine.

## Dependencies
 * `isisClient` - ISIS client library (`libisis`) on the same machine.
 * `libmodbus` - open source libmodbus libraries (http://libmodbus.org) - verified with v3.1.7 (needs `libmodbus` and `libmodbus-devel`).
 * `lbto-libtelemetry` - The library used to export environmental sensor logs as Hdf5 files.

 ## Build Instructions
Run the `build` script in the modsEnv directory.

## Maintaining
The `libtelemetry` dependency requires an updated version of the `leap-seconds.list` file to function. This file is updated every six months (in December and June) and the most recent version can be downloaded here (https://data.iana.org/time-zones/data/leap-seconds.list). The file is expected to be located at `/usr/share/lbto/UT/leap-seconds.list`, but this location can be changed at run time using the *ini file. If the leap-seconds file is not found, then the fallback leap-seconds.list file contained with this repository will be used instead.

## Usage Instructions
Run the executable with the following optional command line arguments.
- `./modsenv <rcfile>`
- `./modsenv`

If Hdf5 logging will be used, then the user running the executable must be in a usergroup named `telemetry`.

## Doxygen Documentation
Generate the documentation by running the `doxygen` command from the modsEnv directory.