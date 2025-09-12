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

Coming soon...

