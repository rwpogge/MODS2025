# lbttcs - MODS LBT TCS Interface Agent

Original Build: 

Last Build: 2025 July 5

## Version 3.1.0-bino [2025 July]
 * AlmaLinux 9 port and updates to adapt to ZeroC Ice version 3.7
 * Tested on the LBT TCS simulator in the OSU Lab
 * Had to hack IIFServer.ice in Build2025X (LBTO GitHub) because "result" is now reserved in the C++ implementation Ice starting with v3.7, renamed iifres and changed in iifUtils code.
 * Added hooks for new telemetry from the telescope (Temps401-408, M1 glass temp, M1Temp2, and AirMass) absent in previous versions
 * Minor code fixes occasioned by stricter g++ compiler in AlmaLinux 9 system

## Version 3.0 [2015 Aug 11]
 * Binocular telescope control implemented starting with LBT IIF TCS Build 2015B.
 * Major revisions, including the new bc (binocular control) command for binocular presets
 * Will test live with MODS1 and MODS2 after summer shutdown
 * Corresponds to the last major build of `iifUtils`

While LUCI2 and MODS2 were on the telescope in late 2013 and early 2014, respectively, LBTO was not yet ready to operate in true binocular mode.  We commissioned
both LUCI2 and MODS2 and one-eyed instruments, being unable to co-point or guide both sides of the telescope until 2015.  Development of a truly binocular
version of iifUtils and lbttcs began in July 2014.

## Version 2.11 [2013 Nov 16]
 * recompiled due to change in OffsetPointing() behavior
 * yet more undocumented changes in IIF behavior with recent TCS builds.  [rwp/osu]
 * Last revision to the flight lbttcs before installation of MODS2 in spring 2014.

## Verision 2.10 [2013 Sept 23]
 * recompile with changes to iifUtils library to satisfy a new requirement for no-value for the filter type and color type in SetStars() [rwp/osu]

## Version 2.9 [2012 Sep 16]
 * updated exit() status codes.  See the tables in doc/lbttcs_ExitStatus.txt for details [rwp/osu]

## Version 2.8 [2012 May 15]
 * added test for receiving a warning back from the IIF server in response to an offset request [rwp/osu]

## Version v2.7.4 [2011-08-11]
Various updates from MODS1 commissioning.  
 * v2.7 uses the new LBTO-approved aliases for Data Dictionary (DD) entries rather than full qualified keywords
 * We finally seem to have fixed the HA sign problem.  Why the DD does not compute HA is still beyond me...

During MODS1 on-sky commissioning from 2010 August through 2011 June (start of 2011 summer shutdown), changes
coming fast and furious.  We went to on-sky with Version 1, big changes in the IIF came down that required 
advancing to V2.0 by start of 2011.  Surfaced from the firehose of commissioning to restart formal version
numbering with 2.7 patch 4.
