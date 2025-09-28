# `tmux` - terminal multiplexer notes

<b>started: 2025 Sept 25</b>

## Install:
```
sudo dnf -y install `tmux` 
```

## Useful Docs

RedHat beginner's guide to `tmux`: https://www.redhat.com/en/blog/introduction-`tmux`-linux

A cheatsheet: https://`tmux`cheatsheet.com/

Customization: https://www.baeldung.com/linux/`tmux`-status-bar-customization

## Development apps

 * `modsTerm` - first cut at a 3-pane `tmux` session for MODS admin

### MODS instrument server apps

These are designed to run on mods1 and mods2

 * `mods1` and `mods2` - `tmux` implementations of the pre-Archon startup and shutdown scripts, derived from `modsTerm`
 * `modsStatus.pl` - perl script using `Curses` that makes a dynamic status display for a modsN server.  Used by `mods1` and `mods2`

### Archon Server apps

These are designed to run on the `mods1blue` et al. machines for running the Archon controllers.

 * `ccdTerm` - first cut at a MODS Archon server admin panel that will run `azcam` and `modsCCD` on a `modsN<chan>` server.

## `tmux` config files

Collection of `.tmux.conf` files for use with MODS custom tmux sessions

## Misc

The `Tests/` folder has working files from various tests.  Don't execute.
