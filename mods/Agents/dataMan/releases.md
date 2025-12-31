# dataMan Release Notes

**Latest Version: v1.0.0, 2025 Dec 23**

## Released Versions (v1.0 and later)

### 2025 Dec 13 - v1.0.0

First beta release for integration with the MODS data-taking system at LBTO.

## Development Notes (v0.x)

### 2025 Dec 31
Lots of debugging since Dec 26.  Live test Dec 30 uncovered the usual bugs, some rework required. LBT had to solve write permissions
issues
 * dataMan.py ready to transition from Sandbox to released beta code at v1.0
 * testing running `dataMan` as a systemd service, verified function
 * added `dataMan` to the `modsStatus.pl` display, verified function
 * first live transfers of data from the Archon servers to LBTO /newdata staging drive, including update of "lastdata" files.
 * cleaned up logging issues
 * minor tweaks to the `modsCCD` agent to make sure prototype dataMan hooks are disabled, but not yet removed.

All on mods1blue and mods1 for now, will distribute and install across systems on migration from the Sandbox

**This is the last Sandbox version**


### 2025 Dec 26
Big architecture change: found a good way to make one `dataMan` server per MODS, which solves problems of raw and processed data
aggregation and configuration management.  Will now run `dataMan` as a systemd service on mods1 and mods2, and use the NFS-mounted
data disk so we protect raw data better.
 * live testing of code with ipython shell to verify and test operation timing with real data
 * reworked `dataMan_vX.py` and `dataman.ini` for the changes
 * corresponding changes in `azcam-mods`, see release notes (server.py and instrument_mods.py)

### 2025 Dec 25
Significant work on `dataMan_vX.py`, with changes to `fixHead.py` and the `archonFITSLab.ipynb` Jupyter notebook.  
 * Broke out preprocessing steps into separate functions.  Lets us toggle on/off with the runtime config file
 * Incorporated overscan/trim/merge (otm) processing from `archonFITSLab` which will append a fast debiased, trimmed and merged image into the FITS file as extension 6.
 * Added preprocessing directives to `dataMan.ini` for the new functions
 * Restructured `dataMan_vX.py` to incorporate the above changes.
 * Performed at test of triggering the dataMan client message to server from inside `azcam-mods` which was successful. Will trigger dataMan processing from the `MODSInstrument()` class `exposure_finish()` method, using native `azcam` core functions instead of grafting into `modsCCD`.  Tested and works like a champ.  Greatly simplifies the image acquisition and post-processing workflow.

   
### 2025 Nov 6
FITS header fix code in `dataMan_vX.py`:
 * Fix `DATE-OBS` to comply with LBT Archive and NOST FITS standards for ISO8601 date+time data
 * Added code to compute MJD, HJD, and BJD for headers
 * Added code to generate a `UNIQNAME` keyword based on date/time
 * Took notes on `modsCCD` additions to implement the client side, see that repository for info

### 2025 Oct 31
Working with data headers from live MODS.  Found a few issues that need fixing:
 * DETSEC and CCDSEC headers in extension Q1..4 are incorrect for ROI subframe reads, found a fix
 * CCDTEMP and BASETEMP reported are -999.9 (no read) but Archon status info in HDU5 are good, so can fix with that snapshot
 * Need code to fix blank and NOCOMM header info, but so far haven't seen any, next step is to simulate

### 2025 Oct 16
First cut before we start getting CCD data with live headers from the MODS.

### 2025 Aug 29
Back at it after the Archon deploy. Added `threading` to the server for a `proc <filename>` command. Set to 10s to process a "FITS" image.
Ran with the client in an `ipython` shell.  Seems OK, need to start thinking of how to test with live data next.

### 2025 July 20
First cut, very simple client/server pair.  Only command is "quit"


