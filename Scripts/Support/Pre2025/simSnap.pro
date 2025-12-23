#
# Snap images of the Sieve Mask in the blue and red channels
#
# R. Pogge, OSU Astronomy Dept
# pogge@astronomy.ohio-state.edu
# 2011 Sept 18 [last updated]
#
# Added an imaging snap, to allow us to check the optics for dust
# 2019 Nov 05 [opk/lbto]
#
# Modified this to minimize mask exchanges. 
# put the slitmask back into the focal plane  
# out of an abundance of caution, as normally 
# there is a mask in the focal plane
# (opk/lbto 9-Mar-22)

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

ImSnap:
  slitmask out
  object Imaging Snap
  go 

  slitmask in

  lamp off

end
