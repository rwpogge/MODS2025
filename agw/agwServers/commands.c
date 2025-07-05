//
// commands.c - application command interpreter
//

/*!
  \file commands.c 
  \brief MODS AGW control application command interpreter functions.

  This module contains the command "action" functions called to service
  ttytool commands.  These consist of a suite of \arg "common" client
  action functions common to most ISIS clients \arg client-specific
  functions that perform the client's particular tasks \arg common
  interface routines for keyboard and ISIS socket interfaces.

  The common client commands include the following:

  These 3 commands are \b REQUIRED of all ISIS client apps:
  \arg \c cmd_quit() terminate a client session (QUIT)
  \arg \c cmd_ping() communications handshaking request (PING) from a remote ISIS node
  \arg \c cmd_pong() communications handshaking acknowledgment (PONG) from
  a remote ISIS node

  These client commands are recommended for most apps:
  \arg \c cmd_version() Report application version information (VERSION)
  \arg \c cmd_info() Report client runtime configuration (INFO, sometimes
  CONFIG in legacy apps).

  These are common commands relevant for the CLI, but generally not available
  to remote ISIS nodes (e.g., they test that the message type is EXEC:):
  \arg \c cmd_verbose() Toggle verbose output to the client console on/off (VERBOSE)
  \arg \c cmd_debug()   Toggle debugging (super-verbose) output to console on/off (DEBUG)
  \arg \c cmd_help()    List interactive client commands (HELP or ?)

  These are then followed by cmd_xxx() action functions that implement the
  various client tasks.

  At the end of this file are the template I/O handlers used by the
  command interpreter:
  \arg KeyboardCommand() Keyboard command handler (command-line interface)
  \arg SocketCommand() Socket command/message handler (client socket interface)

  \author R. Pogge, OSU Astronomy Dept. (pogge@astronomy.ohio-state.edu)
  \date 2006 Jan 11

  \par Modification History:
<pre>
   2008 Jan 28 - extensive changes for the actual AGW stage system and
                 simulation modes [rwp/osu]
   2009 Oct 24 - changes based on pre-deployment work on the AGW stage and
                 requirements for the LBT GCS interface [rdg & rwp/osu]
   2010 March 23 - Removed much of the test/debug scaffolding to make AGW
                   run smoother and faster.
   2010 Aug 10 - Added tranfermation coefficients for AGW prepared by
                 Rick Pogge. Instruction will come from GCS and/or AGW Service
                 standalone functions are in the samples directroy. 
   2010 Sept 18 - Fixed a nuisance home error message from Controllers.
                  Added the sfptoagw, agwtosfp, ccdtosfp, ccdtoagw changes
		  to code on the MODS side.

</pre>
*/

// don't do paths, use -I in the Makefile.build!

#include "instrutils.h"
#include "isl_funcs.h"
#include "islcommon.h"
#include "isl_shmaddr.h"
#include "isisclient.h"     // ISIS common client library header
#include "mmccontrol.h"     // IE control action functions header

// These are in ../include specific to the AGW code

#include "../include/client.h"         // Custom client application header
#include "../include/commands.h"       // Command action functions header
#include "../include/agw_datacodes.h"  // AGW control header

extern agwpar_t agw;       // AGW stage struct

extern char * getDateTime(void);

#include "./mlc.c"

int commandID; // Keeps the command id that identifies the mechanism.

//***************************************************************************
//
// Common client commands
//
// Commands common to most ISIS client applications are defined here.
//

//---------------------------------------------------------------------------
//
// quit command - allowed only if EXEC from remote hosts (keyboard
//                commands are always EXEC.

/*!
  \brief QUIT command - terminate the client session
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK if command executed without errors, #CMD_ERR if an error
  occurred.  On errors \e reply contains the error message.

  \par Usage:
  quit

  Executes the application QUIT command.  Only works if msgtype=EXEC,
  indicating that it is an IMPv2 executive command.  This prevents
  remote applications from prematurely terminating this application
  by sending a QUIT command unqualified by the EXEC: directive.

  \par Lock Behavior:
  Since quit requires EXEC, it overrides any locks.  Maybe dangerous...
*/

int
cmd_quit(char *args, MsgType msgtype, char *reply)
{
  if (msgtype == EXEC) {
    appClient.KeepGoing=0;
    sprintf(reply,"%s=DISABLED MODE=OFFLINE",appClient.ID);
  }
  else {
    strcpy(reply,"005 Cannot execute quit command - operation not allowed except as EXEC:");
    return CMD_ERR;
  }
  return CMD_OK;
}


//---------------------------------------------------------------------------
//
// noop - NOOP empty buff or no command was detected
//        commands are always NOOP.

/*!
  \brief CR with no command - do nothing just to return back safely.
  \param args blank string with no or just garbage
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_NOOP only with message.

  \par Usage:
  BLANK or unknown command

  Executes the application QUIT command.  Only works if msgtype=NOOP,
  indicating that it is an IMPv2 executive command.  This prevents
  remote applications from hanging or prematurely terminating.

*/

int
cmd_noop(char *args, MsgType msgtype, char *reply)
{
  if (msgtype == EXEC) {
    sprintf(reply,"%s=NOOP",appClient.ID);
    return CMD_ERR;
  }
  return CMD_NOOP;
}

//---------------------------------------------------------------------------
//
// ping - communication handshaking request
//

/*!
  \brief PING command - communication handshaking request
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK if command executed without errors, #CMD_ERR if an error
  occurred.  On errors reply contains the error message.

  This function is invoked when the client application receives a PING
  from a remote host requesting a communications handshaking reply.

  PINGs are actually handled separately in the SocketCommand() handler
  (nothing is done by the KeyboardCommand() handler) because the PONG
  sent back acknowledging the comm handshaking request is, in effect, a
  pseudo-command (implicit REQ:), not a "DONE:" response to a command
  request.  This exception to the general messaging syntax has to be
  handled carefully to prevent problems, especially to ensure backwards
  compatibility with older IMPv applications.

  \par Lock Behavior:
  PINGs ignore the lock state.

  \sa cmd_pong
*/

int
cmd_ping(char *args, MsgType msgtype, char *reply)
{
  strcpy(reply,"PONG");
  return CMD_OK;
}


//---------------------------------------------------------------------------
//
// pong - communication handshaking acknowledge
//

/*!
  \brief PONG command - communication handshaking acknowledgment
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_NOOP since PONG is a no-op pseudo command.

  For historical reasons, a "PONG" sent in acknowledgment of a software
  handshaking "PING" looks like an implicit REQ:, and hence like a
  "command request" sent to the recipient, even though it isn't.  It is,
  however, an exception to the strict messaging protocol, which is why
  it needs a separate handler.

  cmd_pong doesn't do anything except return a #CMD_NOOP (since this
  "command" must NOT result in a reply back to the sender).  In more
  sophisticated apps, we might actually use receipt of a pong to do
  something useful (e.g., help build up a node table), so at the very
  least this module works as a placeholder for future expansion.

  \par Lock Behavior:
  PONGs ignore the lock state.

  \sa cmd_ping
*/

int
cmd_pong(char *args, MsgType msgtype, char *reply)
{
  if (appClient.isVerbose)
    printf("PONG received\n");
  return CMD_NOOP;
}

//---------------------------------------------------------------------------
//
// mmcIC - MODS MicroLynx Controller Interrogator Command
//

/*!
  \brief MMCIC command - Independent command for PLC interrogation
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK

  \par Usage:
  mmcIC

  Creates an IMPv2 message (in the \e reply string) with information
  MMCIC allows a way for users or remote apps to interrogate the 
  MicroLynx controller.

  \par Lock Behavior:
  MMCIC ignores the lock state.
*/

int
cmd_mmcIC(char *args, MsgType msgtype, char *reply)
{
  int who;
  int nval;
  int len;
  int ierr=0;
  char techcmd[256];
  char cmd[40];
  char cmd1[10];
  char cmd2[10];
  char cmd3[10];
  char dummy[4096];

  who=0;
  memset(techcmd,0,sizeof(techcmd));
  memset(cmd,0,sizeof(cmd));
  memset(cmd1,0,sizeof(cmd1));

  nval=sscanf(args,"%d %s %s %s",&who,cmd1,cmd2,cmd3);
  if(nval==2) sprintf(cmd,"%s",cmd1);
  else if(nval==3) sprintf(cmd,"%s %s",cmd1,cmd2);
  else if(nval==4) sprintf(cmd,"%s %s %s",cmd1,cmd2,cmd3);
  len=strlen(cmd);

  if(strstr(args,"M#")) { // check for low-level command
    int i;
    for(i=0;args[i]!='M';i++);
    sprintf(techcmd,"%s",&args[i]);
  }

  memset(dummy,0,sizeof(dummy));
  switch (who) {
  case 0:
    StrUpper(cmd);
    if(!strcasecmp(cmd,"RESET")) cmd_reset("",EXEC,reply);
    break;
  case 1:
    if(strstr(args,"M#")) { // check for low-level command
      ierr=mlcTechCmd(appAGW.XIP,techcmd,"AGWX",dummy);
      if(ierr!=0) sprintf(reply,"-1 %s",dummy);
      else sprintf(reply,"%s",dummy);
    } else {
      ierr=sendCommand(appAGW.XIP, cmd, dummy);  // interrogate X
      sprintf(reply,"%s",dummy);
      if(ierr!=0) {
	sprintf(reply,"-1 %s",dummy);
	return CMD_ERR;
      }
    }
    break;
  case 2:
    if(strstr(args,"M#")) { // check for low-level command
      ierr=mlcTechCmd(appAGW.YIP,techcmd,"AGWY",dummy);
      if(ierr!=0) sprintf(reply,"-1 %s",dummy);
      else sprintf(reply,"%s",dummy);
    } else {
      ierr = sendCommand(appAGW.YIP, cmd, dummy);  // interrogate Y
      if(ierr!=0) sprintf(reply,"-1 %s",dummy);
      else sprintf(reply,"%s",dummy);
      sprintf(reply,"%s",dummy);
    }
    break;
  case 4:
    if(strstr(args,"M#")) { // check for low-level command
      ierr=mlcTechCmd(appAGW.FIP,techcmd,"AGWFOC",dummy);
      if(ierr!=0) sprintf(reply,"-1 %s",dummy);
      else sprintf(reply,"%s",dummy);
    } else {
      ierr=sendCommand(appAGW.FIP, cmd, dummy);  // interrogate Focus
      if(ierr!=0) sprintf(reply,"-1 %s",dummy);
      else sprintf(reply,"%s",dummy);
    }
    break;
  case 8:
    if(strstr(args,"M#")) { // check for low-level command
      ierr=mlcTechCmd(appAGW.FWIP,techcmd,"AGWFILT",dummy);
      if(ierr!=0) sprintf(reply,"-1 %s",dummy);
      else sprintf(reply,"%s",dummy);
    } else {
      ierr=sendCommand(appAGW.FWIP, cmd, dummy); // interrogate Filter Wheel
      if(ierr!=0) sprintf(reply,"-1 %s",dummy);
      else sprintf(reply,"%s",dummy);
    }
    break;
  default:
    break;
  }

  if(ierr!=0) return CMD_ERR;
  return CMD_OK;
}

//---------------------------------------------------------------------------
//
// mmccu - MODS MicroLynx Controller Control Unit
//

/*!
  \brief MMCCU command - Independent command for PLC interrogation
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK

  \par Usage:
  mmccu

  Creates an IMPv2 message (in the \e reply string) with information
  MMCCU allows a way for users or remote apps to interrogate the 
  MicroLynx controller.

  \par Lock Behavior:
  MMCCU ignores the lock state.
*/

int
cmd_mmccu(char *args, MsgType msgtype, char *reply)
{
  int who;
  int nval;
  int len;
  int ierr=0;
  char cmd[PAGE_SIZE];
  char dummy[PAGE_SIZE];

  who=0;
  memset(cmd,0,sizeof(cmd));

  nval=sscanf(args,"%d",&who);
  strcpy(cmd,&args[2]);
  cmd[strlen(cmd)]='\0';
  
  memset(dummy,0,sizeof(dummy));
  switch (who) {
  case 1:
    ierr=rawCommand(appAGW.XIP, cmd, dummy);
    if(ierr!=0) sprintf(reply,"-1 %s",dummy);
    else sprintf(reply,"%s",dummy);
    break;

  case 2:
    ierr=rawCommand(appAGW.YIP, cmd, dummy);
    if(ierr!=0) sprintf(reply,"-1 %s",dummy);
    else sprintf(reply,"%s",dummy);
    break;

  case 4:
    ierr=rawCommand(appAGW.FIP, cmd, dummy);
    if(ierr!=0) sprintf(reply,"-1 %s",dummy);
    else sprintf(reply,"%s",dummy);
    break;

  case 16:
    ierr=rawCommand(appAGW.FWIP, cmd, dummy); // interrogate Filter Wheel
    if(ierr!=0) sprintf(reply,"-1 %s",dummy);
    else sprintf(reply,"%s",dummy);
    break;
  default:
    break;
  }

  if(ierr!=0) return CMD_ERR;
  return CMD_OK;
}

//---------------------------------------------------------------------------
//
// version - report application version and compilation info
//

/*!
  \brief VERSION command - report application version and compilation info
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK

  \par Usage:
  version

  Creates an IMPv2 message (in the \e reply string) with the version
  number and any relevant compilation information (e.g., date and time
  of compilation).  VERSION allows a way for users or remote apps to
  verify the runtime version of the current application.

  Example output:
  <pre>
  Version=v1.0 CompileDate=2004-Jun-11 CompileTime=17:08:15
  </pre>

  \par Lock Behavior:
  VERSION ignores the lock state.
*/

int
cmd_version(char *args, MsgType msgtype, char *reply)
{
  
  sprintf(reply,"Version=%s CompileDate=%s CompileTime=%s",
	  APP_VERSION,APP_COMPDATE,APP_COMPTIME);
  return CMD_OK;
}

//---------------------------------------------------------------------------
//
// verbose - toggle verbose console output
//
  
/*!
  \brief VERBOSE command - toggle verbose console output on/off
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK

  \par Usage:
  verbose

  Sets the client isVerbose flag to 1 (enabled) if currently 0
  (disabled) and vis-versa.  Verbose output mode is used for basic
  client debugging information by printing extra information on the
  application console screen.  Disabling throttles verbose console
  output.  VERBOSE mode is normally disabled during normal operations.

  In general "Verbose" output refers only to client application level
  output (i.e., echoing socket message traffic, printing status update
  info, etc.).  An more chatty DEBUG mode is provided that prints more
  engineering-level info for detailed low-level system debugging.

  \par Lock Behavior:
  VERBOSE ignores the lock state.

  \sa cmd_debug
*/

int
cmd_verbose(char *args, MsgType msgtype, char *reply)
{
  if (appClient.isVerbose) {
    appClient.isVerbose = 0;
    sprintf(reply,"verbose mode disabled");
  }
  else {
    appClient.isVerbose = 1;
    sprintf(reply,"verbose mode enabled");
  }
  return CMD_OK;
}

//---------------------------------------------------------------------------
// 
// debug - toggle debugging output (super-verbose mode)
//

/*!
  \brief DEBUG command - toggle debugging (super-verbose) console output on/off
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK

  \par Usage:
  debug

  Sets the client Debug flag to 1 (enabled) if currently 0 (disabled)
  and vis-versa.  

  DEBUG mode is a super-verbose mode that spews lots of I/O chatter onto
  the application console, useful during client debugging or for
  troubleshooting.  For example, in client applications that control
  stepper motors, the full motor control chatter is echoed to the console
  during DEBUG mode to enable the user to follow the steps the system is
  (or is not) taking, watch encoder and limit switches assert (or not), 
  etc.  DEBUG is normally disabled during normal user operations.

  \par Lock Behavior:
  DEBUG ignores the lock state.

  \sa cmd_verbose
*/

int
cmd_debug(char *args, MsgType msgtype, char *reply)
{
  if (appClient.Debug) {
    appClient.Debug = 0;
    sprintf(reply,"debugging output disabled");
  }
  else {
    appClient.Debug = 1;
    sprintf(reply,"debugging output enabled");
  }
  return CMD_OK;
}

//---------------------------------------------------------------------------
//
// info - report application runtime configuration information
//

/*!
  \brief INFO command - report client application runtime information
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on successful creation of the info report, #CMD_ERR
  if errors encountered.

  \par Usage:
  info

  Creates a summary report of the current client application's runtime
  configuration as an IMPv2-compliant message string in which the
  runtime parameters are reported as keyword=value pairs.

  The format of cmd_info should be tailored specifically for the
  particular client application.  If a client controls specific
  instrument or interface functions, the state of those functions should
  be reported in the info string, making it an omnibus "what is your
  status" command.

  Example Output:
  <pre>
HostID=AGW1 HostAddr=darkstar:10702 Mode=STANDALONE Verbose -DEBUG rcfile=agw1.ini
  </pre>

  \par Lock Behavior:
  INFO ignores the lock state, except to report it.

*/

int
cmd_info(char *args, MsgType msgtype, char *reply)
{
  int i;

  /*
  // Start with the node ID, and host info
  */
  sprintf(reply,"HostID=%s HostAddr=%s:%d ",
	  appClient.ID, appClient.Host, appClient.Port);

  /*
  // If configured as an ISIS client, report this and the ISIS host:port info,
  // otherwise if standalone, report that, and the host:port of the last
  // remote host to send us something, if known.
  */
  if (appClient.useISIS) {
    sprintf(reply,"%s Mode=ISISClient ISIS=%s ISISHost=%s:%d ",reply,
	    appClient.isisID,appClient.isisHost,appClient.isisPort);
  }
  else {
    if (strlen(appClient.remHost)>0)
      sprintf(reply,"%s Mode=STANDALONE RemHost=%s:%d ",reply,
	      appClient.remHost,appClient.remPort);
    else
      strcat(reply," Mode=STANDALONE ");
  }

  /*
  // Application-specific info here as required
  */
  sprintf(reply,"%s %s LockHost=%s ",reply,
	  ((appAGW.lock) ? "+lock" : "-lock"),appAGW.lockHost);

  /*
  // Report application runtime flags last in the list
  */
  sprintf(reply,"%s %s %s",reply,
	  ((appClient.isVerbose) ? "Verbose" : "Concise"),
	  ((appClient.Debug) ? "+DEBUG" : "-DEBUG"));

  /*
  // List active comm port(s)
  */
  sprintf(reply,"%s X_ComtrolPort=%s", reply, shm_addr->MODS.commport[appAGW.XIP].Port);
  sprintf(reply,"%s Y_ComtrolPort=%s", reply, shm_addr->MODS.commport[appAGW.YIP].Port);
  sprintf(reply,"%s FOC_ComtrolPort=%s", reply, shm_addr->MODS.commport[appAGW.FIP].Port);
  sprintf(reply,"%s FW_ComtrolPort=%s", reply, shm_addr->MODS.commport[appAGW.FWIP].Port);
	 

  /*
  // Finally, report the application's runtime config info as required
  */
  sprintf(reply,"%s rcfile=%s",reply,appClient.rcFile);

  return CMD_OK;

}

//---------------------------------------------------------------------------
//
// help - print a list of available commands on the client console
//

/*!
  \brief HELP command - print a list of commands on the client console
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_NOOP on success, #CMD_ERR if help executed as a non-EXEC: command

  \par Usage:
  help [cmd]

  cmd_help is usually invoked by the HELP or ? commands.  It prints a
  list of all interactive commands on the client application's console
  screen.  It is only meant to be executed as an EXEC: message type.  If
  the command is not qualifed as EXEC:, it returns an error.

  If an argument is given, it tries to find that string in the command
  list, and if successful, prints a brief description and usage message.

  Help is meant to be simple.  It can give help on particular commands
  (really a reminder of the command's function and syntax), or a list
  of all commands.
  
  \par Lock Behavior:
  HELP ignores the lock state.
*/

int
cmd_help(char *args, MsgType msgtype, char *reply)
{
  int i, icmd, found;
  int ls;
  char argbuf[32];

  if (msgtype!=EXEC) {
    strcpy(reply,"005 Cannot exec help command - remote operation not allowed");
    return CMD_ERR;
  }

  if (strlen(args)>0) {  // we are being asked for help on a specific command
    GetArg(args,1,argbuf);

    found = 0;
    for (i=0; i<NumCommands; i++) {
      if (strcasecmp(cmdtab[i].cmd,argbuf)==0) {
	found++;
	icmd = i;
	break;
      }
    }
    if (found > 0) {
      printf("  %s - %s\n",cmdtab[i].cmd,cmdtab[i].description);
      printf("  usage: %s\n",cmdtab[i].usage);
    } 
    else {
      printf("Unknown Command '%s' (type 'help' to list all commands)\n");
    }

  }
  else { // no arguments, print the whole command list

    printf("Interactive Command Summary:\n");
    for (i=0; i<NumCommands; i++) {
      ls = strlen(cmdtab[i].usage);
      if (ls > 0) {
	if (ls < 6)
	  printf("  %s\t\t - %s\n",cmdtab[i].usage,cmdtab[i].description);

	else if (ls > 13)
	  printf("  %s - %s\n",cmdtab[i].usage,cmdtab[i].description);

	else
	  printf("  %s\t - %s\n",cmdtab[i].usage,cmdtab[i].description);
      }
    }
    printf("Command History: not available\n");
  }

  return CMD_OK;
    
}

//***************************************************************************
//
// *** APPLICATION-SPECIFIC COMMANDS BEGIN HERE ***
//

//---------------------------------------------------------------------------
//
// status - report AGW stage status (alias: getdata)
//

/*!  
  \brief STATUS command - query AGW stage status
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  status

  \par Alias:
  getdata
  
  Queries the AGW stage for the current system status, returning all
  relevant data to the calling host.  getdata is defined as an alias for
  GCS compatibility.

  Returns the parameters as keyword=value pairs ready to be inserted into
  a FITS header template.

  \par Lock Behavior:
  STATUS ignores the lock state and always responds to query requests.
*/

int
cmd_status(char *args, MsgType msgtype, char *reply)
{
  int generalStatus;
  char dummy[512];

  // Get filter number

  sprintf(reply,"AGWXGP=%0.3f AGWYGP=%0.3f AGWFOCUS=%0.3f AGWFILT=%d AGWFNAME='%s' AGWXWFS=%0.3f AGWYWFS=%0.3f AGWXS=%0.3f AGWYS=%0.3f AGWFS=%0.3f %s %s AGWXRATE=%d AGWYRATE=%d %s lockHost=%s AGWXCen=%0.3f AGWYCen=%0.3f AGWFS0=%0.3f Calib=%s",
	  appAGW.xAG,appAGW.yAG,appAGW.focOffset,
	  appAGW.filter,(appAGW.filterID)[appAGW.filter-1],
	  appAGW.xWFS,appAGW.yWFS,appAGW.xStage,appAGW.yStage,appAGW.focus,
	  ((appAGW.autoFocus) ? "+AutoFocus" : "-AutoFocus"),
	  ((appAGW.drift) ? "+Drift" : "-Drift"),
	  appAGW.xDrift, appAGW.yDrift,
	  ((appAGW.lock) ? "+lock" : "-lock"), 
	  appAGW.lockHost, appAGW.xCen,appAGW.yCen,appAGW.foc0,
	  ((appAGW.calib) ? "IN" : "OUT"), appAGW.calib);

  return CMD_OK;

}

//---------------------------------------------------------------------------
//
// open - Open all AGW or 1 AGW comm port
//

/*!  
  \brief OPEN command - open all or 1 comm port
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  open [n|all]
  
  Opens all or a named comm port.

*/
int
cmd_open(char *args, MsgType msgtype, char *reply)
{
  int i;
  int ierr;
  int unit;
  char dummy[PAGE_SIZE];
  char who_selected[24];

  strcpy(who_selected,cmdtab[commandID].cmd);
  StrUpper(who_selected);

  GetArg(args,1,argbuf);
  StrUpper(argbuf);

  if(!strcasecmp(argbuf,"ALL")) { // do them all
    for(unit=0;unit<MAX_ML-1;unit++) {
      if(shm_addr->MODS.host[unit]==1 &&
	 !strncasecmp(shm_addr->MODS.who[unit],"agw",3)) { // for agwServer
	CloseTTYPort(&shm_addr->MODS.commport[unit]);
	MilliSleep(200);
	if(i=OpenTTYPort(&shm_addr->MODS.commport[unit])<0) {
	  sprintf(reply,"%s %s=FAULT Can not open %s communications, Reason: %s", who_selected, argbuf, shm_addr->MODS.who[unit], strerror(errno));
	  return CMD_ERR;
	}
      }
      MilliSleep(100);
    }
    sprintf(reply,"%s AGW=OPEN Opened AGW COMMunication ports",who_selected);

  } else {

    GetArg(args,1,argbuf);

    unit=getMechanismID(argbuf,dummy); // Get mechanism device ID

    if(unit==-1) {
      sprintf(reply,"%s %s",who_selected,dummy);
      return CMD_ERR;
    }

    StrUpper(argbuf);
    
    CloseTTYPort(&shm_addr->MODS.commport[unit]);
    MilliSleep(100);

    if(OpenTTYPort(&shm_addr->MODS.commport[unit])<0) {
      sprintf(reply,"%s %s=FAULT Can not open %s communications, Reason: %s",who_selected, argbuf, shm_addr->MODS.who[unit], strerror(errno));

      return CMD_ERR;
    } else {
      sprintf(reply,"%s %s=OPEN %s communications are opened",
	      who_selected,argbuf,shm_addr->MODS.who[unit]);
    }
  }
  return CMD_OK;
}

//---------------------------------------------------------------------------
//
// close - Close a comm port
//

/*!  
  \brief CLOSE command - close a comm port
  re-initialize the microstep drive
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

(  \par Usage:
  close port
  
  Closes a named comm port.

*/
int
cmd_close(char *args, MsgType msgtype, char *reply)
{
  int ierr;
  int unit;
  char dummy[PAGE_SIZE];
  char who_selected[24];

  strcpy(who_selected,cmdtab[commandID].cmd);
  StrUpper(who_selected);

  GetArg(args,1,argbuf);
  StrUpper(argbuf);
  if(!strcasecmp(argbuf,"ALL")) { // do them all
    for(unit=0;unit<MAX_ML-1;unit++) {
      if(shm_addr->MODS.host[unit]==1 &&
	 !strncasecmp(shm_addr->MODS.who[unit],"agw",3));
      CloseTTYPort(&shm_addr->MODS.commport[unit]);
      MilliSleep(100);
    }
    sprintf(reply,"%s AGW=CLOSED agwServer communications are closed",who_selected);
    
  } else {

    GetArg(args,1,argbuf);
    unit=getMechanismID(argbuf,dummy); // Get mechanism device ID

    if(unit==-1) {
      sprintf(reply,"%s %s",who_selected,dummy);
      return CMD_ERR;
    }

    CloseTTYPort(&shm_addr->MODS.commport[unit]);
    MilliSleep(100);

    StrUpper(args);
    sprintf(reply,"%s %s=CLOSED %s communications are closed",who_selected, args, shm_addr->MODS.who[unit]);

  }

  return CMD_OK;
}

//---------------------------------------------------------------------------
//
// reset - reload the runtime config file and re-initialize the AGW table
//

/*!  
  \brief RESET command - reload the runtime config paramters and
  re-initialize the AGW stage parameters
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  reset
  
  Calls loadconfig() to re-read the runtime config file, restoring all
  initial client parameters, then resets the AGW stage.

  This provides the client application with a warm restart facility.

  \par Lock Behavior:
  If the lock is set, it only permits RESET by the lock holder, or if the RESET
  was issued as an executive override command (type EXEC:).  Note that RESET
  unlocks the AGW, even if a lock was held before the reset.  Any calling process
  resetting a locked AGW will have to re-assert that lock if required after
  the reset request completes.

*/

int
cmd_reset(char *args, MsgType msgtype, char *reply)
{
  int i;
  int ierr;
  int unit;
  char dummy[79];

  /*
  // Re-initialize the AGW Stage and Filter Wheel
  */
  appAGW.xStage = 0;
  appAGW.yStage = 0;
  appAGW.focus  = 0;
  
  ierr=sendCommand(appAGW.YIP,"INITIAL",dummy);
  shm_addr->MODS.pos[appAGW.XIP]=0.0;
  
  ierr=sendCommand(appAGW.XIP,"INITIAL",dummy);
  shm_addr->MODS.pos[appAGW.YIP]=0.0;
  
  ierr=sendCommand(appAGW.FIP,"INITIAL",dummy);
  shm_addr->MODS.pos[appAGW.FIP]=0.0;
  
  appAGW.filter=mlcBitsBase10(appAGW.FWIP,"PRINT IO 22,IO 21",dummy)+1;
  
  shm_addr->MODS.pos[appAGW.FWIP]=(float)appAGW.filter;
  
  sprintf(reply,"097 re-initializing AGW Stage");
  
  return CMD_OK;
  
}

//---------------------------------------------------------------------------
//
// setfilter/getfilter - set/query the Acquisition/Guide camera filter
//

/*!  
  \brief SETFILTER and GETFILTER commands - set/query the Acquistion/Guide camera filter
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  setfilter [1..4]<br>
  getfilter

  Selects an Acquisition/Guide camera filter (1 thru 4).  getfilter returns the
  current A/G camera filter selection.

  \par Lock Behavior:
  Query is permitted regardless of the lock status.  Set is only allowed
  by the lockHost if the lock is set.
*/

int
cmd_filter(char *args, MsgType msgtype, char *reply)
{
  char FWaxis[24];
  char argbuf[32];
  char dummy[79];
  int doQuery = 0;
  int reqPos = 0;
  int curPos = 0;
  int options=0;
  int Nmove = 0;
  int ierr;
  int cnt;

  /*
  // Examine the command line 'args' and parse if needed
  */

  if (strlen(args)<=0) {   // no arguments, means query position
    doQuery = 1;

  } else {

    if (agwIsLocked(&appAGW,reply)) { // check the lock state
      strcpy(reply,"008 AGW locked");
      return CMD_ERR;
    }


    GetArg(args,1,argbuf); // get filter number
    reqPos = atoi(argbuf);

    if (reqPos < 1 || reqPos > appAGW.numFilters) {
      sprintf(reply,"025 Invalid filter '%s', must be 1..%d",argbuf,appAGW.numFilters);
      return CMD_ERR;
    }
    doQuery = 0;
  }

  /*
  // If this is only a query, or the filter is already in position,
  // return status
  */
  if (doQuery || (reqPos == appAGW.filter) ||
      mlcBitsBase10(appAGW.FWIP,"PRINT IO 22,IO 21",dummy)+1==reqPos) {
    sprintf(reply,"AGWFILT=%d AGWFNAME='%s'",
	    appAGW.filter,(appAGW.filterID)[appAGW.filter-1]);
    return CMD_OK;
  }
  
  GetArg(args,2,argbuf);   // get options
  options = atoi(argbuf);

  appAGW.filter = reqPos;

  sprintf(FWaxis,"TARGNUM=%d",reqPos-1);
  ierr = sendTwoCommand(appAGW.FWIP,FWaxis,"BEGIN",dummy);

  if(ierr!=0) {
    sprintf(reply,"-1 %s",dummy);
    return CMD_ERR;
  }

  if (options==OPT_BLOCK) {
    for(cnt=0;cnt<40000;cnt++) {
      rawCommand(appAGW.FWIP,"PRINT MVG",dummy);
      if(!strcasecmp(dummy,"FALSE")) break;
    }

  }
  appAGW.filter=mlcBitsBase10(appAGW.FWIP,"PRINT IO 22,IO 21",dummy)+1;
  
  shm_addr->MODS.pos[appAGW.FWIP]=(float)appAGW.filter;

  if(appAGW.filter!=reqPos) {
    sprintf(reply,"105 AGWFILT=FAULT AGWFNAME=FAULT Requested filter %d, but sensors show filter %d position",reqPos, appAGW.filter);
    return CMD_ERR;
  }

  sprintf(reply,"AGWFILT=%d AGWFNAME='%s'",
	  appAGW.filter,(appAGW.filterID)[appAGW.filter-1]);

  return CMD_OK;
}

//---------------------------------------------------------------------------
//
// setxy/getxy - set/query the AGW stage absolute X,Y position
//

/*!  
  \brief SETXY/GETXY commands - set/query the AGW stage absolute X,Y position
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  setxy [x y]<br>
  getxy

  \par Units:
  x,y in millimeters

  Sets the AGW stage guide probe to an absolute X,Y position in
  millimeters in the AGW coordinate system where (X,Y)=(0,0) is the home
  (stowed) position of the AGW stage.

  If given with no arguments, it reports the current AGW stage position
  in millimeters.  Actual linear resolution is #AGW_XYRES millimeters

  User gives the desired guide probe field-center position (XAG,YAG)
  in millimeters, this routine translates that to the corresponding stage
  (XS,YS) and focus position corresponding to that field center

  If AutoFocus is enabled, it computes the nominal autofocus value at
  the requested position, otherwise it uses the current focus value.

  \par Lock Behavior:
  Query is permitted regardless of the lock status.  Set is only allowed by the
  lockHost if the lock is set.

  \sa cmd_dxy, cmd_home
*/

int
cmd_xy(char *args, MsgType msgtype, char *reply)
{
  char argbuf[132];
  char Yaxis[64];
  char Xaxis[64];
  char Faxis[64];
  char dummy[132];

  int doQuery = 0;
  int yBitState = 0;
  int xBitState = 0;
  int focBitState = 0;
  int options=0;
  int ierr = 0;
  int i, break_cnt ;

  double reqXAG = 0;
  double cur_XAG = 0;
  double reqXS = 0;
  double cur_XS = 0;
  double dX = 0;

  double reqYAG = 0;
  double cur_YAG = 0;
  double reqYS = 0;
  double cur_YS = 0;
  double dY = 0;

  double cur_foc = 0;
  double reqFoc = 0;
  double dF = 0;

  double movetime = 0;

  double movevec = 0;

  memset(dummy,0,sizeof(dummy));

  /*
  // Examine the command line 'args' and parse if needed
  */

  if (strlen(args)<=0) {   // no arguments, means query position
    doQuery = 1;

  } else {

    /* 
    // Check for Power Failure variable PWRFAIL in PLC ladder code
    // for X and Y.
    */
    ierr = rawCommand(appAGW.XIP,"PRINT PWRFAIL",dummy);
    if(atoi(dummy)) {
      sprintf(reply,"201 AGWXS=PWRFLR Power Failure, (reset)");
      return CMD_ERR;
    }

    ierr = rawCommand(appAGW.YIP,"PRINT PWRFAIL",dummy);
    if(atoi(dummy)) {
      sprintf(reply,"201 AGWYS=PWRFLR Power Failure, (reset)");
      return CMD_ERR;
    }

    /* 
    // Check for Cable Failures on X,Y, and Focus Stage
    */
    if(mlcBitsBase10(appAGW.XIP,"PRINT IO 22,IO 21",dummy)==3) {
      sprintf(reply,"102 X Stage cable disconnected or sensor fault");
      return CMD_ERR;
    }

    if(mlcBitsBase10(appAGW.YIP,"PRINT IO 22,IO 21",dummy)==3) {
      sprintf(reply,"103 Y Stage cable disconnected or sensor fault");
      return CMD_ERR;
    }

    if(mlcBitsBase10(appAGW.FIP,"PRINT IO 22,IO 21",dummy)==3) {
      sprintf(reply,"104 Focus cable disconnected or sensor fault");
      return CMD_ERR;
    }

    if (agwIsLocked(&appAGW,reply)) {  // check the lock state
      strcpy(reply,"008 AGW locked");

      return CMD_ERR;
    }

    if(appAGW.calib==1) { 
      strcpy(reply,"201 Calibration Tower is in the beam, move disallowed");

      return CMD_ERR;
    }

    /*
    // Validate the requested position
    */

    GetArg(args,1,argbuf); // get X axis parameter
    reqXAG = atof(argbuf);

    if (reqXAG < AGW_AGXMIN || reqXAG > AGW_AGXMAX) {
      sprintf(reply,"022 Invalid guide probe X position '%0.3f', must be %d..%d", 
	      reqXAG,
	      AGW_AGXMIN,
	      AGW_AGXMAX);

      return CMD_ERR;
    }

    GetArg(args,2,argbuf); // get Y axis parameter
    reqYAG = atof(argbuf);

    if (reqYAG < AGW_AGYMIN || reqYAG > AGW_AGYMAX) {
      sprintf(reply,"017 Invalid guide probe Y position '%0.3f', must be %d..%d", 
	      reqYAG,
	      AGW_AGYMIN,
	      AGW_AGYMAX);

      return CMD_ERR;
    }

    doQuery = 0;
  }

  /*
  // Query the current stage position so subsequent actions are not
  // based on stale information
  */
  agwGetPos(&appAGW);

  GetArg(args,3,argbuf); // get options
  options = atoi(argbuf);

  if(doQuery) { // if only a query, return the current focus and stage position

    sprintf(reply,"AGWXGP=%0.3f AGWYGP=%0.3f AGWFOCUS=%0.3f AGWXWFS=%0.3f AGWYWFS=%0.3f AGWXS=%0.3f AGWYS=%0.3f AGWFS=%0.3f AGWFS0=%0.3f",
	    appAGW.xAG,appAGW.yAG,appAGW.focOffset,
	    appAGW.xWFS,appAGW.yWFS,
	    appAGW.xStage,appAGW.yStage,appAGW.focus,
	    appAGW.foc0);
    
    shm_addr->MODS.pos[appAGW.XIP]=appAGW.xStage;
    shm_addr->MODS.pos[appAGW.YIP]=appAGW.yStage;
    shm_addr->MODS.pos[appAGW.FIP]=appAGW.focus*2.0;
    
    return CMD_OK;
  }

  /*
  // Are we already there?  If so, report our position and return
  */
  dX = reqXAG - appAGW.xAG;
  dY = reqYAG - appAGW.yAG;
  dF = reqFoc - cur_foc;

  movevec = sqrt(dX*dX + dY*dY);
  if (movevec == 0) {
    sprintf(reply,"AGWXGP=%0.3f AGWYGP=%0.3f AGWFOCUS=%0.3f AGWXWFS=%0.3f AGWYWFS=%0.3f AGWXS=%0.3f AGWYS=%0.3f AGWFS=%0.3f AGWFS0=%0.3f",
	      appAGW.xAG,appAGW.yAG,appAGW.focOffset,
	      appAGW.xWFS,appAGW.yWFS,
	      appAGW.xStage,appAGW.yStage,appAGW.focus,
	      appAGW.foc0);
      
    return CMD_OK;
  }

  /*
  // Convert the requested probe position (XAG,YAG) into Stage (XS,YS,Foc)
  */
  gpToAGW(&appAGW,reqXAG,reqYAG,&reqXS,&reqYS,&reqFoc);

  ierr=0; // start with no errors.
  
  sprintf(Xaxis,"XPOS=%f",-(reqXS)); // X axis motion
  shm_addr->MODS.reqpos[appAGW.XIP]=-(reqXS);
  ierr = sendTwoCommand(appAGW.XIP,Xaxis,"MOVETARG",dummy) + ierr;

  sprintf(Faxis,"FPOS=%f",reqFoc); // Focus mechanism motion
  ierr = sendTwoCommand(appAGW.FIP,Faxis,"MOVETARG",dummy) + ierr;
  shm_addr->MODS.reqpos[appAGW.FIP] = reqFoc;

  sprintf(Yaxis,"YPOS=%f",-(reqYS)); // Y axis motion
  shm_addr->MODS.reqpos[appAGW.YIP]=-(reqYS);
  ierr = sendTwoCommand(appAGW.YIP,Yaxis,"MOVETARG",dummy);

  if(ierr!=0) {
    sprintf(reply,"098 One or more MOVE errors found on X,Y, and Focus");
    return CMD_ERR;
  }

  for(i=0,break_cnt=0;i<40000;i++) {
    if(break_cnt==2) { // Monitor Y Stage
      rawCommand(appAGW.YIP,"PRINT MVG",dummy);
      if(!strcasecmp(dummy,"FALSE")) break_cnt++;
      shm_addr->MODS.pos[appAGW.YIP]=positionToShrMem(appAGW.YIP,dummy);

    } else if(break_cnt==1) {  // Monitor X Stage
      rawCommand(appAGW.XIP,"PRINT MVG",dummy);
      if(!strcasecmp(dummy,"FALSE")) break_cnt++;
      shm_addr->MODS.pos[appAGW.XIP]=positionToShrMem(appAGW.XIP,dummy);
      
    } else if(break_cnt==0) {  // Monitor Focus Stage
      rawCommand(appAGW.FIP,"PRINT MVG",dummy);
      if(!strcasecmp(dummy,"FALSE")) break_cnt++;
      shm_addr->MODS.pos[appAGW.FIP]=positionToShrMem(appAGW.FIP,dummy);
      
    }

    if(break_cnt==3) break;
    MilliSleep(10);
  }
  
  sprintf(reply,"AGWXGP=%0.3f AGWYGP=%0.3f AGWFOCUS=%0.3f AGWXWFS=%0.3f AGWYWFS=%0.3f AGWXS=%0.3f AGWYS=%0.3f AGWFS=%0.3f AGWFS0=%0.3f",
	  appAGW.xAG,appAGW.yAG,appAGW.focOffset,
	  appAGW.xWFS,appAGW.yWFS,
	  appAGW.xStage,appAGW.yStage,appAGW.focus,
	  appAGW.foc0);
    
  shm_addr->MODS.pos[appAGW.XIP]=appAGW.xStage;
  shm_addr->MODS.pos[appAGW.YIP]=appAGW.yStage;
  shm_addr->MODS.pos[appAGW.FIP]=appAGW.focus*2.0;

  return CMD_OK;
}

//---------------------------------------------------------------------------
//
// setfocus/getfocus - set/query the absolute camera focus
//

/*!  
  \brief SETFOCUS/GETFOCUS command - set/QUERY the absolute camera focus in millimeters
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  setfocus [f]<br>
  getfocus

  \par Units:
  f in millimeters, typically to 0.01mm precision, though 0.001mm (1 micron)
  precision is reported 

  setfocus sets the AGW stage focus to +/-f millimeters relative to the
  nominal best-focus surface.  This change is persistent for all subsequent
  moves.  Note that the stage Y-axis actuator will move in reflex to preserve
  the physical location of the guide-probe field center relative to the 
  instrument focal plane.  The range of focus travel permitted is +/-
  #AGW_DFOCMAX millimeters (e.g., +/-5mm relative to the nominal focus).

  getfocus returns the current guide probe focus value in millimeters to
  0.001mm precision.

  The focus motion is along the Y-axis, so we have to move the camera
  stage in reflex in the Y-axis direction (dYS = -dFocus) to make sure
  the probe center doesn't move relative to the MODS science field.

  This routine returns 3 values as key=value pairs: AGWFOCUS is the
  camera focus datum in millimeters, AGWFS is the stage focus actuator
  position in millimeteres, and AGWFS0 is the nominal focus zero point
  (AGWFS at science-field center) in millimeters.  All are reported to
  0.001mm, although note that the AGW stage focus drive motor has a
  full-step resolution of 0.01mm, so we oversample a full step by a
  factor of 10.

  \par Lock Behavior:
  Query is permitted regardless of the lock status.  Set is only allowed
  by the lockHost if the lock is set.

*/

int
cmd_focus(char *args, MsgType msgtype, char *reply)
{

  char Yaxis[24];
  char Faxis[24];
  char argbuf[24];
  char dummy[132];
  int doQuery = 0;
  int options=0;
  int ierr=0;
  int i, break_cnt;

  double reqFoc = 0;
  double cur_foc = 0;
  double dF = 0;

  double curXS, curYS, curFS; // current stage actuator data
  double xGP, yGP;            // current guide probe position

  double newFS, newYS;

  memset(dummy,0,sizeof(dummy));

  /*
  // Examine the command line 'args' and parse if needed
  */

  /* Check for Power Failure variable PWRFAIL in PLC ladder code */
  //ierr = rawCommand(appAGW.FIP,"PRINT PWRFAIL",dummy);
  //  if(atoi(dummy)) {
  //    sprintf(reply,"AGWFS=PWRFLR Power Failure, (reset)");
  //    return CMD_ERR;
  //  }

  if (strlen(args)<=0) { // no arguments, means query focus
    doQuery = 1;
  }
  else {

    if (agwIsLocked(&appAGW,reply)) { // check the lock state
      strcpy(reply,"008 AGW locked");
      return CMD_ERR;
    }

    // Check the calibration tower, disallow if the tower is in
    if(appAGW.calib==1) { 
      strcpy(reply,"201 Calibration Tower is in the beam, move disallowed");
      return CMD_ERR;
    }

    GetArg(args,1,argbuf); // get the command-line arguments
    reqFoc = (double)atof(argbuf);

    // validate the requested focus change

    if (reqFoc < -AGW_DFOCMAX || reqFoc > AGW_DFOCMAX) {
      sprintf(reply,"029 Invalid AGW camera focus '%0.3f' requested, must be %d..%d", 
	      reqFoc, -AGW_DFOCMAX, AGW_DFOCMAX);

      return CMD_ERR;
    }
    doQuery = 0;
  }

  // First, query the current AGW stage positions, so we don't act on
  // stale information

  agwGetPos(&appAGW);

#ifdef DEBUG
  printf("(cmd_focus) current: xs=%0.3f ys=%0.3f fs=%0.3f f0=%0.3f focus=%0.3f\n",
	 appAGW.xStage,appAGW.yStage,appAGW.focus,appAGW.foc0,appAGW.focOffset);
#endif

  GetArg(args,2,argbuf); // get options
  options = atoi(argbuf);

  // If this is only a query, report the focus value and return now.
  // We Return the current focus datum as AGWFOCUS, the stage focus
  // value AGWFS and the nominal zeropoint AGWFS0.  The last two are
  // for reference the first is the focus datum associated with
  // get/setfocus.  Also report these data if the requested focus
  // datum is the current datum

  if (doQuery || appAGW.focOffset == reqFoc) {

    sprintf(reply,"AGWFOCUS=%0.3f AGWFS=%0.3f AGWFS0=%0.3f",
	    appAGW.focOffset,appAGW.focus,appAGW.foc0);
    return CMD_OK;
  }

  // Note the current stage XS,YS,FS data

  curXS = appAGW.xStage;
  curYS = appAGW.yStage;
  curFS = appAGW.focus;

  // and the current acqusition/guide camera probe coordinates.
  // The change in focus must not change XGP and YGP

  xGP = appAGW.xAG;
  yGP = appAGW.yAG;

  // Before we can change anything, make sure that the change
  // will not drive either the YS or FS stage actuators beyond
  // their operating range.

  dF = reqFoc - appAGW.focOffset;
  newFS = curFS + dF;
  newYS = curYS - dF;
  if (newFS < AGW_FOCMIN || newFS > AGW_FOCMAX) {
    sprintf(reply,"101 setfocus %0.3f would move the AGW stage focus actuator out of range, move disallowed",reqFoc);
    return CMD_ERR;
  }
  if (newYS < AGW_YSMIN || newYS > AGW_YSMAX) {
    sprintf(reply,"101 setfocus %0.3f would move the AGW stage Y-axis actuator out of range, move disallowed",reqFoc);
    return CMD_ERR;
  }

  // We have a safe move, store the new focus offset...

  appAGW.focOffset = reqFoc;

  // and apply it

#ifdef DEBUG
  printf ("(cmd_focus) focus change requested: reqFoc=%0.3f newFS=%0.3f newYS=%0.3f\n",
	  reqFoc,newFS,newYS);
#endif

  ierr=0; // start with no errors
  sprintf(Faxis,"FPOS=%f",newFS);
  ierr = sendTwoCommand(appAGW.FIP,Faxis,"MOVETARG",dummy) + ierr; // Focus
  shm_addr->MODS.reqpos[appAGW.FIP] = newFS; 

  // Y axis
  sprintf(Yaxis,"YPOS=%f",-(newYS));
  ierr = sendTwoCommand(appAGW.YIP,Yaxis,"MOVETARG",dummy); // Y
  shm_addr->MODS.reqpos[appAGW.YIP]=-(newYS);

  if(ierr!=0) {
    sprintf(reply,"098 One or more MOVE errors found on Y and/or Focus");
    return CMD_ERR;
  }

  for(i=0,break_cnt=0;i<40000;i++) {
    if(break_cnt==0) { // Monitor Focus Stage
      rawCommand(appAGW.FIP,"PRINT MVG",dummy);
      if(!strcasecmp(dummy,"FALSE")) break_cnt++;
      shm_addr->MODS.pos[appAGW.FIP]=positionToShrMem(appAGW.FIP,dummy);

    } else if(break_cnt==1) {  // Monitor Y Stage
      rawCommand(appAGW.YIP,"PRINT MVG",dummy);
      if(!strcasecmp(dummy,"FALSE")) break_cnt++;
      shm_addr->MODS.pos[appAGW.YIP]=positionToShrMem(appAGW.YIP,dummy);
      
    } else if(break_cnt==2) break;
    MilliSleep(100);
    
  }

  cmd_focus("",EXEC,reply);

  return CMD_OK;

}

//---------------------------------------------------------------------------
//
// focoffset - set/query the focus offset - ***OBSOLETE***
//

/*!  
  \brief FOCOFFSET command - set/query the focus offset
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  focoffset [df]

  \par Units:
  df in microns

  Sets the focus offset relative to the nominal zero focus value loaded
  from the runtime configuration file.  If given without arguments, it returns
  the current focus offset value.  Setting focoffset 0.0 disables focus offset
  and restores the runtime default.

  The runtime configuration file loads the nominal field center and
  focus value at field center.  If this nominal field-center focus
  changes, the user can enter a simple +/- offset in microns that will
  be applied by the autofocus system.  In general, if a persistent
  offset is required of the same value, the runtime configuration file
  should be updated to change both Focus0 and YCenter to reflect the new
  nominal focus and Y position at field center.

  \par Lock Behavior:
  Query is permitted regardless of the lock status.  Set is only allowed
  by the lockHost if the lock is set.

  \sa anything that moves focus or in x,y
*/

int
cmd_foffset(char *args, MsgType msgtype, char *reply)
{
  char argbuf[32];
  int dfoc;
  double df;
  double reqFoc, reqYS;
  char dummy[132];
  char Yaxis[24];
  char Faxis[24];
  int ierr;
  int options=0;
  int i, break_cnt;

  // disable the command

  sprintf(reply,"Command obsolete - use setfocus/getfocus instead");
  return CMD_OK;

  /*
   * Check the lock - only permit action if unlocked, or command is from
   * the lock holder
   */ 

  if (agwIsLocked(&appAGW,reply)) {
    strcpy(reply,"008 AGW locked");
    return CMD_ERR;
  }

  /* Check for Power Failure variable PWRFAIL in PLC ladder code */
  //ierr = rawCommand(appAGW.FIP,"PRINT PWRFAIL",dummy);
  //  if(atoi(dummy)) {
  //    sprintf(reply,"AGWFS=PWRFLR Power Failure, (reset)");
  //    return CMD_ERR;
  //}

  if (strlen(args) == 0) { // just a query, report current values
    sprintf(reply,"AGWFS0=%0.3f AGWFOCUS=%0.3f",appAGW.foc0,appAGW.focOffset);

    return CMD_OK;
  }

  /* Change requested, do it */

  GetArg(args,1,argbuf);
  dfoc = atoi(argbuf);
  if (abs(dfoc) > AGW_DFOCMAX) {
    sprintf(reply,"031 Invalid focus offset '%s' maximum is +/-%d microns",
	    argbuf,AGW_DFOCMAX);
    return CMD_ERR;
  }

  GetArg(args,2,argbuf); // get options
  options = atoi(argbuf);

  /* Do we have to change the offset? */

  df = (double)(dfoc) - appAGW.focOffset;

  if (df != 0.0) { // new offset is different, so we have to move
    agwGetPos(&appAGW); // query current position
    
    reqFoc = appAGW.focus + df; // increment focus by df
    reqYS = appAGW.yStage - df; // decrement yStage by df
    
    
    if (reqYS < AGW_YSMIN || reqYS > AGW_YSMAX) { // Validate the move request
      sprintf(reply,"101 focoffset of %d would move the Y stage past its limits, change disallowed",dfoc);
      return CMD_ERR;
    }
    
    if (reqFoc < AGW_FOCMIN || reqFoc > AGW_FOCMAX) {
      sprintf(reply,"101 focoffset of %d would move the focus actuator past its limits, change disallowed",dfoc);
      return CMD_ERR;
    }
    
    /* Move is valid, do it... */

    sprintf(Yaxis,"YPOS=%0.3f",-(reqYS));     // Y stage offset
    shm_addr->MODS.reqpos[appAGW.YIP]=-(reqYS);
    ierr = sendTwoCommand(appAGW.YIP,Yaxis,"MOVETARG",dummy); // Y move

    sprintf(Faxis,"FPOS=%0.3f",reqFoc);
    ierr = sendTwoCommand(appAGW.FIP,Faxis,"MOVETARG",dummy); // Focus
    shm_addr->MODS.reqpos[appAGW.FIP]=(reqFoc);
    
    for(i=0,break_cnt=0;i<40000;i++) {
      if(break_cnt==0) { // Monitor Focus Stage
	rawCommand(appAGW.FIP,"PRINT MVG",dummy);
	if(!strcasecmp(dummy,"FALSE")) break_cnt++;
	shm_addr->MODS.pos[appAGW.FIP]=positionToShrMem(appAGW.FIP,dummy);
	
      } else if(break_cnt==1) {  // Monitor Focus Stage
	rawCommand(appAGW.YIP,"PRINT MVG",dummy);
	if(!strcasecmp(dummy,"FALSE")) break_cnt++;
	shm_addr->MODS.pos[appAGW.YIP]=positionToShrMem(appAGW.YIP,dummy);
	
      } else if(break_cnt==2) break;
      MilliSleep(100);
    
    }

    /* Done, set the new offset value and update the new stage position */

    appAGW.focOffset = (double)(dfoc);
    agwGetPos(&appAGW); // query new position
    
    /* 
     * Craft the reply.  Because we've moved we have to report the new
     * focus and Y Stage positions after offsetting the focus
     */

    sprintf(reply,"AGWFOCUS=%0.3f AGWFS=%0.3f AGWFS0=%0.3f AGWYS=%0.3f",
	    appAGW.focOffset,appAGW.focus,appAGW.foc0,appAGW.yStage);

  } else { // No change requested (request=current), just report current values
    sprintf(reply,"AGWFOCUS=%0.3f AGWFS=%0.3f AGWFS0=%0.3f",
	    appAGW.focOffset,appAGW.focus,appAGW.foc0);

  }

  return CMD_OK;
}

//---------------------------------------------------------------------------
//
// autofoc - autofocus set/query
//

/*!  
  \brief AUTOFOCUS command - set/query the autofocus state
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  autofocus [on|off]

  Enables/disables autofocus.  If the command is given without
  arguments, it reports the current autofocus state (enabled/disabled).

  \par Lock Behavior:
  Query is permitted regardless of the lock status.  Set is only allowed by the
  lockHost if the lock is set.

  \sa anything that moves focus or in x,y
*/

int
cmd_autofoc(char *args, MsgType msgtype, char *reply)
{
  char argbuf[32];
  char dummy[132];
  int ierr;
  int i, break_cnt;

  // Check the lock - only permit action if unlocked, or command is from the lock holder

  if (agwIsLocked(&appAGW,reply)) {
    strcpy(reply,"008 AGW locked");
    return CMD_ERR;
  }

  // If no arguments, this is just a query

  if (strlen(args)<=0) { // just a query
    sprintf(reply,"%s AGWFOCUS=%0.3f AGWFS=%0.3f AGWFS0=%0.3f",	  
	    ((appAGW.autoFocus) ? "+AutoFocus" : "-AutoFocus"),
	    appAGW.focOffset,appAGW.focus,appAGW.foc0);

    return CMD_OK;
  }

  GetArg(args,1,argbuf);
  if (strcasecmp(argbuf,"on")==0) {
    appAGW.autoFocus = 1;
    sprintf(reply,"AutoFocus enabled, +AutoFocus AGWFOCUS=%0.3f AGWFS=%0.3f AGWFS0=%0.3f",
	    appAGW.focOffset,appAGW.focus,appAGW.foc0);

  }
  else if (strcasecmp(argbuf,"off")==0) {
    appAGW.autoFocus = 0;
    sprintf(reply,"AutoFocus disabled, -AutoFocus AGWFOCUS=%0.3f AGWFS=%0.3f AGWFS0=%0.3f",
	    appAGW.focOffset,appAGW.focus,appAGW.foc0);
  }
  else {
    sprintf(reply,"032 Unrecognized argument(s) '%s', Usage: AUTOFOCUS [ON|OFF]",args);
    return CMD_ERR;
  }

  return CMD_OK;

}

//---------------------------------------------------------------------------
//
// init - initialize the AGW stage and filter wheel
//

/*!  
  \brief INIT command - initialize the AGW stage and filter wheel
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  init

  Initializes the AGW X,Y,Focus stage and filter wheel.

  \par Lock Behavior:
  Initialization overrides the lock status

  \sa anything that moves focus or in x,y
*/

int
cmd_init(char *args, MsgType msgtype, char *reply)
{
  int options=0;
  int i, break_cnt;
  int ierr;
  char Yaxis[24];
  char Xaxis[24];
  char Faxis[24];
  char FWaxis[24];
  char who_selected[24];
  char dummy[132];
  char opts[10];
  double dX = 0;
  double dY = 0;
  double dF = 0;
  double Xgp, Ygp;
  
  strcpy(who_selected,cmdtab[commandID].cmd);
  StrUpper(who_selected);

  if(!strcasecmp(who_selected,"INIT")) {
    GetArg(args,1,opts);
    options=atoi(opts);

    appAGW.xStage = 0;
    appAGW.yStage = 0;
    appAGW.focus  = 0;

    ierr=sendCommand(appAGW.YIP,"MOVA 1",dummy);
    ierr=sendCommand(appAGW.XIP,"MOVA 1",dummy);
    ierr=sendCommand(appAGW.FIP,"MOVA -1",dummy);

    for(i=0,break_cnt=0;i<40000;i++) {
      if(break_cnt==0) { // Monitor Y Stage
	rawCommand(appAGW.YIP,"PRINT MVG",dummy);
	if(!strcasecmp(dummy,"FALSE")) break_cnt++;
	shm_addr->MODS.pos[appAGW.YIP]=positionToShrMem(appAGW.YIP,dummy);

      } else if(break_cnt==1) {  // Monitor X Stage
	rawCommand(appAGW.XIP,"PRINT MVG",dummy);
	if(!strcasecmp(dummy,"FALSE")) break_cnt++;
	shm_addr->MODS.pos[appAGW.XIP]=positionToShrMem(appAGW.XIP,dummy);

      } else if(break_cnt==2) {  // Monitor Focus Stage
	rawCommand(appAGW.FIP,"PRINT MVG",dummy);
	if(!strcasecmp(dummy,"FALSE")) break_cnt++;
	shm_addr->MODS.pos[appAGW.FIP]=positionToShrMem(appAGW.FIP,dummy);

      } else if(break_cnt==3) break;
      MilliSleep(100);

    }

    ierr=sendCommand(appAGW.YIP,"INITIAL",dummy);
    shm_addr->MODS.pos[appAGW.YIP]=0.0;

    ierr=sendCommand(appAGW.XIP,"INITIAL",dummy);
    shm_addr->MODS.pos[appAGW.XIP]=0.0;
    
    ierr=sendCommand(appAGW.FIP,"INITIAL",dummy);
    shm_addr->MODS.pos[appAGW.FIP]=0.0;

    appAGW.filter=mlcBitsBase10(appAGW.FWIP,"PRINT IO 22,IO 21",dummy)+1;
    shm_addr->MODS.pos[appAGW.FWIP]=(float)appAGW.filter;

    if(options==AGW_START) {
      memset(dummy,0,sizeof(dummy));
      ierr=utilities("localhost","10435","agc on",dummy);
      MilliSleep(100);
      ierr=utilities("localhost","10435","wfs on",dummy);
      if(ierr!=0) {
	sprintf(reply,"-1 Could not turn ON AGW \n");
	return CMD_ERR;
      }
    } else if(options==AGW_STOP) {
      memset(dummy,0,sizeof(dummy));
      ierr=utilities("localhost","10435","agc off",dummy);
      MilliSleep(100);
      ierr=utilities("localhost","10435","wfs off",dummy);
      if(ierr!=0) {
	sprintf(reply,"-1 Could not turn OFF AGW \n");
	return CMD_ERR;
      }
    }
    
  } else if(!strcasecmp(who_selected,"INITY")) {
    appAGW.yStage = 0;

    // Check the calibration tower position, disallow if the tower is in
    //if(appAGW.calib==1) { 
    //strcpy(reply,"201 Calibration Tower is in the beam, move disallowed");
    //return CMD_ERR;
    //}

    ierr=sendCommand(appAGW.YIP,"INITIAL",dummy);
    shm_addr->MODS.pos[appAGW.YIP]=0.0;

  } else if(!strcasecmp(who_selected,"INITX")) {
    appAGW.xStage = 0;

    ierr=sendCommand(appAGW.XIP,"INITIAL",dummy);
    shm_addr->MODS.pos[appAGW.XIP]=0.0;

  } else if(!strcasecmp(who_selected,"INITFOC")) {
    appAGW.focus = 0;

    ierr=sendCommand(appAGW.FIP,"INITIAL",dummy);
    shm_addr->MODS.pos[appAGW.FIP]=0.0;

  } else if(!strcasecmp(who_selected,"INITFILT")) {
    appAGW.filter=mlcBitsBase10(appAGW.FWIP,"PRINT IO 22,IO 21",dummy)+1;

    shm_addr->MODS.pos[appAGW.FWIP]=(float)appAGW.filter;

  }

  return CMD_OK;
}

//---------------------------------------------------------------------------
//
// home - send the AGW stage to its home (safely stowed) position
//

/*!  
  \brief HOME command - send the AGW stage to its home (stowed) position
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  home

  Sends the AGW stage to its home position, safely stowed out of the beam, and
  retracts the pickoff focus arm.

  \par Lock Behavior:
  Query is permitted regardless of the lock status.  Set is only allowed by the
  lockHost if the lock is set.

  \sa cmd_xy
*/

int
cmd_home(char *args, MsgType msgtype, char *reply)
{
  char dummy[132];
  char argbuf[64];
  double dX = 0;
  double dY = 0;
  double dF = 0;
  double movevec = 0;
  long foctime = 0;
  int ierr;
  int i, break_cnt;
  int options=0;
  double Xgp,Ygp;

  memset(argbuf,0,sizeof(argbuf));

  GetArg(args,1,argbuf);
  StrUpper(argbuf);

  // Check the lock - only permit action if unlocked, or command is from the lock holder

  if (agwIsLocked(&appAGW,reply)) {
    strcpy(reply,"008 AGW locked");
    return CMD_ERR;
  }

  // First, update the current AGW stage position (don't allow stale info)

  agwGetPos(&appAGW);

  // Perform the requested command options.

  if (!strcasecmp(argbuf,"X")) { // code for homing Xs

    dX = 0 - appAGW.xStage;

    ierr=sendCommand(appAGW.XIP,"MOVA 0",dummy);

    if(ierr!=0) {
      ierr=sendCommand(appAGW.XIP,"INITIAL",dummy);
    }

    for(i=0,break_cnt=0;i<40000;i++) {
      rawCommand(appAGW.XIP,"PRINT MVG",dummy);
      if(!strcasecmp(dummy,"FALSE")) break;
      shm_addr->MODS.pos[appAGW.XIP]=positionToShrMem(appAGW.XIP,dummy);
	
      MilliSleep(100);
    }

    appAGW.xStage = shm_addr->MODS.pos[appAGW.XIP] = positionToShrMem(appAGW.XIP,dummy);

    sprintf(reply,"HOME AGWXS=%0.3f",appAGW.xStage);

  } else if (!strcasecmp(argbuf,"Y")) { // homeing Y Stage
    
    dY = 0 - appAGW.yStage;

    ierr=sendCommand(appAGW.YIP,"MOVA 0",dummy);
    if(ierr!=0) {
      ierr=sendCommand(appAGW.YIP,"INITIAL",dummy);
    }

    for(i=0,break_cnt=0;i<40000;i++) {
      rawCommand(appAGW.YIP,"PRINT MVG",dummy);
      if(!strcasecmp(dummy,"FALSE")) break;
      shm_addr->MODS.pos[appAGW.YIP]=positionToShrMem(appAGW.YIP,dummy);

      MilliSleep(100);
    }	

    appAGW.yStage = shm_addr->MODS.pos[appAGW.YIP] = positionToShrMem(appAGW.YIP,dummy);
    sprintf(reply,"HOME AGWYS=%0.3f\n",appAGW.yStage);

    
  } else if (!strcasecmp(argbuf,"FOCUS")) { // homing Focus

    dF = 0 - appAGW.focus;

    ierr=sendCommand(appAGW.FIP,"MOVA 0",dummy);
    if(ierr!=0) {
      ierr=sendCommand(appAGW.FIP,"INITIAL",dummy);
    }

    for(i=0,break_cnt=0;i<40000;i++) {
      rawCommand(appAGW.FIP,"PRINT MVG",dummy);
      if(!strcasecmp(dummy,"FALSE")) break;
      shm_addr->MODS.pos[appAGW.FIP]=positionToShrMem(appAGW.FIP,dummy);

      MilliSleep(100);
    }

    appAGW.focus = shm_addr->MODS.pos[appAGW.FIP]=positionToShrMem(appAGW.FIP,dummy);
    sprintf(reply,"HOME AGWFS=%0.3f\n",appAGW.focus);

  } else if (!strcasecmp(argbuf,"FILTER")) { // Show filter position

    appAGW.filter=mlcBitsBase10(appAGW.FWIP,"PRINT IO 22,IO 21",dummy)+1;

    shm_addr->MODS.pos[appAGW.FWIP] = appAGW.filter;
    sprintf(reply,"HOME AGWFilt=%d\n", appAGW.filter);

  } else if (!strcasecmp(argbuf,"ALL")) { // Show filter position

    dX = 0 - appAGW.xStage;
    dY = 0 - appAGW.yStage;
    dF = 0 - appAGW.focus;

    ierr=0;
    ierr=sendCommand(appAGW.FIP,"MOVA 0",dummy) + ierr;
    ierr=sendCommand(appAGW.XIP,"MOVA 0",dummy) + ierr; 
    ierr=sendCommand(appAGW.YIP,"MOVA 0",dummy) + ierr;    

    for(i=0,break_cnt=0;i<40000;i++) {
      if(break_cnt==0) { // Monitor Focus Stage
	rawCommand(appAGW.FIP,"PRINT MVG",dummy);
	if(!strcasecmp(dummy,"FALSE")) break_cnt++;
	shm_addr->MODS.pos[appAGW.FIP]=positionToShrMem(appAGW.FIP,dummy);

      } else if(break_cnt==1) {  // Monitor X Stage
	rawCommand(appAGW.XIP,"PRINT MVG",dummy);
	if(!strcasecmp(dummy,"FALSE")) break_cnt++;
	shm_addr->MODS.pos[appAGW.XIP]=positionToShrMem(appAGW.XIP,dummy);
	
      } else if(break_cnt==2) {  // Monitor Y Stage
	rawCommand(appAGW.YIP,"PRINT MVG",dummy);
	if(!strcasecmp(dummy,"FALSE")) break_cnt++;
	shm_addr->MODS.pos[appAGW.YIP]=positionToShrMem(appAGW.YIP,dummy);
	
      } else if(break_cnt==3) break;
      MilliSleep(100);
    
    }

    ierr=sendCommand(appAGW.YIP,"INITIAL",dummy);
    if(ierr!=0) {
      sprintf(reply,"-1 %s",dummy);
      return CMD_ERR;
    }

    ierr=sendCommand(appAGW.XIP,"INITIAL",dummy);
    if(ierr!=0) {
      sprintf(reply,"-1 %s",dummy);
      return CMD_ERR;
    }

    ierr=sendCommand(appAGW.FIP,"INITIAL",dummy);
    if(ierr!=0) {
      sprintf(reply,"-1 %s",dummy);
      return CMD_ERR;
    }
    
    appAGW.filter=mlcBitsBase10(appAGW.FWIP,"PRINT IO 22,IO 21",dummy)+1;
    
    appAGW.xStage = shm_addr->MODS.pos[appAGW.XIP] = positionToShrMem(appAGW.XIP,dummy);
    appAGW.yStage = shm_addr->MODS.pos[appAGW.YIP] = positionToShrMem(appAGW.YIP,dummy);
    appAGW.focus = shm_addr->MODS.pos[appAGW.FIP]=positionToShrMem(appAGW.FIP,dummy);

    sprintf(reply,"HOME AGWXS=%0.3f AGWYS=%0.3f AGWFS=%0.3f AGWFilt=%d\n",
	    appAGW.xStage,appAGW.yStage,appAGW.focus,appAGW.filter);
    
  } else {  // HOME all motors
    sprintf(reply,"099 Invalid '%s' argument, Usage home [x|y|focus|filter]",argbuf);
    return CMD_ERR;    
  }
  return CMD_OK;
}

//---------------------------------------------------------------------------
//
// agw - send commands to the AGW stage
//

/*!  
  \brief agw command - send commands to the AGW stage
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  agw [xpos ypos fpos]

  Sends the AGW stage a raw position safely.

  \par Lock Behavior:
  Query is permitted regardless of the lock status.  Set is only allowed by the
  lockHost if the lock is set.

  \sa cmd_xy
*/

int
cmd_agw(char *args, MsgType msgtype, char *reply)
{
  char who_selected[24];

  memset(argbuf,0,sizeof(argbuf));
  strcpy(who_selected,cmdtab[commandID].cmd);
  StrUpper(who_selected);

  /*
  // AGW Stage status for All, X, Y, Focus or Filter Wheel
  */
  if(!strcasecmp(who_selected,"AGW")) {
    sprintf(reply,"%s AGWXS=%0.3f AGWYS=%0.3f AGWFS=%0.3f AGWFILT=%d AGWFNAME=%s",
	    who_selected, appAGW.xStage,appAGW.yStage,appAGW.focus,
	    appAGW.filter,(appAGW.filterID)[appAGW.filter-1]);
    
  } else if(!strcasecmp(who_selected,"AGWX")) {
    sprintf(reply,"%s AGWXS=%0.3f",who_selected,appAGW.xStage);

  } else if(!strcasecmp(who_selected,"AGWY")) {
    sprintf(reply,"%s AGWYS=%0.3f",who_selected,appAGW.yStage);
    
  } else if(!strcasecmp(who_selected,"AGWFOC")) {
    sprintf(reply,"%s AGWFS=%0.3f",
	    who_selected,appAGW.focus);
    
  } else if(!strcasecmp(who_selected,"AGWFILT")) {
    sprintf(reply,"%s AGWFILT=%d AGWFNAME=%s", 
	    who_selected, 
	    appAGW.filter,(appAGW.filterID)[appAGW.filter-1]);
  }
  return CMD_OK;
}

//---------------------------------------------------------------------------
//
// center - send the AGW stage to the science field center
//

/*!  
  \brief CENTER command - send the AGW stage to the science field center
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  center

  Sends the AGW stage to the center of the science field, defined as
  guide probe position (0,0).

  \par Lock Behavior: 
  Query is permitted regardless of the lock status.  Set is only allowed
  by the lockHost if the lock is set.

  \sa cmd_xy, cmd_home
*/

int
cmd_center(char *args, MsgType msgtype, char *reply)
{
  char argbuf[64];
  char Xaxis[64];
  char Yaxis[64];
  char Faxis[64];
  char dummy[79];
  double dX = 0;
  double dY = 0;
  double dF = 0;
  double movevec = 0;
  long foctime = 0;
  int ierr = 0;
  int i;
  int break_cnt;
  int options=0;
  double reqXS;
  double reqYS;
  double reqFoc;

  /*
  // Check the lock - only permit action if unlocked, or command is from 
  // the lock holder
  */

  if (agwIsLocked(&appAGW,reply)) {
    strcpy(reply,"008 AGW locked");
    return CMD_ERR;
  }

  /* 
  // Check the calibration tower position, disallow if the tower is in
  */
  if(appAGW.calib==1) { 
    strcpy(reply,"201 Calibration Tower is in the beam, move disallowed");
    return CMD_ERR;
  }

  /*
  // First, update the current AGW stage position (don't allow stale info)
  */
  agwGetPos(&appAGW);


  /*
  // Compute the absolute move distance to the field center
  */
  gpToAGW(&appAGW,0.0,0.0,&reqXS,&reqYS,&reqFoc);

  dX = reqXS  - appAGW.xStage;
  dY = reqYS  - appAGW.yStage;
  dF = reqFoc - appAGW.focus;

  if (dX==0 && dY==0 && dF==0) {  // Are we already there?
    sprintf(reply,"AGW Stage at Science Field Center, AGWXGP=%0.3f AGWYGP=%0.3f AGWFOCUS=%0.3f AGWXWFS=%0.3f YWSF=%0.3f AGWXS=%0.3f AGWYS=%0.3f AGWFS=%0.3f AGWFS0=%0.3f",
  	    appAGW.xAG,appAGW.yAG,appAGW.focOffset,
	    appAGW.xWFS,appAGW.yWFS,
	    appAGW.xStage,appAGW.yStage,appAGW.focus,
	    appAGW.foc0);

    return CMD_OK;
  }


  GetArg(args,1,argbuf);  // Get options
  options = atoi(argbuf);

  /*
  // Start moving the Stage
  */
  sprintf(Faxis,"FPOS=%f",reqFoc);  // Focus axis
  ierr = sendTwoCommand(appAGW.FIP,Faxis,"MOVETARG",dummy); // Focus
  shm_addr->MODS.reqpos[appAGW.FIP] = reqFoc;


  sprintf(Xaxis,"XPOS=%f",-(reqXS));  // X axis
  ierr = sendTwoCommand(appAGW.XIP,Xaxis,"MOVETARG",dummy) + ierr;


  sprintf(Yaxis,"YPOS=%f",-(reqYS));  // Y axis
  shm_addr->MODS.reqpos[appAGW.YIP]=-(reqYS);
  ierr = sendTwoCommand(appAGW.YIP,Yaxis,"MOVETARG",dummy);
  

  if (options==OPT_BLOCK) { // AGW polling
    for(i=0,break_cnt=0;i<40000;i++) {
      if(break_cnt==0) { // Monitor Focus Stage
	rawCommand(appAGW.FIP,"PRINT MVG",dummy);
	if(!strcasecmp(dummy,"FALSE")) break_cnt++;
	shm_addr->MODS.pos[appAGW.FIP]=positionToShrMem(appAGW.FIP,dummy);

      } else if(break_cnt==1) {  // Monitor X Stage
	rawCommand(appAGW.XIP,"PRINT MVG",dummy);
	if(!strcasecmp(dummy,"FALSE")) break_cnt++;
	shm_addr->MODS.pos[appAGW.XIP]=positionToShrMem(appAGW.XIP,dummy);
	
      } else if(break_cnt==2) {  // Monitor Y Stage
	rawCommand(appAGW.YIP,"PRINT MVG",dummy);
	if(!strcasecmp(dummy,"FALSE")) break_cnt++;
	shm_addr->MODS.pos[appAGW.YIP]=positionToShrMem(appAGW.YIP,dummy);
	
      } else if(break_cnt==3) break;
      MilliSleep(10);
    
    }
  }

  /*
  // Craft the reply string
  */
  sprintf(reply,"CENTER AGWXGP=%0.3f AGWYGP=%0.3f AGWFOCUS=%0.3f AGWXWFS=%0.3f AGWYWFS=%0.3f AGWXS=%0.3f AGWYS=%0.3f AGWFS=%0.3f AGWFS0=%0.3f",
	  appAGW.xAG,appAGW.yAG,appAGW.focOffset,
	  appAGW.xWFS,appAGW.yWFS,
	  appAGW.xStage,appAGW.yStage,appAGW.focus,
	  appAGW.foc0);

  shm_addr->MODS.pos[appAGW.XIP]=appAGW.xStage;
  shm_addr->MODS.pos[appAGW.YIP]=appAGW.yStage;
  shm_addr->MODS.pos[appAGW.FIP]=appAGW.focus*2.0;
  
  return CMD_OK;
}


//---------------------------------------------------------------------------
//
// calib - simulate insert/retract of the calibration tower
//

/*!  
  \brief CALIB command - insert or retract a simulated MODS Calibration Tower
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  calib [in|out]
 
  Simulates insertion or retraction of the MODS Calibration Tower.  The
  calibration tower and the AGW stage occupy the same space in the
  science field, and could collide.  In hardware, there are interlocks
  to prevent such collisions.  Users of the MODS AGW simulator can
  simulate the behavior of the system with the calibration tower
  inserted into the beam by typing the "calib in", which will generate
  the necessary "can't do that..." error messages for your GCS client
  under test.  "calib out" cancels the test, and "calib" without
  arguments gives the current tower state (in or out).

  calib is only an Executive command, and cannot be issued by a remote
  socket client (unless posed as an executive override command, which is
  frankly a bit obtuse).

  \par Lock Behavior: 
  This command ignores the command lock.  Note that a GCS client will
  never issue the calib command.

*/

int
cmd_calib(char *args, MsgType msgtype, char *reply)
{

  StrUpper(args);

  if (strlen(args)<=0) {   // no arguments, means query position
    if(appAGW.calib==0) sprintf(reply,"CALIB=OUT");
    else sprintf(reply,"CALIB=IN");
    return CMD_OK;
  }

  if(strstr(args,"IN")){
    appAGW.calib=1;
    sprintf(reply,"CALIB=IN");
  } else {
    appAGW.calib=0;
    sprintf(reply,"CALIB=OUT");
  }
  return CMD_OK;

}

//---------------------------------------------------------------------------
//
// lock - assert a command lock on the AGW stage
//

/*!  
  \brief LOCK command - assert a command lock on the AGW stage
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  lock

  Assert command-lock on the AGW stage.  When the AGW is locked, it
  means that all subsequent commands will only be accepted from your
  host ID.  This allows a remote host to lock out conflicting moves from
  other hosts during critical operations.

  To release a lock, the host holding the lock must send the UNLOCK
  command.

  If a host requests a lock currently being held by another host, it
  returns an error, reporting the ID of the host holding the lock.

  \par Lock Behavior:
  If lock is set, reports lock status if called by the lockHost, or an
  error if not with the identity of lockHost reported in the error message.
  If the lock is not set, the lock is set and the command host is made
  the lock holder.
*/

int
cmd_lock(char *args, MsgType msgtype, char *reply)
{

  /*
  // If the lock is already asserted, say so, and by whom
  */
  if (appAGW.lock) {
    if (strcasecmp(appAGW.cmdHost,appAGW.lockHost)==0) { // command is from current lock holder
      sprintf(reply,"012 AGW already locked, +lock lockHost=%s",appAGW.lockHost);
      return CMD_ERR;
    }
    else { // request from someone not the lock holder, complain
      sprintf(reply,"012 Cannot LOCK, AGW lock already held by lockHost=%s +lock", appAGW.lockHost);
      return CMD_ERR;
    }
  }

  /* 
  // lock is not current held, give it to the requestor
  */
  appAGW.lock = 1;
  strcpy(appAGW.lockHost,appAGW.cmdHost);
  sprintf(reply,"LOCK +lock lockHost=%s",appAGW.lockHost);
  return CMD_OK;

}

//---------------------------------------------------------------------------
//
// unlock - release a command lock on the AGW stage
//

/*!  
  \brief UNLOCK command - release a command lock on the AGW stage
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  unlock

  Release a command lock held on the AGW stage.  You must be the lock holder
  to release a lock.

  If a host attempts to unlock the AGW but is not the lock holder, it
  returns an error, reporting the ID of the host holding the lock.

  To provide a way to break deadlock situations (e.g., host holding the
  lock terminates without releasing the lock), if UNLOCK is sent as an
  EXEC command, it forces the lock to be released.

  \par Lock Behavior: 
  If lock is not set, it returns immediately with an OK message.  If the
  lock is set, it releases the lock if the caller is the current lock
  holder, otherwise it reports an error and the lock holder ID.  The
  exception is for executive override, which ignores the lock holder and
  releases the lock.
*/

int
cmd_unlock(char *args, MsgType msgtype, char *reply)
{

  /*
  // If the lock isn't set, no need to try releasing it.
  */
  if (appAGW.lock == 0) {
    sprintf(reply,"095 lock not asserted, lock=Disabled, lockHost=none");
    return CMD_OK;
  }

  /*
  // Override lock-holder check if unlock issued as an executive override
  // command
  */
  if (msgtype == EXEC) {
    appAGW.lock = 0;
    strcpy(appAGW.lockHost,"none");
    sprintf(reply,"096 Command LOCK released by Executive Override lock=Disabled lockHost=none");
    return CMD_OK;
  }

  // Check to see who requested unlock.  If the lockHost, release the lock

  if (strcasecmp(appAGW.cmdHost,appAGW.lockHost)==0) {
    appAGW.lock = 0;
    sprintf(reply,"LOCK released by %s, lock=Disabled lockHost=none",appAGW.lockHost);
    strcpy(appAGW.lockHost,"none");
    return CMD_OK;
  }

  /*
  // Someone other than the lockHost requested an unlock, complain but also
  // tell them the lockHost's identity.  They can always try again using an
  //  EXEC: message to override the lock.
  */
  sprintf(reply,"013 Cannot UNLOCK, Command LOCK held by lockHost=%s",appAGW.lockHost);
  return CMD_ERR;

}

//---------------------------------------------------------------------------
//
// driftvec - set/query the stage drift vector
//

/*!  
  \brief DRIFTVEC command - Set/query the stage drift vector
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  driftvec [xdot ydot]

  \par Units:
  xdot and ydot in microns/second

  Sets the drift vector rates in X and Y in units of microns/second.
  This command only sets rates, it does not initiate a drift (see DRIFT).

  If given with no arguments, it reports the current drift vector rates.

  Note that you cannot set the drift rate while the probe is moving.

  \par Lock Behavior:
  Query is permitted regardless of the lock status.  Set is only allowed by the
  lockHost if the lock is set.
*/

int
cmd_driftvec(char *args, MsgType msgtype, char *reply)
{
  char argbuf[32];
  int doQuery = 0;
  long reqXrate = 0;
  long reqYrate = 0;

  /*
  // Examine the command line 'args' and parse if needed
  */

  if (strlen(args)<=0) {   // no arguments, means query position
    doQuery = 1;
  }
  else {

    if (agwIsLocked(&appAGW,reply)) { // Check the lock state
      strcpy(reply,"008 AGW locked");
      return CMD_ERR;
    }

    /*
    // Cannot change rates while the probe is drifting
    */
    if (appAGW.drift) {
      strcpy(reply,"043 Cannot change drift rates while drift is enabled");
      return CMD_ERR;
    }

    GetArg(args,1,argbuf);
    reqXrate = atol(argbuf);
    if (fabs(reqXrate) > AGW_XMAXRATE) {
      sprintf(reply,"023 Invalid X-axis drift rate '%0.3f', must be +/-0..%d um/sec",reqXrate,AGW_XMAXRATE);

      return CMD_ERR;
    }

    GetArg(args,2,argbuf);
    reqYrate = atof(argbuf);
    if (fabs(reqYrate) > AGW_YMAXRATE) {
      sprintf(reply,"018 Invalid Y-axis drift rate '%0.3f', must be +/-0..%d um/sec)",reqYrate,AGW_YMAXRATE);

      return CMD_ERR;
    }

    doQuery = 0;
  }

  /*
  // If only a query, return the current AGW drive vector rates
  */
  if (doQuery) {
    sprintf(reply,"DRIFTVEC AGWXRATE=%d AGWYRATE=%d um/sec",appAGW.xDrift,appAGW.yDrift);
    return CMD_OK;
  }

  /*
  // Set the drift rate vectors
  */
  appAGW.xDrift = reqXrate;
  appAGW.yDrift = reqYrate;
  sprintf(reply,"DRIFTVEC AGWXRATE=%d AGWYRATE=%d um/sec",appAGW.xDrift,appAGW.yDrift);
  return CMD_OK;

}

//---------------------------------------------------------------------------
//
// drift - start/stop/query drift
//

/*!  
  \brief DRIFT command - start/stop/query drift
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  drift [start|stop]

  Turns drift on/off.  When enabled, the probe will be driven at the
  current drift rates (see DRIFTVEC).  If DRIFT is given with no arguments,
  it will return the current drift status (enabled/disabled) and the drift
  vector rates.

  If the drift vector is 0um/sec in both axes, DRIFT START will return an error.

  \par Lock Behavior:
  Query is permitted regardless of the lock status.  Set is only allowed by the
  lockHost if the lock is set.
*/

int
cmd_drift(char *args, MsgType msgtype, char *reply)
{

  /*
  // If no arguments, this is just a query
  */
  if (strlen(args)<=0) { // just a query
    sprintf(reply,"%s AGWXRATE=%d AGWYRATE=%d",
	    ((appAGW.drift) ? "+Drift" : "-Drift"),
	    appAGW.xDrift,appAGW.yDrift);
    return CMD_OK;
  }

  if (agwIsLocked(&appAGW,reply)) {  // Check the lock state
    strcpy(reply,"008 AGW locked");
    return CMD_ERR;
  }

  if (strcasecmp(args,"start")==0) {
    if (appAGW.xDrift == 0 && appAGW.yDrift == 0) {
      strcpy(reply,"040 No Drift Vector set, cannot initiate drift");
      return CMD_ERR;
    }
    appAGW.drift = 1;
    sprintf(reply,"DRIFT started, +Drift AGWXRATE=%d AGWYRATE=%d",
	    appAGW.xDrift,appAGW.yDrift);
    // agwStartDrift(appAGW);  // begin drift...
  }
  else if (strcasecmp(args,"stop")==0) {
    appAGW.drift = 0;
    sprintf(reply,"DRIFT stopped, -Drift");
    // agwStopDrift(appAGW);  // stop drift...
  }
  else {
    sprintf(reply,"042 Unrecognized argument '%s', Usage: DRIFT <START|STOP>",args);
    return CMD_ERR;
  }

  return CMD_OK;

}

//---------------------------------------------------------------------------
//
// abort - abort all AGW stage motions and lock position pending further instruction
//

/*!  
  \brief ABORT command - abort all AGW stage motions and hold position pending
         further instructions
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  abort

  Immediately stops all AGW stage motions and holds the current position
  (sets the brakes). The AGW stage then idles waiting for further instructions.


  \par Lock Behavior:
  ABORT does not check the lock status, this allows anybody to hit the
  panic button and override any lock status that might be enabled.  On
  ABORT, the lock is released.
*/

int
cmd_abort(char *args, MsgType msgtype, char *reply)
{
  int ierr;
  char *esc;
  char temp[512];
  
  shm_addr->MODS.busy[appAGW.XIP]=0; // Clear the HOST ALL AGW busy bit.
  shm_addr->MODS.busy[appAGW.YIP]=0;
  shm_addr->MODS.busy[appAGW.FIP]=0;
  shm_addr->MODS.busy[appAGW.FWIP]=0;

  sprintf(esc,"%c",27);
  ierr=0;
  ierr = sendCommand(appAGW.XIP,esc,temp);
  ierr+=ierr;
  ierr = sendCommand(appAGW.YIP,esc,temp);
  ierr+=ierr;
  ierr = sendCommand(appAGW.FIP,esc,temp);
  ierr+=ierr;
  ierr = sendCommand(appAGW.FWIP,esc,temp);
  ierr+=ierr;
  sprintf(reply,"096 MODS AGW Stage Motion Aborted, AGW Idle");

  if(ierr!=0) return CMD_ERR;

  return CMD_OK;
}

//---------------------------------------------------------------------------
//
// setposition - set single-axis positions
//

/*!  
  \brief setposition commands - sets single-axis position
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  setposition [x|y|focus|filter] pos

  \par Units:
  x, y, and focus in millimeters

  setposition sets the named axis to the requested position.  Units are
  millimeters from home for x, y, focus, and valid device position for
  filter.

  Note that unlike other moves, this is very low level and does not update
  the derived quantities in the appAGW data struture.

  \par Lock Behavior:
  setposition checks the lock status, and only allows motion by the 
  current lock holder.

  \sa cmd_getpos
*/

int
cmd_setpos(char *args, MsgType msgtype, char *reply)
{
  int i;
  int cnt;
  int nconv;
  int ierr;
  int calib_inout;
  int options=0;
  double reqPos;
  char reqAxis[16];
  char dummy[132];
  char temp[16];

  memset(temp,0,sizeof(temp));  

  if (agwIsLocked(&appAGW,reply)) {  // Check the lock state
    strcpy(reply,"008 AGW locked");
    return CMD_ERR;
  }

  /*
  // Check the arguments, we require 2
  */
  if (strlen(args)<=0) { // arguments are required for this command
    sprintf(reply,"097 setposition requires 2 arguments, Usage: setposition [x|y|focus|filter] pos");
    return CMD_ERR;
  }

  /*
  // Check the calibration tower position, disallow if the tower is in
  */
  if(appAGW.calib==1) { 
    strcpy(reply,"201 Calibration Tower is in the beam, move disallowed");
    return CMD_ERR;
  }

  /* ******************** */
  GetArg(args,1,reqAxis);
  GetArg(args,2,temp);
  reqPos=atof(temp);  

  GetArg(args,3,temp);
  options=atoi(temp);  

  /*
  // We got two arguments, validate and take action as required.
  */
  if (strcasecmp(reqAxis,"x")==0) {

    if (reqPos < 0.0 || reqPos > AGW_XSMAX) {
      sprintf(reply,"098 Invalid XS %0.3f, must be 0..%d",reqPos,AGW_XSMAX);
      return CMD_ERR;
    }

    sprintf(temp,"XPOS=%f",-(reqPos));
    ierr = sendTwoCommand(appAGW.XIP,temp,"MOVETARG",dummy);

    if(ierr!=0) {
      sprintf(reply,"-1 %s",dummy);
      return CMD_ERR;
    }

    if (options==OPT_BLOCK) {
      for(i=0;i<40000;i++) {
	ierr=rawCommand(appAGW.XIP,"PRINT MVG",dummy);
	if(!strcasecmp(dummy,"FALSE")) break;
	shm_addr->MODS.pos[appAGW.XIP]=positionToShrMem(appAGW.XIP,dummy);
      }
    }

    sprintf(reply,"AGWXS=%0.3f",reqPos);
    shm_addr->MODS.pos[appAGW.XIP] = reqPos;

  } else if (strcasecmp(reqAxis,"y")==0) {

    if (reqPos < 0.0 || reqPos > AGW_YSMAX) {
      sprintf(reply,"099 Invalid YS %0.3f, must be 0..%d",
	      reqPos,
	      AGW_YSMAX);

      return CMD_ERR;
    }

    sprintf(temp,"YPOS=%f",-(reqPos));
    ierr = sendTwoCommand(appAGW.YIP,temp,"MOVETARG",dummy);
    
    if(ierr!=0) {
      sprintf(reply,"-1 %s",dummy);
      return CMD_ERR;
    }

    if (options==OPT_BLOCK) {
      for(i=0;i<40000;i++) {
	ierr=rawCommand(appAGW.YIP,"PRINT MVG",dummy);
	if(!strcasecmp(dummy,"FALSE")) break;
	shm_addr->MODS.pos[appAGW.YIP]=positionToShrMem(appAGW.YIP,dummy);
      }
    }

    sprintf(reply,"AGWYS=%0.3f",reqPos);
    shm_addr->MODS.pos[appAGW.YIP] = reqPos;

  } else if (strcasecmp(reqAxis,"focus")==0) {

    if (reqPos < 0.0 || reqPos > AGW_FOCMAX) {
      sprintf(reply,"030 Invalid Focus %0.3f, must be 0..%d",reqPos,AGW_FOCMAX);
      return CMD_ERR;
    }

    sprintf(temp,"FPOS=%f",reqPos);
    ierr = sendTwoCommand(appAGW.FIP,temp,"MOVETARG",dummy); // Focus
    
    if(ierr!=0) {
      sprintf(reply,"-1 %s",dummy);
      return CMD_ERR;
    }

    if (options==OPT_BLOCK) {
      for(i=0;i<40000;i++) {
	ierr=rawCommand(appAGW.FIP,"PRINT MVG",dummy);
	if(!strcasecmp(dummy,"FALSE")) break;
	shm_addr->MODS.pos[appAGW.FIP]=positionToShrMem(appAGW.FIP,dummy);
      }
    }

    sprintf(reply,"AGWFS=%0.3f",reqPos);
    shm_addr->MODS.pos[appAGW.FIP] = reqPos;

  } else if (strcasecmp(reqAxis,"filter")==0) {

    if ((int)reqPos < 1 || (int)reqPos > appAGW.numFilters) {
      sprintf(reply,"025 Invalid Filter %d, must be 1..%d",(int)reqPos,appAGW.numFilters);
      return CMD_ERR;

    }
    appAGW.filter = (int)reqPos;

    sprintf(temp,"TARGNUM=%d",appAGW.filter-1);
    ierr = sendTwoCommand(appAGW.FWIP,temp,"BEGIN",dummy);
    
    if(ierr!=0) {
      sprintf(reply,"-1 %s",dummy);
      return CMD_ERR;
    }

    if (options==OPT_BLOCK) {
      for(cnt=0;cnt<40000;cnt++) {
	rawCommand(appAGW.FWIP,"PRINT MVG",dummy);
	if(!strcasecmp(dummy,"FALSE")) break;
      }
    }
    appAGW.filter=mlcBitsBase10(appAGW.FWIP,"PRINT IO 22,IO 21",dummy)+1;

    shm_addr->MODS.pos[appAGW.FWIP]=(float)appAGW.filter;
      
    sprintf(reply,"AGWFILT=%d",appAGW.filter);
    
  } else {
    sprintf(reply,"051 Unrecognized axis '%s', Usage: setposition [x|y|focus|filter] pos", reqAxis);
    return CMD_ERR;

  }

  return CMD_OK;
}
  
//---------------------------------------------------------------------------
//
// getposition - Get single-axis mechanism positions
//

/*!  
  \brief getposition commands - get single-axis raw device positions
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  getposition [x|y|focus|filter]

  \par Units:
  x, y, and focus in millimeters to 0.001mm (1 micron) precision

  Low-level engineering command to query the requested mechanism
  controller for its current position.  Positions are returned in units
  of millimeters for linear mechanisms (Xs, Ys, and Focus), or index
  (Filter).  Values are rounded to the nearest micron (0.001mm).

  Note that this routine only queries and returns data, it does not
  update the appAGW data structure, use getxy etc. for that.

  \par Lock Behavior: 

  getposition allows anyone to query regardless of the lock status.

  \sa cmd_setpos
*/

int
cmd_getpos(char *args, MsgType msgtype, char *reply)
{
  int ierr;
  char dummy[512];

  memset(dummy,0,sizeof(dummy));

  if (strlen(args)<=0) { // arguments required here
    sprintf(reply,"096 getposition requires an argument, Usage: getposition [x|y|focus|filter]");
    return CMD_ERR;
  }

  /*
  // Live mode queries the individual mechanisms
  */
  if (strcasecmp(args,"x")==0) {
    sprintf(reply,"AGWXS=%0.3f",fabs(positionToShrMem(appAGW.XIP,dummy)));

  } else if (strcasecmp(args,"y")==0) {
    sprintf(reply,"AGWYS=%0.3f",fabs(positionToShrMem(appAGW.YIP,dummy)));

  } else if (strcasecmp(args,"focus")==0) {
    sprintf(reply,"AGWFS=%0.3f",positionToShrMem(appAGW.FIP,dummy)*2.0);

  } else if (strcasecmp(args,"filter")==0) {
    
    appAGW.filter=mlcBitsBase10(appAGW.FWIP,"PRINT IO 22,IO 21",dummy)+1;

    sprintf(reply,"AGWFilt=%d",appAGW.filter);

  } else {
    sprintf(reply,"050 Unrecognized argument '%s', Usage: getposition [x|y|focus|filter]",args);
    return CMD_ERR;

  }
  return CMD_OK;

}

//---------------------------------------------------------------------------
//
// Coordinate transformation function placeholders
//
//
// Default coordinate transformation coefficients (pseudo-function)
// agwno = AGw unit ID number, either MODS1 or MODS2
//
// These are sensible starting defaults that give non-degenerate
// transformations.  The actual transformation coefficients are
// measured in lab and on-sky tests, and loaded from the AGW control
// agent runtime initialization file, or whenever mods_agwInit() is
// executed.
//
// The numerical values below on the right-hand side of the equations
// should really be implemented as #defs in the relevant .h file and
// not embedded in code (SFPtoAGW and AGWtoSFP are unlikely to change
// much, CCDtoAGW however is still subject to measurement).
// Coordinate transform coefficients (defined in the agwutils.h file)
// Transformations
/*
  sfp_x0[2], sfp_xx[2], sfp_xy[2];// SFP X to GP X
  sfp_y0[2], sfp_yx[2], sfp_yy[2];// SFP Y to GP Y

  gp_x0[2], gp_xx[2], gp_xy[2]; // GP X to SFP X
  gp_y0[2], gp_yx[2], gp_yy[2]; // GP Y to SFP Y
  
  ccd_xx[2], ccd_xy[2]; // CCD dX to AGw GP dX offset
  ccd_yx[2], ccd_yy[2]; // CCD dY to AGw GP dY offset
*/
//
int 
cmd_ccdAgwSfp(char *args, MsgType msgtype, char *reply) 
{
  int nval;
  int agwno;
  double xgp, ygp, xsfp, ysfp;
  double dxgp, dygp, dxccd, dyccd, dxsfp, dysfp;
  double dx,dy;
  double xccd, yccd;
  double xref, yref;

  char who_selected[24];
  char cmd_instruction[PAGE_SIZE];

  memset(cmd_instruction,0,sizeof(cmd_instruction));
  strcpy(who_selected,cmdtab[commandID].cmd);
  StrUpper(who_selected);

  GetArg(args,1,cmd_instruction);
  // agwno=atoi(cmd_instruction)-1;
  agwno=atoi(cmd_instruction);
 
  if (agwno>1 || agwno<0) {
    sprintf(reply,"Invalid Instrument ID [%d] must be 0 or 1 'MODS1,MODS2 respectively'", agwno);
    return CMD_ERR;
  }             
  memset(cmd_instruction,0,sizeof(cmd_instruction));
  GetArg(args,1,cmd_instruction);
  
  if( strcasecmp(who_selected,"INITTRANS") == 0) {

    if(LoadConfig(&appAGW,&appClient,APP_CFGFILE)!=0)
      printf("Could not load the config file for defaults\n");

    sprintf(reply,"INITTRANS %0.8f %0.8f %0.8f %0.8f %0.8f %0.8f %0.8f %0.8f %0.8f %0.8f %0.8f %0.8f %0.8f %0.8f %0.8f %0.8f",
	    appAGW.sfp_x0[agwno],
	    appAGW.sfp_xx[agwno],
	    appAGW.sfp_xy[agwno],
	    appAGW.sfp_y0[agwno],
	    appAGW.sfp_yx[agwno],
	    appAGW.sfp_yy[agwno],
	    appAGW.gp_x0[agwno],
	    appAGW.gp_xx[agwno],
	    appAGW.gp_xy[agwno],
	    appAGW.gp_y0[agwno],
	    appAGW.gp_yx[agwno],
	    appAGW.gp_yy[agwno],
	    appAGW.ccd_xx[agwno],
	    appAGW.ccd_xy[agwno],
	    appAGW.ccd_yx[agwno],
	    appAGW.ccd_yy[agwno]);

    return CMD_OK;

  } else  if( strcasecmp(who_selected,"SFPTOAGW") == 0) {

    //----------------------------------------------------------------
    //
    // mods_SFPtoAGW() - convert SFP (x,y) to AGw Stage guide probe (x,y)
    //
    // Given standard focal plane coordinates (xsfp,ysfp) return AGw stage
    // guide probe coordinates (xgp,ygp)
    //

    GetArg(args,2,cmd_instruction);
    xsfp=atof(cmd_instruction);
    GetArg(args,3,cmd_instruction);
    ysfp=atof(cmd_instruction);

    xgp = appAGW.sfp_x0[agwno] + appAGW.sfp_xx[agwno]*xsfp + appAGW.sfp_xy[agwno]*ysfp;
    ygp = appAGW.sfp_y0[agwno] + appAGW.sfp_yx[agwno]*xsfp + appAGW.sfp_yy[agwno]*ysfp;

    sprintf(reply,"XGP %0.4f YGP %0.4f", xgp, ygp);

  } else if( strcasecmp(who_selected,"AGWTOSFP") == 0) {
    //----------------------------------------------------------------
    //
    // mods_AGWtoSFP() - convert AGw Stage guide probe (x,y) to SFP (x,y)
    //
    // Given AGw stage guide probe coordinates (xgp,ygp) return standard
    // focal plane coordinates (xsfp,ysfp)
    //
    GetArg(args,2,cmd_instruction);
    xgp=atof(cmd_instruction);
    GetArg(args,3,cmd_instruction);
    ygp=atof(cmd_instruction);

    xsfp = appAGW.gp_x0[agwno] + appAGW.gp_xx[agwno]*xgp + appAGW.gp_xy[agwno]*ygp;
    ysfp = appAGW.gp_y0[agwno] + appAGW.gp_yx[agwno]*xgp + appAGW.gp_yy[agwno]*ygp;
    sprintf(reply,"XSFP %0.4f YSFP %0.4f", xsfp, ysfp);

  } else  if( strcasecmp(who_selected,"CCDTOAGW") == 0) {

    //----------------------------------------------------------------
    //
    // mods_CCDtoAGW() - convert CCD offset (dx,dy) to AGw Stage coord.
    //
    // Given a pixel offset (dx,dy)_CCD on the guide camera CCD frame,
    // compute the corresponding AGW Stage position
    //
    
    GetArg(args,2,cmd_instruction);
    dxccd=atof(cmd_instruction);
    GetArg(args,3,cmd_instruction);
    dyccd=atof(cmd_instruction);

    dxgp = appAGW.ccd_xx[agwno]*dxccd + appAGW.ccd_xy[agwno]*dyccd;
    dygp = appAGW.ccd_yx[agwno]*dxccd + appAGW.ccd_yy[agwno]*dyccd;

    sprintf(reply,"DXGP %0.4f DYGP %0.4f", dxgp, dygp);

  } else  if( strcasecmp(who_selected,"CCDTOSFP") == 0) {

    //----------------------------------------------------------------
    //
    // CCDtoSFP() - convert CCD (x,y) relative to a reference (x,y) pixel
    //              into an SFP offset.
    //
    // Given a pixel location (x,y)_CCD of an object in the guide camera CCD,
    // and the pixel location of a reference location (x,y)_ref, compute the
    // corresponding SFP coordinate offset (dx,dy)_SFP.
    //
    // This transformation computes the CCD pixel offset between the reference
    // pixel and the object:
    //    dx = xref - xccd;
    //    dy = yref - yccd;
    // Then algebraically it uses the calibration of the CCD-to-AGW
    // transformation
    // to compute the corresponding guide probe offset, then this is converted
    // into the SFP offset using the AGW-to-SFP transformation.  Because this
    // is a relative rather than absolute transform, we work it out
    // algebraically here using the coefficients of the two relevant 
    // transforms.
    //
    // The detailed derivation is described in a separate document
    // [citation TBD]
    //

    GetArg(args,2,cmd_instruction);
    xccd = atof(cmd_instruction);
    GetArg(args,3,cmd_instruction);
    yccd = atof(cmd_instruction);
    GetArg(args,4,cmd_instruction);
    xref = atof(cmd_instruction);
    GetArg(args,5,cmd_instruction);
    yref =atof(cmd_instruction);
    
    // Compute the offset between the reference pixel and object pixel coords

    dx = xref - xccd;
    dy = yref - yccd;

    // Convert pixel offset (dx,dy) to SFP offset in millimeters

    dxsfp = (appAGW.gp_xx[agwno]*appAGW.ccd_xx[agwno] + appAGW.gp_xy[agwno]*appAGW.ccd_yx[agwno])*dx + (appAGW.gp_xx[agwno]*appAGW.ccd_xy[agwno] + appAGW.gp_xy[agwno]*appAGW.ccd_yy[agwno])*dy;
    
    dysfp = (appAGW.gp_yx[agwno]*appAGW.ccd_xx[agwno] + appAGW.gp_yy[agwno]*appAGW.ccd_yx[agwno])*dx + (appAGW.gp_yx[agwno]*appAGW.ccd_xy[agwno] + appAGW.gp_yy[agwno]*appAGW.ccd_yy[agwno])*dy;
    
    sprintf(reply,"DXSFP %0.4f DYSFP %0.4f", dxsfp, dysfp);
    
  }

  return CMD_OK;
}

int 
SFPtoAGWgeneric(char *args, MsgType msgtype, char *reply) 
{

  //(char* host, int agw_no, spos_xy tcssfp, angle pa, spos_xy *agwgeneric, angle *gprot);

  sprintf(reply,"100 SFPtoAGWgeneric");
  return CMD_ERR;
}

int 
AGWgenericToSFP(char *args, MsgType msgtype, char *reply) 
{
//(char* host, int agw_no, spos_xy agwgeneric, angle pa, spos_xy *tcssfp);

  sprintf(reply,"100 AGWgenericToSFP");
  return CMD_ERR;

}

int 
SFPToCCDgeneric(char *args, MsgType msgtype, char *reply) 
{

  //(char* host, int agw_no, spos_xy tcssfp, spos_xy ccdref, angle pa, angle gprot, spos_xy *ccdgeneric);

  sprintf(reply,"100 SFPtoCCDgeneric");
  return CMD_ERR;
}

int 
CCDgenericToSFP(char *args, MsgType msgtype, char *reply) 
{

  //(char* host, int agw_no, spos_xy ccdgeneric, spos_xy ccdref, angle pa, angle gprot, spos_xy *tcssfp);

  sprintf(reply,"100 CCDgenericToSFP");
  return CMD_ERR;
}


//***************************************************************************
//
// Command Interpreter I/O Handlers
//

//---------------------------------------------------------------------------
//
// GCSCommand() - process a command from islServer
//

/*!  
  \brief Process a command from the GCS client's
  \param line string with the GCS request
  \param agw_reply reply string

  This function is setup as a callback for islServer(). This gives the
  client application's information for the GCS/TCS.

  This function parses the interactive command line and calls the
  appropriate low-level cmd_xxx() command action functions for excuting
  most commands.

  All keyboard commands are treated as EXEC: type IMPv2 messages.  The
  downstream cmd_xxx() action functions come from a remote GCS 
  application.

*/

void
GCSCommand(char *line, char agw_reply[])
{
  char cmd[BIG_STR_SIZE];       // command string (oversized)
  char args[BIG_STR_SIZE];      // command-line argument buffer (oversized)
  char reply[BIG_STR_SIZE];     // command reply buffer
  char dummy[BIG_STR_SIZE];

  /*  
  // Variables used to traverse the command tree
  */
  int i;
  int nfound=0;
  int icmd=-1;
  char *message;   // Pointer for the keyboard message buffer
  char *expansion; // Stuff for the history facility
  int result;

  if (line==NULL) return;// If line is NULL, we have nothing to do, return

  if (strlen(line)==0) { // Similarly, if line is blank, return
    sprintf(agw_reply,"ERROR:%s",reply);
    memset(line,0,sizeof(line));
    return;
  }

  /*
  // Allocate memory for the message buffer and clear it
  */
  message = (char *)malloc((ISIS_MSGSIZE)*sizeof(char));
  memset(message,0,sizeof(message));

  /*
  // Copy the command input line into the message buffer 
  // We're all done with the original string from the agwServer, free it
  */
  strcpy(message,line);
  memset(line,0,sizeof(line));

  /*
  // Make sure Ctrl+C is set for motion aborts
  */
  signal(SIGINT,HandleInt);  // reset the SIGINT handler

  /*  
  // Remove any \n terminator on the message string
  */
  if (message[strlen(message)-1]=='\n') message[strlen(message)]='\0';

  /*
  // Clear the command handling strings
  */
  memset(agw_reply,0,sizeof(agw_reply));
  memset(reply,0,sizeof(reply));
  memset(args,0,sizeof(args));
  memset(cmd,0,sizeof(cmd));

  /*
  // Split message into command and argument strings
  // and free its memory
  */
  sscanf(message,"%s %[^\n]",cmd,args);
  memset(message,0,sizeof(message));
  free(message);

  /*
  //*******************
  // Message Handling:
  //*******************
  */

  /*
  // Traverse the command table, matches are case-insensitive, but
  // must be exact word matches (no abbreviations or aliases)
  */
  nfound = 0;
  for (i=0; i<NumCommands; i++) {
    if (strcasecmp(cmdtab[i].cmd,cmd)==0) { 
      nfound++;
      icmd=i;
      break;
    }
  }

  if (nfound == 0) { // Send to the comm port as-is if non-blank
    if (strlen(cmd)>0)
      sprintf(agw_reply,"ERROR:014 unknown command - %s",cmd);
  } else {

    commandID = icmd; // save to identify mechanism name
    switch (cmdtab[icmd].action(args,EXEC,reply)) {    // All messages are treated as EXEC: type messages
      
    case CMD_ERR:
      sprintf(agw_reply,"ERROR:%s",reply);
      break;

    case CMD_OK:
      sprintf(agw_reply,"DONE: %s",reply);
      break;
      
    case CMD_NOOP:
      sprintf(agw_reply,"NOOP:%s",reply);
    default:
      break;
      
    } // end of switch()
  }
}

//---------------------------------------------------------------------------
//
// SocketCommand() - process a message/command from a remote ISIS server/client
//

/*!  
  \brief Process a message/command from a remote ISIS server/client application
  \param buf string with the IMPv2 message received from the remote application

  This function parses a message received from a remote ISIS server or
  client application, and interprets the message.  All EXEC: and
  implicit REQ: type messages are passed to the corresponding cmd_xxx()
  action functions for handling, while the remaining informational
  messages are simply echoed to the console screen.

  More sophisticated handlers might pass such messages on to
  parsers/handlers of their own if the inputs were actually used for
  something other than "visual" information for the user of this
  application.
 
  All messages received from an ISIS node are assumed to be in the
  proper IMPv2 messaging syntax.
 
  Note that EXEC: is new to IMPv2.  It allows remote nodes to transmit
  protected "executive" commands to clients, giving them access to
  commands that would otherwise only be available on the console
  keyboard (e.g., the "quit" command).  Thus a remote EXEC: command
  means "act as if this was typed at the keyboard".  It is the
  responsibility of the remote application to make sure that EXEC: is
  used with care, as you could do something stupid (though your client
  application should not allow actions that would be physically unsafe
  to personnel or equipment).

  \sa KeyboardCommand()
*/

void
SocketCommand(char *buf)
{

  /*
  // ISIS message components 
  */
  char msg[ISIS_MSGSIZE];       // Full ISIS message buffer
  char srcID[ISIS_NODESIZE];    // ISIS message sending node ID
  char destID[ISIS_NODESIZE];   // ISIS message destination node ID
  MsgType msgtype = REQ;        // ISIS message type, defined in isisclient.h
  char msgbody[ISIS_MSGSIZE];   // ISIS message/command body

  /*
  // Command components (command args)
  */
  char cmd[BIG_STR_SIZE];       // command string (oversized)
  char args[BIG_STR_SIZE];      // command-line argument buffer (oversized)
  char reply[BIG_STR_SIZE];     // command reply string
  char dummy[BIG_STR_SIZE];

  /*
  // Other working variables
  */
  int i;
  int nfound=0;
  int icmd=-1;

  /*
  // Some simple initializations
  */
  memset(reply,0,sizeof(reply));
  memset(args,0,sizeof(args));
  memset(cmd,0,sizeof(cmd));

  /*
  // Split the ISIS format message into components
  */
  if (SplitMessage(buf,srcID,destID,&msgtype,msgbody)<0) {
    //    if (appClient.isVerbose) {
    //      printf("\nISIS IN: %s\n",buf);
    //    }
    return;
  }
   
  /*
  // Record the command host.  We use this to authenticate commands bound
  // to a specific host, like LOCK/UNLOCK
  */
  strcpy(appAGW.cmdHost,srcID);

  /*
  // Immediate action depends on the type of message received as
  // recorded by the msgtype code.
  */
  switch(msgtype) {

  case STATUS:  // we've been sent a status message, echo to console
    printf("%s\n",buf);
    break;
	  
  case DONE:    // command completion message (?), echo to console.
    printf("%s\n",buf);
    break;
	  
  case ERROR:   // error messages, echo to console, get fancy later
  case WARNING:
  case FATAL:
    printf("%s\n",buf);
    break;
	  
  case REQ:    // implicit command requests
  case EXEC:   // and executive override commands

    memset(msg,0,ISIS_MSGSIZE);

    sscanf(msgbody,"%s %[^\n]",cmd,args);  // split into command + args

    /*
    // Traverse the command table, exact case-insensitive match required
    */
    nfound = 0;
    for (i=0; i<NumCommands; i++) {
      if (strcasecmp(cmdtab[i].cmd,cmd)==0) { 
	nfound++;
	icmd=i;
	break;
      }
    }

    if (nfound == 0) {
      sprintf(msg,"%s>%s ERROR:014 unknown command - %s\n",
              appClient.ID,srcID,msgbody);
    }
    else {
      switch(cmdtab[icmd].action(args,msgtype,reply)) {

      case CMD_ERR: // command generated an error
	sprintf(msg,"%s>%s ERROR:%s\n",appClient.ID,srcID,reply);
	break;

      case CMD_NOOP: // command is a no-op, debug verbose output only
	if (appClient.isVerbose)
	  printf("IN: %s from ISIS node %s\n",msgbody,srcID);
	break;

      case CMD_OK:  // command executed OK, return reply
      default:
	sprintf(msg,"%s>%s DONE: %s\n",appClient.ID,srcID,reply);
	break;
	
      } // end of switch on cmdtab.action()

    }

    /*
    // An incoming PING requires special handling - it is an exception
    // to the usual messaging syntax since PONG is sent in reply 
    */
    if (strcasecmp(cmd,"PING") == 0)
      sprintf(msg,"%s>%s %s\r",appClient.ID,srcID,reply);
      
    break;

  default:  // we don't know what we got, print for debugging purposes
    printf("Malformed message received on client port: %s\n",buf);
    break;

  } // end of switch(msgtype) -- default falls through with no-op

  /*
  // Do we have something to send back? 
  //
  // If we are configured as an ISIS client (appClient.useISIS=true), send
  // the reply back to the ISIS server for handling with
  // SendToISISServer().
  //
  // If we are configured as standalone (appClient.useISIS=false), send the
  // reply back to the remote host with SendToHost().
  */
  if (strlen(msg)>0) { // we have something to send
    if (appClient.useISIS)
      SendToISISServer(&appClient,msg);
    else 
      ReplyToRemHost(&appClient,msg);

    if (appClient.isVerbose) {
      msg[strlen(msg)-1]='\0';
      printf("OUT: %s\n",msg);
    }
  } // end of reply handling

}
