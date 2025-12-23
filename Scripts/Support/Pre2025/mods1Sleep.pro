#
# mods1Sleep - Put MODS1 to sleep at the end of the night
#
# Puts MODS1 into a safe sleep mode at the end of a night.  Puts
# select systems into a low-power standby mode, ready to
# be awakened with mods1Wake.
#
# Takes the following actions:
#   Closes the hatch
#   Disables the IMCS laser but keeps the heater/interface running
#   Homes the AGw stage
#   Inserts the Calibration Tower
#   Turns off any calibration lamps
#   Put in the Dark Mask
#   Sends a "fitsflush" command to make sure all pending FITS
#      file writes are completed.
#   Remove any "gotchas" from the spectrograph channel setup
#
# A mods1Sleep would be executed after a series of end-of-night
# calibrations are completed.
#
# See
#   mods1Wake to wake up a sleeping MODS1
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
  print Putting MODS1 to sleep for the night...
  hatch close
  irlaser disable
  calmode
  lamp off
#  slitmask DarkMask
  red nimgs 1
  blue nimgs 1

  # Flush any pending images in the transfer queues

  blue fitsflush
  red fitsflush

  print Done - MODS1 has been put to bed.

end
