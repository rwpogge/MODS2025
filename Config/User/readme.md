# MODS runtime config file

Last Update: 2025 Dec 20 [rwp/osu]

This directory contains the bash runtime configuration files for the MODS instrument
and Archon server user accounts (mods and observer)

 * `dot_bashrc.txt` - copy into `.bashrc`.  Sets up the paths for the `mods` and `observer` accounts to access MODS tools 
 * `vnc_config.txt` - copy into `~/.vnc/config` for our VNC config, but only if authorizing VNC for that user
 * `vnc_xstartup.txt` - copy into `~/.vnc/xstartup` for our VNC config, but only if authorizing VNC for that user

## vncserver use

### Startup

```shell
  vncserver
```
to start the `vncserver` by hand.  See `vncserver help` for more options

### Stop
```shell
   vncserver -list

   X DISPLAY #	PROCESS ID
   :1		133444

   vncserver -kill :1

   Killing Xvnc process ID 133444
```

### systemd services

To run vncserver as a systemd service
```
$ systemctl start vncserver@:x
```
Run this as the root user or:
```
$ sudo systemctl start vncserver@:x
```
Don't forget to replace the `:x` by the actual number you configured in
the user mapping file. For example:
```
$ sudo systemctl start vncserver@:1
```
This starts a TigerVNC server for user `test` with GNOME session.

In case you want your server to be automatically started at boot, you
can run:
```
$ sudo systemctl enable vncserver@:1
```
