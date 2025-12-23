#
# mods1Wake - Wake up a sleeping MODS1
#
# Wakes up MODS1 after a mods1Sleep.  This setups up a few
# select systems to ready the instrument for observing.
#
# Takes the following actions:
#   Powers on the IMCS laser and enables the beam.
#   Initializes the CCD controller state
#   Resets exposure paramters
#
# See
#   mods1Sleep to put MODS1 to sleep at the end of a night
#   mods1Open to open up MODS1 for night-time observing
#   mods1Park park MODS1 for a long period (between runs)
#   mods1ColdStart to do full cold instrument startup
#   mods1WarmStart to do a warm instrument startup
#   mods1Shutdown for a full instrument shutdown
#
# R. Pogge, OSU Astronomy Dept.
# pogge@astronomy.ohio-state.edu
# 2015 Nov 12 [review]
#
###########################################################################

SETUPMODE

Exec:
  print Waking up MODS1...

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

  # Clear and reset the MODS1 control panel

  print Resetting the exposure controls

  obsinit

  print Done - MODS1 is awake.

end
