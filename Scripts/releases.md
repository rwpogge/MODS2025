# mods Scripts Release Notes

<b>Last Update: 2026 Apr 15 [rwp/osu]</b>

## 2026 Apr 15: modsAlign v3.3.1
Updates from live testing
 * Bug in the autoscaling code for slits when short exposures lead to low signal levels in bias-subtracted images. Fixed by computing the Poisson estimated noise with the absolute value of the median signal level to avoid taking the square root of a negative number.  Tested and released.


## 2026 Apr 12: modsAlign v3.3.0
Updates from shared-risk observing during the April OSURC run

 * For longslit acquisitions which now use the full 3Kx3K imaging CCD ROI, put in auto zoom (ds9: `zoom to 1` following `zoom to fit`) that zooms in on the central slit used for acquisitions. A little closer zoom than the raw 1Kx1K acquisition, but what the observers were using during an actual acquisition.  This autozoom eliminates at least 3-4 mouse clicks depending on how the user would do it before.
 * the `-r` option now works with thru-slit confirmation images, providing correct autoscaling and computation of small correction offsets.  The only oddity is that if using a slit offset on the original mask+field acquisition longslit pair (like the usual `-y 11` to avoid the quadrant boundaries), you have to include it *first* on the command line (`modsAlign -y 11 -r mods1r.20260410.0027.fits`), otherwise it raises an exception and quits. Will fix the logic on the next iteration
 * Restored `-v/--verbose` mode. An LBTO update for DS9 timeouts was assigning the `verbose` boolean in main to the `debug=` option for their function, which overrode the internal verbose output mode and made it damned near impossible to debug the issues described above. Definitely bad programming style to overload things without testing.
 * Tested on-site (via vnc) with all standard and a couple of non-standard acquisition mods (longslit, std star, MOS mask, longslit refine, MOS turbo mode). 
 * Minor code cleanup and internal docs in progress


## 2026 Feb 20: modsAlign v3.2.2
Updates from tests with live images
 * Change min scaling for mask images to 0 since (a) we bias subtract and (b) need to see sky in boxes
 * Bug, forgot to transform reference position for xSlit,ySlit coords
 * np.linalg.lstsq() changed behavior after numpy v2, coeffs coming back goofy, have to do a handstand (`coeffs.ravel().tolist()[0]`) to get a simple list of coefficients to process and pass back. 
 * added `medSlitScale()` function to do more sensible display scaling when we just have a slit image - derived from `slitRegion()` and `medImgScale()` functions. Rescales after it displays the polygon region around the slit location.

## 2026 Feb 18: modsAlign v3.2.0
Updated `modsAlign` to v3.2 to address issues found during recent ECD and cloudy nights.
 * Changed code for the present position for the `-r` option to work with any ROI, not just 1024x1024. * Introduced using the field image scaling when displaying the mask images for 2-image acquisitions
 * Fixed bug found by Olga to explain why field image wasn't displaying in -r option.


## 2026 Jan 25: modsStatus.pl and ccdStatus scripts
 * Updated `modsStatus.pl` to version 2 to include `azcam-mods` and `modsCCD` status, cleaned up display
 * Added `ccdStatus` csh script to go on the Archon servers to provide remote status info for `azcam-mods` and `modsCCD`
 * Added `binoMODS` csh script to `Public/` used to give a fast simultaneous response to prompts by `acqMODS` or `execMODS` run in binocular mode with `acq/execBinoMODS`.  This is a convenience function for observers.


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

