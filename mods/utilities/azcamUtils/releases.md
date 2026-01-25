# azcamUtils Library Releases

## Version 2 - For python azcam server

### Version 2.0.4 - 2026 Jan 25
 * `azcamutils.c` - added new function `void replaceEq()` to replace = sign in outgoing azcam command strings.  Used in `iosubs.c` function `azcamCmd()` to strip an remaining "=" characters before it is sent to the azcam server.  This is the terminal defence against this particular "feature" of the azcam server's remote interface
 * `iosubs.c` - added `replaceEq()` to `azcamCmd()` as above.
 * Live tested with mod1b and released


### Version 2.0.3 - 2026 Jan 24
 * segmentation faults on string argument reads.  
 * added `getObsDate()` function to image.c to allow hosts to ask azcam what the obsDate is, part of making filename more automated to eliminate human errors in setting the CCYYMMD date tag correctly (UTC time at LBT, for example). Queries the `azcam-mods` method `obsDate()` in `mods.py`


### Version 2.0.2 - 2025 Oct 15
 * Removed ArchonTemp - very strange interaction caused segfaults


### Verison 2.0.1 - 2025 Aug 3
 * Updates after live testing with the Archon servers at LBT
 * Changes to azcamutils.c, iosubs.c to fix issues with bad return from `azcamCmd()`
 * Fixed random problems with quoted info going to the azcam server (much pickier than expected)
 * cleaned up stray status message typos


### Version 2.0.0 - 2025 July 23
Python azcam server version start
 * adaptation of the 2005 Y4KCam AzCamUtils code for Archon and the python azcam server system, last version was 1.6.0
 * thorough overhaul needed as a lot has changed in 20 years...

## Version 1 - LabView azcam server

Retired, was developed in 2025 for the Y4KCam camera (4Kx4K STA-0500 CCD in an IR Labs dewar run using an ARC Gen-3 controller)
