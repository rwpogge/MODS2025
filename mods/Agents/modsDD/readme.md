# MODS Data Dictionary Agent

**Version: 1.0.1 - 2025 Sept 21**

**Status: beta testing**

See [release notes](releases.md) for details.

`modsDD` is a standalone (non-client) program that interrogates the MODS shared memory
segment on an active MODS instrument server machine (`mods1` or `mods2`) and uploads
relevant MODS instrument state information into the observatory IIF data dictionary (DD) 
at a set cadence (notionally 5-10 seconds).  

This program requires IIF Build 2025A or greater which has the extended
MODS data dictionary entries.  

This provides LBTO with full-time updates of instantaneous instrument status 
info that may be used by observatory dashboards or alarm state monitoring 
systems without their needing to directly interrogate the MODS instrument systems.
This should eliminate risks associated with asynchronous third-party status queries 
interrupting real-time instrument control and data-acqusition systems.

The code is designed to be run non-interactively as a systemd service.

