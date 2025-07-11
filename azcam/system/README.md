# MODS azcam system files

We restructured the configuration for azcam for MODS to segregate system configuration files and logs from raw imaging data.  This is to help
streamline our configuration management (4 Archon controllers across 2 instruments in the same rack), and to make getting at raw science image
transparent without exposing critical system configuration files.

We use `system` as the root folder for all system-specific files
 * `archon/` for the Archon controller files (.acf and .ncf)
 * `logs/` for azcam server logs
 * `parameters/` for azcam server parameter files
 * `templates/` for FITS header template and IIF data dictionary files

