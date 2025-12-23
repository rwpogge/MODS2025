#
# transformInit - Initialize a MODS for executing an AGw/SFP
#                 transform measurement
#
# Configures a MODS for running the LBTO transform_collect IDL 
# procedure that gathers data for computing the SFP-to-AGw
# coordinate transformations.
#
# Puts the MODS instrument into dual imaging mode with
# a 1Kx1K ROI, clears the exposure tables, gets it into
# observing mode (hatch open/calib retracted) and resets
# the AGw stage.
#
# R. Pogge, OSU Astronomy Dept.
# pogge@astronomy.ohio-state.edu
# 2014 May 2 [rwp]
#
# Edited to use red channel on both MODS1 and MODS2 and 
# to insert the PointingMask
#
# Edited to configure MODS1 Red and MODS2 Blue
# 2018 Nov 01 [opk/lbto]

# Edited back to using red channel on both MODS1 & MODS2
# though the script will configures both channels.
# The transform_collect IDL procedure (or procedures/functions called by it) 
# will control the exposure time and channel used,
# i.e. will send the <red,blue> exptime and <red,blue> go 
# commands, overwriting whatever is set for these here.
# We configure both blue & red channels to reasonable settings just in case.
#
# 2023 Oct 16 [opk, last modification]
###########################################################################

OBSMODE Imaging

Archive:
  PARTNER COMMISSIONING
  PROPID transform_collect
  PI_NAME COMMISSIONING

Exec:
  print Configuring to run transform_collect.pro

  instconfig dual imaging
  agw reset
  slitmask PointingMask
  object transform_collect
  blue exptime 10
  red exptime 5
  nimgs 1
  ccdbin 1 1
  blue filter g_sdss
  blue roi 1Kx1K
  #
  red filter r_sdss
  red roi 1Kx1K

  print Done - MODS ready to run transform_collect.pro

end
