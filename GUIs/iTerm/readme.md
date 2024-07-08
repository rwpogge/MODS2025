# iTerm - ICIMACS terminal
Version: 2.2.0

Author: Richard Pogge (OSU Astronomy) pogge.1@osu.edu

## Overview
iTerm is a Qt6 GUI that implements a "terminal" style command interface to communicate with the MODS instrument control system at the ICIMACS command protocol level (aka "bare metal").

It connects to the local isis server with UDP sockets and handles command and response traffic.

## Build Instructions
Run the following commands from the iTerm directory.
1) `qmake -o Makefile`
2) `make`

## Usage Instructions
Run the executable with the following optional command line arguments.
- `./iterm <config_file> <font_size>`
- `./iterm <font_size>`
- `./iterm`

- <config_file>: A path to the configuration *.ini file (default='./iterm.ini').
- <font_size>: The font size that should be used (default=12).