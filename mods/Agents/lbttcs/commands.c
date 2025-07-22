//
// commands.c - application command interpreter
//

/*!
  \file commands.c 
  \brief lbttcs Client application command interpreter functions.

  This module contains the command "action" functions called to
  service lbttcs client commands.  These consist of a suite of \arg
  "common" client action functions common to most ISIS clients \arg
  client-specific functions that perform the client's particular tasks
  \arg common interface routines for keyboard and ISIS socket
  interfaces.

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
  \arg \c cmd_history() Print the recent interactive command history (HISTORY)

  These are then followed by cmd_xxx() action functions that implement the
  various client tasks.

  At the end of this file are the template I/O handlers used by the
  command interpreter:
  \arg KeyboardCommand() Keyboard command handler (command-line interface)
  \arg SocketCommand() Socket command/message handler (client socket interface)

  \author R. Pogge, OSU Astronomy Dept. (pogge@astronomy.ohio-state.edu)
  \date 2015 Aug 11
*/

#include "client.h"     // Custom client application header file
#include "commands.h"   // Command action functions header file

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

*/

int
cmd_quit(char *args, MsgType msgtype, char *reply)
{
  if (msgtype == EXEC) {
    client.KeepGoing=0;
    sprintf(reply,"%s=DISABLED MODE=OFFLINE",client.ID);
  }
  else {
    strcpy(reply,"Cannot execute quit command - operation not allowed except as EXEC:");
    return CMD_ERR;
  }
  return CMD_OK;
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

  \sa cmd_ping
*/

int
cmd_pong(char *args, MsgType msgtype, char *reply)
{
  if (client.isVerbose && useCLI)
    printf("PONG received\n");
  return CMD_NOOP;
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
*/

int
cmd_version(char *args, MsgType msgtype, char *reply)
{
  
  sprintf(reply,"lbttcs Version=%s CompileDate=%s CompileTime=%s",
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

  \sa cmd_debug
*/

int
cmd_verbose(char *args, MsgType msgtype, char *reply)
{
  if (client.isVerbose && useCLI) {
    client.isVerbose = 0;
    sprintf(reply,"verbose mode disabled");
  }
  else {
    client.isVerbose = 1;
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

  \sa cmd_verbose
*/

int
cmd_debug(char *args, MsgType msgtype, char *reply)
{
  if (client.Debug) {
    client.Debug = 0;
    sprintf(reply,"debugging output disabled");
  }
  else {
    client.Debug = 1;
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

*/

int
cmd_info(char *args, MsgType msgtype, char *reply)
{
  int i;

  // Start with the node ID, and host info

  sprintf(reply,"HostID=%s HostAddr=%s:%d",
	  client.ID, client.Host, client.Port);

  // If configured as an ISIS client, report this and the ISIS host:port info,
  // otherwise if standalone, report that, and the host:port of the last
  // remote host to send us something, if known.

  if (client.useISIS) {
    sprintf(reply,"%s Mode=ISISClient ISIS=%s ISISHost=%s:%d",reply,
	    client.isisID,client.isisHost,client.isisPort);
  }
  else {
    if (strlen(client.remHost)>0)
      sprintf(reply,"%s Mode=STANDALONE RemHost=%s:%d",reply,
	      client.remHost,client.remPort);
    else
      strcat(reply," Mode=STANDALONE");
  }

  // IIF interface information

  sprintf(reply,"%s IIFProxy=%s InstID=%s LBTFocus=%s LBTSide=%s"
	  " propFile=%s",
	  reply,clientProxy_Name,instrument_Name,lbt.focStation,lbt.side,
	  lbt.iceProps);

  // Report application runtime flags

  sprintf(reply,"%s %s %s",reply,
	  ((client.isVerbose) ? "Verbose" : "Concise"),
	  ((client.Debug) ? "+DEBUG" : "-DEBUG"));
	 
  // Finally, report the application's runtime config info as required

  sprintf(reply,"%s rcfile=%s",reply,client.rcFile);

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
  
*/

int
cmd_help(char *args, MsgType msgtype, char *reply)
{
  int i, icmd, found;
  int ls;
  char argbuf[32];

  if (!useCLI) return 0;

  if (msgtype!=EXEC) {
    strcpy(reply,"Cannot exec help command - remote operation not allowed");
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
      printf("Unknown Command '%s' (type 'help' to list all commands)\n",argbuf);
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
    printf("Command History:\n");
    printf("  !!  \t\t - repeat last command\n");
    printf("  !cmd\t\t - repeat last command matching 'cmd'\n");
    printf("  arrow keys for command-line & history editing\n");
  }

  return CMD_NOOP;
    
}

//---------------------------------------------------------------------------
//
// history - show the history list
//

#include <readline/readline.h>  // Gnu readline utility
#include <readline/history.h>   // Gnu history utility

/*!
  \brief HISTORY command - show the application's interactive command history
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_NOOP on success, #CMD_ERR if history executed as a non-EXEC: 
  command

  \par Usage:
  history

  List the application's interactive command history on the console.
  The KeyboardCommand() function uses the GNU readline and history
  system to record all commands entered.  This prints a list of the most
  recent commands.

  \sa KeyboardCommand()
*/

int
cmd_history(char *args, MsgType msgtype, char *reply)
{
  register HIST_ENTRY **the_list;
  register int ihist;

  if (!useCLI) return 0;

  if (msgtype == EXEC) {
    the_list = history_list();
    if (the_list) {
      for (ihist=0; the_list[ihist]; ihist++) 
	printf("%5d   %s\n",ihist+history_base,the_list[ihist]->line);
    }
    return CMD_NOOP;
  }

  // can't do history unless you're on the console...

  strcpy(reply,"Cannot exec history command - remote operation not allowed");
  return CMD_ERR;

}

//***************************************************************************
//
// *** CLIENT-SPECIFIC COMMANDS BEGIN HERE ***
//

/*!  
  \brief tcinit command - initialize the LBT TCS interface
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  tcinit
  iifinit [proxyID instID focStation side]
  
  Initializes the LBT IIF instance, opening connections to the IIF
  server via the ZeroICE system, and performing various system setup.

  If no arguments are given, it uses the current defaults for the IIF
  ICE proxy, LBT-format instrument ID (e.g., MODS or IRTC), the LBT
  focal station code (e.g., directGregorian), and the telescope side
  (left, right, or both) specified in either the runtime configuration
  file at startup, or set previously using the command-line options in
  an iifinit/tcinit command invocation.  This allows us to use this
  agent in an engineering/test situation to override the runtime
  defaults at need.

  \sa cmd_tcclose()
*/

int
cmd_tcinit(char *args, MsgType msgtype, char *reply)
{
  char proxyID[64];
  char instID[32];
  char focStation[32];
  char mySide[10];
  char fullFoc[64];
  int numArgs;

  // if there are command arguments, parse them now

  if (strlen(args)>0) {
    numArgs = sscanf(args,"%s %s %s %s",proxyID,instID,focStation,mySide);
    if (numArgs != 4) {
      strcpy(reply,"Insufficient arguments, usage: tcinit [proxyID instID focStation side]");
      return CMD_ERR;
    }
    strcpy(lbt.proxy,proxyID);
    strcpy(lbt.instID,instID);
    strcpy(lbt.focStation,focStation);
    strcpy(lbt.side,mySide);
  }
  sprintf(fullFoc,"%s %s",lbt.focStation,lbt.side);

  // Do it..

  switch(lbt.opMode) {
  case LIVE:
    if (mods_IIFInit(lbt.proxy,fullFoc,lbt.instID)!=0) {
      sprintf(reply,"Cannot initialize LBT IIF instance - %s",mods_error());
      return CMD_ERR;
    }
    else {
      strcpy(reply,"LBT IIF instance initialized");
      lbt.Link = 1;
    }
    break;

  case LABSIM:
    sprintf(reply,"LBT IIF SIMULATION MODE initialized");
    lbt.Link = 1;
    break;
  }

  // Various common initializations as required...

  lbt.Latitude  = LBTLAT; 
  lbt.Longitude = LBTLON;
  lbt.Altitude  = LBTALT;
  lbt.Turbo = 0;
  return CMD_OK;

}

/*!  
  \brief tcclose command - terminate the LBT IIF instance
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  tcclose
  
  Terminates the LBT IIF instance, shutting down the link to the LBT
  TCS and related systems.

  \sa cmd_tcinit()
*/

int
cmd_tcclose(char *args, MsgType msgtype, char *reply)
{
  switch (lbt.opMode) {
  case LIVE:
    if (mods_IIFClose()!=0) {
      sprintf(reply,"IIF Error, could not terminate IIF instance - %s",
	      mods_error());
      return CMD_ERR;
    }
    strcpy(reply,"lbttcs ICE/IIF instance terminated");
    break;
    
  case LABSIM:
    strcpy(reply,"lbttcs agent simulation closed");
    break;
  }
  lbt.Link = 0;  // link is down
  return CMD_OK;
}

/*!  
  \brief tcstatus command - query the TCS and return status info
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  tcstatus [left|right|both]
  
  Queries the TCS to get the current telescope pointing info and
  system status, and returns this data as an IMPv2-compliant status
  string with the data encoded as FITS-style key=value pairs.

  If the IIF link is down, it returns the time and date information so
  that a minimal FITS header may be constructed.

  If the side is specified explicitly, the TCSTATUS report is tailored
  for the particular focal station.  Without arguments, it returns the
  status of both focal stations.  The side qualifer might be used, for
  example, by the CCD controller to get information for the instrument
  at a particular focal station, whereas the general query is for
  getting global status information.

  In "Turbo" mode, we only query time, but believe that the RA/Dec
  info is current.  This is used for very high-cadence observations
  where we only query the TCS on the first image, then only update the
  UTC time on all subsequent queries, bypassing the latencies in the
  TCS query.

*/

int
cmd_tcstatus(char *args, MsgType msgtype, char *reply)
{
  char csgn;
  int numArgs;
  int isLeft = 0;
  int isBoth = 1;

  if (!lbt.Link) {  // if the IIF link is down, just return time/date info
    getUTCTime(&utc);
    strcpy(lbt.utcDate,utc.ISO);
    strcpy(lbt.utcTime,utc.Time);
    sprintf(reply,"TCSLINK=Down DATE-OBS=%s UTC-OBS=%s"
	    " TIMESYS=UTC MJD-OBS=%.6f",
	    lbt.utcDate,lbt.utcTime,utc.MJD);
    return CMD_OK;
  }

  // Check for command arguments LEFT or RIGHT (aliases are SX and DX,
  // respectively)

  if (strlen(args)>0) {
    if (strcasecmp(args,"LEFT")==0 ||
	strcasecmp(args,"SX")==0) {
      isBoth = 0;
      isLeft = 1;
    }
    else if (strcasecmp(args,"RIGHT")==0 ||
	     strcasecmp(args,"DX")==0) {
      isBoth = 0;
      isLeft = 0;
    }
    else {
      sprintf(reply,"Unrecognized option '%s', usage: TCSTATUS [left|right]",
	      args);
      return CMD_ERR;
    }
  }
  else {
    if (strcasecmp(lbt.side,"left")==0) {
      isBoth = 0;
      isLeft = 1;
    }
    else if (strcasecmp(lbt.side,"right")==0) {
      isBoth = 0;
      isLeft = 0;
    }
    else {
      isBoth = 1;
      isLeft = 0;
    }
  }
	     
  // If the IIF link is up and we're not in turbo mode, get the
  // telescope information

  if (!lbt.Turbo) {
    if (queryLBTTCS()<0) {
      if (useCLI) printf("Error: Could not query the LBT TCS\n");
      getUTCTime(&utc);
      strcpy(lbt.utcDate,utc.ISO);
      strcpy(lbt.utcTime,utc.Time);
      sprintf(reply,"TCSLINK=Down DATE-OBS=%s UTC-OBS=%s TIMESYS=UTC MJD-OBS=%.6f",
	      lbt.utcDate,lbt.utcTime,utc.MJD);
      return CMD_OK;  // return time/date as if OK, but note link down.
    }
  }

  // Build the telescope status string
 
  // Query the time now to remove any IIF query latencies

  getUTCTime(&utc);
  strcpy(lbt.utcDate,utc.ISO);
  strcpy(lbt.utcTime,utc.Time);
  if (lbt.Turbo) HAZDCalc(&lbt,&utc);

  if (lbt.opMode == LIVE) {
    if (lbt.Turbo)
      strcpy(reply,"TCSLINK=Turbo");
    else
      strcpy(reply,"TCSLINK=Live");

    sprintf(reply,"%s DATE-OBS=%s UTC-OBS=%s TIMESYS=UTC"
	    " TELRA=%s TELDEC=%s TELALT=%.5f TELAZ=%.5f EQUINOX=%.1f RADECSYS=FK5"
	    " HA=%s LST-OBS=%s ZD=%.2f AIRMASS=%.2f MJD-OBS=%.6f PARANGLE=%.5f"
	    " OBJNAME='%s' OBJRA=%s OBJDEC=%s OBJPMRA=%.2f OBJPMDEC=%.2f OBJEPOCH=%.2f"
	    " GUINAME='%s' GUIRA=%s GUIDEC=%s GUIPMRA=%.2f GUIPMDEC=%.2f GUIEPOCH=%.2f",
	    reply,lbt.utcDate,lbt.utcTime,
	    lbt.telRA,lbt.telDec,lbt.telEl,lbt.telAz,lbt.Equinox,
	    lbt.HA,lbt.LST,lbt.ZD,lbt.SecZ,utc.MJD,lbt.parAngle,
	    lbt.objName,lbt.objRA,lbt.objDec,lbt.objPMRA,lbt.objPMDec,lbt.objEpoch,
	    lbt.guiName,lbt.guiRA,lbt.guiDec,lbt.guiPMRA,lbt.guiPMDec,lbt.guiEpoch);
  }
  else {
    sprintf(reply,"TCSLINK=Sim DATE-OBS=%s UTC-OBS=%s TIMESYS=UTC"
	    " TELRA=%s TELDEC=%s TELALT=%.5f TELAZ=%.5f EQUINOX=%.1f RADECSYS=FK5"
	    " HA=%s LST-OBS=%s ZD=%.2f AIRMASS=%.2f MJD-OBS=%.6f PARANGLE=%.5f"
	    " OBJNAME='%s' OBJRA=%s OBJDEC=%s OBJPMRA=%.2f OBJPMDEC=%.2f OBJEPOCH=%.2f"
	    " GUINAME='%s' GUIRA=%s GUIDEC=%s GUIPMRA=%.2f GUIPMDEC=%.2f GUIEPOCH=%.2f",
	    lbt.utcDate,lbt.utcTime,
	    lbt.telRA,lbt.telDec,lbt.telEl,lbt.telAz,lbt.Equinox,
	    lbt.HA,lbt.LST,lbt.ZD,lbt.SecZ,utc.MJD,lbt.parAngle,
	    lbt.objName,lbt.objRA,lbt.objDec,lbt.objPMRA,lbt.objPMDec,lbt.objEpoch,
	    lbt.guiName,lbt.guiRA,lbt.guiDec,lbt.guiPMRA,lbt.guiPMDec,lbt.guiEpoch);
  }

  // Telescope-dependent and focus-dependent entries go here

  if (isLeft)
    sprintf(reply,"%s TELESCOP=LBT-SX OBSERVAT=MGIO-LBT"
	    " LATITUDE=%.5f LONGITUD=%.5f ELEVATIO=%.1f"
	    " ROTANGLE=%.5f POSANGLE=%.5f ROTMODE=%s LDGROT=%.5f",
	    reply,lbt.Latitude,lbt.Longitude,lbt.Altitude,
	    lbt.rotAngle,lbt.posAngle,lbt.rotMode,lbt.telRot);
  else
    sprintf(reply,"%s TELESCOP=LBT-DX OBSERVAT=MGIO-LBT"
	    " LATITUDE=%.5f LONGITUD=%.5f ELEVATIO=%.1f"
	    " ROTANGLE=%.5f POSANGLE=%.5f ROTMODE=%s RDGROT=%.5f",
	    reply,lbt.Latitude,lbt.Longitude,lbt.Altitude,
	    lbt.rotAngle,lbt.posAngle,lbt.rotMode,lbt.telRot);

  // Data we only provide on a live TCS link

  if (lbt.opMode == LIVE) {
  
    // LBT mirror collimation data

    sprintf(reply,"%s M1POSX=%.3f M1POSY=%.3f M1POSZ=%.3f M1ROTX=%.3f M1ROTY=%.3f M1ROTZ=%.3f"
	    " M1CTEMP=%.2f M1CTEMP2=%.2f M1GTEMP=%.2f M1ATEMP=%.2f",
	    reply,lbtTCS.m1PosX,lbtTCS.m1PosY,lbtTCS.m1PosZ,
	    lbtTCS.m1RotX,lbtTCS.m1RotY,lbtTCS.m1RotZ,lbtTCS.m1CTemp,lbtTCS.m1CTemp2,
	    lbtTCS.m1GTemp,lbtTCS.m1ATemp);

    sprintf(reply,"%s M2POSX=%.3f M2POSY=%.3f M2POSZ=%.3f M2ROTX=%.3f M2ROTY=%.3f M2ROTZ=%.3f"
	    " M2CTEMP=%.2f",
	    reply,lbtTCS.m2PosX,lbtTCS.m2PosY,lbtTCS.m2PosZ,
	    lbtTCS.m2RotX,lbtTCS.m2RotY,lbtTCS.m2RotZ,lbtTCS.m2CTemp);

    // LBT telescope structure temperature sensors

    sprintf(reply,"%s TTEMP201=%.3f TTEMP202=%.3f TTEMP203=%.3f TTEMP204=%.3f TTEMP205=%.3f"
	    " TTEMP206=%.3f TTEMP207=%.3f TTEMP208=%.3f TTEMP209=%.3f TTEMP210=%.3f",
	    reply,lbtTCS.ttemp201,lbtTCS.ttemp202,lbtTCS.ttemp203,lbtTCS.ttemp204,lbtTCS.ttemp205,
            lbtTCS.ttemp206, lbtTCS.ttemp207,lbtTCS.ttemp208,lbtTCS.ttemp209,lbtTCS.ttemp210);

    sprintf(reply,"%s TTEMP301=%.3f TTEMP302=%.3f TTEMP303=%.3f TTEMP304=%.3f TTEMP305=%.3f"
	    " TTEMP306=%.3f TTEMP307=%.3f TTEMP308=%.3f TTEMP309=%.3f TTEMP310=%.3f",
	    reply,lbtTCS.ttemp301,lbtTCS.ttemp302,lbtTCS.ttemp303,lbtTCS.ttemp304,lbtTCS.ttemp305,
            lbtTCS.ttemp306, lbtTCS.ttemp307,lbtTCS.ttemp308,lbtTCS.ttemp309,lbtTCS.ttemp310);

    sprintf(reply,"%s TTEMP401=%.3f TTEMP402=%.3f TTEMP403=%.3f TTEMP404=%.3f TTEMP405=%.3f"
	    " TTEMP406=%.3f TTEMP407=%.3f TTEMP408=%.3f",
	    reply,lbtTCS.ttemp401,lbtTCS.ttemp402,lbtTCS.ttemp403,lbtTCS.ttemp404,lbtTCS.ttemp405,
            lbtTCS.ttemp406, lbtTCS.ttemp407,lbtTCS.ttemp408);

    // LBT weather station information

    if (lbtTCS.lbtTemp > 100) lbtTCS.lbtTemp = 0.0;
    sprintf(reply,"%s LBTWLINK=%s LBTTEMP=%.2f LBTPRES=%.2f LBTHUM=%.2f LBTDWPT=%.2f",
	    reply, ((lbtTCS.lbtLink) ? "Up":"Down"), 
	    lbtTCS.lbtTemp,lbtTCS.lbtPres,lbtTCS.lbtHum,lbtTCS.lbtDwpt);

    // GCS AGw and Guide Star engineering information

    sprintf(reply,"%s GPREQ_X=%.3f GPREQ_Y=%.3f GPACT_X=%.3f GPACT_Y=%.3f"
	    " GSPRED_X=%.3f GSPRED_Y=%.3f GSMEAS_X=%.3f GSMEAS_Y=%.3f"
	    " GSDELTAX=%.3f GSDELTAY=%.3f",
	    reply,lbtTCS.reqPCSX,lbtTCS.reqPCSY,lbtTCS.actPCSX,lbtTCS.actPCSY,
	    lbtTCS.gsPredX,lbtTCS.gsPredY,lbtTCS.gsMeasX,lbtTCS.gsMeasY,
	    lbtTCS.gsDeltaX,lbtTCS.gsDeltaY);
  }


  // All done, return...
  
  return CMD_OK;
}

/*!  
  \brief gettime command - query UTC date/time with no IIF query
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  gettime
  
  Queries the system clock and returns UTC date and time info in the
  same FITS key=value pair format as a TCSTATUS query, but does not do
  an IIF status query.  This is a way to get precision time info
  without overhead from the IIF queries, which must be serialized and
  can take as much as a second or more depending on how they are
  behaving.

  If there is a TCS link active, it also uses the most recent RA,Dec
  to compute HA, ZD, SecZ, LST, and HJD.

*/

int
cmd_gettime(char *args, MsgType msgtype, char *reply)
{
  getUTCTime(&utc);
  strcpy(lbt.utcDate,utc.ISO);
  strcpy(lbt.utcTime,utc.Time);
  if (HAZDCalc(&lbt,&utc)<0) {
    sprintf(reply,"TCSLINK=GetTime DATE-OBS=%s UTC-OBS=%s TIMESYS=UTC MJD-OBS=%.6f"
	    " HA=none LST-OBS=none ZD=-1.0 AIRMASS=-1.0",
	    lbt.utcDate,lbt.utcTime,utc.MJD);
  }
  else {
    sprintf(reply,"TCSLINK=GetTime DATE-OBS=%s UTC-OBS=%s TIMESYS=UTC MJD-OBS=%.6f"
	    " HA=%s LST-OBS=%s ZD=%.6f AIRMASS=%.2f",
	    lbt.utcDate,lbt.utcTime,utc.MJD,lbt.HA,lbt.LST,lbt.ZD,lbt.SecZ);
  }
  return CMD_OK;
}

/*!  
  \brief telpos command - query the TCS for just position info
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  telpos [left|right|both]
  
  Queries the TCS to get the current telescope pointing info.  This
  returns only a small subset of TCSTATUS info, and is primarily for
  quickly asking "where is LBT pointing right now?".

  If the side is specified explicitly, the TELPOS report is tailored
  for the particular focal station.  Without arguments, it returns the
  status of both focal stations.  The side qualifer might be used, for
  example, by the CCD controller to get information for the instrument
  at a particular focal station, whereas the general query is for
  getting global status information.

  Use TCSTATUS for proper full FITS information

*/

int
cmd_telpos(char *args, MsgType msgtype, char *reply)
{
  char csgn;
  int numArgs;
  int isLeft = 0;
  int isBoth = 1;

  if (!lbt.Link) {  // if the IIF link is down, nothing to say
    strcpy(reply,"No current pointing info available, TCSLINK=Down");
    return CMD_OK;
  }

  // Check for command arguments LEFT or RIGHT (aliases are SX and DX,
  // respectively)

  if (strlen(args)>0) {
    if (strcasecmp(args,"LEFT")==0 ||
	strcasecmp(args,"SX")==0) {
      isBoth = 0;
      isLeft = 1;
    }
    else if (strcasecmp(args,"RIGHT")==0 ||
	     strcasecmp(args,"DX")==0) {
      isBoth = 0;
      isLeft = 0;
    }
    else {
      sprintf(reply,"Unrecognized option '%s', usage: TELPOS [left|right]",
	      args);
      return CMD_ERR;
    }
  }
  else {
    if (strcasecmp(lbt.side,"left")==0) {
      isBoth = 0;
      isLeft = 1;
    }
    else if (strcasecmp(lbt.side,"right")==0) {
      isBoth = 0;
      isLeft = 0;
    }
    else {
      isBoth = 1;
      isLeft = 0;
    }
  }
	     
  // If the IIF link is up, get the pointing info

  if (queryLBTTCS()<0) {
    if (useCLI) printf("Error: Could not query the LBT TCS\n");
    getUTCTime(&utc);
    strcpy(lbt.utcDate,utc.ISO);
    strcpy(lbt.utcTime,utc.Time);
    strcpy(reply,"Could not get current pointing info, TCSLINK=Down");
    return CMD_OK;
  }

  // Build the telescope position string
 
  // Query the time now to remove any IIF query latencies

  getUTCTime(&utc);
  strcpy(lbt.utcDate,utc.ISO);
  strcpy(lbt.utcTime,utc.Time);
  if (lbt.Turbo) HAZDCalc(&lbt,&utc);

  if (lbt.opMode == LIVE) {
    sprintf(reply,"DATE-OBS=%s UTC-OBS=%s TIMESYS=UTC"
	    " TELRA=%s TELDEC=%s TELALT=%.5f TELAZ=%.5f"
	    " HA=%s LST-OBS=%s ZD=%.2f AIRMASS=%.2f MJD-OBS=%.6f PARANGLE=%.5f",
	    lbt.utcDate,lbt.utcTime,lbt.telRA,lbt.telDec,lbt.telEl,lbt.telAz,
	    lbt.HA,lbt.LST,lbt.ZD,lbt.SecZ,utc.MJD,lbt.parAngle);
  }
  else {
    sprintf(reply,"TCSLINK=Sim DATE-OBS=%s UTC-OBS=%s TIMESYS=UTC"
	    " TELRA=%s TELDEC=%s TELALT=%.5f TELAZ=%.5f"
	    " HA=%s LST-OBS=%s ZD=%.2f AIRMASS=%.2f MJD-OBS=%.6f PARANGLE=%.5f",
	    lbt.utcDate,lbt.utcTime,lbt.telRA,lbt.telDec,lbt.telEl,lbt.telAz,
	    lbt.HA,lbt.LST,lbt.ZD,lbt.SecZ,utc.MJD,lbt.parAngle);
  }

  // Telescope-dependent and focus-dependent entries go here

  if (isLeft)
    sprintf(reply,"%s TELESCOP=LBT-SX"
	  " ROTANGLE=%.5f POSANGLE=%.5f ROTMODE=%s LDGROT=%.5f",
	    reply,lbt.rotAngle,lbt.posAngle,lbt.rotMode,lbt.telRot);
  else
    sprintf(reply,"%s TELESCOP=LBT-DX"
	  " ROTANGLE=%.5f POSANGLE=%.5f ROTMODE=%s RDGROT=%.5f",
	    reply,lbt.rotAngle,lbt.posAngle,lbt.rotMode,lbt.telRot);

  // All done, return...
  
  return CMD_OK;
}

/*!  
  \brief STATUS command - report the client status
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  status

  Returns the current client status info as an IMPv2-compliant
  status message.  

*/

int
cmd_status(char *args, MsgType msgtype, char *reply)
{
  if (lbt.Link)
    strcpy(reply,"TCSLINK=UP");
  else
    strcpy(reply,"TCSLINK=Down");

  switch(lbt.opMode) {
  case LIVE:
    strcat(reply," TCSMODE=Live");
    break;
  case LABSIM:
    strcat(reply," TCSMODE=Sim");
    break;
  }

  sprintf(reply,"%s TELESCOPE=%s OBSERVAT='MGIO-LBT'",
	  reply,lbt.telescope);

  // IIF interface information

  sprintf(reply,"%s IIFProxy=%s InstID=%s LBTFocus=%s LBTSide=%s",
	  reply,clientProxy_Name,instrument_Name,lbt.focStation,lbt.side);

  if (queryLBTTCS()<0) {
    strcpy(reply,"Cannot query the LBT TCS");
    return CMD_ERR;
  }

  return CMD_OK;
}

/*!  
  \brief CONFIG command - report the TCS configuration [engineering]
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  config

  Engineering command that dumps what we know about the current
  configuration to the console.  

  Also reports the last TCS information it knows about, including the
  time of the last status query to the IIF server.

*/

int
cmd_config(char *args, MsgType msgtype, char *reply)
{

  // Print the last query info

  printLBTInfo(&lbt);

  return CMD_OK;
}

/*!  
  \brief +TURBO command - set Turbo (time-only) query mode
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  +turbo

  Sets Turbo query mode, whereby only time info is updated and the TCS
  is "believed" to be current.  This rapid-response mode is used for
  high-cadence observations where we "know" the telescope pointing is
  the same, and we want immediate time updates.

  Turbo mode stays in effect until either -turbo or tcinit or restart.

  To ensure that the TCS info is current, we make a full TCS status
  query now before returning.

  \sa cmd_setfull
*/

int
cmd_setturbo(char *args, MsgType msgtype, char *reply)
{
  lbt.Turbo = 1;
  strcpy(reply,"Turbo query mode enabled");

  // To make absolutely sure that the TCS info is current

  if (queryLBTTCS()<0) 
    strcpy(reply,"Turbo query mode enabled, but could not query TCS");
  else
    strcpy(reply,"Turbo query mode enabled, TCS info current");

  return CMD_OK;
}

/*!  
  \brief -TURBO command - restore full TCS query mode
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  -turbo

  Disables Turbo query mode, restoring full TCS queries on tcstatus.
  Full query mode is the default mode upon startup and after "tcinit" 
  commands.

  \sa cmd_setturbo
*/

int
cmd_setfull(char *args, MsgType msgtype, char *reply)
{
  lbt.Turbo = 0;
  strcpy(reply,"Turbo query mode disabled");
  
  return CMD_OK;
}

/*!  
  \brief OBJNAME command - set/query target object name
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  objname string

  Sets the target object name for the next preset.

  \sa cmd_objcoords, cmd_preset, cmd_guiname
*/

int
cmd_objname(char *args, MsgType msgtype, char *reply)
{
  char argbuf[32];

  // Must have someone to talk to...

  if (!lbt.Link) {
    sprintf(reply,"LBT TCS link not enabled");
    return CMD_ERR;
  }

  if (strlen(args)>0)
    strcpy(lbt.objName,args);

  sprintf(reply,"OBJNAME='%s'",lbt.objName);
  return CMD_OK;
}

/*!  
  \brief OBJCOORDS command - set/query the target object RA/Dec coordinates
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  objcoords RA Dec [pmr pmd epoch]

  Sets the target object RA and Dec for the next preset.  Optionally
  include the proper motion (in mas/year) and epoch for the object.

  RA must be sexigesimal hours (hh:mm:ss.s), and Dec in sexigesmial
  degrees (+/-dd:mm:ss.s) - SIGN REQUIRED.  We will assume Equinox is
  J2000 for this implementation.

  Proper motion is expressed in units of mas/year, and epoch refers to
  the (RA,Dec) location in the J2000 Equinox for the object.  If not
  given, they are set to 0.0, 0.0, and 2000.0, respectively.

  At present we do not provide a means to enter the object magnitude
  as nobody has come up with a good reason to provide this information
  to the IIF.

  \sa cmd_objname, cmd_preset, cmd_guicoords
*/

int
cmd_objcoords(char *args, MsgType msgtype, char *reply)
{
  char argbuf[32];
  char RA[32];
  char Dec[32];
  double dra;
  double ddec;
  double pmr, pmd, epoch;
  double decSign;
  char cmdstr[128];
  int  numArgs;

  int rah, ram;
  double ras;
  int decd, decm;
  double decs;
  char dsign[2];

  // Must have someone to talk to...

  if (!lbt.Link) {
    sprintf(reply,"LBT TCS link not enabled");
    return CMD_ERR;
  }

  // Parse the arguments and validate the range
  
  if (strlen(args)>0) {
    numArgs = sscanf(args,"%s %s %lf %lf %lf",RA,Dec,&pmr,&pmd,&epoch);
    if (numArgs == 2) {
      lbt.objPMRA = 0.0;
      lbt.objPMDec = 0.0;
      lbt.objEpoch = 2000.0;
    }
    else if (numArgs = 5) {
      lbt.objPMRA = pmr;
      lbt.objPMDec = pmd;
      lbt.objEpoch = epoch;
    }
    else {
      strcpy(reply,"Insufficient arguments, usage: objcoords RA Dec [pmr pmd epoch]");
      return CMD_ERR;
    }
    sscanf(RA,"%d:%d:%lf",&rah,&ram,&ras);
    sscanf(Dec,"%1s%d:%d:%lf",dsign,&decd,&decm,&decs);

    // Look for a sign on the declination.  If none, assume +

    if (strcmp(dsign,"-")==0) 
      decSign = -1;
    else if (strcmp(dsign,"+")==0) 
      decSign = +1;
    else { // oops, no sign means implicit +, rescan Dec
      sscanf(Dec,"%d:%d:%lf",&decd,&decm,&decs);
      decSign = +1;
    }

    if (client.Debug && useCLI) printf("ras=%.3f decs=%.3f\n",ras,decs);
    dra = (double)(rah) + ((double)(ram)/60.0) + ((double)(ras)/3600.0) ;
    if (dra < 0.0 || dra >= 24.0) {
      sprintf(reply,"Invalid RA '%s', must be 0..24h in hh:mm:ss.s format",RA);
      return CMD_ERR;
    }
    ddec = (double)(decd) + ((double)(decm)/60.0) + ((double)(decs)/3600.0) ;
    ddec = decSign*ddec ;
    if (ddec < -90.0 || ddec > 90.0) {
      sprintf(reply,"Invalid Dec '%s', must be -90..+90 in dd:mm:ss.s format",Dec);
      return CMD_ERR;
    }
    strcpy(lbt.objRA,RA);
    strcpy(lbt.objDec,Dec);
    if (lbt.opMode == LABSIM) {
      strcpy(lbt.telRA,RA);   // for the simulation, telXXX = objXXX
      strcpy(lbt.telDec,Dec);
    }
    // convert to radians for subsequent IIF calls
    lbt.objRARad = hrrad(dra); // defined in skyutils.h
    lbt.objDecRad = degrad(ddec);
    
  }
  sprintf(reply,"OBJRA=%s OBJDEC=%s OBJPMRA=%.2f OBJPMDEC=%.2f OBJEPOCH=%.2f"
	  " OBJRA_R=%.8f OBJDEC_R=%.8f",
	  lbt.objRA,lbt.objDec,lbt.objPMRA,lbt.objPMDec,lbt.objEpoch,
	  lbt.objRARad,lbt.objDecRad);
  return CMD_OK;
}

/*!  
  \brief GUINAME command - set/query the guide object name
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  guiname string

  Sets the target guiect name for the next preset.

  \sa cmd_guicoords, cmd_preset, cmd_guiname
*/

int
cmd_guiname(char *args, MsgType msgtype, char *reply)
{
  char argbuf[32];

  // Must have someone to talk to...

  if (!lbt.Link) {
    sprintf(reply,"LBT TCS link not enabled");
    return CMD_ERR;
  }

  if (strlen(args)>0)
    strcpy(lbt.guiName,args);

  sprintf(reply,"GUINAME='%s'",lbt.guiName);
  return CMD_OK;
}

/*!  
  \brief GUICOORDS command - set/query the guide object RA/Dec coordinates
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  guicoords RA Dec [pmr pmd epoch] [mag]

  Sets the guide object RA and Dec for the next preset.  Optionally
  provide the proper motion and epoch of the guide object.

  RA must be ub sexigesimal hours (hh:mm:ss.s), and Dec in sexigesmial
  degrees (+/-dd:mm:ss.s) - SIGN REQUIRED.  We will assume Equinox is
  J2000 for this implementation.

  If the guide object has a measured proper motion, the user may give
  the proper motion in mas/year and epoch in decimal years.  Defaults
  if not given are 0.0, 0.0, and 2000.0, respectively.

  The optional guide star magnitude may be given either with
  coordinates but no proper motion, or after the proper motion and
  epoch values.

  \sa cmd_guiname, cmd_preset, cmd_objcoords
*/

int
cmd_guicoords(char *args, MsgType msgtype, char *reply)
{
  char argbuf[32];
  char RA[32];
  char Dec[32];
  double dgra;
  double dgdec;
  double dtmp1,dtmp2,dtmp3,dtmp4;
  char cmdstr[128];
  int  numArgs;
  int rah, ram;
  double ras;
  int decd, decm;
  double decs;
  char dsign[2];
  double decSign;

  // Must have someone to talk to...

  if (!lbt.Link) {
    sprintf(reply,"LBT TCS link not enabled");
    return CMD_ERR;
  }

  // Parse the arguments and validate the ranges.  We also
  // set the defaults for the 

  if (strlen(args)>0) {
    numArgs = sscanf(args,"%s %s %lf %lf %lf %lf",RA,Dec,&dtmp1,&dtmp2,&dtmp3,&dtmp4);
    if (numArgs == 2) {
      lbt.guiPMRA = 0.0;
      lbt.guiPMDec = 0.0;
      lbt.guiEpoch = 2000.0;
      lbt.guiMag = 0.0;
    }
    else if (numArgs == 3) {
      lbt.guiPMRA = 0.0;
      lbt.guiPMDec = 0.0;
      lbt.guiEpoch = 2000.0;
      lbt.guiMag = dtmp1;
    }
    else if (numArgs == 5) {
      lbt.guiPMRA = dtmp1;
      lbt.guiPMDec = dtmp2;
      lbt.guiEpoch = dtmp3;
      lbt.guiMag = 0.0;
    }
    else if (numArgs == 6) {
      lbt.guiPMRA = dtmp1;
      lbt.guiPMDec = dtmp2;
      lbt.guiEpoch = dtmp3;
      lbt.guiMag = dtmp4;
    }
    else {
      strcpy(reply,"Insufficient arguments, usage: guicoords RA Dec [pmr pmd epoch] [mag]");
      return CMD_ERR;
    }
    sscanf(RA,"%d:%d:%lf",&rah,&ram,&ras);
    sscanf(Dec,"%1s%d:%d:%lf",dsign,&decd,&decm,&decs);

    // Look for a sign on the declination.  If none, assume +

    if (strcmp(dsign,"-")==0) 
      decSign = -1;
    else if (strcmp(dsign,"+")==0) 
      decSign = +1;
    else { // oops, no sign means implicit +, rescan Dec
      sscanf(Dec,"%d:%d:%lf",&decd,&decm,&decs);
      decSign = +1;
    }

    dgra = (double)(rah) + ((double)(ram)/60.0) + ((double)(ras)/3600.0) ;
    if (dgra < 0.0 || dgra >= 24.0) {
      sprintf(reply,"Invalid RA '%s', must be 0..24h in hh:mm:ss.s format",RA);
      return CMD_ERR;
    }
    dgdec = (double)(decd) + ((double)(decm)/60.0) + ((double)(decs)/3600.0) ;
    dgdec = decSign*dgdec ;
    if (dgdec < -90.0 || dgdec > 90.0) {
      sprintf(reply,"Invalid Dec '%s', must be -90..+90 in dd:mm:ss.s format",Dec);
      return CMD_ERR;
    }
    strcpy(lbt.guiRA,RA);
    strcpy(lbt.guiDec,Dec);

    // convert to radians for the IIF calls

    lbt.guiRARad = hrrad(dgra); // defined in skyutils.h
    lbt.guiDecRad = degrad(dgdec);
  }
  sprintf(reply,"GUIRA=%s GUIDEC=%s GUIPMRA=%.2f GUIPMDEC=%.2f GUIEPOCH=%.2f GUIMAG=%.2f"
	  " GUIRA_R=%.8f GUIDEC_R=%.8f",
	  lbt.guiRA,lbt.guiDec,lbt.guiPMRA,lbt.guiPMDec,lbt.guiEpoch,lbt.guiMag,
	  lbt.guiRARad,lbt.guiDecRad);
  return CMD_OK;
}

/*!  
  \brief ROTATOR command - set/query the instrument rotator angle and mode
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  rotator angle [position|parallactic|idle]

  Sets the rotator angle and operating mode of the instrument rotator
  for the next preset.  Angle must be given in decimal degrees.

  mode=position sets the celestial position angle on the sky to
  maintain while tracking.

  mode=parallactic will set the rotator to the given parallactic angle.

  mode=idle will set the physical angle of the ISS rotator and holds the
  position without tracking.

  The rotator position is set by the next preset command for the
  active side of the telescope (left, right, or both).

  \sa cmd_rotmode, cmd_preset
*/

int
cmd_rotator(char *args, MsgType msgtype, char *reply)
{
  double PA;
  char rotMode[32];
  int  numArgs;

  // Must have someone to talk to...

  if (!lbt.Link) {
    sprintf(reply,"LBT TCS link not enabled");
    return CMD_ERR;
  }

  if (strlen(args)>0) {
    numArgs = sscanf(args,"%lf %s",&PA,rotMode);
    if (numArgs != 2) {
      strcpy(reply,"Insufficient arguments, usage: rotator angle mode");
      return CMD_ERR;
    }
    if (strcasecmp(rotMode,"POSITION")==0)
       strcpy(lbt.rotMode,"POSITION");
    else if  (strcasecmp(rotMode,"PARALLACTIC")==0)
       strcpy(lbt.rotMode,"PARALLACTIC");
    else if  (strcasecmp(rotMode,"IDLE")==0)
       strcpy(lbt.rotMode,"IDLE");
    else {
      sprintf(reply,"Invalid rotator mode '%s', must be POSITION, PARALLACTIC, or IDLE",
	      rotMode);
      return CMD_ERR;
    }
    lbt.reqPA = PA;  // need to validate this someday...
  }
  sprintf(reply,"ROTANGLE=%.5f ROTMODE=%s",lbt.reqPA,lbt.rotMode);
  return CMD_OK;
}

/*!  
  \brief REFWAVE command - set/query the target reference wavelength
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  refwave lambda [left|right|both]

  Sets the target reference wavelength for the current or next preset
  in microns.  If the optional side is not specified, the default side
  is used.

  \sa cmd_preset
*/

int
cmd_refwave(char *args, MsgType msgtype, char *reply)
{
  float lam;
  int  numArgs;
  char mySide[16];

  // Must have someone to talk to...

  if (!lbt.Link) {
    sprintf(reply,"LBT TCS link not enabled");
    return CMD_ERR;
  }

  // If arguments treat as a command, otherwise treat as a query 

  if (strlen(args)>0) {
    numArgs = sscanf(args,"%f %s",&lam,mySide);
    if (numArgs == 1)
      strcpy(mySide,lbt.side);
    else if (numArgs == 2) {
      if (strcasecmp(mySide,"left")==0)
	strcpy(mySide,"left");
      else if (strcasecmp(mySide,"right")==0)
	strcpy(mySide,"right");
      else if (strcasecmp(mySide,"both")==0)
	strcpy(mySide,"both");
      else {
	sprintf(reply,"Unknown side '%s', usage: refwave lambda [left|right|both]",
		mySide);
	return CMD_ERR;
      }
    }
    else {
      sprintf(reply,"Invalid arguments '%s', usage: refwave lambda [left|right|both]",
	      args);
      return CMD_ERR;
    }

    // Validate the wavelength

    if (lam < 0.3 || lam > 15.0) {
      sprintf(reply,"Invalid wavelength %.2f, must be 0.3 to 15 microns",
	      lam);
      return CMD_ERR;
    }
  }
  else {
    sprintf(reply,"REFWAVE=%.2f",lbt.lambda);
    return CMD_OK;
  }

  switch (lbt.opMode) {
  case LIVE:
    if (mods_UpdateTargetWavelength(lam,mySide)!=0) {
      sprintf(reply,"IIF error - %s",mods_error());
      return CMD_ERR;
    }
    else
      sprintf(reply,"REFWAVE=%.2f lbtSide=%s",lam,mySide);
    break;
    
  case LABSIM:
    sprintf(reply,"REFWAVE=%.2f",lam);
    break;
  }
  lbt.lambda = lam;
  return CMD_OK;
}

/*!  
  \brief SIDE command - set/query the active telescope side
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  side [left|right|both]

  Sets the active side of the telescope for the next telescope command.
  Must be one of "left", "right" or "both.

  \sa cmd_preset, cmd_skyoffset, cmd_slitoffset, cmd_rotoffset
*/

int
cmd_side(char *args, MsgType msgtype, char *reply)
{
  // Must have someone to talk to...

  if (!lbt.Link) {
    sprintf(reply,"LBT TCS link not enabled");
    return CMD_ERR;
  }

  if (strlen(args)>0) {
    if (strcasecmp(args,"left")==0)
      strcpy(lbt.side,"left");
    else if (strcasecmp(args,"right")==0)
      strcpy(lbt.side,"right");
    else if (strcasecmp(args,"both")==0)
      strcpy(lbt.side,"both");
    else {
      sprintf(reply,"Invalid argument '%s', must be one of {left,right,both}",args);
      return CMD_ERR;
    }
  }
  sprintf(reply,"LBTSIDE=%s",lbt.side); // need to verify this is the right keyword
  return CMD_OK;
}

/*!  
  \brief PRESET command - send a telescope preset
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  preset active|guide|acquire|track|static [left|right|both]

  Tells the telescope to slew to the coordinates given by the most
  recent OBSNAME, OBSCOORDS, GUINAME, and GUICOORDS commands.

  ACTIVE slews the telescope, sets the rotator, starts tracking at
  target, sets the guide probe, then closes the autoguider and
  off-axis WFS for active optics correction loops.

  GUIDE slews the telescope, sets the rotator, starts tracking at
  target, set the guide probe, then closes the autoguider loop, but
  not the off-axis WFS.

  ACQUIRE slews the telescope, sets the rotator, starts tracking at
  target, set the guide probe, then starts acquiring guide camera
  images but does not close the guide or off-axis WFS control loops.

  TRACK slews the telescope, sets the rotator and starts tracking, but
  does not setup the guider or off-axis WFS.

  STATIC slews the telescope and sets the rotator, but does not track
  or setup the guider or off-axis WFS.  This latter mode is only
  useful for static pointing for engineering.

  If side is not specified, assume the default side, otherwise it must
  be either left, right, or both.

  \sa cmd_objcoords, cmd_objname, cmd_guicoords, cmd_guiname, cmd_refwave, cmd_rotator
*/

int
cmd_preset(char *args, MsgType msgtype, char *reply)
{
  char reqMode[32];
  double paRad;
  char mySide[16];
  int numArgs;
  double objPMRARad, objPMDecRad;
  double guiPMRARad, guiPMDecRad;

  if (strlen(args)==0) {
    strcpy(reply,"Insufficient arguments, usage: preset active|track|guide|acquire [side]");
    return CMD_ERR;
  }

  numArgs = sscanf(args,"%s %s",reqMode,mySide);
  if (strcasecmp(reqMode,"ACTIVE")==0)
    strcpy(lbt.presetMode,"ACTIVE");
  else if (strcasecmp(reqMode,"GUIDE")==0)
    strcpy(lbt.presetMode,"GUIDE");
  else if (strcasecmp(reqMode,"TRACK")==0)
    strcpy(lbt.presetMode,"TRACK");
  else if (strcasecmp(reqMode,"ACQUIRE")==0)
    strcpy(lbt.presetMode,"ACQUIRE");
  else if (strcasecmp(reqMode,"STATIC")==0)
    strcpy(lbt.presetMode,"STATIC");
  else {
    sprintf(reply,"Unknown preset mode '%s', usage: preset active|track|guide|acquire [side]",
	    args);
    return CMD_ERR;
  }
  if (numArgs == 1)
    strcpy(mySide,lbt.side);  // use the default side
  else {
    if (strcasecmp(mySide,"left")==0)
      strcpy(mySide,"left");
    else if (strcasecmp(mySide,"right")==0)
      strcpy(mySide,"right");
    else if (strcasecmp(mySide,"both")==0)
      strcpy(mySide,"both");
    else {
      sprintf(reply,"Unknown side '%s', usage: preset active|track|guide|acquire [left|right|both]",
	      mySide);
      return CMD_ERR;
    }
  }

  // Set it up...

  switch (lbt.opMode) {
  case LIVE:
    if (mods_ClearStars()!=0) {
      sprintf(reply,"IIF error - %s",mods_error());
      return CMD_ERR;
    }
    // Proper motions must be converted to radians...
    objPMRARad = degrad(lbt.objPMRA/3600000.0);
    objPMDecRad = degrad(lbt.objPMDec/3600000.0);
    guiPMRARad = degrad(lbt.guiPMRA/3600000.0);
    guiPMDecRad = degrad(lbt.guiPMDec/3600000.0);

    if (mods_SetStars(lbt.objName,lbt.objRARad,lbt.objDecRad,lbt.objEpoch,
		      objPMRARad,objPMDecRad,
		      lbt.guiName,lbt.guiRARad,lbt.guiDecRad,lbt.guiEpoch,
		      guiPMRARad,guiPMDecRad,
		      lbt.guiMag,lbt.lambda) != 0) {
      sprintf(reply,"IIF error - %s",mods_error());
      return CMD_ERR;
    }
    paRad = degrad(lbt.reqPA);
    if (mods_PresetTelescope(paRad,lbt.rotMode,lbt.presetMode,mySide) != 0) {
      sprintf(reply,"IIF error - %s",mods_error());
      return CMD_ERR;
    }
    sprintf(reply,"%s Preset Completed lbtSide=%s",lbt.presetMode,mySide);
    break;
    
  case LABSIM:
    sprintf(reply,"%s Preset Completed (simulation mode)",lbt.presetMode);
    break;
  }
  return CMD_OK;
}

/*!  
  \brief OFFSETPOINTING command - sends a telescope pointing offset
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  offsetPointing dPA dRA dDec [detxy|radec] [abs|rel] [left|right|both]

  Sends a generalized OffsetPointing command to the telescope using
  the full IIF command syntax.  This is an expert-level command
  used for engineering or very low level operations like slit mask
  alignment where we need to perform the offset in the most general
  way understood by the LBT.  This is sufficiently different from how
  astronomers have come to understand "offset" that we set it apart
  as a separate command.  It combines pointing and rotation offsets.
  For this reason we provide separated offsetting commands that
  will be more familiar to astronomers.  The only concession to
  astronomers is we accept arcsec and degrees and translate into
  the radians required by the IIF.

  NOTE the order of arguments is the same as for the IIF call...

  dPA is the rotator offset in degrees.

  dRA and dDec are in units of arcseconds on the tangent plane to the
  celestial sphere (i.e., cos(Dec) corrections are applied to dRA) for
  RADEC coordinates, or arcseconds in the rotator PA-invariant PCS_XY
  coordinate system which is nominally aligned with detector pixels,
  but in MODS is aligned with the slitmask xy in the common focal
  plane.

  See skyoffset of RA/Dec offsets without rotator offset
  
  See slitoffset for DETXY X/Y offsets without rotator offset

  See rotoffset for rotator offsets with no XY/RaDec offset

  \sa cmd_update, cmd_slitoffset, cmd_rotoffset, cmd_skyoffset
*/

int
cmd_offset(char *args, MsgType msgtype, char *reply)
{
  char argStr1[32];
  char argStr2[32];
  double dRA;
  double dDec;
  double dPA;
  int numArgs;
  char mySide[16];
  char coordSys[16];
  char offMode[16];

  // Check for an LBT TCS link first

  if (!lbt.Link) {
    sprintf(reply,"LBT TCS link not enabled");
    return CMD_ERR;
  }

  // We require arguments

  if (strlen(args)==0) {
    strcpy(reply,"Insufficient arguments, usage: offsetpointing dPA dRA dDec [radec|detxy] [rel|abs] [side]");
    return CMD_ERR;
  }

  numArgs = sscanf(args,"%lf %lf %lf %s %s %s",&dPA,&dRA,&dDec,coordSys,offMode,argStr1);
  if (numArgs == 5) {
    strcpy(mySide,lbt.side);
  }
  else if (numArgs == 6) { 
    if (strcasecmp(argStr1,"left")==0)
      strcpy(mySide,"left");
    else if (strcasecmp(argStr1,"right")==0)
      strcpy(mySide,"right");
    else if (strcasecmp(argStr1,"both")==0)
      strcpy(mySide,"both");
    else {
      sprintf(reply,"Unknown side '%s', usage: offsetpointing dPA dRA dDec [radec|detxy] [abs|rel] [left|right|both]",
	      argStr1);
      return CMD_ERR;
    }
  }
  else {
    strcpy(reply,"Insufficient arguments, usage: offsetpointing dPA dRA dDec [radec|detxy] [abs|rel] [left|right|both]");
    return CMD_ERR;
  }

  if (strcasecmp(coordSys,"radec")==0) 
    strcpy(coordSys,"RADEC");
  else if (strcasecmp(coordSys,"detxy")==0) 
    strcpy(coordSys,"DETXY");
  else {
    sprintf(reply,"Unknown coordinate system '%s', usage: offsetpointing dPA dRA dDec [radec|detxy] [abs|rel] [left|right|both]",
	    coordSys);
    return CMD_ERR;
  }
      
  if (strcasecmp(offMode,"REL")==0)
    strcpy(offMode,"REL");
  else if (strcasecmp(offMode,"ABS")==0)
    strcpy(offMode,"ABS");
  else {
    sprintf(reply,"Unknown offset mode '%s', usage: offsetpointing dPA dRA dDec [radec|detxy] [abs|rel] [left|right|both]",
	    offMode);
    return CMD_ERR;
  }

  // convert to radians for the IIF

  dRA = degrad(dRA/3600.0); // defined in skyutils.h
  dDec = degrad(dDec/3600.0);
  dPA = degrad(dPA);
  
  // Eventualy validate offset requests other than mode once we know
  // what sensible limits are...

  switch (lbt.opMode) {
  case LIVE:
    if (mods_OffsetPointing(dPA,dRA,dDec,coordSys,offMode,mySide)!=0) {
      sprintf(reply,"IIF error - %s",mods_error());
      return CMD_ERR;
    }
    sprintf(reply,"%s %S OffsetPointing Completed lbtSide=%s",offMode,coordSys,mySide);
    break;
    
  case LABSIM:
    sprintf(reply,"%s %s OffsetPointing Completed (simulation mode)",offMode,coordSys);
    break;
  }
  return CMD_OK;
}

/*!  
  \brief SKYOFFSET command - sends a telescope RA/Dec pointing offset
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  skyoffset dRA dDec abs|rel [left|right|both]
  skyoffset home [left|right|both]

  Tells the telescope to offset pointing in RA, and Dec in either
  relative or absolute units.  dRA and dDec are in arcseconds on the
  tangent plane to the celestial sphere (i.e., cos(Dec) corrections
  are applied to dRA).

  See the slitoffset command for a version of offset that uses DETXY
  coordinates to execute the offset in the PCS_XY coordinate system
  plane relative to the slit (x,y) rather than celestial coordinates
  (e.g., for dithering a target along the slit, aligning a target with
  a long-slit or multi-slit, etc.)

  See the rotoffset command for applying a *relative* rotator offset.

  \sa cmd_update, cmd_slitoffset, cmd_rotoffset, cmd_offset
*/

int
cmd_skyoffset(char *args, MsgType msgtype, char *reply)
{
  char offMode[32];
  char argStr1[32];
  char argStr2[32];
  double dRA;
  double dDec;
  double dPA = 0.0;
  int numArgs;
  char mySide[16];

  // Check for an LBT TCS link first

  if (!lbt.Link) {
    sprintf(reply,"LBT TCS link not enabled");
    return CMD_ERR;
  }

  // We require arguments

  if (strlen(args)==0) {
    strcpy(reply,"Insufficient arguments, usage: skyoffset dRA dDec [rel|abs] [side]");
    return CMD_ERR;
  }

  // Look at the first argument, see if it is HOME, which will execute
  // an absolute offset to 0,0 to the pointing reference.

  GetArg(args,1,argStr1);
  if (strcasecmp(argStr1,"HOME")==0) {
    dRA = 0.0;
    dDec = 0.0;
    strcpy(offMode,"ABS");
    GetArg(args,2,argStr2);
    if (strlen(argStr2) > 0) {
      if (strcasecmp(argStr2,"left")==0)
	strcpy(mySide,"left");
      else if (strcasecmp(argStr2,"right")==0)
	strcpy(mySide,"right");
      else if (strcasecmp(argStr2,"both")==0)
	strcpy(mySide,"both");
      else {
	sprintf(reply,"Unknown argument '%s', usage: skyoffset home [left|right|both]",
		argStr2);
	return CMD_ERR;
      }
    }
    else 
      strcpy(mySide,lbt.side);
  }
  else {
    numArgs = sscanf(args,"%lf %lf %s %s",&dRA,&dDec,offMode,argStr1);
    if (numArgs == 3) {
      strcpy(mySide,lbt.side);
    }
    else if (numArgs == 4) { 
      if (strcasecmp(argStr1,"left")==0)
	strcpy(mySide,"left");
      else if (strcasecmp(argStr1,"right")==0)
	strcpy(mySide,"right");
      else if (strcasecmp(argStr1,"both")==0)
	strcpy(mySide,"both");
      else {
	sprintf(reply,"Unknown side '%s', usage: skyoffset dRA dDec abs|rel [left|right|both]",
		argStr1);
	return CMD_ERR;
      }
    }
    else {
      strcpy(reply,"Insufficient arguments, usage: skyoffset dRA dDec abs|rel [left|right|both]");
      return CMD_ERR;
    }
    if (strcasecmp(offMode,"REL")==0)
      strcpy(offMode,"REL");
    else if (strcasecmp(offMode,"ABS")==0)
      strcpy(offMode,"ABS");
    else {
      sprintf(reply,"Unknown offset mode '%s', usage: skyoffset dRA dDec abs|rel [left|right|both]",
	      offMode);
      return CMD_ERR;
    }
    // convert to radians for the IIF
    dRA = degrad(dRA/3600.0); // defined in skyutils.h
    dDec = degrad(dDec/3600.0);
  }

  // Eventualy validate offset requests other than mode once we know
  // what sensible limits are...

  switch (lbt.opMode) {
  case LIVE:
    if (mods_OffsetPointing(dPA,dRA,dDec,"RADEC",offMode,mySide)!=0) {
      sprintf(reply,"IIF error - %s",mods_error());
      return CMD_ERR;
    }
    sprintf(reply,"%s Sky Offset Completed lbtSide=%s",offMode,mySide);
    break;
    
  case LABSIM:
    sprintf(reply,"%s Sky Offset Completed (simulation mode)",offMode);
    break;
  }
  return CMD_OK;
}

/*!  
  \brief SLITOFFSET command - sends an offset in slit plane coordinates
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  slitoffset dX dY abs|rel [left|right|both]
  slitoffset home [left|right|both]

  Tells the telescope control system to offset the telescope in the
  slit coordinate system (aka DETXY in LBT parlance, but our common
  plane is the slit plane not one of the detectors).  The slit
  coordinate system is fixed relative to the instrument rotator - dY
  is along the slit in arcsec, dX is perpendicular to the slit in
  arcsec.

  See the skyoffset command for a traditional celestial coordinate system
  offset command, rotoffset for rotator offsets

  \sa cmd_update, cmd_skyoffset, cmd_rotoffset, cmd_offset
*/

int
cmd_slitoffset(char *args, MsgType msgtype, char *reply)
{
  char offMode[32];
  char argStr1[32];
  char argStr2[32];
  double dX;
  double dY;
  double dPA = 0.0;
  int numArgs;
  char mySide[16];

  // Check for an LBT TCS link first

  if (!lbt.Link) {
    sprintf(reply,"LBT TCS link not enabled");
    return CMD_ERR;
  }

  // We require arguments

  if (strlen(args)==0) {
    strcpy(reply,"Insufficient arguments, usage: slitoffset dX dY [rel|abs] [side]");
    return CMD_ERR;
  }

  // Look at the first argument, see if it is HOME, which will execute
  // an absolute offset to 0,0 to the pointing reference.

  GetArg(args,1,argStr1);
  if (strcasecmp(argStr1,"HOME")==0) {
    dX = 0.0;
    dY = 0.0;
    dPA = 0.0;
    strcpy(offMode,"ABS");
    GetArg(args,2,argStr2);
    if (strlen(argStr2) > 0) {
      if (strcasecmp(argStr2,"left")==0)
	strcpy(mySide,"left");
      else if (strcasecmp(argStr2,"right")==0)
	strcpy(mySide,"right");
      else if (strcasecmp(argStr2,"both")==0)
	strcpy(mySide,"both");
      else {
	sprintf(reply,"Unknown argument '%s', usage: slitoffset home [left|right|both]",
		argStr2);
	return CMD_ERR;
      }
    }
    else 
      strcpy(mySide,lbt.side);
  }
  else {
    numArgs = sscanf(args,"%lf %lf %s %s",&dX,&dY,offMode,argStr1);
    if (numArgs == 3) {
      strcpy(mySide,lbt.side);
    }
    else if (numArgs == 4) { 
      if (strcasecmp(argStr1,"left")==0)
	strcpy(mySide,"left");
      else if (strcasecmp(argStr1,"right")==0)
	strcpy(mySide,"right");
      else if (strcasecmp(argStr1,"both")==0)
	strcpy(mySide,"both");
      else {
	sprintf(reply,"Unknown side '%s', usage: slitoffset dX dY abs|rel [left|right|both]",
		argStr1);
	return CMD_ERR;
      }
    }
    else {
      strcpy(reply,"Insufficient arguments, usage: slitoffset dX dY abs|rel [left|right|both]");
      return CMD_ERR;
    }
    if (strcasecmp(offMode,"REL")==0)
      strcpy(offMode,"REL");
    else if (strcasecmp(offMode,"ABS")==0)
      strcpy(offMode,"ABS");
    else {
      sprintf(reply,"Unknown slitoffset mode '%s', usage: slitoffset dX dY abs|rel [left|right|both]",
	      offMode);
      return CMD_ERR;
    }
    // convert to radians for the IIF
    dX = degrad(dX/3600.0); // defined in skyutils.h
    dY = degrad(dY/3600.0);
  }

  // Eventualy validate offset requests other than mode once we know
  // what sensible limits are...

  switch (lbt.opMode) {
  case LIVE:
    // Send this as a DETXY coordinate system OffsetPointing() command
    if (mods_OffsetPointing(dPA,dX,dY,"DETXY",offMode,mySide)!=0) {
      sprintf(reply,"IIF error - %s",mods_error());
      return CMD_ERR;
    }
    sprintf(reply,"%s Slit Offset Completed lbtSide=%s",offMode,mySide);
    break;
    
  case LABSIM:
    sprintf(reply,"%s Slit Offset Completed (simulation mode)",offMode);
    break;
  }
  return CMD_OK;
}


/*!  
  \brief ROTOFFSET command - sends a rotator offset
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  rotoffset dPA [left|right|both]

  Tells the telescope control system to offset the instrument rotator
  by dPA degrees relative to the current position.  + is clockwise,
  - is counter clockwise.
  
  Because the LBT can be extremely confusing, we only implement
  relative offsets and then "absorb" them once the offset
  is sent.
  
  \sa cmd_update, cmd_skyoffset, cmd_slitoffset
*/

int
cmd_rotoffset(char *args, MsgType msgtype, char *reply)
{
  char offMode[32];
  char argStr1[32];
  char argStr2[32];
  double dX = 0.0;
  double dY = 0.0;
  double dPA;
  int numArgs;
  char mySide[16];

  // Check for an LBT TCS link first

  if (!lbt.Link) {
    sprintf(reply,"LBT TCS link not enabled");
    return CMD_ERR;
  }

  // We require arguments

  if (strlen(args)==0) {
    strcpy(reply,"Insufficient arguments, usage: rotoffset dPA [side]");
    return CMD_ERR;
  }

  numArgs = sscanf(args,"%lf %s",&dPA,argStr1);
  if (numArgs == 1) {
    strcpy(mySide,lbt.side);
  }
  else if (numArgs == 2) { 
    if (strcasecmp(argStr1,"left")==0)
      strcpy(mySide,"left");
    else if (strcasecmp(argStr1,"right")==0)
      strcpy(mySide,"right");
    else if (strcasecmp(argStr1,"both")==0)
      strcpy(mySide,"both");
    else {
      sprintf(reply,"Unknown side '%s', usage: rotoffset dPA [left|right|both]",
	      argStr1);
      return CMD_ERR;
    }
  }
  else {
    strcpy(reply,"Insufficient arguments, usage: rotoffset dPA [left|right|both]");
    return CMD_ERR;
  }

  // Convert to radians for the IIF

  dPA = degrad(dPA); // defined in skyutils.h

  // Send the rotator offset 

  switch (lbt.opMode) {
  case LIVE:
    // Send this as a DETXY coordinate system OffsetPointing() command
    if (mods_OffsetPointing(dPA,0.0,0.0,"DETXY","REL",mySide)!=0) {
      sprintf(reply,"IIF error - %s",mods_error());
      return CMD_ERR;
    }
    sprintf(reply,"Rotator Offset Completed lbtSide=%s",mySide);
    break;
    
  case LABSIM:
    sprintf(reply,"Rotator Offset Completed (simulation mode)");
    break;
  }
  return CMD_OK;
}


/*!  
  \brief UPDATE command - instruct the IIF to update the pointing reference
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  updatepointing [radec|detxy] [left|right|both]

  Tells the telescope IIF to make the current position the pointing
  reference.  This establishes, among other things, the "home"
  position for absolute offsets after a pointing offset.

  If no arguments are given, assume detxy on the default side. This is
  the most common use of this command.

  If no side is indicated, use the default side.

  \sa cmd_offset
*/

int
cmd_update(char *args, MsgType msgtype, char *reply)
{
  char mySide[16];
  char coordSys[10];
  char argStr1[32];
  char argStr2[32];
  int numArgs;

  // Nobody to talk to...

  if (!lbt.Link) {
    sprintf(reply,"LBT TCS link not enabled");
    return CMD_ERR;
  }

  // Optional side argument, otherwise use default side

  if (strlen(args)>0) {
    numArgs = sscanf(args,"%s %s",argStr1,argStr2);
    if (numArgs < 1 || numArgs > 2) {
      sprintf(reply,"Syntax error, UpdatePointing [radec|detxy] [left|right|both]",args);
      return CMD_ERR;
    }
    if (strcasecmp(argStr1,"radec")==0)
      strcpy(coordSys,"RADEC");
    else if (strcasecmp(argStr1,"detxy")==0)
      strcpy(coordSys,"DETXY");
    else {
      sprintf(reply,"Unrecognized option '%s', usage: UpdatePointing [radec|detxy] [left|right|both]",args);
      return CMD_ERR;
    }
    if (numArgs == 2) {
      if (strcasecmp(argStr2,"left")==0)
	strcpy(mySide,"left");
      else if (strcasecmp(argStr2,"right")==0)
	strcpy(mySide,"right");
      else if (strcasecmp(argStr2,"both")==0)
	strcpy(mySide,"both");
      else {
	sprintf(reply,"Unrecognized option '%s', usage: UpdatePointing [radec|detxy] [left|right|both]",args);
	return CMD_ERR;
      }
    }
    else {
      strcpy(mySide,lbt.side);
    }
  }
  else {
    strcpy(mySide,lbt.side);
    strcpy(coordSys,"DETXY");
  }

  switch (lbt.opMode) {
  case LIVE:
    if (mods_UpdatePointingReference(coordSys,mySide) != 0) {
      sprintf(reply,"IIF error - %s",mods_error());
      return CMD_ERR;
    }
    break;
    
  case LABSIM: 
    break;
  }
  sprintf(reply,"Telescope %s pointing reference updated lbtSide=%s",
	  coordSys,mySide);
  return CMD_OK;
}

/*!  
  \brief CLEARSTARS command - clears the target and guide object data
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  clearstars

  Tells the telescope IIF to clear the target and guide object coordinates.

*/

int
cmd_clear(char *args, MsgType msgtype, char *reply)
{
  if (!lbt.Link) {
    sprintf(reply,"LBT TCS link not enabled");
    return CMD_ERR;
  }

  switch (lbt.opMode) {
  case LIVE:
    if (mods_ClearStars()!=0) {
      sprintf(reply,"IIF error - %s",mods_error());
      return CMD_ERR;
    }
    break;
    
  case LABSIM: 
    break;
  }

  // Now reset all of the coordinate data so we don't have stale info

  strcpy(lbt.objName,"none");
  strcpy(lbt.objRA,"none");
  strcpy(lbt.objDec,"none"); 
  lbt.objRARad = 0.0;
  lbt.objDecRad = 0.0;
  lbt.objPMRA = 0.0;
  lbt.objPMDec = 0.0;
  lbt.objEpoch = 2000.0;
  strcpy(lbt.guiName,"none");
  strcpy(lbt.guiRA,"none");
  strcpy(lbt.guiDec,"none"); 
  lbt.guiRARad = 0.0;
  lbt.guiDecRad = 0.0;
  lbt.guiPMRA = 0.0;
  lbt.guiPMDec = 0.0;
  lbt.guiEpoch = 2000.0;
  lbt.guiMag = 0.0;

  strcpy(reply,"Target and Guide Star coordinate data cleared...");
  return CMD_OK;
}

/*!  
  \brief remove command - clears the target and guide object data
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  remove

  Tells the telescope IIF to clear all target, offset, and hotspot
  objects.  ClearStars is more useful than this, included for
  completeness.

*/

int
cmd_remove(char *args, MsgType msgtype, char *reply)
{
  if (!lbt.Link) {
    sprintf(reply,"LBT TCS link not enabled");
    return CMD_ERR;
  }

  switch (lbt.opMode) {
  case LIVE:
    if (mods_Remove()!=0) {
      sprintf(reply,"IIF error - %s",mods_error());
      return CMD_ERR;
    }
    break;
    
  case LABSIM: 
    break;
  }

  // Now reset all of the coordinate data so we don't have stale info

  strcpy(lbt.objName,"none");
  strcpy(lbt.objRA,"none");
  strcpy(lbt.objDec,"none"); 
  lbt.objRARad = 0.0;
  lbt.objDecRad = 0.0;
  lbt.objPMRA = 0.0;
  lbt.objPMDec = 0.0;
  lbt.objEpoch = 2000.0;
  strcpy(lbt.guiName,"none");
  strcpy(lbt.guiRA,"none");
  strcpy(lbt.guiDec,"none"); 
  lbt.guiRARad = 0.0;
  lbt.guiDecRad = 0.0;
  lbt.guiPMRA = 0.0;
  lbt.guiPMDec = 0.0;
  lbt.guiEpoch = 2000.0;
  lbt.guiMag = 0.0;

  strcpy(reply,"Target, Hotspot, and Offset objects removed");
  return CMD_OK;
}

/*!  
  \brief AUTHORIZED command - query the authorization status 
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  authorized

  Checks the authorization status of the current IIF instance.
  returns TELAUTH=T or TELAUTH=F
*/

int
cmd_authorize(char *args, MsgType msgtype, char *reply)
{
  char leftInst[64];  // left side authorized instrument
  char leftFoc[64];   // left side authorized focal station
  char rightInst[64]; // right side authorized instrument
  char rightFoc[64];  // right side authorized focal station

  // Must have someone to talk to first...

  if (!lbt.Link) {
    sprintf(reply,"LBT TCS link not enabled, TELAUTH=F");  // by definition
    return CMD_ERR;
  }

  switch (lbt.opMode) {
  case LIVE:
    lbt.authorized = mods_Authorize();
#ifdef __UseOldDD
    mods_GetParameter("iif.side[0].authorizedInstrument",leftInst);
    mods_GetParameter("iif.side[0].authorizedFocalStation",leftFoc);
    mods_GetParameter("iif.side[1].authorizedInstrument",rightInst);
    mods_GetParameter("iif.side[1].authorizedFocalStation",rightFoc);
#else
    mods_GetParameter("L_Instrument",leftInst);
    mods_GetParameter("L_FocalStation",leftFoc);
    mods_GetParameter("R_Instrument",rightInst);
    mods_GetParameter("R_FocalStation",rightFoc);
#endif
  case LABSIM: 
    break;
  }
  sprintf(reply,"TELAUTH=%s LeftInst=%s LeftFoc=%s RightInst=%s RightFoc=%s",
	  ((lbt.authorized) ? "T" : "F"),leftInst,leftFoc,rightInst,rightFoc);
  return CMD_OK;
}

/*!  
  \brief BC Command - send the TCS a Binocular Control command
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  bc [clearpreset|syncpreset|syncoffset|adjustbalance|
      cancelsyncpreset|cancelsyncoffset|releasesyncpreset|
      releasesyncoffset|leftmonocular|rightmonocular]

  Sends a Binocular Control (BC) command to the TCS to instruct the TCS
  to use special binocular handling for subsequent PresetTelescope
  or OffsetPointing commands.  See section 6.6 and 8.3 of the
  LBTIIF ICD (481s013s) for details on the binocular control
  command options.

  Requires a command word.  Returns on error if no command is given
  or command is not recognized.

  \sa cmd_preset, cmd_offset
*/

int
cmd_bc(char *args, MsgType msgtype, char *reply)
{
  char bcFlag[32];
  char argStr1[32];
  int numArgs;

  // Nobody to talk to...

  if (!lbt.Link) {
    sprintf(reply,"LBT TCS link not enabled");
    return CMD_ERR;
  }

  // One argument is required, fail otherwise

  if (strlen(args) == 0) {
    sprintf(reply,"Insufficient arguments - BC requires one argument",args);
    return CMD_ERR;
  }

  numArgs = sscanf(args,"%s",argStr1);

  // Validate the BinocularControl() command "flag".  Allow limited
  // convenience aliases

  if (strcasecmp(argStr1,"clearpreset")==0 || strcasecmp(argStr1,"clear")==0)
    strcpy(bcFlag,"CLEARPRESET");

  else if (strcasecmp(argStr1,"syncpreset")==0)
    strcpy(bcFlag,"SYNCPRESET");

  else if (strcasecmp(argStr1,"syncoffset")==0)
    strcpy(bcFlag,"SYNCOFFSET");

  else if (strcasecmp(argStr1,"adjustbalance")==0 || strcasecmp(argStr1,"adjust")==0)
    strcpy(bcFlag,"ADJUSTBALANCE");

  else if (strcasecmp(argStr1,"cancelsyncpreset")==0)
    strcpy(bcFlag,"CANCELSYNCPRESET");

  else if (strcasecmp(argStr1,"cancelsyncoffset")==0)
    strcpy(bcFlag,"CANCELSYNCOFFSET");

  else if (strcasecmp(argStr1,"releasesyncpreset")==0)
    strcpy(bcFlag,"RELEASESYNCPRESET");

  else if (strcasecmp(argStr1,"releasesyncoffset")==0)
    strcpy(bcFlag,"RELEASESYNCOFFSET");

  else if (strcasecmp(argStr1,"leftmonocular")==0 || strcasecmp(argStr1,"left")==0)
    strcpy(bcFlag,"LEFTMONOCULAR");

  else if (strcasecmp(argStr1,"rightmonocular")==0 || strcasecmp(argStr1,"right")==0)
    strcpy(bcFlag,"RIGHTMONOCULAR");

  else {
    sprintf(reply,"Unrecognized option '%s', usage: BC flag",args);
    return CMD_ERR;
  }

  // We got something valid, send it...

  switch (lbt.opMode) {
  case LIVE:
    if (mods_BinocularControl(bcFlag) != 0) {
      sprintf(reply,"IIF error - %s",mods_error());
      return CMD_ERR;
    }
    break;
    
  case LABSIM: 
    break;
  }
  sprintf(reply,"Sent BinocularControl command %s",bcFlag);
  return CMD_OK;
}


/*!  
  \brief LOGEVENT command - log an instrument "event"
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  logevent eventID description of the event...

  Logs an instrument event in the IIF logger.  Events are given a
  single-word eventID, followed by a brief text description of the
  event.

  This appears to be a future expansion option for the IIF/TCS system,
  as we were given no examples beyond the trivial nor guidance as to
  where events are logged, accessed or used, etc.

*/

int
cmd_logevent(char *args, MsgType msgtype, char *reply)
{
  char eventID[32];
  char eventDescr[256];

  if (strlen(args)==0) {
    strcpy(reply,"Insufficent arguments, usage: logevent eventID description of event...");
    return CMD_OK;
  }

  sscanf(args,"%s %[^\n]",eventID,eventDescr);

  if (strlen(eventDescr)==0) {
    sprintf(reply,"%s event description is blank, usage: logevent eventID description text",
	    eventID);
    return CMD_OK;
  }

  switch (lbt.opMode) {
  case LIVE:
    if (mods_LogEvent(eventID,eventDescr)!=0) {
      sprintf(reply,"IIF error - %s",mods_error());
      return CMD_ERR;
    }
    break;
    
  case LABSIM:
    break;
  }
  sprintf(reply,"logged event %s = '%s'",eventID,eventDescr);
  return CMD_OK;
}

/*!  
  \brief SETPARAMETER command - set a parameter in the data dictionary
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  setparameter DDkeyword value

  Sets Data Dictionary entry DDkeyword to value.

  \sa cmd_getparam
*/

int
cmd_setparam(char *args, MsgType msgtype, char *reply)
{
  char ddKey[256];
  char ddValue[256];
  int numArgs;

  if (strlen(args)==0) {
    strcpy(reply,"Insufficent arguments, usage: setparameter DDkey DDvalue");
    return CMD_OK;
  }

  numArgs = sscanf(args,"%s %s",ddKey,ddValue);
  if (numArgs != 2) {
    strcpy(reply,"Insufficent arguments, usage: setparameter DDkey DDvalue");
    return CMD_OK;
  }

  switch (lbt.opMode) {
  case LIVE:
    if (mods_SetParameter(ddKey,ddValue)!=0) {
      sprintf(reply,"IIF error - %s",mods_error());
      return CMD_ERR;
    }
    break;
    
  case LABSIM:
    break;
  }
  sprintf(reply,"Set DD entry %s=%s",ddKey,ddValue);
  return CMD_OK;
}

/*!  
  \brief GETPARAMETER command - get a parameter from the data dictionary
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage: getparameter DDkeyword

  Gets the current value of DDkeyword from the Data Dictionary.

  \sa cmd_setparam
*/

int
cmd_getparam(char *args, MsgType msgtype, char *reply)
{
  char ddName[256];
  char ddKey[256];

  if (strlen(args)==0) {
    strcpy(reply,"Insufficent arguments, usage: getparameter DDkey");
    return CMD_OK;
  }
  strcpy(ddName,args);
  switch (lbt.opMode) {
  case LIVE:
    if (mods_GetParameter(ddName,ddKey)!=0) {
      sprintf(reply,"IIF error - %s",mods_error());
      return CMD_ERR;
    }
    sprintf(reply,"%s=%s",ddName,ddKey);
    break;
    
  case LABSIM:
    sprintf(reply,"Got DD entry %s (simulation mode)",args);
    break;
  }
  return CMD_OK;
}

/*!  
  \brief PauseGuiding command - temporarily pause guiding
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage: PauseGuiding [left|right|both]

  Instructs the telescope to temporarily pause guiding and WFS active 
  collimation correction (the latter only if an ACTIVE preset).  gpause
  is a recognized alias.

  Resume guiding with ResumeGuiding (alias gresume)

  \sa cmd_gresume
*/

int
cmd_gpause(char *args, MsgType msgtype, char *reply)
{
  char mySide[16];

  // Must have someone to talk to...

  if (!lbt.Link) {
    sprintf(reply,"LBT TCS link not enabled");
    return CMD_ERR;
  }

  // Optional side argument, otherwise use default side

  if (strlen(args)>0) {
    if (strcasecmp(args,"left")==0)
      strcpy(mySide,"left");
    else if (strcasecmp(args,"right")==0)
      strcpy(mySide,"right");
    else if (strcasecmp(args,"both")==0)
      strcpy(mySide,"both");
    else {
      sprintf(reply,"Unrecognized option '%s', usage: PauseGuiding [left|right|both]",args);
      return CMD_ERR;
    }
  }
  else 
    strcpy(mySide,lbt.side);

  switch (lbt.opMode) {
  case LIVE:
    if (mods_PauseGuiding(mySide)!=0) {
      sprintf(reply,"IIF error - %s",mods_error());
      return CMD_ERR;
    }
    break;
    
  case LABSIM:
    break;
  }
  sprintf(reply,"Guiding Paused lbtSide=%s",mySide);
  return CMD_OK;
}

/*!  
  \brief ResumeGuiding command - resume guiding after a pause
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage: ResumeGuiding [left|right|both]

  Instructs the telescope to resume guiding and WFS active collimation
  correction (the latter only if an ACTIVE preset) after it was paused.

  Must follow a PauseGuiding (alias gpause) command.  If guiding is not
  in progress, it will return an error.

  \sa cmd_gpause
*/

int
cmd_gresume(char *args, MsgType msgtype, char *reply)
{
  char mySide[16];

  // Must have someone to talk to...

  if (!lbt.Link) {
    sprintf(reply,"LBT TCS link not enabled");
    return CMD_ERR;
  }

  // Optional side argument, otherwise use default side

  if (strlen(args)>0) {
    if (strcasecmp(args,"left")==0)
      strcpy(mySide,"left");
    else if (strcasecmp(args,"right")==0)
      strcpy(mySide,"right");
    else if (strcasecmp(args,"both")==0)
      strcpy(mySide,"both");
    else {
      sprintf(reply,"Unrecognized option '%s', usage: ResumeGuiding [left|right|both]",args);
      return CMD_ERR;
    }
  }
  else 
    strcpy(mySide,lbt.side);

  switch (lbt.opMode) {
  case LIVE:
    if (mods_ResumeGuiding(mySide)!=0) {
      sprintf(reply,"IIF error - %s",mods_error());
      return CMD_ERR;
    }
    break;
    
  case LABSIM:
    break;
  }
  sprintf(reply,"Guiding Resumed lbtSide=%s",mySide);
  return CMD_OK;
}

/*!  
  \brief ROTREADY command - enable/disable the ISS rotator
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  rotready enable|disable left|right

  This engineering command is used to enable or disable the ISS rotator 
  on the named side.  It should only be used for engineering functions,
  like rotating the instrument to a specific service position.  Otherwise
  the rotator should only be set using the PresetTelescope() and OffsetPointing()
  functions as these work in useful rotator coordinates (e.g., celestial 
  position angle or parallactic angle), not native (raw) device angle.

  Subsequent rotator positions for an enabled rotator can be set with
  the rotpos command.

  \sa cmd_rotsvcpos
*/

int
cmd_rotready(char *args, MsgType msgtype, char *reply)
{
  char argStr[32];
  char mySide[16];
  int rotEnable;
  int numArgs;

  // Must have someone to talk to...

  if (!lbt.Link) {
    sprintf(reply,"LBT TCS link not enabled");
    return CMD_ERR;
  }

  // Two arguments are required for this command.  As an engineering
  // command implicit arguments would be unsafe.

  if (strlen(args)>0) {
    numArgs = sscanf(args,"%s %s",argStr,mySide);
    if (numArgs == 2) {
      if (strcasecmp(mySide,"left")==0)
	strcpy(mySide,"left");
      else if (strcasecmp(mySide,"right")==0)
	strcpy(mySide,"right");
      else {
	sprintf(reply,"Unknown side '%s', usage: rotready enable|disable side",
		mySide);
	return CMD_ERR;
      }
    }
    else {
      sprintf(reply,"Invalid arguments '%s', 2 required, usage: rotready enable|disable side",
	      args);
      return CMD_ERR;
    }
    if (strcasecmp(argStr,"ENABLE")==0)
      rotEnable = 1;
    else if (strcasecmp(argStr,"DISABLE")==0)
      rotEnable = 0;
    else {
      sprintf(reply,"Unknown argument '%s', usage: rotready enable|disable side",
	      argStr);
      return CMD_ERR;
    }
  }
  else {
    strcpy(reply,"Usage: rotready enable|disable [left|right]");
    return CMD_ERR;
  }

  switch (lbt.opMode) {
  case LIVE:
    if (mods_RotReady(rotEnable,mySide)!=0) {
      sprintf(reply,"IIF error - %s",mods_error());
      return CMD_ERR;
    }
    else
      sprintf(reply,"%s direct gregorian rotator %s",
	      mySide,((rotEnable) ? "Enabled":"Disabled"));
    break;
    
  case LABSIM:
    sprintf(reply,"%s direct gregorian rotator %s",
	    mySide,((rotEnable) ? "Enabled":"Disabled"));
    break;
  }
  return CMD_OK;
}

/*!  
  \brief ROTPOS command - slew the ISS rotator to a given position [Engineering]
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  rotpos angle [left|right]

  Slew the ISS rotator to a given raw (native units) position angle in
  decimal degrees.  This is an engineering level command used to
  implement the RotServiceMode function of the IIF.  Note that moving
  the ISS rotator in this way is equivalent to operating the rotator
  with the relevant MCS control panel.  All proper safety precautions
  must be observed.  As such, this command is reserved purely for 
  engineering purposes.

  \sa cmd_rotready
*/

int
cmd_rotsvcpos(char *args, MsgType msgtype, char *reply)
{
  double rotAngle;
  char mySide[16];
  int  numArgs;

  // Must have someone to talk to...

  if (!lbt.Link) {
    sprintf(reply,"LBT TCS link not enabled");
    return CMD_ERR;
  }

  if (strlen(args)>0) {
    numArgs = sscanf(args,"%lf %s",&rotAngle,mySide);
    if (numArgs != 2) {
      strcpy(reply,"Insufficient arguments, 2 required, usage: rotpos angle side");
      return CMD_ERR;
    }
    if (strcasecmp(mySide,"left")==0)
      strcpy(mySide,"left");
    else if (strcasecmp(mySide,"right")==0)
      strcpy(mySide,"right");
    else {
      sprintf(reply,"Unknown side '%s', usage: rotpos angle side",mySide);
      return CMD_ERR;
    }
    if (rotAngle < -90.0 || rotAngle > 460.0) {
      sprintf(reply,"Unvalid angle '%f' must be -90..460 degrees",rotAngle);
      return CMD_ERR;
    }
  }
  else {
    strcpy(reply,"Usage: rotpos angle side");
    return CMD_ERR;
  }

  switch (lbt.opMode) {
  case LIVE:
    if (mods_RotServicePosition(rotAngle,mySide)!=0) {
      sprintf(reply,"IIF error - %s",mods_error());
      return CMD_ERR;
    }
    else
      sprintf(reply,"ROTANGLE=%.5f",rotAngle);
    break;
    
  case LABSIM:
    sprintf(reply,"ROTANGLE=%.5f",rotAngle);
    break;
  }
  return CMD_OK;
  
}


//***************************************************************************
//
// Command Interpreter I/O Handlers
//

//---------------------------------------------------------------------------
//
// KeyboardCommand() - process a command from the keyboard
//

#include <readline/readline.h>  // Gnu readline utility
#include <readline/history.h>   // Gnu history utility

/*!  
  \brief Process a command from the client's console keyboard
  \param line string with the keyboard command

  This function is setup as a callback for readline(), the GNU
  command-line library that provides Emacs-like key bindings for
  command-line editing, rapid "arrow keys" command history browsing, and
  convenient command history commands (!, !!, etc.).  This gives the
  client application's command-line interface a look-and-feel familiar
  to most users of Unix system command shells (e.g., the tcsh shell).

  This function parses the interactive command line and calls the
  appropriate low-level cmd_xxx() command action functions for excuting
  most commands, as well as servicing ">XX msgtype: command" format raw
  IMPv2 message sending requests.

  Once the parser gets past the main command tree, instead of griping
  about an unknown command, it assumes that it might be a raw host
  command and ships it to the port as-is.  Syntax errors or unknown
  commands will result in gripes from the controller, which is the
  idea at this point.

  All keyboard commands are treated as EXEC: type IMPv2 messages.  This
  makes the downstream cmd_xxx() action functions insensitive to whether
  or not the command came from the keyboard or from a remote ISIS server
  or client application.

  \sa SocketCommand()
*/

void
KeyboardCommand(char *line)
{
  char cmd[BIG_STR_SIZE];       // command string (oversized)
  char args[BIG_STR_SIZE];      // command-line argument buffer (oversized)
  char reply[ISIS_MSGSIZE];     // command reply buffer

  // ISIS message handling stuff

  char msg[ISIS_MSGSIZE];       // ISIS message buffer
  char srcID[ISIS_NODESIZE];    // ISIS message sending node ID
  char destID[ISIS_NODESIZE];   // ISIS message destination node ID
  char msgbody[ISIS_MSGSIZE];   // ISIS message body

  // Variables used to traverse the command tree

  int i;
  int nfound=0;
  int icmd=-1;

  // Pointer for the keyboard message buffer

  char *message;

  // Stuff for the history facility

  char *expansion;
  int result;

  // Debugging and diagnosis

  double t0, dt;

  // If line is NULL, we have nothing to do, return

  if (line==NULL) return;

  // Similarly, if line is blank, return

  if (strlen(line)==0) {
    free(line);
    return;
  }

  // Allocate memory for the message buffer and clear it

  message = (char *)malloc((ISIS_MSGSIZE)*sizeof(char));
  memset(message,0,ISIS_MSGSIZE);

  // Copy the keyboard input line into the message buffer 

  strcpy(message,line);

  // do any history expansion (!, !!, etc.) if required

  if (line[0]) {
    result = history_expand(line,&expansion);
    if (result)
      printf("%s\n",expansion);
    
    if (result < 0 || result==2) {
      free(expansion);
      return;
    }

    add_history(expansion);
    memset(message,0,ISIS_MSGSIZE);
    sprintf(message,"%s",expansion);
    free(expansion);
  }

  // We're all done with the original string from readline(), free it

  free(line);

  // Make sure Ctrl+C is set for motion aborts

  signal(SIGINT,HandleInt);  // reset the SIGINT handler

  // Remove any \n terminator on the message string

  if (message[strlen(message)-1]=='\n') message[strlen(message)-1]='\0';

  // Clear the command handling strings

  memset(reply,0,sizeof(reply));
  memset(args,0,sizeof(args));
  memset(cmd,0,sizeof(cmd));

  // Split message into command and argument strings

  sscanf(message,"%s %[^\n]",cmd,args);

  // We're all done with the message string, free its memory

  free(message);

  // Message Handling:

  // >XX commands
  //
  // Look for > in cmd, this means a redirect to another ISIS node.
  // This is handled outside the usual command tree, for the obvious
  // reason that the syntax is unique to this operation.

  if (strncasecmp(cmd,">",1)==0) {
    if (client.useISIS) {
      memset(msg,0,sizeof(msg));
      memset(destID,0,sizeof(destID));
      memset(msgbody,0,sizeof(msgbody));

      sscanf(cmd,">%s",destID); // extract the destination node ID
      strcpy(msgbody,args);     // and the message body

      // The trick here is that REQ doesn't put anything in the
      // msgtype field, so that whatever msgtype designator is
      // in the message string gets retained.

      strcpy(msg,ISISMessage(client.ID,destID,REQ,msgbody));

      // and send it off

      SendToISISServer(&client,msg);
      if (client.isVerbose && useCLI) {
	msg[strlen(msg)-1]='\0';
	printf("OUT: %s\n",msg);
      }
    }
    else {
      printf("No ISIS server active, > command unavailable\n");
    }
  }
  
  // All other commands use the cmd_xxx() action calls

  else { 

    // Traverse the command table, matches are case-insensitive, but
    // must be exact word matches (no abbreviations or aliases)
    
    nfound = 0;
    for (i=0; i<NumCommands; i++) {
      if (strcasecmp(cmdtab[i].cmd,cmd)==0) { 
	nfound++;
	icmd=i;
	break;
      }
    }

    // If unknown command, gripe, otherwise do it

    if (nfound == 0) {
      if (strlen(cmd)>0) {
	printf("ERROR: Unknown Command '%s' (type 'help' to list all commands)\n",cmd);
      }
    }
    else {
	
      // All console keyboard are treated as EXEC: type messages
	
      t0 = SysTimestamp();
      switch (cmdtab[icmd].action(args,EXEC,reply)) {
	
      case CMD_ERR:
	printf("ERROR: %s %s\n",cmd,reply);
	break;
	
      case CMD_OK:
	printf("DONE: %s %s\n",cmd,reply);
	break;
	
      case CMD_NOOP:
      default:
	break;
	
      } // end of switch()
      dt = SysTimestamp() - t0;
      if (client.Debug && useCLI)
	printf("   %s command execution time %.3f seconds\n",cmd,dt);
    }
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

  // ISIS message components 

  char msg[ISIS_MSGSIZE];       // Full ISIS message buffer
  char srcID[ISIS_NODESIZE];    // ISIS message sending node ID
  char destID[ISIS_NODESIZE];   // ISIS message destination node ID
  MsgType msgtype = REQ;        // ISIS message type, defined in isisclient.h
  char msgbody[ISIS_MSGSIZE];   // ISIS message/command body

  // Command components (command args)

  char cmd[BIG_STR_SIZE];       // command string (oversized)
  char args[BIG_STR_SIZE];      // command-line argument buffer (oversized)
  char reply[ISIS_MSGSIZE];     // command reply string

  // Other working variables

  int i;
  int nfound=0;
  int icmd=-1;
  double t0, dt;

  // Some simple initializations

  memset(reply,0,sizeof(reply));
  memset(args,0,sizeof(args));
  memset(cmd,0,sizeof(cmd));
  memset(msg,0,ISIS_MSGSIZE);

  // Split the ISIS format message into components

  if (SplitMessage(buf,srcID,destID,&msgtype,msgbody)<0) {
    if (client.isVerbose && useCLI) printf("\nISIS IN: %s\n",buf);
    return;
  }
        
  // Immediate action depends on the type of message received as
  // recorded by the msgtype code.

  switch(msgtype) {

  case STATUS:  // we've been sent a status message, echo to console
    if (useCLI) printf("%s\n",buf);
    break;
	  
  case DONE:    // command completion message (?), echo to console.
    if (useCLI) printf("%s\n",buf);
    break;
	  
  case ERROR:   // error messages, echo to console, get fancy later
    if (useCLI) printf("%s\n",buf);
    break;

  case WARNING:
    if (useCLI) printf("%s\n",buf);
    break;

  case FATAL:
    if (useCLI) printf("%s\n",buf);
    break;
    
  case REQ:    // implicit command requests
  case EXEC:   // and executive override commands

    memset(msg,0,ISIS_MSGSIZE);
    sscanf(msgbody,"%s %[^\n]",cmd,args);  // split into command + args

    // Traverse the command table, exact case-insensitive match required

    nfound = 0;
    for (i=0; i<NumCommands; i++) {
      if (strcasecmp(cmdtab[i].cmd,cmd)==0) { 
	nfound++;
	icmd=i;
	break;
      }
    }

    // Unknown command, gripe back to the sender, otherwise try to do it

    if (nfound == 0) {
      sprintf(msg,"%s>%s ERROR: Unknown command - %s\n",
              client.ID,srcID,msgbody);
    }
    else {
      t0 = SysTimestamp();

      switch(cmdtab[icmd].action(args,msgtype,reply)) {

      case CMD_ERR: // command generated an error
	sprintf(msg,"%s>%s ERROR: %s %s\n",client.ID,srcID,cmd,reply);
	break;

      case CMD_NOOP: // command is a no-op, debug/verbose output only
	if (client.isVerbose && useCLI) printf("IN: %s from ISIS node %s\n",msgbody,srcID);
	break;

      case CMD_OK:  // command executed OK, return reply
      default:
	sprintf(msg,"%s>%s DONE: %s %s\n",client.ID,srcID,cmd,reply);
	break;
	
      } // end of switch on cmdtab.action()

      dt = SysTimestamp() - t0;
      if (client.Debug && useCLI)
	printf("  %s command execution time %.3f seconds\n",cmd,dt);
    }

    // An incoming PING requires special handling - it is an exception
    // to the usual messaging syntax since PONG is sent in reply 

    if (strcasecmp(cmd,"PING") == 0)
      sprintf(msg,"%s>%s %s\n",client.ID,srcID,reply);
      
    break;

  default:  // we don't know what we got, print for debugging purposes
    if (useCLI) printf(" [Malformed message received on client port: '%s']\n",buf);
    break;

  } // end of switch(msgtype) -- default falls through with no-op

  // Do we have something to send back? 
  //
  // If we are configured as an ISIS client (client.useISIS=true), send
  // the reply back to the ISIS server for handling with
  // SendToISISServer().
  //
  // If we are configured as standalone (client.useISIS=false), send the
  // reply back to the remote host with SendToHost().

  if (strlen(msg)>0) { // we have something to send
    if (client.useISIS)
      SendToISISServer(&client,msg);
    else 
      ReplyToRemHost(&client,msg);

    if (client.isVerbose && useCLI) {
      msg[strlen(msg)-1]='\0';
      printf("OUT: %s\n",msg);
    }
  } // end of reply handling

}

