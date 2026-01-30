#
# modsColdStart - Do a MODS cold instrument startup
#
# Performs a full hardware reset on MODS.  This has to be done
# after powering up the instrument, or to recover from unusually
# bad faults.
#
# Operations are identical for MODS1 and MODS2 so may be
# executed binocular or monocular as needed.
#
# modsColdStart would be used to recover from a modsShutdown
#
# MODS must first be physically powered on and the various
# software services started.
#
# See
#   modsWake to wake up a sleeping MODS
#   modsOpen to open up MODS for night-time observing
#   modsPark park MODS for a long period (between runs)
#   modsShutdown for a full instrument shutdown
#   modsWarmStart to do a warm instrument startup
#
# R. Pogge, OSU Astronomy Dept.
# pogge@astronomy.ohio-state.edu
# 2012 Nov 12 [review]
#
# 2025 Dec 23 - updated for the Archon controller system [rwp/osu]
#
###########################################################################

SETUPMODE

Exec:
   print ***********************************
   print  MODS Instrument Full Cold Startup
   print ***********************************
   print This will take a few minutes.
   print Make sure MODS is powered up and all basic systems running.
   print or Abort at the PAUSE if you do not wish to proceed...

   pause

   # Begin: bring up the systems one-by-one in order

   print Starting up MODS...
   
   print Resetting calibration and AGw units...
   
   hatch reset
   calib reset
   agw reset
   lamp off

   print Resetting the slitmask system...

   slitmask reset
   slitmask

   print Resetting the dichroic...
   
   dichroic reset
   dichroic dual

   print Resetting blue channel mechanisms...
 
   bcolfoc reset
   bgrating reset
   bcamfoc reset
   bfilter reset
   bfilter clear

   print Resetting red channel mechanisms...

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

   print Initializing the science CCD systems

   red igpower on
   blue igpower on

   # The Archon controllers require 30 seconds to reboot
   
   red archon on
   blue archon on
   print Waiting 30 sec sec for Archon controllers to start
   sleep 30
   
   # Reset exposure and detector readout states

   red object None
   red exptime 1
   red nimgs 1
   red roi off
   red ccdbin 1 

   blue object None
   blue exptime 1
   blue nimgs 1
   blue roi off
   blue ccdbin 1 

   # power on the AGw cameras

   print Powering on the AGw camera controllers

   util agc on
   sleep 5
   util wfs on

   # IR laser

   print Starting up the IMCS IR laser...

   irlaser on
   irlaser enable
   print Waiting 5 sec for laser power up...
   sleep 5
   irlaser power 2.0

   # configure for dual imaging, w/o imcs lock
   
   instconfig dual imaging
   
   # all done

   print Done - MODS cold start complete.
   
end
