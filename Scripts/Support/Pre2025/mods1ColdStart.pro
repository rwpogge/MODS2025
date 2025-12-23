#
# mods1ColdStart - Do a full MODS1 cold instrument startup
#
# Performs a full hardware reset on MODS1.  This has to be done
# after powering up the instrument, or to recover from unusually
# bad faults.
#
# modsColdStart would be used to recover from a modsShutdown
#
# MODS1 must first be physically powered on and the various
# interfaces started (isis, ie, and agw at a minimum)
#
# See
#   modsWake to wake up a sleeping MODS1
#   modsOpen to open up MODS1 for night-time observing
#   modsPark park MODS1 for a long period (between runs)
#   modsShutdown for a full instrument shutdown
#   modsWarmStart to do a warm instrument startup
#
# R. Pogge, OSU Astronomy Dept.
# pogge@astronomy.ohio-state.edu
# 2012 Nov 12 [review]
#
###########################################################################

SETUPMODE

Exec:
   print **********************************
   print MODS1 Instrument Full Cold Startup
   print **********************************
   print This could take many minutes.
   print Make sure MODS1 is powered up and all basic systems running.
   print or Abort at the PAUSE if you do not wish to proceed...

   pause

   # Go for it, bring up the systems one-by-one in order

   print Starting up MODS1...
   
   print Resetting Calibration and AGw units...
   
   hatch reset
   calib reset
   agw reset
   lamp off

   print Resetting the SlitMask system...

   slitmask reset
   slitmask
   dichroic reset
   dichroic dual

   print Resetting the Blue Channel...
 
   bcolfoc reset
   bgrating reset
   bcamfoc reset
   bfilter reset
   bfilter clear

   print Resetting the Red Channel...

   rcolfoc reset
   rgrating reset
   rcamfoc reset
   rfilter reset
   rfilter clear

   print Resetting primary grating tilts

   bgrtilt1 hardreset
   bgrtilt1 primary

   rgrtilt1 hardreset
   rgrtilt1 primary

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
   red roi 8Kx3K
   red ccdbin 1 
   blue object None
   blue exptime 1
   blue nimgs 1
   blue roi 8Kx3K
   blue ccdbin 1 

   # Powering on the AGw Cameras

   print Powering on the AGw Cameras

   util agc on
   util wfs on

   # final step, configure for dual imaging, our default
   # startup state.

   print Starting up the IMCS IR laser...

   irlaser on
   irlaser enable
   print Waiting 5 sec for laser power up...
   sleep 5
   irlaser power 1.0
   
   print Configuring the Instrument for dual imaging mode...

   instconfig dual imaging

end
