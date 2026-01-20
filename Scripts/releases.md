# mods Scripts Release Notes

<b>Last Update: 2026 Jan 20 [rwp/osu]</b>

## 2026 Jan 20: go_mods script
 * all-in-one script for access to MODS servers, 1 script to replace 6

## 2026 Jan 18: updated modsAlign and modsDisp
 * `modsAlign` updated to use the azcam MEF format for the Archons, advanced version to 3.1.x
 * `modsDisp` updated to use the azcam MEF format for the Archons, version 3.1.0

## 2026 Jan 16: new scripts
 * `start_ccd` is an experimental script to start `azcam-mods` and `modsCCD` in the correct order.
 * `updateSlits` is an updated version of a script of the same name used by instrument support personnel after mask changes to commit the slitmask tables to the running MMC servers.  This version does both MODS at once, and can be run from `robs` or `obs` machines wihtout having to login to the MODS servers proper.
 * cleaned up internal comments to better match the code logic

## 2025 Dec 31: new and modified scripts
 * `stop_ccd` a convenience script to stop `modsCCD` and `azcam-mods` gracefully in a `ccdTerm` tmux session.
 * `modsStatus.pl` - added `dataMan` service to the systemd services list to monitor

## 2025 Dec 30: start_azcam
 * `start_azcam` is a safe way to ensure the azcam server is started in a `ccdTerm` tmux session by the right user on the right host, and that it is not 
already running (and by who).  Live tested at LBTO with MODS1B
 * `start_modsccd` is a safe way to start the `modsCCD` agent.  It verifies we an authorized user on an authorized machine, that the `ccdTerm` tmux session is rrunning and we are in it, that the `azcam` server is running, and that `modsCCD` is already running.
 * `ccdTerm` updates from live testing at LBTO, includes some changes in the configuration management scheme since this only runs on the MODS archon servers so we have more latitude.

## 2025 Dec 23: modsAdmin
 * After live testing at LBTO with MODS run by LBT personnel from the `robs1` machine, developed a new `modsAdmin` tool to provide a cleaner way to have a live MODS server monitor running that merged what would have to be done in 3 separate xterms. Tested and released on the public MODS tools path.
 
## 2025 Dec 19: imcsTools
 * created an updated version of `imcsTools` for the Archon system.

## 2025 Nov 25: resize issue
 * `resize` command was out of place in `mods1` and `mods2`, causing users to have to tap Enter twice to get the command to start.  Found and fixed

## 2025 Oct 14: observing scripts

Added the Public/ subfolder with the public scripts we put into `/lbt/lbto/mods/bin` for users on the
mountain observing machines.
 * `mods1.pub` and `mods2.pub` which are renamed `mods1` and `mods2` on the public path, these provide access to a safe subset of MODS server status and GUI startup functions limited by userID.  This replicates the analogous scripts from before the Archon updates
 * Wrote `mtStatus.pl` to provide a text-only presentation of the MODS server status in response to `mods1 status` from the public scripts. Verified from the osurc user account logged in on the summit `obs1` machine.
 * Modified `mods/TclTk/vueinfo.c` to change power and env status for better formatted output for the MODS status 
 * Added "temps" option to `mods1` and `mods2` public scripts to show temperatures, replaces function of `modsTemps.pl` from the pre-Archon system.  This versio is less invasive.
 * First beta versions of `acqBinoMODS` and `execBinoMODS` for tmux, and the `modsTerm` tmux session launch/attach script.

## 2025 Oct 5: updates

Updates after live testing with MODS1 and MODS2 at LBTO
 * Added display of MODS subsystem power status to modsStatus.pl.  Relies on the latest version
of `vueinfo` which provides the `pstatus` option (terse version of `vueinfo power`).
 * Added display of dewar pressure and temperature data using new `vueinfo dewars` option
 * Added resize of display as part of `mods1` and `mods2` script launch, currently using 42x128 size, likely to change (needs xterm-resize installed)
 * Start of work on `modsTerm` and `xxxBinoMODS` scripts in the [TMux Sandbox](../Sandbox/TMux), learning more about `tmux` "features", like how to get aliases we need defined in tmux session launch (tmaliases)
 * Created `.config/` in `/lbt/lbto/mods` where we will store config files for `tmux` et al. for the new script engines.

## 2025 Oct 2: Archon controller update

New versions of `mods1` and `mods2` scripts for MODS instrument 
server admin with the Archon CCD controllers, a new `modsUI` script
and other support files.  

These are only run on the two Linux instrument
servers with the same names.

Biggest changes from the old versions:
 * launches and runs in a custom `tmux` session
 * typed without argument will either start the `modsAdmin` tmux session and attach it, or attach a running `modsAdmin` session.
 * When in the `tmux` session, typed without argument gives a usage message
 * Merges MODS instrument server functions that used to be spread across 2 machines, but now a subset of those are started automatically at computer boot time, with auto-restart on failure.  Only those server functions that require manual restart or could give actionable diagnostic info if they stop are available (mmc=ID, agw, and the 2 IMCS agents).
 * The `modsUI` GUI is now launched usign the `modsUI` script, but its runtime status will be shown
 * The `acqBinoMODS` and `execBinoMODS` scripts have been moved from [modsPerl](../modsPerl) as they are being rewritten to use a `modsTerm` tmux session for execution.

