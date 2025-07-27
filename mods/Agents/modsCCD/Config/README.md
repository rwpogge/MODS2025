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

 * `modsccd_MODS1B.ini` - MODS1B
 * `modsccd_MODS1R.ini` - MODS1R
 * `modsccd_MODS2B.ini` - MODS2B
 * `modsccd_MODS2R.ini` - MODS2R
 * `modsccd.ini` - OSU lab testing file

## Installation

Copy the contents of this `Config/` folder into a subfolder in the public `/home/dts/Config` directory
name `modsCCD`:
```shell
  cd /home/dts/Config
  mkdir modsCCD  (if it does not already exist)
  cd modsCCD
  cp MODS2025/mods/Agents/modsCCD/Config/modsccd_MODS*.ini .
  cd ..
```
Then make a link pointing to the correct file for the particular MODS channel:
```
  cd /home/dts/Config
  ln -s modsCCD/modsccd_MODS2R.ini modsccd.ini
```
for example, to setup that machine's `modsCCD` agent to run the MODS2 red channel archon controller.

  
## Configuration File Structure
A typical runtime config file has the following structure:
```
#
# modsCCD client runtime config file
#
# R. Pogge, OSU Astronomy Dept.
# pogge.1@osu.edu
# Updated: 2025 July 24 [rwp/osu]
#
# MODS1B LBTO Archon system
#
################################################################

# modsCCD's ISIS client info (Host=localhost is implicit)

ID   M1.BC
Port 10401

# Application Mode: either STANDALONE or ISISclient

#Mode Standalone
Mode ISISclient

# Instrument Name

Instrument MODS1B

# azcam server info

AzCamHost 192.168.139.132
AzCamPort 2402
TimeOut 10
   
# ISIS server info - only used if Mode=ISISclient

ISISID   IS
ISISHost 192.168.139.130
ISISPort 6600

# Runtime flags 

VERBOSE
#nolog
#debug
```
As this example shows, the goal is that runtime configuration files
are easily read and created by humans.  A common syntax makes
maintenance of many clients easier.

