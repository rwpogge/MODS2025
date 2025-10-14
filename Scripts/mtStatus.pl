#!/usr/bin/perl
#
# mtStatus - text-based MODS data-taking system status
#
# usage: mtStatus
#
# Show current MODS data-taking system status in text format
# which is best for ssh sessions without terminal ID forwarding.
# This is unlike modsStatus, which assumes a tmux session.
#
# Author:
#   Rick Pogge, OSU Astronomy Dept
#   pogge.1@osu.edu
#
# Date:
#   2025 Oct 14 - new app
#
# Modification History:
#
#---------------------------------------------------------------------------

use strict;
use warnings;
use POSIX qw(strftime);

# Who are we and where are we running?

my $hostID = `hostname`;
chomp($hostID);
my $modsID = uc($hostID);
my $userID = (getpwuid($<))[0];

# process lists

my $userProcs = ["mmcServer","agwServer","redIMCS","blueIMCS","modsUI"]; # unix names
my $sysdProcs = ["isis","lbttcs","modsenv","modsDD"];

# Text based status

print "$modsID server status:\n";

print "\nProcess    Status    UserID\n";

foreach my $procID (@$userProcs) {
    my $isUp = `ps h -C $procID`;
    chomp($isUp);
    if (length($isUp) > 0) {
	my $procUser = `ps eo user h -C $procID`;
	print "$procID   running   $procUser\n";
    } else {
	print "$procID   stopped   $procUser\n";
    }
    $svcRow++;
}

print "$modsID systemd services:\n";

print "Service   Status   Restart\n";

foreach my $sysID (@$sysdProcs) {
    my $isActive = `systemctl is-active $sysID`;
    my $isEnabled = `systemctl is-enabled $sysID`;
    print "$sysID   $isActive   $isEnabled\n";
}

`/usr/local/bin/vueinfo power`;

#
# Dewar status: temperature and pressures
#
# Retrieve using `vueinfo dewars` --> blueTemp bluePres redTemp redPres
#

my $dewTempPres = `/usr/local/bin/vueinfo dewars`;
chomp($dewTempPres);
my @dewTP = split(' ',$dewTempPres);

print "$modsID dewars:\n";
print "  Blue: T=$dewTP[0] C  P=$dewTP[1] torr\n";
print "   Red: T=$dewTP[2] C  P=$dewTP[3] torr\n";

exit();
