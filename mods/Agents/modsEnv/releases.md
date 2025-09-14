# modsenv Release Notes
Last Build: 2024 Sept 14

## Version 3.3.2
2025 Sept 14

Fixed bug in `ionutils.c` - `getIonPressure()` was not closing TCP socket on error initializing the socket connection, leading
to a stale socket deadlocking the Comtrol TCP/RS485 port.

Fixed annoyance in `clientutils.c` in warning messages not including \n so the CLI output was messy, obfuscating warnings 
during engineering diagnosis of the problem above.


## Version 3.3.1
2025 Aug 25/26

Added `ionutils.c` and `ionutils.h` for dewar vacuum ionization gauge I/O integration.  Tested on
MODS1 at LBTO in the highbay on the cart on Aug 26, confirmed ion gauge operation.  Time to readout
the pressure was timed at 0.20+/0.02 sec, so reduced the default comm timeout from 5 seconds to 1
(~5x margin) and recompiled.  timing hooks are in clientutil.c and commented out unless needed
for diagnostics.  Will go back and put that under the debug flag.  Still need to remove error
messages embedded inside `ionutils.c` for the next, we hope operational, release.

## Version 3.2.2
2025 Aug 20

HDF5 logging enabled and tested both in-situ and with the nfs-mounted telemetry drive at LBTO.
Required code in logutils.c and updated .ini files to clear up details of how the LBT lib-telemetry
code created the directories.  For HDF5 to work, modsenv must be executed as root or sudo. Most
likely mode is to run as a systemd service.


## Version 3.2.1
2025 Aug 19

Changes after field test with live MODS1 on its cart in the LBTO high-bay area. Numerous bugs squashed, clearly 
one RTD in the MODS1 IUB is faulty (AGHSTEMP reads -184C), and we have not yet implemented reading out the ionization
gauges to get dewar pressure. ASCII text logging works, but HDF5 logging is not quite there.

Converting raw RTD data from the WAGO to temperature using the correct formula that wraps for negative temps
(copied the `ptRTD2C()` function from the mmcServer).

## Version 3.2.0
2025 Aug 17

Added updates for the Archon head electronics boxes and new power controls in the utility box implemented at LBTO in
early August.  Enabled pushing environmental data into the MODS shared memory segment, which now requires that modsEnv
be running on a host with the `modsalloc` service enabled.  Added new data to the LBTO HDF5 telemetry stream.

## Version 3.1.1
Wednesday, August 14th, 2024

Added a fallback leap-seconds file. The fallback file ensures that if a leap-seconds file is not found, the program will not crash.

## Version 3.1.0
Monday, July 15th, 2024

Enviornmental sensing data can now be logged to Hdf5 files in the LBTO telemetry format.

## Version 3.0
Monday, Feb 20th, 2024

Version 3.0 was written in 2024 to retire use of the proprietary FieldTalk libraries in favor of the open source `libmodbus` libraries 
(http://libmodbus.org) for Modbus/TCP communication with the WAGO fieldbus controllers. This allowed us to port the `modsEnv` code to 
AlmaLinux 9.x which is the new OS standard at LBT for all instruments and subsystems.  **Version 3 replaces all Version 2.x**

- FieldTalk -> libmodbus

## Version 2.0
2013

Version 2.0 was written in 2013 to support the commissioning of MODS2 and initiation of binocular operation at the LBT.  At that time we 
experimentally implemented sharing the environmental data via the MMS shared memory (shmem) segment. This worked but conveyed 
no particular utility to sustain it. V2 will be retired along with the old server hardware in 2025 as we can no longer support
using the proprietary FieldTalk libraries for WAGO Modbus interaction.

## Version 1.X
Wednesday Dec 15th, 2010

Introduced UseTTY for non-interactive (background) operation.

## Version 1.X
Monday Jun 21st, 2010

New client application.
