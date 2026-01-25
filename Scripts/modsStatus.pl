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
#   2025 Oct 4 - added MODS subsystem power and dewar T/P display [rwp/osu]
#   2025 Oct 13 - check if on a TMUX session [rwp/osu]
#   2025 Dec 31 - added dataMan systemd service [rwp/osu]
#
#   2026 Jan 25 - adding remote ccdMon and azcam monitoring [rwp/osu]
#
#---------------------------------------------------------------------------

use strict;
use warnings;
use Curses;
use POSIX qw(strftime);

# Version number and date - dates in ISO8601 format

my $verNum  = "v2.0.0";
my $verDate = "2026-01-25";

# Who are we and where are we running?

my $hostID = `hostname`;
chomp($hostID);
my $modsID = uc($hostID);
my $userID = (getpwuid($<))[0];

# Remote server keys and hosts

my $keyPath = "/lbt/lbto/mods/keys/id_ecdsa_mods";
my $remPath = "/home/dts/bin";

# Archon server hosts

my $redHost = "192.168.139.131";
my $blueHost = "192.168.139.132";

if ($hostID eq "mods2") {
    $redHost = "192.168.139.231";
    $blueHost = "192.168.139.232";
}

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
    init_color(COLOR_CYAN,0,1000,1000);
    init_color(COLOR_BLUE,0,0,1000);
    # Define color pairs: (pair_id, foreground_color, background_color)
    init_pair(1, COLOR_WHITE, COLOR_BLACK);   # White text on black background
    init_pair(2, COLOR_YELLOW, COLOR_BLACK);  # Yellow text on black background
    init_pair(3, COLOR_GREEN, COLOR_BLACK);   # Green text on black background
    init_pair(4, COLOR_RED, COLOR_BLACK);     # Red text on black background
    init_pair(5, COLOR_CYAN, COLOR_BLACK);    # Cyan text on black background
    init_pair(6, COLOR_BLUE, COLOR_BLACK);    # Blue text on black background
} else {
    addstr("Your terminal does not support colors.\n");
    getch();
    endwin();
    exit;
}

# State Table Colors

my $headCol  = COLOR_PAIR(1); # white on black headings

# server tables

my $dataCol  = COLOR_PAIR(2); # yellow on black is the data ID or warning
my $normCol  = COLOR_PAIR(3); # green on black is normal
my $alertCol = COLOR_PAIR(4); # red on black is alert/fault

# power state table

my $onCol = COLOR_PAIR(3); # green on black is On
my $offCol = COLOR_PAIR(6); # blue on black is Off (barely visible - no reliable gray in tmux)

# process lists and headers

my $userProcs = ["mmcServer","agwServer","redIMCS","blueIMCS","modsUI"]; # unix names
my $userHead = ["Process","Status","UserID"];
my $ccdHead = ["","Blue","Red"];

my $sysdProcs = ["isis","lbttcs","modsenv","modsDD","dataMan"];
my $sysdHead = ["Service","Status","Restart"];

my @subSystems = qw(UTIL LLB GCAM WFS IEB_B HEB_B Archon_B IGPower_B IEB_R HEB_R Archon_R IGPower_R);
#my $pwrHead = ["Inst","Blue","Red"];
my $pwrHead = ["","Blue ","Red "];

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

    #------------------------------------------------
    #
    # Instrument server status
    #

    my $svcRow = 1;
    
    attron($headCol);
    addstr($svcRow,1, "$modsID instrument server:");

    my $colNum = 1;
    $svcRow += 1;
    foreach my $hdrStr (@$userHead) {
        addstr($svcRow,$c0+($colNum-1)*$cpad, $hdrStr);
        $colNum++;
    }
    attroff($headCol);
 
    my $procUser = "";
    
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
            $procUser = `ps eo user h -C $procID`;
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

    #------------------------------------------------
    #
    # Instrument server systemd service status
    #
    
    my $sysdRow = 9;   # was $svcRow + 1;

    attron($headCol);
    addstr($sysdRow, 1, "$modsID systemd services:");
    $sysdRow += 1;
    $colNum = 1;
    foreach my $hdrStr (@$sysdHead) {
        addstr($sysdRow, $c0+($colNum-1)*$cpad, $hdrStr);
        $colNum++;
    }
    attroff($headCol);

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

    #------------------------------------------------
    #
    # MODS power status
    #
    # returned by vueinfo, in order
    #   iub llb gcam wfs ieb_b heb_b archon_b ig_b ieb_r heb_r archon_r ig_r
    #
    
    my $pwrRow = 17;  # was $sysdRow + 1;

    my $pwrStates = `/usr/local/bin/vueinfo pstatus`;
    chomp($pwrStates);
    my @sysPower = split(' ',$pwrStates);
    
    attron($headCol);
    addstr($pwrRow, 1, "$modsID subsystem power:");
#    $pwrRow += 1;
#    $colNum = 1;
#    foreach my $hdrStr (@$pwrHead) {
#        addstr($pwrRow, $c0+($colNum-1)*$cpad, $hdrStr);
#        $colNum++;
#    }
#    attroff($headCol);

    # power status of subsystems

    $pwrRow++;

    foreach my $clrRow ((0,1,2,3)) {
        move($pwrRow+$clrRow,$c0);
        clrtoeol();
    }
    $colPair = $dataCol;
    my $subSys = "";
    foreach my $pCol ((0,1,2)) {
        foreach my $pRow ((0,1,2,3)) {
            my $iPwr = $pRow + 4*$pCol;
            if ($sysPower[$iPwr] eq "On") {
                $colPair = $onCol;
            } elsif ($sysPower[$iPwr] eq "Off") {
                $colPair = $offCol;
            } elsif ($sysPower[$iPwr] eq "Fault") {
                $colPair = $alertCol;
            } else {
                $colPair = $headCol;
            }
            attron($colPair);
	    addstr($pwrRow+$pRow,$c0+$pCol*$cpad,$subSystems[$iPwr]);
            attroff($colPair);
        }
    }

    #------------------------------------------------
    #
    # MODS CCD system status
    #
    
    my $ccdRow = 23; # was $sysdRow + 1;

    # Dewar temperatures and pressures read using vueinfo
    
    my $dewTempPres = `/usr/local/bin/vueinfo dewars`;
    chomp($dewTempPres);
    my @dewTP = split(' ',$dewTempPres);

    # MODS channel azcam and modsCCD status read remotely
    
    my $blueCCD = `ssh -i $keyPath mods\@$blueHost $remPath/ccdStatus`;
    chomp($blueCCD);
    my @blueStat = split(' ',$blueCCD);
    
    my $redCCD = `ssh -i $keyPath mods\@$redHost $remPath/ccdStatus`;
    chomp($redCCD);
    my @redStat = split(' ',$redCCD);

    # heading and parameters:
    
    attron($headCol);
    addstr($ccdRow,1,"$modsID CCD status:");
    $colNum = 1;
    foreach my $hdrStr (@$ccdHead) {
        addstr($ccdRow, $c0+($colNum-1)*$cpad, $hdrStr);
        $colNum++;
    }
    attroff($headCol);

    attron($dataCol);
    addstr($ccdRow+1,$c0,"azcam-mods");
    addstr($ccdRow+2,$c0,"modsCCD");
    addstr($ccdRow+3,$c0,"Dewar Temp");
    addstr($ccdRow+4,$c0,"Dewar Pres");
    attroff($dataCol);

    # azcam server status

    $colPair = $normCol;

    if ($blueStat[2] eq "running") {
	$colPair = $normCol;
	$procUser = $blueStat[3];
    } else {
	$colPair = $alertCol;
	$procUser = "";
    }
    move($ccdRow+1,$c0+1*$cpad);
    clrtoeol();
    attron($colPair);
    addstr($ccdRow+1,$c0+1*$cpad,$blueStat[2]);
    attroff($colPair);
    attron($dataCol);
    addstr($ccdRow+1,$c0+1*$cpad+9,$procUser);
    attroff($dataCol);

    if ($redStat[2] eq "running") {
	$colPair = $normCol;
	$procUser = $redStat[3];
    } else {
	$colPair = $alertCol;
	$procUser = "";
    }
    attron($colPair);
    addstr($ccdRow+1,$c0+2*$cpad,$redStat[2]);
    attroff($colPair);
    attron($dataCol);
    addstr($ccdRow+1,$c0+2*$cpad+9,$procUser);
    attroff($dataCol);

    # modsCCD agent status
    
    if ($blueStat[5] eq "running") {
	$colPair = $normCol;
	$procUser = $blueStat[6];
    } else {
	$colPair = $alertCol;
	$procUser = "";
    }
    move($ccdRow+2,$c0+1*$cpad);
    clrtoeol();
    attron($colPair);
    addstr($ccdRow+2,$c0+1*$cpad,$blueStat[5]);
    attroff($colPair);
    attron($dataCol);
    addstr($ccdRow+2,$c0+1*$cpad+9,$procUser);
    attroff($dataCol);

    if ($redStat[5] eq "running") {
	$colPair = $normCol;
	$procUser = $redStat[6];
    } else {
	$colPair = $alertCol;
	$procUser = "";
    }
    attron($colPair);
    addstr($ccdRow+2,$c0+2*$cpad,$redStat[5]);
    attroff($colPair);
    attron($dataCol);
    addstr($ccdRow+2,$c0+2*$cpad+9,$procUser);
    attroff($dataCol);

    # Temperature in C

    attron($normCol);
    move($ccdRow+3,$c0+1*$cpad);
    clrtoeol();
    addstr($ccdRow+3,$c0+1*$cpad+2,"$dewTP[0] C");
    addstr($ccdRow+3,$c0+2*$cpad+2,"$dewTP[2] C");
    attroff($normCol);

    # Pressure in torr

    attron($normCol);
    move($ccdRow+4,$c0+1*$cpad);
    clrtoeol();
    addstr($ccdRow+4,$c0+1*$cpad,"$dewTP[1] torr");
    addstr($ccdRow+4,$c0+2*$cpad,"$dewTP[3] torr");
    attroff($normCol);

    #------------------------------------------------
    #
    # Bottom row: info update date/time
    #
    
    my $timeRow = 29; # was $ccdRow + 6;
    my @now = localtime;
    my $dateNow = strftime "%Y-%m-%d %H:%M:%S", @now;
    attron($headCol);
    if (defined $ENV{'TMUX'}) {
	addstr($timeRow, 1, "Updated: $dateNow");
    }
    else {
	addstr($timeRow, 1, "Updated: $dateNow (Ctrl+C to exit status)");
    }
    attroff($headCol);
    addstr($timeRow,0,"");
        
    # update the screen and sleep for $cadence
    
    refresh();

    if ($cadence > 0) {
	sleep($cadence);
    }
} 

attroff(A_BOLD);

# bottom of loop

endwin();  # Clean up curses
