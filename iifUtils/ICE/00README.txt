iifUtils ZeroC ICE Slice language files
Original: 2015 Aug 10 [rwp/osu]
Updated: 2024 July 03 [rwp/osu]

This directory contains the Slice language .ice files used to generate
the C++ code for the LBT ICE-based IIF functions.

Overview:

  LBT uses the ZeroC Internet Communications Engine (ICE) to
  communicate with the Telescope Control System's Instrument Interface
  (TCS IIF).  The ICE interface routines for the IIF are implemented
  in Slice (Specification Language for Ice).  A Slice file has the
  .ice extension.

  For the MODS IIF client we need two .ice files:

    Factory.ice   - Interface that controls client connections
    iifServer.ice - IIF server implementation


Ice Version

  On the MODS server machines (mods1 and mods2), we have installed
  ZeroC Ice version 3.3.1.  LBTO used v3.3 back in 2015 when we did
  this for binocular operation, but as of 2025 for the MODS2025
  Archon controller update, LBTO is using v3.7, including the
  TCS simulator they provided.

Retrieve from GitHub:
  https://github.com/LBTO/tcs/tree/2024A/iif/ice

  Get Factory.ice and IIFServer.ice et al.

  I had to hack IIFServer.ice to get a because there is a conflict of
  using "result" which is apparently reserved.  Changed this to
  "iifres", and elsewhere to get clean builds late.
  
Compilation:

  The .ice files are compiled using the slice2cpp program installed along
  with the ZeroC Ice code:
     slice2cpp Factory.ice
     slice2cpp IIFServer.ice

  will generate Factory.cpp/.h and IIFServer.ccp/.h files.  These need
  to be copied into the parent directory with the main iifUtils source
  code before building the iifUtils library.


------------------------------
R. Pogge, OSU Astronomy Dept.
pogge.1@osu.edu
