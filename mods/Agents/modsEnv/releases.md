# modsenv Release Notes
Last Build: 2025 Oct 16

## Version 3.3.5
2025 Oct 16

Fixed bug in blue HEB power state WAGO read for archon and ion gauge during
live testing at LBTO with the Archons.

## Version 3.3.4
2025 Oct 4

Changed logic for reporting the various power states for subsystems that are controlled by the utility box.
The actual power state is
<pre>
   powerState = f(switchState,breakerState)
</pre>
 * If switch is on and breaker is on, power = 1 (On)
 * If switch is off and breaker is off, power = 0 (Off)
 * If switch is on and breaker is off, power = -1 (Fault)
 * If switch is off and breaker is on, power = -1 (Fault - probably means power relay fused on)


## Version 3.3.3
2025 Sept 30

Added time delay of 10msec in `modbusutils.c` between calls to `modbus_new_tcp()` and `modbus_connect()` to
break up an apparent race condition when opening a WAGO connection. Error reported was
<pre>
   ERROR: Cannot connect to WAGO host 192.168.139.242: Operation now in progress 
</pre>
Adding the `usleep()` gave enough of a beat between opening and connecting the WAGO TCP socket.  Live
test with fast cadence (5s) showed no recurrence.  Before was every dozen connections (or faster).


## Version 3.3.2
2025 Sept 14

Fixed bug in `ionutils.c` - `getIonPressure()` was not closing TCP socket on error initializing the socket connection, leading
to a stale socket deadlocking the Comtrol TCP/RS485 port.

Fixed annoyance in `clientutils.c` in warning messages not including \n so the CLI output was messy, obfuscating warnings 
during engineering diagnosis of the problem above.

Increased `ION_TIMEOUT_LENGTH` in `ionutils.h` from 1 to 2 seconds. This is to address timeouts on the MODS2 red dewar ion
gauge resulting in occasional read faults.


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
