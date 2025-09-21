# modsEnv Agent Runtime Configuration Files

**Updated: 2025 Aug 18 [rwp/osu]**

These are files used by modsEnv agents for each of the binocular MODS instruments.

## Config directory

For a "flight" MODS data-taking system, the `modsEnv` agent will look in `/home/dts/Confg/` for the approriate
configuration file.  There are 2 variants, each named for each MODS instrument (MODS1 and MODS2).

Only one `modsEnv` agent is needed for each MODS, so a total of 2 agents for a binocular system.

`modsEnv` runs on the instrument server machines (hosts `mods1` and `mods2`).

This is the same as the way `modsEnv` was run on the pre-Archon MODS data-taking system.

### modsEnv Agent Runtime Configuration Files:

 * `modsenv_MODS1.ini` - MODS1
 * `modsenv_MODS2.ini` - MODS2

## Installation

Copy the contents of this `Config/` folder into the MODS1 and MODS2 subfolders in the 
public `/home/dts/Config` folder:
```shell
  cd /home/dts/Config
  cp MODS2025/mods/Agents/modsEnv/Config/modsenv_MODS1.ini MODS1/
  cp MODS2025/mods/Agents/modsEnv/Config/modsenv_MODS2.ini MODS2/
  cd ..
```
Then make a link pointing to the correct file for the particular MODS instance:
```
  cd /home/dts/Config
  ln -s MODS1/modsenv_MODS1.ini modsenv.ini
```
This example sets up `modsEnv` on the `mods1` instrument server.
