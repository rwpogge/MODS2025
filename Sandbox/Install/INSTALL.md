# MODS Instrument Control and Data-Taking System Installation

Needs good installation instructions.  This is a stub to build on

## Preparations

### Make sure you have all ancillary packages, compilers, and libraries ready

```shell
sudo dnf -y install tk tcsh emacs doxygen
sudo dnf -y install readline readline-devel
sudo dnf -y install libmodbus libmodbus-devel
sudo dnf -y install qt6-qtbase-devel qt6-qtsvg-devel
sudo dnf -y install dnsutils wget (nice, but not required)
```

The LBTO repository has much we need. This can only be done when on the mountain
and connected to the internal observatory network
```
sudo dnf install -y http://yumsrv.tucson.lbto.org/rpms/almalinux9/release/lbto-yum-repository-1-1.noarch.rpm
```
[Note: it can be done with VPN when not on the mountain, but you have to be physically
logged in because their VPN prohibits remote mode]

Also need the local LBTO versions of hdf5 and lbto-libtelemetry, build from local rpms
or the copies in ~/Libs/ in a properly-configured account on the mods machines.

For ICE, follow the instructions at https://www.zeroc.com/ice/downloads/3.7/cpp
```
sudo dnf -y install https://download.zeroc.com/ice/3.7/el9/ice-repo-3.7.el9.noarch.rpm

sudo dnf -y install libice-c++-devel libice3.7-c++ python3-ice
sudo dnf -y install icebox
```
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

### LBTO conda environment

Currently we are using Anaconda Python 3.12 (latest release is 3.12.11 at this writing), and conda 24.9.2

Install of the standard LBTO py312 environment (as root)
```
conda create -y --prefix /usr/local/conda/envs/py312 python=3.12 astropy numpy scipy matplotlib jupyterlab pandas conda-build requests msgpack-python lz4 pymongo colorama
```
Then, still as root, activate py312
```
conda activate /usr/local/conda/envs/py312
```
Python modules we need to add for MODS:
#### pymodbus
```
pip install pymodbus
```
#### Zero-C ICE and the LBTO IIF
```
conda install conda-forge::zeroc-ice
pip install --trusted-host yumsrv.tucson.lbto.org --extra-index-url http://yumsrv.tucson.lbto.org/pip/repo lbto-iif
```
All done, ready to go.  This has to be done on all MODS machines to make sure we have the same python
environment as the rest of the observatory machines.

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

#### MODS instrument servers (mods1 and mods2):
```shell
cp -r MODS2025/mods .
cp -r MODS2025/ISIS .
cp -r MODS2025/Config .
cp -r MODS2025/modsPerl .
```
Note that we are temporarily overriding the `modsPerl` distribution on the LBTO GitHub repository
while we test and verify updates needed for the Archon controllers.

#### MODS Archon servers (mods1blue, mods1red, mods2blue, mods2red)

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
./build
```
Remember to follow the instructions for implementing the 
shared memory sector (`modsalloc` README)

### MODS Agents

#### MODS instrument servers (mods1 or mods2)

```shell
cd ~/mods/Agents
cd lbttcs
./build
cp lbttcs ~/bin/
cd ../modsEnv
./build
cp modsenv ~/bin/
cd ../modsHEB
./build
cp modsheb ~/bin/ 
```
We do not run `modsCCD` (or azcam) on the main instrument servers.  `modsHEB` is only for
engineering purposes.

#### MODS Archon servers (mods1blue, mods1red, mods2blue, mods2red)

```shell
cd ~/mods/Agents
cd ../modsCCD
./build
cp modsCCD ~/bin/ 
cd ../modsHEB
./build
cp modsheb ~/bin/ 
```
We do not run `modsEnv` or `lbttcs` on Archon servers.  `modsHEB` is only used for engineering
purposes, it is not run during regular observing operations.

### MODS GUIs

#### MODS instrument servers (mods1 and mods2)

Build
 * iTerm
 * imsTool
 * modsUI


#### MODS Archon servers (mods1blue, mods1red, mods2blue, mods2red)

Build 
 * iTerm

#### pre-install preparation 
```shell
cd ~/mods/GUIs
qmake --version
   -> if it says "Using Qt version 5.15.2 in /opt/Anaconda3/lib", type
conda deactivate
qmake --version 
   -> it should say "Using Qt version 6.6.2 in /usr/lib64", proceed, otherwise "conda deactivate" again
```
Remember to do `conda activate` to restore the anaconda environment after building the GUIs for a machine. F

#### iTerm build (all machines)

```
cd iTerm
qmake -o Makefile
make clean
make
cp iterm ~/bin/
```


#### modsUI build (mods1 and mods2)

```
cd ../modsUI
qmake -o Makefile
make clean
make
cp modsUI ~/bin/
```

#### imsTool build (mods1 and mods2)

```shell
cd ../imsTool
qmake -o Makefile
make clean
make
[No copy to ~/bin/!]
```

Note we do not install the imsTool GUI in a public-facing directory as this is for engineering use only.


## Post-Build Installation

### modsalloc systemd service

For the server machines (mods1 and mods2) you need to install and start the `modsalloc` service
to run at boot-time using systemd.

Because the LBTO mountain machines are configured to use SELinux in enforcing mode, we have to take extra installation steps.

After the initial build, go to `/home/dts/mods/malloc` and copy these files to `/usr/local/bin` as root or sudo:
```
% sudo cp modsalloc /usr/local/bin/
% sudo cp modsalloc.sh /usr/local/bin/
% sudo chmod +x /usr/local/bin/modsalloc.sh
```
Then
```
% cd ../modsalloc
% sudo cp modsalloc.service /usr/lib/systemd/system/
```
and enable for boot-time execution:
```
% sudo systemctl enable modsalloc
Created symlink /etc/systemd/system/multi-user.target.wants/modsalloc.service → /usr/lib/systemd/system/modsalloc.service.
```
After the next reboot the `modsalloc` service will be started automatically. 

Before the first reboot, you will need to start the service for the first time:
```
% sudo systemctl restart modsalloc
```

### Other programs

These programs need to be installed in `/usr/local/bin` for use by common data-taking services.  This install step
makes the "release" versions, allowing us to recompile and test new versions without interrupting programs critical
for observing (why we install "by hand" instead of as part of the make process).

```
cd /home/dts/mods/bin
sudo cp vueinfo /usr/local/bin
sudo cp mlcRecover /usr/local/bin
```
(more coming as we converge)

## Data and Telemetry NFS mounts

MODS machines NFS mount two remote observatory drives
 * `/lbt/data/new` - newdata repository
 * `/lbt/data/telemetry` - observatory HDF5 telemetry streams

Two rpms are setup to do this for us, need to be run one-time when a machine is setup
```
sudo dnf install lbto-nfs-mountain-newdata lbto-nfs-mountain-telemetry
```

`modsEnv` writes environmental sensor logs in `/lbt/data/telemetry/instruments/mods#/...` (#=1,2)
