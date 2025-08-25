# modsenv - MODS environmental sensor monitor agent
Version: 3.3.1 (2025 Aug 25)

Authors: R. Pogge & X. Carroll, OSU Astronomy

## Overview
modsEnv collects and logs MODS instrument environmental and power-status data as an autonomous background process. Environmental sensors and 
power-status data are obtained from WAGO Modbus/TCP fieldbus controllers in electronics boxes on each MODS (utility, IEBs, and HEBs).
One instance of the modsEnv agent must be run for each MODS instrument.

Basic operation is that modsEnv for a MODS instance runs in the background started by the MODS user on each MODS server machine 
(host `mods1` or `mods2`).

The modsEnv agent has two modes
 * If environmental data is requested by another data-taking system process via the ISIS server, it queries the instrument and returns status info and logs the requested info.
 * If no external requests are pending, the agent will automatically query and log environmental sensor and power status on a fixed cadence (default is 60s, but set in the runtime initialization file), then wait for the next query time or request from another data-taking system agent.

Logs are kept on the local MODSn server machine in `/home/Logs/Env`.

## Dependencies
 * `isisClient` - ISIS client library (`libisis`) on the same machine.
 * `libmodbus` - open source libmodbus libraries (http://libmodbus.org) - verified with v3.1.7 (needs `libmodbus` and `libmodbus-devel`).
 * `lbto-libtelemetry` - The library used to export environmental sensor logs as hdf5 files.

These are best loaded on the mountain to make sure everything is correct. Once at LBTO, you don't have to mess with VPN or access to the local repositories.  Those
are installed with 
```shell
dnf config-manager --enablerepo=lbto,lbto-updates

dnf install -y http://yumsrv.tucson.lbto.org/rpms/almalinux9/release/lbto-yum-repository-1-1.noarch.rpm
```
before you add the `lbto-libtelemetry` bits.

Then, install LBTO common versions of HDF5 and the observatory telemetry library `libtelemetry`:
```shell
dnf -y install hdf5-1.12.1-11_lbto.el9.x86_64.rpm
dnf -y install hdf5-devel-1.12.1-11_lbto.el9.x86_64.rpm

dnf -y install lbto-libtelemetry-leapseconds-5-0.el9.x86_64.rpm
dnf -y install lbto-libtelemetry-5-0.el9.x86_64.rpm
dnf -y install lbto-libtelemetry-devel-5-0.el9.x86_64.rpm
```

## Build Instructions
Run the `build` script in the modsEnv directory.

## Maintaining
The `libtelemetry` dependency requires an updated version of the `leap-seconds.list` file to function. This file is updated every six months (in
December and June) and the most recent version can be downloaded here (https://data.iana.org/time-zones/data/leap-seconds.list). The file is 
expected to be located at `/usr/share/lbto/UT/leap-seconds.list`, but this location can be changed at run time using the *ini file. If 
the leap-seconds file is not found, then the fallback leap-seconds.list file contained with this repository will be used instead.

## LBTO telemetry

### Directory Structure

To create telemetry files with a path and name for LBTO telemetry, it would be setup like this:
```
base path: /lbt/data/telemetry/instruments/

system name: modsl (MODS1) or modsr (MODS2)

telemeter name: env
```
Under each system folder the `lbttelemetry` library creates directories by date (`ccyy/mm/dd`), and a symbolic link
to the current active HDF5 stream.

A long example, the environmental sensor log for MODS1 for 2025 Aug 20 UTC would be
```
    /lbt/data/telemetry/instruments/modsl/2025/08/20/202508201618.modsl.env.h5
```
If not already done, you do need to install the mounts for the telemetry nfs mount:
```
sudo dnf install lbto-nfs-mountain-telemetry
```

### Run as root

For HDF5 files to be created, `modsenv` needs to be run as root.  We will probably do this by running it as a systemd service.

## Usage Instructions
Run the executable with the following optional command line arguments.
- `./modsenv <rcfile>`
- `./modsenv`

If hdf5 logging will be used, then the user running the executable must be in a usergroup named `telemetry`.

## Doxygen Documentation
Generate the documentation by running the `doxygen` command from the modsEnv directory.
