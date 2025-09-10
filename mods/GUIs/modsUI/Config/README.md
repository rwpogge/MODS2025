# modsUI GUI Runtime Configuration Files

**Updated: 2025 Sept 10 [rwp/osu]**

These are files used by the modsUI GUIs for each of the MODS instances (MODS1 and MODS2)

## Config directory

For a "flight" MODS data-taking system, the `modsUI` GUI will look in `/home/dts/Confg/` for the approriate
configuration file.  There are 2 variants, one each for MODS1 and MODS2.

On the azcam server machine, make a symbolic link for the MODS instance:
```shell
cd /home/dts/Config
ln -s ./MODS1/modsUI_MODS1.ini modsUI.ini
```
That way when the modsUI instance is launched, it will always come up as the correct instance.

## Installation

Copy the contents of this `Config/` folder into a subfolder in the public `/home/dts/Config` directory:
```shell
  cd /home/dts/Config
  cp ~/MODS2025/mods/GUIs/modsUI/Config/modsUI_MODS1.ini MODS1/
  cp ~/MODS2025/mods/GUIs/modsUI/Config/modsUI_MODS2.ini MODS2/
```
Then make a link pointing to the correct file for the particular MODS instance:
```
  cd /home/dts/Config
  ln -s MODS1/modsUI_MODS1.ini modsUI.ini
```
for example, to setup that machine's `modsUI` GUI to talk to the MODS1 system.

## Configuration File Structure
A typical runtime config file has the following structure:
```
#
# MODS User Interface Runtime Configuration File for LBT
#
# R. Pogge, OSU Astronomy Dept.
# pogge.1@osu.edu
# 2025 Sept 10    
#
# MODS1 Version
#
# Modifications:
#   Archon CCD controller system updates [rwp/osu]
#
#---------------------------------------------------------------------------

Name MODS Control Panel

# ISIS Server and Client info

ISISID IS
ISISHOST 192.168.139.130 # mods1 server at LBT
ISISPort 6600
BasePort 20800

# Which functions do we enable?

use MODS1
#use MODS2
use Dispatcher   # Use the message dispatcher queue
cadence 500      # Dispatcher queue cadence is 500 msec
use ExpBell      # Ring the end-of-exposure bell
MaxLampTime 3600 # Maximum time calibration lamps may be lit in seconds
#use HeartBeat    # enable ISIS client heartbeat
#HeartBeat 120    # ISIS client heartbeat interval in seconds

# Which Consoles do we enable? 
#   Some are "dangerous" engineering/support functions we disable for
#   observing

# Use the TCS interface(s)?

use TCS          # telescope control system
TCSHost M1.TC    # TCS agent host ID

# Use a Housekeeping Manager?

use HKM             # housekeeping manager
HouseKeeping M1.HK  # housekeeping manager host ID
use EnvMonitor      # enable regular environmental status monitor queries
EnvMonitor 60       # enviromental status query interval in seconds

# Use a Utilities Manager?
# Disabled starting 2013B [rwp/osu]
#Use Utils
#Utility UM

# Use an ITerm Console?  

#use ITerm
#ITerm MC.IT

# Instrument Configuration

MODS1:
  Side     Left
  IEHost   M1.IE
  BlueHost M1.BC
  RedHost  M1.RC
  BDMHost  M1.DB
  RDMHost  M1.DR
  EnvHost  M1.ENV

MODS2:
  Side     Right
  IEHost   M2.IE
  BlueHost M2.BC
  RedHost  M2.RC
  BDMHost  M2.DB
  RDMHost  M2.DR
  EnvHost  M2.ENV
```
As this example shows, the goal is that runtime configuration files
are easily read and created by humans.  A common syntax makes
maintenance of many clients easier.

