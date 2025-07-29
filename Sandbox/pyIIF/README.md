# python-iif sandbox

This is code we're working with for teaching azcam-mods how to get
TCS header info it needs directly from the LBT TCS through the
new python-iif interface (https://github.com/LBTO/python-iif).

## Contents

### TCS Interface

 * `modsTCS_left.txt` - Yaml file to map IIF TCS data dictionary entries to FITS header keywords for MODS1 at SX (left) direct Gregorian
 * `modsTCS_right.txt` - Yaml file to map IIF TCS data dictionary entries to FITS header keywords for MODS2 at DX (right) direct Gregorian
 * `telescope_LBT.py` - testing version of a Telescope() class object for azcam-mods
 * `IIF Header Definition YAML Sandbox.ipynb' - Jupyter notebook were I worked out reading yaml files, structuring the dicts and lists for Header() and IIF, etc.  Instructive
 * `getIIF.py` - test program
 
 ### Instrument Interface Concept
 
 * `MODS1B_Inst.txt` - Draft file to map IIF MODS instrument data dictionary entries to FITS header keywords to put instrument info into MODS1B headers.
 * `MODS_New_DDNames.txt` - List of new DD entries needed to implement an instrument header "pull" into azcam using a mapping file like `MODS1B_Inst.txt`.
 * `MODS1B_Inst_draft.xlsx` - Draft excel spreadsheet showing DD to MODS shared memory mapping
 
 ## Notes
 
 azcam integrates telescope and instrument functions into an azcam server using Telescope() and Instrument() class instances built on base classes of the
 same names defined  in the main azcam code.  Buried inside each of these base classes is a "named tool" (my nomenclature) instance of the Header() base class.  It is this header "tool" that is used to build the structured subsystem FITS header keyword sets that go into the FITS headers on raw data.
 
It *is* documented, just took some digging to make the right connections.  See https://azcam.readthedocs.io/classes/ and look at the Header Class summary and its
documentation (https://azcam.readthedocs.io/autocode/azcam_header/).  The `read_header()` method is the key bit, and the essential attributes are 
the `keywords`, `values`, `comments` and `typestrings` dictionaries.  On instantiating the Header() class, you can specify the title ("telescope", "instrument") and a name
of a template file with the static header information (emphasis on the words *static*).

I found that switching to yaml to configure the header information file that is read and turned into the lists and dictionaries needed for Header() et al. was a much
cleaner solution.

This sandbox is various code stubs, configuration files, etc. that will be used to build the Telescope() and Instrument() classes for `azcam-mods`.  First up is the
`Telescope()` class since we need to integrate telescope data using the LBT IIF (`python-iif` module).  

A notion for `Instrument()` is to have the next-gen MODS data-taking system "push" dynamic instrument info we need in FITS headers into the IIF DD, then have
azcam "pull" that info using the `get_header()` et al. methods in its `Instrument()` class instance.

### Timing

Will be interesting to see what this does with a live system instead of the simulator in a real mountain network, but in my alpha version of telecope_LBT.py I found
that if I wrote a function that instantiated a proxy, did the GetParameters() query, then destroyed the proxy, the impact on timing was at the 10s of milliseconds level.
azcam-mods interrogation of the TCS DD for headers is going to be low duty cycle, so we can stand a little setup/tear-down overhead if it means we are more safe
against the TCS going down taking the azcam server with it. 

### Installation Notes

The python-iif code is installed in these steps
```shell
% conda activate /usr/local/conda/envs/py312
% pip install --trusted-host yumsrv.tucson.lbto.org --extra-index-url http://yumsrv.tucson.lbto.org/pip/repo lbto-iif
```
The last step can sometimes take a while as it digs up some of the bits it needs.  

#### missing 
