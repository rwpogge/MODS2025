# iTerm - ICIMACS terminal
Version: 2.1.0

Author: Richard Pogge (OSU Astronomy) pogge.1@osu.edu

## Overview
iTerm is a Qt5 GUI that implements a "terminal" style command interface to communicate with the MODS instrument control system at the ICIMACS command protocol level (aka "bare metal").

It connects to the local isis server with UDP sockets and handles command and response traffic.

## Build Instructions
Run the following commands from the iTerm directory.
1) `qmake -o Makefile`
2) `make`