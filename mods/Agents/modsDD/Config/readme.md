# modsDD Agent Runtime Configuration Files

**Updated: 2025 Sept 21 [rwp/osu]**

These files are used by the modsDD agent and the IIF ICE utilities to communicate with the LBT TCS, and list of parameters to retrieve telescope and observatory information from the LBT Data Dictionary.

These need to be copied into the `/home/lbt/Config/` folder, and setup as either MODS1 or MODS2 as needed in the modsDD.ini runtime
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
  
## Installation

Copy the contents of this `Config/` folder into the MODS1 and MODS2 subfolders in the 
public `/home/dts/Config` folder:
```shell
  cd /home/dts/Config
  cp MODS2025/mods/Agents/modsDD/Config/modsDD_MODS1.ini MODS1/
  cp MODS2025/mods/Agents/modsDD/Config/modsDD_MODS2.ini MODS2/
  cd ..
```
Then make a link pointing to the correct file for the particular MODS instance:
```
  cd /home/dts/Config
  ln -s MODS1/modsDD_MODS1.ini modsDD.ini
```
This example sets up `modsDD` on the `mods1` instrument server.
  
