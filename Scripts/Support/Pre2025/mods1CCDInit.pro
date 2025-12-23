#
# mods2CCDInit - Initialize the MODS1 CCD controllers
#
# Initializes the MODS1 CCD controllers after a controller
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
# pogge.1@osu.edu
# 2018 Apr 26 [MODS1+2]
#
###########################################################################

SETUPMODE

Exec:
  print Initializing the MODS1 CCD controllers

  # Makes sure that the HEBs have their TECs and Ion Gauges running but
  # the lab LED indicator lamps OFF (do not want LEDs on inside MODS1)

  red tedpower on
  red ledpower off
  red igpower on
  blue tedpower on
  blue ledpower off
  blue igpower on

  # Reset the readout configuration

  red object None
  red exptime 1
  red nimgs 1
  red roi off
  red ccdbin 1
  red overx 0
  red overy 0
  red saveconfig

  blue object None
  blue exptime 1
  blue nimgs 1
  blue roi off
  blue ccdbin 1
  blue overx 0
  blue overy 0
  blue saveconfig

  print Done - MODS1 CCD controller state initialized

end
