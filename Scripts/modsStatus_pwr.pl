#!/usr/bin/perl
#
# modsStatus - monitor MODS data-taking system status
#
# usage: modsStatus
#
# Uses Curses to show MODS system status updating at a given cadence on
# a status display.  The intent is to run in a tmux pane in a running
# on the system, but it could run standalone in any xterm logged into
# a mods server (mods1 or mods2)
#
# Usual launch method:
#   % ssh mods@mods1
#     ...
#   % mods1 <--  attach to or launch a modsAdmin tmux session
#
# mods1 starts the monitor on launch, and makes sure it is running on 
# re-attaching to an already running modsAdmin tmux session
#
# Author:
#   Rick Pogge, OSU Astronomy Dept
#   pogge.1@osu.edu
#
# Date:
#   2025 Sept 26 - new app
#
# Modification History:
#   2025 Oct 3 - first full release [rwp/osu]
#
#---------------------------------------------------------------------------

use strict;
use warnings;
use Curses;
use POSIX qw(strftime);

# Who are we and where are we running?

my $hostID = `hostname`;
chomp($hostID);
my $modsID = uc($hostID);
my $userID = (getpwuid($<))[0];

# Initialize curses

initscr();
noecho();
cbreak();
keypad(stdscr, 1);

# Check for color support and start color mode

if (has_colors()) {
    start_color();
    # brighten up colors we use
    init_color(COLOR_WHITE,1000,1000,1000);
    init_color(COLOR_YELLOW,1000,1000,0);
    init_color(COLOR_RED,1000,0,0);
    init_color(COLOR_GREEN,0,1000,0);
    # Define color pairs: (pair_id, foreground_color, background_color)
    init_pair(1, COLOR_WHITE, COLOR_BLACK);   # White text on black background
    init_pair(2, COLOR_YELLOW, COLOR_BLACK);  # Yellow text on black background
    init_pair(3, COLOR_GREEN, COLOR_BLACK);   # Green text on black background
    init_pair(4, COLOR_RED, COLOR_BLACK);     # Red text on black background
} else {
    addstr("Your terminal does not support colors.\n");
    getch();
    endwin();
    exit;
}

# color states:

my $headCol  = COLOR_PAIR(1); # white on black headings
my $dataCol  = COLOR_PAIR(2); # yellow on black is the data ID or warning
my $normCol  = COLOR_PAIR(3); # green on black is normal
my $alertCol = COLOR_PAIR(4); # red on black is alert

# process lists and headers

my $userProcs = ["mmcServer","agwServer","redIMCS","blueIMCS","modsUI"]; # unix names
my $userHead = ["Process","Status","UserID"];

my $sysdProcs = ["isis","lbttcs","modsenv","modsDD"];
my $sysdHead = ["Service","Status","Restart"];

my $subSystems = ["UTIL","LLB","GCAM","WFS","IEB","HEB","Archon","IonGauge","IEB","HEB","Archon","IonGauge"];
my $pwrHead = ["Inst","Blue","Red"];

# Update cadence

my $cadence = 1; # seconds

# table column padding

my $cpad = 16;
my $c0 = 4;

# status loop

my $keepGoing = 1;
$SIG{INT} = sub { $keepGoing = 0; };

# status loop

attron(A_BOLD);

while ($keepGoing) {

    # what time is it?

    my $svcRow = 1;
    
    attron($headCol);
    addstr($svcRow,1, "$modsID server status:"); 

    # user services header

    my $colNum = 1;
    $svcRow += 1;
    foreach my $hdrStr (@$userHead) {
        addstr($svcRow,$c0+($colNum-1)*$cpad, $hdrStr);
	$colNum++;
    }
    attroff($headCol);

    # status of user procs

    $svcRow++;
    foreach my $procID (@$userProcs) {
	attron($dataCol);
	addstr($svcRow,$c0,$procID);
	attroff($dataCol);
	
	my $isUp = `ps h -C $procID`;
	chomp($isUp);
	move($svcRow,$c0+1*$cpad);
	clrtoeol();
	if (length($isUp) > 0) {
	    attron($normCol);
	    addstr($svcRow,$c0+1*$cpad,"running");
	    attroff($normCol);
	    my $procUser = `ps eo user h -C $procID`;
	    chomp($procUser);
	    attron($dataCol);
	    addstr($svcRow,$c0+2*$cpad,$procUser);
	    attroff($dataCol);
	} else {
	    attron($alertCol);
	    addstr($svcRow,$c0+1*$cpad,"stopped");
	    attroff($alertCol);
	}
	$svcRow++;
    }

    # systemd services header

    my $sysdRow = $svcRow + 1;

    attron($headCol);
    addstr($sysdRow, 1, "$modsID systemd services:");
    $sysdRow += 1;
    $colNum = 1;
    foreach my $hdrStr (@$sysdHead) {
        addstr($sysdRow, $c0+($colNum-1)*$cpad, $hdrStr);
	$colNum++;
    }
    attroff($headCol);

    # status of systemd services

    $sysdRow++;
    my $colPair = $dataCol;
    foreach my $sysID (@$sysdProcs) {
	attron($dataCol);
	addstr($sysdRow,$c0,$sysID);
	attroff($dataCol);
	
	move($sysdRow,$c0+1*$cpad);
	clrtoeol();
	my $isActive = `systemctl is-active $sysID`;
	chomp($isActive);
	if ($isActive eq "active") {
	    $colPair = $normCol;
	} else {
	    $colPair = $alertCol;
	}	    
	attron($colPair);
	addstr($sysdRow,$c0+1*$cpad,$isActive);
	attroff($colPair);

	my $isEnabled = `systemctl is-enabled $sysID`;
	chomp($isEnabled);
	if ($isEnabled eq "enabled") {
	    $colPair = $normCol;
	} else {
	    $colPair = $alertCol;
	}	    
	attron($colPair);
	addstr($sysdRow,$c0+2*$cpad,$isEnabled);
	attroff($colPair);
	$sysdRow++
    }

    # MODS power status
    #
    # returned by vueinfo, in order
    #   iub llb gcam wfs ieb_b heb_b archon_b ig_b ieb_r heb_r archon_r ig_r
    
    my $pwrStates = `/usr/local/bin/vueinfo pstatus`;
    chomp($pwrStates);
    my @sysPower = split(' ',$pwrStates);
    
    # MODS subsystem power status header

    my $pwrRow = $sysdRow + 1;

    attron($headCol);
    addstr($pwrRow, 1, "$modsID subsystem power:");
    $pwrRow += 1;
    $colNum = 1;
    foreach my $hdrStr (@$pwrHead) {
        addstr($pwrRow, $c0+($colNum-1)*$cpad, $hdrStr);
	$colNum++;
    }
    attroff($headCol);

    # power status of subsystems

    $pwrRow++;

    my $colPair = $dataCol;
    foreach my $pCol ((0,1,2)) {
	foreach my $pRow ((0,1,2,3)) {
	    my $iPwr = $pRow + 4*$pCol;
	    if ($sysPower[$iPwr] eq "On") {
		$colPair = $normCol;
	    } elsif ($sysPower[$iPwr] eq "Off") {
		$colPair = $alertCol;
	    } else {
		$colPair = $dataCol;
	    }
	    attron($colPair);
	    addStr($pwrRow+$pRow,$c0+$pCol*$cpad,$subSystems[$iPwr]);
	    attroff($colPair);
	}
    }

    # Bottom row: info update date/time
    
    my @now = localtime;
    my $dateNow = strftime "%Y-%m-%d %H:%M:%S", @now;
    attron($headCol);
    addstr($pwfRow+1, 1, "Updated: $dateNow");
    attroff($headCol);
    addstr($pwrRow+2,0,"");
	
    # update the screen and sleep for $cadence
    
    refresh();

    sleep($cadence);
} 

attroff(A_BOLD);

# bottom of loop

endwin();  # Clean up curses
