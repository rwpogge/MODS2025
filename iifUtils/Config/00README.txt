LBT MODS IIF Configuration Files
Updated: 2014 July 1 [rwp/osu]

These files are used by the lbttcs agent and the IIF ICE utilities to
communicate with the LBT TCS, and list of parameters to retrieve
telescope and observatory information from the LBT Data Dictionary.

These need to be copied into the relevant ~mods/Config/IIF/ folder,
and setup as either MODS1 or MODS2 as needed in the lbttcs.ini runtime
configuration file.

For a complete current list of public Data Dictionary entries available, see

   http://wiki.lbto.org/pub/Software/TCSsoftware

and retrieve the most recent "IIF Public Data Dictionary Variable List"


lbttcs Agent Runtime Configuration File:

   lbttcs.ini   

IIF client Configuration Files:

  These are selected by setting the PropFile parameter (without .client)
  in the lbttcs.ini file

    lbtIIF.client  -- live LBT client config for an active instrument
    lbtSim.client  -- LBT telescope simulator (lab or LBT)

LBT Data Dictionary Tables:

  These are selected by setting the Side parameter in the lbttcs.ini file
  to either "left" or "right"

    iifDD.left  -- DD variable for Left Direct Gregorian (MODS1)
    iifDD.right -- DD variables for Right Direct Gregorian (MODS2)

------------------------------
R. Pogge, OSU Astronomy Dept.
pogge@astronomy.ohio-state.edu
