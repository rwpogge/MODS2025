# imsTool - MicroLYNX motor controller tool
Version: 1.3.0

Authors: Ray Gonzalez (ex-OSU Astronomy), R. Pogge, X. Carroll

## Overview

`imsTool` is a Qt6 app that implements a basic interface GUI to communicate with an Intelligent Motion Systems MicroLYNX microstepping motorcontroller
connected to a Comtrol TCP/IP serial port server in one of the MODS IEBs.

`imsTool` provides engineering-level access to an individual MicroLYNX motor controller on a live system.  User can talk directly 
with the MicroLYNX using the control language, or upload/download IMS command-language programs into the MicroLYNX non-volatile
memory (NVM) registers.

imsTool's most common use is to install microcode on a replacement for a MicroLYNX unit.

## Code Overview
- readme.md - This file
- releases.md - Comments on new versions
- ims.pro - Uses qmake to build the Makefile
- main.cpp - ims main starter
- client.cpp - GUI ims dialog and display methods
- client.h - GUI class definitions
- microLynxErrors.h - GUI error definitions
- timeMethods.h - secs, msecs, and usecs

## Build Instructions
Run the following commands from the imsTool directory:
1) `conda deactivate` - to avoid conflicts with anaconda3 Qt stuff
2) `qmake -o Makefile`
3) `make`

## Usage Instructions
Run the executable with the following optional command line arguments.
- `./imsTool <host> <port>`
- `./imsTool`
