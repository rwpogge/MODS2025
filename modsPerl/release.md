# modsPerl Release Notes

Last Update: 2025 Sept 10 [rwp/osu]


## 2025 Sep 10: Archon controller update

Modifications to modsPerl and associated scripts for the Archon CCD
controller update during summer shutdown 2025.

Changes:
 * Server IP addresses for new system (.130 and .230, for mods1 and mods2)
 * `modsPerl` path must be `/home/dts/modsPerl/` or `/home/modseng/modsPerl`

Advanced versions as follows:
 * `acqMODS.pl` and `execMODS.pl` - 2.5.0-bino
 * `execISIS`, `isisCmd`, `modsCmd` - 2.2.0


## 2024 Jun 13: offset and offsetxy timeout

Increased the timeout for offset and offsetxy to match that for syncoffset
to address issue that emerged with syncoffset followed by offset or offsetxy
would timeout prematurely because of incommensurate timeouts.  Changes were
to acqMODS.pl and execMODS.pl
 
## 2021 Mar 16: Ctrl+C not aborting exposures in acqMODS

acqMODS was not aborting on Ctrl+C, it would ask the user to
confirm the abort, but the abort was not being executed.  Problem traced to
two bugs, an upcase (uc) that should have been lowercase (lc) before 
string comparison, and then comparing with the wrong strings when servicing
exposure in progress aborts. Double fault!
  
## 2019 Jul 1: syncoffset timeout

Increased the synchoffset timeout to address an issue with large syncoffsets
that move the telescope outside the co-pointing limits and the usual short offset
inside the co-pointing patch is too short and we don't give the large syncoffset
enough time to complete before raising a timeout error.  We set the syncoffset
timeout to the same long (300s) timeout used for presets, since a big syncoffset
could in some cases take about as long as a preset.  Addresses an issue raised
in IT#6939.
  
## 2018 Dec 7: Binocular Patch 3

A bug in Patch 2 when synchoffset is run in an imaging
script with execMODS.
  
## 2018 Nov 14: Binocular Patch 2

Fixed broken patch applied on Oct 19 (users found an unexpected
way through the logic).

## 2018 Oct 19: Binocular Patch

Modified acqMODS and execMODS to explicitly ignore the SYNCOFFSET
command if not a binocular observation. Was originally supposed to be
benign if a binocular directive sent in single-eye mode, but that is
not actually true in practice.

## 2016 Oct 17: Binocular Release

Minor tweaks in testing.  In particular, verified that execMODS
properly sends a syncpreset directive when imaging (.img) scripts are
executed in binocular context.  Executing .img scripts in binocular
context is only permitted in unmixed ("twining") mode when either one
.img script is executed by both MODS ("identical twinning") or two
.img scripts are executed by each MODS ("fraternal twinning").

Note: In the future if a mixed imaging and spectroscopic observation
is being executed in "sibling mode" with MODS1 and MODS2, the imaging
script needs to be split into separate .acq and .obs scripts.  This is
because of the need to synchronize acquisitions for imaging (nearly
trivial) and spectroscopy.

## 2016 Aug 01: Binocular Release Version (v2.3.x)

Release version for testing prior to the start of Semester 2016B.
Implements modifications based on shared-risk binocular operations
during 2016A.  Includes better handling of exits in binocular mode,
and implements binocular operations hooks in acqMODS and execMODS.

## 2016 Jun 19: Binocular Shared-Risk Version (v2.2.x)

Shared-risk partner use started in May 2016 using v2.1.0.  Based
on that experience, made changes to work better in partner observing.
v2.2 incorporates better trapping of fatal errors that caused the
child xterm window to close, taking error messages with it.  Replaced
all instances of the standard Perl "exit" command with "binoExit"
which checks the binoACQ flag, and if being run in binocular context,
will hold the screen at the fatal (abort and return to OS) error
message with a prompt to hit "Enter" to exit.

## 2016 Jan 05: Binocular Commissioning Version (v2.1.x)

New programs:
 * acqMODS_bino - Binocular test version of acqMODS
 * acqBinoMODS - simple wrapper script to run 2 instances of
 * acqMODS_bino in separate xterms for Binocular MODS operation testing

acqMODS_bino adds the --bino/-b flag to run in binocular context.
Absent this flag it assumes monocular context for the given MODS
instance.  $useMODS is no 0, so there is no assumed default MODS1 or
MODS2.

## 2014 Apr 18: Version 2.0BETA

Hooks for binocular MODS1+2 operation.  Provide a single set of
script engines and support program for operating two MODS at the
telescope.  We follow the current LBT Binocular operations model in
which each instrument is operated independently.

Added keywords to now direct command execution to a particular MODS instance:
 * `--mods1` = direct to MODS1
 * `--mods2` = direct to MODS2

Internally, the useMODS parameter sets the default MODS instance,
thus setting $useMODS=1 makes MODS1 default and no `--mods1` is needed.
For binocular operation $useMODS=0 means no default, so `--mods1` and
`--mods2` are required.

