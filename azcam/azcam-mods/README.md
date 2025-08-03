# azcam-mods
Version 1.0.5

Last Update: 2025 Aug 3

## Purpose

This repository contains the *azcam-mods* azcam environment.  It contains code and data files for the Large Binocular Telescope Observatory (LBTO) 
MODS camera system so we can operate the MODS science CCD detectors with STA Archon controllers.

This is a development project for the MODS2025 upgrade.

## Status

Tested with the lab test dewar and Archon controller in the LBTO Tucson lab, ready for initial deployment at LBTO for the MODS2025 detecctor system upgrade

## Installation

Download the code into the *azcam* root folder (e.g., `/home/user/azcam`) and install:

```shell
cd /home/user/azcam
<copy from the Git respository>
python -m pip install -e ./azcam-mods
```

## Usage

Launch in an ipython interactive shell for code debugging
```shell
ipython -i -m azcam_mods.server -- -mods1b
```
Otherwise, launch as a python program, either foreground or background.

## Development Notes

### 2025 Aug 3
Final lab checkout complete, ready to deploy on the mountain computers alongside physical installation
of the MODS Archon controllers in MODS1 and MODS2

### 2025 Aug 1
Extensive testing with the live Archon system in the LBTO Tucson lab with the MODS test dewar (3x8K CCD).
Advanced a working version ready for initial deployment at LBTO with the MODS instruments to prepare for
re-commissioning in September.  Verified to work with the new `modsCCD` IC emulation agent that will be the
primary interface between the MODS azcam server and the existing instrument control and data acquisition 
system.

Remaining issues:
 * No STOP command equivalent in the azcam server, need to research this
 * No instrument status integration yet (no live system to help develop it)
 * If there is a problem on write and the expose1() thread hangs, as yet no obvious way to recover the data from the Archon buffer, but still early days after identifying the issue today.

### 2025 July 26
Server tested with the `modsCCD` azcam client that will take the place of the DOS "IC" basic program functions.
Version ready for live testing in the Tucson lab before Archon installation in MODS at LBTO first week of 
August.

### 2025 July 23
Multiple updates while developing the C++ client app that will sit between MODS and the azcam server

### 2025 July 18
Got telescope_mods.py working with the LBT python-iif code to pull TCS info into azcam

### 2025 July 8
Starting work for the MODS Archon detector upgrade at LBTO in Summer 2025 [rwp/osu]

