#!/usr/bin/perl 
#
# execMODS - execute a MODS command script
#
# Usage:
#    execMODS [--mods1|--mods2] [options] scriptFile
# 
# Options:
#    --mods1  use MODS1 (alias: -m 1)
#    --mods2  use MODS2 (alias: -m 2)
#    --bino   run in binocular execution mode (default: monocular)
#    -n       dry-run the script w/o execution (alias --dryrun)
#    -r N     run the script starting from command number N
#               (alias --runfrom N)
#    -e       run from the Exec: block to the end (alias --exec)
#    -f name  run from the name: block to the end (alias --fromblock)
#    -b name  run only the name: block commands   (alias --runblock)
#    -u       run unattended and abort on errors  (alias --unattended)
#
# Filename extension convention:
#    Observing Scripts: .obs
#    Calibration Scripts: .cal
#    Imaging Script (hybrid acquisition and obs script): .img
#    Instrument Setup Procedures: .pro
#
# Description:
#   Executes a MODS command script for observing, calibration, or
#   instrument setup.  Scripts are plain text files with MODS commands
#   to be executed in sequence from the start to end of the file.
#   Script execution will halt if there is an error and print the
#   offending command and its command number on the screen.
#
#   execMODS is not used to execute target acquisition scripts.
#   Target acquisition requires greater choreography between the
#   instrument and LBT telescope control and guider/active optics
#   systems than routine data acquisition.  The acqMODS command is
#   provided for target acquisition.  execMODS will refuse to execute
#   an acquisition script.
#
#   In addition to start-to-finish execution, scripts may be executed
#   re-entrantly from a particular command number or from a labeled
#   command block.  This allows a basic level of re-entrancy for fault
#   recovery.
#
#   For most scripts, what you see is what is executed.  However, we
#   have a very few implicit commands that follow high-level directives
#   the main exception is INSTCONFIG which is always followed by an
#   implicit IMCSLOCK.  We keep these to an absolute minimum.  See the
#   MODS Scripting Guide for details (URL etc. below).
#
#   The --mods1 or --mods2 keywords are required if $useMODS=0 is set
#   as the default (this allows a place to set $useMODS=1 to make
#   MODS1 the default, for example).  These flags direct the script to
#   either MODS1 or MODS2.  It is intended to be annoying as we
#   transition from monocular to routine binocular operation.
#
# Option Descriptions:
#
#   The -n (alias --dryrun) option will show the commands to be
#   executed, and run internal script commands like print, pause, and
#   sleep to allow a quick check of logic or test without executing on
#   a live instrument.
#
#   --mods1 or -m 1 or --mods 1  - executes the script on MODS1
#   --mods2 or -m 2 or --mods 2  - executes the script on MODS2
#
#   These are required, if absent, and error message will be printed.
#
#   The -r, -e, -f, and -b options allow for limited re-entrancy for 
#   basic fault recovery:
#
#   -r N              starts execution at command number N to the
#   --runfrom N         end of the script.  Use listMODS to get
#                       command numbers for a script.
#
#   -e                start execution at the Exec: command block to the
#   --exec              end of the script.
#
#   -f name           start execution from the name: command block to the
#   --fromblock name    end of the script. Labels are case *insensitive*.
#
#   -b name           only execute the commands in the name: command block
#   --runblock name      stopping at the next block or the end of the script, 
#                        whichever comes first. Labels are case insensitive
#
# See Also: 
#   listMODS, acqMODS, and chkMODS
#   modsCmd and isisCmd for executing single commands
#
# ToDo: 
#   All the bug fixes and other fun we need as we learn to use these...
#
# Documentation:
#   MODS Scripting Guide (www.astronomy.ohio-state.edu/MODS/ObsTools/Docs/MODSScripts.pdf)
#   Download and Examples: www.astronomy.ohio-state.edu/MODS/ObsTools/modsTools/
#
# Author:
#   R. Pogge, OSU Astronomy Dept.
#   pogge.1@osu.edu
#   2010 Dec 8
#
# Modification History:
#   2010 Dec 17 - Added the PRINT command and current command echo [rwp/osu]
#   2010 Dec 18 - Added stripFirst subroutine [rwp/osu]
#   2011 Feb 9  - Added PAUSE internal script command [rwp/osu]
#   2011 Feb 10 - Added Getopt::Long and smart flag handling.  MODS scripts
#                    can be run re-entrantly via the -r flag.  Started experiments
#                    with -e/--exec to run only the Exec: block.  [rwp/osu]
#   2011 Feb 11 - Enabled timeout option on sendCommand() calls [rwp/osu]
#   2011 Feb 14 - PRESET needs at least a 300s timeout, and fixed the
#                    indented-comment bug [rwp/osu]
#   2011 Feb 19 - Added block-oriented re-entrancy [rwp/osu]
#   2011 Feb 20 - Bug fixes and inline comments [rwp/osu]
#   2011 Mar 16 - Added implicit IMCSLOCK following INSTCONFIG [rwp/osu]
#   2011 Mar 18 - Added color to screen printouts (thanks, Steven!) [rwp/osu]
#   2011 Apr 16 - Changed myID definition (was engineering value) [rwp/osu]
#   2011 May 05 - Added abort/retry/ignore option on command errors [rwp/osu]
#   2011 Sep 18 - SETUPMODE checking fixed, and $baseTO reduced to
#                    60 seconds [rwp/osu]
#   2011 Sep 26 - Color of PRINT is now cyan for greater visibility [rwp/osu]
#   2011 Oct 14 - Added WFSWait from acqMODS for imaging scripts [rwp/osu]
#   2011 Oct 18 - Added POSANGLE command  [rwp/osu]
#   2011 Oct 28 - Added more sophisticated Ctrl+C handling, and an
#                  end-of-script cleanup routine. [rwp/osu]
#   2011 Nov 03 - Further work on abort cleanup at telescope [rwp/osu]
#   2011 Dec 09 - Make sure lamps are off when entering CALMODE, to avoid
#                  problems of delinquent lamps following previous script
#                  execution faults [rwp/osu]
#   2011 Dec 31 - Fixed bugs in the Abort/Retry/Ignore test [rwp/osu]
#   2012 Jun 15 - Fixed bugs in abort servicing [rwp/osu]
#   2012 Oct 13 - Aggressive whitespace abatement [rwp/osu]
#  --------------
#   2013 May 27 - Version 2.0 start for running two MODS at LBT [rwp/osu]
#   2014 Apr 18 - Tweaks as we now really have two MODS at LBT... [rwp/osu]
#   2014 May 01 - Added hooks for processing explicit IMCSLOCK commands
#                  as sometimes appear in procedure scripts [rwp/osu]
#   2016 Jun 21 - Added binocular operations hooks first prototyped in
#                  acqMODS, particularly graceful exit in spawned windows
#                  on fatal errors [rwp/osu]
#   2016 Aug 01 - Binocular operations release for testing for 2016B [rwp/osu]
#   2016 Oct 17 - Tweaks and corrections after testing [rwp/osu]
#   2018 Oct 19 - Explicitly ignore any binocular directives when --bino
#                 is not set [rwp/osu]
#   2019 Jul 01 - Change syncoffset timeout to presetTO to make it longer
#                 for large offsets outside the co-point (treat like a 
#                 mini-preset) [rwp/osu]
#   2024 Jun 13 - Change offset/offsetxy timeout to presetTO [rwp/osu]
#   2025 Sep 10 - Archon CCD controller update changes [rwp/osu]
#---------------------------------------------------------------------------

# Custom ISIS perl module.  These are all the places it can live on
# the machines at LBT and the OSU instrument lab

use lib "/home/modseng/modsPerl";
use lib "/home/dts/modsPerl";

use ISIS;

# Other perl modules, part of the standard distribution

use Getopt::Long;  # command-line parsing
use Term::ANSIColor qw(:constants);  # color output 

# Version number and date - dates in ISO8601 format, please.

$verNum  = "v2.5.0-bino";
$verDate = "2024-06-13";

# Make sure text reverts to normal on using color

$Term::ANSIColor::AUTORESET = 1;

# Lists of MODS1/2 ISIS server and client information

@isisHosts = ("192.168.139.130","192.168.139.230");  # modsN server IP Addrs
@isisPorts = (6600,6600);     # modsNdata server socket ports
@isisIDs   = ("IS","IS");     # MODSn ISIS Host ID
@myIDs     = ("OBS1","OBS2"); # Our ID: OBSn = MODSn Observing Script
@uiHosts   = ("MC1","MC2");   # MODSn control panel host

# Command option defaults

$startCmd = 1;  # start with command 1
$endCmd  = -1;  # -1 = run to the end
$dryRun   = ''; # if true, dry-run the script
$runExec  = ''; # if true, run only exec: commands
$showHelp = ''; # if true, show help/usage message and exit
$showVers = ''; # if true, show version info and exit
$runBlock = ''; # run only the commands in the named block
$fromBlock= ''; # run starting form the named block to the end
$cmdAbort = ''; # if true, aborts on all errors, otherwise 
                # prompt user for abort/retry/ignore 

$useMODS  = 0;  # Default MODS to use (1 or 2).  If useMODS=0, then
                #   neither is default and --mods1 or --mods2 is REQUIREd
$useMODS1 = ''; # (bool) if true, run the script on MODS1
$useMODS2 = ''; # (bool) if true, run the script on MODS2

$binoOBS = '';  # if true, executing a binocular observation

# Get the options from the command line

$retCode = GetOptions('runfrom|r=i'=>\$startCmd,
		      'dryrun|chk|n'=>\$dryRun,
		      'exec|e'=>\$runExec,
		      'unattended|u'=>\$cmdAbort,
		      'runblock|b=s'=>\$runBlock,
		      'fromblock|f=s'=>\$fromBlock,
		      'version|V'=>\$showVers,
		      'mods1' => \$useMODS1,
		      'mods2' => \$useMODS2,
		      'mods|m=i' => \$useMODS,
		      'bino' =>\$binoOBS,
		      'help'=>\$showHelp);

if (!$retCode) {
    print RED "*** ERROR: execMODS terminated with errors\n";
    print "           Type execMODS --help to see usage instructions.\n"; 
    &binoExit(1);
}

# Check the options that terminate execution (help/version)

if ($showHelp) {
    &myUsage;
    &binoExit(0);
}

if ($showVers) {
    print "execMODS $verNum [$verDate]\n";
    &binoExit(0);
}

if ($startCmd < 1) {
    print "ERROR: Argument of -r/--runfrom must be >=1\n";
    &binoExit(1);
}

# What are the remaining command-line elements?

$numArgs = $#ARGV + 1;
if ($numArgs == 1) {
    $scriptFile = $ARGV[0];
}
elsif ($numArgs == 0) {
    &myUsage;
    &binoExit(1);
}
else {
    print RED "** ERROR: Unrecognized command-line option(s)\n";
    &myUsage;
    &binoExit(1);
}

# Make sure the script file exists

if (! -e $scriptFile) {
    print RED "** ERROR: MODS command script file $scriptFile not found.\n";
    &binoExit(1);
}

# Which MODS are we?

if ($useMODS1) {
    $useMODS = 1;
}
elsif ($useMODS2) {
    $useMODS = 2;
}

# No instrument was specified...
if ($useMODS == 0) {
    print RED "** ERROR: Did not specify which MODS Instrument to use.\n";
    print     "            --mods1 = use MODS1\n";
    print     "            --mods2 = use MODS2\n";
    print     "            (alternative: -m 1 or -m 2)\n";
    print     "          execMODS aborting\n";
    &binoExit(2);
}
# or we got a valid MODS instrument to use...
elsif ($useMODS == 1 || $useMODS == 2) {
    $isisHost = $isisHosts[$useMODS-1];
    $isisPort = $isisPorts[$useMODS-1];
    $isisID   = $isisIDs[$useMODS-1];
    $myID     = $myIDs[$useMODS-1];
    $uiHost   = $uiHosts[$useMODS-1];
}
# or we got something invalid...
else {
    print RED "** ERROR: Invalid argument '$useMODS' for -m: must be 1 or 2.\n";
    print     "          execMODS aborting\n";
    &binoExit(2);
}

# Open up a UDP socket channel for communicating with an ISIS server.

$isOK = openISIS($isisHost,$isisPort,$isisID,$myID);

if (!$isOK) {
  print RED "** ERROR: Could not start ISIS connection to MODS${useMODS}. death!\n";
  print "          execMODS aborting\n";
  &binoExit(2);
}

# Trap Ctrl+C gracefully

$SIG{INT} = 'intHandler';

# Initialize the command word and command timeout arrays, counters,
# runtime flags, and block label hash tables we need for script
# execution.

$cmd = ();
$cmdTO = ();
$numCmd = 0;
$numLoops = 0;
$numLines = 0;
$numLabels = 0;
%startBlock = ();
%endBlock = ();
$key = '';

$hasMode = 0;    # do we know our operation mode?
$isSetup = 0;    # is this a setup (.pro) script?
$useLamps = 0;   # are we using calibration lamps?

# Command Execution Timeouts - all are in seconds

$baseTO   =  60;  # base timeout for most commands
$configTO =  90;  # Instrument configuration command timeout (longer)
$imcsTO   = 120;  # IMCS initial setup/recentering timeout
$presetTO = 300;  # Telescope preset timeout (same as for LBT IIF)

# Open the script file and parse it into the command array, counting
# commands as we go and keeping track of command block labels.  We do
# very minimal preprocessing so that there are only a very few,
# carefully chosen implicit commands.

open (MSC,"<$scriptFile") || die "** ERROR: Cannot open $scriptFile: $!\n";

while (<MSC>) {
    $newLine = $_;
    $numLines++;
    chomp($newLine);
    $inLine = simplify($newLine);
    if (length($inLine) > 0) { # skip blank lines
	if (substr($inLine,0,1) eq "#") {
	    # skip comment lines and lines filled with spaces
	}
	else {
	    @lineStr = split('#',$inLine);  # split off inline comments
	    @lineBits = split(' ',$lineStr[0]);

	    $cmdStr  = $lineStr[0];     # full verbatim command and arguments
	    $cmdWord = lc $lineBits[0]; # lowercase command verb
	    $cmdArg = lc $lineBits[1];  # lowercase first command argument

	    # Pre-preprocess the script commands, keeping count

	    # Label: is a command block label

	    if (substr($cmdWord,-1) eq ":") {
		$lstr = length($cmdWord)-1;
		$newKey = uc substr($cmdWord,0,$lstr);
		if (exists($startBlock{$newKey})) {
		    print RED "** ERROR at line ${numLines} - command block label $lineBits[0] already in use\n";
		    print "         execMODS aborting\n";
		    &binoExit(1);
		}
		$numLabels++;
		if ($numLabels == 1) {
		    $key = $newKey;
		}
		else {
		    $endBlock{$key} = $numCmd-1;
		    $key = $newKey;
		}
		$startBlock{$key} = $numCmd;
	    }

	    # LOOP - top of a command loop.  These are DEPRECATED as they are not re-entrant
	    #        but retained for backwards compatibility with older engineering scripts

	    elsif ($cmdWord eq "loop") {
		if ($lineBits[1] > 1) {
		    $cmd[$numCmd] = simplify($cmdStr);
		    $cmdTO[$numCmd] = 0;
		    $loopCount[$numLoops] = $lineBits[1]; # set the loop iteration counter
		    $loopAddr[$numLoops] = $numCmd+1;     # address of command after "loop N"
		    $numLoops++;
		    $numCmd++;
		}
		else {
		    print RED "** ERROR at line ${numLines} - LOOP must specify >1 iterations\n";
		    print "         execMODS aborting\n";
		    &binoExit(1);
		}
	    }

	    # Trap ACQMODE which would identify this as a target acquisition script!

	    elsif ($cmdWord eq "acqmode") {
		print RED "** ERROR at line $numLines - invalid $lineBits[0] command found\n";
		print "         Target acquisition scripts may only be executed using acqMODS.\n";
		print "         execMODS aborting\n";
		&binoExit(1);
	    }

	    # Trap ACQxyz commands - execMODS does not run .acq scripts

	    elsif (substr($cmdWord,0,3) eq "acq") {
		print RED "** ERROR at line $numLines - invalid target-acquisition command $lineBits[0]\n";
		print "         Target acquisition scripts may only be executed using acqMODS.\n";
		print "         execMODS aborting\n";
		&binoExit(1);
	    }

	    # OBSMODE, CALMODE, and SETUPMODE set the type of script this is

	    elsif ($cmdWord eq "obsmode" || $cmdWord eq "calmode" || $cmdWord eq "setupmode") {
		$hasMode = 1;
		$cmd[$numCmd] = simplify($cmdStr);
		$cmdTO[$numCmd] = $configTO;
		$numCmd++;
		if ($cmdWord ne "setupmode") {
		    $cmd[$numCmd] = "nimgs 1";  # reset the image counter
		    $cmdTO[$numCmd] = 10;
		    $numCmd++;
		    $isSetup = 0;
		}
		else {
		    $isSetup = 1;
		}
		# new: lamp off sanity-check for calmode - someday internal to the IE...
		if ($cmdWord eq "calmode") {
		    $cmd[$numCmd] = "lamp off";  # make sure any lamp are OFF
		    $cmdTO[$numCmd] = 10;
		    $numCmd++;
		}		
	    }

	    # GO and DGO do not use timeouts

	    elsif ($cmdWord eq "dgo" || $cmdWord eq "go"  || $cmdArg eq "go") {
		$cmd[$numCmd] = simplify($cmdStr);
		$cmdTO[$numCmd] = 0;  
		$numCmd++;
	    }

	    # PRESET moves the telescope, so has a special timeout

	    elsif ($cmdWord eq "preset") {
		if ($binoOBS) {
		    $cmd[$numCmd] = "BC SYNCPRESET";
		    $cmdTO[$numCmd] = $baseTO;  
		    $numCmd++;
		}
		$cmd[$numCmd] = simplify($cmdStr);
		$cmdTO[$numCmd] = 1.5*$presetTO;  
		$numCmd++;
	    }

	    # POSANGLE - specify the celestial position angle of the instrument.
	    #            Inserts ROTATOR xx POSITION into the command stream

	    elsif ($cmdWord eq "posangle") {
		if (length($cmdArg) > 0) {
		    $cmd[$numCmd] = "ROTATOR $cmdArg POSITION";
		    $cmdTO[$numCmd] = $baseTO;
		    $numCmd++;
		}
		else {
		    print RED "** ERROR at line $numLines - POSANGLE requires an argument\n";
		    print "   Usage: POSANGLE angle  (celestial PA in degrees N-to-E)\n";
		    print "   execMODS aborting.\n";
		    &binoExit(1);
		}
	    }

	    # INSTCONFIG is always followed by an IMCSLOCK command if
	    # not in setupmode

	    elsif ($cmdWord eq "instconfig") {
		$cmd[$numCmd] = simplify($cmdStr);
		$cmdTO[$numCmd] = $configTO;  
		$numCmd++;
		if ($isSetup == 0) {
		    $cmd[$numCmd] = "imcslock";
		    $cmdTO[$numCmd] = $imcsTO;  
		    $numCmd++;
		}
	    }

	    # SLITMASK can sometimes take as long as INSTCONFIG if a
	    # full retract/travel/insert sequence is required

	    elsif ($cmdWord eq "slitmask") {
		$cmd[$numCmd] = simplify($cmdStr);
		$cmdTO[$numCmd] = $configTO;  
		$numCmd++;
	    }

	    # An *explicit* IMCSLOCK requires a longer timeout

	    elsif ($cmdWord eq "imcslock") {
		$cmd[$numCmd] = simplify($cmdStr);
		$cmdTO[$numCmd] = $imcsTO;  
		$numCmd++;
	    }

	    # Any LAMP command also sets the useLamps flag

	    elsif ($cmdWord eq "lamp") {
		$cmd[$numCmd] = simplify($cmdStr);
		$cmdTO[$numCmd] = $baseTO;  
		$numCmd++;
		$useLamps = 1;
	    }

	    # SYNCOFFSET - binocular mode synchronized offset, wrap in
	    #              a BC prefix to direct to the IIF agent
	    #              Treat like a preset for timeout

	    elsif ($cmdWord eq "syncoffset"){
		if ($binoOBS) {
		    $cmd[$numCmd] = "bc syncoffset";
		    $cmdTO[$numCmd] = $presetTO;  
		    $numCmd++;
		}
	    }
	    
	    # OFFSET/OFFSETXY - telescope offsets
            #              Treat like a preset for timeouts
            
	    elsif ($cmdWord eq "offset" || $cmdWord eq "offsetxy") {
		$cmd[$numCmd] = simplify($cmdStr);
		$cmdTO[$numCmd] = $presetTO;  
		$numCmd++;
	    }

	    # Assume what is left is a valid command that uses the base
	    # timeout - always an adventure...

	    else {
		$cmd[$numCmd] = simplify($cmdStr);
		$cmdTO[$numCmd] = $baseTO;
		$numCmd++;
	    }
	}
    }
}
close(MSC);

# The last block ends with the last command in the file

$endCmd = $numCmd-1;
$endBlock{$key} = $endCmd;

# Some quick checks to see if we have anything to do

if ($numCmd < 1) {
    print RED "** ERROR: No executable commands found in $scriptFile\n";
    print "          execMODS aborting\n";
    &binoExit(1);
}

# Is the starting command number actually in the script?

if ($startCmd > $numCmd) {
    print RED "** ERROR: Cannot start running script at command $startCmd\n";
    print "          There are only $numCmd commands in the $scriptFile\n";
    print "          execMODS aborting\n";
    &binoExit(1);
}

# Has no execution mode been sent (OBSMODE, CALMODE, or SETUPMODE)?

if (!$hasMode) {
    print RED "** ERROR: No operating mode has been set in this script.\n";
    print "          Scripts must have OBSMODE, CALMODE, or SETUPMODE as the first line\n";
    print "          execMODS aborting\n";
    &binoExit(1);
}

# We have been asked to run from the Exec block to the end

if ($runExec) {
    if (exists($startBlock{"EXEC"})) { # does the goto has have an EXEC: label?
	$startCmd = $startBlock{"EXEC"} + 1;
	print BLUE "** Running from the Exec: block to the end of $scriptFile\n";
    }
    else {
	print RED "** ERROR: exec-only option requested, but no EXEC: label was\n";
	print "          found in this script.\n";
	print "          execMODS aborting\n";
	&binoExit(1);
    }
}

# Have we been asked to run just a named block?

if (length($runBlock) > 0) {
    $key = uc $runBlock;
    if (exists($startBlock{$key})) {
	$startCmd = $startBlock{$key} + 1;
	$endCmd = $endBlock{$key};
	print BLUE "** Running only the $runBlock command block of $scriptFile\n";
    }
    else {
	print RED "** ERROR: Command block label ${runBlock}: not found in this script.\n";
	print "          execMODS aborting\n";
	&binoExit(1);
    }
}

# Have we been asked to run from a named block to the end?

elsif (length($fromBlock) > 0) {
    $key = uc $fromBlock;
    if (exists($startBlock{$key})) {
	$startCmd = $startBlock{$key} + 1;
	print BLUE "** Running from command block $fromBlock to the end of $scriptFile\n";
    }
    else {
	print RED "** ERROR: Command block label ${fromBlock}: not found in this script.\n";
	print "          execMODS aborting\n";
	&binoExit(1);
    }
}

# Last checks before we commit to run...

# Is there an Acquire: label?  If so, assume this is an acquisition
# script that must be run by acqMODS and abort.  It may mean someone
# set a valid mode (not ACQMODE), but snuck in an Acquire block (e.g.,
# a thoughtless cut-n-paste job or just tired)

if (exists($startBlock{"ACQUIRE"})) {
    print RED "** ERROR: Acquire: label found - is this a Target Acquisition (.acq) script?\n";
    print "          Target acquisition scripts may only be executed using acqMODS.\n";
    print "          execMODS aborting\n";
    &binoExit(1);
}

#----------------------------------------------------------------
#
# We have a list of script commands to execute - run it...
#

if ($binoOBS) {
    $idTag = "binocular MODS${useMODS}";
}
else {
    $idTag = "MODS${useMODS}";
}

if ($dryRun) {
    print BLUE "\n** Dry-run of ${idTag} script ${scriptFile}\n\n";
}
else {
    print BLUE "\n** Executing ${idTag} script ${scriptFile}\n\n";
}

$curLoop = -1;
$execGo = 1;
$iCmd = $startCmd - 1;

while ($execGo) {
    if ($iCmd > $endCmd) {
	$cmd[$iCmd] = "end";
	if (!$dryRun) { &scriptCleanup; }
	print BLUE "\n** Script $scriptFile done\n";
	&binoExit(0); # exit 0;
    }

    @cmdBits = split(' ',$cmd[$iCmd]);
    $cmdWord = lc $cmdBits[0];

    # END command - terminates the script
    
    if ($cmdWord eq "end") {
	if (!$dryRun) { &scriptCleanup; }	
	print BLUE "\n** Script $scriptFile done\n";
	&binoExit(0); # exit 0;
    }

    # END_LOOP - bottom of the current loop - DEPRECATED as loops are not re-entrant 

    elsif ($cmdWord eq "end_loop") {
	if ($curLoop >= 0) {
	    $loopCount[$curLoop]--;  # decrement the current loop counter
	    if ($loopCount[$curLoop] > 0) { # still more to go
		$iCmd = $loopAddr[$curLoop];
	    }
	    else {
		$curLoop--;
		$iCmd++;
	    }
	}
    }

    # SLEEP - suspend script execution and sleep for a specified time
    #         interval in seconds before resuming

    elsif ($cmdWord eq "sleep") {
	$sleepTime = $cmdBits[1];
	if ($sleepTime > 0) {
	    print "\n";
	    $| = 1;
	    for ($time=$sleepTime;$time>0;$time--) {
		printf "%c[2K",27;
		print BLUE "** Sleeping for $sleepTime sec, will wake in $time sec...\r";
		sleep 1;
	    }
	    printf "%c[2K",27;
	    print BLUE "** Sleep done\n\n";
	}
	$iCmd++;
    }

    # GOTO - branch to a label - DEPRECATED as branching is not readily re-entrant

    elsif ($cmdWord eq "goto") {
	$key = lc $cmdBits[1];
	if (exists($startBlock{$key})) {
	    $iCmd = $startBlock{$key};  # address of first command after "label:"
	}
	else {
	    print RED "** ERROR: unknown label \"$key\"\n";
	    print "   execMODS aborting\n";
	    &binoExit(1);
	}
    }

    # SETUPMODE - this is an instrument setup procedure (.pro) script.
    #             unlike OBSMODE or CALMODE, this is a no-op placeholder,
    #             but it make take on setup-specific functions later.

    elsif ($cmdWord eq "setupmode") {
	$iCmd++;
    }

    # LOOP - top of a command iteration loop - DEPRECATED as loops are not re-entrant

    elsif ($cmdWord eq "loop") {
	$curLoop++;   # increment the loop counter
	$loopCount[$curLoop] = $cmdBits[1];
	$iCmd = $loopAddr[$curLoop];  # address of first command in this loop
    }

    # PRINT - print a message on the terminal (stdout)

    elsif ($cmdWord eq "print") {
	$prStr = stripFirst($cmd[$iCmd]);
	print CYAN "\n$prStr\n";
        $iCmd++;
    }

    # PAUSE - Pause script execution and wait for user action
    #         Enter/R = Resume, X/A = eXit/Abort

    elsif ($cmdWord eq "pause") {
	print MAGENTA "\nScript Execution PAUSED, hit Enter to RESUME or X to ABORT... ";
	$kbdIn = <STDIN>;
	last unless defined $kbdIn;
	chomp($kbdIn);
	$pauseCmd = uc $kbdIn;
	if ($pauseCmd eq "X" || $pauseCmd eq "A") {
	    print RED "\n** $scriptFile Aborted after PAUSE.\n\n";
	    &binoExit(1);
	}
	print BLUE "Script Execution Resuming...\n\n";
	$iCmd++;
    }

    # WFSWAIT - Pause script execution and wait for active optics system to
    #           to converge.  Enter/R = Resume, X/A = eXit/Abort

    elsif ($cmdWord eq "wfswait") {
	print MAGENTA "\n** Wait for WFS collimation to converge, then hit Enter to RESUME\n";
	print MAGENTA "   or X to ABORT... ";
	$kbdIn = <STDIN>;
	last unless defined $kbdIn;
	chomp($kbdIn);
	$pauseCmd = uc $kbdIn;
	if ($pauseCmd eq "X" || $pauseCmd eq "A") {
	    print RED "\n** $scriptFile aborted after PAUSE.\n\n";
	    &binoExit(1);
	}
	print BLUE "\n** Target Acquisition Resuming...\n";
	$iCmd++;
    }

    # Add other non-instrument script commands here.

    # All that's left is a data-taking system command, send it

    else {
	if ($dryRun) {
	    print ">> $cmd[$iCmd]\n";
	    $iCmd++;
	}
	else {
	    print ">> $cmd[$iCmd]\n";
	    if (sendCommand($uiHost,"$cmd[$iCmd]",$cmdTO[$iCmd])) {
		printf "%c[2K",27;
		$outStr = stripFirst($ISIS::reply);
		print BLUE "$outStr\n";
		$iCmd++;
	    }
	    else {
		printf "%c[2K",27;
		$outStr = stripFirst($ISIS::reply);
		print RED "** $outStr\a\n";
		$errLine = $iCmd+1;
		if ($cmdAbort) { # unattended operation, all command errors abort
		    print RED "   $scriptFile terminated with errors at line $errLine command '$cmd[$iCmd]'\n";
		    &binoExit(2);
		}
		else { # prompt the user to abort/retry/ignore in response to errors
		    print RED "** Abort, Retry, or Ignore? > ";
		    $kbdIn = <STDIN>;
		    last unless defined $kbdIn;
		    chomp($kbdIn);
		    $retryOpt = substr($kbdIn,0,1);
		    if (uc $retryOpt eq "R") {
			print BLUE "** Retrying command \'$cmd[$iCmd]\'\n";
		    }
		    elsif (uc $retryOpt eq "I") {
			print BLUE "** Ignoring error and continuing...\n";
			$iCmd++;
		    }
		    else {
			print RED "\n** $scriptFile aborted with errors at line $errLine command '$cmd[$iCmd]'\n";
			&scriptCleanup;
			&binoExit(2);
		    }
		}
	    }
	}
    }
    if ($iCmd == $numCmd) {
	$execGo = 0;
    }
}

# All done, close it down (if somehow we get here)

print BLUE "\n** Script $scriptFile done\n";
if (!$dryRun) { 
    $cmd[$iCmd] = "end";
    &scriptCleanup; 
}
closeISIS();
&binoExit(0); # exit 0;

#---------------------------------------------------------------------------
#
# simplify - simplify a character string
#
# Removes leading and trailing whitespace from a string and collapses
# excess embedded white space to single spaces (including removing TAB
# characters).  Uses regexp for speed.
#

sub simplify {
    my @out = @_;
    for (@out) {
	s/^\s+//;  # remove leading whitespace
	s/\s+$//;  # remove trailing whitespace
        s/\s+/ /g; # clean up embedded whitespace
    }
    return wantarray ? @out : $out[0];
}

#---------------------------------------------------------------------------
#
# Strip off the first word from a multi-word string
#

sub stripFirst {
    my $inStr = $_[0];
    my @strBits = split(' ',$inStr);
    my $numWords = $#strBits+1;
    if ($numWords > 1) {
	$prStr = "$strBits[1]";
	if ($numWords > 2) {
	    for ($i=2;$i<$numWords;$i++) {
		$prStr = "$prStr $strBits[$i]";
	    }
	}
	return $prStr;
    }
    else {
	return $inStr;
    }
}

#---------------------------------------------------------------------------
#
# myUsage - print the script usage message
#

sub myUsage {
    print "\nUsage: execMODS [--mods1|--mods2] [options] scriptFile\n";
    print "\n  Executes the MODS command script named scriptFile on the named MODS\n";
    print "    (--mods1 or --mods2) - these are REQUIRED.\n";
    print "\n  scriptFile may be an observing (.obs) script, a calibration (.cal)\n";
    print "    script, an imaging script (.img), or a setup procedure (.pro) script.\n";
    print "\nNote: Target acquisition (.acq) scripts may only be run using acqMODS\n";
    print "\nOptions:\n";
    print "  --bino   execute this script in binocular mode.\n";
    print "  -n       dry-run the script w/o execution (alias --dryrun/--chk)\n";
    print "  -r N     run the script starting from command number N (alias --runfrom)\n";
    print "             (use listMODS to get command numbers\n";
    print "  -e       run from the Exec: block to the end (alias --exec)\n";
    print "  -f name  run from the name: block to the end (alias --fromblock)\n";
    print "  -b name  run only the name: block commands   (alias --runblock)\n";
    print "  -u       run unattended and abort on errors  (alias --unattended)\n";
    print "\nSee Also: listMODS, acqMODS, chkMODS\n";
    print "\n";
}

#---------------------------------------------------------------------------
#
# intHandler - SIGINT (Ctrl+C) Handler
#
# Provides for more graceful Ctrl+C interruption of script execution,
# including post-abort scriptCleanup
#

sub intHandler {
    print RED "** Script interrupted by Ctrl+C during the '$cmd[$iCmd]' command.\n";
    print BLUE "   Is this what you want to do <Y|N>? ";
    $kbdIn = <STDIN>;
    last unless defined $kbdIn;
    chomp($kbdIn);
    $abortOpt = uc $kbdIn;
    if ($abortOpt eq "Y") {
	print RED "** Script aborting...\n";
	print BLUE "** post-abort clean up...\n";
	&scriptCleanup;
	print BLUE "** Script abort sequence completed.\n";
	&binoExit(2);
    }
    else {
	print BLUE "** OK, Ignoring Ctrl+C interrupt and continuing...\n";
    }
}

#---------------------------------------------------------------------------
#
# scriptCleanup - end-of-script clean up
#
# Actions:
#   If in an exposure, send an ABORT
#   If in an instconfig, clear state flags
#   If in a preset or offset, clear IIF flags
#   Turn off any lamps that might be on
#
# Goal:
#   More graceful abort that should speed fault recovery
#

sub scriptCleanup {
    # Take apart the last command before we got here.

    @lastBits = split(' ',$cmd[$iCmd]);
    $lastCmd = lc $lastBits[0]; # lowercase command verb
    $lastArg = lc $lastBits[1];  # lowercase first command argument

    # If this is normal script termination on "end", do all routine
    # end-of-script clean up here

    if ($lastCmd eq "end") {
	if ($useLamps) { sendCommand($uiHost,"lamp off",10); }
	closeISIS();
    }

    # Abort (abnormal script termination) - do abort clean up here

    else {
	if ($lastCmd eq "dgo" || $lastCmd eq "go"  || $lastArg eq "go") { # abort exposures
	    if ($lastCmd eq "red" || $lastCmd eq "blue") {
		$abortCmd = "$lastCmd abort";
	    }
	    else {
		$abortCmd = "abort";
	    }
	    print BLUE "   ... aborting exposure(s) in progress ...\n";
	}
	elsif ($lastCmd eq "instconfig") { # clear instconfig state
	    $abortCmd = "refresh instconfig";
	    print BLUE "   ... clearing instconfig state flags ...\n";
	}
	elsif ($lastCmd eq "imcslock") { # abort an IMCSLOCK request
	    $abortCmd = "imcsabort";
	    print BLUE "   ... aborting IMCS lock request ...\n";
	}
	elsif ($lastCmd eq "preset" || $lastCmd eq "offsetxy" || $lastCmd eq "offset") { # clear IIF state
	    print BLUE "   ... clearing $lastCmd ...\n";
	    $abortCmd = "clearstars";
	}
	else {
	    $abortCmd = "";
	}
	if (length($abortCmd) > 0) {
	    if (sendCommand($uiHost,$abortCmd,30)) {
		printf "%c[2K",27;
		$outStr = stripFirst($ISIS::reply);
		print BLUE "$outStr\n";
	    }
	    else {
		printf "%c[2K",27;
		$outStr = stripFirst($ISIS::reply);
		print RED "** $outStr\a\n";
		print RED "   ... abort sequence terminated with errors.\n";
		&binoExit(2);
	    }
	}

	# Make sure lamps are turned off if used
	
	if ($useLamps) {
	    sendCommand($uiHost,"lamp off",10);
	}
    }
    # Anything else?
}

#---------------------------------------------------------------------------
#
# binoExit - Exit binocular mode
#
# Because binocular observations are launched in spawned terminal
# windows, when a fatal error exits to the shell, the window will
# close before the error message may be read and acted upon by the
# observer.
#
# This function replaces the stock Perl exit() function, so that if a
# fatal error requiring exit() occurs during a binocular observation,
# the exit is halted at a prompt instructing the observer to press the
# Enter key to continue.
#
# It does this by checking the global $binoOBS boolean:
#   If binoOBS is False, it exits immediately.
#   If binoOBS is True, it stops and prompts to hit Enter, giving the user
#      time to read error messages.
#
 
sub binoExit {
    my $exStatus = $_[0];
    if ($binoOBS) {
	if ($exStatus != 0) {
	    print RED "\n** Binocular MODS${useMODS} script aborting on fatal error.\n";
	}
	print BLUE "\n** Hit the <Enter> key to exit and close this window...";
	$kbdIn = <STDIN>;
	last unless defined $kbdIn;
	print BLUE "Bye!\n";
    }
    exit $exStatus;
}
