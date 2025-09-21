# modsDD systemd service

**Updated: 2025 Sept 20 [rwp/osu]**

## Overview

`modsDD` is a MODS data-taking system agent writes a relevant subset
of MODS instrument state and environmental sensor data into the LBT
Data Dictionary (DD).  It is a standalone (non-ISIS) agent written in
C/C++.  It links to the IIF ZeroC Ice libaries like `lbttcs`.  We
purposely separate the function of `modsDD` (updating the DD for MODS)
form `lbttcs` so that MODS status reporting to the DD runs
independently of observing operations.

We run it as a systemd service so that it is always started on a
system, automatically restarted if it fails, and automatically started
at boot time.  This ensures we are always pushing current MODS instrument
state information into the observatory DD.  This exposes MODS state
information to observatory programs like dashboards or alarm-state
monitors without those third-party systems having to interrogate
the data-taking systems directly, with subsequent risks that their
asynchronous queries could interrupt real-time instrument data-taking
or control operations.

If `modsDD` is stopped, either because of errors or because the IIF
server goes down and raises fatal exceptions on live client proxies,
it will restart automatically after a brief delay (usually 10s).


## Install the service

Because the LBTO mountain machines are configured to use SELinux in
enforcing mode, we have to take extra installation steps.

After the initial build, go to `/home/dts/mods/Agents/modsDD` and copy these files to `/usr/local/bin` as root or sudo:
```
% sudo cp modsDD /usr/local/bin/
% sudo cp systemd/modsDD.sh /usr/local/bin/
```
Then, logged in as root (or sudo):
```
% sudo cp systemd/modsDD.service /usr/lib/systemd/system/
```
and enable for boot-time execution:
```
% sudo systemctl enable modsDD
Created symlink /etc/systemd/system/multi-user.target.wants/modsDD.service → /usr/lib/systemd/system/modsDD.service.
```
After the next reboot the `modsDD` service will be started automatically. 

Before the first reboot, you will need to start the service for the first time:
```
% sudo systemctl restart modsDD
```
See [Testing](#Testing) to verify it is working.  After this it should start
automatically every time the system reboots.

### Post-installation checks

Check to see if modsDD started OK:
```
% systemctl status modsDD

...

```
This shows what you'd see for starting the `modsDD` service on `mods1`.

## Testing

check contents of the MODS DD using commands to an active lbttcs agent, or
using the DMS?
