#!/usr/bin/perl 
#
# listMODS - List all functional parts of a MODS script
#
# Usage:
#   listMODS scriptFile
#
# Description:
#   Lists the functional contents of scriptFile, skipping blank lines
#   and comments.  The command line numbers reported by listMODS are
#   the same numbers reported by execMODS on errors.  These are the
#   command numbers that are used by the -r (--runfrom) option in
#   execMODS.  Block labels are given without numbers to identify the
#   functional blocks (e.g., for use with -e, -f, or -b in execMODS).
#
#   Version 2 is listed as "MODS1+2", but currently there is no
#   instrument instance awareness built in.
#
# ToDo:
#   Include analysis?
#
# Author:
#   R. Pogge, OSU Astronomy Dept.
#   pogge.1@osu.edu
#   2010 Dec 18
#
# Modification History:
#   2011 Feb 11 - Print block labels without numbers [rwp/osu]
#   2011 Mar 16 - Added IMCSLOCK implicit command following INSTCONFIG
#                   so the command numbering works right. [rwp/osu]
#   2011 Aug 05 - Check for ACQMode and don't list ACQ scripts [rwp/osu]
#   2011 Dec 09 - Minor tweaks based on 2011B feedback [rwp/osu]
#   2012 Oct 13 - Aggressive whitespace abatement [rwp/osu]
#   2014 Apr 18 - MODS1+2 version [rwp/osu]
#   2016 Jun 18 - Binocular operations release for 2016B [rwp/osu]
#   2025 Sep 10 - Archon CCD controller updates [rwp/osu]
#
#---------------------------------------------------------------------------

# Version number and date - date in ISO8601 format 

$verNum  = "v2.2.0";
$verDate = "2025-09-10";

# Standard modules we use

use Term::ANSIColor qw(:constants);  # color output 
$Term::ANSIColor::AUTORESET = 1;     # revert to normal after using color

# Process command-line arguments

$numArgs = $#ARGV + 1;
if ($numArgs == 1) {
    $scriptFile = $ARGV[0];
}
else {
    print "Usage: listMODS scriptFile\n";
    exit 1;
}

# Make sure the script file exists

if (! -e $scriptFile) {
    print RED "***ERROR: script file $scriptFile not found.\n";
    exit 1;
}

# Open the script file and list it, numbering only the functional lines
# ignoring spaces and comments.

open (MSC,"<$scriptFile") || die "\nERROR: cannot open $scriptFile: $!\n";

print BLUE "\n*** Listing of $scriptFile ***\n\n";
$numCmd = 0;
$numLoops = 0;
$numLines = 0;
while (<MSC>) {
    $nextLine = $_;
    $numLines++;
    chomp($nextLine);
    $cmd = simplify($nextLine);
    if (length($cmd) > 0) { # skip blank lines
	if (substr($cmd,0,1) eq "#") {
	    # skip comments and lines filled with spaces
	}
	else {
	    @lineBits = split(' ',$cmd);
	    $cmdWord = lc $lineBits[0];
	    if (substr($cmdWord,-1) eq ":") { # label, show it
		print BLUE "$cmd\n";
	    }
	    else {
		if ($cmdWord eq "acqmode") {
		    print RED "$scriptFile is a target acquisition (.acq) script\n";
		    print RED "listMODS does not work with .acq scripts\n";
		    print "listMODS aborting\n";
		    exit 1;
		}
		$numCmd++;
		printf " %3d: $cmd\n",$numCmd;
		if ($cmdWord eq "instconfig") { # instconfig is followed by an implicit IMCSLOCK
		    $numCmd++;
		    printf " %3d: *imcslock*\n",$numCmd;
		}
		elsif ($cmdWord eq "obsmode" || $cmdWord eq "calmode") {
                    $numCmd++;
		    printf " %3d: *nimgs 1*\n",$numCmd;
		    if ($cmdWord eq "calmode") {
			$numCmd++;
			printf " %3d: *lamp off*\n",$numCmd;
		    }
                }
	    }
	}
    }
}
close(MSC);
print "\n";
exit;

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

