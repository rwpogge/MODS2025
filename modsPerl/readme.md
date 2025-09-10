# modsPerl - MODS Scripting Engine and Instrument Support Tools

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


## Release Notes

<dl>
<dt>2024 Jun 13: offset and offsetxy timeout
<dd>Increased the timeout for offset and offsetxy to match that for syncoffset
to address issue that emerged with syncoffset followed by offset or offsetxy
would timeout prematurely because of incommensurate timeouts.  Changes were
to acqMODS.pl and execMODS.pl
 
<dt>2021 Mar 16: Ctrl+C not aborting exposures in acqMODS
<dd>acqMODS was not aborting on Ctrl+C, it would ask the user to
confirm the abort, but the abort was not being executed.  Problem traced to
two bugs, an upcase (uc) that should have been lowercase (lc) before 
string comparison, and then comparing with the wrong strings when servicing
exposure in progress aborts. Double fault!
  
<dt>2019 Jul 1: syncoffset timeout
<dd>Increased the synchoffset timeout to address an issue with large syncoffsets
that move the telescope outside the co-pointing limits and the usual short offset
inside the co-pointing patch is too short and we don't give the large syncoffset
enough time to complete before raising a timeout error.  We set the syncoffset
timeout to the same long (300s) timeout used for presets, since a big syncoffset
could in some cases take about as long as a preset.  Addresses an issue raised
in IT#6939.
  
<dt>2018 Dec 7: Binocular Patch 3
<dd>A bug in Patch 2 when synchoffset is run in an imaging
script with execMODS.
  
<dt>2018 Nov 14: Binocular Patch 2
<dd>Fixed broken patch applied on Oct 19 (users found an unexpected
way through the logic).

<dt>2018 Oct 19: Binocular Patch
<dd>Modified acqMODS and execMODS to explicitly ignore the SYNCOFFSET
command if not a binocular observation. Was originally supposed to be
benign if a binocular directive sent in single-eye mode, but that is
not actually true in practice.

<dt>2016 Oct 17: Binocular Release
<dd>Minor tweaks in testing.  In particular, verified that execMODS
properly sends a syncpreset directive when imaging (.img) scripts are
executed in binocular context.  Executing .img scripts in binocular
context is only permitted in unmixed ("twining") mode when either one
.img script is executed by both MODS ("identical twinning") or two
.img scripts are executed by each MODS ("fraternal twinning").

<dd>Note: In the future if a mixed imaging and spectroscopic observation
is being executed in "sibling mode" with MODS1 and MODS2, the imaging
script needs to be split into separate .acq and .obs scripts.  This is
because of the need to synchronize acquisitions for imaging (nearly
trivial) and spectroscopy.

<dt>2016 Aug 01: Binocular Release Version (v2.3.x)
<dd>Release version for testing prior to the start of Semester 2016B.
Implements modifications based on shared-risk binocular operations
during 2016A.  Includes better handling of exits in binocular mode,
and implements binocular operations hooks in acqMODS and execMODS.

<dt>2016 Jun 19: Binocular Shared-Risk Version (v2.2.x)
<dd>Shared-risk partner use started in May 2016 using v2.1.0.  Based
on that experience, made changes to work better in partner observing.
v2.2 incorporates better trapping of fatal errors that caused the
child xterm window to close, taking error messages with it.  Replaced
all instances of the standard Perl "exit" command with "binoExit"
which checks the binoACQ flag, and if being run in binocular context,
will hold the screen at the fatal (abort and return to OS) error
message with a prompt to hit "Enter" to exit.

<dt>2016 Jan 05: Binocular Commissioning Version (v2.1.x)

<dd>New programs:
<dd>acqMODS_bino - Binocular test version of acqMODS
<dd>acqBinoMODS - simple wrapper script to run 2 instances of
acqMODS_bino in separate xterms for Binocular MODS operation testing

<dd>acqMODS_bino adds the --bino/-b flag to run in binocular context.
Absent this flag it assumes monocular context for the given MODS
instance.  $useMODS is no 0, so there is no assumed default MODS1 or
MODS2.

<dt>2014 Apr 18: Version 2.0BETA

<dd>Hooks for binocular MODS1+2 operation.  Provide a single set of
script engines and support program for operating two MODS at the
telescope.  We follow the current LBT Binocular operations model in
which each instrument is operated independently.

<dd>Added keywords to now direct command execution to a particular MODS instance:
<dd>--mods1 = direct to MODS1
<dd>--mods2 = direct to MODS2

<dd>Internally, the useMODS parameter sets the default MODS instance,
thus setting $useMODS=1 makes MODS1 default and no --mods1 is needed.
For binocular operation $useMODS=0 means no default, so --mods1 and
--mods2 are required.

</dl>

## Author & History

modsPerl was written by Rick Pogge, The Ohio State University
Department of Astronomy (pogge.1@osu.edu).
