# `tmux` - terminal multiplexer notes

<b>started: 2025 Sept 25</b>

## Install:
```
sudo dnf -y install `tmux` 
```

## Useful Docs

The tmux(1) helpfile: https://man.openbsd.org/tmux (big, bad, useful)

RedHat beginner's guide to `tmux`: https://www.redhat.com/en/blog/introduction-tmux-linux

A helpful cheatsheet: https://tmuxcheatsheet.com/

Customization: 
 * status bar customization - https://www.baeldung.com/linux/tmux-status-bar-customization

## Development apps

 * `modsTerm` - first cut at a 3-pane `tmux` session for MODS admin

### MODS instrument server apps

These are designed to run on the `mods1` and `mods2` instrument server machines, replacing the old
versions of the same name for the Archon controller MODS.

 * `mods1` and `mods2` - `tmux` implementations of the pre-Archon startup and shutdown scripts, derived from `modsTerm`
 * `modsStatus.pl` - perl script using `Curses` that makes a dynamic status display for a modsN server.  Used by `mods1` and `mods2`

### Archon Server apps

These are designed to run on the `mods1blue` et al. machines for running the Archon controllers.

 * `ccdTerm` - first cut at a MODS Archon server admin panel that will run `azcam` and `modsCCD` on a `modsN<chan>` server.

## `tmux` config files

Collection of `.tmux.conf` files for use with MODS custom tmux sessions

## Misc

The `Tests/` folder has working files from various tests.  Don't execute.

## Screenshots

![mods1 server admin tmux session live test with MODS1 on 2025 Sept 28](mods1_serverAdmin_2025Sept28.jpg)

