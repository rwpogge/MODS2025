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

printf("\n%s server status:\n",$modsID);

printf("%12s %12s %12s\n","Process","Status","UserID");

foreach my $procID (@$userProcs) {
    my $isUp = `ps h -C $procID`;
    chomp($isUp);
    if (length($isUp) > 0) {
	my $procUser = `ps eo user h -C $procID`;
	chomp($procUser);
	printf("%12s %12s %12s\n",$procID,"running",$procUser);
    } else {
	printf("%12s %12s\n",$procID,"stopped");
    }
}

printf("\n%s systemd services:\n",$modsID);

printf("%12s %12s %12s\n","Service","Status","Restart");

foreach my $sysID (@$sysdProcs) {
    my $isActive = `systemctl is-active $sysID`;
    chomp($isActive);
    my $isEnabled = `systemctl is-enabled $sysID`;
    chomp($isEnabled);
    printf("%12s %12s %12s\n",$sysID,$isActive,$isEnabled);
}

my $pwrInfo = `/usr/local/bin/vueinfo power`;
print "\n$pwrInfo";

#
# Dewar status: temperature and pressures
#
# Retrieve using `vueinfo dewars` --> blueTemp bluePres redTemp redPres
#

my $dewTempPres = `/usr/local/bin/vueinfo dewars`;
chomp($dewTempPres);
my @dewTP = split(' ',$dewTempPres);

print "\n$modsID dewars:\n";
print "  Blue: T=$dewTP[0] C  P=$dewTP[1] torr\n";
print "   Red: T=$dewTP[2] C  P=$dewTP[3] torr\n";

exit();
