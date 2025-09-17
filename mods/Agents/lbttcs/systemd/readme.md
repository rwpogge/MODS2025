# lbttcs systemd service

**Updated: 2025 Sept 17 [rwp/osu]**

## Overview

`lbttcs` is an ISIS-client application that provides an IMPv2 protocol
agent that interfaces the MODS data-taking system apps with the LBT
Telescope Control System's (TCS) Instrument Interface (IIF).

The LBT TCS uses ZeroC Ice for interprocess communication via their
IIF (Instrument InterFace) application.  IIF is a subsystem of the TCS
system that combines all of the subsystems needed to operate the LBT
mount and enclosure.

If the IIF Ice proxy terminates, it will take down the `lbttcs` client
with it. We are going to us the restart capability of `systemd` to
ensure that the `lbttcs` client restarts (or tries to) within a
certain interval (10 seconds) if it fails to start.

## Install the service

Because the LBTO mountain machines are configured to use SELinux in enforcing mode, we have to take extra installation steps.

After the initial build, go to `/home/dts/mods/Agents/lbttcs` and copy these files to `/usr/local/bin` as root or sudo:
```
% sudo cp lbttcs /usr/local/bin/
% sudo cp systemd/lbttcs.sh /usr/local/bin/
```
Then, logged in as root (or sudo):
```
% sudo cp systemd/lbttcs.service /usr/lib/systemd/system/
```
and enable for boot-time execution:
```
% sudo systemctl enable lbttcs
Created symlink /etc/systemd/system/multi-user.target.wants/lbttcs.service → /usr/lib/systemd/system/lbttcs.service.
```
After the next reboot the `lbttcs` service will be started automatically. 

Before the first reboot, you will need to start the service for the first time:
```
% sudo systemctl restart lbttcs
```
See [Testing](#Testing) to verify it is working.  After this it should start
automatically every time the system reboots.

### Post-installation checks

Check to see if lbttcs started OK:
```
% systemctl status lbttcs

```

## Testing

A few things to check that the `lbttcs` agent is running

### Use `isisCmd`

Use the `isisCmd` script to query the lbttcs through ISIS.  For MODS1, this
command would be:
```
% isisCmd --mods1 m1.tc status

```
This correctly reports info on the current `lbttcs` agent instance.

You should also see the `lbttcs` info in the ISIS host table
```
% ./isisCmd --mods1 is hosts

```

