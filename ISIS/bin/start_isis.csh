#!/bin/csh -f
#
# start_isis - wrapper used to invoke the ISIS server
#              in a safe and painless fashion with log rotation. 
#
# usage: start_isis [-finifile]
#
# where: -finifile = use inifile as the runtime config file instead of
#                    the default (used only for engineering)
#
# R. Pogge, OSU Astronomy Dept.
# pogge@astronomy.mps.ohio-state.edu
# 2003 January 30 
#
############################################################################

if ($#argv > 0) then
   setenv ISargs $1
else
   setenv ISargs " "
endif

setenv ISBin /home/dts/ISIS/bin
setenv ISLogs /data/Logs/ISIS

#
# rotate the logs.  We retain them 9-deep (crudely implemented)
#

\mv $ISLogs/isis.log.8 $ISLogs/isis.log.9 >& /dev/null
\mv $ISLogs/isis.log.7 $ISLogs/isis.log.8 >& /dev/null
\mv $ISLogs/isis.log.6 $ISLogs/isis.log.7 >& /dev/null
\mv $ISLogs/isis.log.5 $ISLogs/isis.log.6 >& /dev/null
\mv $ISLogs/isis.log.4 $ISLogs/isis.log.5 >& /dev/null
\mv $ISLogs/isis.log.3 $ISLogs/isis.log.4 >& /dev/null
\mv $ISLogs/isis.log.2 $ISLogs/isis.log.3 >& /dev/null
\mv $ISLogs/isis.log.1 $ISLogs/isis.log.2 >& /dev/null
\mv $ISLogs/isis.log   $ISLogs/isis.log.1 >& /dev/null

# launch isis *in this window*

echo "]2;ISIS Command Window"

${ISBin}/isis ${ISargs} 

