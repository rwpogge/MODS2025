# MODS LBT Telescope Control System interface agent

## Overview

`lbttcs` is an ISIS-client application that acts as an IMPv2 protocol agent
to interface MODS data-taking system apps with the LBT Telescope Control System.

The LBT TCS uses ZeroC Ice for interprocess communication via their IIF (Instrument InterFace) 
application.  IIF is a subsystem of the TCS system that combines all of the subsystems needed
to operate the LBT mount and enclosure.  

For the MODS2025 update, we have ported `lbttcs` to ZeroC Ice version 3.7 (using the custom repo
setup by LBTO), and tested using the LBTO TCS simulator.  At the time of this writing, the
ZeroC slice files needed for the interface are for TCS "Build 2025X" on the LBTO GitHub repository
([https://github.com/LBTO/tcs/tree/2024A/iif/ice](iif/ice). We need the `Factory.ice` and `IIFServer.ice`
files, and process using `slice2cpp`.  These are in `iifUtils` which is the MODS-specific wrappers for
the LBTO IIFServer calls.

## Update Notes

### LBTO slice files

The `Factory.ice` and `IIFServer.ice` files from the LBTO GitHub repository needed to be modified because
of a namespace conflict producing broken C++ code with `slice2cpp` as the `result` data structure defined in
`IIFServer.ice` conflicts with a `result` type defined starting with ZeroC Ice v3.7.

Our solution is to edit `IIFServer.ice` to rename `result` as `iifres` and then creating the .ccp and .h files.
This required corresponding changes in the `iifUtils` code.
