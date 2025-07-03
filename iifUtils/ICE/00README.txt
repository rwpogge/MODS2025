iifUtils ZeroC ICE Slice language files
Updated: 2015 Aug 10 [rwp/osu]

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
  ZeroC Ice version 3.3.1.  LBT generally uses v3.3, but as of now the
  current release from zeroc.com is v3.5.


Retrieve from SVN
  svn export https://svn.lbto.org/repos/tcs/branches/2015B/iif/ice/Factory.ice
  svn export https://svn.lbto.org/repos/tcs/branches/2015B/iif/ice/IIFServer.ice

Compilation:

  The .ice files are compiled using the slice2cpp program installed along
  with the ZeroC Ice code.

     slice2cpp Factory.ice
     slice2cpp IIFServer.ice

  will generate Factory.cpp/.h and IIFServer.ccp/.h files.  These need
  to be copied into the parent directory with the main iifUtils source
  code before building the iifUtils library.


------------------------------
R. Pogge, OSU Astronomy Dept.
pogge@astronomy.ohio-state.edu
