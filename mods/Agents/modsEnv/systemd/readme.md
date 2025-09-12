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

○ modsenv.service - Runs the MODS data-taking system shared memory allocator (modsenv)
     Loaded: loaded (/usr/lib/systemd/system/modsenv.service; disabled; preset: disabled)
     Active: inactive (dead)

Jul 17 15:14:26 mods1blue systemd[1]: modsenv.service: Failed with result 'exit-code'.
Jul 17 15:14:39 mods1blue systemd[1]: Started Runs the MODS data-taking system shared memory allocator (modsenv).
Jul 17 15:14:39 mods1blue modsenv.sh[1222496]: shm_get: id=2, size is 151552 bytes
Jul 17 15:14:39 mods1blue modsenv.sh[1222496]: semid_get: key=-1 id=0
Jul 17 15:14:39 mods1blue modsenv.sh[1222496]: semid_get: key=-1 id=0
Jul 17 15:14:39 mods1blue modsenv.sh[1222496]: cls_get: id 0
Jul 17 15:14:39 mods1blue modsenv.sh[1222496]: skd_get: id 0
Jul 17 15:14:39 mods1blue modsenv.sh[1222496]: brk_get: id 0
Jul 17 15:14:39 mods1blue modsenv.sh[1222496]: semid_get: key=-1 id=0
Jul 17 15:14:39 mods1blue systemd[1]: modsenv.service: Deactivated successfully.
```
The last line is correct, it does not stay running forever, it just needs to run once then deactivate to create the
shared memory segment.

## Testing

`vueinfo` is a custom program we use to interrogate the MODS shared memory segment.  

If the shared memory segment is active, you'll see the IP address map for the data-taking system elements, like this
```shell
% vueinfo ipmap
IEB1 [ML1]hatch,  192.168.139.201:8001,  BUSY[0],  HOST[0])
IEB1 [ML2]dichroic,  192.168.139.201:8002,  BUSY[0],  HOST[0])
IEB1 [ML3]rcolttfa,  192.168.139.201:8003,  BUSY[0],  HOST[0])
...
```
If the data-taking system has not been initialized since the last reboot, then you'll see this:
```shell
% vueinfo ipmap
IEB0 [ML1],  ,  BUSY[0],  HOST[0])
IEB0 [ML2],  ,  BUSY[0],  HOST[0])
IEB0 [ML3],  ,  BUSY[0],  HOST[0])
...
```
This is what an initialized but empty shared memory segment will show. Once you've started the `mmcService` (the MODS "IE")
you'll see something more substantial.

If, however, shared memory is **not** initialized, you'll get this error message:
```shell
shm_att: translating key failed: No such file or directory
```
