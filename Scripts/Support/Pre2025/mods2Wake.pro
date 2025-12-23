#
# mods2Wake - Wake up a sleeping MODS2
#
# Wakes up MODS2 after a mods2Sleep.  This setups up a few
# select systems to ready the instrument for observing.
#
# Takes the following actions:
#   Powers on the IMCS laser and enables the beam.
#   Initializes the CCD controller state
#   Resets exposure paramters
#
# See
#   mods2Sleep to put MODS2 to sleep at the end of a night
#   mods2Open to open up MODS2 for night-time observing
#   mods2Park park MODS2 for a long period (between runs)
#   mods2ColdStart to do full cold instrument startup
#   mods2WarmStart to do a warm instrument startup
#   mods2Shutdown for a full instrument shutdown
#
# R. Pogge, OSU Astronomy Dept.
# pogge@astronomy.ohio-state.edu
# 2015 Nov 12 [review]
# 2015 Nov 22 - setting irlaser power to 1.5mW to try to address
#               red IMCS failure to lock [rwp/osu]
# 2017 Dec 28 - setting irlaser power back to 1.0mW [opk/lbto]
#
###########################################################################

SETUPMODE

Exec:
  print Waking up MODS2...

  print Turning on the IMCS IR laser
  irlaser on
  irlaser enable
  print Waiting 5s for the IR laser to power up...
  sleep 5
  irlaser power 1.0
  irlaser

  # 2018 Jan - with spare QCboard on red, lower threshold from 0.3 to 0.1 [rwp]
  # 2024 Sep - qcmin increased to 0.3, 2-3x bias, on advice of rwp to resolve IT8564 [opk]

  rimcs qcmin 0.3

  # Makes sure that the HEBs have their TECs running

  print Initializing the CCDs

  red tedpower on
  red ledpower off
  red igpower on
  blue tedpower on
  blue ledpower off
  blue igpower on

  # Clear and reset the MODS2 control panel

  print Resetting the exposure controls

  obsinit

  print Done - MODS2 is awake.

end
