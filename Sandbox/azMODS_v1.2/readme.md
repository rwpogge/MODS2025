# azcam-mods v1.2

Updated: 2025 Nov 28 [rwp/osu]

Sandbox where we are gathering the elements of the upcoming v1.2 of the MODS azcam server.

This includes working Archon configuration (aka timing) files for the 4 CCDs after
work by Mike Lesser to get them operational.

This occasioned changes in these `azcam-mods` files:
 * `server.py`
 * `console.py`
 * `detector_mods.py`

New bits are a template yaml-format `.ini` file for azcam-mods server for the 
different MODS "systems" so we don't have to hard-code system-specific items
into `server.py`.
 * `azcam.ini` - server .ini template 
 * `modsYAML.ipynb` - notebook for YAML parser debugging

