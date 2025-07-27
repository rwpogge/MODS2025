# MODS data-taking system agents

This folder contains versions of the MODS data-taking system agents ported to AlmaLinux 9
for the MODS2025 update.  These are working files that will later be integrated into a proper
"re-commissioning" repository at OSU then migrate to the LBTO GitHub as final "flight" repository.

## Contents

 * `lbttcs` - LBT Telescope Control Interface agent.  Built on ZeroC Ice v3.7 and LBT TCS Build 2025X
 * `modsEnv` - MODS environmental sensor logger. Uses hdf5 for LBT-compatible telemetry logs 
 * `modsHEB` - MODS Archon Head Electronics Box (HEB) agent - beta version
 * `modsCCD` - MODS Azcam/Archon "IC" agent - beta version
 * `dataMan` - MODS data manager, currently in Sandbox development (pre-alpha)

