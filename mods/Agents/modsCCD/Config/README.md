# modsCCD Agent Runtime Configuration Files

**Updated: 2025 July 27 [rwp/osu]**

These are files used by modsCCD agents for each of the binocular MODS channels.

## Config directory

For a "flight" MODS data-taking system, the `modsCCD` agent will look in `/home/dts/Confg/` for the approriate
configuration file.  There are 4 variants, each named for a MODS channel (e.g., `_MODS1B`, `_MODS2R`, etc.).

Two modsCCD agents are needed to run for each MODS1 and MODS2, so 4 total agents for a full binocular system.

On the azcam server machine for each mods channel (e.g., host `mods1blue`) make a symbolic link pointing to
the version for that machine.  For example, for the MODS1B azcam server machine:
```shell
cd /home/dts/Config
ln -s ./modsCCD/modsccd_MODS1B.ini modsccd.ini
```
That way when the modsCCD instance is launched, it will always come up as the correct instance.

This is part of why in the earlier phases of the upgrade project we have one azcam server
machine for each MODS channel.

### modsCCD Agent Runtime Configuration Files:

 * `modsccd.ini` - runtime configuration files:
   * `modsccd_MODS1B.ini` - MODS1B
   * `modsccd_MODS1R.ini` - MODS1R
   * `modsccd_MODS2B.ini` - MODS2B
   * `modsccd_MODS2R.ini` - MODS2R


