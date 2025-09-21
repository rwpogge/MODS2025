# modsDD Release Notes
Last Build: 2025 Sept 21

## Version 1.0.1
2025 Sept 21

Verified and updated based on live testing at LBTO with MODS1 and IIF Build 2025A.  Includes
all mechanisms groups, power states, and most environmental sensor readings. Will not include
data that can only originate from the Archons, which will be sent via the azcam server
using the `SetParameter()` method.

Prototype systemd service files, but will wait until we can live test with a full instrument
mechanism suite running before we'll start testing.

