#!/usr/bin/perl 
#
# acqMODS - execute a MODS target acquisition (.acq) script.
#
# Usage:
#    acqMODS [options] file.acq
# 
# Options:
#    -m n  run on MODSn.  Aliases: --mods n, --mods1/m1 --mods2/m2
#            this option is REQUIRED.
#    -b    run as part of a binocular acquisition (alias --bino)
#    -n    dry-run script w/o execution (aliases: --dryrun or --chk)
#    -a    only execute the Acquire: block (alias: --acquire)
#    -p    only perform the target and preset block (alias --preset)
#    -i    only perform the instrument configuration (alias --inst)
#    -l    load telescope target/preset parameters but don't execute
#             (alias --load)
#
# Description:
#   Executes a MODS target acquisition script.  ACQ scripts include
#   instructions for pointing the telescope configuring the instrument
#   for imaging in the selected channel, and taking field and
#   thru-slit acquisition images as requested.
#
#   Unlike data acquisition, calibration, or instrument "procedure"
#   scripts, target acquisition requires relatively tight choreography
#   between the instrument, telescope, and guider/active optics
#   systems.  We therefore split off the acquisition process to
#   explicitly account for these important differences.  Structurally
#   an acquisition script looks the same as an observing or
#   calibration script, but it is preprocessed and handled
#   differently.
#
#   Acquisition scripts should be given the .acq file extension to
#   distinguish them from observing (.obs), calibration (.cal), and
#   procedure (.pro) scripts.  The separate execMODS command is used
#   for non-acquisition script execution.
#
#   If the cmdRetry flag is set true, then errors arising from the
#   instrument proper will print an error message followed by an
#   "Abort/Retry" prompt, allowing the observer to retry a failed
#   command (e.g., pick up after a comm glitch) or abort and take the
#   error as bad enough to stop the observing process.
#
# Option Descriptions:
#
#   The -n (aliases --dryrun or -chk) option will show the commands to
#   be executed, and run internal script commands like print, pause,
#   and sleep to allow a quick check of logic or test without
#   executing on a live instrument.
#
#   The -a, -p, -i and -l options allow for limited re-entrancy for 
#   basic fault recovery:
#
#   -a (--acquire) only takes the acquisition images in the Acquire: block,
#                    but does not point the telescope or configure the 
#                    instrument. It assumes MODS is on-target and setup
#                    correctly for acquisition images.
#
#   -p (--preset)  only executes the telescope preset (load RA/Dec and 
#                    point telescope), but does not configure the 
#                    instrument or take acquisition images
#
#   -i (--inst)    only executes the instrument configuration, but does 
#                    not point the telescope or take acquisition images.
#
#   -l (--load)    only uploads the target and guide-star coordinates to 
#                    the MODS control panel but does not execute the preset,
#                    configure the instrument, or take images.
#
#   For observing and calibration data acquisition and instrument
#   setup and configuration scripts, see execMODS.
#
# See Also: 
#   execMODS, listMODS, and chkMODS
#
# Author:
#   R. Pogge, OSU Astronomy Dept.
#   pogge.1@osu.edu
#   2010 Feb 20
#
# Modification History:
#   2011 Feb 20 - Beta version based on execMODS v2.3 [rwp/osu]
#   2011 Mar 18 - Added color to screen printouts (thanks, Steven!) [rwp/osu]
#   2011 Mar 22 - Added ACQROI to allow a different ROI than 3Kx3K [rwp/osu]
#   2011 Apr 14 - Updates and bug fixes from April 2011 commissioning [rwp/osu]
#   2011 Apr 15 - Added abort/retry/ignore option on errors [rwp/osu]
#   2011 May 05 - Added ignore to the error trap options [rwp/osu]
#   2011 Sep 27 - Minor tweaks [rwp/osu]
#   2011 Oct 18 - Added POSANGLE command [rwp/osu]
#   2011 Oct 28 - Added more sophisticated Ctrl+C handling [rwp/osu]
#   2011 Nov 03 - Further work on abort cleanup at the telescope [rwp/osu]
#   2011 Dec 10 - v1.1 script engine release clean-up [rwp/osu]
#   2011 Dec 31 - Fixed bug in abort/retry/fail test, advanced version
#                  to v1.0, out of beta testing [rwp/osu]
#   2012 Feb 16 - Fixed major bug - was looking for Telescope: not the
#                  Target: label.  Oops.  Affected use of -p [rwp/osu]
#   2012 Mar 22 - Added focus offset option for MOS mode acquisitions
#                  change offFocRed/Blue below [rwp/osu]
#   2012 Jun 15 - tweaks to focus option for MOS, on-sky testing [rwp/osu]
#   2012 Oct 13 - Added aggressive whitespace abatement [rwp/osu]
#   2013 Oct 05 - changed "RED CAMFOC STEP" to "RCAMFOC STEP" to work
#                  around a GUI command parser bug [rwp/osu]
#   ------------
#   2014 Jan 24 - Version 2 - single script engine to control either MODS
#                 instrument (not both simultaneously) [rwp/osu]
#   2014 Apr 18 - Tweaks introduced after MODS2 was delivered to LBT
#                 and we have 2 MOD on one network [rwp/osu]
#   2016 Jan 05 - **BETA** binocular version [rwp/osu]
#   2016 Jun 19 - Tweaks based on field experience, especially enhanced
#                 error detection/trapping for binocular operation [rwp/osu]
#   2016 Aug 01 - Binocular operations release for testing for 2016B [rwp/osu]
#   2016 Oct 17 - Minor tweaks based on testing [rwp/osu]
#   2018 Oct 19 - Explicitly ignore any binocular directives when --bino
#                 is not set [rwp/osu]
#   2019 Jul 01 - Increased syncpreset timeout to presetTO (and made 
#                 presetTO a parameter instead of hard coded) [rwp/osu]
#   2021 Mar 16 - fixed problem of not aborting exposure on Ctrl+C [rwp/osu]
#   2024 Jun 13 - Increased offset/offsetxy timeout to presetTO [rwp/osu]
#   2024 Jun 14 - Typo at line 715 "$go" --> "go" [rwp/osu]
#   2025 Sep 10 - Archon CCD controller system updates [rwp/osu]
#   2025 Oct 03 - Add LBTO NFS-mounted common path [rwp/osu]
#
#---------------------------------------------------------------------------

# Custom ISIS.pm module.  These are all the places it can be on the
# LBTO mountain and OSU VLAN linux systems.

use lib "/lbt/lbto/mods/lib/modsPerl"; # LBTO mountain network common path
use lib "/home/dts/modsPerl"; # MODS data-taking system path

use ISIS;

# Standard modules we use

use Getopt::Long;  # command-line parsing
use Term::ANSIColor qw(:constants);  # color output 

# Version number and date - date in ISO8601 format

$verNum  = "v2.5.1-bino";
$verDate = "2025-10-03";

# Make sure text reverts to normal on using color

$Term::ANSIColor::AUTORESET = 1;

# Lists of MODS1/2 ISIS server and client information

@isisHosts = ("192.168.139.130","192.168.139.230");  # modsN server IP Addrs
@isisPorts = (6600,6600);     # modsNdata server socket ports
@isisIDs   = ("IS","IS");     # MODSn ISIS Host ID
@myIDs     = ("ACQ1","ACQ2"); # Our ID: OBSn = MODSn Observing Script
@uiHosts   = ("MC1","MC2");   # MODSn control panel host
@focalStation = ("LDG","RDG");  # MODSn focal stations

# If cmdRetry is 1, prompt for abort/retry/ignore on errors

$cmdRetry = 1; 

# Command option defaults

$startCmd = 1;  # start with command 1
$dryRun   = ''; # if true, dry-run the script
$runAcq   = ''; # if true, run only Acquire: block commands
$runPreset= ''; # if true, only load target coords and execute the preset
$runInst  = ''; # if true, only run the instrument configuration block
$loadOnly = ''; # if true, only load the next target coords, but don't execute preset
$showHelp = ''; # if true, show help/usage message and exit
$showVers = ''; # if true, show version info and exit

$useMODS  = 0;  # Default MODSn instance, n=(1|2). If useMODS=0
                #   then no default, and --mods1/--mods2 are REQUIRED

$useMODS1 = ''; # Use MODS1
$useMODS2 = ''; # Use MODS2

$binoACQ = ''; # Binocular acquisition

# Get the options from the command line

$retCode = GetOptions('dryrun|chk|n'=>\$dryRun,
		      'mods1|m1'=>\$useMODS1,
		      'mods2|m2'=>\$useMODS2,
		      'mods|m=i'=>\$useMODS,
		      'bino|b'=>\$binoACQ,
		      'preset|p'=>\$runPreset,
		      'inst|i'=>\$runInst,
		      'load|l'=>\$loadOnly,
		      'acquire|a'=>\$runAcq,
		      'version|V'=>\$showVers,
		      'help'=>\$showHelp);

if (!$retCode) {
    print RED "*** ERROR: acqMODS terminated with errors\n";
    print "           Type acqMODS --help to see usage instructions.\n";
    &binoExit(1);
}

# Check the options that terminate execution (help/version)

if ($showHelp) {
    &acqUsage;
    &binoExit(0);
}

if ($showVers) {
    print "acqMODS $verNum [$verDate]\n";
    &binoExit(0);
}

# What are the remaining command-line elements?

$numArgs = $#ARGV + 1;
if ($numArgs == 1) {
    $acqFile = $ARGV[0];
}
elsif ($numArgs == 0) {
    &acqUsage;
    &binoExit(1);
}
else {
    print RED "** ERROR: Unrecognized command-line option(s)\n";
    &acqUsage;
    &binoExit(1);
}

# Must have specified MODS1 or MODS2 explicitly

if ($useMODS1) {
    $useMODS = 1;
}
elsif ($useMODS2) {
    $useMODS = 2;
}

if ($useMODS == 0) {
    print RED "** ERROR: MODS instrument (MODS1 or MODS2) not specified.\n";
    print "   Please indicate which MODS this script is to be run on\n";
    print "   (--mods1 or --mods2)\n";
    &binoExit(1);
}
# or we got a valid MODS instrument to use...
elsif ($useMODS == 1 || $useMODS == 2) {
    $isisHost = $isisHosts[$useMODS-1];
    $isisPort = $isisPorts[$useMODS-1];
    $isisID   = $isisIDs[$useMODS-1];
    $myID     = $myIDs[$useMODS-1];
    $uiHost   = $uiHosts[$useMODS-1];
    $myFocus  = $focalstation[$useMODS-1];  # MODSn focal stations
}
# or we got something invalid...
else {
    print RED "** ERROR: Invalid MODS number '$useMODS': must be 1 or 2.\n";
    print     "          acqMODS aborting\n";
    &binoExit(1);
}

# Make sure the script file exists

if (! -e $acqFile) {
    print RED "** ERROR: MODS Target Acquisition Script \"$acqFile\" not found.\n";
    print "          acqMODS aborting\n";
    &binoExit(1);
}

# We got through basic validations (acq script validations are ahead),
# start the ball rolling.

if ($binoACQ) {
    print CYAN "\n** Starting binocular MODS${useMODS} acquisition script $acqFile\n\n";
} else {
    print CYAN "\n** Starting MODS${useMODS} acquisition script $acqFile\n\n";
}

# Open up a UDP socket channel for communicating with an ISIS server.
# This is the last point where the MODS1/MODS2 distinction matters.

$isOK = openISIS($isisHost,$isisPort,$isisID,$myID);

if (!$isOK) {
    print RED "** ERROR: Could not start ISIS connection to MODS${useMODS}. death!\n";
    print "          acqMODS aborting\n";
    &binoExit(1);
}

# Trap Ctrl+C gracefully

$SIG{INT} = 'intHandler';

# Initialize the command word and command timeout arrays, element
# counters, runtime flags, and block label hash tables.   MODS1 and
# MODS2 are as identical as we could make them, so there should be
# no difference in these parameters between the two instances

$cmd = ();
$cmdTO = ();
$numCmd = 0;
$numLines = 0;
$numLabels = 0;
%startBlock = ();
%endBlock = ();
$key = '';
$haveMode = 0;
$baseTO = 120;   # base timeout interval in seconds
$shortTO = 60;   # short timeout interval in seconds
$presetTO = 300; # long timeout for preset, syncoffset, and offsets
$acqCamera = '';  
$acqMode   = '';    
$objName   = '';
$maskName  = '';
$offFocRed = 250; # Red Channel MOS acquisition image focus offset
$offFocBlue= 200; # Blue Channel MOS acquisition image focus offset

# Flags

$hasMode = 0;
$hasPreset = 0;
$hasObject = 0;
$hasMask = 0;
$hasEnd = 0;

# Open the script file and parse it into the command array, counting
# commands as we go and keeping track of command block labels.

open (MSC,"<$acqFile") || die "** ERROR: Cannot open $acqFile: $!\n";

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
	    $cmdArg  = lc $lineBits[1]; # lowercase first command argument

	    # pre-preprocessing...
	    
	    # LABEL: is a command block label, start the command block info

	    if (substr($cmdWord,-1) eq ":") { # a command block label
		$lstr = length($cmdWord)-1;
		$newKey = uc substr($cmdWord,0,$lstr);
		if (exists($startBlock{$newKey})) {
		    print RED "** Syntax Error at line $numLines - duplicate block label \"$lineBits[0]\"\n";
		    print RED "   Block labels must only be used once inside a script.\n";
		    print     "   acqMODS aborting.\n";
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

		# Some block labels have implicit initializations required

		if ($key eq "TARGET") {
		    # Make sure there are no delinquent target/guide star
		    # coordinates in the IIF/TCS memory from a previous
		    # preset

		    $cmd[$numCmd] = "ClearStars";
		    $cmdTO[$numCmd] = $shortTO;
		    $numCmd++;
		}

	    }

	    # OBSMODE, CALMODE, or SETUPMODE mean this is not an acquisition script - abort!

	    elsif ($cmdWord eq "obsmode") {
		print RED "** ERROR at line $numLines - Invalid acquisition script command $lineBits[0]\n";
		print "         Observing scripts may only be executed using execMODS.\n";
		print "         acqMODS aborting\n";
		&binoExit(1);
	    }
	    elsif ($cmdWord eq "calmode") {
		print RED "** ERROR at line $numLines - Invalid acquisition script command $lineBits[0]\n";
		print "         Calibration scripts may only be executed using execMODS.\n";
		print "         acqMODS aborting\n";
		&binoExit(1);
	    }
	    elsif ($cmdWord eq "setupmode") {
		print RED "** ERROR at line $numLines - Invalid acquisition script command $lineBits[0]\n";
		print "         Procedure scripts may only be executed using execMODS.\n";
		print "         acqMODS aborting\n";
		&binoExit(1);
	    }
	    
	    # ACQMode - The acquisition mode: Imaging, LongSlit, or MultiSlit
	    #           Also "PRESET" if the script is only executing a preset

	    elsif ($cmdWord eq "acqmode") {
		if ($cmdArg eq "imaging" || 
		    $cmdArg eq "longslit" ||
		    $cmdArg eq "multislit"){
		    $acqMode = $cmdArg;
		}
		elsif ($cmdArg eq "preset") {
		    $acqMode = $cmdArg;
		    $hasMask = 1;        # an implicit mask
		}
		elsif ($cmdArg eq "slit") { # plausible alias for "longslit"
		    $acqMode = "longslit";
		}
		elsif ($cmdArg eq "mos") { # plausible alias for "multislit"
		    $acqMode = "multislit";
		}
		else {
		    print RED "** ERROR at line $numLines - Invalid ACQMode '$lineBits[1]'\n";
		    print "         Must be one of Imaging, LongSlit, MOS, or Preset\n";
		    print "         acqMODS aborting\n";
		    &binoExit(1);
		}
		$hasMode = 1;

		# We must execute in OBSMODE, as the acquisition
		# script might be following a calibration!

		$cmd[$numCmd] = "OBSMODE";
		$cmdTO[$numCmd] = $baseTO;
		$numCmd++;

	    }

	    # OBJNAME command sets the hasObject flag and the objName parameter

	    elsif ($cmdWord eq "objname") { 
		$cmd[$numCmd] = simplify($cmdStr);
		$cmdTO[$numCmd] = $shortTO;  
		$numCmd++;
		$hasObject = 1;
		$objName = stripFirst($cmdStr);
	    }

	    # PRESET commands require an extended timeout interval of 300sec 
	    #        and are followed by an IMCSLock

	    elsif ($cmdWord eq "preset") { 
		if ($cmdArg eq "active" || $cmdArg eq "guide" || 
		    $cmdArg eq "track"  || $cmdArg eq "acquire") {
		    if ($loadOnly) {
			$cmd[$numCmd] = "PRESETMODE $cmdArg";
			$cmdTO[$numCmd] = $shortTO;  
			$numCmd++;
		    }
		    else {
			if ($binoACQ) {
			    $cmd[$numCmd] = "BC SYNCPRESET";
			    $cmdTO[$numCmd] = $shortTO;
			    $numCmd++;
			} 
			$cmd[$numCmd] = simplify($cmdStr);
			$cmdTO[$numCmd] = 1.5*$presetTO;  
			$numCmd++;

			# if -p set, follow the PRESET with IMCSLOCK
			if ($runPreset) {
			    $cmd[$numCmd] = "IMCSLOCK";
			    $cmdTO[$numCmd] = $baseTO;
			    $numCmd++;
			}
		    }
		}
		else {
		    print RED "** ERROR at line $numLines - Invalid PRESET mode '$lineBits[1]'\n";
		    print "   Must be Active, Guide, Track, or Acquire\n";
		    print "   acqMODS aborting\n";
		    &binoExit(1);
		}
		$hasPreset = 1;
	    }

	    # SYNCOFFSET - binocular mode synchronized offset, wrap in
	    #              a BC prefix to direct to the IIF agent
            #              Treat like a preset for timeouts
            
	    elsif ($cmdWord eq "syncoffset") {
                if ($binoACQ) {
		    $cmd[$numCmd] = "BC SYNCOFFSET";
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

	    # POSANGLE - specify the celestial position angle of the instrument.
	    #            Inserts ROTATOR xx POSITION into the command stream

	    elsif ($cmdWord eq "posangle") {
		if (length($cmdArg) > 0) {
		    $cmd[$numCmd] = "ROTATOR $cmdArg POSITION";
		    $cmdTO[$numCmd] = $shortTO;
		    $numCmd++;
		}
		else {
		    print RED "** ERROR at line $numLines - POSANGLE requires an argument\n";
		    print "   Usage: POSANGLE angle  (celestial PA in degrees, N-to-E)\n";
		    print "   acqMODS aborting.\n";
		    &binoExit(1);
		}
	    }

	    # ACQCamera - specify the camera, red or blue, for the acquisition imaging
	    #             Alias: Camera

	    elsif ($cmdWord eq "acqcamera" || $cmdWord eq "camera") {
		if ($cmdArg eq "red" || $cmdArg eq "blue") {
		    $acqCamera = $cmdArg;
		}
		else {
		    print RED "** ERROR at line $numLines - Invalid ACQCamera '$lineBits[1]'\n";
		    print "         Must be Red or Blue\n";
		    print "         acqMODS aborting.\n";
		    &binoExit(1);
		}
		
		# Setup the instconfig command and the following imcslock, including
		# and multiobject mode focus offset

		$cmd[$numCmd] = "INSTCONFIG $acqCamera imaging";
		$cmdTO[$numCmd] = $baseTO;
		$numCmd++;
		if ($acqMode eq "multislit") {
		    if ($acqCamera eq "red") {
			$cmd[$numCmd] = "RCAMFOC STEP $offFocRed";
		    }
		    else {
			$cmd[$numCmd] = "BCAMFOC STEP $offFocBlue";
		    }
		    $cmdTO[$numCmd] = $baseTO;
		    $numCmd++;
		}
		$cmd[$numCmd] = "IMCSLOCK";
		$cmdTO[$numCmd] = $baseTO;
		$numCmd++;
		
		# Make sure multiple images from previous observations are not sticky

		$cmd[$numCmd] = "NIMGS 1";
		$cmdTO[$numCmd] = $shortTO;
		$numCmd++;

	    }

	    # ACQFilter - specify the camera filter for acquisition imaging
	    #             Alias: FILTER

	    elsif ($cmdWord eq "acqfilter" || $cmdWord eq "filter") {
		if (length($cmdArg)==0){
		    print RED "** ERROR at line $numLines - no filter ($cmdWord) specified for $lineBits[0]\n";
		    print     "     $cmdWord requires a valid filter name\n";
		    print     "     acqMODS aborting\n";
		    &binoExit(1);
		}
		if (length($acqCamera)==0) {
		    print RED "** ERROR at line $numLines - no Camera (ACQCamera) specified for $lineBits[0]\n";
		    print "         ACQCamera must preceed all other ACQxxx commands in the script\n";
		    print "         acqMODS aborting\n";
		    &binoExit(1);
		}
		$cmd[$numCmd] = "$acqCamera filter $cmdArg";
		$cmdTO[$numCmd] = $shortTO;
		$numCmd++;
	    }

	    # ACQExpTime - specify the acquisition image exposure time in seconds
	    #              Alias: EXPTIME

	    elsif ($cmdWord eq "acqexptime" || $cmdWord eq "exptime") {
		if (length($cmdArg)==0) {
		    print RED "** ERROR at line $numLines - no exposure time specified for $lineBits[0]\n";
		    print     "     $cmdWord requires an exposure time.\n";
		    print     "     acqMODS aborting\n";
		    &binoExit(1);
		}
		if (length($acqCamera)==0) {
		    print RED "** ERROR at line $numLines - No Camera (ACQCamera) specified for $lineBits[0]\n";
		    print "         ACQCamera must preceed all other ACQxxx commands in the script\n";
		    print "         acqMODS aborting\n";
		    &binoExit(1);
		}
                # Ensure that OBJECT preceeds exptime - avoids possible errors if the previous
		# IMAGETYP was something else (especially BIAS)
		if ($hasObject) {
		    $cmd[$numCmd] = "$acqCamera object $objName Acquisition";
		    $cmdTO[$numCmd] = $shortTO;
		    $numCmd++;
		} 
		else {
		    $cmd[$numCmd] = "$acqCamera object Target Acquisition";
		    $cmdTO[$numCmd] = $shortTO;
		    $numCmd++;
		}
		$cmd[$numCmd] = "$acqCamera exptime $cmdArg";
		$cmdTO[$numCmd] = $shortTO;
		$numCmd++;
	    }

	    # ACQROI - specify the CCD region-of-interest (ROI) readout mode for
            #          acquisition imaging
	    #          Alias: ROI

	    elsif ($cmdWord eq "acqroi" || $cmdWord eq "roi") {
		if (length($cmdArg)==0) {
		    print RED "** ERROR at line $numLines - no $cmdWord specified for $lineBits[0]\n";
		    print     "     $cmdWord requires a CCD region-of-interest (ROI) mode.\n";
		    print     "     acqMODS aborting\n";
		    &binoExit(1);
		}
		if (length($acqCamera)==0) {
		    print RED "** ERROR at line $numLines - no Camera (ACQCamera) specified for $lineBits[0]\n";
		    print "         ACQCamera must preceed all other ACQxxx commands in the script\n";
		    print "         acqMODS aborting\n";
		    &binoExit(1);
		}
		$cmd[$numCmd] = "$acqCamera roi $cmdArg";
		$cmdTO[$numCmd] = $shortTO;
		$numCmd++;
	    }

	    # SLITMASK - select the slitmask to use. If ACQMode=Imaging, the mask
	    #            is deployed immediately
	    #            If ACQMode={LongSlit|MultiSlit}, the mask cassette is staged
	    #            but the mask is not deployed until a SLITGO or SLITMASK IN
	    #
	    #   Setting SLITMASK NONE will make sure no mask is in the beam.
	    #
	    elsif ($cmdWord eq "slitmask") {
		if (length($cmdArg) == 0) {
		    print RED "** ERROR at line $numLines - no mask specified for $lineBits[0]\n";
		    print "         acqMODS aborting.\n";
		    &binoExit(1);
		}
		$maskName = $cmdArg;
		if ($acqMode eq "imaging") {
		    $cmd[$numCmd] = simplify($cmdStr);
		    $cmdTO[$numCmd] = $shortTO;
		    $numCmd++;
		}
		else {
		    $cmd[$numCmd] = "slitmask out";
		    $cmdTO[$numCmd] = $shortTO;
		    $numCmd++;
		    if ($maskName ne "none") {
			$cmd[$numCmd] = "mselect $maskName";
			$cmdTO[$numCmd] = $shortTO;
			$numCmd++;
		    }
		}
		$hasMask = 1;
	    }

	    # ACQGO - Take a field image with no mask (except ACQMode=Imaging)

	    elsif ($cmdWord eq "acqgo") {
		if (length($acqCamera)==0) {
		    print RED "** ERROR at line $numLines - No Camera (ACQCamera) specified for $lineBits[0]\n";
		    print "         ACQCamera must preceed all other ACQxxx commands in the script\n";
		    print "         acqMODS aborting\n";
		    &binoExit(1);
		}
		if ($acqMode ne "imaging") {
		    $cmd[$numCmd] = "slitmask out";
		    $cmdTO[$numCmd] = $shortTO;
		    $numCmd++;
		}
		$cmd[$numCmd] = "go";
		$cmdTO[$numCmd] = $baseTO;
		$numCmd++;
	    }

	    # SLITGO - Insert the mask and take a thru-slit image

	    elsif ($cmdWord eq "slitgo") {
		if (length($acqCamera)==0) {
		    print RED "** ERROR at line $numLines - No Camera (ACQCamera) specified for $lineBits[0]\n";
		    print "         ACQCamera must preceed SLITGO in the script\n";
		    print "         acqMODS aborting\n";
		    &binoExit(1);
		}
		if ($acqMode eq "imaging") {
		    print "** ERROR at line $numLines - SLITGO invalid (and silly) for ACQMode=Imaging\n";
		    print "         acqMODS aborting\n";
		    &binoExit(1);
		}
		$cmd[$numCmd] = "slitmask in";
		$cmdTO[$numCmd] = $shortTO;
		$numCmd++;
		$cmd[$numCmd] = "go";
		$cmdTO[$numCmd] = $baseTO;
		$numCmd++;
	    }
	    
	    # END command marks the end of the script

	    elsif ($cmdWord eq "end") { 
		$cmd[$numCmd] = simplify($cmdStr);
		$cmdTO[$numCmd] = 0;  
		$numCmd++;
		$hasEnd = 1;
	    }

	    # Assume what is left is a valid command - always an adventure,,,

	    else {
		$cmd[$numCmd] = simplify($cmdStr);
		$cmdTO[$numCmd] = $baseTO;  # use the base timeout
		$numCmd++;
	    }
	}
    }
}
close(MSC);

# We are out of contents, did we get an END?  If not, add an implicit
# end.

if (!$hasEnd) {
    $cmd[$numCmd] = "end";
    $cmdTO[$numCmd] = 0;  
    $numCmd++;
}

# Last block ends with the last command in the file

$endCmd = $numCmd-1;
$endBlock{$key} = $endCmd;

# Some quick checks to see if we have anything to do

if ($numCmd < 1) {
    print RED "** ERROR: No executable commands found in $acqFile\n";
    print "          acqMODS aborting\n";
    &binoExit(1);
}

if (!$hasMode) {
    print RED "** ERROR: No acquisition mode has been set in this script.\n";
    print "          Acquisition scripts must begin with ACQMode mode, where mode is one of\n";
    print "          Imaging, LongSlit, or MultiSlit\n";
    print "          acqMODS aborting\n";
    &binoExit(1);
}

if (!$hasMask) {
    print RED "** ERROR: No slitmask was specified in this script.\n";
    print "          If no mask is required, use SLITMASK NONE to make sure there is\n";
    print "          no mask in the beam.\n";
    print "          acqMODS aborting\n";
    &binoExit(1);
}

# Have we been asked to run only specific subsets of the script?

if ($runAcq) {
    if (exists($startBlock{"ACQUIRE"})) { # Do we have an Acquire: block? (fail-safe point)
	$startCmd = $startBlock{"ACQUIRE"} + 1;
	print CYAN "** Executing the target acquisition image sequence without presetting\n";
	print CYAN "** the telescope or configuring the instrument.\n";
    }
    else {
	print RED "** ERROR: -a/--acquire option requested but no Acquire: block\n";
	print "          was found in this script.\n";
	print "          acqMODS aborting\n";
	&binoExit(1);
    }
}
elsif ($loadOnly) {
    if (exists($startBlock{"TARGET"})) {
	$startCmd = $startBlock{"TARGET"} + 1;
	$endCmd = $endBlock{"TARGET"};
	print CYAN "** Loading target and guide star coordinates into the MODS control panel but\n";
	print CYAN "** not doing the preset, instrument config, or taking acquisition images.\n";
    }
    else {
	print RED "** ERROR: There is no Target: block in this acquisition script\n";
	print "          -l option failed, acqMODS aborting\n";
	&binoExit(1);
    }
}
elsif ($runPreset) {
    if (exists($startBlock{"TARGET"})) {
	$startCmd = $startBlock{"TARGET"} + 1;
	$endCmd = $endBlock{"TARGET"};
	print CYAN "** Executing the telescope preset without configuring the\n";
	print CYAN "** instrument or taking acquisition images.\n";
    }
    else {
	print RED "** ERROR: There is no Target: block in this acquisition script\n";
	print "          -p option failed, acqMODS aborting\n";
	&binoExit(1);
    }
}
elsif ($runInst) {
    if (exists($startBlock{"INSTRUMENT"})) {
	$startCmd = $startBlock{"INSTRUMENT"} + 1;
	$endCmd = $endBlock{"INSTRUMENT"};
	print CYAN "** Configuring the instrument for target acquisition without doing the\n";
	print CYAN "** telescope preset or taking acquisition images.\n";
    }
    else {
	print RED "** ERROR: There is no Instrument: block in this acquisition script\n";
	print "          -i option failed, acqMODS aborting\n";
	&binoExit(1);
    }
}

# Last checks before we commit to run...

# Is there an Exec: label?  If so, assume this is an observing
# script that must be run by execMODS and abort. 

if (exists($startBlock{"EXEC"})) {
    print RED "** ERROR: Exec: label found - is this is a regular MODS command script?\n";
    print "          acqMODS is only for executing target acquisition (.acq) scripts.\n";
    print "          Use execMODS instead -- acqMODS aborting\n";
    &binoExit(1);
}

# Similarly, is there no Acquire: block in an acquisition mode other
# than preset?  Death!

if (!exists($startBlock{"ACQUIRE"}) && $acqMode ne "preset") {
    print RED "** ERROR: No \"Acquire:\" block found in $acqFile\n";
    print "          Acquire: blocks are required for $acqMode mode acquisition scripts\n";
    print "          acqMODS aborting\n";
    &binoExit(1);
}

# Are we running the whole thing or just a specific block


#------------------------------------------------------------------------
#
# We have an array of target acquisiton commands to execute - run it...
#

if ($dryRun) {
    print CYAN "\n** Dry-run of MODS${useMODS} acquisition script $acqFile\n\n";
}
else {
    print CYAN "\n** Executing MODS${useMODS} acquisition script $acqFile\n\n";
}

$execGo = 1;
$iCmd = $startCmd - 1;

while ($execGo) {
    if ($iCmd > $endCmd) {
	closeISIS();
	print CYAN "\n** MODS${useMODS} Script $acqFile done\n";
	&binoExit(0); # exit 0;
    }

    @cmdBits = split(' ',$cmd[$iCmd]);
    $cmdWord = lc $cmdBits[0];

    # END command - terminates the script
    
    if ($cmdWord eq "end") {
	closeISIS();
	print CYAN "\n** MODS${useMODS} Script $acqFile done\n";
	&binoExit(0); # exit 0;
    }

    # SLEEP - suspend script execution and sleep for a specified time
    #         interval in seconds before resuming

    elsif ($cmdWord eq "sleep") {
	$sleepTime = $cmdBits[1];
	if ($sleepTime > 0) {
	    $| = 1;
	    print "\n";
	    for ($time=$sleepTime;$time>0;$time--) {
		printf "%c[2K",27;
		print CYAN "** MODS${useMODS} sleeping for $sleepTime sec, will wake in $time sec...\r";
		sleep 1;
	    }
	    printf "%c[2K",27;
	    print CYAN "** Sleep done\n\n";
	}
	$iCmd++;
    }

    # PRINT - print a message on the terminal screen in cyan

    elsif ($cmdWord eq "print") {
	$prStr = stripFirst($cmd[$iCmd]);
	print CYAN "\n$prStr\n";
        $iCmd++;
    }

    # PAUSE - Pause script execution and wait for user action
    #         Enter/R = Resume, X/A = eXit/Abort
    # Alias: ACQPAUSE

    elsif ($cmdWord eq "pause" || $cmdWord eq "acqpause") {
	print MAGENTA "\n** MODS${useMODS} Acquisition PAUSED, hit Enter to RESUME or X to ABORT... ";
	$kbdIn = <STDIN>;
	last unless defined $kbdIn;
	chomp($kbdIn);
	$pauseCmd = uc $kbdIn;
	if ($pauseCmd eq "X" || $pauseCmd eq "A") {
	    print RED "\n** MODS${useMODS} $acqFile target acquisition aborted after PAUSE.\n\n";
	    &binoExit(1);
	}
	print CYAN "** MODS${useMODS} Target Acquisition Resuming...\n";
	$iCmd++;
    }

    # WFSWAIT - Pause script execution and wait for active optics system to
    #           to converge.  Enter/R = Resume, X/A = eXit/Abort

    elsif ($cmdWord eq "wfswait") {
	print MAGENTA "\n** Wait for MODS${useMODS} WFS collimation to converge, then hit Enter to RESUME\n";
	print MAGENTA "   or X to ABORT... ";
	$kbdIn = <STDIN>;
	last unless defined $kbdIn;
	chomp($kbdIn);
	$pauseCmd = uc $kbdIn;
	if ($pauseCmd eq "X" || $pauseCmd eq "A") {
	    print RED "\n** MODS${useMODS} $acqFile target acquisition aborted after PAUSE.\n\n";
	    &binoExit(1);
	}
	print CYAN "\n** MODS${useMODS} Target Acquisition Resuming...\n";
	$iCmd++;
    }

    # ***** Add other non-instrument script commands here *****

    # All that's left is a data-taking system command, send it.  If cmdRetry
    # is true, it prompts for abort/retry on faults...

    else {
	if ($dryRun) {
	    print "M${useMODS}> $cmd[$iCmd]\n";
	    $iCmd++;
	}
	else {
	    print "M${useMODS}> $cmd[$iCmd]\n";
	    if (sendCommand($uiHost,"$cmd[$iCmd]",$cmdTO[$iCmd])) {
		printf "%c[2K",27;
		$outStr = stripFirst($ISIS::reply);
		print CYAN "$outStr\n";
		$iCmd++;
	    }
	    else {
		printf "%c[2K",27;
		$outStr = stripFirst($ISIS::reply);
		print RED "** $outStr\a\n";
		$errLine = $iCmd+1;
		if ($cmdRetry) {
		    print RED "** Abort, Retry, or Ignore? > ";
		    $kbdIn = <STDIN>;
		    last unless defined $kbdIn;
		    chomp($kbdIn);
		    $retryOpt = substr($kbdIn,0,1);
		    if (uc $retryOpt eq "R") {
			print CYAN "** Attempting retry of $cmd[$iCmd]\n";
		    }
		    elsif (uc $retryOpt eq "I") {
			print CYAN "** Ignoring error and continuing...\n";
			$iCmd++;
		    }
		    else {
			print RED "\n** MODS${useMODS} $acqFile aborted with errors at line $errLine command '$cmd[$iCmd]'\n";
			&binoExit(1);
		    }
		}
		else {
		    print RED "   MODS${useMODS} $acqFile terminated with errors at line $errLine command '$cmd[$iCmd]'\n";
		    &binoExit(1);
		}
	    }
	}
    }
    if ($iCmd == $numCmd) {
	$execGo = 0;
    }
}


# All done, close it down

closeISIS();
print CYAN "** MODS${useMODS} Target Acquisition Script $acqFile done\n";
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
# acqUsage - print the usage message
#

sub acqUsage {
    print "\nUsage: acqMODS [--mods1|--mods2] [options] file.acq\n";
    print "\n  Executes the MODS target acquisition (.acq) script file.acq\n";
    print "    on the named MODS (--mods1 or --mods2) - these are REQUIRED.\n";
    print "\nOptions:\n";
    print "  --bino  execute this MODS acquisition in binocular mode.\n";
    print "  -n      dry-run the script w/o execution (alias --dryrun/--chk)\n";
    print "  -a      only run the acquisition image sequence (alias --acquire)\n";
    print "  -p      only preset the telescope (alias --preset)\n";
    print "  -i      only configure the instrument (alias --inst)\n";
    print "  -l      load target coordinates but don't execute (alias --load)\n";
    print "\nSee Also: listMODS, execMODS, chkMODS\n";
    print "\n";
}

#---------------------------------------------------------------------------
#
# intHandler - SIGINT (Ctrl+C) Handler
#
# Provides for more graceful Ctrl+C interruption of script execution,
# including post-abort cleanup
#

sub intHandler {
    print RED "\n** MODS${useMODS} Acquisition script interrupted by Ctrl+C.\n";
    print CYAN "   Is this what you want to do <Y|N>? ";
    $kbdIn = <STDIN>;
    last unless defined $kbdIn;
    chomp($kbdIn);
    $abortOpt = uc $kbdIn;
    if ($abortOpt eq "Y") {
	print RED "** MODS${useMODS} Acquisition script aborting...\n";
	&cleanup;
	&binoExit(1);
    }
    else {
	print CYAN "** OK, Ignoring Ctrl+C interrupt and continuing...\n";
    }
}

#---------------------------------------------------------------------------
#
# cleanup - script post-abort clean up
#
# Actions:
#   If in an exposure, send an ABORT
#   If in an instconfig, reset the state flags
#   If in a preset or offset, clear IIF flags
#
# Goal:
#   More graceful abort that should speed fault recovery
#

sub cleanup {
    print CYAN "** MODS${useMODS} Post-abort clean up...\n";

    $lastCmd = lc $cmd[$iCmd];

    if ($lastCmd eq "go" || $lastCmd eq "red go" || $lastCmd eq "blue go") {  # abort exposures
	print CYAN "   ... aborting acquisition exposure in progress ...\n";
	$abortCmd = "$acqCamera abort";
    }
    elsif ($lastCmd eq "instconfig") { # clear instconfig state flags
	$abortCmd = "refresh instconfig";
    }
    elsif ($lastCmd eq "preset" || $lastCmd eq "offsetxy" || $lastCmd eq "offset") { # clear IIF state
	print CYAN "   ... clearing $lastCmd ...\n";
	$abortCmd = "clearstars";
    }
    else {
	$abortCmd = "";
    }

    if (length($abortCmd) > 0) {
	if (sendCommand($uiHost,$abortCmd,30)) {
	    printf "%c[2K",27;
	    $outStr = stripFirst($ISIS::reply);
	    print CYAN "$outStr\n";
	}
	else {
	    printf "%c[2K",27;
	    $outStr = stripFirst($ISIS::reply);
	    print RED "** $outStr\a\n";
	    print RED "   ... abort sequence terminated with errors.\n";
	    &binoExit(1);
	}
    }

    # Anything else?  If not, the abort sequence is complete.

    print CYAN "** MODS${useMODS} Acquisition script abort sequence completed.\n";
}

#---------------------------------------------------------------------------
#
# binoExit - Exit binocular mode
#
# Because binocular acquisitions are launched in spawned terminal
# windows, when a fatal error exits to the shell, the window will
# close before the error message may be read and acted upon by the
# observer.
#
# This function replaces the stock Perl exit() function, so that if a
# fatal error requiring exit() occurs during a binocular acqusition,
# the exit is halted at a prompt instructing the observer to press the
# Enter key to continue.
#
# It does this by checking the global $binoACQ boolean:
#   If binoACQ is False, it exits immediately.
#   If binoACQ is True, it stops and prompts to hit Enter, giving the user
#      time to read error messages.
#
 
sub binoExit {
    my $exStatus = $_[0];
    if ($binoACQ) {
	if ($exStatus != 0) {
	    print RED "\n** Binocular MODS${useMODS} acquisition aborting on fatal error.\n";
	}
	print CYAN "\n** Hit the <Enter> key to exit and close this window...";
	$kbdIn = <STDIN>;
	last unless defined $kbdIn;
	print CYAN "Bye!\n";
    }
    exit $exStatus;
}
