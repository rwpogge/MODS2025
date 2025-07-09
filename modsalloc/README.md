# modsalloc - MODS shared memory sector

**Updated: 2025 July 9 [rwp/osu]**

## Overview

modsalloc allocates a shared memory sector used by the MODS mechanism
controller server (mmcService) and the AGw unit server (agwServer).
Other programs can use shared memory as needed, for example the
IMSC agents and TclTk scripts that run the IMCS tools like the
"radar screen" gui and data readout gui.

## Compile

Compile in place as follows as user "dts":
<pre>
% cd /home/dts/mods/modsalloc
% ./build
</pre>
This puts a copy of the modsalloc executable in /home/dts/mods/bin


## Install the service

We install modsalloc as a system service executed automatically at
boot-time using systemd.  This is the modern way to do this. The
original pre-2025 MODS system used rc.local which is now
deprecated across Linux systems.

Logged in as root (or sudo):
<pre>
% sudo cp modsalloc.service /usr/lib/systemd/system/
</pre>
enable for boot-time execution:
<pre>
% sudo systemctl enable modsalloc
</pre>

this reports
<pre>
Created symlink /etc/systemd/system/multi-user.target.wants/modsalloc.service → /usr/lib/systemd/system/modsalloc.service.
</pre>
Then on reboot the modsalloc service is started automatically.  The first time out (before the first reboot), type
<pre>
% sudo systemctl restart modsalloc
</pre>
To start the first time

### Post-installation checks

Check to see if modsalloc started OK:
<pre>
% systemctl status modsalloc --no-pager

○ modsalloc.service - Runs the MODS data-taking system shared memory allocator (modsalloc)
     Loaded: loaded (/usr/lib/systemd/system/modsalloc.service; enabled; preset: disabled)
     Active: inactive (dead) since Wed 2025-07-09 11:38:05 EDT; 1h 11min ago
   Duration: 5ms
    Process: 218717 ExecStart=/home/dts/mods/bin/modsalloc.sh (code=exited, status=0/SUCCESS)
   Main PID: 218717 (code=exited, status=0/SUCCESS)
        CPU: 5ms

Jul 09 11:38:05 mods2025 systemd[1]: Started Runs the MODS data-taking system shared memory allocator (modsalloc).
Jul 09 11:38:05 mods2025 modsalloc.sh[218718]: shm_get: id=16, size is 151552 bytes
Jul 09 11:38:05 mods2025 modsalloc.sh[218718]: semid_get: key=-1 id=5
Jul 09 11:38:05 mods2025 modsalloc.sh[218718]: semid_get: key=-1 id=5
Jul 09 11:38:05 mods2025 modsalloc.sh[218718]: cls_get: id 0
Jul 09 11:38:05 mods2025 modsalloc.sh[218718]: skd_get: id 0
Jul 09 11:38:05 mods2025 modsalloc.sh[218718]: brk_get: id 0
Jul 09 11:38:05 mods2025 modsalloc.sh[218718]: semid_get: key=-1 id=5
Jul 09 11:38:05 mods2025 systemd[1]: modsalloc.service: Deactivated successfully.
</pre>
The last line is correct, it does not stay running forever, it just needs to run once then deactivate to create th
shared memory sector.
