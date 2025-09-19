# modsDD Agent Runtime Configuration Files

**Updated: 2025 Sept 19 [rwp/osu]**

These files are used by the modsDD agent and the IIF ICE utilities to communicate with the LBT TCS, and list of parameters to retrieve telescope and observatory information from the LBT Data Dictionary.

These need to be copied into the relevant `/home/lbt/Config/IIF/` folder, and setup as either MODS1 or MODS2 as needed in the modsDD.ini runtime
configuration file.

For a complete current list of public Data Dictionary entries available, see https://github.com/LBTO/ in `tcs/iif/configuration/IIF/DDNames`

## Config directory

For a "flight" MODS data-taking system, the `modsDD` and related data-taking system agents will look for runtime configuration files in
the `/home/mods/Config` folder.  Each of the two configuration file types has `_MODS1` and `_MODS2` variants.

To install the configuration files, copy the MODS1 or MODS2 variant as needed into `modsDD.ini` and `lbtIIF.client` (or make symbolic links).
Always avoid editing the files if possible.

### modsDD Agent Runtime Configuration Files:

 * `modsDD_MODS1.ini` - MODS1 installed at SX direct gregorian
 * `modsDD_MODS2.ini` - MODS2 installed at DX direct gregorian
  
### IIF client Configuration Files:

These are selected by setting the `PropFile` parameter (without .client) in the `modsDD.ini` file
 * `lbtIIF_MODS1.client` - for MODS1 installed at SX direct gregorian
 * `lbtIIF_MODS2.client` - for MODS2 installed at DX direct gregorian
 * `tcsSim.client` - LBTO telescope simulator (obtained from LBTO), for example running at OSU
     
### LBT Data Dictionary Tables:

These are selected by setting the `Side` parameter in the `modsDD.ini` file to either "left" or "right"
 * `iifDD.left` - DD variable for Left (SX) Direct Gregorian focal station (MODS1)
 * `iifDD.right` - DD variables for Right (DX) Direct Gregorian focal station (MODS2)This is a collection of runtine configuration files for the `modsDD` agent for
the MODS data-taking system
