# lbtTCS - LBT Telescope Control System interface agent

Version: v3.0.1-bino - **Replacement will be v4.0.0 under Agents**

Author: R. Pogge, OSU Astronomy (pogge.1@osu.edu)

Last Revision: 2015 Aug 11

Last Build: 2015 Dec 31 [LBT flight]

## Overview

This is the MODS interface agent between the MODS instrument control system and the LBT Telescope Control System (TCS).
v3.x is the binocular LBT interface built starting wtih LBT IIF TCS build 2015B on 2015 Aug 11.  There has been very
little relevant change since that build, so this has been very stable.

The LBT TCS interface uses Zero-C ICE to communicate with the TCS. Our interface on the MODS side is purely as a command/query
agent: we send requests for information or commands for action and wait for response.  The TCS cannot send commands or 
asynchronous communications to us.

The lbttcs has two modes:
 * connect to the ICE/IIF interface and talk to a live IIF
 * a simulation mode that creates plausible 
looking FITS header info for replies to a lab data-acquisition system like the IC.

Communication with the MODS instrument control system is via UDP sockets to an isis server using the ICIMACSv2.0 protocl
