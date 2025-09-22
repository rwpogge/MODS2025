# MODS data-taking system agents

This folder contains versions of the MODS data-taking system agents ported to AlmaLinux 9
for the MODS2025 update.  

This is a start on the "installation and re-commissioning" repository at OSU that will
migrate to the LBTO GitHub as final "flight" repository after recommissioning.

## Contents

All programs are released for operations except as noted.

 * `lbttcs` - LBT Telescope Control Interface agent.  Built on ZeroC Ice v3.7 and LBT TCS Build 2025A
 * `modsEnv` - MODS environmental sensor logger. Uses hdf5 for LBT-compatible telemetry logs 
 * `modsHEB` - MODS Archon Head Electronics Box (HEB) agent
 * `modsCCD` - MODS Azcam/Archon "IC" agent - Ready for initial deployment, tested with an Archon and the MODS lab test dewar
 * `modsDD` - MODS Data Dictionary update agent.  Build on ZeroC Ice v3.7 and LBT TCS Build 2025A
 * `dataMan` - MODS data manager, currently in Sandbox development (pre-alpha)

