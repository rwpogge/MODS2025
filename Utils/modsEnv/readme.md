# modsEnv - MODS environmental sensor logger

Version 2.2.0 [2014 July 15]

Author: Richard Pogge (OSU Astronomy) pogge.1@osu.edu

Last Build: 2016 July 1 [LBT Flight]

## Overview

modsEnv is a MODS data-taking system agent that queries all systems for enviromental sensor data (temperature, humidity, liquid pressure, etc.) 
and logs that information in an ASCII text file.

It communicates with MODS system via UCP socket connection to an ISIS server using the ICIMACSv2 command protocol.

The modbus/TCP layers used the proprietary FieldTalk libraries for which OSU had a developer license in the early 2000s. Because of
change of ownership of the libraries and with it changes to terms and conditions OSU legal cannot support, we are retiring this version
of modsEnv and rewriting it to use the open source libmodbus libraries.
