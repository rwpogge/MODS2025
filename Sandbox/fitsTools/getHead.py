#!/usr/bin/env python

import os, sys
from astropy.io import fits

if len(sys.argv) == 3:
    fitsKey = sys.argv[1]
elif len(sys.argv) > 3:
    fitsKey = sys.argv[1]
else:
    print(f"Usage: {os.path.basename(sys.argv[0])} keyword fitsFile(s)")
    sys.exit(1)

for fitsFile in sys.argv[2:]:
    try:
        hdu = fits.open(fitsFile)
    except Exception as exp:
        print(f"ERROR: cannot open {fitsFile} - {exp}")
        sys.exit(1)

    try:
        print(f"{fitsFile}: {hdu[0].header[fitsKey]}")
    except Exception as exp:
        print(f"ERROR: cannot read FITS header - {exp}")
    hdu.close()
    
sys.exit(0)
