#
# Measure setup and readout time overheads
#
# Does this 8Kx3K, 4Kx3K, 3Kx3K, and 1Kx1K ROI, binned 1x1 and 2x2
# 5 frames each to get reliable statistics (glitch avoidance)
#
# Approximate execution time: 30 minutes
#
# R. Pogge, OSU Astronomy Dept
# pogge@astronomy.ohio-state.edu
# 2012 Dec 7 [last updated]
#

SETUPMODE

Archive:
  PARTNER CALIBRATION
  PROPID CALIBRATION
  PI_NAME Pogge

Exec:
  nimgs 5

  bias 8Kx3K 1x1
  red roi 8Kx3K
  blue roi 8Kx3K
  ccdbin 1
  dgo

  bias 8Kx3K 2x2
  ccdbin 2
  dgo

  bias 4Kx3K 1x1
  red roi 4Kx3K
  blue roi 4Kx3K
  ccdbin 1
  dgo

  bias 4Kx3K 2x2
  ccdbin 2
  dgo

  bias 3Kx3K 1x1
  red roi 3Kx3K
  blue roi 3Kx3K
  ccdbin 1
  dgo

  bias 3Kx3K 2x2
  ccdbin 2
  dgo

  bias 1Kx1K 1x1
  red roi 1Kx1K
  blue roi 1Kx1K
  ccdbin 1
  dgo

  bias 1Kx1K 2x2
  ccdbin 2
  dgo

  nimgs 1
  ccdbin 1

end
