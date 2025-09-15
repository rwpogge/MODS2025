#!/usr/bin/perl 
#
# isisCmd - send a single IMPv2 protocol message to an ISIS host
#
# usage: isisCmd hostID cmdStr
#
# where:
#    hostID = valid ISIS client or server node name
#    cmdStr = command string to send to hostID
#
# Sends a single IMPv2 message string to the named ISIS client
# via the ISIS server and wait for a reply.  This program will
# "block", waiting forever for a reply or until interrupted
# by Ctrl+C.
#
# See Also:
#   execISIS for a way to execute scripts of IMPv2 protocol commands.
#
# ToDo:
#   Document it...
#
# Author:
#   R. Pogge, OSU Astronomy Dept.
#   pogge.1@osu.edu
#   2010 Jan 6 
#
# Modification History:
#   2011 May 05 - added Getopt and color output, annotated [rwp/osu]
#   2014 Apr 14 - modifications for MODS1 and MODS2 at LBT [rwp/osu]
#   2016 Jun 15 - binocular operations release for 2016B [rwp/osu]
#   2025 Sep 10 - Archon CCD controller system updates [rwp/osu]
#
#---------------------------------------------------------------------------

# Custom ISIS module - these are all the places it could be on machines
# at LBT and the OSU instrument lab

use lib "/home/modseng/modsPerl";
use lib "/home/dts/modsPerl";

use ISIS;

# Additional Perl modules we use, part of the standard distribution

use Getopt::Long;  # command-line parsing
use Term::ANSIColor qw(:constants);  # color output 

# Version number and date, date in ISO8601 format

$verNum = "2.2.0";
$verDate = "2025-09-10";

# ISIS network and host IDs for MODS1 and MODS2 instances

@modsHosts = ("192.168.139.130","192.168.139.230");
@modsPorts = (6600,6600);
@modsIDs = ("IS","IS");
@myIDs = ("IPC1","IPC2");
@modsNames = ("MODS1","MODS2");

# Make sure text reverts to normal on using color

$Term::ANSIColor::AUTORESET = 1;

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
    print RED "*** ERROR: isisCmd terminated with errors\n";
    print "           Type isisCmd --help to see usage instructions.\n";
    exit 1;
}

# Check the options that terminate execution (help/version)

if ($showHelp) {
    &myUsage;
    exit 0;
}

if ($showVers) {
    print "isisCmd $verNum [$verDate]\n";
    exit 0;
}

# Make sure a sensible timeout is given

if ($timeout < 0) {
    print RED "*** ERROR: Invalid timeout '$timeout', must be 0 or positive\n";
    exit 1;
}

$numArgs = $#ARGV+1;
if ($numArgs == 0) {
    &myUsage;
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
    print     "          isisCmd aborting\n";
    exit 2;
}
# or we got a MODS instrument to use...
elsif ($useMODS == 1 || $useMODS == 2) {
    $isisHost = $modsHosts[$useMODS-1];
    $isisPort = $modsPorts[$useMODS-1];
    $isisID = $modsIDs[$useMODS-1];
    $myID = $myIDs[$useMODS-1];
    $modsName = $modsNames[$useMODS-1];
}
# or what was specified is invalid...
else {
    print RED "** ERROR: Invalid argument '$useMODS' for -m: must be 1 or 2.\n";
    print     "          isisCmd aborting\n";
    exit 2;
}

# Process the remaining command arguments

$argStr = "@ARGV";
@argBits = split(' ',$argStr);
$hostID = $argBits[0];
$cmdStr = stripFirst($argStr);

# Trap Ctrl+C gracefully

$SIG{INT} = sub { print RED "isisCmd interrupted by Ctrl+C\n"; die "aborting\n" };

# Open up a UDP socket channel for communicating with an ISIS server.

$isOK = openISIS($isisHost,$isisPort,$isisID,$myID);

if (!$isOK) {
  print RED "*** ERROR: Could not connect to the data-taking system.\n";
  exit 2;
}

# Execute the command

if ($verbose) {
    print GREEN "$modsName:$hostID> $cmdStr\n";
}

if (sendCommand($hostID,"$cmdStr",$timeout)) {
    $replyStr = stripFirst($ISIS::reply);
    print BLUE "$replyStr\n";
}
else {
    $replyStr = stripFirst($ISIS::reply);
    print RED "*** $replyStr\n";
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
    print "\nUsage: isisCmd --modsN hostID command_string\n";
    print "\nSends command_string to ISIS client hostID.\n"; 
    print "  --mods1 = send command to MODS1 (alias: -m 1)\n";
    print "  --mods2 = send command to MODS2 (alias: -m 2)\n";
    print "Either --mods1 or --mods2 are *REQUIRED*\n";
    print "\nOptions:\n";
    print "  -t sec   timeout interval in seconds (alias: --timeout)\n";
    print "           Default: no timeout (block on reply)\n";
    print "\nSee Also: execISIS\n";
    print "\n";
}
