# MODS2025
Version 1.2.0

Last Update: 2025 July 20 [rwp/osu]

Lead: Richard Pogge (OSU Astronomy) pogge.1@osu.edu

## Overview
MODS2025 Project to update MODS1+2 CCD and instrument control systems at the LBT

**This our very messy working repository, not the final flight code repository that will be built later.  
Proceed at your own risk**

However, it is taking on many of the structures of the real thing, so not totally sloppy.  Sort of.

## Contents

  * `mods` - MODS instrument control and data-acquisition system
  * `ISIS` - ISIS message-passing server and client
  * `azcam` - AzCam server for the MODS Archon CCD controllers
  * `Sandbox` - place to put test codes and other bits we are using, the action is in the subfolders
  * `Config` - MODS instance runtime configuration files released for observing operations
  * `Scripts` - Startup and status scripts (coming soon...)

## Installation

See the INSTALL.md file in this repository. 

## Package dependencies

This is a running list during development phase as we discover what is missing from the 
AlmaLinux 9.x systems we're setting up.

```shell
dnf -y install tk tcsh emacs doxygen
dnf -y install readline readline-devel
dnf -y install libmodbus libmodbus-devel
dnf -y install qt6-qtbase-devel qt6-qtsvg-devel
dnf -y install libice3.7-c++ libice-c++-devel python3-ice
...
dnf -y install dnstools wget (nice, but not required)
...
Also need the local LBTO versions of hdf5 and lbto-libtelemetry, build from local rpms
```

## Authors and Contributors

- Rick Pogge (OSU Astronomy) pogge.1@osu.edu
- Xander Carroll (OSU Astronomy) carroll.892@osu.edu
- Jerry Mason (OSU Astronomy) mason.8@osu.edu
- Mike Lesser (UA)
- Matthieu Bec (LBTO) mbec@lbto.org
- Glenn Eychaner (LBTO) geychaner@lbto.org
- ...
