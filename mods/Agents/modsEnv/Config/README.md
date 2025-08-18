# modsEnv Agent Runtime Configuration Files

**Updated: 2025 Aug 18 [rwp/osu]**

These are files used by modsEnv agents for each of the binocular MODS instruments.

## Config directory

For a "flight" MODS data-taking system, the `modsEnv` agent will look in `/home/dts/Confg/` for the approriate
configuration file.  There are 2 variants, each named for each MODS instrument (MODS1 and MODS2).

Only one modsEnv agent is needed for each MODS, so a total of 2 agents for a binocular system.

modsEnv runs on the instrument server machines (hosts `mods1` and `mods2`).

This is the same as the way modsEnv was run on the pre-Archon MODS data-taking system.

### modsEnv Agent Runtime Configuration Files:

 * `modsenv_MODS1.ini` - MODS1
 * `modsenv_MODS2.ini` - MODS2

## Installation

Copy the contents of this `Config/` folder into a subfolder in the public `/home/dts/Config` directory
name `modsEnv`:
```shell
  cd /home/dts/Config
  mkdir modsEnv  (if it does not already exist)
  cd modsEnv
  cp MODS2025/mods/Agents/modsEnv/Config/modsenv_MODS*.ini .
  cd ..
```
Then make a link pointing to the correct file for the particular MODS channel:
```
  cd /home/dts/Config
  ln -s modsEnv/modsenv_MODS2.ini modsenv.ini
```
for example, to setup the `mods2` instrument server machine's `modsEnv` agent for MODS2.
  
## Configuration File Structure
A typical modsEnv runtime config file has the following structure:
```
#
# modsenv agent runtime configuration file
#
# Version for MODS1 at LBTO
#
# R. Pogge, OSU Astronomy Dept.
# pogge.1@osu.edu
# 2025 Aug 18
#
################################################################

# modsenv's ISIS client info (Host=localhost is implicit)

ID   M1.ENV
Port 10901

# Application Mode: either STANDALONE or ISISclient

#Mode Standalone
Mode ISISclient

# ISIS Server Info - only releveant if Mode=ISISclient

ISISID   IS
ISISHost 192.168.139.130
ISISPort 6600

# Environmental Monitor Parameters

InstID MODS1
UseTTY T
Cadence 60
DataLog /home/Logs/Env
HdfLog  /home/Logs/Env

# WAGO IP addresses for MODS1

IUB     192.168.139.122
IEB_R   192.168.139.100
IEB_B   192.168.139.101
LLB     192.168.139.120
HEB_R   192.168.139.141
HEB_B   192.168.139.142

# Ionization Gauge Comtrol port info

RedIG_Addr 192.168.139.103
RedIG_Port 8018
RedIG_Chan 5

BlueIG_Addr 192.168.139.113
BlueIG_Port 8018
BlueIG_Chan 5

# Runtime flags 
#NoLog
#Debug
UseHdf5
Verbose
```
