# MODS2025
Version 1.2.0

Last Update: 2025 July 16 [rwp/osu]

Lead: Richard Pogge (OSU Astronomy) pogge.1@osu.edu

## Overview
MODS2025 Project to update MODS1+2 CCD and instrument control systems at the LBT

**This our very messy working repository, not the final flight code repository that will be built later.  
Proceed at your own risk**

## Contents

  * `mods` - MODS instrument control and data-acquisition system
  * `ISIS` - ISIS message-passing server and client
  * `azcam` - AzCam server for the MODS Archon CCD controllers
  * `Sandbox` - place to put test codes and other bits we are using
  * `Config` - MODS instance runtime configuration files released for observing operations

## Build order

### ISIS client and server
```shell
cd ~/ISIS/isisClient
./build
cd ../isisServer
./build
cp isis isisd ~/bin/
```

### MODS agw and mmc Servers
```shell
cd ~/mods
chmod +x build */build */*/build
./build
```
Remember to follow the instructions for implementing the 
shared memory sector (`modsalloc` README)

### MODS Agents
```shell
cd ~/mods/Agents
chmod +x */build
cd lbttcs
./build
cp lbttcs ~/bin/
cd ../modsEnv
../
```
### MODS GUIs
```shell
cd ~/mods/GUIs
conda deactivate
conda deactivate (twice!)
qmake --version -> verify v6.6.x
cd iTerm
qmake -o Makefile
make clean
make
cp iterm ~/bin/
cd ../imsTool
qmake -o Makefile
make clean
make
[No copy to ~/bin/!]
cd ../modsUI
qmake -o Makefile
make clean
make
cp modsUI ~/bin/
```
Note we do not install the imsTool GUI in a public-facing directory as this is for engineering use only.




## Package dependence

This is a running list during development phase as we discover what is missing from the 
AlmaLinux 9.x systems we're setting up.

```shell
dnf -y install tk tcsh emacs doxygen
dnf -y install readline readline-devel
dnf -y install libmodbus libmodbus-devel
dnf -y install qt6-qtbase-devel qt6-qtsvg-devel
dnf -y install libice3.7-c++ libice-c++-devel python3-ice
...
dnf -y install dnstools wget (nice, but not required)
...
```


## Authors and Collaborators

- Rick Pogge (OSU Astronomy) pogge.1@osu.edu
- Jerry Mason (OSU Astronomy) mason.8@osu.edu
- Xander Carroll (OSU Astronomy) carroll.892@osu.edu
- ...
