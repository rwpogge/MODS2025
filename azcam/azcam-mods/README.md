# azcam-mods

Last Update: 2025 July 11

## Purpose

This repository contains the *azcam-mods* azcam environment.  It contains code and data files for the Large Binocular Telescope Observatory (LBTO) 
MODS camera system so we can operate the MODS science CCD detectors with STA Archon controllers.

This is a development project for the MODS2025 upgrade.

**Status:** Work in progress for MODS2025 deployment in August 2025

## Installation

Download the code (usually into the *azcam* root folder such as `c:\azcam`) and install.

```shell
cd /wherever/azcam
git clone https://github.com/mplesser/azcam-mods
python -m pip install -e ./azcam-mods
```

## Usage

Launch in an ipython interactive shell for code debugging
```shell
ipython -i -m azcam_mods.server -- -mods1b
```
Otherwise, launch as a python program.


## Development Notes

2025 Jul 8 - Starting work for the MODS Archon detector upgrade at LBTO in Summer 2025 [rwp/osu]

