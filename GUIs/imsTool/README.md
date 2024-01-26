# imsTool - MicroLYNX motor controller tool

Version: 1.1.0
Author: Ray Gonzalez (ex-OSU Astronomy)

## Overview

imsTool provides a basic interface GUI to communicate with an IMC MicroLYNX microstepping motorcontroller
connected to a Comtrol TCP/IP serial port server on the network.

imsTool is used to provide engineering-level access to an individual motor controller on a live system.  User can talk directly 
with the MicroLYNX using the control language, or upload/download IMS command-language programs into the MicroLYNX non-volatile
memory (NVM) registers.

imsTool is how we update microcode on the MODS mechanism motor controllers after we swap out a failed MicroLYNX unit.
