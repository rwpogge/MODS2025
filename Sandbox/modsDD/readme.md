# MODS DD Agent - Sandbox

`modsDD` is standalone (non-client) program that interrogates the shared memory
segment and send relevant MODS instrument state information into the IIF data
dictionary (DD).  This requires IIF Build 2025A or greater which has the extended
MODS data dictionary entries.  This provides LBTO with full-time update of
instantaneous (~5-10s cadence) instrument status info that may be used on 
dashboards or for alarm state monitoring by observatory systems without needing
to directly interrogate the MODS instrument systems, with the attendant
risk of interrupting real-time instrument control and data-acqusition systems.

The code is designed to be run non-interactively as a systemd service.

Check that it is properly sending the DD info by interrogating the DD.
