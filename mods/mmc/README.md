# MODS Mechanism Control (mmc) Server
 
**Version 3.2.4**

**Updated: 2025 Dec 31 [rwp/osu]**

See [release notes](releases.md) for details.

## Overview

This is the MODS mechanism control (mmc) server, aka the "IE" host for MODS.  This code must be run on a properly configured MODS linux server that has the `modsalloc`
shared memory facility installed.  This version has been ported to AlmaLinux 9.x in 2025 for the MODS2025 CCD controller and system upgrade, and will be used at LBTO
going forward.  This port was a move from 32-bit to 64-bit architecture, so required much more testing and scruitiny of low-level routines than a simple re-compile. 
Large sections of code had to be rewritten to refactor functions from the old system to what we use with the electronics to support the Archons.

The requirement is one and only one mmcServer running per MODS instance (i.e., one each for MODS1 and MODS2), so we never run these programs directly, instead
using wrapper scripts that check that instances are not running already.

The `mmcServer` program will identify on an ISIS network as a node named `m1.ie` or `m2.ie` for MODS1 or MODS2, respectively. `mmcServer` runs on UDP port 10435. 
If you must run with a port other 10435,  change the PORT define in `modscomm.h` file in the `mods/include` directory, and recompile all codes that use this interface.

The use of UDP port bindings will usually prevent users from running a second instance trying to bind the same port on the same machine.  However, if hot spare machines
are live on the same network, care should be taken to make sure you cannot run a conflicting mmcServer instance on another machine.  

## Code

 * `mmcServers` - MODS Mechanism Control (MMC) servers, including the MODS "IE" program (`mmcServer`), the IMCS quad cell readout apps, and associated helper apps.
 * `app` - builds the `libmmcutils` library used by `mmcServer` et al.
 * `API` - builds `mmcapi.o` used by code in `mmcServers` etc.
 * `microlynx` - builds the `islmlynx` and `islmlynxShm` low-level apps for MicroLynx stepper motor controller interactions
 * `microloader` - legacy code for maintaining MicroLynx stepper motor controller microcode, replaced by the `imsTool` GUI app

