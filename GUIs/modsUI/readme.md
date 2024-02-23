# modsUI - MODS spectrograph user interface
Version: 2.1.0

Author: Richard Pogge (OSU Astronomy) pogge.1@osu.edu

## Overview
modsUI is the primary user interface for the MODS spectrograph. It is written in C++ with the Qt5 GUI framework.

Longer description to follow.

## Build Instructions
Run the following commands from the modsUI directory.
1) `qmake -o Makefile`
2) `make`

## Usage Instructions
Run the executable with the following optional command line arguments.
- `./modsUI <config_file> <font_size>`
- `./modsUI <config_gile>`
- `./modsUI`

- <config_file>: A path to the configuration *.ini file (default='./iterm.ini').
- <font_size>: The font size that should be used (default=12).