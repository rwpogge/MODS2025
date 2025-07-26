# libazcam - azcam client utility library

Version: 2.0.0

Last Update: 2025 July 26 [rwp/osu]

Heritage: AzCamUtils library for the Y4KCam at the CTIO 1m with a Windows AzCamServer and ARC Gen3 (May 2005)

## Overview

Updated version of the Y4KCam `libazcam` for the MODS Archon controller update project in 2025 at the
Large Binocular Telescope Observatory.  The azcam server we use today is very different than in 2005, 
so while the shell is the same, the contents have changed significantly.  We are now talking to a 
python azcam server running on Linux, with a custom `azcam-mods` module built for the particularities 
of the MODS instruments at the Large Binocular Telescope.

One major change is in the exposure sequence, and provision of robust and reasonably granular reporting
of exposure state information.  This has simplified the event sequencing that we'll use in the
`modsCCD` client application.

Here we've reworked and added features to `libazcam` to update it.

## Contents

 * azcamutils.c
 * ccdtemp.c
 * exposure.c
 * image.c
 * iosubs.c
 * server.c

## Build instructions

Type
```shell
./build
```
in the current directory to build the package in place.
