# iifUtils - IIF interface utility functions for MODS

Original Build: 2014 July 1

Last Build: 2025 Jul 4

## Version 2.1.0 - 2025 July 4
 * Port to AlmaLinux 9 for the MODS2025 updates
 * Changes required to run under ZeroC Ice v3.7, mainly changing the "result" type from IIFServer.ice to "iifres" to avoid name conflicts with reserved type "result" in ZeroC Ice 3.7++
 * Moved code out from modsUI/Utilities into mods/utilities and libraries and headers now go in common into mods/ulib and mods/include like other MODS function libraries
 * Rebuilt Makefile for the new code distibution
 * Cleaned up code and added bits needed for doing future revision control with GitHub

## Version 2.0.2 - 2015 Aug 11
 * Minor modification for binocular operation referenced to IIF Build2015B

## Version 2.0 - 2014 July 1
 * Version 2 is a major overhaul for binocular operation begun during summer shutdown 2014 to be ready for operation in 2015 (target for first MODS1+2 binocular operation)

