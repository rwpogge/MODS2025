#
# transformInit - Initialize a MODS for executing an AGw/SFP
#                 transform measurement
#
# Configures a MODS for running the LBTO transform_collect IDL 
# procedure that gathers data for computing the SFP-to-AGw
# coordinate transformations.
#
# Puts the MODS instrument into red-only imaging mode with
# a 1Kx1K ROI, clears the exposure tables, gets it into
# observing mode (hatch open/calib retracted) and resets
# the AGw stage.
#
# R. Pogge, OSU Astronomy Dept.
# pogge@astronomy.ohio-state.edu
# 2014 May 2 [last modification]
#
###########################################################################

OBSMODE Imaging

Archive:
  PARTNER LBTO
  PROPID transform_collect
  PI_NAME None

Exec:
  print Configuring to run transform_collect.pro

  instconfig dual imaging
  agw reset
  slitmask out
  red object transform_collect
  red exptime 1
  red nimgs 1
  red roi 1Kx1K
  red ccdbin 1 1
  red filter r_sdss

  print Done - MODS ready to run transform_collect.pro

end
