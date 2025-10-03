#
# ISIS.pm - ISIS communications module
#
# R. Pogge, OSU Astronomy Dept.
# pogge.1@osu.edu
# 2009 Aug 20
#
# Modification History:
#   2010 Dec 19: cleaned up the STATUS: printing [rwp/osu]
#   2011 Apr 14: DONE: et al. tests are now case-insensitive [rwp/osu]
#   2011 Oct 28: minor tweaks to error reporting [rwp/osu]
#   2025 Oct 03: Archon CCD controller updates [rwp/osu]
#
###########################################################################

package ISIS;
use strict;
use vars qw(@ISA @EXPORT $VERSION);

use Socket;

use Exporter;
$VERSION = 1.2.0;
@ISA = qw(Exporter);

@EXPORT = qw(openISIS sendCommand sendToISIS closeISIS);

###########################################################################
#
# Start of package functions:
#

$ISIS::verbose=0;

#
#  openISIS - open ISIS communications socket
#
#  usage: $isOK = initISIS(isisHost,isisPort,isisID,myID);
#
#  returns 1 if OK, 0 if error (Perl convention)
#

sub openISIS {
use Socket;

my $numArgs = scalar(@_);

if ($numArgs != 4) {
print "ERROR: openISIS needs 4 args: isisHost,isisPort,isisID,myID\n";
return 0;
}
$ISIS::isisHost = $_[0];
$ISIS::isisPort = $_[1];
$ISIS::isisID = $_[2];
$ISIS::myID = $_[3];

# Open the UDP client socket

socket(SOCKET,PF_INET,SOCK_DGRAM,getprotobyname("udp"))
or die "Error: cannot open socket: $!\n" ;

# Setup the sockaddr_in structure for the "server"

$ISIS::serverAddr = inet_aton($ISIS::isisHost);
$ISIS::serverPort = sockaddr_in($ISIS::isisPort,$ISIS::serverAddr);

if ($ISIS::verbose) {
print "ISIS client socket opened.\n";
}

return 1;
}

#
# sendCommand - send a command to an ISIS client and wait for completion
#
# usage: $isOK = sendCommand("hostID","message to send"[,timeout]);
#
# Returns 1 if successful, 0 otherwise.  The global
# $ISIS::reply contains the full text of any reply
#
# An optional timeout in seconds may be included in the argument list
# to fail if there is no response from hostID in timeout seconds.
# Setting timeout = 0 is equivalent to omitting the argument (block on read)
#
# See Also: sendToISIS() to send a message to the ISIS server proper
#

sub sendCommand {
use Socket;

my ($portNum, $ipAddr, $portAddr, $hostName);
my $numArgs = scalar(@_);

my ($hostID, $msgStr, $timeout);

if ($numArgs == 2) {
$hostID = $_[0];
$msgStr = $_[1];
$timeout = 0;
}
elsif ($numArgs == 3) {
$hostID = $_[0];
$msgStr = $_[1];
$timeout = $_[2];
}
else {
print "ERROR: sytnax error, usage: sendCommand(hostID,\"command string\"[,timeout])\n";
return 0;
}

my $isisMsg = "$ISIS::myID>$hostID $msgStr\r" ;

send(SOCKET,$isisMsg,0,$ISIS::serverPort) == length($isisMsg)
or die "ERROR: cannot send to $ISIS::isisHost:$ISIS::isisPort: $!\n" ;

if ($ISIS::verbose) {
print ">$hostID $isisMsg\n";
}

# ... and wait for a DONE: or ERROR: message in reply.  If
# a STATUS: message, print it and wait more

my $maxLength = 2048;

my $isDone = 0;
while ($isDone==0) {
my $rout = '';
my $rin = '';
vec($rin,fileno(SOCKET),1)=1;
my $nfound = 0;
if ($timeout > 0) {
  $nfound = select($rout=$rin,undef,undef,$timeout);
}
else {
  $nfound = select($rout=$rin,undef,undef,undef);
}
if ($nfound == 0) {
  $ISIS::reply = "$ISIS::myID> ERROR: Command '$msgStr' timed out after $timeout seconds";
  return 0;
}
else {
  if (vec($rout,fileno(SOCKET),1)) {
      $portAddr = recv(SOCKET,$ISIS::reply,$maxLength,0) 
	  or die "ERROR(sendCommand) recv: $!\n"; 

      chop($ISIS::reply); # remove \r from the reply string

      # Who sent it to us?  We only care if debugging

      if ($ISIS::verbose) {
	  ($portNum, $ipAddr) = sockaddr_in($portAddr);
	  $hostName = gethostbyaddr($ipAddr,AF_INET);
	  print "$hostID> $ISIS::reply\n";
      }
      
      # check the reply string for the words "ERROR:" etc. if badness
      
      my @chunk = split(" ",$ISIS::reply);
      my $msgType = lc $chunk[1];
      if ($msgType eq "error:" || $msgType eq "fatal:") {
	  return 0; 
      }
      elsif ($msgType eq "done:") { # command is done
	  return 1;
      }
      else { # a status or other interim message, print and continue
	  $| = 1;
#		  print "                                                      \r";
	  printf "%c[2K",27; # 27 is 33 in octal, 0o33[2K is the VT100 escape code to erase the current console line
	  if ($ISIS::verbose) {
	      print "$ISIS::reply\r";
	  } else {
	      my $statStr = stripFirst("$ISIS::reply");
	      print "$statStr\r";
	  }
      }
  }
}
}
}

#
# sendToISIS - send a message to the ISIS server proper
#
# usage: $isOK = sendToISIS("message to send");
#
# Returns 1 if successful, 0 otherwise.  The global
# $ISIS::reply contains the full text of any reply
#
# This is a convenience function that acts as a wrapper for sendCommand
# and hardwires the target client to be ISIS:isisID
#
# See also: sendCommand - to send a command and wait for a reply
#

sub sendToISIS {
  use Socket;

  my ($msg) = @_;
  my ($portNum, $ipAddr, $portAddr, $hostName);
  my $numArgs = scalar(@_);

  if ($numArgs != 1) {
    print "ERROR: sytnax error, usage: sendToISIS(\"command string\")\n";
    return 0;
  }

  sendCommand($ISIS::isisID,$msg);

}

#
# closeISIS - close the ISIS socket
#

sub closeISIS {
  close(SOCKET);
}

#
# Strip off the first word from a multi-word string
#

sub stripFirst {
    my ($prStr,$i);
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


# end of package

1;
