# lbttcs Agent Runtime Configuration Files

**Original: 2014 July 1 [rwp/osu]**

**Updated: 2025 July 9 [rwp/osu]**

These files are used by the lbttcs agent and the IIF ICE utilities to
communicate with the LBT TCS, and list of parameters to retrieve
telescope and observatory information from the LBT Data Dictionary.

These need to be copied into the relevant ~mods/Config/IIF/ folder,
and setup as either MODS1 or MODS2 as needed in the lbttcs.ini runtime
configuration file.

For a complete current list of public Data Dictionary entries available, see
https://github.com/LBTO/ in tcs/iif/configuration/IIF/DDNames

## Config directory

For a "flight" MODS data-taking system, the `lbttcs` and related data-taking
system agents will look for runtime configuration files is `/home/mods/Config/IIF`

### lbttcs Agent Runtime Configuration File

 * `lbttcs.ini` - runtime configuration file for the `lbttcs`. Two versions are provided for MODS1 and MODS2
   * `lbttcs_MODS1.ini` - MODS1 installed at SX direct gregorian
   * `lbttcs_MODS2.ini` - MODS2 installed at DX direct gregorian

### IIF client Configuration Files:

These are selected by setting the `PropFile` parameter (without .client) in the `lbttcs.ini` file
 * `lbtIIF.client` - live LBT client config for an active instrument. Two versions:
   * `lbtIIF_MODS1.client` - for MODS1 installed at SX direct gregorian
   * `lbtIIF_MODS2.client` - for MODS2 installed at DX direct gregorian
 * `tcsSim.client` - LBTO telescope simulator (obtained from LBTO), for example running at OSU
     
### LBT Data Dictionary Tables:

These are selected by setting the `Side` parameter in the `lbttcs.ini` file to either "left" or "right"
 * `iifDD.left` - DD variable for Left (SX) Direct Gregorian focal station (MODS1)
 * `iifDD.right` - DD variables for Right (DX) Direct Gregorian focal station (MODS2)This is a collection of runtine configuration files for the `lbttcs` agent for
the MODS data-taking system
