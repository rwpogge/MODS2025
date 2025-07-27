# modsHEB Agent Runtime Configuration Files

**Updated: 2025 July 27 [rwp/osu]**

These are files used by modsHEB agents for each of the binocular MODS channels.

## Config directory

For a "flight" MODS data-taking system, the `modsHEB` agent will look in `/home/dts/Confg/` for the approriate
configuration file.  There are 4 variants, each named for a MODS channel (e.g., `_MODS1B`, `_MODS2R`, etc.).

Two modsHEB agents are needed to run for each MODS1 and MODS2, so 4 total agents for a full binocular system.

On the azcam server machine for each mods channel (e.g., host `mods1blue`) make a symbolic link pointing to
the version for that machine.  For example, for the MODS1B azcam server machine:
```shell
cd /home/dts/Config
ln -s ./modsHEB/modsheb_MODS1B.ini modsheb.ini
```
That way when the modsHEB instance is launched, it will always come up as the correct instance.

This is part of why in the earlier phases of the upgrade project we have one azcam server
machine for each MODS channel.

### modsHEB Agent Runtime Configuration Files:

 * `modsheb_MODS1B.ini` - MODS1B
 * `modsheb_MODS1R.ini` - MODS1R
 * `modsheb_MODS2B.ini` - MODS2B
 * `modsheb_MODS2R.ini` - MODS2R

## Installation

Copy the contents of this `Config/` folder into a subfolder in the public `/home/dts/Config` directory
name `modsHEB`:
```shell
  cd /home/dts/Config
  mkdir modsHEB  (if it does not already exist)
  cd modsHEB
  cp MODS2025/mods/Agents/modsHEB/Config/modsheb_MODS*.ini .
  cd ..
```
Then make a link pointing to the correct file for the particular MODS channel:
```
  cd /home/dts/Config
  ln -s modsHEB/modsheb_MODS2R.ini modsheb.ini
```
for example, to setup that machine's `modsHEB` agent to run the MODS2 red channel archon controller.

  
## Configuration File Structure
A typical runtime config file has the following structure:
```
```

