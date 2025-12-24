#
# ccdInit - Initialize the MODS CCD controllers
#
# Initializes the MODS CCD controllers after a controller
# restart independent of a general instrument restart.  It makes
# sure that certain initial states are recovered.
#
# Takes the following actions for each of the Red and Blue CCDs: 
#   Turns on the HEB TED and Ionization Gauge
#   Turns off the HEB box LED indicators
#   Sets CCD binning to 1x1
#   Sets CCD readout region of interest to full frame
#   Resets the control panel exposure counter and exposure time to 1s
#
# R. Pogge, OSU Astronomy Dept.
# pogge@astronomy.ohio-state.edu
# 2011 Feb 17 [last mod]
#
###########################################################################

SETUPMODE

Exec:
  print Initializing the MODS CCD controllers

  # Makes sure that the HEBs have their TECs and Ion Gauges running but
  # the lab LED indicator lamps OFF (don't want LEDs on inside MODS)

  red tedpower on
  red ledpower off
  red igpower on
  blue tedpower on
  blue ledpower off
  blue igpower on

  # Reset the readout state

  object None
  exptime 1
  nimgs 1
  red roi 8Kx3K
  blue roi 8Kx3K
  ccdbin 1

  print Done - MODS CCD controller state initialized

end
