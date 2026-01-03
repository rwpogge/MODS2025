#!/usr/bin/env python

from astropy.io import fits
import sys

if len(sys.argv) == 3:
    fitsKey = sys.argv[1]
elif len(sys.argv) > 3:
    fitsKey = sys.argv[1]
else:
    print("Usage: getHead.py keyword fitsFile(s)")
    sys.exit(1)

for fitsFile in sys.argv[2:]:
    
    try:
        hdu = fits.open(fitsFile)
    except Exception as exp:
        print(f"ERROR: cannot open {fitsFile} - {exp}")
        sys.exit(1)

    try:
        print(f"{fitsFile}: {fitsKey} = {hdu[0].header[fitsKey]}")
    except Exception as exp:
        print(f"ERROR: cannot read FITS header - {exp}")
        hdu.close()
        sys.exit(1)
    hdu.close()
    
sys.exit(0)
