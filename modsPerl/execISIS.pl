#!/usr/bin/perl 
#
# execISIS - execute a MODS command script
#
# Usage:
#    execISIS [options] scriptFile
# 
# Options:
#    -n       dry-run the script w/o execution (alias --dryrun)
#    -r N     run the script starting from command number N
#               (alias --runfrom N)
#    -u       run unattended and abort on errors  (alias --unattended)
#
# Description:
#   Executes an ISIS command script containing low-level data-taking
#   system commands of the form
#       hostID command arg1 arg2 ...
#   where "hostID" is the name of an IMPv2 client or server node.
#
#   execISIS is meant to run engineering or other low-level setup
#   tasks independent of any user control panels.  It should be
#   nearly instrument independent (at least as regards OSU instruments
#   connected via an ISIS server).
#
# Option Descriptions:
#
#   The -n (alias --dryrun) option will show the commands to be
#   executed, and run internal script commands like print, pause, and
#   sleep to allow a quick check of logic or test without executing on
#   a live instrument.
#
#   The -r option allows for limited re-entrancy if no loops are used.
#   if loops are employed, all bets are off (making loops re-entrant
#   is effectively impossible with this simple script engine).
#
#   -r N              starts execution at command number N to the
#   --runfrom N         end of the script.  Use listMODS to get
#                       command numbers for a script.
#
# See Also: 
#   isisCmd for executing single data-taking system commands
#
# ToDo: 
#   Document it ...
#
# Author:
#   R. Pogge, OSU Astronomy Dept.
#   pogge@astronomy.ohio-state.edu
#   2011 May 5
#
# Modification History:
#   2011 May 05 - based on execMODS and an extension of isisCmd [rwp/osu]
#   2011 Dec 31 - fixed bugs in abort/retry/ignore code [rwp/osu]
#   2012 Oct 13 - Aggressive whitespace abatement [rwp/osu]
#   2014 Apr 18 - Updates for two MODS at the LBT [rwp/osu]
#   2016 Aug 01 - Binocular operations release for 2016B [rwp/osu]
#
#---------------------------------------------------------------------------

# Custom ISIS.pm module.  These are all the places it can be on the
# LBT and OSU lab systems

use lib "/home/modseng/modsPerl";
use lib "/lhome/dts/modsUI/modsPerl";
use lib "/home/dts/modsPerl";

use ISIS;

# Standard modules we use

use Getopt::Long;  # command-line parsing
use Term::ANSIColor qw(:constants);  # color output 

# Version number and date in ISO8601 format

$verNum  = "v2.1.0";
$verDate = "2016-08-01";

# Make sure text reverts to normal on using color

$Term::ANSIColor::AUTORESET = 1;

# Lists of MODS1/2 ISIS server and client information

@isisHosts = ("192.168.139.131","192.168.139.231");  # modsNdata server IP Addrs
@isisPorts = (6600,6600);     # modsNdata server socket ports
@isisIDs   = ("IS","IS");     # MODSn ISIS Host ID
@myIDs     = ("ISS1","ISS2"); # Our ID: ISSn = MODSn ISIS Script
@myExes    = ("execMODS1","execMODS2"); # executable IDs (tailors messages)

# Command option defaults

$startCmd = 1;  # start with command 1
$endCmd  = -1;  # -1 = run to the end
$dryRun   = ''; # if true, dry-run the script
$showHelp = ''; # if true, show help/usage message and exit
$showVers = ''; # if true, show version info and exit
$cmdAbort = ''; # if true, aborts on all errors, otherwise 
                # prompt user for abort/retry/ignore 

$useMODS  = 0;  # Default MODS to use (1 or 2).  If useMODS=0, then
                #   neither is default and --mods1 or --mods2 is REQUIRED
$useMODS1 = ''; # (bool) if true, run the script on MODS1
$useMODS2 = ''; # (bool) if true, run the script on MODS2

# Get the options from the command line

$retCode = GetOptions('runfrom|r=i'=>\$startCmd,
		      'dryrun|n'=>\$dryRun,
		      'unattended|u'=>\$cmdAbort,
		      'version|V'=>\$showVers,
		      'mods1' => \$useMODS1,
		      'mods2' => \$useMODS2,
		      'mods|m=i' => \$useMODS,
		      'help'=>\$showHelp);

if (!$retCode) {
    print RED "*** ERROR: execISIS terminated with errors\n";
    print "           Type execISIS --help to see usage instructions.\n"; 
    exit 1;
}

# Check the options that terminate execution (help/version)

if ($showHelp) {
    &myUsage;
    exit 0;
}

if ($showVers) {
    print "execISIS $verNum [$verDate]\n";
    exit 0;
}

if ($startCmd < 1) {
    print "ERROR: Argument of -r/--runfrom must be >=1\n";
    exit 1;
}

# What are the renaming command-line elements?

$numArgs = $#ARGV + 1;
if ($numArgs == 1) {
    $scriptFile = $ARGV[0];
}
elsif ($numArgs == 0) {
    &myUsage;
    exit 1;
}
else {
    print RED "** ERROR: Unrecognized command-line option(s)\n";
    &myUsage;
    exit 1;
}

# Make sure the script file exists

if (! -e $scriptFile) {
    print RED "** ERROR: ISIS script file $scriptFile not found.\n";
    exit 1;
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
    print     "          execISIS aborting\n";
    exit 2;
}
# or we got a valid MODS instrument to use...
elsif ($useMODS == 1 || $useMODS == 2) {
    $isisHost = $isisHosts[$useMODS-1];
    $isisPort = $isisPorts[$useMODS-1];
    $isisID   = $isisIDs[$useMODS-1];
    $myID     = $myIDs[$useMODS-1];
    $myExe    = $myExes[$userMODS-1];
}
# or we got something invalid...
else {
    print RED "** ERROR: Invalid argument '$useMODS' for -m: must be 1 or 2.\n";
    print     "          execISIS aborting\n";
    exit 2;
}

# Open up a UDP socket channel for communicating with an ISIS server.

$isOK = openISIS($isisHost,$isisPort,$isisID,$myID);

if (!$isOK) {
  print RED "** ERROR: Could not start ISIS connection. death!\n";
  print "          execISIS aborting\n";
  exit 2;
}

# Trap Ctrl+C gracefully

$SIG{INT} = sub { die "\n$scriptFile script interrupted by Ctrl+C\nexecISIS aborting\n" };

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
$baseTO = 120;  # base timeout in seconds

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
		    print RED "** ERROR at line ${numLines} - block label '$lineBits[0]' already in use\n";
		    print "         execISIS aborting\n";
		    exit 1;
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

	    # LOOP - top of a command loop.  Beware, loops are not re-entrant

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
		    print "         execISIS aborting\n";
		    exit 1;
		}
	    }

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
    print "          execISIS aborting\n";
    exit 1;
}

# Is the starting command number actually in the script?

if ($startCmd > $numCmd) {
    print RED "** ERROR: Cannot start running script at command $startCmd\n";
    print "          There are only $numCmd commands in the $scriptFile\n";
    print "          execISIS aborting\n";
    exit 1;
}

#----------------------------------------------------------------
#
# We have an array of commands to execute - run it...
#

if ($dryRun) {
    print BLUE "\n** Dry-run of $scriptFile\n\n";
}
else {
    print BLUE "\n** Executing $scriptFile\n\n";
}

$curLoop = -1;
$execGo = 1;
$iCmd = $startCmd - 1;

while ($execGo) {
    if ($iCmd > $endCmd) {
	closeISIS();
	print BLUE "\n** Script $scriptFile done\n";
	exit 0;
    }

    @cmdBits = split(' ',$cmd[$iCmd]);
    $cmdWord = lc $cmdBits[0];

    # END command - terminates the script
    
    if ($cmdWord eq "end") {
	closeISIS();
	print BLUE "\n** Script $scriptFile done\n";
	exit 0;
    }

    # END_LOOP - bottom of the current loop

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

    # GOTO - branch to a label - beware: branching complicates re-entrancy

    elsif ($cmdWord eq "goto") {
	$key = lc $cmdBits[1];
	if (exists($startBlock{$key})) {
	    $iCmd = $startBlock{$key};  # address of first command after "label:"
	}
	else {
	    print RED "** ERROR: unknown label \"$key\"\n";
	    print "   execISIS aborting\n";
	    exit 1;
	}
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
	print BLUE "$prStr\n";
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
	    exit 1;
	}
	print BLUE "Script Execution Resuming...\n\n";
	$iCmd++;
    }

    # All that's left is a low-level data-taking system command
    # of the form
    #     hostID command arg1 arg2 ...
    # split off the ISIS command hostID and command string and
    # send it on its way

    else {
	if ($dryRun) {
	    print ">$cmd[$iCmd]\n";
	    $iCmd++;
	}
	else {
	    print ">$cmd[$iCmd]\n";
	    $hostID = $cmdBits[0];
	    $cmdStr = stripFirst($cmd[$iCmd]);
	    if (sendCommand($hostID,"$cmdStr",$cmdTO[$iCmd])) {
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
		    exit 2;
		}
		else { # prompt the user to abort/retry/ignore in response to errors
		    print RED "** Abort, Retry, or Ignore? > ";
		    $kbdIn = <STDIN>;
		    last unless defined $kbdIn;
		    chomp($kbdIn);
		    $retryOpt = substr($kbdIn,0,1);
		    if (uc $retryOpt eq "R") {
			print BLUE "** Attempting retry of $cmd[$iCmd]\n";
		    }
		    elsif (uc $retryOpt eq "I") {
			print BLUE "** Ignoring error and continuing...\n";
			$iCmd++;
		    }
		    else {
			print RED "\n** $scriptFile aborted with errors at line $errLine command '$cmd[$iCmd]'\n";
			exit 2;
		    }
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
print BLUE "** Script $scriptFile done\n";

exit 0;

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
    print "\nUsage: execISIS [--mods1|--mods2] [options] scriptFile\n";
    print "\n  Executes the ISIS script named scriptFile on the named MODS\n";
    print "    (--mods1 or --mods2) - these are REQUIRED.\n";
    print "\n  scriptFile is an ASCII text file containing low-level\n";
    print "    IMPv2 protocol data-taking system commands.\n";
    print "\nOptions:\n";
    print "  -n       dry-run the script w/o execution (alias --dryrun)\n";
    print "  -r N     run the script starting from command number N (alias --runfrom)\n";
    print "              (use listMODS to get command numbers\n";
    print "  -u       run unattended and abort on errors  (alias --unattended)\n";
    print "\nSee Also: isisCmd\n";
    print "\n";
}
