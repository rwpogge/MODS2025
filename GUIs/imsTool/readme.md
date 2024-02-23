# imsTool - MicroLYNX motor controller tool
Version: 1.2.0

Author: Ray Gonzalez (ex-OSU Astronomy)

## Overview
imsTool is a Qt5 app that implements a basic interface GUI to communicate with an IMC MicroLYNX microstepping motorcontroller connected to a Comtrol TCP/IP serial port server on the network.

imsTool is used to provide engineering-level access to an individual motor controller on a live system.  User can talk directly 
with the MicroLYNX using the control language, or upload/download IMS command-language programs into the MicroLYNX non-volatile
memory (NVM) registers.

imsTool is how we update microcode on the MODS mechanism motor controllers after we swap out a failed MicroLYNX unit.

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
Run the following commands from the imsTool directory.
1) `qmake -o Makefile`
2) `make`