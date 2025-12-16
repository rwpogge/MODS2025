# MODS azcam server

Updated: 2025 Dec 16

## Overview

The MODS azcam server is based on Mike Lesser's `azcam` (https://github.com/mplesser/azcam) and `azcam-mods` 
(https://github.com/mplesser/azcam-mods), but substantially modified, so do not use the original versions
from Mike's GitHub repository for MODS.

While we are leaving the core `azcam` code mostly unmodified (see the release notes), `azcam-mods` is the 
focus of our development, and will not work on the MODS machines without our version of the core `azcam` code, 
which is why we bundle them together.

During engineering work by Mike to get the MODS flight CCDs to work with the Archon controllers and tune
up the Archon programming, we also installed a version of `azcam-console` which carries most of Mike's 
tools for characterizing CCD performance. This was the first Linux version of that, so will be our version
moving forward.

See the release notes:
 * `azcam` [Release Notes](azcam/releases.md)
 * `azcam-mods` [Release Notes](azcam-mods/releases.md)
 * `azcam-console` [Release Notes](azcam-console/releases.md) 

for details.

Work in progress, stay tuned...
