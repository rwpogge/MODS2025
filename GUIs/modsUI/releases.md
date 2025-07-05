# modsUI Release Notes
Original Build: 2009 Feb 24

Last Build: 2024 Jul 8

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
 * Large number of modifications and additions derived from MODS1 on-sky commissioning activities.
   * Put in additional safeties for IEB/IUB power controls
   * Env monitoring for the IE now handled by the modsenv agent rather than bothering the IE
   * Many small tweaks, patches, and bug fixes have been applied.
 * Dashboard control panel reworked to make some functions monitor instead of active controls to prevent careless observers from badly misconfiguring the instrument (instconfig is now high level)
 * Changes to status to ensure correct filenames pending the September 2011 science verification run and IC code v6.0 implementation.
