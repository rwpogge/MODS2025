# modsHeb Release Notes
Last Build: 2025 Oct 4

## Version 1.0.2
2025 Oct 4

Updated `modbusutils.c` to include a 10msec pause between `modsbus_new_tcp()` and `modsbus_connect()`
calls to allow the open to finish before connecting. Was seeing an "operation now in progress" error
on `modbus_connect()` on the MODS2 red HEB.  Same issue seen in `modsEnv` and `mmcServer`
read of HEB data.  

## Version 1.0.1
2025 August 26

Verified and updated based on live testing at LBTO with MODS1 and MODS2 during 
summer shutdown following installation of the new Archon HEB systems. Decision made
to use for engineering rather than regular observing operations, and moved HEB
sensor monitoring functions into the `modsEnv` agent for logging and injection into
the LBTO telemetry stream.

## Version 1.0.0
2025 July 20

First release version of modsHEB to the development GitHub, preparation to
test at LBTO during Archon installation.
