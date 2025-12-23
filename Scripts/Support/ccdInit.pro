#
# ccdInit - Initialize the MODS Archon controllers
#
# Initializes the MODS Archon CCD controllers after a controller
# restart independent of a general instrument restart.  It makes
# sure that certain initial states are recovered.
#
# Because this operation is identical to MODS1 and MODS2, this
# script may be run binocular or monocular as needed.
#
# Takes the following actions for each of the Red and Blue CCDs: 
#   Turns on the HEB Ionization Gauge
#   Sets CCD binning to 1x1
#   Sets CCD readout region of interest to full frame unbinned
#   Reset the exposure time to 1s
#   Reset the number of exposures to 1
#   Reset the object name to None
#   Other steps?
#
# R. Pogge, OSU Astronomy Dept.
# pogge.1@osu.edu
# 2018 Apr 26 [MODS1+2]
#
# Modification History:
#  2025 Dec 23 - updated for the Archon controller system [rwp/osu]
#
###########################################################################

SETUPMODE

Exec:
  print Initializing the Archon CCD controllers

  # Make sure the HEB ionization gauges are powered on

  red igpower on
  blue igpower on

  # Reset the readout configuration

  red object None
  red exptime 1
  red nimgs 1
  red roi off
  red ccdbin 1

  blue object None
  blue exptime 1
  blue nimgs 1
  blue roi off
  blue ccdbin 1

  print Done - Archon CCD controller state initialized

end
