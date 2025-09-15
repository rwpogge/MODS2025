# isisServer systemd service

**Updated: 2025 Sept 12 [rwp/osu]**

## Overview

ISIS is a lightweight message-passing server used by the data-taking system of OSU instruments 
for interprocess communication and coordination. The `isis` program is written in ANSI C and 
communicates via UDP/IP sockets and physical serial ports.

For the MODS Archon update at LBTO we are going to experiment with running the ISIS server as
a systemd service.  It needs to run all the time and this way to operate on the instrument
servers makes sense.  

If `isis` is stopped, either because of errors or because it was explicitly stopped by the system or
by sending the `exec: quit` command via the ISIS server, it will restart automatically in about 10s.
The number of times we've seen an isis server fail in the field in the past 15 years at LBTO can be
counted on one hand and have plenty of fingers left.

## Install the service

Because the LBTO mountain machines are configured to use SELinux in enforcing mode, we have to take extra installation steps.

After the initial build, go to `/home/dts/ISIS` and copy these files to `/usr/local/bin` as root or sudo:
```
% sudo cp isisServer/isis /usr/local/bin/
% sudo cp systemd/isis.sh /usr/local/bin/
```
Then, logged in as root (or sudo):
```
% sudo cp systemd/isis.service /usr/lib/systemd/system/
```
and enable for boot-time execution:
```
% sudo systemctl enable isis
Created symlink /etc/systemd/system/multi-user.target.wants/isis.service → /usr/lib/systemd/system/isis.service.
```
After the next reboot the `isis` service will be started automatically. 

Before the first reboot, you will need to start the service for the first time:
```
% sudo systemctl restart isis
```
See [Testing](#Testing) to verify it is working.  After this it should start
automatically every time the system reboots.

### Post-installation checks

Check to see if isis started OK:
```
% systemctl status isis

...

```

## Testing

...
