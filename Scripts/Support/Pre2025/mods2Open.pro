#
# mods2Open - Open up MODS2 for night-time observing
#
# Opens up MODS2 and makes it ready to go on-sky.  mods2Open
# must be sent before the first pointing of the night.
#
# Takes the following actions:
#   Turns off any calibration lamps
#   Stows the calibration tower
#   Puts the guide probe at the field center
#   Opens the hatch
#
# See
#   mods2Sleep to put MODS2 to sleep at the end of a night
#   mods2Wake to wake up MODS2 after a mods2Sleep
#   mods2Park park MODS2 for a long period (between runs)
#   mods2ColdStart to do full cold instrument startup
#   mods2WarmStart to do a warm instrument startup
#   mods2Shutdown for a full instrument shutdown
#
# R. Pogge, OSU Astronomy Dept.
# pogge@astronomy.ohio-state.edu
# 2015 Nov 22 [MODS1+2]
#
###########################################################################

SETUPMODE

Exec:
  print Opening up MODS2 to the sky...
  print Retracting the calibration tower and turning off lamps...
  lamp off
  calib out
  print Moving the AGw guide probe on-axis
  gprobe 0 0
  print Opening the dark hatch
  hatch open
  print Done - MODS2 is open to the sky.

end
