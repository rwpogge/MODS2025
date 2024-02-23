# modsUI Release Notes
Original Build: 2009 Feb 24

Last Build: 2024 Feb 23

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