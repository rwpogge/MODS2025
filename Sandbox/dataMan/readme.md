# dataMan - MODS Data Manager

**Updated: 2025 Nov 6 [rwp/osu]**

Development sandbox for a python-based data manager for the Archon MODS 
system that will post-process raw images taken with MODS.

Post-processing steps include
 * fixing fits header issues
 * logging the images
 * pushing the raw images to the data repository
 * converting raw MEF to standard FITS?
 * inserting telemetry metadata (guider, wfs, weather?) as FITS bintable extensions
 * other?

The idea is to let the azcam server continue taking data moving post-processing "offline" from
the azcam server and the modsCCD interface client.  

This is to replace functions performed by the `modslogger` script executed by the `caliban` data
transfer agents in the pre-2025 MODS data-taking system. Faster machines and greater python
FITS handling capability gives us many more options.

## contents

 * `dataMan_vX.py` - development version of `dataMan`.
 * `dataMan.ini` - yaml runtime configuration file template.
 * `dataMan_server.py` - simple UDP socket server demo, run as `python dataMan_server.py`
 * `clientDemo.py` - simple dm client - run in ipython shell, and use `sendToDM()` to send messages. `quit` will shutdown the server demo

## Notes

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


