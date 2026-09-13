#!/usr/bin/perl 
#
# intTest - test interrupt handling in tmux context
#
#---------------------------------------------------------------------------

# Standard modules we use

use Getopt::Long;  # command-line parsing
use Term::ANSIColor qw(:constants);  # color output 

# Trap Ctrl+C gracefully

$SIG{INT} = 'intHandler';

# pretty simple, just waits forever

while (1) {
    sleep 1;
}

#---------------------------------------------------------------------------
#
# intHandler - SIGINT (Ctrl+C) Handler
#
# Provides for more graceful Ctrl+C interruption of script execution,
# including post-abort cleanup
#
# fixed error in keyboard input processing [rwp/osu]
#
sub intHandler {
    print RED "\n** intTest interrupted by Ctrl+C.\n";
    print CYAN "   Is this what you want to do <Y|N>? ";
    $kbdIn = <STDIN>;
    last unless defined $kbdIn;
    chomp($kbdIn);
    $abortOpt = substr($kbdIn,0,1);
    if (uc $abortOpt eq "Y") {
	print CYAN "\n** intTest aborting...\n";
        print CLEAR "\n";
	exit 0;
    }
    else {
	print CYAN "** OK, Ignoring Ctrl+C interrupt and continuing...\n";
    }
}
