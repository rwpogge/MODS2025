# MODS azcam system files

**Updated**: 2025 July 31

We restructured the configuration for azcam for MODS to segregate system configuration files and logs from raw imaging data.  This is to help
streamline our configuration management (4 Archon controllers across 2 instruments in the same rack), and to make getting at raw science image
transparent without exposing critical system configuration files.

We use `system/` as the root folder for all system-specific files
 * `parameters/` for azcam server parameter files
 * `templates/` for FITS header templates and IIF data dictionary files
 * `archon/` for the Archon controller files (.acf and .ncf)
 * `logs/` for azcam server logs

Each MODS instance has its own directory:
 * `MODS1B` - MODS1 Blue Channel
 * `MODS1R` - MODS1 Red Channel
 * `MODS2B` - MODS2 Blue Channel
 * `MODS2R` - MODS2 Red Channel

In addition, there is a separate entry for the MODS lab test dewar
 * `test` - MODS lab test dewar

## Parameters

Parameter initialization files for the azcam server and console apps. Each
MODS instrument has its own set of parameters.  `<modsID>` = (MODS1B/R,MODS2B/R):
 * `server_<modsID>.ini` - azcam server parameters for `<modsID>`
 * `console_<modsID>.ini` - azcam console parameters for `<modsID>`

## Templates

Header and other templates used by the azcam server.  The FITS header template is the
static header structure for each `<modsID>` system.  The azcam server will add various
other parameters as it will, and populate the headers with dates, times, and specific
data like actual exposure time, total dark time, etc.
 * `header_<modsID>.txt` - static exposure info FITS header template for `<modsID>` channel.
 * `instHdr_<modsID>.txt` - LBT IIF data dictionary to Instrument FITS header keyword mapping used by `instrument_mods.py` for `<modsID>` channel.
 * `modsTCS_<side>.txt` - LBT IIF data dictionary to TCS FITS header keyword mapping used by `telescope_lbt.py`. <side> is left (SX) or right (DX), same for both channels in a single MODS

## Archon Control Files

The archon controller use `.acf` or `.ncf` files for the detailed detector readout
configuration. There may be many test or alternative files in this folder, but
the `<modsID>.ncf` file will be considered the "flight" configuration.  Use
symbolic links or judicious copying (careful!) to enable an alternative or test
configuration

Once on the ground in Tucson with the live system we identified the correct .ncf files
and was able to convert it to an ACF format using the STA `archongui`.  With help
from Greg Bredthauer, we learned we have to edit out the `[SYSTEM]` block to upload
from a remote host, but need that section for local download into `archongui`.

The reference file is `MODS_Example_2.ncf`, which we converted into `mods_1.acf` fore
testing.  Copies of each are in the individual system `MODS#c/archon` folders named
for the system channel in case we need to individually modify it.  So far this is
just multiple redundant copies.


