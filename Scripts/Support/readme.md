# MODS Support Scripts

**Updated: 2026 Jul 15 [rwp/osu]**

## Overview

These are the general support scripts for the MODS spectrographs.
This set has been modified for the Archon controller system
implemented in late 2025. MODS1 and MODS2 scripts have mostly been
merged since operations are identical for both instruments.

A few older scripts have been retired, but all of the pre-2025 scripts
are in the `attic/` folder for reference.

The `Eng` folder contains engineering scripts. See the separate
readme in that folder for information.

## Descriptions

### `ccdInit.pro` - Initialize CCD state

Initialize the Archon controller state through the MODS GUI.  Resets
readout paramters like binning, ROI, exposure number and time,
clears the object name.  It also makes sure the vacuum ionization
gauges are powered on.

### `imcsTTFs.pro` - IMCS TTF zero-point measurement

With the telescope at El=60-deg and PA=0-deg (rotAngle=207), run
through each of the six MODS modes and measure the collimator mirror
tip/tilt/focus (TTF) actuator zero points.  We use redundant imcslock
commands to ensure refined values.  Takes a snapshot in a 3Kx3K ROI
for confirmation in each mode.

Collimator TTF values are extracted from the FITS headers of the
snapshots.

### `sieveSnap.pro` - sieve mask snapshot

Snap images of the Sieve Mask in the blue and red channels

### `imSnap.pro` - imaging mask snapshot

Snap images of the imaging mask in the red and blue channels

### `modsColdStart.pro` - Do a MODS cold instrument startup

Performs a full hardware reset on MODS.  This has to be done after
powering up the instrument, or to recover from unusually bad faults.

Operations are identical for MODS1 and MODS2 so may be executed
binocular or monocular as needed.

`modsColdStart` would be used to recover from a `modsShutdown`

MODS must first be physically powered on and the various software
services started.

### `modsOpen.pro` - Open MODS for observing

Opens up MODS and makes it ready to go on-sky. `modsOpen` must be sent
before the first pointing of the night.

Operations are identical for both MODS, may be run binocular or
monocular as needed.

Takes the following actions:
 * Turns off any calibration lamps
 * Stows the calibration tower
 * Puts the guide probe at the field center
 * Opens the hatch

### `modsShutdown.pro` - Full MODS instrument shutdown

Performs a full hardware shutdown of MODS, including parking all
mechanisms at their home positions (as appropriate), retracting all
masks, then powering down key subsystems.  This is used to power-down
MODS in preparation for a loss of power at the telescope, or to
prepare MODS for dismounting from the telescope.

`modsColdStart` would be used to recover from a `modsShutdown`

MODS must powered on at the telescope and the software services must
be running (at minimum: isis, ie, and agw) for this script to work.

Operations are identical for MODS1 and MODS2 and this script may be
executed binocular or monocular as needed.

### `modsSleep.pro` - Put MODS to sleep

Puts MODS into a safe sleep mode at the end of a night.  Puts select
systems into a low-power standby mode, ready to be awakened with
`modsWake`.

Takes the following actions:
 * Closes the hatch
 * Disables the IMCS laser but keeps the heater/interface running
 * Homes the AGw stage
 * Inserts the Calibration Tower
 * Turns off any calibration lamps
 * Put in the dark mask
 * Remove any "gotchas" from the spectrograph channel setup

A `modsSleep` would be executed after a series of end-of-night
calibrations are completed.

The operation is identical for MODS1 and MODS2, so may be executed
binocular or monocular as needed

### `modsWake.pro` - Wake up a sleeping MODS

Wakes up MODS after a `modsSleep`.  This setups up a few select systems
to ready the instrument for observing.

Operations are identical for MODS1 and MODS2, so may be executed
binocular or monocular as needed.

Takes the following actions:
 * Powers on the IMCS laser and enables the beam.
 * Initializes the CCD controller state
 * Resets exposure paramters


### `modsWarmStart.pro` - MODS warm instrument (re)start

Performs a warm startup of MODS, done with an already powered up an
initialized instrument that has been in use for a while with all
mechanisms in known positions.

Operations are identical for MODS1 and MODS2, so may be executed
binocular or monocular as needed.

**NOTE: If MODS has been off AC power for a long time, you need to run `modsColdStart.pro`**


### `transformInit.pro` - Setup MODS for AGw/SFP coordinate transform measurement

Configures a MODS for running the LBTO transform_collect IDL procedure
that gathers data for computing the SFP-to-AGw coordinate
transformations.

Puts the MODS instrument into dual imaging mode with the 3Kx3K ROI,
clears the exposure tables, gets it into observing mode (hatch
open/calib retracted) and resets the AGw stage.
