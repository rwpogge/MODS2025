# dataMan - MODS Data Manager

**Updated: 2026 Jan 14 [rwp/osu]**

See the [Release Notes](releases.md) for details.

## Overview

`dataMan` is python-based data management agent for the Archon MODS 
system that will post-process raw FITS images taken with MODS and transfer
post-processed images to the observatory `/newdata` disk where they
are exposed to astronomers for target acquisition and quick-look, and
staged for ingestion into the LBTO Data Archive.

The idea is to let the azcam server continue taking data moving post-processing "offline" from
the azcam server and the modsCCD interface client.  

`dataMan` replaces the functions performed by the `modslogger` script executed by the `caliban` data
transfer agents in the pre-2025 MODS data-taking system. Faster machines and greater python
FITS handling capability gives us many more options.

## Contents

### Core code

 * `dataMan.py` - simple UDP socket server and pre-processor. Designed to be run as a systemd service
 * `Config/dataMan_MODSn.ini` - yaml runtime configuration file templates for MODS1 and MODS2.
 * `systemd/` - folder with systemd service files

### Extras

 * `fixHead.py` - standalone program for doing the header preprocessing steps, but not data transfer
 * `archonFITSLab.ipynb` - Jupyter notebook for working out pre-processing functions

## Processing

We limit the processing to those items that require exposure to the
time-critical or evanescent state of the data-acquisition system.
Processes that can be done at relative leisure offline should be done
offline, as steps here ultimately delay arrival of the data where it
is needed, e.g., for target acquisition, so time is of the essence.
This limits us to

 * items we must fix before the data are presented to on-site observers
 * items we must fix before the data are presented to the data archive
 * items that can only be fixed using data available on the acquisition machine
     
One exception to time-critical updates is getting the observation date
and time info in the correct standard ISO8601 format required by FITS
and the LBT Archive. Absent this, the LBT Archive might not ingest the
file.
    
Once processing steps are completed, the last step is to copy the
processed FITS file to the observatory's central `/newdata/`
NFS-mounted repository folder where it may be accessed by observers
and SciOps personnel on site.  From newdata, the images are
transferred to the repository and queued for ingestion into the LBTO
Data Archive.  This latter places a requirement on fixing any FITS
header issues that might inhibit ingestion into the data archive.

### Processing Steps

#### Fix DETSEC/CCDSEC keywords

Fix the pixel coordinate ranges in the `DETSEC` and `CCDSEC` keywords
for each quadrant and the `LVTn` coordinate transform coefficients are
incorrect.  If you read the raw MODS FITS image using ds9 and open as
"Mosaic IRAF", the quadrants will not be stitched together correctly
on the display (huge gaps) and the cursor will read the wrong
coordinates back.  This fixes the headers in post-processing.
Eventually we should dig deep into the `azcam` code and fix it there,
but not now.

#### Fix Date/Time keywords

Converts the `azcam` style `DATE-OBS` keyword in to LBT Archive
compliant ISO8601 format (`azcam` is old school).

Also computes MJD and heliocentric and barycentric JD and add it to
the headers as `HJD-OBS` and `BJD-OBS` at start of exposure useful for
MODS observations requiring precision timing.

#### Fix Archon CCD and controller temperatures

Reads the CONPARS extension (5) and extracts the CCD, Base, and Archon
backplane temperatures at start of exposure and puts these into
primary HDU.  This corrects wrong `CCDTEMP` and `BASETEMP` that comes
from `azcam`, and sets by instrument-specific wiring (MODS1 different
than MODS2).

#### Fix Miscellanous FITS header keywords

Function to take care of the numerous keywords that need different fixes
to satisfy LBT data archive and data-reduction pipelines that expect them
Most are legacy keywords from the old MODS system.  Some involve converting
native Archon formats into legacy MODS controller format for backwards
compatibility.

#### Create a preview bias-subtracted, merged image

Extracts each raw quadrant images from FITS extension 1..4, computes
the median overscan bias, subtracts it from the quadrant data pixels,
then trims off the overscan columns.  If quadrants 2 and 4 it flips
the pixels along rows because of the different readout direction, then
merges all 4 quadrants into a single image and returns it as a numpy
array (32-bit float) plus lists of the median bias levels and their
standard deviations.
    
The resulting image is appended to the MEF File as the MERGED extension.
This can be used for target acquisition and examined for quick-look
2d or 1d data examination.
    
