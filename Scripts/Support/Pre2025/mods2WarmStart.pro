#
# mods2WarmStart - Do a warm instrument startup
#
# Performs a warm startup of MODS2, done with an already powered up
# an initialized instrument that has been in use for a while with
# all mechanisms in known positions.
#
# NOTE: If MODS2 has been off AC power for a long time, you need to
#       run mods2ColdStart.pro
#
# R. Pogge, OSU Astronomy Dept.
# pogge@astronomy.ohio-state.edu
# 2015 Nov 22
#
###########################################################################

SETUPMODE

Exec:
   print ***********************************
   print    MODS2 Instrument Warm Startup
   print ***********************************
   print This will do a warm start of an already running MODS2.
   print It will take about 1 minute to finish.
   print Abort at the PAUSE if you do not wish to proceed...

   pause

   # Go for it, bring up the systems one-by-one in order

   print MODS2 Warm Start...
   
   print Closing Hatch
   
   hatch reset
   lamp off
   calmode

   print Resetting the SlitMask system...

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

#   bgrtilt1 softreset  # we don't do this for MODS2 yet
#   rgrtilt1 softreset

   print Initializing the CCD controllers

   # Makes sure that the HEBs have their TECs running

   red tedpower on
   red ledpower off
   red igpower on
   blue tedpower on
   blue ledpower off
   blue igpower on

   # Reset the readout state

   red object None
   red exptime 1
   red nimgs 1
   red ccdbin 1 

   # Powering on the AGw Cameras

   print Powering on the AGw Cameras

   util agc on
   sleep 5
   util wfs on

   # Power up the IMCS IR laser

   print Starting up the IMCS IR laser...

   irlaser on
   irlaser enable
   print Waiting 5 sec for laser power up...
   sleep 5
   irlaser power 1.0

   # With spare QC board on red, lower threshold from 0.3 to 0.1 [rwp]
   # Increase threshold to 0.3 to exceed bias by 2-3x on advice of rwp [opk]
  
   rimcs qcmin 0.3

   # Configure for dual imaging mode
   
   print Configuring the Instrument for dual imaging mode...

   instconfig dual imaging
   
   # lock IMCS

   imcslock

end
