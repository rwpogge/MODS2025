# MODS Archon Configuration Files

This is where we keep MODS Archon configuration files (acf and ncf)
developed during lab testing with the Archon and MODS Lab Test
Dewar in Tucson, and later testing at the LBT with the MODS science
detectors and Archons.

These are designed primarily to be used with the `archongui` engineering
tool

## IMPORTANT NOTE - READ THIS NOW!!!

Do not use any of these files on a live system except at the
direct instruction of Rick Pogge at OSU.  These include test
and example configurations that are **NOT** valid for operating
the actual MODS detectors.

These contain "bare metal" instructions to the controller to
operate the CCD.  Changes made without care could damage the
CCD or controller.

## Contents

 * `MODS_Actual.ncf` is the current MODS flight or near-flight testing configuration file, but is formatted for use with the `archongui` program only.  If used with `azcam`, it will fail to load.

 * `MODS_Actual.acf` is an edited ACF file based on `MODS_Actual.ncf` that is edited to use with `azcam`, but will not load with `archongui`.

 * `MODS_Example_2.ncf` is the NCF file used for Tucson lab testing with the MODS Lab Test Dewar system.  It is formatted for use with the `archongui` and will nto work with `azcam`.  `MODS_Actual.xxx` are derived from this template, but there are important differences, so do not use for "flight".
 
 * `mods_test.ncf` is a copy of `MODS_Example_2.ncf` that may be used for rapid testing work without touching the reference copy of the file.
 
### The `Old/` folder:

These are config files of historial interest developed during the early stages
of MODS Archon development.  They are kept here in case we need to revisit some of the earlier work.

 * `MODS_mode_3k.ncf` and `MODS_mode_3K_1.ncf" are files developed while testing ROI and binning modes. **These are here for historical reference only and are never to be used with a live system**

 * `MODS_Example_1.ncf` is an early full-frame readout file that does not include the extended timing multiplier to allow exposure times longer than about 17.4 minutes.  **Do Not Use**

