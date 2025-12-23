#
# mods1Open - Open up MODS1 for night-time observing
#
# Opens up MODS1 and makes it ready to go on-sky.  mods1Open
# must be sent before the first pointing of the night.
#
# Takes the following actions:
#   Turns off any calibration lamps
#   Stows the calibration tower
#   Puts the guide probe at the field center
#   Opens the hatch
#
# See
#   mods1Sleep to put MODS1 to sleep at the end of a night
#   mods1Wake to wake up MODS1 after a mods1Sleep
#   mods1Park park MODS1 for a long period (between runs)
#   mods1ColdStart to do full cold instrument startup
#   mods1WarmStart to do a warm instrument startup
#   mods1Shutdown for a full instrument shutdown
#
# R. Pogge, OSU Astronomy Dept.
# pogge@astronomy.ohio-state.edu
# 2015 Nov 22 [MODS1+2]
#
###########################################################################

SETUPMODE

Exec:
  print Opening up MODS1 to the sky...
  print Retracting the calibration tower and turning off lamps...
  lamp off
  calib out
  print Moving the AGw guide probe on-axis
  gprobe 0 0
  print Opening the dark hatch
  hatch open
  print Done - MODS1 is open to the sky.

end
