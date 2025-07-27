# MODS public runtime configuration files

Last Update: 2025 July 16 [rwp/osu]

This directory contains the runtime configuration files released for MODS
observing operations.  

These should be fully vetted and working versions copied from templates maintined
deeper in the source code tree.  Applications run by users for LBT night-time
observing and engineering operations with MODS will reference these runtime
configuration files.

## Updating instructions

## MODS data-taking system UDP ports

These are the assignments for the MODS2025 system
```
MODS1:
   M1.IE  UDP/IP 192.168.139.130 10700 # mmcServer
   M1.TC  UDP/IP 192.168.139.130 10801 # SX lbttcs
   M1.RC  UPD/IP 192.168.139.131 10402 # red modsCCD agent
   M1.BC  UPD/IP 192.168.139.132 10401 # blue modsCCD agent
   M1.RHE UDP/IP 192.168.139.131 10502 # red modsHEB agent
   M1.BHE UDP/IP 192.168.139.132 10501 # blue modsHEB agent
   M1.ENV UDP/IP 192.168.139.130 10901 # modsEnv agent
   modsUI UDP/IP 192.168.139.131 20800 (base), 20801 (dashboard)
   
MODS2:
   M2.IE  UDP/IP 192.168.139.230 10700
   M2.TC  UDP/IP 192.168.139.230 10801
   M2.RC  UPD/IP 192.168.139.231 10402 # red modsCCD agent
   M2.BC  UPD/IP 192.168.139.232 10401 # blue modsCCD agent
   M2.RHE UDP/IP 192.168.139.242 10502 # red modsHEB agent
   M2.BHE UDP/IP 192.168.139.232 10501 # blue modsHEB agent
   M2.ENV UDP/IP 192.168.139.230 10901
   modsUI UDP/IP 192.168.139.231 20800 (base), 20801 (dashboard)
```
