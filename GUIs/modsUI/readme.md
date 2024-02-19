# modsUI - MODS spectrograph user interface

Version: 2.0 bino

Author: Richard Pogge (OSU Astronomy) pogge.1@osu.edu

Original Build: 2009 Feb 24

Last Build: 2020 Aug 15

## Overview

modsUI is the primary user interface for the MODS spectrograph. It is written in C++ with the Qt4 GUI framework.

Longer description to follow.


## Porting Changes
The following places produced warnings and were commented out. Eventually it would be more efficent to refactor the code in these places to use if statements rather than switches. These locations are marked with TODOs.
- setup/fileconfig.cpp 154: cmdDone and cmdFault flags.
- setup/instconfig.cpp 203: cmdDone flag.
- setup/configmanager.cpp 421: cmdFault flag.
- monitor/envpanel.cpp 517: cmdDone and cmdFault flags.
- instutils/llbpanel.cpp 355: cmdDone and cmdFault flags.
- instutils/iubpanel.cpp 420: cmdDone and cmdFault flags.
- instutils/iebpanel.cpp 274: cmdDone and cmdFault flags.
- dashboard/modschannel.cpp 685: cmdWarn flag.
- dashboard/focalplane.cpp 352: cmdDone flag.

In several instances QString::simplified was used - but the return value wasn't. These locations were fixed and marked with TODOs.


- dashboard/modschannel.cpp 2627: Ambigous braces.
