# modsEnv systemd service

**Updated: 2025 Sept 12 [rwp/osu]**

## Overview

`modsEnv` is a MODS data-taking system agent that monitors the environmental sensors in a MODS instrument.  It is
an ISIS client node written in C/C++.  Environmental data read by `modsEnv` is stored in 2 locations:
 * LBTO telemetry file in hdf5 format in `/lbt/data/telemetry/instruments`, in the `mods1` or `mods2` subfolders
 * In the MODS shared memory segment on the instrument server (`mods1` or `mods2`)

We run it as a systemd service so that it is always started on a system, automatically restarted if it fails, and
automatically started at boot time.  This ensures we are always collecting environmental sensor data telemetry
when a MODS is active.

If `modsenv` is stopped, either because of errors or because it was explicitly stopped by the system or
by sending the `exec: quit` command via the ISIS server, it will restart automatically in about 30s.  This
is half the default sensor measurement cadence (60s).

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

● modsenv.service - MODS environmental sensor monitoring agent
     Loaded: loaded (/usr/lib/systemd/system/modsenv.service; enabled; preset: disabled)
     Active: active (running) since Fri 2025-09-12 15:02:37 EDT; 5s ago
   Main PID: 13755 (modsenv.sh)
      Tasks: 3 (limit: 99329)
     Memory: 13.5M
        CPU: 9ms
     CGroup: /system.slice/modsenv.service
             ├─13755 /bin/sh /usr/local/bin/modsenv.sh
             └─13757 /usr/local/bin/modsenv

Sep 12 15:02:37 mods1 systemd[1]: Started MODS environmental sensor monitoring agent.
Sep 12 15:02:37 mods1 modsenv.sh[13757]: Ignoring unrecognized config file entry - UseHdf5
Sep 12 15:02:37 mods1 modsenv.sh[13757]: Started modsenv as ISIS client node M1.ENV on mods1 port 10901
Sep 12 15:02:38 mods1 modsenv.sh[13757]: Telemetry Notice: The leap-seconds.list file is expired. Will check again in an h…nds file.
Sep 12 15:02:38 mods1 modsenv.sh[13757]: Telemetry Notice: HDF5 library version 1.12.1
Sep 12 15:02:38 mods1 modsenv[13757]: env stream consumer thread id 13762 starting
Hint: Some lines were ellipsized, use -l to show in full.

```
This shows what you'd see for starting the `modsenv` service on `mods1`.  It has successfully started the ISIS client
and the HDF data stream.

## Testing

`vueinfo` is a custom program we use to interrogate the MODS shared memory segment.  

If the shared memory segment is active, you'll see the last read of environmental data:
```shell
$ vueinfo env
Environmental Sensors:
  Glycol Supply: P=0.01 psi-g  T=13.1 C
         Return: P=34.65 psi-g  T=13.0 C
  IUB Inside Air T=13.3 C   Ambient T=12.4 C   HeatSink T=13.5 C
  Blue IEB Inside Air T=13.5 C  Glycol Return T=13.3 C
   Red IEB Inside Air T=17.1 C  Glycol Return T=13.3 C
  MODS Air Top T=11.8 C   Bottom T=12.0C
       Truss Top T=12.3 C   Bottom T=12.4C
  Blue HEB Inside Air T=19.7 C  Dewar T=11.9 C P=4.64e-02 torr
   Red HEB Inside Air T=19.1 C  Dewar T=12.0 C P=1.96e-01 torr
```
You can also look at the running telemetry file.  For example,
to see what modsenv is doing on mods2:
```
% ls -l /lbt/data/telemetry/instruments/mods1/
total 4
drwxrwxr-x. 1 root  8 Sep  4 14:10 2025/
lrwxrwxrwx. 1 root 74 Sep 12 15:02 current.mods1.env.h5 -> /lbt/data/telemetry/instruments/mods1/2025/09/12/202509121902.mods1.env.h5
```
The date on the `current.mods1.env.h5` file, and the symlink at right should be for the current date if it is running. This
check was run shortly after the `vueinfo env` test above.

## Updating

If changes need to be made to the `modsenv.service` configuration file, here is how to restart the service
with the new configuration
```shell
sudo systemctl stop modsenv
sudo systemctl disable modsenv
sudo cp modsenv.service /usr/lib/systemd/system/
sudo systemctl daemon-reload
sudo systemctl enable modsenv
sudo systemctl restart modsenv
```