# MODS Instrument Control and Data-Taking System Installation

Needs good installation instructions.  This is a stub to build on

## Preparations

### Make sure you have all ancillary packages, compilers, and libraries ready

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
Also need the local LBTO versions of hdf5 and lbto-libtelemetry, build from local rpms

### Create directories we need

In the dts account, make sure that these directories are all present with the correct permission
```shell
mkdir bin
mkdir Logs
chmod 777 Logs
```
And in /home
```shell
cd /home
sudo mkdir data
chmod 777 data
sudo mkdir Logs
chmod 777 Logs
```

### Get the code repo

Code is installed logged in as user `dts` on the Linux server.
```shell
cd ~/
git clone https://github.com/rwpogge/MODS2025
```
If you already have a MODS2025 repo, then
```
cd ~/MODS2025
git pull
```
to update your local copy.

### Distribute the code

Yes, this is sloppy, but this is also development mode.  We copy the code we need out of MODS2025/ into the
public space.  Once we're past the development phases this will be a much cleaner build, until then...
```shell
cp -r MODS2025/mods .
cp -r MODS2025/ISIS .
cp -r MODS2025/azcam .
cp -r MODS2025/Config .
```

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
qmake --version
   -> if it says "Using Qt version 5.15.2 in /opt/Anaconda3/lib", type
conda deactivate
qmake --version 
   -> it should say "Using Qt version 6.6.2 in /usr/lib64", proceed, otherwise "conda deactivate" again
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




