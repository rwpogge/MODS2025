#!/usr/bin/perl 
#
# modsCmd - execute a single MODS data-taking system command
#
# Usage:
#    modsCmd --mods1 cmdString 
#
# This perl script sends a single command to the MODS1 control
# panel for execution.  The script blocks until a reply is received.
# It knows how to handle GO image acquisition sequences.  The name
# of the receiving instrument, --mods1 or --mods2, is REQUIRED.
#
# For mixed MODS1 and MODS2 usage, aliases can be defined to
# simplify things for the user:
#
#   alias mods1Cmd 'modsCmd --mods1'
#   alias mods2Cmd 'modsCmd --mods2'
#
# See Also: 
#   execMODS and acqMODS for multi-command scripts
#   isisCmd for executing single low-level ISIS commands
#
# Author:
#    R. Pogge, OSU Astronomy Dept.
#    pogge.1@osu.edu
#    2009 Aug 28
#
# Modification History:
#   2010 Jan 8 - minor rework of the acquisition message handler [rwp/osu]
#   2011 Feb 11 - renamed modsCmd, rework for general use [rwp/osu]
#   2011 May 09 - added Getopt and -t for timeout handling [rwp/osu]
#   2012 Oct 13 - Aggressive whitespace abatement [rwp/osu]
#   2014 Apr 14 - Changes for MODS1 and MODS2 at LBT [rwp/osu]
#   2016 Jun 14 - Binocular Release for 2016B [rwp/osu]
#   2025 Sep 10 - Archon CCD controller system updates [rwp/osu]
#   2025 Oct 03 - Add LBTO NFS-mounted common path [rwp/osu]
#   2026 Jan 22 - Filter = for azcam parser [rwp/osu]
#
#---------------------------------------------------------------------------

# Custom ISIS.pm module.  These are all the places it can be on the
# LBTO mountain and OSU VLAN linux systems.

use lib "/lbt/lbto/mods/lib/modsPerl"; # LBTO mountain network common path
use lib "/home/dts/modsPerl"; # MODS data-taking system path

use ISIS;

# Standard modules we use, part of the regular Perl distribution

use Getopt::Long;  # command-line parsing
use Term::ANSIColor qw(:constants);  # color output 

$Term::ANSIColor::AUTORESET = 1;     # revert to normal after using color

# Version number.  Date should be ISO8601 format

$verNum = "2.2.2";
$verDate = "2026-01-22"; 

# ISIS network and host IDs for MODS1 and MODS2 instances

@modsHosts = ("192.168.139.130","192.168.139.230");
@modsPorts = (6600,6600);
@modsIDs = ("IS","IS");
@uiHosts = ("MC1","MC2");
@myIDs = ("PC1","PC2");
@modsNames = ("MODS1","MODS2");

# Command option defaults

$useMODS = 0;   # Default MODS instance to use, 1 or 2.
                # Setting useMODS=0 implies neither, thus
                #   REQUIRING use of  --mods1 or --mods2

$useMODS1 = ''; # (bool) if true, send commands to MODS1
$useMODS2 = ''; # (bool) if true, send commands to MODS2
$timeout  = 0;  # no user timeout on commands (block)
$showHelp = ''; # if true, show help/usage message and exit
$showVers = ''; # if true, show version info and exit
$verbose  = ''; # if true, print additional verbose output (default: concise)

# Get the options from the command line

$retCode = GetOptions('mods1' => \$useMODS1,
		      'mods2' => \$useMODS2,
		      'mods|m=i' => \$useMODS,
		      'timeout|t=i'=>\$timeout,
		      'version|V'=>\$showVers,
		      'verbose|v'=>\$verbose,
		      'help'=>\$showHelp);

if (!$retCode) {
    print RED "*** ERROR: modsCmd terminated with errors\n";
    print "           Type modsCmd --help to see usage instructions.\n"; 
    exit 1;
}

# Check the options that terminate execution (help/version)

if ($showHelp) {
    &myUsage;
    exit 0;
}

if ($showVers) {
    print "modsCmd $verNum [$verDate]\n";
    exit 0;
}

# Make sure a sensible timeout is given

if ($timeout < 0) {
    print RED "*** ERROR: Invalid timeout '$timeout', must be 0 or positive\n";
    exit 1;
}

$numArgs = $#ARGV + 1;
if ($numArgs == 0) {
    &myUsage();
    exit 1;
}

# Which MODS instrument is to be used?

if ($useMODS1) {
    $useMODS = 1;
}
elsif ($useMODS2) {
    $useMODS = 2;
}

# No instrument was specified...
if ($useMODS == 0) {
    print RED "** ERROR: You did not specify which MODS Instrument to use\n";
    print     "            --mods1 = send command to MODS1 (alias: -m 1)\n";
    print     "            --mods2 = send command to MODS2 (alias: -m 2)\n";
    print     "          modsCmd aborting\n";
    exit 2;
}
# or we got a MODS instrument to use...
elsif ($useMODS == 1 || $useMODS == 2) {
    $isisHost = $modsHosts[$useMODS-1];
    $isisPort = $modsPorts[$useMODS-1];
    $isisID = $modsIDs[$useMODS-1];
    $uiHost = $uiHosts[$useMODS-1];
    $myID = $myIDs[$useMODS-1];
    $modsName = $modsNames[$useMODS-1];
}
# or what was specified is invalid...
else {
    print RED "** ERROR: Invalid argument '$useMODS' for -m: must be 1 or 2.\n";
    print     "          modsCmd aborting\n";
    exit 2;
}

# Open up a UDP socket channel for communicating with the instrument's ISIS server.

$isOK = openISIS($isisHost,$isisPort,$isisID,$myID);

if (!$isOK) {
  print RED "ERROR: Could not connect to the $modsName data-taking system.\n";
  exit 2;
}

# Execute the command, stripping extraneous whitespace

$inStr = "@ARGV";
$cmdStr = simplify($inStr);
$cmdStr =~ s/=/ /g;

if ($verbose) {
    print GREEN "$modsName>> $cmdStr\n";
}

if (sendCommand($uiHost,"$cmdStr",$timeout)) {
    $outStr = stripFirst($ISIS::reply);
    print CYAN "$modsName>> $outStr\n";
}
else {
    $outStr = stripFirst($ISIS::reply);
    print RED "$modsName>> *** $outStr\n";
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
    print "\nUsage: modsCmd [--mods1|--mods2] [-t sec] command_string\n";
    print "\nSends command_string to the named MODS control panel.\n";
    print "  --mods1 = send command to MODS1 (alias: -m 1)\n";
    print "  --mods2 = send command to MODS2 (alias: -m 2)\n";
    print "Either --mods1 or --mods2 are *REQUIRED*\n";
    print "\nOptions:\n";
    print "  -t sec   timeout interval in seconds (alias: --timeout)\n";
    print "           Default: no timeout (block on reply)\n";
    print "\nNote:\n";
    print "  If command_string contains negative numbers (e.g., offset -20.2 5)\n";
    print "  then command_string will give \"Unknown option\" errors. In this case\n";
    print "  typing either\n";
    print "    modsCmd --mods1 'offset -20.2 5'\n";
    print "  or\n";
    print "    modsCmd --mods2 -- offset -20.2 5\n";
    print "  will avoid processing negative numbers as command options\n";
    print "\nSee Also: execMODS, acqMODS\n";
    print "\n";
}

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

