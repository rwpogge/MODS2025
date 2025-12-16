# azcam-mods Release Notes

**Current Version: 1.1.6**

**Last Update: 2025 Dec 10 [rwp/osu]**

### Version 1.1.6 - 2025 Dec 10
 * Set `exposure.shutter_delay = 500` msec on testing with 1Kx1K images acquired with MODS2R camera and the pinhole sieve mask, 1s exposure. Stop seeing streaks at 300ms.  The old controller used 1s, which was arbitrary.

### Version 1.1.5 - 2025 Nov 21
 * Added `exposure.shutter_delay = 250` to add a 250-msec shutter delay before starting readout.  This is needed because of the slower MODS shutter.  This is an initial value that will be optimized later to be a best for all 4 cameras.  Change was introduced by Mike Lesser during MODS detector testing.

### Verison 1.1.4 - 2025 Oct 22/23
 * Fixed bug in `mods.py` function `obsDate()` because the old code assumed local computers had clocks set to local time but LBTO standard is to set clocks on mountain machines to UTC.  Now explicitly code local time (zone US/Arizona), requiring importing
`pytz`.

### Version 1.1.3 - 2025 Oct 21
Numerous bug fixes, additions to logging, etc., driven primarily by verifying interaction with the LBT telescope control system (`pyIIF` code).

### Version 1.1.2 - 2025 Oct 15

Multiple patches during rapid fire testing with live systems.  All kinds of fun on first contact with what goes into header and finding ways to get into a tight spin. 


### Version 1.1.1 - 2025 Oct 13
 * Changed `heaterData()` method in `mods.py` to be `archonStatus()` to make it more general. For now this adds the backplane temperature and the CCD power state code (0..5).

### Version 1.1.1 - 2025 Oct 2
 * Fixed bugs in `instrument_mods.py` during live testing
 * Changes to `mods.py` to change how we read status info (using `get_status()` method and picking data out of the returned dict).
 * Added `heaterData()` method to the `MODS` class to extract heater info (temps, output V, and PID parameters)
 * Updated ACF files with heater control parameters suggested by Greg Bredthauer, prep for cooling down science dewars

### Version 1.1.0 - 2025 Sep 23

LBT TCS Build 2025A incorporates new data dictionary keywords for MODS instrument configuration 
info. This is how we are going to get instrument information into FITS headers. 
 * Added `instrument_mods.py`, an azcam `Instrument()` class instance to create the instrument FITS header
block.
 * In `telescope_lbto.py renamed `LBTTCS()` to `LBTTelescope()` to make its heritage from the azcam `Telescope()` base class explicit. Also made minor tweaks to `telescope_lbt.py` to fix a few typos in error message.
 * Updated `server.py` to use this new `Instrument()` class instead of the default azcam instance, and to implement changes in the LBTTelescope() class.
 * Moved release notes from README.md into this file to match the convention for the other parts of the MODS code repository.
 * Created `instHdr_MODS#<chan>.txt` yaml-format header config files for the new instrument header bits above.  These are based on the similar yaml files for telescope headers. These are in the respective `azcam/system/MODS#<chan>/templates/` folders.

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

