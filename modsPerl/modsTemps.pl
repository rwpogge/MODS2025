#!/usr/bin/perl 
#
# modsTemps - query MODS environmental sensors
#
# usage: modsTemps mods# cmdStr
#
# where:
#    mods# = MODS instrument (1 or 2)
#    cmdStr = slit mask command string
#
# Operate the AGw stage in MODS1 or MODS2. Primarily designed
# for use by the telescope operator in order to readily operate
# the MODS stage or prepare a MODS for technical observing with
# the AGw unit cameras when the instrument proper is used.
#
# It requires that the MODS instrument server (IE) and AGw
# stage server are running with an ISIS message-passing server.
#
# Commands:
#   glycol - Instrument glycol supply/return temps and pressures.
#   red    - Red CCD dewar temperatures and vacuum pressure
#   blue   - Blue CCD dewar temperatures and vacuum pressure
#   inst   - Unfiltered full instrument estatus
#   ...
#
# Flags:
#   -V   - print current revision number of the script (alias --version)
#   -h   - print a brief usage/help message (alias --help)
#   
# ToDo:
#   Document it on the LBTO MODS wiki
#
# Author:
#   R. Pogge, OSU Astronomy Dept.
#   pogge@astronomy.ohio-state.edu
#   2011 Nov 1
#
# Modification History:
#   2011 Nov 02 - Minor tweaks to the beta release [rwp/osu]
#   2013 Jan 19 - Added MODS2 support [rwp/osu]
#   2014 Apr 18 - Formal release with modsPerl v2 upon MODS2 installation
#                  at LBT [rwp/osu]
#
#---------------------------------------------------------------------------

# Custom ISIS module - these are all the places it could be on machines
# at LBT and the OSU instrument lab

use lib "/lhome/dts/modsUI/modsPerl";
use lib "/home/dts/modsPerl";
use lib "/home/MODSeng/modsPerl";

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
@redHost  = ("M1.RC","M2.RC");
@blueHost = ("M1.BC","M2.BC");
$myID = "ENV";

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
    print "modsTemps $verNum [$verDate]\n";
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

$SIG{INT} = sub { print RED "modsTemps interrupted by Ctrl+C\n"; die "aborting\n" };

# Validate modsID: must be 1 or 2

if ($modsID == 1 || $modsID == 2) {
    # using MODS1 or MODS2
}
else {
    print RED "** ERROR: Invalid MODS \#$modsID, must be 1=MODS1 or 2=MODS2\n";
    &myUsage;
    print RED "   modsTemps aborting\n";
    exit 2;
}

# Look at the command and see if we got something valid to do

@cmdBits = split(' ',$cmdStr);
$cmdWord = lc $cmdBits[0]; # lowercase command verb
$cmdArg1 = lc $cmdBits[1]; # lowercase first argument
$cmdArg2 = lc $cmdBits[2]; # lowercase second argument

# GLYCOL - Query MODS instrument glycol status

if ($cmdWord eq "glycol") {
    $envCmd = "estatus";
    $envHost = $ieHost[$modsID-1];
    $timeOut = 10;
}

# RED - Query MODS Red Channel CCD dewar environmental status

elsif ($cmdWord eq "red") {
    $envCmd = "estatus";
    $envHost = $redHost[$modsID-1];
    $timeOut = 15;
}

# BLUE - Query MODS Blue Channel CCD dewar environmental status

elsif ($cmdWord eq "blue") {
    $envCmd = "estatus";
    $envHost = $blueHost[$modsID-1];
    $timeOut = 15;
}

# INST - Unfiltered full instrument estatus

elsif ($cmdWord eq "inst") {
    $envCmd = "estatus";
    $envHost = $ieHost[$modsID-1];
    $timeout = 10;
}

elsif ($cmdWord eq "hosts") {
    print "\nInstrument: MODS$modsID\n";
    print " ISIS Host: $isisID[$modsID-1] on host $isisHost[$modsID-1]:$isisPort[$modsID-1]\n";
    print "   IE Host: $ieHost[$modsID-1]\n";
    print "  Red Host: $redHost[$modsID-1]\n";
    print " Blue Host: $blueHost[$modsID-1]\n";
    print "  clientID: $myID\n\n";
    exit 0;
}

# Unrecognized command - squawk and exit

else {
    print RED "** ERROR: Unrecognized command '$cmdStr'\n";
    print RED "   modsTemps aborting.\n";
    exit 2;
}

# Execute the command

# Open up a UDP socket channel for communicating with an ISIS server.

$isOK = openISIS($isisHost[$modsID-1],$isisPort[$modsID-1],$isisID[$modsID-1],$myID);

if (!$isOK) {
  print RED "** ERROR: Could not connect to the MODS$modsID control system.\n";
  print RED "   Are the MODS$modsID ISIS server, IE, and AGw servers running?\n";
  print RED "   modsTemps aborting.\n";
  exit 2;
}

if (sendCommand($envHost,"$envCmd",$timeOut)) {
    $replyStr = stripFirst($ISIS::reply);
    @dataBits = split(' ',$replyStr);
    %envData = ();
    $numVals = 0;
    foreach $word (@dataBits) {
	if (index($word,"=")>0) {
	    @wordBits = split(/=/,$word);
	    $envData{$wordBits[0]} = $wordBits[1];
	    $numVals++;
	}
    }
    if ($numVals > 0) {
	if ($cmdWord eq "red") {
	    print "\nMODS$modsID Red CCD:\n";
	    printf "       CCD Temp: %.1f C\n",$envData{'CCDTEMP'};
	    printf "     Dewar Temp: %.1f C\n",$envData{'DEWTEMP'};
	    printf "     Dewar Pres: %.2e torr\n", $envData{'DEWPRES'};
	    printf "       HEB Temp: %.1f C\n", $envData{'HEBTEMP'};
	    printf "HEB Glycol Temp: %.1f C in/%.1f C out\n\n", $envData{'CTEMPIN'},$envData{'CTEMPOUT'};
	}
	elsif ($cmdWord eq "blue") {
	    print "\nMODS$modsID Blue CCD:\n";
	    printf "       CCD Temp: %.1f C\n",$envData{'CCDTEMP'};
	    printf "     Dewar Temp: %.1f C\n",$envData{'DEWTEMP'};
	    printf "     Dewar Pres: %.2e torr\n", $envData{'DEWPRES'};
	    printf "       HEB Temp: %.1f C\n", $envData{'HEBTEMP'};
	    printf "HEB Glycol Temp: %.1f C in/%.1f C out\n\n", $envData{'CTEMPIN'},$envData{'CTEMPOUT'};
	}
	elsif ($cmdWord eq "glycol") {
	    print "\nMODS$modsID Instrument Glycol:\n";
	    print "   Supply Temp: $envData{'GSTEMP'} C\n";
	    print "          Pres: $envData{'GSPRES'} psi\n";
	    print "   Return Temp: $envData{'GRTEMP'} C\n";
	    print "          Pres: $envData{'GRPRES'} psi\n\n";
	}
	else {
	    $edump = stripFirst($replyStr);
	    print "\nMODS$modsID $edump\n\n";
	}
    }
    else {
	print "\nMODS$modsID - $replyStr\n\n";
    }
}
else {
    $replyStr = stripFirst($ISIS::reply);
    print RED "** $replyStr\n";
    print RED "   modsTemps aborting\n";
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
    print "\nUsage: modsTemps modsID cmdStr\n";
    print "\nwhere: modsID = 1 (MODS1) or 2 (MODS2)\n";
    print "       cmdStr = command to execute\n";
    print "\nMask Commands:\n";
    print "   red    - report MODSn Red CCD dewar temperatures and vacuum pressure\n";
    print "   blue   - report MODSn Blue CCD dewar temperatures and vacuum pressure\n";
    print "   glycol - report MODSn instrument glycol supply and return status\n";
    print "   inst   - raw, unfiltered instrument ESTATUS (engineering only)\n";
    print "\nFlags:\n";
    print "   -V   - print version info and exit (alias: --version)\n";
    print "   -h   - print help info and exit (alias: --help)\n";
    print "\nExamples:\n";
    print "   modsTemps 1 red\n";
    print "       Report the MODS1 Red CCD dewar temps and pressure\n";
    print "   modsTemps 2 glycol\n";
    print "       Report the MODS2 glycol supply and return temps and pressures\n";
    print "\n";
}
