# azcam-mods

Last Update: 2025 July 25

## Purpose

This repository contains the *azcam-mods* azcam environment.  It contains code and data files for the Large Binocular Telescope Observatory (LBTO) 
MODS camera system so we can operate the MODS science CCD detectors with STA Archon controllers.

This is a development project for the MODS2025 upgrade.

**Status:** Work in progress for MODS2025 deployment in August 2025

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

2025 July 23 - multiple updates while developing the C++ client app that will sit between MODS and the azcam server
2025 July 18 - got telescope_mods.py working with the LBT python-iif code to pull TCS info into azcam
2025 July 8 - Starting work for the MODS Archon detector upgrade at LBTO in Summer 2025 [rwp/osu]

