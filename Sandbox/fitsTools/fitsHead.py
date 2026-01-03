#!/usr/bin/env python

import os, sys
from astropy.io import fits

if len(sys.argv) == 3:
    fitsFile = sys.argv[1]
    fitsExt = int(sys.argv[2])
elif len(sys.argv) == 2:
    fitsFile = sys.argv[1]
    fitsExt = 0
else:
    print(f"Usage: {os.path.basename(sys.argv[0])} fitsFile [extNum]")
    sys.exit(1)

try:
   hdu = fits.open(fitsFile)
except Exception as exp:
   print(f"ERROR: cannot open {fitsFile} - {exp}")
   sys.exit(1)

try:
    print(repr(hdu[fitsExt].header))
except Exception as exp:
    print(f"ERROR: cannot read FITS header in extension {fitsExt} - {exp}")
    sys.exit(1)

sys.exit(0)
