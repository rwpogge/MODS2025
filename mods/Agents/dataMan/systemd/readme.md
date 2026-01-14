# dataMan systemd service

**Updated: 2025 Dec 31 [rwp/osu]**

## Overview

`dataMan` is a raw CCD image post-processor and archiver server running on one of the MODS
instrument servers (**mods1** or **mods2**).  After an exposure is finished, the name of the 
raw FITS file is passed by the `azcam-mods` server via UDP datagram to the `dataMan`
where the image is conditioned (headers fixed, overscan-bias subtracted, trimmed, and merge
image created and inserted), then a copy of the conditioned image is stored on the instrument
server's data disk and a copy is dispatched to the LBTO /newdata/ staging disk where it is
exposed to the observers and ingested by the observatory data archive.

If `dataMan` is stopped, either because of errors or because it was explicitly stopped by the system or
by sending the `quit` command, it will restart automatically in about 10s.

### IMPORTANT!

**Do not install or run the dataMan systemd service on the archon servers (mods1blue, et al.)  It only runs on the instrument servers mods1 and mods2**

## Install the service

Because the LBTO mountain machines are configured to use SELinux in enforcing mode, we have to take extra installation steps.

### Configuration Files

First copy the `dataMan` runtime configuration files to the server configuration directory:
```
cd /home/dts/Config
cp /home/dts/mods/Agents/dataMan/Config/dataman_MODS1.ini MODS1/
cp /home/dts/mods/Agents/dataMan/Config/dataman_MODS2.ini MODS2/
```
and then create a symbolic link for the server instance (mods1 or mods2).

#### MODS1 dataMan service
```
cd /home/dts/Config
ln -s MODS1/dataman_MODS1.ini dataman.ini
```

#### MODS2 dataMan service
```
cd /home/dts/Config
ln -s MODS2/dataman_MODS2.ini dataman.ini
```
This must be done **before** installing and starting the systemd service.

### systemd install

After installing the runtime configuration files, go to `/home/dts/mods/Agents/dataMan` and copy these 
files to `/usr/local/bin` as root or sudo:
```
cd /home/dts/mods/Agents/dataMan
sudo cp dataMan.py /usr/local/bin/dataMan
sudo cp systemd/dataMan.sh /usr/local/bin/
sudo chmod +x /usr/local/bin/dataMan.sh
```
Then, logged in as root (or sudo):
```
sudo cp systemd/dataMan.service /usr/lib/systemd/system/
```
and enable for boot-time execution:
```
sudo systemctl enable dataMan
Created symlink /etc/systemd/system/multi-user.target.wants/dataMan.service → /usr/lib/systemd/system/dataMan.service.
```
After the next reboot the `dataMan` service will be started automatically. 

Before the first reboot, you will need to start the service for the first time:
```
sudo systemctl restart dataMan
```
After this it should start automatically every time the system reboots, and it will automatically
restart after 10s if it crashes.

### Post-installation checks

Check to see if dataMan started OK:
```
% systemctl status dataMan
● dataMan.service - dataMan server for a MODS instrument
     Loaded: loaded (/usr/lib/systemd/system/dataMan.service; enabled; preset: disabled)
     Active: active (running) since Thu 2026-01-01 01:10:48 GMT; 6s ago
   Main PID: 1234067 (dataMan.sh)
      Tasks: 2 (limit: 99336)
     Memory: 111.7M
        CPU: 706ms
     CGroup: /system.slice/dataMan.service
             ├─1234067 /bin/sh /usr/local/bin/dataMan.sh
             └─1234068 /usr/local/conda/envs/py312/bin/python /usr/local/bin/dataMan

Jan 01 01:10:48 mods2 systemd[1]: Started dataMan server for a MODS instrument.
```
