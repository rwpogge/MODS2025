# modsPerl - MODS Scripting Engine and Instrument Support Tools

Latest Version: 2025 Sept 10 [rwp/osu]

See [Release Notes](release.md) for the current verison

## Overview

modsPerl is a suite of Perl scripts that implement the MODS command
scripting engines and instrument support tools.

All scripts are designed so that if executed without arguments they
will print a usage message.  This is the same as using the -h (--help)
option.

## Installation

To make the MODS scripting tools "public" for observers, copy the
current executables into /home/modseng/bin/ on one of the obsN
computers at LBTO, stripping off the .pl extension.

Make sure you first make backup copies of the current executables in case 
you need to roll-back a version.  This is operational "flight" software, 
so committing an edited executable to /home/modseng/bin/ should only be 
done after the new version is fully tested.

Note that the main ISIS.pm perl module must be copied into
/home/modseng/modsPerl and never copied into bin (it is not
"executable" per-se).


## Dependences

The MODS scripting engine was developed in standard Perl 5 and uses no special modules.


## Runtime Environment

The MODS scripting engine uses no shell environment variables.

By default, it assumes that the system Perl (/usr/bin/perl) is the correct version to use.

## Documentation

See [www.astronomy.ohio-state.edu/MODS/ObsTools/modsTools/](http://www.astronomy.ohio-state.edu/MODS/ObsTools/modsTools/) for a description of the MODS observing scripting system.

## modsPerl Files

### ISIS.pm

Perl module that implements IMPv2 protocol UDP socket communications
and IMPv2 message handling for all MODS scripts and commands.

### Observing and Acquisition Scripting Tools

#### acqMODS

MODS target acquisition scripting engine.  Executes a MODS target
acquisition (.acq) script, which will point ("preset") the telescope,
setup the guide star, and take acquisition images of the field and (if
required) through the desired slit mask.  It can also apply blind
offsets to place faint targets on a slit.  Acquisition is a distinct
operation from observation, so all science observations made using
MODS will require separate .acq and .obs scripts.  It also
communicates with the MODS GUI backdoor socket interface, so it
requires that the MODS GUI is running.  Errors prompt the user to
abort/retry/ignore the error, and there are various re-entrant
execution modes.

#### execMODS 

MODS observation scripting engine.  It will execute a MODS observation
script file.  Scripts are used by observers to execute observations
(.obs), calibrations (.cal) or instrument procedures (.pro).
Implements a suite of high-level instrument configuration and
observing commands, but excludes target acquisition and telescope
presets, although offset motions are permitted.  The script engine
communicates with the MODS control panel GUI backdoor socket
interface, so it requires that the MODS GUI is running.  MODS
observing scripts are fully re-entrant at the line or block level,
permitting straightforward restart on fatal errors.  In default mode,
errors prompt the user to Abort/Retry/Ignore an error, giving a high
level of runtime control. An unattended option is provided for
after-morning calibration so a script won't stall out on the
abort/retry/ignore prompt after the observer has gone to bed.

#### acqBinoMODS

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

#### modsCmd

Send a single command to the backdoor socket interface of a MODS GUI
control panel, and wait for a reply.  If the -t option is used, it
will wait for a reply or timeout, whichever comes first.  Requires
that the MODS control panel GUI is running.

#### listMODS

Lists a MODS observing (.obs), calibration (.cal), or procedure (.pro)
script, giving line numbers that can be used to run a script
re-entrantly with execMODS.

### Instrument Support Tools

#### isisCmd

Sends a single command to the named ISIS client node or the ISIS
server proper.  The ISIS client address must be a valid and running
ISIS node.  Assumes an intimate acquaintance with the IMPv2 protocol,
and so is only meant to be used by LBTO or OSU support and engineering
personnel for low-level instrument support and testing purposes.

#### execISIS

Executes a list of IMPv2 commands (a superset of isisCmd).  Also a
low-level engineering tool that should only be used by LBTO and OSU
instrument support and engineering personnel. A single line of an isis
"script" looks like the command options of isisCmd.

#### modsMask

Operate the MODS1 or MODS2 slit mask cassette systems when doing mask
exchanges.  Works directly with the instrument ISIS and IE servers, so
no MODS control panel GUI needs to be running.  Only implements that
subset of mask commands needed for safe mask exchanges.

#### modsAGW

Operate the MODS1 or MODS2 AGw stage system. This gives full access to
the functions of the AGw stage, primarily for the Telescope Operators
or persons engaged in AGw/guider engineering observations.  Does not
require a running MODS control panel GUI.

#### modsTemps

Readout the temperatures and pressures of MODS1 or MODS2 dewars and
glycol cooling systems.  Works directly with ISIS and the IE/IC
servers, so no MODS control panel GUI needs to be running.  Intended
for use by instrument support personnel to make instantaneous readings
of critical temperatures and pressures in the system (e.g., after
filling the dewar or checking glycol cooling status).


## Author & History

modsPerl was written by Rick Pogge, The Ohio State University
Department of Astronomy (pogge.1@osu.edu).
