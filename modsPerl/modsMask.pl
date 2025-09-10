#!/usr/bin/perl 
#
# modsMask - operate a MODS mask system for mask exchanges
#
# usage: modsMask mods# cmdStr
#
# where:
#    mods# = MODS instrument (1 or 2)
#    cmdStr = slit mask command string
#
# Operate the mask cassette system in MODS1 or MODS2 when exchanging
# slit masks. User specifies the instrument number (1 or 2), then
# one of these commands:
#   status - which mask is in the select position and it status
#   reset  - reset the mask system
#   mask N - select mask N, but do not insert it into the beam
#   out    - extract the mask from the beam (alias: stow)
#   insert - insert the mask into the beam (alias: in)
#
# Flags:
#   -V  - print current revision number of the script (alias --version)
#   -h   - print a brief usage/help message (alias --help)
#   
# ToDo:
#   Document it on the LBTO MODS wiki
#
# Author:
#   R. Pogge, OSU Astronomy Dept.
#   pogge@astronomy.ohio-state.edu
#   2011 Oct 30
#
# Modification History:
#   2014 Apr 18 - Formal v2 for MODS1+2 at LBT [rwp/osu]
#   
#---------------------------------------------------------------------------

# Custom ISIS module - these are all the places it could be on machines
# at LBT and the OSU instrument lab

use lib "/home/modseng/modsPerl";
use lib "/lhome/dts/modsUI/modsPerl";
use lib "/home/dts/modsPerl";

use ISIS;

# Additional Perl modules we use, part of the standard distribution

use Getopt::Long;  # command-line parsing
use Term::ANSIColor qw(:constants);  # color output 

# Version number and date, date in ISO8601 format

$verNum = "2.0.1";
$verDate = "2014-04-18";

# Local ISIS server and IE server info for MODS1 and MODS2

@isisID   = ("IS","M2.ISIS");
@isisHost = ("192.168.139.131","192.168.139.231");
@isisPort = (6600,6600);
@ieHost   = ("M1.IE","M2.IE");
$myID = "MASK";
$minMask =  1;
$maxMask = 24;

# Make sure text reverts to normal on using color

$Term::ANSIColor::AUTORESET = 1;

# Command option defaults

$showHelp = ''; # if true, show help/usage message and exit
$showVers = ''; # if true, show version info and exit

# Get the options from the command line

GetOptions('version|V|v'=>\$showVers,
           'help'=>\$showHelp);

# Check the options that terminate execution (help/version)

$numArgs = $#ARGV + 1;

if ($showHelp) {
    &myUsage;
    exit 0;
}

if ($showVers) {
    print "modsMask $verNum [$verDate]\n";
    exit 0;
}

if ($numArgs == 0) {
    &myUsage;
    exit 0;
}

# Process the remaining command arguments

$argStr = "@ARGV";
$numArgs = $#ARGV + 1;
if ($numArgs > 1) {
    @argBits = split(' ',$argStr);
    $modsID = $argBits[0];
    $cmdStr = stripFirst($argStr);
}
else {
    print RED "** ERROR: Invalid command syntax\n";
    &myUsage;
    exit 2;
}

# Trap Ctrl+C gracefully

$SIG{INT} = sub { print RED "modsMask interrupted by Ctrl+C\n"; die "aborting\n" };

# Validate modsID: must be 1 or 2

if ($modsID == 1 || $modsID == 2) {
    # using MODS1 or MODS2
}
else {
    print RED "** ERROR: Invalid MODS \#$modsID, must be 1=MODS1 or 2=MODS2\n";
    &myUsage;
    print RED "   modsMask aborting\n";
    exit 2;
}

# Look at the command and see if we got something valid to do

@cmdBits = split(' ',$cmdStr);
$cmdWord = lc $cmdBits[0]; # lowercase command verb
$cmdArg  = lc $cmdBits[1]; # lowercase first argument

# STATUS - Query current mask status

if ($cmdWord eq "status") {
    $maskCmd = "slitmask";
    $timeOut = 10;
}

# RESET - reset the slitmask system

elsif ($cmdWord eq "reset") {
    $maskCmd = "slitmask reset";
    $timeOut = 60;
    print BLUE "** Resetting the MODS$modsID slit mask system...\n";
}

# INSERT - insert the slit mask.  Alias: "IN"

elsif ($cmdWord eq "insert" || $cmdWord eq "in") {
    $maskCmd = "slitmask in";
    $timeOut = 20;
    print BLUE "** Inserting the current MODS$modsID mask into the beam...\n";
}

# OUT - extract the slit mask.  Alias: "stow"

elsif ($cmdWord eq "out" || $cmdWord eq "stow") {
    $maskCmd = "slitmask out";
    $timeOut = 20;
    print BLUE "** Extracting the current MODS$modsID mask into the storage cassette...\n";
}

# MASK - mask number follows.  If no argument, queries mask

elsif ($cmdWord eq "mask") {
    if (length($cmdArg) == 0) { # no argument, query
	$maskCmd = "slitmask";
	$timeOut = 10;
    }
    else {
	if ($cmdArg < $minMask || $cmdArg > $maxMask) {
	    print RED "** ERROR: Invalid mask position '$cmdArg', must be ${minMask}..${maxMask}\n";
	    print RED "   modsMask aborting.\n";
	    exit 2;
	}
	$maskCmd = "mselect $cmdArg";
	$timeOut = 40;
	print BLUE "** Selecting MODS$modsID mask cassette position ${cmdArg}...\n";

    }
}

# An internal command for debugging

elsif ($cmdWord eq "hosts") {
    print "Instrument: MODS$modsID\n";
    print " ISIS Host: $isisID[$modsID-1] on host $isisHost[$modsID-1]:$isisPort[$modsID-1]\n";
    print "   IE Host: $ieHost[$modsID-1]\n";
    print "  clientID: $myID\n";
    exit 0;
}

# Unrecognized command - squawk and exit

else {
    print RED "** ERROR: Unrecognized command '$cmdStr'\n";
    print RED "   modsMask aborting.\n";
    exit 2;
}

# Execute the command

# Open up a UDP socket channel for communicating with an ISIS server.

$isOK = openISIS($isisHost[$modsID-1],$isisPort[$modsID-1],$isisID[$modsID-1],$myID);

if (!$isOK) {
  print RED "** ERROR: Could not connect to the MODS$modsID control system.\n";
  print RED "   Are the MODS$modsID ISIS server, IE, and AGw servers running?\n";
  print RED "   modsMask aborting.\n";
  exit 2;
}

if (sendCommand($ieHost[$modsID-1],"$maskCmd",$timeOut)) {
    $replyStr = stripFirst($ISIS::reply);
    print BLUE "MODS$modsID - $replyStr\n";
}
else {
    $replyStr = stripFirst($ISIS::reply);
    print RED "** $replyStr\n";
    print RED "   modsMask aborting\n";
    exit 2;
}

# All done, close it down

closeISIS();

exit;

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
    print "\nUsage: modsMask modsID cmdStr\n";
    print "\nwhere: modsID = 1 (MODS1) or 2 (MODS2)\n";
    print "       cmdStr = mask command to execute\n";
    print "\nMask Commands:\n";
    print "   status - report the status of the mask system\n";
    print "   reset  - reset the mask system, leaving mask 1 at the load port.\n";
    print "   mask N - position mask N at the load port, N=1..24\n";
    print "   in     - insert the active mask into the beam (alias: insert)\n";
    print "   out    - extract the active mask into the storage cassette (alias: stow)\n";
    print "\nFlags:\n";
    print "   -V   - print version info and exit (alias: --version)\n";
    print "   -h   - print help info and exit (alias: --help)\n";
    print "\nExamples:\n";
    print "   modsMask 1 status  -  report the status of the MODS1 mask system\n";
    print "   modsMask 2 mask 4  -  position MODS2 mask 4 at the cassette load port\n";
    print "\n";
}
