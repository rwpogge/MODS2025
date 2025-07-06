# MODS Utility Libraries

**Updated: 2025 July 5 [rwp/osu]**

Various libraries we use in common across core MODS data-taking system codes.

This is work in progress while the MODS2025 AlmaLinux 9 port is proceeding.  Expect frequent changes 
until we gather all these various bits together into the flight repository...

## Libraries

 * `ISLUtils` - makes `libislutils.a` for shared memory et al.
 * `INSTRUtils` - make `libinstutils.a` for the MicroLynx stepper motor controller interface
 * `iifUtils` - makes `libiifutils.a` (MODS/IIF telescope control system interface
 * `skyUtils` - MODS time and celestial calculation utility functions
 * `ISLTimes` - makes `libisltimes.a` for legacy time MST, GST, etc. (legacy)
 * `LogUtils` - makes `liblogutils.a` for MMC logging (legacy)

