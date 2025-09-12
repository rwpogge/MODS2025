# modsEnv - systemd service

**Updated: 2025 Sept 12 [rwp/osu]**

## Overview

`modsEnv` is a MODS data-taking system agent that monitors the environmental sensors in a MODS instrument.  It is
an ISIS client node written in C/C++.  Environmental data read by `modsEnv` is stored in 2 locations:
 * LBTO telemetry file in hdf5 format in `/lbt/data/telemetry/instruments`, in the `mods1` or `mods2` subfolders
 * In the MODS shared memory segment on the instrument server (`mods1` or `mods2`)

We run it as a systemd service so that it is always started on a system, automatically restarted if it fails, and
automatically started at boot time.  This ensures we are always collecting environmental sensor data telemetry
when a MODS is active.

## Install the service

Because the LBTO mountain machines are configured to use SELinux in enforcing mode, we have to take extra installation steps.

After the initial build, go to `/home/dts/mods/Agents/modsEnv` and copy these files to `/usr/local/bin` as root or sudo:
```
% sudo cp modsenv /usr/local/bin/
% sudo cp systemd/modsenv.sh /usr/local/bin/
% sudo chmod +x /usr/local/bin/modsenv.sh
```
Then, logged in as root (or sudo):
```
% sudo cp systemd/modsenv.service /usr/lib/systemd/system/
```
and enable for boot-time execution:
```
% sudo systemctl enable modsenv
Created symlink /etc/systemd/system/multi-user.target.wants/modsenv.service → /usr/lib/systemd/system/modsenv.service.
```
After the next reboot the `modsenv` service will be started automatically. 

Before the first reboot, you will need to start the service for the first time:
```
% sudo systemctl restart modsenv
```
See [Testing](#Testing) to verify it is working.  After this it should start
automatically every time the system reboots.

### Post-installation checks

Check to see if modsenv started OK:
```
% systemctl status modsenv

```

## Testing

`vueinfo` is a custom program we use to interrogate the MODS shared memory segment.  

If the shared memory segment is active, you'll see the last read of environmental data:
```shell
% vueinfo env
Environmental Sensors:
  Glycol Supply: P=100.01 psi-g  T=13.0 C
         Return: P=18.22 psi-g  T=15.0 C
  IUB Inside Air T=16.0 C   Ambient T=12.7 C   HeatSink T=15.8 C
  Blue IEB Inside Air T=16.1 C  Glycol Return T=13.1 C
   Red IEB Inside Air T=16.0 C  Glycol Return T=13.0 C
  MODS Air Top T=12.3 C   Bottom T=11.6C
       Truss Top T=12.0 C   Bottom T=12.3C
  Blue HEB Inside Air T=18.8 C  Dewar T=11.8 C P=0.00e+00 torr
   Red HEB Inside Air T=17.9 C  Dewar T=11.2 C P=0.00e+00 torr
```
You can also look at the running telemetry file.  For example,
to see what modsenv is doing on mods2:
```
% ls -l /lbt/data/telemetry/instruments/mods2/
total 4
drwxrwxr-x. 1 root  4 Sep 12 14:21 2025/
lrwxrwxrwx. 1 root 74 Sep 12 14:21 current.mods2.env.h5 -> /lbt/data/telemetry/instruments/mods2/2025/09/12/202509121821.mods2.env.h5
```
The date on the `current.mods2.env.h5` file, and the symlink at right should be for the current date if it is running.
