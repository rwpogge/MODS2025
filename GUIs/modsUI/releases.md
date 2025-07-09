# modsUI Release Notes
Original Build: 2009 Feb 24

Last Build: 2024 Jul 8

## Version 3.0.0
2025 July 9

* Start of development of the version for the Archon controller system for MODS.  New features need to be added and old features disabled, so advancing the major version number.

## Version 2.2.0
Friday, July 8th, 2024

- Qt 5.X -> Qt 6.X
    - The GUI should now be compiled using Qt 6.

## Version 2.1.0
Friday, February 23rd, 2024

- Qt 4.X -> Qt 5.X
    - The GUI should now be compiled using Qt 5.
    - In several instances `QString::simplified()` was used - but the return value wasn't. These locations were fixed and marked with TODOs.
- Unused flag variables (cmdDone, cmdFault, and cmdWarn) were commented out in many files to remove compiler warnings. Eventually it would be more efficient to refactor the code in these places to use if statements rather than switches. The altered locations were marked with TODOs.
    - setup/fileconfig.cpp: cmdDone and cmdFault flags.
    - setup/instconfig.cpp: cmdDone flag.
    - setup/configmanager.cpp: cmdFault flag.
    - monitor/envpanel.cpp: cmdDone and cmdFault flags.
    - instutils/llbpanel.cpp: cmdDone and cmdFault flags.
    - instutils/iubpanel.cpp: cmdDone and cmdFault flags.
    - instutils/iebpanel.cpp: cmdDone and cmdFault flags.
    - dashboard/modschannel.cpp: cmdWarn flag.
    - dashboard/focalplane.cpp: cmdDone flag.
- An ambigous brace warning was fixed.
    - dashboard/modschannel.cpp.
    
## Version 2.0 bino

The version used when this repository was started.

## Version 1.25.x
2013 Aug 25

 * Many modifications from MODS1 in the field and MODS2 from during lab acceptance testing.  
 * Added sticky, pop-up (modeless dialog) alerts via the monitor/envpanel widget.  These alerts indicate when temperatures or pressures go outside operating limits.  They also note when conditions return to normal.  Alerts also include sensing breaker trips and loss of contact with the CCD sequencers.
 * Added "sticky" logging of command errors and warnings to the instrument dashboard (dashboard/dashboard.cpp) by adding a secondinstance of a CommLog widget.  This is to ensure that errors and warnings are not lost during high-traffic periods (e.g., during CCD readout or instrument config changes).
 * Many (*many*) minor tweaks and corrections to clean up annoyances from MODS1 that would doubly annoy with MODS1+2.

## Version 1.16.x
2011 Aug 11
 * Large number of modifications and additions derived from MODS1 on-sky commissioning activities, first release after summer shutdown activities.
   * Put in additional safeties for IEB/IUB power controls
   * Env monitoring for the IE now handled by the modsenv agent rather than bothering the IE
   * Many small tweaks, patches, and bug fixes have been applied.
 * Dashboard control panel reworked to make some functions monitor instead of active controls to prevent careless observers from badly misconfiguring the instrument (instconfig is now high level)
 * Changes to status to ensure correct filenames pending the September 2011 science verification run and IC code v6.0 implementation.

## Version 1.5 through 1.15
2010 Nov to 2011 June
 * Many versions during on-sky/on-telescope commissioning.  Mostly reactive so was not keeping detailed releases.  Revisions and updates were coming at a pace of 2 per observing run (one before the start, one after the run was complete, occasionally mid-run if something important).
   
## Version 1.3
2010 Oct 15
 * Version used during first-light with MODS1, now read/write operations with the live LBT TCS, changes drive by changes in the lbttcs agent which is the only program directly facing the LBT TCS.

## Version 1.1
2010 Aug 30
 * First version of the GUI used for MODS1 post-installation on the telescope, reacting to development of the LBT TCS and data dictionary, but only in readonly mode.
   
## Version 1.0 
2010 Feb 10
 * First 1.0 release following the successful MODS1 Laboratory Acceptance Review in Columbus in January 2010. Version incorporated numerous changes from the lab testing version at OSU, and was locked down to be used again when MODS1 is re-integrated in the mountain instrument lab during summer 2010.
 * Added labeled sexagesimal entry widget AngleEntry.cpp, based on specs for the TCS interface that became available after Lab Acceptance review.

## Version 0.9
2009 December 30
 * Last release after lab testing of MODS1 at OSU over the holidays. This is the release we will use for the Laboratory Acceptance Review in January. It was crunch time for producing the lab acceptance testing report, so changes were not adequately documented, mostly reactive bug fixes and no real changes in overall code architecture.

## Versions 0.5 - 0.9
April through December 2009

 * new versions with each installation of an instrument group to the instrument and subsequent testing - numbers advanced roughly 1-2 month intervals
 * adding interaction with the CCD controllers for the red and blue channels (blue and red IC DOS computers)
 * added "psychological" progress bars for exposure countdown and readout, the latter tracking status messages from the IC programs. Use exposure start status message to start the countdown timer, stop a couple of seconds before end of countdown to account for comm lags, so countdown progress bar freezes for ~1-3-seconds then is zeroed when the IC says readout had started.  Most observers did not notice the slight discrepancy.
   
## Version 0.4
2009 Mar 1
 * Added the loadIDTable() method to read mechanism IDs from files in indexedmechanisms.cpp

## Version 0.3
2009 Feb 16 and 20
 * changed the GUI color scheme for better visibility in the select entry widget in the linearmechanism container class
 * add idsChanged() signal/slit and setIDList() method for handling mechanisms with named positions (filter wheels etc.) in indexedmechanisms.cpp
 * added distinction between data "ready" and data "committed" states in the numberentry widget

## Version 0.2
2008 May 26
 * Huge overhaul in the data entry widgets (doubleentry.cpp, intentry.cpp, selectentry.cpp, etc.)
 * Changes in textdisplay.cpp color manipulation using QPallet for greater consistency with Qt best practices

## Version 0.1
2008 Feb 25
 * First numbered version, still learning how to build the GUI structure, basic ISIS interaction, and logging.  Learning Qt as I go...


