# modsUI - MODS spectrograph user interface

Version: 3.2.3-archon

**Updated: 2026 Jan 24**

Author: Richard Pogge (OSU Astronomy) pogge.1@osu.edu

See [release notes](releases.md) for details

## Overview

modsUI is the primary user interface for the MODS spectrograph. It is
written in C++ with the Qt6 GUI framework.

Longer description to follow later..

## Build Instructions
Run the following commands from the modsUI directory.
1) `conda deactivate` (avoids anaconda3 qt5.x), we want qt6.6
2) `qmake -o Makefile`
3) `make`

## Usage Instructions
Run the executable with the following optional command line arguments.
- `./modsUI <config_file> <font_size>`
- `./modsUI <config_file>`
- `./modsUI`

- <config_file>: A path to the configuration *.ini file.
- <font_size>: The font size that should be used (default=12).
