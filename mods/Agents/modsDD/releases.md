# modsDD agent release notes
Last Build: 2025 Oct 3

## Version 1.1
2025 Oct 3
 * Updates for IIF Build 2025B
 * added S_MODSBlueGratingtilt and S_MODSRedGratingTilt parameters added to the DD in Build 2025B
 * added S_MODSMode data (OBSMODE/CALMODE) added to the DD in Build 2025B
 * changed value of S_MODSInstConfig to be "Dual/Blue/Red Imaging/Grating/Prism" using captured dichroic and grating turret positions

## Version 1.0
2025 Sept 21

Verified and updated based on live testing at LBTO with MODS1 and IIF Build 2025A. The data
sent are all MODS mechanisms states, subsystem power states, and environmental sensor readings
that are exposed via the MODS shared memory segment. `modsDD` will not send sensor data
that originates from the Archon controllers, principally the CCD detector and detector mount base
temperatures and the Archon electronics backplane temperature.  These data will be sent by the MODS
`azcam` server using the `SetParameter()` method in `lbto-iif` python module.

Prototype systemd service files have been created but won't be used until we can do a live test with a 
full instrument mechanism suite running.
