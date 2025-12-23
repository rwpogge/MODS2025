#
# modsWarmStart - do a MODS warm instrument (re)start.
#
# Performs a warm startup of MODS, done with an already powered up
# an initialized instrument that has been in use for a while with
# all mechanisms in known positions.
#
# Operations are identical for MODS1 and MODS2, so may be executed
# binocular or monocular as needed.
#
# NOTE: If MODS has been off AC power for a long time, you need to
#       run modsColdStart.pro
#
# R. Pogge, OSU Astronomy Dept.
# pogge@astronomy.ohio-state.edu
# 2015 Nov 22
#
# 2025 Dec 23 - updated for the Archon controller system [rwp/osu]
#
###########################################################################

SETUPMODE

Exec:
   print **********************************
   print    MODS Instrument Warm Startup
   print **********************************
   print This will do a warm start of an already running MODS.
   print It will take about 1 minute to finish.
   print Abort at the PAUSE if you do not wish to proceed...

   pause

   # Go for it, bring up the systems one-by-one in order

   print MODS Warm Start...
   
   print Closing Hatch
   
   hatch reset
   lamp off
   calmode

   print Resetting the slit mask system...

   slitmask reset
   slitmask

   print Querying Mechanisms

   dichroic
   bcolfoc
   bgrating reset
   bcamfoc
   bfilter
   rcolfoc
   rgrating reset
   rcamfoc
   rfilter

   print Soft reset of the grating tilts

   bgrtilt1 softreset
   rgrtilt1 softreset

   print Initializing the science CCDs

   red igpower on
   blue igpower on

   # red ccdpower on  # not yet implemented
   # blue ccdpower on
   
   red object None
   red exptime 1
   red nimgs 1
   red ccdbin 1 
   red roi off

   blue object None
   blue exptime 1
   blue nimgs 1
   blue ccdbin 1 
   blue roi off
   
   print Powering on the AGw cameras

   util agc on
   sleep 5
   util wfs on

   # Power up the IMCS IR laser

   print Starting up the IMCS IR laser...

   irlaser on
   irlaser enable
   print Waiting 5 sec for laser power up...
   sleep 5
   irlaser power 1.5

   # configure for dual imaging w/o IMCS lock

   print Configuring for dual imaging...
   
   instconfig dual imaging

   # all done

   print Done - MODS warm start complete.
   
end
