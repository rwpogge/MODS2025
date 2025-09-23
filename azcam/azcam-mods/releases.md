# azcam-mods Release Notes

**Current Version: 1.1.0**

**Last Update: 2025 Sept 23 [rwp/osu]**

### Version 1.1.0 - 2025 Sep 23

LBT TCS Build 2025A incorporates new data dictionary keywords for MODS instrument configuration 
info. This is how we are going to get instrument information into FITS headers. 
 * Added `instrument_mods.py`, an azcam `Instrument()` class instance to create the instrument FITS header
block.
 * In `telescope_lbto.py renamed `LBTTCS()` to `LBTTelescope()` to make the relation to the azcam `Telescope()` base class explicit. Included minor tweaks to `telescope_lbt.py` to fix a few typos in error message.
 * Modified `server.py` to use this new `Instrument()` class instead of the default azcam instance, and to incorporate change in the telescope interface.

Moved release notes from README.md into this file to match the convention for the other parts of the MODS code repository.


### Version 1.0.7 - 2025 Aug 7

Changes in code based on results of work with the flight MODS systems at LBTO during Archon/HEB 
installation and testing in the MODS1 and MODS2 instruments.  Biggest changes are the assignment
if the in-dewar temperature sensors (CCD and mount base) used by the Archon temperature controller
(older assignments were incorrect). Changes also made to the .ncf and .acf files for temperature
control as they had indicated the incorrect sensor type (they are RTD100 - 100-ohm Pt RTDs
not DT-670 diodes which were common in ARC/Leach controller dewars).  Also created `MODS_Actual.ncf`
with these changes, and turning continuous readout OFF initially for mountain testing.  We will
derive the baseline `MODS_Actual.acf` configuration file from this once it is finalized.

### Verison 1.0.5 - 2025 Aug 3 

Final lab checkout complete, ready to deploy on the mountain computers
alongside physical installation of the MODS Archon controllers in
MODS1 and MODS2

### Version 1.0.3 - 2025 Aug 1

Extensive testing with the live Archon system in the LBTO Tucson lab
with the MODS test dewar (3x8K CCD).  Advanced a working version ready
for initial deployment at LBTO with the MODS instruments to prepare
for re-commissioning in September.  Verified to work with the new
`modsCCD` IC emulation agent that will be the primary interface
between the MODS azcam server and the existing instrument control and
data acquisition system.

Remaining issues:
 * No STOP command equivalent in the azcam server, need to research this
 * No instrument status integration yet (no live system to help develop it)
 * If there is a problem on write and the expose1() thread hangs, as yet no obvious way to recover the data from the Archon buffer, but still early days after identifying the issue today.

### Version 1.0.1 - 2025 July 26

Server tested with the `modsCCD` azcam client that will take the place
of the DOS "IC" basic program functions.  Version ready for live
testing in the Tucson lab before Archon installation in MODS at LBTO
first week of August.

### Version 0.8.0 - 2025 July 23

Multiple updates while developing the C++ client app that will sit between MODS and the azcam server

### Version 0.5.0 - 2025 July 18

Got telescope_mods.py working with the LBT python-iif code to pull TCS info into azcam

### Version 0.1.0 - 2025 July 8

Starting work for the MODS Archon detector upgrade at LBTO in Summer 2025 [rwp/osu]

