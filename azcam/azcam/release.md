# azcam release notes

Updated: 2025 Dec 3

MODS azcam Notes

## 2025 Dec 3 [ml/itl]

Mike Lesser changed `azcam/utils.py` to remove `check_keyboard` which is unsupported on Linux.

## 2025 Aug 18 [rwp/osu]

Minor changes after return to OSU, continued lab testing and tweaking based on notes from the LBTO run.

## 2025 Aug 1 [rwp/osu]

Extensive testing with the live Archon + lab test dewar in the LBTO Tucson lab.  Still have not needed to modify the core code beyond the issue noted below. 

## 2025 July 25 [rwp/osu]

Made first change in core code, fixed bug in ``azcam/azcam/header.py``, the `read_file()` method used <code>break</code> in the header template read loop if a # comment or blank line instead of <code>continue</code>, so the template read was truncated at the first comment or blank line. 

