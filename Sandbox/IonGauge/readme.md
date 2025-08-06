# IonGuage
Version: 1.0.0

Author: X. Carroll

## Overview
This project contains C and Python code for interacting with the Micro-Ion Plus Vacuum Guage Module. The `python` folder contains a Jupyter notebook with instructions for interacting with the ion-gauge module. The `src` folder contains the C code.

Both the Jupyter notebook and C libraries provide error handling if the ion module is disconnected or powered off.

The C code is designed to be used as a library, providing two helper functions needed to interact with the ion module. In practice, these two functions would be included in another project, or compiled into a library that is used in another project. However, a main function is included here so that the code can be tested when it is built.

## Build Instructions
Run the `make` command from the IonGuage directory.

## Usage Instructions
Run the generated executable.
- `./test`

## Ion Gauge settings
These are the ion gauge interface settings for each channel on each MODS:

* Comtrol Device Master setting is: `IP address : Port Number`, see below
* RS485 (2-wire) config data are: `baud`, `parity`, `data bits`, `stop bit`
* All are setup with RS485 channel **5** using the unit front panel rotary switch

### MODS1

#### Red Channel
 * Comtrol Device Master: 192.168.139.103:8018 (port 2)
 * RS485 setup: xxxx/none/8/1

#### Blue Channel 
 * Comtrol Device Master: 192.168.139.113:8018 (port 2)
 * RS485 setup: xxxx/none/8/1
### MODS2

#### Red Channel
 * Comtrol Device Master: 192.168.139.203:8018 (port 2)
 * RS485 setup: 9600/none/8/1

Note: MODS2 red dewar ion gauge is different than the others

#### Blue Channel 
 * Comtrol Device Master: 192.168.139.213:8018 (port 2)
 * RS485 setup: 19200/none/8/1
