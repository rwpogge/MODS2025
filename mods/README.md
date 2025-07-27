# MODS instrument control and data acquisition system

Main code tree for the MODS instrument control and data-taking software.

## Main Components

  * `Agents` - client agents of the data-taking system (`lbttcs`, `modsEnv`, `modsCCD`, etc.)
  * `Config` - runtime configuration files, including instrument mechanisms, filter tables, grating tables, etc.
  * `GUIs` - graphical user interfaces: `modsUI`, and engineering tools like `iTerm` and `imsTool`
  * `TclTk` - tcl/tk code for the IMCS monitor GUIs (aka the IMCS "radar screens")
  * `agw` - MODS AGw server code (agwServer)
  * `mmc` - MODS Mechanism Controller (mmc) service (aka the MODS "IE")
  * `utilities` - MODS utilities libraries
  * `modsalloc` - MODS instrument server shared memory segment service (run as a `systemctl` service)
  * `plc` - MODS MicroLYNX-7 stepper motor controller microcode for the MODS mechanisms

### System folders
  * `ulib` - compiled data-taking system module libraries (e.g. `libagwutils.a`)
  * `include` - data-taking system header files (e.g., `mmc_commands.h`)
  * `bin` - compiled binaries out of the public path (usually copied to /home/dts/bin or /usr/local/bin)
  * `obj` - compiled object files
  * `log` - log files generated during code testing to keep test log files out of the "flight" telemetry logs
  * `data` - data files generated during code testing to keep test data out of the archive and repository
