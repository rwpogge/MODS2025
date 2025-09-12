# modsalloc - MODS shared memory segment

**Updated: 2025 August 7 [rwp/osu]**

## Overview

`modsalloc` allocates a shared memory segment used by the MODS mechanism controller server (mmcService) and the AGw unit server (agwServer).
Other programs can use shared memory as needed, for example the IMSC agents and Tcl/Tk scripts that run the IMCS tools like the
"radar screen" gui and data readout gui. 

## Compile

Compile in place as follows as user "dts":
```
% cd /home/dts/mods/modsalloc
% ./build
```
This puts a copy of the modsalloc executable in /home/dts/mods/bin

## Install the service

We install modsalloc as a system service executed automatically at boot-time using systemd.  This is the modern way to do this. The
original pre-2025 MODS system used rc.local which is now deprecated across Linux systems.

Because the LBTO mountain machines are configured to use SELinux in enforcing mode, we have to take extra installation steps.

After the initial build, go to `/home/dts/mods/malloc` and copy these files to `/usr/local/bin` as root or sudo:
```
% sudo cp modsalloc /usr/local/bin/
% sudo cp modsalloc.sh /usr/local/bin/
% sudo chmod +x /usr/local/bin/modsalloc.sh
```
Then, logged in as root (or sudo):
```
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
See [Testing](#Testing) to verify it is working.  After this it should start
automatically every time the system reboots.

### Post-installation checks

Check to see if modsalloc started OK:
```
% systemctl status modsalloc

○ modsalloc.service - Runs the MODS data-taking system shared memory allocator (modsalloc)
     Loaded: loaded (/usr/lib/systemd/system/modsalloc.service; disabled; preset: disabled)
     Active: inactive (dead)

Jul 17 15:14:26 mods1blue systemd[1]: modsalloc.service: Failed with result 'exit-code'.
Jul 17 15:14:39 mods1blue systemd[1]: Started Runs the MODS data-taking system shared memory allocator (modsalloc).
Jul 17 15:14:39 mods1blue modsalloc.sh[1222496]: shm_get: id=2, size is 151552 bytes
Jul 17 15:14:39 mods1blue modsalloc.sh[1222496]: semid_get: key=-1 id=0
Jul 17 15:14:39 mods1blue modsalloc.sh[1222496]: semid_get: key=-1 id=0
Jul 17 15:14:39 mods1blue modsalloc.sh[1222496]: cls_get: id 0
Jul 17 15:14:39 mods1blue modsalloc.sh[1222496]: skd_get: id 0
Jul 17 15:14:39 mods1blue modsalloc.sh[1222496]: brk_get: id 0
Jul 17 15:14:39 mods1blue modsalloc.sh[1222496]: semid_get: key=-1 id=0
Jul 17 15:14:39 mods1blue systemd[1]: modsalloc.service: Deactivated successfully.
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
