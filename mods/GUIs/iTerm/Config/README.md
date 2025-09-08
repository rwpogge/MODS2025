# iTerm GUI Runtime Configuration Files

**Updated: 2025 Sept 8 [rwp/osu]**

These are files used by iTerm GUIs for each of the MODS instances (MODS1 and
MODS2)

## Config directory

For a "flight" MODS data-taking system, the `iTerm` GUI will look in `/home/dts/Confg/` for the approriate
configuration file.  There are 2 variants, one each for MODS1 and MODS2.

On the azcam server machine, make a symbolic link for the MODS instance:
```shell
cd /home/dts/Config
ln -s ./MODS1/iterm_MODS1.ini iterm.ini
```
That way when the iTerm instance is launched, it will always come up as the correct instance.

## Installation

Copy the contents of this `Config/` folder into a subfolder in the public `/home/dts/Config` directory:
```shell
  cd /home/dts/Config
  cp ~/MODS2025/mods/GUIs/iTerm/Config/iterm_MODS*.ini .
  mv iterm_MODS1.ini MODS1/
  mv iterm_MODS2.ini MODS2/
```
Then make a link pointing to the correct file for the particular MODS channel:
```
  cd /home/dts/Config
  ln -s MODS1/iterm_MODS1.ini iterm.ini
```
for example, to setup that machine's `iTerm` GUI to talk to the MODS1 system.

## Configuration File Structure
A typical runtime config file has the following structure:

#
# ITerm Runtime Configuration File
#
# R. Pogge, OSU Astronomy Dept.
# pogge.1@osu.edu
# 2025 Sept 8 [rwp/osu]
#
# Session Info

Name ITerm Command Console # Toolbar name

# Our ID and Port number

ID M1.IT
Port 0      # 0 means "any free port"

# ISIS Server Info

ISISID IS
ISISHost 192.168.139.130 # mods1 server
ISISPort 6600

# A default remote host target, none if omitted

RemHost M1.IE

# Runtime Flags

TimeTag
LocalTime
```
As this example shows, the goal is that runtime configuration files
are easily read and created by humans.  A common syntax makes
maintenance of many clients easier.

