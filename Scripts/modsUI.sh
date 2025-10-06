#!/bin/csh -f
#
# modsUI - start/stop/check MODS user interface (modsUI)
#
# usage: modsUI {start|stop|status}
#
# Starts, stops, or shows the status of the MODS graphical user
# interface, modsUI, to ensure we don't run a duplicate version.
#
# For the MODS Archon CCD controller update, which required a
# major port and refactoring of the MODS software from 32-bit
# to 64-bit systems, we split off the modsUI app from the other
# MODS instrument servers, and launch it standalone.
#
# The modsUI executable is now in /home/dts/mods/bin, *outside*
# the default path, and in /home/dts/bin/ we put the modsUI.sh
# script.
#
# R. Pogge (pogge.1@osu.edu)
# OSU Astronomy Department
# 2025 Oct 5 - derived from old mods1/mods2 script code
#
#---------------------------------------------------------------------------

# MODS executable binary folder (outside user path on the MODS servers)

setenv binDir /home/dts/mods/bin

# Usage message

set usage = "\n   modsUI {start|stop|status}\n"

# Parse the command-line options

set numArgs = $#argv

if ($numArgs == 0 || $numArgs > 2) then
   printf "Usage: ${usage}\n"
   exit 0
endif

# Execute the request


switch ($1)
    # Start the named instrument services, either individually or in
    # functional groups as makes sense.  For some groups the order matters.
    case 'start':
	ps h -C modsUI >& /dev/null
        if ($status) then
           printf "The MODS control panel GUI is now started using 'modsUI start'\n"
	   exit 1
        else
	   set svcUser = `ps eo user h -C ${service}`
           printf "The MODS control panel GUI is already running (user $svcUser)\n"
        endif
	breaksw
	
   # Stop the named instrument services - could make this cleaner by testing
   # to see if running first, but for now this suffices.

    case 'stop':
	printf "Stopping the MODS control panel GUI (modsUI)...\n"
        killall modsUI
	breaksw

    case 'status':
	ps h -C modsUI >& /dev/null
        if ($status) then
	   printf "modsUI is not running"
        else
           set svcUser = `ps eo user h -C ${service}`
           printf "modsUS is running, user ${svcUser}"
        endif
	breaksw
	
   # Unknown option - squawk and exit

   default:
      printf "ERROR: unknown option '$1'\n"
      printf "Usage: ${usage}\n"
      exit 1
      breaksw

endsw
exit 0
