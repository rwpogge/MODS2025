# azcam-mods
Version 1.1.10

Last Update: 2026 Jan 20

See [Release Notes](releases.md) for details

## Purpose

This repository contains the *azcam-mods* azcam environment, consisting of code and data files for the Large Binocular Telescope 
Observatory (LBTO) to operate the Multi-Object Double Spectrograph (MODS) science CCD detectors with STA Archon controllers.

## Status

Tested with the lab test dewar and Archon controller in the LBTO Tucson lab, ready for initial deployment at LBTO for the 
MODS2025 detecctor system upgrade. Added Telescope() and Instrument() class instances (derived from the azcam base classes) 
to access telescope and instrument configuration data stored in the LBT Instrument Interface (IIF) data dictionary for 
the image FITS headers.

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

## Logging

Logging is now in `/home/Logs/azcam/` for all MODS installations.  Permissions
must be defined to make sure all users able to start the azcam server can
write logs there.  See the MODS2025 INSTALL.md file for details.
