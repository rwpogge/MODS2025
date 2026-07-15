#
# imcsTTFs - measure IMCS TTF zero points for all modes
#
# With the telescope at El=60-deg and PA=0-deg (rotAngle=207), run
# through each of the six MODS modes and measure the collimator mirror
# tip/tilt/focus (TTF) actuator zero points.  We use redundant imcslock
# commands to ensure refined values.  Takes a snapshot in a 3Kx3K ROI
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

  object IMCS TTF Calibration
  lamp vflat 4.0 on
  irlaser on
  irlaser enable
  print Waiting 5s for the IR laser to power up...
  sleep 5
  irlaser power 2.0
  red exptime 1.0
  blue exptime 10.0
  slitmask sievemask

Imaging:
  nimgs 1
  instconfig dual imaging
  red roi 3Kx3K
  blue roi 3Kx3K
  red filter r_sdss
  blue filter g_sdss
  imcslock
  sleep 3
  go

  instconfig red imaging
  red roi 3Kx3K
  imcslock
  sleep 3
  red go

  instconfig blue imaging
  blue roi 3Kx3K
  imcslock
  sleep 3
  blue go

Grating:
  nimgs 1
  instconfig dual grating
  red roi 3Kx3K
  blue roi 3Kx3K
  imcslock
  sleep 3
  go

  instconfig red grating
  red roi 3Kx3K
  imcslock
  sleep 3
  red go

  instconfig blue grating
  blue roi 3Kx3K
  imcslock
  sleep 3
  blue go

Prism:
  nimgs 1
  instconfig dual prism
  red roi 3Kx3K
  blue roi 3Kx3K
  imcslock
  sleep 3
  go

  instconfig red prism
  red roi 3Kx3K
  imcslock
  sleep 3
  red go

  instconfig blue prism
  blue roi 3Kx3K
  imcslock
  sleep 3
  blue go

  lamp off
  print Done - Get TTF info from the image FITS headers

end
