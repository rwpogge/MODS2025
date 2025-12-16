# MODS Lab Test Dewar Archon Configuration Files

**Updated: 2025 Dec 16 [rwp/osu]**

This is where we keep MODS Archon configuration files (acf and ncf)
developed during lab testing with the Archon and MODS Lab Test Dewar
in Tucson.

Because the lab test dewar is wired differently than the "flight" dewars
on the MODS instruments, these files may only be used with the lab test
dewar.

## IMPORTANT NOTE - READ THIS NOW!!!

Do not use any of these files on a MODS instrument Archon on the
mountain.  The science dewars were wired differently (order of serial
lines) an the lab test dewar config files won't work with those systems.

## Contents

 * `MODS_Actual.ncf` is the current MODS flight or near-flight testing configuration file, but is formatted for use with the `archongui` program only.  If used with `azcam`, it will fail to load.

 * `MODS_Actual.acf` is an edited ACF file based on `MODS_Actual.ncf` that is edited to use with `azcam`, but will not load with `archongui`.

 * `MODS_Example_2.ncf` is the NCF file used for Tucson lab testing with the MODS Lab Test Dewar system.  It is formatted for use with the `archongui` and will nto work with `azcam`.  `MODS_Actual.xxx` are derived from this template, but there are important differences, so do not use for "flight".
 
 * `mods_test.ncf` is a copy of `MODS_Example_2.ncf` that may be used for rapid testing work without touching the reference copy of the file.

## `MODS_Actual.acf` Archon Configuration File

### The `Old/` folder

These are old config files of historial interest developed during the early stages
of MODS Archon development.  They are kept here in case we need to revisit some of the earlier work.

 * `MODS_mode_3k.ncf` and `MODS_mode_3K_1.ncf" are files developed while testing ROI and binning modes. **These are here for historical reference only and are never to be used with a live system**

 * `MODS_Example_1.ncf` is an early full-frame readout file that does not include the extended timing multiplier to allow exposure times longer than about 17.4 minutes.  **Do Not Use**

