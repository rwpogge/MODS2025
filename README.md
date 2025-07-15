# MODS2025
Version 1.1.2

Lead: Richard Pogge (OSU Astronomy) pogge.1@osu.edu

## Overview
MODS2025 Project to update MODS1+2 CCD and instrument control systems at the LBT

**This our very messy working repository, not the final flight code repository that will be built later.  
Proceed at your own risk**

## Package dependence

This is a running list during development of things we need

```shell
dnf -y install tk tcsh emacs doxygen
dnf -y install readline readline-devel
dnf -y install libmodbus libmodbus-devel
dnf -y install qt6-qtbase-devel qt6-qtsvg-devel
dnf -y install libice3.7-c++ libice-c++-devel python3-ice
...
dnf -y install dnstools  (nice, but not required)
...
```
## Collaborators
- Jerry Mason (OSU Astronomy) mason.8@osu.edu
- Xander Carroll
