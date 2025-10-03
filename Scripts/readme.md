# MODS operations scripts

Latest Version: 2025 Oct 2 [rwp/osu]

See [Release Notes](releases.md) for the current verison

## Overview

...

All scripts are designed so that if executed without arguments they
will print a usage message.  This is the same as using the -h (--help)
option.

## Installation

...


## Dependences

Requires `tmux` be installed, and the `.tmux.conf.xyz` dot files (in [dotFiles](dotFiles/) folder be
installed in the login `$HOME` directory of the user.


## Runtime Environment

The MODS operations scripts to not require custom shell environment variables.

By default, it assumes that the system Perl (/usr/bin/perl) is the correct version to use.

## Script overview and usage

coming soon...

### `mods1` and `mods2` server admin

### modsTerm

#### acqBinoMODS`

Wrapper shell script to execute a binocular acquisition.  It takes one
or two .acq scripts on the command liney and executes acqMODS with the
correct instrument and binocular context options.  If one .acq script
is given, it executed on both MODS ("twinning").  If two .acq scripts
are given, they are implicitly executed in the order MODS1 MODS2.

#### execBinoMODS

Wrapper shell script to execute a binocular observation.  It takes one
or two .obs scripts or 1 or 2 .img scripts on the command line and
executes execMODS with the correct instrument and binocular context
options.  Note that you cannot mix .obs and .acq scripts with
execBinoMODS.  If one .acq script is given, it executed on both MODS
("twinning").  If two .acq scripts are given, they are implicitly
executed in the order MODS1 MODS2.

