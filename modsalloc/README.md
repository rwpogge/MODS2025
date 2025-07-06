# modsalloc - MODS shared memory sector

**Updated: 2025 June 23 [rwp/osu]**

## Overview

modsalloc allocates a shared memory sector used by the MODS mechanism
controller server (mmcService) and the AGw unit server (agwServer).
Other programs can use shared memory as needed, for example the
IMSC agents and TclTk scripts that run the IMCS tools like the
"radar screen" gui and data readout gui.

## Compile

Compile in place as follows as user "dts":
<pre>
   cd /home/dts/mods/modsalloc
   ./build
</pre>
This puts a copy of the modsalloc executable in /home/dts/mods/bin


## Install the service

We install modsalloc as a system service executed automatically at
boot-time using systemd.  This is the modern way to do this. The
original pre-2025 MODS system used rc.local which is now
deprecated across Linux systems.

Logged in as root (or sudo):
<pre>
% cp /home/dts/mods/modsalloc/modsalloc.service /usr/local/lib/systemd/system/
</pre>
enable for boot-time execution:
<pre>
% systemctl enable /usr/local/lib/systemd/system/modsalloc.service
</pre>

this reports
<pre>
Created symlink /etc/systemd/system/multi-user.target.wants/modsalloc.service → /usr/local/lib/systemd/system/modsalloc.service.
</pre>
Then on reboot the modsalloc service is started automatically

### Post-installation checks

Logged in as root:
<pre>
% systemctl status modsalloc --no-pager

○ modsalloc.service - Runs the MODS data-taking system shared memory allocator (modsalloc)
     Loaded: loaded (/usr/local/lib/systemd/system/modsalloc.service; bad; preset: disabled)
     Active: inactive (dead) since Tue 2025-06-24 12:30:44 EDT; 3min 36s ago
   Duration: 188ms
    Process: 801 ExecStart=/home/dts/mods/bin/modsalloc.sh (code=exited, status=0/SUCCESS)
   Main PID: 801 (code=exited, status=0/SUCCESS)
        CPU: 15ms

Jun 24 12:30:43 localhost systemd[1]: Started Runs the MODS data-taking system shared memory allocator (modsalloc).
Jun 24 12:30:43 localhost modsalloc.sh[803]: shm_get: id=0, size is 151552 bytes
Jun 24 12:30:43 localhost modsalloc.sh[803]: semid_get: key=-1 id=3
Jun 24 12:30:43 localhost modsalloc.sh[803]: semid_get: key=-1 id=3
Jun 24 12:30:43 localhost modsalloc.sh[803]: cls_get: id 0
Jun 24 12:30:43 localhost modsalloc.sh[803]: skd_get: id 0
Jun 24 12:30:43 localhost modsalloc.sh[803]: brk_get: id 0
Jun 24 12:30:43 localhost modsalloc.sh[803]: semid_get: key=-1 id=3
Jun 24 12:30:44 localhost systemd[1]: modsalloc.service: Deactivated successfully.
</pre>

Logged in as an unprivileged (non-sudo) user like dts or mods:
<pre>
% systemctl status modsalloc --no-pager

○ modsalloc.service - Runs the MODS data-taking system shared memory allocator (modsalloc)
     Loaded: loaded (/usr/local/lib/systemd/system/modsalloc.service; bad; preset: disabled)
     Active: inactive (dead) since Tue 2025-06-24 12:30:44 EDT; 4min 19s ago
   Duration: 188ms
    Process: 801 ExecStart=/home/dts/mods/bin/modsalloc.sh (code=exited, status=0/SUCCESS)
   Main PID: 801 (code=exited, status=0/SUCCESS)
        CPU: 15ms
</pre>
