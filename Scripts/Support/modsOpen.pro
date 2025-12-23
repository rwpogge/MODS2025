#
# modsOpen - Open up MODS for night-time observing
#
# Opens up MODS and makes it ready to go on-sky.  modsOpen
# must be sent before the first pointing of the night.
#
# Operations are identical for both MODS, may be run
# binocular or monocular as needed.
#
# Takes the following actions:
#   Turns off any calibration lamps
#   Stows the calibration tower
#   Puts the guide probe at the field center
#   Opens the hatch
#
# See
#   modsSleep to put MODS to sleep at the end of a night
#   modsWake to wake up MODS after a modsSleep
#   modsPark park MODS for a long period (between runs)
#   modsColdStart to do full cold instrument startup
#   modsWarmStart to do a warm instrument startup
#   modsShutdown for a full instrument shutdown
#
# R. Pogge, OSU Astronomy Dept.
# pogge@astronomy.ohio-state.edu
# 2015 Nov 22 [MODS+2]
#
# 2025 Dec 23 - updated for the Archon CCD system [rwp/osu]
#
###########################################################################

SETUPMODE

Exec:
  print Opening up MODS to the sky...
  print Retracting the calibration tower and turning off lamps...
  lamp off
  calib out
  print Moving the AGw guide probe on-axis
  gprobe 0 0
  print Opening the dark hatch
  hatch open
  print Done - MODS is open to the sky.

end
