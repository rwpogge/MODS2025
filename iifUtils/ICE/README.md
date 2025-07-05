# iifUtils ZeroC ICE Slice files

**Original: 2015 Aug 10 [rwp/osu]**

**Updated: 2024 July 4 [rwp/osu]**

This directory contains the Slice language .ice files used to generate the C++ code for the LBT ICE-based IIF functions.

**Current Build: Build2025X**

## Overview

LBT uses the ZeroC Internet Communications Engine (ICE) to communicate with the Telescope Control System's Instrument Interface
(TCS IIF).  The ICE interface routines for the IIF are implemented in Slice (Specification Language for Ice).  A Slice file has the
.ice extension.

For the MODS IIF client we need two .ice files:
 * `Factory.ice` - Interface that controls client connections
 * `iifServer.ice` - LBT IIF server implementation

## Ice Version

For the first binocular science operations version of the MODS/IIF interface for MODS1 and MODS2
we used ZeroC Ice version 3.3.1. 

As of 2025, LBTO is using **Ice v3.7** which is compatible with their TCS simulator.

## Slice File Source:

Retrieve the slice files appropriate for the current TCS build from the LBTO GitHub repository:
 * July 2025 update: https://github.com/LBTO/tcs/tree/2024A/iif/ice

Get `Factory.ice` and `IIFServer.ice` from this directory (we don't need the other files):

### "result" hack:

I had to hack `IIFServer.ice` to get compilable C++ code because there is a conflict of using "result" which is now a 
reserved data type starting with Ice v3.7.  For this copy we hanged `result` to `iifres` and changed all the downstream
code in `iifUtils` to get a clean build.

#### WARNING

In future LBT TCS builds we expect this problem will be eliminated and we will update this repository notes with compatible
code instead of the initial testing versions using our `iifres` hack.
  
## Compilation

The .ice files are compiled using the slice2cpp program installed along
with the ZeroC Ice code:
 > slice2cpp Factory.ice
 > slice2cpp IIFServer.ice

will generate `Factory.cpp/.h` and `IIFServer.ccp/.h` files that must be copied into the `iifUtils` main directory before
compiling the `libiifutils` library.
