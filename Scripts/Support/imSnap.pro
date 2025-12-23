#
# Snap images of the Sieve Mask in the blue and red channels
#
# R. Pogge, OSU Astronomy Dept
# pogge@astronomy.ohio-state.edu
# 2011 Sept 18 [last updated]
#
# 2019 Nov 05 - imaging flat snap to check for field lens dust [opk/lbto]
# 2025 Dec 23 - updated for the Archon controller system [rwp/osu]
#

CALMODE

Archive:
  PARTNER CALIBRATION
  PROPID CALIBRATION
  
Instrument:
  instconfig dual imaging
  slitmask Imaging

Exec:
  lamp vflat 4.0 on
  blue filter g_sdss
  blue exptime 10
  red filter r_sdss
  red exptime 1
  object Imaging Mask Snap
  go 
  lamp off

end
