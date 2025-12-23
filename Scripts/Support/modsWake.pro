#
# modsWake - Wake up a sleeping MODS
#
# Wakes up MODS after a modsSleep.  This setups up a few
# select systems to ready the instrument for observing.
#
# Operations are identical for MODS1 and MODS2, so may be
# executed binocular or monocular as needed.
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
# 2015 Nov 12 [review]
#
# 2025 Dec 23 - updated for the Archon controller system [rwp/osu]
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
  irlaser power 1.5
  irlaser

  print Initializing the science CCD cameras

  red igpower on
  blue igpower on

  # Archon controllers should be on, but power on the CCD

  # red ccdpower on
  # blue ccdpower on
  
  # Clear and reset the MODS control panel

  print Resetting the exposure settings on the MODS GUI

  obsinit

  print *** Done - MODS is awake ***

end
