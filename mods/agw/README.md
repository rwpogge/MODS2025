# MODS Acquisition/Guide and Wavefront Sensor (AGW) Server

**Version 1.3**

**Updated: 2025 Oct 2 [rwp/osu]**

See the [releases.md](release notes) for details.

Ported to AlmaLinux 9.x in June 2025 for the MODS2025 CCD controller and system upgrade.

## Code 

 * `agwServers` - AGw server program
 * `agwUtils` - builds `libagwutils`, the AGw server library
 * `API` - builds `agwapi.o` used by code in `agwServer` and `agwUtils`

