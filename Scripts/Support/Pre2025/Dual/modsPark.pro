#
# modsPark - Park MODS for a long down period (e.g., between runs)
#
# Puts MODS into a low-power state for long-term inactivity, as
# between observing runs.  Turns off all but essential systems
# for monitoring instrument health.  We use CALMODE for this 
# procedure instead of the generic SETUPMODE
#
# Takes the following actions:
#   Closes the hatch
#   Powers off the IMCS laser
#   Turns off all calibration lamps
#   Homes the AGw stage
#   Inserts the Calibration Tower
#   Inserts a Dark Mask to protect the field lens
#
# See
#   modsSleep to put MODS to sleep at the end of a night
#   modsWake to wake up a sleeping MODS
#   modsOpen to open up MODS for night-time observing
#   modsColdStart to do full cold instrument startup
#   modsWarmStart to do a warm instrument startup
#   modsShutdown for a full instrument shutdown
#
# R. Pogge, OSU Astronomy Dept.
# pogge@astronomy.ohio-state.edu
# 2012 Nov 12 [review]
#
###########################################################################

CALMODE

print Parking MODS for a long sleep...

Exec:
  irlaser off
  lamp off
  agwfilt clear
#  slitmask DarkMask

  # Generic CCD channel config

  bias Bias Frame
  ccdbin 1
  nimgs 1

  # Blue Channel base configuration

  blue fitsflush
  blue filter clear
  blue roi 8Kx3K

  # Red Channel base configuration

  red fitsflush
  red filter clear
  red roi 8Kx3K

  print Done - MODS has been parked for a long sleep.

end
