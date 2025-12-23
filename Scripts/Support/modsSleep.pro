#
# modsSleep - Put MODS to sleep at the end of the night
#
# Puts MODS into a safe sleep mode at the end of a night.  Puts
# select systems into a low-power standby mode, ready to
# be awakened with modsWake.
#
# Takes the following actions:
#   Closes the hatch
#   Disables the IMCS laser but keeps the heater/interface running
#   Homes the AGw stage
#   Inserts the Calibration Tower
#   Turns off any calibration lamps
#   Put in the Dark Mask
#   Remove any "gotchas" from the spectrograph channel setup
#
# A modsSleep would be executed after a series of end-of-night
# calibrations are completed.
#
# The operation is identical for MODS1 and MODS2, so may be
# executed binocular or monocular as needed
#
# See
#   modsWake to wake up a sleeping MODS
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
# 2025 Dec 23 - updated for the Archon CCD system [rwp/osu]
#
###########################################################################

SETUPMODE

Exec:
  print Putting MODS to sleep for the night...
  hatch close
  irlaser disable
  calmode
  lamp off
  red nimgs 1
  blue nimgs 1

  # Instruct the Archons to power off the CCD

  # red ccdpower off
  # blue ccdpower off

  print Done - MODS has been put to sleep.

end
