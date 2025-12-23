#
# modsShutdown - Do a full instrument MODS shutdown
#
# Performs a full hardware shutdown of MODS, including parking
# all mechanisms at their home positions (as appropriate), retracting
# all masks, then powering down key subsystems.  This is used to
# power-down MODS in preparation for a loss of power at the telescope,
# or to prepare MODS for dismounting from the telescope.
#
# modsColdStart would be used to recover from a modsShutdown
#
# MODS must powered on at the telescope and the software services
# must be running (at minimum: isis, ie, and agw) for this script to work.
#
# Operations are identical for MODS1 and MODS2 and this script
# may be executed binocular or monocular as needed.
#
# See
#   modsWake to wake up a sleeping MODS
#   modsOpen to open up MODS for night-time observing
#   modsColdStart to do full cold instrument startup
#
# R. Pogge, OSU Astronomy Dept.
# pogge@astronomy.ohio-state.edu
# 2015 Nov 22 [review]
#
# 2025 Dec 23 - updated for the Archon controller system [rwp/osu]
#
###########################################################################

CALMODE

Exec:
   print ********************************
   print  MODS Instrument Full Shut-Down
   print ********************************
   print This could take a couple of minutes.
   print This is only done when preparing to dismount MODS from the
   print telescope or for a full emergency shutdown (e.g., expected
   print loss of power on the telescope proper) to safe the instrument.
   print It should not be used for putting MODS to sleep between runs.
   print Hit Enter to proceed or type Abort at the PAUSE if you do not 
   print wish to fully shutdown MODS...

   pause

   # Start: bring down the MODS subsystems one-by-one in order

   print Shutting down MODS...
   
   print Resetting the calibration and agw units...
   
   hatch reset
   calib reset
   agw reset
   lamp off

   print Powering down the IMCS IR laser...

   irlaser disable
   sleep 4
   irlaser off
   
   print Clearing and parking the slit mask system...

   slitmask reset

   print Parking the dichroic...
   
   dichroic blue

   print Parking blue channel mechanisms...
 
   bcolfoc reset
   bgrating 1
   bgrtilt1 hardreset
   bcamfoc reset
   bfilter 1

   print Parking red channel mechanisms...

   rcolfoc reset
   rgrating 1
   rgrtilt1 hardreset
   rcamfoc reset
   rfilter 1

   print Powering off the red and blue CCD systems

   util heb_b archon off
   util heb_r archon off
   util heb_b igpower off
   util heb_b igpower off
   util heb_b off
   util heb_r off

   print Powering off the AGw camera controllers...

   util agc off
   util wfs off

   print Powering off the lamp/laser box...

   util llb off

   print Powering off the red and blue IEBs...

   util ieb_b off
   util ieb_r off

   # All done

   print *** MODS Shutdown Completed ***

end
