# MODS data-taking system agents

This folder contains versions of the MODS data-taking system agents ported to AlmaLinux 9
for the MODS2025 update.  

This is a start on the "installation and re-commissioning" repository at OSU that will
migrate to the LBTO GitHub as final "flight" repository after recommissioning.

## Contents

 * `lbttcs` - LBT Telescope Control Interface agent.  Built on ZeroC Ice v3.7 and LBT TCS Build 2025X
 * `modsEnv` - MODS environmental sensor logger. Uses hdf5 for LBT-compatible telemetry logs 
 * `modsHEB` - MODS Archon Head Electronics Box (HEB) agent - beta version
 * `modsCCD` - MODS Azcam/Archon "IC" agent - beta version
 * `dataMan` - MODS data manager, currently in Sandbox development (pre-alpha)

