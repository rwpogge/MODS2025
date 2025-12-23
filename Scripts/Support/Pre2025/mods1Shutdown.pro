#
# mods1ShutDown - Do a full instrument shutdown
#
# Performs a full hardware shutdown of MODS1, including parking
# all mechanisms at their home positions (as appropriate), retracting
# all masks, then powering down key subsystems.  This is used to
# power-down MODS1 in preparation for a loss of power at the telescope,
# or to prepare MODS1 for dismounting from the telescope.
#
# modsColdStart would be used to recover from a modsShutDown
#
# MODS1 must powered on at the telescope and the software services
# must be running (at minimum: isis, ie, and agw) for this script to work.
#
# See
#   modsWake to wake up a sleeping MODS1
#   modsOpen to open up MODS1 for night-time observing
#   modsColdStart to do full cold instrument startup
#
# R. Pogge, OSU Astronomy Dept.
# pogge@astronomy.ohio-state.edu
# 2015 Nov 22 [review]
#
###########################################################################

CALMODE

Exec:
   print *******************************
   print MODS1 Instrument Full Shut-Down
   print *******************************
   print This could take a couple of minutes.
   print This is only done when preparing to dismount MODS1 from the
   print telescope or for a full emergency shutdown (e.g., expected
   print loss of power on the telescope proper) to safe the instrument.
   print It should not be used for putting MODS1 to sleep between runs.
   print Hit Enter to proceed or type Abort at the PAUSE if you do not 
   print wish to fully shutdown MODS1...

   pause

   # Go for it, bring up the systems one-by-one in order

   print Shutting Down MODS1...
   
   print Resetting Calibration and AGw units...
   
   hatch reset
   calib reset
   agw reset
   lamp off

   print Powering down the IMCS IR laser...

   irlaser disable
   sleep 4
   irlaser off
   
   print Clearing and parking the Slit Mask system...

   slitmask reset
   dichroic reset
   dichroic dual

   print Homing Blue Channel mechanisms...
 
   bcolfoc reset
   bgrating 1
   bgrtilt1 hardreset
   bcamfoc reset
   bfilter 1

   print Homing Red Channel mechanisms...

   rcolfoc reset
   rgrating 1
   rgrtilt1 hardreset
   rcamfoc reset
   rfilter 1

   # Reset the detector controllers to sensible defaults

   print Clearing CCD parameters...

   red object None
   red exptime 1
   red nimgs 1
   red roi 8Kx3K
   red ccdbin 1 
   red saveconfig
   blue object None
   blue exptime 1
   blue nimgs 1
   blue roi 8Kx3K
   blue ccdbin 1 
   blue saveconfig

   # Powering off the AGw Camera Controllers

   print Powering OFF the AGw Camera Controllers...

   util agc off
   util wfs off

   # Powering off the lamp/laser controller

   print Powering OFF the Lamp/Laser Box...

   util llb off

   # Powering off the IEBs

   print Powering OFF the Red and Blue IEBs...

   util ieb_b off
   util ieb_r off

   # Powering off the HEBs

   print Powering OFF the Red and Blue CCD Head Electronics Boxes...

   util heb_b off
   util heb_r off

   # All done

   print *** MODS1 Powered OFF ***

end
