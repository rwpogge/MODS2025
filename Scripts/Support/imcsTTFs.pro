#
# imcsTTFs - measure IMCS TTF zero points for all modes
#
# With the telescope at El=60-deg and PA=0-deg (rotAngle=207)
# for MODS1, run through each of the 6 MODS mods and measure
# the TTF zero points.  We use redundant imcslock commands
# to ensure refined values.  Takes a snapshot in a 1Kx1K ROI
# for confirmation in each mode.
#
# Extract the TTF zero points from the raw FITS image primary
# header data unit the WCSTools gethead program as follows:
#
#  gethead dichname gratname colttfa colttfb colttfc *.fits
#
# for the files.
#
# While running this script it is recommended that you be
# running the imcsTools to monitor IMCS convergence.
#
# Typical execution time is under 15 minutes.
#
# R. Pogge, OSU Astronomy Dept.
# pogge@astronomy.ohio-state.edu
# 2011 Nov 01 [last mod]
#
# Modification History
#   2025 Dec 23 - updated for the Archon controller system [rwp/osu]
#
###########################################################################

CALMODE

Archive:
  PARTNER CALIBRATION
  PROPID CALIBRATION
  PI_NAME Support

Exec:
  print Set the LBT elevation to 60-degrees and PA=0 (rotAngle=207)
  print Hit Enter when ready
  pause

  object IMCS TTF Calibration El=60 rotAngle=0
  lamp vflat 4.0 on
  irlaser on
  irlaser enable
  print Waiting 5s for the IR laser to power up...
  sleep 5
  irlaser power 1.5
  red exptime 1.0
  blue exptime 10.0
  slitmask sievemask

Imaging:
  nimgs 1
  instconfig dual imaging
  red roi 1Kx1K
  blue roi 1Kx1K
  red filter r_sdss
  blue filter g_sdss
  imcslock
  go

  instconfig red imaging
  red roi 1Kx1K
  imcslock
  red go

  instconfig blue imaging
  blue roi 1Kx1K
  imcslock
  blue go

Grating:
  nimgs 1
  instconfig dual grating
  red roi 1Kx1K
  blue roi 1Kx1K
  imcslock
  go

  instconfig red grating
  red roi 1Kx1K
  imcslock
  red go

  instconfig blue grating
  blue roi 1Kx1K
  imcslock
  blue go

Prism:
  nimgs 1
  instconfig dual prism
  red roi 1Kx1K
  blue roi 1Kx1K
  imcslock
  go

  instconfig red prism
  red roi 1Kx1K
  imcslock
  red go

  instconfig blue prism
  blue roi 1Kx1K
  imcslock
  blue go

  lamp off
  print Done - Get TTF info from the image FITS headers

end
