#
# Snap images of the Sieve Mask in the blue and red channels
#
# R. Pogge, OSU Astronomy Dept
# pogge@astronomy.ohio-state.edu
# 2011 Sept 18 [last updated]
#
# 2025 Dec 23 - cleared for use with the Archon system [rwp/osu]
#

CALMODE

Archive:
  PARTNER CALIBRATION
  PROPID CALIBRATION
  
Instrument:
  instconfig dual imaging
  slitmask SieveMask

Exec:
  lamp vflat 4.0 on
  object Sieve Mask
  red filter r_sdss
  red exptime 1
  blue filter g_sdss
  blue exptime 10
  go
  lamp off

end
