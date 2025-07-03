# iifUtils

Interface between the ZeroC Ice IIF for the LBT and the MODS `lbttcs` interface agent.  The `lbttcs` agent 
is an ISIS client that connects all MODS instrument services to the LBT TCS system.  All the interactions
with Ice/IIF are contained in the `iifUtils` library functions.

The code on this repository is the version running at LBTO since 2014 for both MODS1 and MODS2.  The
`Factory.h` file was generated from `Factory.ice` using Ice version 3.3.1.

The `Makefile.build` is modified from the version used on the mountain as this is start of the port
of the code to AlmaLinux 9.x.  We have ZeroC Ice 3.7 installed on the AlmaLinux box at OSU.

The ICE folder contains the Factory.ice and IIFServer.ice files for the different TCS builds.  For
The MODS2025 development, we are using a `ICE/Build2025X` (our name for it), hacked to fix a bug
whereby "result" defined in the .ice file has a conflict with a reserved "result" definition in 
Ice 3.7, changed to "iifres" to avoid this, edited, and regenerated files with `slice2cpp`.  Apparently
this is not an issue with Java apps using Ice 3.7 
