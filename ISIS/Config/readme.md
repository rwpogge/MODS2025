# ISIS server Runtime Configuration Files

**Updated: 2025 July 27 [rwp/osu]**

These are files used by the ISIS message-passing servers for each of the binocular MODS instruments.

## Config directory

For a "flight" MODS data-taking system, the `isis` server will look in `/home/dts/Confg/` for the appropriate
configuration file.  There are 2 variants, each named for each MODS instrument (MODS1 and MODS2).

Only one ISIS server is needed for each MODS, so a total of 2 servers for a binocular system.

ISIS runs on the instrument server machines (hosts `mods1` and `mods2`).

This is the same as the way ISIS was run on the pre-Archon MODS data-taking system.

### ISIS server Runtime Configuration Files:

 * `isis_MODS1.ini` - MODS1
 * `isisd_MODS1.ini` - MODS1 "daemon" mode
 * `isis_MODS2.ini` - MODS2
 * `isisd_MODS2.ini` - MODS2 "daemon" mode

## Installation

Copy the contents of this `Config/` folder into a subfolder in the public `/home/dts/Config` directory
name `ISIS`:
```shell
  cd /home/dts/Config
  mkdir ISIS  (if it does not already exist)
  cd ISIS
  cp MODS2025/mods/Agents/ISIS/Config/isis_MODS*.ini .
  cd ..
```
Then make a link pointing to the correct file for the particular MODS channel:
```
  cd /home/dts/Config
  ln -s ISIS/isis_MODS2.ini isis.ini
```
for example, to setup the `mods2` instrument server machine's `ISIS` server for MODS2.
  
## Configuration File Structure
A typical ISIS runtime config file has the following structure:
```
#
# ISIS Server Runtime Initialization File
#
# MODS1 ISIS at LBTO
#
# R. Pogge, OSU Astronomy Dept.
# pogge.1@osu.edu
# 2025 July 26
#
################################################################

# ISIS Server Info

ServerID   IS
ServerPort 6600
ServerLog  /home/dts/Logs/ISIS/isis
LogDate    UTC

# List all serial ports we might use here, max 8
# Comment out any unused ports to avoid startup errors

# No serial ports on the new hardware

#TTYPort /dev/ttyD0
#TTYPort /dev/ttyS0

# Define any preset UDP ports we wish to ping on startup or restart, max 12

# MODS1 mountain: 192.168.139.1xx
#  M1.IE  mmcServer .130 port 10700
#  M1.TC  lbttcs agent: .130 port 10801
#  M1.ENV lbtEnv agent: .130 port 10901
#  M1.RC  modsCCD agent: .131 port 10402
#  M1.BC  modsCCD agent: .132 port 10401
#  M1.RHE modsHEB agent: .131 port 10502
#  M1.BHE modsHEB agent: .132 port 10501
#  modsUI GUI .130, ports 20800 and 20801 (dashboard)

UDPPort 192.168.139.130 10700
UDPPort 192.168.139.130 10801
UDPPort 192.168.139.130 10901
UDPPort 192.168.139.131 10402
UDPPort 192.168.139.132 10401
UDPPort 192.168.139.131 10502
UDPPort 192.168.139.132 10501
UDPPort 192.168.139.130 20801

# Runtime Flags

#Verbose

# Instrument ID

Instrument MODS1
```
