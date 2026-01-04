# fitsTools - simple FITS tools

**Updated: 2026 Jan 03 [rwp/osu]**

## Overview

Set of simple python command-line programs for extracting information from FITS
headers.  Helpful for looking at MODS MEF headers without having to crack open
IRAF.

### Contents
 * `fitsHead.py` - dump a FITS header to stdout for a given FITS file, one at a time. If no second argument, dumps the Primary HDU, otherwise give the number of the extension to dump.
 * `getHead.py` - get the value of a Primary HDU header card from one or more FITS files.  Works with wildcard.

### Installation
```
   chmod +x fitsHead.py getHead.py
   cp fitsHead.py /path/to/wherever/fitshead
   cp getHead.py /path/to/wherever/gethead
```
