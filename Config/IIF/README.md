# lbttcs Agent Runtime Configuration Files

**Original: 2014 July 1 [rwp/osu]**

**Updated: 2025 Sept 17 [rwp/osu]**

These files are used by the lbttcs agent and the IIF ICE utilities to
communicate with the LBT TCS, and list of parameters to retrieve
telescope and observatory information from the LBT Data Dictionary.

These need to be copied into the relevant `/home/lbt/Config/IIF/`
folder, and setup as either MODS1 or MODS2 as needed in the `lbttcs.ini`
runtime configuration file.

For a complete current list of public Data Dictionary entries
available, see https://github.com/LBTO/ in `tcs/iif/configuration/IIF/DDNames`

## Config directory

For a "flight" MODS data-taking system, the `lbttcs` and related
data-taking system agents will look for runtime configuration files in
the `/home/mods/Config/IIF` folder.  Each of the two configuration
file types has `_MODS1` and `_MODS2` variants.

To install the configuration files, copy the MODS1 or MODS2 variant as
needed into `lbttcs.ini` and `lbtIIF.client` (or make symbolic links).
Always avoid editing the files if possible.

### lbttcs Agent Runtime Configuration Files:

 * `lbttcs_MODS1.ini` - MODS1 installed at SX direct gregorian
 * `lbttcs_MODS2.ini` - MODS2 installed at DX direct gregorianf
  
### IIF client Configuration Files:

These are selected by setting the `PropFile` parameter (without .client) in the `lbttcs.ini` file
 * `lbtIIF_MODS1.client` - for MODS1 installed at SX direct gregorian
 * `lbtIIF_MODS2.client` - for MODS2 installed at DX direct gregorian
 * `tcsSim.client` - LBTO telescope simulator (obtained from LBTO), for example running at OSU

### azcam server FITS header tables

The `azcam-mods` server uses YAML files to tell it which data dictionary (DD) data are to be
retrieved and stored in the FITS headers for telescope and instrument configuration state.  These
are in 4 files:
 * `modsTCS_left.txt` - TCS state FITS headers for MODS1 at SX (left) direct gregorian
 * `modsTCS_right.txt` - TCS state FITS headers for MODS2 at DX (right) direct gregorian
 * `modsInst_left.txt` - Instrument state FITS data for MODS1 at SX (left) direct gregorian
 * `modsInst_right.txt` - Instrument state FITS data for MODS2 at DX (right) direct gregorian


### LBT Data Dictionary Tables:

These are selected by setting the `Side` parameter in the `lbttcs.ini` file to either "left" or "right"
 * `iifDD.left` - DD variables for Left (SX) Direct Gregorian focal station (MODS1)
 * `iifDD.right` - DD variables for Right (DX) Direct Gregorian focal station (MODS2)This is a collection of runtine configuration files for the `lbttcs` agent for
the MODS data-taking system
