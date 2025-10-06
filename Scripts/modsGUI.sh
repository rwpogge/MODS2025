#!/bin/csh -f
#
# modsGUI - start/stop/check MODS graphical user interface (modsUI)
#
# usage: modsGUI {start|stop|status}
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

set usage = "\n   modsGUI {start|stop|status}\n"

# Parse the command-line options

set numArgs = $#argv

if ($numArgs == 0 || $numArgs > 2) then
   printf "\nUsage: ${usage}\n"
   exit 0
endif

# Execute the request

switch ($1)
    case 'start':
	ps h -C modsUI >& /dev/null
        if ($status) then
           printf "\nStarting the MODS GUI...\n"
	   ${binDir}/modsUI &
        else
	   set svcUser = `ps eo user h -C ${service}`
           printf "\nThe MODS GUI is already running, user: ${svcUser}\n\n"
        endif
	breaksw
	
    case 'stop':
	ps h -C modsUI >& /dev/null
        if ($status) then
	   printf "\nThe MODS GUI is not running, nothing to stop.\n\n"
	else
	   printf "\nStopping the MODS GUI...\n"
           killall modsUI
	endif
	breaksw

    case 'status':
	ps h -C modsUI >& /dev/null
        if ($status) then
	   printf "\nThe MODS GUI is not running\n\n"
        else
           set svcUser = `ps eo user h -C ${service}`
           printf "\nThe MODS GUI is running, user ${svcUser}\n\n"
        endif
	breaksw
	
   default:
      printf "ERROR: unknown option '$1'\n"
      printf "\nUsage: ${usage}\n"
      exit 1
      breaksw

endsw
exit 0
