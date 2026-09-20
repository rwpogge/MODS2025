# MODS Ginga Display Notes

Ginga version 7.4.0

## Overview

Sandbox to test adopting Ginga as our raw image display tool to replace
SAOImage ds9 which is now using SAMP and is not always reliable or the
best choice going forward (has internal dependency on LBTO libraries
for SAMP handling).

Began work on 2026 Sept 19, largely in response to on-going complaints
about latency and mis-display with modsDisp, not being able to look back
at images, or do sensible quick-look (PSF measurement, line plots) with
ds9.

## Ginga Startup Notes

Start app with remote control and port defined

   ginga --modules=RC --rcport=12345

Add the image picker (measuring stars) and cuts (plot along lines)

   ginga --modules=RC,Pick,Cuts --rcport=12345

Which ports?  Follow convention for B/R using 1/2 in range

   MODSxB - 11711
   MODSxR - 11712

## Code stubs
 
 * `testGinga.py` - ipython stub for playing with Ginga and python




