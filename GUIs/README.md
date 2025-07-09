# MODS Graphical User Interfaces

These are the graphical user interfaces for MODS observing and engineering support.

## modsUI

Current Version: 3.0.0-archon (2025 July)

`modsUI` is the primary user interface for the MODS spectrograph. It is written in C++ with the Qt6 GUI framework.

This is the instrument dashboard GUI for each MODS.  The MODS observing scripts work through the GUI.  

No GUI, no Observing (or only very painful, very inefficient observing).

Respect the MODS GUI.

## iTerm

Current Version: 2.2.0

`iTerm` is a Qt6 GUI that implements a "terminal" style command-line interface (CLI) for engineering-level communication with the MODS 
instrument control system at the "bare metal" IMPv2 command protocol level.

It connects to the local ISIS server with UDP sockets and handles command and response traffic.

If you don't know what any of that meant, this GUI is not for you.

## imsTool

Current Version: 1.3.0

`imsTool` provides engineering-level access to an individual MicroLYNX motor controller on a live system.  User can talk directly 
with the MicroLYNX using the control language, or upload/download IMS command-language programs into the MicroLYNX non-volatile
memory (NVM) registers.

imsTool's most common use is to install microcode on a replacement for a failed MicroLYNX unit.

There are only 2 people in the world allowed to use imsTool.  You are likely not one of them.
