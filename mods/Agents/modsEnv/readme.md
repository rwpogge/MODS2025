# modsenv - MODS environmental sensor monitor agent
Version: 3.3.1 (2025 Aug 26)

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

## LBTO Telemetry System Integration 

With version 3 of `modsEnv` for the summer 2025 MODS detector controller upgrade to Archons and new head electronics boxes, we will
use `modsEnv` to integrate MODS environmental sensor data into the LBTO telemetry system. Log files will be created using 
HDF5 file streams with the `lbto-libtelemetry` libraries to store all MODS environmental sensor and power status data in the standard 
LBTO shared instrument telemetry folders (`/lbt/data/telemetry/instruments/') mounted using nfs.  We will likely phase out the ASCII text 
log files after we recommission MODS with the Archons and shake out the bugs.

In addition, with this version we are also putting all MODS environmental sensor data in the MODS shared memory segment (created at boottime
by the `modsalloc` systemd service).  This will allow injection of MODS sensor data of particular interest, for example for alarm monitors
for dewar health (LN2 reservoir temperature and vacuum pressure) into the observatory data dictionary.  Observatory alarm monitors will
be able to get current data by interrogating the data dictionary, avoiding problems of having to interrogate the instrument systems 
piecemeal, with attendant risk to interrupt real-time data acquisition.

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

system names: mods1 or mods2

telemeter name: env
```
Under each system folder the `lbttelemetry` library creates directories by date (`ccyy/mm/dd`), and a symbolic link
to the current active HDF5 stream.

A long example, the environmental sensor log for MODS1 for 2025 Aug 20 UTC would be
```
    /lbt/data/telemetry/instruments/mods1/2025/08/20/202508201618.modsl.env.h5
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

If hdf5 logging will be used, then the user running the executable must be in a usergroup named `telemetry` or it must
be run as root/sudo.  The latter is actually the most practical option, and it may be best to run `modsenv` as a systemd
service.

## Doxygen Documentation

Generate the documentation by running the `doxygen` command from the modsEnv directory.

## Dewar vacuum ion gauge configuration

**Updated/Verified**: 2025 August 6 [rwp/osu]

These are the ion gauge interface settings for each channel on each MODS:

 * Comtrol Device Master setting is: `IP address : Port Number`, see below.  All are physical port 2 of 2.
 * RS485 (2-wire) config data are: `baud`, `parity`, `data bits`, `stop bit`
 * All are setup with RS485 channel **5** using the unit front panel rotary switch

| Instrument | Channel | Comtrol Port | RS485 Config |
|:---:|:---:|:---:|:---:|
| MODS1 | Red  | 192.168.139.103:8018 | 9600/none/8/1 |
|       | Blue | 192.168.139.113:8018 | 9600/none/8/1 |
| MODS2 | Red  | 192.168.139.203:8018 | 9600/none/8/1 |
|       | Blue | 192.168.139.213:8018 | 19200/none/8/1 |

**NOTE**: The MODS2 blue dewar ion gauge is the oddball: it was never reconfigured to our standard 9600 baud because a field repair was
rushed and it fell off the to-do list (setting is non-trivial: it is made through firmware, not a front-panel setting).
