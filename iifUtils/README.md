# iifUtils

Interface between the ZeroC Ice IIF for the LBT and the MODS `lbttcs` interface agent.  The `lbttcs` agent 
is an ISIS client that connects all MODS instrument services to the LBT TCS system.  All the interactions
with Ice/IIF are contained in the `iifUtils` library functions.

The code on this repository is the version running at LBTO since 2014 for both MODS1 and MODS2.  The
`Factory.h` file was generated from `Factory.ice` using Ice version 3.3.1.

The `Makefile.build` is modified from the version used on the mountain as this is start of the port
of the code to AlmaLinux 9.x.  We have ZeroC Ice 3.7 installed on the AlmaLinux box at OSU.
