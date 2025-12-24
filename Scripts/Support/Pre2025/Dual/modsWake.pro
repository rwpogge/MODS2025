#
# modsWake - Wake up a sleeping MODS
#
# Wakes up MODS after a modsSleep.  This setups up a few
# select systems to ready the instrument for observing.
#
# Takes the following actions:
#   Powers on the IMCS laser and enables the beam.
#   Initializes the CCD controller state
#   Resets exposure paramters
#
# See
#   modsSleep to put MODS to sleep at the end of a night
#   modsOpen to open up MODS for night-time observing
#   modsPark park MODS for a long period (between runs)
#   modsColdStart to do full cold instrument startup
#   modsWarmStart to do a warm instrument startup
#   modsShutdown for a full instrument shutdown
#
# R. Pogge, OSU Astronomy Dept.
# pogge@astronomy.ohio-state.edu
# 2012 Nov 12 [review]
#
###########################################################################

SETUPMODE

Exec:
  print Waking up MODS...

  print Turning on the IMCS IR laser
  irlaser on
  irlaser enable
  print Waiting 5s for the IR laser to power up...
  sleep 5
  irlaser power 1.0
  irlaser

  # Makes sure that the HEBs have their TECs running

  print Initializing the CCDs

  red tedpower on
  red ledpower off
  red igpower on
  blue tedpower on
  blue ledpower off
  blue igpower on

  # Clear and reset the MODS control panel

  print Resetting the exposure controls

  obsinit

  print Done - MODS is awake.

end
