#!/usr/bin/perl 
#
# modsAGW - operate a MODS AGw stage from the Linux commmand line
#
# usage: modsAGW mods# cmdStr
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
#   status    - report the current status of the AGw stage
#   home      - home the AGw stage (alias: reset, init)
#   open      - put the MODS instrument into observing mode, opening it to the
#               sky (alias: obsmode)
#   close     - close the MODS instrument and stow the AGw stage, closing
#               it to the sky (alias: calmode)
#   getxy     - report the XY coordinates of the guide probe (in mm)
#   setxy x y - set the guide probe to the requested XY position (in mm)
#   getfilter - report the current AGw guide camera filter
#   setfilter - set the AGw guide camera filter (by number or name)
#   wfs [ON|OFF] - report or change the power state of the AGw unit's WFS
#                  camera Leach controller.
#   gcam [ON|OFF] - report or change the power state of the AGw unit's Guide
#                   camera Leach controller.
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
#   pogge.1@osu.edu
#   2011 Oct 31
#
# Modification History:
#   2014 Apr 18 - MODS1+2 hooks upon MODS2 delivery to LBT [rwp/osu]
#   2025 Sep 10 - Archon CCD controller system updates[rwp.osu]
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

$verNum = "2.1.0";
$verDate = "2025-09-10";

# Local ISIS server and IE server info for MODS1 and MODS2

@isisID   = ("IS","IS");
@isisHost = ("192.168.139.130","192.168.139.230");
@isisPort = (6600,6600);
@ieHost   = ("M1.IE","M2.IE");
$myID = "AGW";
@agwFilters = ("Clear","F525LP","B_Bessel","ND1.0");  # AGw guide camera filterse

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
    print "modsAGW $verNum [$verDate]\n";
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

$SIG{INT} = sub { print RED "modsAGW interrupted by Ctrl+C\n"; die "aborting\n" };

# Validate modsID: must be 1 or 2

if ($modsID == 1 || $modsID == 2) {
    # using MODS1 or MODS2
}
else {
    print RED "** ERROR: Invalid MODS \#$modsID, must be 1=MODS1 or 2=MODS2\n";
    &myUsage;
    print RED "   modsAGW aborting\n";
    exit 2;
}

# Look at the command and see if we got something valid to do

@cmdBits = split(' ',$cmdStr);
$cmdWord = lc $cmdBits[0]; # lowercase command verb
$cmdArg1 = lc $cmdBits[1]; # lowercase first argument
$cmdArg2 = lc $cmdBits[2]; # lowercase second argument

# STATUS - Query current AGw status

if ($cmdWord eq "status") {
    $agwCmd = "agw";
    $timeOut = 10;
}

# HOME - reset the AGw stage, parking it at the home (stowed) position
#        aliases: reset, init

elsif ($cmdWord eq "home" || $cmdWord eq "reset" || $cmdWord eq "init") {
    $agwCmd = "agw reset";
    $timeOut = 60;
    print BLUE "** Resetting/Homing the MODS$modsID AGw Stage...\n";
}

# OPEN - open MODSn to the sky for observing (alias: obsmode)

elsif ($cmdWord eq "open" || $cmdWord eq "obsmode") {
    $agwCmd = "obsmode";
    $timeOut = 60;
    print BLUE "** Opening MODS$modsID to the sky...\n";
}

# CLOSE - close MODSn and stow the AGw stage (alias: calmode)

elsif ($cmdWord eq "close" || $cmdWord eq "calmode") {
    $agwCmd = "calmode";
    $timeOut = 60;
    print BLUE "** Closing MODS$modsID and stowing the AGw stage...\n";
}

# GETXY - get the current XY position of the stage

elsif ($cmdWord eq "getxy") {
    $agwCmd = "agw";
    $timeOut = 10;
}

# GETFILTER - get the current guide camera filter (alias: getfilt)

elsif ($cmdWord eq "getfilter" || $cmdWord eq "getfilt") {
    $agwCmd = "agwfilt";
    $timeOut = 10;
}

# SETXY - move the guide probe to the specified XY position in mm

elsif ($cmdWord eq "setxy") {
    if (length($cmdArg1) == 0 || length($cmdArg2) == 0) {
	print RED "** ERROR: setxy requires 2 arguments\n";
	print RED "   modsAGW aborting.\n";
	exit 2;
    }
    $agwCmd = "gprobe $cmdArg1 $cmdArg2";
    $timeOut = 60;
}

# SETFILTER - select the AGw guide camera filter by number or name

elsif ($cmdWord eq "setfilter" || $cmdWord eq "filter") {
    if (length($cmdArg1) == 0) {
	print RED "** ERROR: $cmdword requires 1 argument\n";
	print RED "   modsAGW aborting\n";
	exit 2;
    }
    # validate the filter selection
    $filtNum = 0;
    $isValid = 0;
    $agwCmd = "";
    foreach $filt (@agwFilters) {
	$filtNum++;
	if (lc $cmdArg1 eq lc $filt) {
	    $agwCmd = "agwfilt $filtNum";
	    $isValid = 1;
	}
    }
    if ($isValid == 0) {
	print RED "** ERROR: Invalid MODS$modsID AGw guide camera filter '$cmdArg1'\n";
	print RED "   Must be one of {@agwFilters}\n";
	print RED "   modsAGW aborting.\n";
	exit 2;
    }
    $timeOut = 10;
}

# GCAM - query/set the Guide Camera CCD controller power state

elsif ($cmdWord eq "gcam") {
    if (length($cmdArg1) == 0) {
	$agwCmd = "util agc";
	$timeOut = 10;
    }
    else {
	if ($cmdArg1 eq "on") {
	    $agwCmd = "util agc on";
	    $timeOut = 15;
	}
	elsif ($cmdArg1 eq "off") {
	    $agwCmd = "util agc off";
	    $timeOut = 15;
	}
	else {
	    print RED "** ERROR: Invalid option '$cmdArg1', must be ON or OFF\n";
	    print RED "   modsAGW aborting\n";
	    exit 2;
	}
    }
}

# WFS - query/set the WFS CCD controller power state

elsif ($cmdWord eq "wfs") {
    if (length($cmdArg1) == 0) {
	$agwCmd = "util wfs";
	$timeOut = 10;
    }
    else {
	if ($cmdArg1 eq "on") {
	    $agwCmd = "util wfs on";
	    $timeOut = 15;
	}
	elsif ($cmdArg1 eq "off") {
	    $agwCmd = "util wfs off";
	    $timeOut = 15;
	}
	else {
	    print RED "** ERROR: Invalid option '$cmdArg1', must be ON or OFF\n";
	    print RED "   modsAGW aborting\n";
	    exit 2;
	}
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
    print RED "   modsAGW aborting.\n";
    exit 2;
}

# Execute the command

# Open up a UDP socket channel for communicating with an ISIS server.

$isOK = openISIS($isisHost[$modsID-1],$isisPort[$modsID-1],$isisID[$modsID-1],$myID);

if (!$isOK) {
  print RED "** ERROR: Could not connect to the MODS$modsID control system.\n";
  print RED "   Are the MODS$modsID ISIS server, IE, and AGw servers running?\n";
  print RED "   modsAGW aborting.\n";
  exit 2;
}

if (sendCommand($ieHost[$modsID-1],"$agwCmd",$timeOut)) {
    $replyStr = stripFirst($ISIS::reply);
    @dataBits = split(' ',$replyStr);
    %agwData = ();
    $numVals = 0;
    foreach $word (@dataBits) {
	if (index($word,"=")>0) {
	    @wordBits = split(/=/,$word);
	    $agwData{$wordBits[0]} = $wordBits[1];
	    $numVals++;
	}
    }
    if ($numVals > 0) {
	if ($cmdWord eq "getxy" || $cmdWord eq "setxy") {
	    print BLUE "MODS$modsID guide probe x=$agwData{'AGWXGP'} y=$agwData{'AGWYGP'}\n";
	}
	elsif ($cmdWord eq "getfilter" || $cmdWord eq "setfilter" || $cmdWord eq "filter") {
	    print BLUE "MODS$modsID guide camera filter=$agwData{'AGWFNAME'} [position $agwData{'AGWFILT'}]\n";
	}
	elsif ($cmdWord eq "gcam") {
#	    print "'$agwData{'AGC'}'\n";
	    if (uc $agwData{'AGC'} eq 'ON') {
		if (uc $agwData{'AGC_BRK'} eq 'FAULT') {
		    print RED "MODS$modsID Guide camera controller circuit breaker is tripped\n";
		}
		else {
		    print BLUE "MODS$modsID Guide camera controller is ON\n";
		}
	    }
	    else {
		print BLUE "MODS$modsID Guide camera controller is OFF\n";
	    }
	}
	elsif ($cmdWord eq "wfs") {
	    if (uc $agwData{'WFS'} eq 'ON') {
		if (uc $agwData{'WFS_BRK'} eq 'FAULT') {
		    print RED "MODS$modsID WFS camera controller circuit breaker is tripped\n";
		}
		else {
		    print BLUE "MODS$modsID WFS camera controller is ON\n";
		}
	    }
	    else {
		print BLUE "MODS$modsID WFS camera controller is OFF\n";
	    }
	}
	else {
	    print BLUE "MODS$modsID - $replyStr\n";
	}
    }
    else {
	print BLUE "MODS$modsID - $replyStr\n";
    }
}
else {
    $replyStr = stripFirst($ISIS::reply);
    print RED "** $replyStr\n";
    print RED "   modsAGW aborting\n";
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
    print "\nUsage: modsAGW modsID cmdStr\n";
    print "\nwhere: modsID = 1 (MODS1) or 2 (MODS2)\n";
    print "       cmdStr = AGw command to execute\n";
    print "\nMask Commands:\n";
    print "   status - report the status of the MODSn AGw Stage\n";
    print "   home   - home the MODSn AGw stage (stows the guide probe)\n";
    print "   open   - put MODSn into observing mode, opening it to the sky\n";
    print "   close  - close MODSn (\"calibration mode\") and stow the AGw stage\n";
    print "   getxy  - report the XY coordinates of the MODSn guide probe (in mm)\n";
    print "   setxy x y - set the MODSn guide probe to the requested XY position (in mm)\n";
    print "   getfilter - report the MODSn AGw guide camera filter\n";
    print "   setfilter filtID - set the MODSn AGw guide camera filter\n";
    print "                      must be one of {@agwFilters}\n";
    print "   wfs [ON|OFF] - report or change the power state of the MODSn AGw unit's\n";
    print "                  WFS CCD camera controller.\n";
    print "   gcam [ON|OFF] - report or change the power state of the MODSn AGw unit's\n";
    print "                   Guide Camera CCD camera controller.\n";
    print "\nFlags:\n";
    print "   -V   - print version info and exit (alias: --version)\n";
    print "   -h   - print help info and exit (alias: --help)\n";
    print "\nExamples:\n";
    print "   modsAGW 1 status\n";
    print "       Report the full unfiltered status of the MODS1 AGw stage.\n";
    print "   modsAGW 1 getxy\n";
    print "       Report the current XY position of the MODS1 AGw stage.\n";
    print "   modsAGW 2 setfilter clear\n";
    print "       Put the clear filter into the MODS2 guide camera beam.\n";
    print "   modsAGW 2 gcam\n";
    print "       Report the power state of the MODS2 guide camera CCD controller\n";
    print "\n";
}
