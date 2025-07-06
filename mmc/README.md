# MODS Mechanism Control (mmc) Server
 
**Version 3.1**

**Updated: 2025 July 3 [rwp/osu]**

Ported to AlmaLinux 9.x in 2025 for the MODS2025 CCD controller and system upgrade.

## Overview

This is the MODS mechanism control (mmc) server, aka the "IE" host for MODS.  This code must be run on a properly configured MODS linux server that has the `modsalloc`
shared memory facility installed.

The requirement is one and only one mmcService running per MODS instance (i.e., one each for MODS1 and MODS2).

The `mmcService` program will identify on an ISIS network as a node named `m1.ie` or `m2.ie` for MODS1 or MODS2, respectively. `mmcService` runs on UDP port 10435. 
If you must run with a port other 10435,  change the PORT define in `modscomm.h` file in the `mods/include` directory, and recompile all codes that use this interface.

The use of UDP port bindings will usually prevent users from running a second instance trying to bind the same port on the same machine.  However, if hot spare machines
are live on the same network, care should be taken to make sure you cannot run a conflicting mmcService instance on another machine.  

## Code

 * `mmcServers` - MODS Mechanism Control (MMC) servers, including the MODS "IE" program (`mmcService`), the IMCS quad cell readout apps, and associated helper apps.
 * `app` - builds the `libmmcutils` library used by `mmcService` et al.
 * `API` - builds `mmcapi.o` used by code in `mmcServers` etc.
 * `microlynx` - builds the `islmlynx` and `islmlynxShm` low-level apps for MicroLynx stepper motor controller interactions
 * `microloader` - legacy code for maintaining MicroLynx stepper motor controller microcode, replaced by the `imsTool` GUI app

