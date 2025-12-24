# Engineering Scripts

Warning: Some of these are likely to be replaced by more efficient codes
that run on the azcam servers for the Archon controllers, or with 
azcam tools designed to do this independently of the instrument data-taking
system.

## CCD Tests

Measure photon transfer curves to estimate gain and readout noise per
quadrant, characterize dark counts, and estimate execution overheads.

The photon transfer curves methods are likely to be run using the 
`azcam-console` suite of test programs that includes automated analysis 
and report generation.

Stay tuned...

### Scripts

 * `ccdTime.pro` - estimate execution times for different modes.
 * `darks.cal` - dark calibration (increasing time series of binned darks)
 * `photTran_b.cal` - acquire blue-channel CCD photon transfer curve data
 * `photTran_r.cal` - acquire red-channel CCD photon transfer curve data

## Dice 5 Test

Execute a "five dice" offset pattern with 1- and 10-arcsec throws to help
evaluate the RADEC offset precision.

A five dice pattern is
<pre>
     2   1
       0
     3   4
</pre>
in the telescope RADEC coordinates.  The offset pattern is
<pre>
  0 - origin
  1 - +throw,+throw
  2 - -throw,+throw
  3 - -throw,-throw
  4 - +throw,-throw
</pre>
in absolute coordinates relative to the origin.

### Scripts

 * `dice1.img` - 5-dice pattern with a 1-arcsecond throw
 * `dice10.img` - 5-dice pattern with a 10-arcsecond throw
 * `runDice5` - csh script to execute these each 5 times


