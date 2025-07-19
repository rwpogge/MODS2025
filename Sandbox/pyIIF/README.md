# python-iif sandbox

This is code we're working with for teaching azcam-mods how to get
TCS header info it needs directly from the LBT TCS through the
new python-iif interface (https://github.com/LBTO/ptyhon-iif).

## Contents

 * `modsTCS_left.txt` - Draft file to map IIF TCS data dictionary entries to FITS header keywords to put TCS info from SX (left) direct Gregorian into MODS1
 * `modsTCS_right.txt` - Draft file to map IIF TCS data dictionary entries to FITS header keywords to put TCS info from DX (right) direct Gregorian into MODS2
 * `getIIF.py` - test program
 * `MODS1B_Inst.txt` - Draft file to map IIF MODS instrument data dictionary entries to FITS header keywords to put instrument info into MODS1B headers.
 * `MODS_New_DDNames.txt` - List of new DD entries needed to implement an instrument header "pull" into azcam using a mapping file like `MODS1B_Inst.txt`.
 
 ## Notes
 
 azcam integrates telescope and instrument functions into an azcam server using Telescope() and Instrument() class instances built on base classes of the
 same names defined  in the main azcam code.  Buried inside each of these base classes is a "named tool" (my nomenclature) instance of the Header() base class.  It is this header "tool" that is used to build the structured subsystem FITS header keyword sets that go into the FITS headers on raw data.
 
It *is* documented, just took some digging to make the connection.  See https://azcam.readthedocs.io/classes/ and look at the Header Class summary and its
documentation (https://azcam.readthedocs.io/autocode/azcam_header/).  The `get_header()` method is the key bit, and the essential attributes are 
the `keywords`, `values`, `comments` and `typestrings` dictionaries.  On instantiating the Header() class, you can specify the title ("telescope", "instrument") and a name
of a template file with the static header information (emphasis on the words *static*).

This sandbox is various code stubs, configuration files, etc. that will be used to build the Telescope() and Instrument() classes for `azcam-mods`.  First up is the
`Telescope()` class since we need to integrate telescope data using the LBT IIF (`python-iif` module).  

A notion for `Instrument()` is to have the next-gen MODS data-taking system "push" dynamic instrument info we need in FITS headers into the IIF DD, then have
azcam "pull" that info using the `get_header()` et al. methods in its `Instrument()` class instance.

