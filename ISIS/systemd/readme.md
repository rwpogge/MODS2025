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

● isis.service - ISIS server for a MODS instrument
     Loaded: loaded (/usr/lib/systemd/system/isis.service; disabled; preset: disabled)
     Active: active (running) since Mon 2025-09-15 14:03:22 EDT; 1min 37s ago
   Main PID: 67884 (isis.sh)
      Tasks: 2 (limit: 99329)
     Memory: 780.0K
        CPU: 3ms
     CGroup: /system.slice/isis.service
             ├─67884 /bin/sh /usr/local/bin/isis.sh
             └─67885 /usr/local/bin/isis -d

Sep 15 14:03:22 mods1 systemd[1]: Started ISIS server for a MODS instrument.
```

## Testing

A few things to check that ISIS is running

### Use `isisCmd`

Use the `isisCmd` script to query the status of the ISIS server.  For the
ISIS server running on `mods1`, try
```
% isisCmd --mods1 is status
DONE: STATUS HostName=IS HostAddr=mods1:6600 rcFile=/home/dts/Config/isis.ini logFile=/home/Logs/ISIS/isis.20250915.log numClients=2 Concise +LOG +HANDSHAKE -CLI
```
This correctly reports info on the current ISIS server.

You should also be able to communicate with ISIS clients through the server
```
% ./isisCmd --mods1 is hosts
DONE: HOST numHosts=2 maxHosts=32 host0=M1.ENV host1=IPC1

% ./isisCmd --mods1 m1.env info
DONE: info HostID=M1.ENV HostAddr=mods1:10901 Mode=ISISClient ISIS=IS ISISHost=192.168.139.130:6600 InstID=MODS1 Cadence=60 IUBAddr=192.168.139.122 IEBBAddr=192.168.139.110 IEBRAddr=192.168.139.100 HEBBAddr=192.168.139.142 HEBRAddr=192.168.139.141 B_IGAddr=192.168.139.113 R_IGAddr=192.168.139.103 runState=Active Logging=Enabled logFile=/home/Logs/Env/mods1.20250915.log HdfLogging=Enabled HdfLogDir=/lbt/data/telemetry/instruments/ Verbose -DEBUG rcfile=/home/dts/Config/modsenv.ini
```
Here we looked at what clients are attached to ISIS, see `M1.ENV`, then ask `M1.ENV` for its configuration via the ISIS server.

