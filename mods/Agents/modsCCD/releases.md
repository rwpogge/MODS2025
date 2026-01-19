# modsCCD ISIS client Releases

## Version 1.1.2 - disable saveconfig
2026 Jan 18 [rwp/osu]
 * Sending saveconfig (modsUI) triggered segfault in some cases, errors in other but no segfault.  Fix is to just return CMD_OK and make a soft no-op for now until we can decide how to handle this back-compatibility feature.
 * Seeing missed "done" state on 1Kx1K ROI, backed off to 0.2sec


## Version 1.1.1 - disable dataMan
2025 Dec 31 [rwp/osu]
 * set all dataMan interfaces default OFF, removed reminder of OFF
 * set readout polling cadence to 0.5sec, was 0.2sec, now that readout is 20sec vs. 8sec, this is too fast

While the dataMan C++ code is still in place, during work with `azcam-mods` on the live system
it was determined that it was very easy to trigger the `dataMan` "proc filename" command
from azcam proper, removing all the tricky logic to detect that a file had been written
by `azcam-mods`. In other words: cut out the middleman. The default configuration of
`modsCCD` is setup to have useDM false and disabled, and the hooks removed from the 
runtime configuration files to prevent enabling it.


## Version 1.1.0 - Data transfer manager (dataMan) testing
2025 Nov 10 [rwp/osu]

Version testing dataMan hooks
 * `dataman.c` and `dataman.h` for the new python dataMan agent
 * Put hooks into `client.h`, `clientutils.h`, `main.c`, `commands.c` and `config.c`
 * A big change from the old Y4KCam system is that we put the dm config info into the `modsccd.ini` file instead of a standalone config file for just dataMan. The `dataMan.ini` for the python version uses yaml and has a very different config profile.

Note that the dataMan interface is provisional.  We are also exploring sending `proc` directives to dataMan by azcam proper, which cuts out the middleman and could
simplify the command flow of modCCD.

Version ready for live testing and debugging.

## Version 1.0.2 - LBTO Live on-telescope testing
2025 Oct 15 [rwp/osu]
 
Version deployed across the MODS archon servers (`mods1blue` et al.) for initial live testing on-telescope at
LBTO.
 * fixed major bug that was causing memory problems, restored function
 * initial work with the `modsTerm` tmux session

ToDo:
 * need a clean launch script to avoid duplications
 * need a clean way to monitor up/down status across machines
 * no automatic FITS image upload to the LBTO summit repository. `dataMan` still in alpha stage in the Sandbox awaiting getting real live data with the full instrument suite to tell us what pre-processing steps are needed

## Version 1.0.0 - LBTO live on-floor testing
2025 Aug 3 [rwp/osu]

Version used to check out the Archon controller installations in MODS, including interaction with mountain
machines, network issues, etc.

2025 July 27 [rwp/osu]
 * many layers of changes, tested against an azcam-mods server, cleared bugs in the full command set.

## Version 0.7.0 - First beta release
2025 July 27 [rwp/osu]
 * many layers of changes, tested against an azcam-mods server, cleared bugs in the full command set.
 * migrated from the Sandbox to MODS2025/mods/Agents/, and passed tests for readiness to run in the lab
 * Still to come, testing with a live Archon system and working out interactions in real-time
 * Will advance to 1.0 when we get through full end-to-end testing and verification
   
## Version 0.2.0 - First development version
2025 July 23 [rwp/osu]
 * adaptation of the 2005 Y4KCam ISIS client code for Archon and the python azcam server system
