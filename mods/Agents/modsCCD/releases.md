# modsCCD ISIS client Releases

## Version 1 - Observing operations

### Version 1.1.6 - 2026 Feb 01
From live testing, noted that readout messages were coming so fast they caused a logjam with the MODS GUIs dispatcher.  Readout progress
was being reported every 0.2 sec.  Modified `main.c` to introduce a readout progress counter that sends a report of progress only every
5th report after the first. For a full-frame readout this is 20 reports, or about 1 per second, whcih is fast enough.

### Version 1.1.5 - 2026 Jan 24
 * `commands.c/h` - added getObsDate() method to create an "obsdate" command to query the azcam server for the current observing date. Supercedes methods that simply read the system clock, allowing more nuanced obsDate algorithms (e.g., obsDate noon-to-noon local time, or UTC but next-day after local noon, etc.)

### Version 1.1.4 - 2026 Jan 22
 * `commands.c` - rooted out a bug that caused long strings in object names, partner, pi_info, etc. string data to crash out with segmentation fault because of shamefully sloppy use of `strcpy()`. Code is now being more robust and using `snprintf()` like we should have all along.


### Version 1.1.3 - 2026 Jan 19
 * `commands.c` - in `cmd_go()` set `ccd.state=SETUP` before calling `doExposure()`
 * `commands.c/.h` - added no-op version of `cmd_comment()` for old MODS IC back compatibility
 * `clientutils.c` - in `doExposure()` call `notifyClient()` that we are initializing, this send the remote client the `EXPSTATUS=INITIALIZING` message that triggers exposure initialization in the modsUI GUI, setting the exposure counter and other paramters correctly. This was missing during live testing last night.
 * `commands.c` - `cmd_ccdbin()`, `ccd_xbin()`, and `ccd_ybin()`, restrict binning factors to 1 or 2.  Archon controllers do not support binning factors 4 and 8.


### Version 1.1.2 - 2026 Jan 18
 * Sending saveconfig (modsUI) triggered segfault in some cases, errors in other but no segfault.  Fix is to just return CMD_OK and make a soft no-op for now until we can decide how to handle this back-compatibility feature.
 * Seeing missed "done" state on 1Kx1K ROI, backed off to 0.2sec


### Version 1.1.1 - 2025 Dec 31
 * set all dataMan interfaces default OFF, removed reminder of OFF
 * set readout polling cadence to 0.5sec, was 0.2sec, now that readout is 20sec vs. 8sec, this is too fast

While the dataMan C++ code is still in place, during work with `azcam-mods` on the live system
it was determined that it was very easy to trigger the `dataMan` "proc filename" command
from azcam proper, removing all the tricky logic to detect that a file had been written
by `azcam-mods`. In other words: cut out the middleman. The default configuration of
`modsCCD` is setup to have useDM false and disabled, and the hooks removed from the 
runtime configuration files to prevent enabling it.


### Version 1.1.0 - 2025 Nov 10

Version testing dataMan hooks
 * `dataman.c` and `dataman.h` for the new python dataMan agent
 * Put hooks into `client.h`, `clientutils.h`, `main.c`, `commands.c` and `config.c`
 * A big change from the old Y4KCam system is that we put the dm config info into the `modsccd.ini` file instead of a standalone config file for just dataMan. The `dataMan.ini` for the python version uses yaml and has a very different config profile.

Note that the dataMan interface is provisional.  We are also exploring sending `proc` directives to dataMan by azcam proper, which cuts out the middleman and could
simplify the command flow of modCCD.

Version ready for live testing and debugging.

### Version 1.0.2 - 2025 Oct 15 
 
Version deployed across the MODS archon servers (`mods1blue` et al.) for initial live testing on-telescope at
LBTO.
 * fixed major bug that was causing memory problems, restored function
 * initial work with the `modsTerm` tmux session

ToDo:
 * need a clean launch script to avoid duplications
 * need a clean way to monitor up/down status across machines
 * no automatic FITS image upload to the LBTO summit repository. `dataMan` still in alpha stage in the Sandbox awaiting getting real live data with the full instrument suite to tell us what pre-processing steps are needed

### Version 1.0.0 - 2025 Aug 3

LBTO live on-floor testing


## Version 0.x - Beta Testing

Version used to check out the Archon controller installations in MODS, including interaction with mountain
machines, network issues, etc.

### Version 0.7.0 - 2025 July 27
 * many layers of changes, tested against an azcam-mods server, cleared bugs in the full command set.
 * migrated from the Sandbox to MODS2025/mods/Agents/, and passed tests for readiness to run in the lab
 * Still to come, testing with a live Archon system and working out interactions in real-time
 * Will advance to 1.0 when we get through full end-to-end testing and verification
   
### Version 0.2.0 - 2025 July 23
 * adaptation of the 2005 Y4KCam ISIS client code for Archon and the python azcam server system
