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

## Contents

The code creates the `libiifutils` library with the MODS/TCS interface

### Code base:

  * `Factory.cpp` and `Factory.h` - ZeroC Ice value factory for the LBTO TCS IIF interface for the current TCS build, generated from `Factory.ice` with `slice2cpp`
 * `IIFServer.cpp` and `IIFServer.h` - LBT IIF server interface code for the current TCS build, generated from `IIFServer.ice` using `slice2cpp`
 * `modsIIFUtils.cpp` - MODS-specific wrappers for the IIF interface functions
 * `modsIIFUtils.h` - header file for MODS apps using `libiifutils`
 * `modsIIFCalls.h` - MODS IIF function call definition header
 * `modsIIFGlobals.h` - Global variable header used for the MODS/IIF interface, including the ZeroC Ice communicator

### Sub-folders:
 * `ICE` - the ZeroC Ice slice files for the LBT TCS IIF interface, provided by LBTO for the current TCS build
 * `Config` - Copies of runtime configurat files used by the IIF client routines (e.g., `lbttcs`) and the Data Dictionary (DD) entries specific to MODS data-taking, with two versions for the left (SX) and right (DX) telescopes, and IIF client setup.  Includes flight and lab configs.

