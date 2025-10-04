# mods Scripts Release Notes

<b>Last Update: 2025 Oct 4 [rwp/osu]</b>

## 2025 Oct 4: modsStatus.pl

Added display of MODS subsystem power status to modsStatus.pl.  Relies on the latest version
of `vueinfo` which provides the `pstatus` option (terse `vuestatus power`).

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

