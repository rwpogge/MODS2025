# MODS AGw Stage Server release notes

## Version 1.3.0: 2025 July
 * AlmaLinux 9 port and clean-up in advance of the MODS2025 Archon controller and data-taking system overhaul [rwp]

## Version 1.2.3: [2013-10-03]
 * Various changes derived from MODS2 OSU Lab Testing getparam removed from samples, makefile cleaned
     minor changes to the include files to better obey limits

## Version 1.2.2: [2010-11-24]
**NOTE: Not applied to the LBTO MODS1 as no time for testing but applied to OSU MODS2 in the lab**
 * GPOFFSET and SETXY will now check sensor bits and return with ERROR if sensors have been asserted after a move.
 * Logging path variable AGWLOG was added to agw.ini file. Variable is located in /home2/mods/agw_1.2.0/include/agwutils.h
 * The agw.log file path is now loaded from /home/mods/Config/MODS1/agw.ini at startup.

## Version 1.2.1: [2010-10-20]
 * Patches derived from the LBT version after the Sept 2010 MODS1 AGw commissioning run - primarily bug patches during early testing [rwp]
 * See also include/MODSAGWCoords.h for minor changes there.  Mostly modification of the AGW stage Y limit to allow a margin of 2mm at the field center.

## Version 1.2.0: [2010-08-26]
 * SVN checked in at LBT for use with the GCS (Torsten Leibold).
 * Last revision before MODS1 installed at the SX direct gregorian focal station end of August

## Version 1.1.3: [2010-03-24]
 * patched in the transformation coefficients written up by Rick Pogge

## Version 1.1.2: [2010-03-24]
 * remove debug/test statements.
 * remove timing scaffolding.

## Version 1.1.1: [01-11-10]
 * Added AGw power control to init.c app.
 * Added agwServer binding identifier.

## Version 1.1.0: [11-05-09]
 * Discussions between LBT0 and OSU staff required major changes.
 * Intensive efforts were made to have the AGw conform to LBTO standards
 * Tested and approved by the LBTO staff via LBTO tunneling to OSU ISIS3
 * development system.

## Version 1.0.0: [2009-09-10]
 * First complete AGw server version with the AGw unit installed on MODS1 in the OSU lab.
