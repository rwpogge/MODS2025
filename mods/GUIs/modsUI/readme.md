# modsUI - MODS spectrograph user interface

Version: 3.2.6-archon

**Updated: 2026 Sept 11**

Author: Richard Pogge (OSU Astronomy) pogge.1@osu.edu

See [release notes](releases.md) for details

## Overview

modsUI is the primary user interface for the MODS spectrographs. It is written in C++ with the Qt6 GUI framework.  It was updated in Summer 2025 to port from Qt4 to Qt6, and to add changes for the Archon CCD controller upgrade installed in 2026.

## Build Instructions
Run the following commands from the modsUI directory:
1) `conda deactivate` (avoids anaconda3 qt5.x), we want qt6.6
2) `make clean`
3) `qmake -o Makefile`
4) `make`
5) `cp modsUI ~/mods/bin`

Typical build time is about 3 minutes.  That last step installs the public version in the default path. 

## Usage Instructions

### Engineering Tests

To run the `modsUI` app directly, use the following optional command line arguments.
- `./modsUI <config_file> <font_size>`
- `./modsUI <config_file>`
- `./modsUI`

- <config_file>: A path to the configuration *.ini file.
- <font_size>: The font size that should be used (default=12).

Note that only one (1) instance of a the `modsUI` GUI must be running at a time, so direct engineering execution must be undertaken by authorized LBTO or MODS/OSU personnel
for engineering check out only.

### Normal Use

Only one (1) instance of `modsUI` may be running at a time. To ensure this we execute `modsUI` using a wrapper script named `modsGUI` that avoids conflicts.  See
See [`modsUI.sh`](../../Scripts/modsUI.sh) for details.
