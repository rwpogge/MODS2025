//
// commands.c - client command interpreter
//

/*!
  \file commands.c 
  \brief azcam client command interpreter functions.

  This module contains the command "action" functions called to service
  azcam client commands.  These consist of a suite of \arg "common"
  client action functions common to most ISIS clients \arg
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

  \author R. Pogge, OSU Astronomy Dept. (pogge.1@osu.edu)
  \original 2003 October 13
  \date 2025 July 24 [rwp/osu] -
  
*/

#include "isisclient.h" // ISIS common client library header
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

  Also saves the runtime configuration file for future restarts.

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

  // Before quitting, save our runtime config for restart

  if (SaveConfig(client.rcFile)<0)
    sprintf(reply,"%s - Warning: config file %s not saved",
	    reply,client.rcFile);
  else
    sprintf(reply,"%s - Saved runtime config in %s",
	    reply,client.rcFile);

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
  if (client.isVerbose)
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
  
  sprintf(reply,"azcam Version=%s CompileDate=%s CompileTime=%s",
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
  if (client.isVerbose) {
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

  Example Output:
  <pre>
ID=AC Host=darkstar:10702 Mode=STANDALONE azcam=172.16.1.56:8002 
Verbose -DEBUG rcfile=port2.ini
  </pre>
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

  // List azcam CCD camera host information

  sprintf(reply,"%s CCDHost=%s:%d Config=%s CCDConf=%s", reply, 
	  ccd.Host,ccd.Port,ccd.cfgFile,ccd.iniFile);

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
  \brief RESET command - reset the session parameters

  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  reset
  
  Resets the azcam server by instructing it to re-execute its
  runtime configuration script.

  !*** NOT SURE HOW THIS MAPS TO ARCHON YET ***!
  
*/

int
cmd_reset(char *args, MsgType msgtype, char *reply)
{
  int i;

  // Re-initialize the AzCamServer parameters

  if (initCCDConfig(&ccd,reply)<0) {
    strcat(reply," - reset failed");
    return CMD_ERR;
  }

  // And rebuild the FITS header database

  uploadFITS(&ccd,&obs,reply);

  // that should be it..

  strcpy(reply,"Runtime parameters reset");
  return CMD_OK;

}

/*!  
  \brief EXPTIME command - Set/Query the Exposure Time in Seconds
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  exptime [sec]

  Set/query the exposure time in decimal seconds.  If given with no arguments, it
  returns the current exposure time.

  Exposure times will be converted to milliseconds when passed to the AzCam
  server, so any time passed to this routine will effectively be rounded
  to the nearest 0.001 seconds.  There is no maximum exposure time, though
  it must be positive.

*/

int
cmd_exptime(char *args, MsgType msgtype, char *reply)
{
  char argbuf[32];
  float expt;

  // check the file descriptor and make sure we have an active connection

  if (ccd.FD<0) {
    strcpy(reply,"No azcam server connection active");
    return CMD_ERR;
  }

  // If we have arguments, this is a set.  No arguments is a get.

  if (strlen(args)>0) {
    GetArg(args,1,argbuf); 
    expt = atof(argbuf);
    if (expt < 0.0) {
      sprintf(reply,"Invalid exposure time %s, must be >0.0 sec",argbuf);
      return CMD_ERR;
    }
    if (setExposure(&ccd,expt,reply)<0)
      return CMD_ERR;
  }
  else {
    if (setExposure(&ccd,-1.0,reply)<0)
      return CMD_ERR;
  }

  // copy into the obs struct
  
  obs.expTime = ccd.expTime;
  
  // Report the exposure time

  sprintf(reply,"EXPTIME=%.3f seconds",obs.ExpTime);
  return CMD_OK;

}

/*!  
  \brief OBSERVER command - Set/Query the Observer name(s)
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  observer [names]

  Sets the observer names to appear in the image FITS headers.  If
  given with no arguments, it reports the current names.

  Sets the #obspars::Observer data member and the OBSERVER card
  in the azcam server's header database.

*/

int
cmd_observer(char *args, MsgType msgtype, char *reply)
{
  char argbuf[32];

  // check the file descriptor and make sure we have an active connection

  if (ccd.FD<0) {
    strcpy(reply,"No azcam server connection active");
    return CMD_ERR;
  }

  // If we have arguments, get the observer names and set them

  if (strlen(args)>0) {
    strcpy(obs.Observer,args);
    if (setKeyword(&ccd,"OBSERVER",obs.Observer,"Observer Name(s)",reply)<0)
      return CMD_ERR;
  }
  
  sprintf(reply,"OBSERVER=(%s)",obs.Observer);
  return CMD_OK;

}

/*!  
  \brief PI_NAME command - Set/Query the data PI_Name
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  pi_name [project PI name(s)]

  Sets the PI_NAME keyword for the FITS header. If given without arguments
  it reports the current value

  This keyword is required by the LBTO data archive

*/

int
cmd_piname(char *args, MsgType msgtype, char *reply)
{
  char argbuf[32];

  // check the file descriptor and make sure we have an active connection

  if (ccd.FD<0) {
    strcpy(reply,"No azcam server connection active");
    return CMD_ERR;
  }

  // If we have arguments, get the PI_NAME and set it

  if (strlen(args)>0) {
    strcpy(obs.PIName,args);
    if (setKeyword(&ccd,"PI_NAME",obs.PIName,"Project PI Name(s)",reply)<0)
      return CMD_ERR;
  }
  
  sprintf(reply,"PI_NAME=(%s)",obs.PIName);
  return CMD_OK;

}

// PARTNER command

int
cmd_partner(char *args, MsgType msgtype, char *reply)
{
  char argbuf[32];

  // check the file descriptor and make sure we have an active connection

  if (ccd.FD<0) {
    strcpy(reply,"No azcam server connection active");
    return CMD_ERR;
  }

  // If we have arguments, get the PARTNER info and set it

  if (strlen(args)>0) {
    strcpy(obs.Partner,args);
    if (setKeyword(&ccd,"PARTNER",obs.Partner,"LBT Project Partner(s)",reply)<0)
      return CMD_ERR;
  }
  
  sprintf(reply,"PARTNER=(%s)",obs.Partner);
  return CMD_OK;

}

// PROPID command

int
cmd_propid(char *args, MsgType msgtype, char *reply)
{
  char argbuf[32];

  // check the file descriptor and make sure we have an active connection

  if (ccd.FD<0) {
    strcpy(reply,"No azcam server connection active");
    return CMD_ERR;
  }

  // If we have arguments, get the PROPID info and set it

  if (strlen(args)>0) {
    strcpy(obs.PropID,args);
    if (setKeyword(&ccd,"PROPID",obs.PropID,"Observing Proposal ID",reply)<0)
      return CMD_ERR;
  }
  
  sprintf(reply,"PROPID=(%s)",obs.PropID);
  return CMD_OK;

}

// SUPPORT command

int
cmd_support(char *args, MsgType msgtype, char *reply)
{
  char argbuf[32];

  // check the file descriptor and make sure we have an active connection

  if (ccd.FD<0) {
    strcpy(reply,"No azcam server connection active");
    return CMD_ERR;
  }

  // If we have arguments, get the SUPPORT info and set it

  if (strlen(args)>0) {
    strcpy(obs.Support,args);
    if (setKeyword(&ccd,"SUPPORT",obs.Support,"LBT Support Scientist(s) on duty",reply)<0)
      return CMD_ERR;
  }
  
  sprintf(reply,"SUPPORT=(%s)",obs.Support);
  return CMD_OK;

}

// TELOPS command

int
cmd_telops(char *args, MsgType msgtype, char *reply)
{
  char argbuf[32];

  // check the file descriptor and make sure we have an active connection

  if (ccd.FD<0) {
    strcpy(reply,"No azcam server connection active");
    return CMD_ERR;
  }

  // If we have arguments, get the SUPPORT info and set it

  if (strlen(args)>0) {
    strcpy(obs.TelOps,args);
    if (setKeyword(&ccd,"TELOPS",obs.TelOps,"LBT Telescope Operator(s) on duty",reply)<0)
      return CMD_ERR;
  }
  
  sprintf(reply,"TELOPS=(%s)",obs.TelOps);
  return CMD_OK;

}

/*!  
  \brief EXPNUM command - Set/Query the file counter for the next image
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  expnum [n]

  Sets the 4-digit file counter for the *next* image written by the
  azcam server.  Images created by the azcam client code have the
  pattern:
  <pre>
       /home/data/instID.CCYYMMDD.nnnn.fits
  </pre>
  Where "instID" is the instrument channel ID (e.g., mods1b),
  "CCYYMMDD" is the observing date (20250721), and "nnnn" is
  a 4-digit integer image sequence number running from
  0001 thru 9999.

  If this command is given with no arguments, it returns the name of
  the next image to be written to disk, otherwise it reports the new
  next filename pattern with the new counter value.

  Sets the #azcam::FileNum data member in the AzCam data structure.  It
  leaves the #azcam::FileName data member unchanged.  Also sets the
  FILENAME card in the azcam server's header database.

  \sa cmd_filename(), cmd_path()
*/

int
cmd_expnum(char *args, MsgType msgtype, char *reply)
{
  char argbuf[32];
  int ctr;
  char fullname[32];

  // check the file descriptor and make sure we have an active connection

  if (ccd.FD<0) {
    strcpy(reply,"No azcam server connection active");
    return CMD_ERR;
  }

  // If we have arguments, set the counter value

  if (strlen(args)>0) {
    GetArg(args,1,argbuf); 
    ctr = atoi(argbuf);
    if (ctr < 1 || ctr > 9999) {
      sprintf(reply,"Invalid file counter %s - must be 1-9999",argbuf);
      return CMD_ERR;
    }
    imgExpNum(&ccd,ctr,reply);

  }
  else {
    // get current exposure number (-1 is a get)
    imgExpNum(&ccd,-1,reply);
  }

  // what is the full filename now (empty filename string means get)
  
  imgFilename(&ccd,(char *)"",reply);
  
  sprintf(reply,"expnum=%d nextfile=%s",ccd.fileNum,ccd.fileName);
  
  return CMD_OK;

}

/*!  
  \brief FILENAME command - Set/Query the filename for the next image
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  filename [str]

  Sets the rootname for the *next* image written by the azcam server,
  and resets the file counter to 0001.  If given without arguments it
  reports the current filename pattern (rootname and number without path
  or .fits extension).  If you need to use a different file sequence
  number, the FILENAME command should be followed by the EXPNUM
  command.

  Images created by the azcam client code have the pattern:
  <pre>
       /home/data/instID.CCYYMMDD.nnnn.fits
  </pre>
  Where "instID" is the instrumetn channel name (e.g., mods1b),
  "CCYYMMDD" is the observing date (e.g., 20250724), and "nnnn" is a
  4-digit file counter. Rootnames may only use valid alphanumeric
  characters (a-z,0-9)

  Note that a filename cannot be a pure number (e.g., 050701) as these
  are very difficult for parsers to handle (if they see them as numbers,
  leading zeroes get dropped, etc.).  Instead, a mix of letters and
  numbers (e.g., y050701) works best.  The azcam client adds the "." 
  used to separate the rootname and file counter fields.

  If this command is given with no arguments, it returns the current
  filename for the next image to be written including the sequence
  number.

  Sets the #azcam::FileName and #azcam::FileNum members in the AzCam
  data structure.  Also sets the FILENAME card in the azcam server's
  header database.

  \sa cmd_expnum(), cmd_path()
*/

int
cmd_filename(char *args, MsgType msgtype, char *reply)
{
  char cmdStr[64];
  char argbuf[32];
  char fullname[32];

  // check the file descriptor and make sure we have an active connection

  if (ccd.FD<0) {
    strcpy(reply,"No azcam server connection active");
    return CMD_ERR;
  }

  // If we have arguments, send it to azcam and let it deal with it

  if (strlen(args)>0) {
    GetArg(args,1,argbuf);
    if (imgFilename(&ccd,argbuf,reply)<0)
      return CMD_ERR;
  }

  if (imgFilename(&ccd,(char*)"",reply)<0) // get filename
    return CMD_ERR;

  sprintf(reply,"FILENAME=%s,ccd.fileName);
  return CMD_OK;

}

/*!  
  \brief PATH command - Set/Query the path for the next image
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  path [str]

  Sets the path to the disk file system where the next image will be
  written.  If given without arguments, it reports the current path.

  Important: This path must be the file path on the azcam server
  machine (linux) and must follow the correct syntax.  We load
  the current default at runtime
  <pre>
      /home/data
  </pre>
  
  
  \sa cmd_filename(), cmd_expnum()
*/

int
cmd_path(char *args, MsgType msgtype, char *reply)
{
  char argbuf[32];
  char fullname[32];

  // check the file descriptor and make sure we have an active connection

  if (ccd.FD<0) {
    strcpy(reply,"No azcam server connection active");
    return CMD_ERR;
  }

  // If we have arguments, send it to azcam and let it deal with it

  if (strlen(args)>0) {
    GetArg(args,1,argbuf);
    if (imgPath(&ccd,argbuf,reply)<0)
      return CMD_ERR;
  }

  if (imgPath(&ccd,(char*)"",reply)<0) // get image data path
    return CMD_ERR;

  sprintf(reply,"PATH=%s",ccd.filePath);
  return CMD_OK;

}

/*!  
  \brief LASTFILE command - Report the name of the last file written
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  lastfile

  Reports the name of the last (known) file written to disk.  See
  FILENAME (cmd_filename()) for the *next* file to be written.

  \sa cmd_expnum(), cmd_filename()
*/

int
cmd_lastfile(char *args, MsgType msgtype, char *reply)
{
  // If we have arguments, send it to azcam and let it deal with it

  if (getLastFile(&ccd,reply)<0) // get last filename
    return CMD_ERR;

  sprintf(reply,"LastFile=%s",ccd.LastFile);
  return CMD_OK;
}

/*!  
  \brief OBJECT command - setup for an object exposure
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  object [target name]

  Sets the ImageTyp to OBJECT (exposed CCD image), and if given an
  optional target name, also sets the target name.  If given without
  arguments, it also reports the current object name if non-blank.

  The target name is stored in the #obspars::Object data member, and
  the image type code in #obspars::ImageTyp.  Also sets the OBJECT
  and IMAGETYP cards in the image header.

  \sa cmd_flat et al.
*/

int
cmd_object(char *args, MsgType msgtype, char *reply)
{
  char argbuf[32];

  // check the file descriptor and make sure we have an active connection

  if (ccd.FD<0) {
    strcpy(reply,"No azcam server connection active");
    return CMD_ERR;
  }

  // If we have arguments, define the target name

  if (strlen(args)>0)
    strcpy(obs.Object,args);

  obs.ImageTyp = OBJECT;
  ccd.ShutterMode = LIGHT_IMAGE;  // Object images open the shutter

  if (strlen(obs.Object)>0) {
    setKeyword(&ccd,"OBJECT",obs.Object,"Target Name",reply);
    setKeyword(&ccd,"IMGTYPE","OBJECT","Astronomical Object",reply);
    sprintf(reply,"IMGTYPE=OBJECT OBJECT=(%s)",obs.Object);
  }
  else {
    setKeyword(&ccd,"IMGTYPE","OBJECT","Astronomical Object",reply); 
    strcpy(reply,"IMGTYPE=OBJECT");
  }

  return CMD_OK;
}

/*!  
  \brief FLAT command - setup for flat-field exposure
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  flat [target name]

  Sets the ImageTyp to FLAT (flat field image), and if given an optional
  target name, also sets the target name.  If given without arguments,
  it also reports the current target name if non-blank.

  The target name is stored in the #obspars::Object data member, and
  the image type code in #obspars::ImageTyp.  Also sets the OBJECT
  and IMAGETYP cards in the image header.

  \sa cmd_object et al.
*/

int
cmd_flat(char *args, MsgType msgtype, char *reply)
{
  char argbuf[32];

  // check the file descriptor and make sure we have an active connection

  if (ccd.FD<0) {
    strcpy(reply,"No azcam server connection active");
    return CMD_ERR;
  }

  // If we have arguments, define the target name

  if (strlen(args)>0)
    strcpy(obs.Object,args);

  obs.ImageTyp = FLAT;
  ccd.ShutterMode = LIGHT_IMAGE;  // flat field images open the shutter

  if (strlen(obs.Object)>0) {
    setKeyword(&ccd,"OBJECT",obs.Object,"Target Name",reply);
    setKeyword(&ccd,"IMGTYPE","FLAT","Flat Field",reply);
    sprintf(reply,"IMGTYPE=FLAT OBJECT=(%s)",obs.Object);
  }
  else {
    setKeyword(&ccd,"IMGTYPE","OBJECT","Flat Field",reply); 
    strcpy(reply,"IMGTYPE=FLAT");
  }

  return CMD_OK;
}

/*!  
  \brief COMP command - setup for comparison source image
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  comp [target name]

  Sets the ImageTyp to COMP (comparison source image), and if given an
  optional target name, also sets the target name.  If given without
  arguments, it also reports the current target name if non-blank.

  The target name is stored in the #obspars::Object data member, and
  the image type code in #obspars::ImageTyp.  Also sets the OBJECT
  and IMAGETYP cards in the image header.

  \sa cmd_object et al.
*/

int
cmd_comp(char *args, MsgType msgtype, char *reply)
{
  char argbuf[32];

  // check the file descriptor and make sure we have an active connection

  if (ccd.FD<0) {
    strcpy(reply,"No azcam server connection active");
    return CMD_ERR;
  }

  // If we have arguments, define the target name

  if (strlen(args)>0)
    strcpy(obs.Object,args);

  obs.ImageTyp = COMP;
  ccd.ShutterMode = LIGHT_IMAGE;  // comp field images open the shutter

  if (strlen(obs.Object)>0) {
    setKeyword(&ccd,"OBJECT",obs.Object,"Target Name",reply);
    setKeyword(&ccd,"IMGTYPE","COMP","Comparison Source Image",reply);
    sprintf(reply,"IMGTYPE=COMP OBJECT=(%s)",obs.Object);
  }
  else {
    setKeyword(&ccd,"IMGTYPE","OBJECT","Comparison Source Image",reply); 
    strcpy(reply,"IMGTYPE=COMP");
  }

  return CMD_OK;
}

/*!  
  \brief STD command - setup for flux standard source image
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  std [target name]

  Sets the ImageTyp to STD (flux standard source image), and if given an
  optional target name, also sets the target name.  If given without
  arguments, it also reports the current target name if non-blank.

  The target name is stored in the #obspars::Object data member, and
  the image type code in #obspars::ImageTyp.  Also sets the OBJECT
  and IMAGETYP cards in the image header.

  \sa cmd_object et al.
*/

int
cmd_std(char *args, MsgType msgtype, char *reply)
{
  char argbuf[32];

  // check the file descriptor and make sure we have an active connection

  if (ccd.FD<0) {
    strcpy(reply,"No azcam server connection active");
    return CMD_ERR;
  }

  // If we have arguments, define the target name

  if (strlen(args)>0)
    strcpy(obs.Object,args);

  obs.ImageTyp = STD;
  ccd.ShutterMode = LIGHT_IMAGE;  // std field images open the shutter

  if (strlen(obs.Object)>0) {
    setKeyword(&ccd,"OBJECT",obs.Object,"Target Name",reply);
    setKeyword(&ccd,"IMGTYPE","STD","Flux Standard Source",reply);
    sprintf(reply,"IMGTYPE=STD OBJECT=(%s)",obs.Object);
  }
  else {
    setKeyword(&ccd,"IMGTYPE","OBJECT","Flux Standard Source",reply); 
    strcpy(reply,"IMGTYPE=STD");
  }

  return CMD_OK;
}

/*!  
  \brief DARK command - setup for a dark exposure
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  dark [target name]

  Sets the ImageTyp to DARK (integration with shutter closed), and if
  given an optional target name, also sets the target name.  If given
  without arguments, it also reports the current target name if
  non-blank.

  The target name is stored in the #obspars::Object data member, and
  the image type code in #obspars::ImageTyp.  Also sets the OBJECT
  and IMAGETYP cards in the image header.

  \sa cmd_object et al.
*/

int
cmd_dark(char *args, MsgType msgtype, char *reply)
{
  char argbuf[32];

  // check the file descriptor and make sure we have an active connection

  if (ccd.FD<0) {
    strcpy(reply,"No azcam server connection active");
    return CMD_ERR;
  }

  // If we have arguments, define the target name

  if (strlen(args)>0)
    strcpy(obs.Object,args);

  obs.ImageTyp = DARK;
  ccd.ShutterMode = DARK_IMAGE;  // darks keep the shutter closed

  if (strlen(obs.Object)>0) {
    setKeyword(&ccd,"OBJECT",obs.Object,"Target Name",reply);
    setKeyword(&ccd,"IMGTYPE","DARK","Dark Image",reply);
    sprintf(reply,"IMGTYPE=DARK OBJECT=(%s)",obs.Object);
  }
  else {
    setKeyword(&ccd,"IMGTYPE","DARK","Dark Image",reply); 
    strcpy(reply,"IMGTYPE=DARK");
  }

  return CMD_OK;
}

/*!  
  \brief BIAS command - setup for a bias exposure
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  bias [target name]

  Sets the ImageTyp to BIAS (zero-integration readout with shutter
  closed), and if given an optional target name, also sets the target
  name.  If given without arguments, it also reports the current target
  name if non-blank.

  The target name is stored in the #obspars::Object data member, and
  the image type code in #obspars::ImageTyp.  Also sets the OBJECT
  and IMAGETYP cards in the image header.  It also sets the exposure
  time to 0.0 seconds.

  \sa cmd_zero
*/

int
cmd_bias(char *args, MsgType msgtype, char *reply)
{
  char argbuf[32];

  // check the file descriptor and make sure we have an active connection

  if (ccd.FD<0) {
    strcpy(reply,"No azcam server connection active");
    return CMD_ERR;
  }

  // If we have arguments, define the target name

  if (strlen(args)>0)
    strcpy(obs.Object,args);

  obs.ImageTyp = BIAS;
  ccd.ShutterMode = DARK_IMAGE;  // biases keep the shutter closed
  obs.ExpTime = 0.0;  // Biases use a 0.0 second integration time
  SetExposure(&ccd,0.0,reply);

  // Craft the reply 

  if (strlen(obs.Object)>0) {
    setKeyword(&ccd,"OBJECT",obs.Object,"Target Name",reply);
    setKeyword(&ccd,"IMGTYPE","BIAS","Bias (Zero) Image",reply);
    sprintf(reply,"IMGTYPE=BIAS OBJECT=(%s) ExpTime=0.0",obs.Object);
  }
  else {
    setKeyword(&ccd,"IMGTYPE","BIAS","Bias (Zero) Image",reply); 
    strcpy(reply,"IMGTYPE=BIAS ExpTime=0.0");
  }

  return CMD_OK;
}

/*!  
  \brief ZERO command - setup for a zero exposure
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  zero [target name]

  Sets the ImageTyp to ZERO (zero-integration readout with shutter
  closed), and if given an optional target name, also sets the target
  name.  If given without arguments, it also reports the current target
  name if non-blank.

  ZERO is an alias for BIAS, included for compatibility with IRAF
  reduction packages that do not recognize the BIAS image type.

  The target name is stored in the #obspars::Object data member, and
  the image type code in #obspars::ImageTyp.  Also sets the OBJECT
  and IMAGETYP cards in the image header.  It also sets the exposure
  time to 0.0 seconds.

  \sa cmd_bias
*/

int
cmd_zero(char *args, MsgType msgtype, char *reply)
{
  char argbuf[32];

  // check the file descriptor and make sure we have an active connection

  if (ccd.FD<0) {
    strcpy(reply,"No azcam server connection active");
    return CMD_ERR;
  }

  // If we have arguments, define the target name

  if (strlen(args)>0)
    strcpy(obs.Object,args);

  obs.ImageTyp = ZERO;
  ccd.ShutterMode = DARK_IMAGE;  // zeroes keep the shutter closed
  obs.ExpTime = 0.0;  // Zeroes use a 0.0 second integration time
  SetExposure(&ccd,0.0,reply);

  // Craft the reply 

  if (strlen(obs.Object)>0) {
    setKeyword(&ccd,"OBJECT",obs.Object,"Target Name",reply);
    setKeyword(&ccd,"IMGTYPE","ZERO","Zero (Bias) Image",reply);
    sprintf(reply,"IMGTYPE=ZERO OBJECT=(%s) ExpTime=0.0",obs.Object);
  }
  else {
    setKeyword(&ccd,"IMGTYPE","ZERO","Zero (Bias) Image",reply); 
    strcpy(reply,"IMGTYPE=ZERO ExpTime=0.0");
  }

  return CMD_OK;
}

/*!  
  \brief CCDBIN command - Set/Query the CCD on-chip binning factor
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  ccdbin [1|2|4]

  Sets the CCD on-chip binning factor to one of 1x1, 2x2, or 4x4. 
  In this system we restrict the binning options to symmetric
  binning on both axes, and to integer powers of 2 up to 4x4.

  If given without arguments, it reports the binning factor.
  
  Sets the values of the #azcam::ColBin and #azcam::RowBin data members,
  and sends a new SetROI command to the azcam server.

*/

int
cmd_ccdbin(char *args, MsgType msgtype, char *reply)
{
  char argbuf[32];
  int bin;

  // check the file descriptor and make sure we have an active connection

  if (ccd.FD<0) {
    strcpy(reply,"No azcam server connection active");
    return CMD_ERR;
  }

  // If we have arguments, get the new binning value.
  // Must be 1, 2, or 4, all others are invalid

  if (strlen(args)>0) {
    GetArg(args,1,argbuf); 
    bin = atoi(argbuf);
    switch(bin) {
    case 1:
    case 2:
    case 4:
      ccd.ColBin = bin;
      ccd.RowBin = bin;
      break;

    default:
      sprintf(reply,"Invalid binning factor %s - must be 1, 2, or 4",argbuf);
      return CMD_ERR;
      break;

    }
    if (SetROI(&ccd,reply)<0) 
      return CMD_ERR;
  }
  
  if (GetDetPars(&ccd,reply)<0) // get the image readout size
    return -1;

  sprintf(reply,"CCDXBin=%d CCDYBin=%d",ccd.ColBin,ccd.RowBin);
  return CMD_OK;

}

/*!  
  \brief CCDTEMP command - Set/Query the CCD temperature
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  ccdtemp [degC]

  Sets the CCD temperature control set point in degrees Celsius.
  If given without arguments, it reports the current setpoiint and
  queries the azcam server to retrieve the current CCD and Dewar
  temperatures.
  
  Sets the values of the #azcam::SetPoint data member, and by
  querying the AzCam ColBin and #azcam::RowBin data members,
  and sends a new SetROI command to the azcam server.

*/

int
cmd_ccdtemp(char *args, MsgType msgtype, char *reply)
{
  char argbuf[32];
  float newtemp;
  double dtemp;

  // check the file descriptor and make sure we have an active connection

  if (ccd.FD<0) {
    strcpy(reply,"No azcam server connection active");
    return CMD_ERR;
  }

  // If we have arguments, change the setpoint and upload it.

  if (strlen(args)>0) {
    GetArg(args,1,argbuf); 
    newtemp = atof(argbuf); 
    if (SetTemp(&ccd,newtemp,reply)<0) 
      return CMD_ERR;
  }

  // Query the azcam server for the current temperatures

  if (GetTemp(&ccd,reply)<0)
    return CMD_ERR;

  // and report them.

  sprintf(reply,"SetPoint=%.1f CCDTemp=%.1f DewarTemp=%.1f",
	  ccd.SetPoint,ccd.CCDTemp,ccd.DewarTemp);

  // Compare the CCDTemp to the SetPoint, and issue a warm-up
  // warming if CCDTemp is >10C above the setpoint

  dtemp = ccd.CCDTemp - ccd.SetPoint;
  if (dtemp > 10.0) 
    strcat(reply," +Warm");
  else
    strcat(reply," +Cold");
 
  return CMD_OK;

}

/*!  
  \brief SHOPEN command - Open the CCD Shutter
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  shopen

  Commands the azcam server to open the shutter.  The shutter will stay
  open until it is explicitly closed.

  \sa cmd_shclose()
*/

int
cmd_shopen(char *args, MsgType msgtype, char *reply)
{

  // check the file descriptor and make sure we have an active connection

  if (ccd.FD<0) {
    strcpy(reply,"No azcam server connection active");
    return CMD_ERR;
  }

  if (OpenShutter(&ccd,reply)<0)
    return CMD_ERR;

  strcpy(reply,"Shutter=1");
  return CMD_OK;
}

/*!  
  \brief SHCLOSE command - Close the CCD Shutter
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  shclose

  Commands the azcam server to close the shutter.

  \sa cmd_shopen()
*/

int
cmd_shclose(char *args, MsgType msgtype, char *reply)
{

  // check the file descriptor and make sure we have an active connection

  if (ccd.FD<0) {
    strcpy(reply,"No azcam server connection active");
    return CMD_ERR;
  }

  if (CloseShutter(&ccd,reply)<0)
    return CMD_ERR;

  strcpy(reply,"Shutter=0");
  return CMD_OK;
}

/*!  
  \brief SHUTTER command - IE-style Shutter Command
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  shutter [0|1]

  Commands the azcam server to open (1) or close (0) the shutter.
  Provided for back-compatibility with older IE-type system
  prototypes.

  If given with no arguments, returns the current shutter state
  (1 or 0).

  \sa cmd_shopen(), cmd_shclose()
*/

int
cmd_shutter(char *args, MsgType msgtype, char *reply)
{
  char argbuf[32];
  int ishut;
  int ierr;

  // check the file descriptor and make sure we have an active connection

  if (ccd.FD<0) {
    strcpy(reply,"No azcam server connection active");
    return CMD_ERR;
  }

  if (strlen(args)>0) { // have an argument
    GetArg(args,1,argbuf);
    ishut = atoi(argbuf);
    if (ishut)
      ierr = OpenShutter(&ccd,reply);
    else
      ierr = CloseShutter(&ccd,reply);
    if (ierr<0)
      return CMD_ERR;
  }
  else
    sprintf(reply,"Shutter=%d",ccd.Shutter);

  return CMD_OK;
}

/*!  
  \brief STATUS command - report the azcam server status
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  status

  Returns the current Y4KCam client status info as an IMPv2-compliant
  status message.  The status message is essentially a dump of the
  contents of the #azcam struct in a human-readable (and IMPv2 client
  parsable) form.

  See cmd_ccdinfo() for azcam server detailed status, cmd_fwinfo() for
  detailed filter wheel status, and cmd_tcinfo() for detailed PC-TCS
  interface status.

*/

int
cmd_status(char *args, MsgType msgtype, char *reply)
{
  float dtemp;

  GetTemp(&ccd,reply);

  sprintf(reply,"Sh=%d Exp=%.3f Xbin=%d Ybin=%d ROI=(%d,%d,%d,%d)",
	  ccd.Shutter,ccd.ExpTime,ccd.ColBin,ccd.RowBin,
	  ccd.FirstCol,ccd.LastCol,ccd.FirstRow,ccd.LastRow);

  sprintf(reply,"%s XOS=%d YOS=%d",
	  reply,ccd.NCoverscan,ccd.NRoverscan);

  switch(obs.ImageTyp) {
  case OBJECT:
    strcat(reply," Typ=OBJECT");
    break;
  case FLAT:
    strcat(reply," Typ=FLAT");
    break;
  case COMP:
    strcat(reply," Typ=COMP");
    break;
  case STD:
    strcat(reply," Typ=STD");
    break;
  case DARK:
    strcat(reply," Typ=DARK");
    break;
  case BIAS:
    strcat(reply," Typ=BIAS");
    break;
  case ZERO:
    strcat(reply," Typ=ZERO");
    break;
  case FOCUS:
    strcat(reply," Typ=FOCUS");
    break;
  }
  
  sprintf(reply,"%s Obj=(%s) File=%s.%4.4d.fits Last=%s",
	  reply,obs.Object,ccd.fileName,ccd.fileNum,ccd.LastFile);

  sprintf(reply,"%s Path=%s Temp=%.1f DTemp=%.1f Filt=%d",
	  reply,ccd.filePath,ccd.CCDTemp,ccd.DewarTemp,obs.Filter);


  // Look at the difference between the setpoint and actual CCD
  // temperature.  If bigger than +10C, we're warm or at least warming
  // up!

  dtemp = ccd.CCDTemp - ccd.SetPoint;
  if (dtemp > 10.0) 
    strcat(reply," +Warm");
  else
    strcat(reply," +Cold");
  
  return CMD_OK;
}

/*!
  \brief CONFIG command - return basic instrument configuration info

  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  config

  Returns the instrument configuration as an IMPv2-compliant message
  string.  This tells the remote client (e.g., Prospero) what features
  are enabled and other useful information.

*/

int
cmd_config(char *args, MsgType msgtype, char *reply)
{
  sprintf(reply,"Inst=%s",obs.Instrument);

  if (ccd.FD>0)
    strcat(reply," IC=Enabled");
  else
    strcat(reply," IC=Disabled");

  if (fw.Link)
    strcat(reply," IE=Enabled");
  else
    strcat(reply," IE=Disabled");

  if (tcs.Link)
    strcat(reply," TC=Enabled");
  else
    strcat(reply," TC=Disabled");

  if (dm.useDM)
    strcat(reply," DM=Enabled");
  else
    strcat(reply," DM=Disabled");

  return CMD_OK;
}

/*!
  \brief SAVECONFIG command - save the current runtime configuration

  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  saveconfig [cfgfile]

  Saves the current runtime configuration of the instrument. If given
  with no arguments, uses the config file it was started with.  If a
  filename is given, it creates a new configuration file.  The file
  (cfgfile) must include the full path, and it must be writable by the
  user.

*/

int
cmd_saveconf(char *args, MsgType msgtype, char *reply)
{
  char cfgfile[64];

  if (strlen(args)>0)
    strcpy(cfgfile,args);
  else
    strcpy(cfgfile,client.rcFile);

  // Should we warn about overwrite?

  if (SaveConfig(cfgfile)<0) {
    sprintf(reply,"Could not save config file %s\n",cfgfile);
    return CMD_ERR;
  }
  sprintf(reply,"Wrote config file %s\n",cfgfile);
  return CMD_OK;

}

/*!  
  \brief CCDINFO command - print the CCD camera configuration on stdout
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  ccdinfo

  Prints the contents of the #azcam string on the console (stdout), giving
  a snapshot of the AzCam CCD camera configuration.  This is an engineering
  command not accessible to remote hosts.

*/

int
cmd_ccdinfo(char *args, MsgType msgtype, char *reply)
{
  if (msgtype == EXEC) {
    AzCamInfo(&ccd);
  }
  else {
    strcpy(reply,"Cannot execute CCDINFO command except as EXEC:");
    return CMD_ERR;
  }
  strcpy(reply,"CCDINFO dump complete.");
  return CMD_OK;
}

/*!  
  \brief AZCAM command - send a raw AzCam command to the azcam server
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  azcam cmd

  Sends a raw command string as-is to the azcam server.  If blank it gripes
  and returns immediately.

*/

int
cmd_azcam(char *args, MsgType msgtype, char *reply)
{

  // check the file descriptor and make sure we have an active connection

  if (ccd.FD<0) {
    strcpy(reply,"No azcam server connection active");
    return CMD_ERR;
  }

  // If we have arguments, get the exposure time and set it

  if (strlen(args)==0) {
    strcpy(reply,"Cannot send a null command to the azcam server");
    return CMD_ERR;
  }

  // Do it, whatever it is

  if (ARCCommand(&ccd,args,reply)<0)
    return CMD_ERR;

  return CMD_OK;
}

//
// Exposure Control Commands
//
// GO n - take n exposures (n=1 if absent).  Returns control to the main event
//        loop, setting the ccd.State variable as required.
//
// FOCUS - Take a focus image, the most complex of these commands
//
// PAUSE - Pause an exposure in progress.  Must follow with Resume or Abort
//
// RESUME - Resume a paused exposure
//
// ABORT - Sends an AbortExposure command to the azcam server
//

/*!  
  \brief GO command - take an image
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_NOOP on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  go

  Starts an exposure with the CCD camera.  If doing a normal exposed
  ("light") or dark image, it calls DoExposure().  If doing a BIAS or
  ZERO image, it calls DoBias().

  Both functions set the ccd.State flag to signal to the main event
  handler loop in main as to the detector state, and otherwise returns
  #CMD_NOOP.  The main loop takes care of sending info out to the client
  who requested the GO command, and for servicing any ABORT requests
  during the exposure (why we do this).

  \sa cmd_abort(), cmd_focus()
*/

int
cmd_go(char *args, MsgType msgtype, char *reply)
{
  char argbuf[32];
  float expt;

  // check the file descriptor and make sure we have an active connection

  if (ccd.FD<0) {
    strcpy(reply,"No azcam server connection active");
    return CMD_ERR;
  }

  // Some care must be taken here to make sure the AzCamServer state is IDLE
  // before executing an exposure

  switch(ccd.State) {
  case PAUSE:
    strcpy(reply,"Cannot GO during a PAUSEd exposure");
    return CMD_ERR;
    break;

  case EXPOSING:
    strcpy(reply,"Exposure in progress, GO not allowed");
    return CMD_ERR;
    break;
    
  case READOUT:
    strcpy(reply,"Readout in progress, GO not allowed");
    return CMD_ERR;
    break;
    
  case IDLE:
    // Good to GO as it were, fall through
    break;

  default:
    strcpy(reply,"AzCamServer State UNKNOWN, GO not allowed");
    return CMD_ERR;
    break;
  }    

  // What we do depends on the type of image we're to acquire

  switch(obs.ImageTyp) {
  case OBJECT:
  case FLAT:
  case COMP:
  case STD:
  case DARK:
    if (DoExposure(&ccd,&obs,reply)<0)
      return CMD_ERR;
    break;

  case BIAS:
  case ZERO:
    if (DoBias(&ccd,&obs,reply)<0)
      return CMD_ERR;
    break;

  default:
    sprintf(reply,"Unknown IMGTYPE %d - Cannot Initiate Exposure",
	    obs.ImageTyp);
    return CMD_ERR;
    break;
  }

  // We're off, ccd.State tells the main event loop what to do

  return CMD_NOOP;

}

/*!  
  \brief FOCUS command - take a focus plate image
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  focus [first|last] nrows

  Takes a focus image, one image in a multiple-exposure sequence in
  which the shutter is opened for #obspars::ExpTime, and then closed but
  not readout (unless "last" is given).  If the first or intermediate
  image in the sequence, the image is shifted vertically by nrows pixels
  after the shutter closes.

  If the first keyword is used, the CCD array is erased and the image
  becomes the first image in the sequence, and the image is shifted by
  nrows after the shutter closes but not read out.  If no post-exposure
  rowshift is required, the command must use nrows=0 (blank is not
  allowed).  The CCD readout mode is set to DEFERRED.

  If neither keyword is used, an exposure is taken without erasing the
  CCD and the image is shifted nrows after the shutter closes.

  If the last keyword is used, the CCD is exposed without first erasing
  the CCD then readout.  In this case the nrows argument is ignored,
  and the CCD readout mode is set to IMMEDIATE.

  The usual sequence of a focus image is as follows:
  <pre>
     focus first 10
     focus 10
     focus 10
     ...
     focus 20
     focus last
  </pre>
  The first image sets up the focus plate, erasing the CCD, taking the
  exposure and then shifting the image up 10 rows.

  Subsequent focus commands instruct the azcam server to take an image
  and then shift the image 10 rows after the shutter closes.  The next
  to last image is taken with a double rowshift of 20 rows to make the
  direction of the image sequence unambiguous.

  The last command takes one last focus image and then readsout the
  array.

  A focus image may be aborted simply by taking a regular exposure,
  which always erases the CCD first.
  
*/

int
cmd_focus(char *args, MsgType msgtype, char *reply)
{
  char argbuf[32];
  int shift = 0;
  int first = 0;
  int last = 0;

  // check the file descriptor and make sure we have an active connection

  if (ccd.FD<0) {
    strcpy(reply,"No azcam server connection active");
    return CMD_ERR;
  }

  // Some care must be taken here to make sure the AzCamServer state is IDLE
  // before executing an exposure

  switch(ccd.State) {
  case PAUSE:
    strcpy(reply,"Exposure PAUSEd, FOCUS not allowed");
    return CMD_ERR;
    break;

  case EXPOSING:
    strcpy(reply,"Exposure in progress, FOCUS not allowed");
    return CMD_ERR;
    break;
    
  case READOUT:
    strcpy(reply,"Readout in progress, FOCUS not allowed");
    return CMD_ERR;
    break;
    
  case IDLE:
    // We're OK to expose, fall through
    break;

  default:
    strcpy(reply,"AzCamServer State UNKNOWN, FOCUS not allowed");
    return CMD_ERR;
    break;
  }    

  // If we have no arguments, assume a focus image with zero row shift.

  if (strlen(args)==0) {
    first = 0;
    last = 0;
    shift = 0;
  }
  else {
    GetArg(args,1,argbuf);
    if (strcasecmp(argbuf,"FIRST")==0) { // first image in the sequence
      first = 1;
      last = 0;
      GetArg(args,2,argbuf);
      shift = atoi(argbuf);
    }
    else if (strcasecmp(argbuf,"LAST")==0) { // last image in the sequence
      last = 1;
      first = 0;
      shift = 0;
    }
    else { // a number of rows to shift
      first = 0;
      last = 0;
      shift = atoi(argbuf);
    }
  }

  if (DoFocus(&ccd,&obs,shift,first,last,reply)<0)
    return CMD_ERR;

  strcpy(reply,"Focus image acquired, Readout Deferred.");

  if (last) {
    strcpy(reply,"Last Focus image acquired, Reading out...");
    return CMD_NOOP;  // because we now hand off to readout processing
  }

  if (first)
    strcpy(reply,"First Focus image acquired, Readout Deferred.");

  return CMD_OK;
}

/*!  
  \brief PAUSE command - Pause an Exposure in Progress
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_NOOP on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  Pause

  Pause an integration in progress by sending a PauseExposure command
  to the azcam server and setting the ccd.State flag to PAUSE.

  Not allowed (or at least very unpredictable) if the ccd.State is
  not EXPOSING.

  \sa cmd_go(), cmd_resume(), cmd_abort()
*/

int
cmd_pause(char *args, MsgType msgtype, char *reply)
{
  
  // check the file descriptor and make sure we have an active connection

  if (ccd.FD<0) {
    strcpy(reply,"No azcam server connection active");
    return CMD_ERR;
  }

  // Some care must be taken as PauseExposure can cause unpredictable
  // behavior if sent when the AzCamServer is in certain states.  What
  // we do depends on the value of ccd.State

  switch(ccd.State) {

  case EXPOSING:  // Exposure in progress, OK to Pause
    if (PauseExposure(&ccd,reply)<0)
      return CMD_ERR;
    break;

  case PAUSE:  // Exposure already paused...
    strcpy(reply,"Already PAUSEd, only RESUME or ABORT allowed");
    return CMD_ERR;
    break;

  case IDLE:  // Cannot Pause while idle (OK, maybe, but unpredictable), gripe
    strcpy(reply,"No exposure in progress, Cannot PAUSE");
    return CMD_ERR;
    break;

  case READOUT:  // Cannot Pause during readout
    strcpy(reply,"Readout in Progress, Cannot PAUSE");
    return CMD_ERR;
    break;

  default:  // Don't know the state, cannot abort
    strcpy(reply,"AzCamServer state is Unknown, PAUSE not allowed");
    return CMD_ERR;
    break;

  }

  // We were able to Pause, set the CCD state to PAUSE

  strcpy(reply,"Exposure PAUSEd - RESUME to continue or ABORT");
  NotifyClient(&obs,reply,STATUS);
  ccd.State = PAUSE;
  ccd.Abort = 0;

  return CMD_NOOP;
}

/*!  
  \brief RESUME command - Resume a Paused Exposure 
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_NOOP on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  Resume

  Resume a PAUSEd integration in progress by sending a ResumeExposure
  command to the azcam server and setting the ccd.State flag to
  EXPOSING.

  Not allowed if the ccd.State is not PAUSE as it is known to cause the
  azcam server to lock up and/or crash and reboot.  Probably all of a
  piece with the hardware interaction issues causing problems with abort
  and exposure progress polling.

  \sa cmd_go(), cmd_pause(), cmd_abort()
*/

int
cmd_resume(char *args, MsgType msgtype, char *reply)
{
  
  // check the file descriptor and make sure we have an active connection

  if (ccd.FD<0) {
    strcpy(reply,"No azcam server connection active");
    return CMD_ERR;
  }

  // Some care must be taken as ResumeExposure can cause unpredictable
  // and predictably bad behavior if sent when the AzCamServer is in
  // certain states.  What we do depends on the value of ccd.State

  switch(ccd.State) {

  case PAUSE:  // We have a PAUSEd exposure, RESUME permitted
    if (ResumeExposure(&ccd,reply)<0)
      return CMD_ERR;
    break;

  case EXPOSING:  // Exposure in progress, Cannot Resume
    strcpy(reply,"Exposure in Progress, only PAUSE or ABORT allowed");
    return CMD_ERR;
    break;

  case IDLE:  // Cannot Resume while idle - can crash/reboot the server
    strcpy(reply,"No exposure in progress, RESUME not allowed");
    return CMD_ERR;
    break;

  case READOUT:  // Cannot RESUME during readout
    strcpy(reply,"Readout in Progress, RESUME not allowed");
    return CMD_ERR;
    break;

  default:  // Don't know the state, cannot abort
    strcpy(reply,"AzCamServer state is Unknown, RESUME not allowed");
    return CMD_ERR;
    break;

  }

  // We were able to Resume, set the CCD state to EXPOSING

  strcpy(reply,"Exposure Resumed...");
  NotifyClient(&obs,reply,STATUS);
  ccd.State = EXPOSING;
  ccd.Abort = 0;

  // Return CMD_NOOP to return exposure control to the main event handler

  return CMD_NOOP;
}

/*!  
  \brief ABORT command - Abort an Exposure in Progress
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  Abort

  Aborts an integration in progress.  The abort is actually a 2-step process
  of sending two commands to the azcam server in sequence:
  <ol>
  <li>PauseExposure
  <li>AbortExposure
  </ol>
  The first pauses the exposure, the second tells it how to exit the
  paused state, here "Abort" = cancel integration and discard the
  image.  

  During development, we found that if an AbortExposure directive is
  sent without being preceeded by PauseExposure, the AzCamServer will
  eventually crash (rebooting the server WinXP system).  The stock
  AzCamTool provided by ITL does the Pause/Abort sequence, which
  suggests this might be of a piece with the ReadExposure-related crash
  we experienced (see PollExposure()).  As such, what we do depends
  on the value of the ccd.State flag.

  On a successful abort, it sets the ccd.State flag to IDLE, and
  the ccd.Abort flag to 1 (true).

  \sa cmd_go(), cmd_focus(), cmd_pause(), cmd_resume()
*/

int
cmd_abort(char *args, MsgType msgtype, char *reply)
{

  // check the file descriptor and make sure we have an active connection

  if (ccd.FD<0) {
    strcpy(reply,"No azcam server connection active");
    return CMD_ERR;
  }

  // Some care must be taken as AbortExposure can cause unpredictable
  // behavior if sent when the AzCamServer is in certain states.  What
  // we do depends on the value of ccd.State

  switch(ccd.State) {

  case EXPOSING:  // Exposure in progress, AbortExposure allowed, but PauseExposure first
    if (PauseExposure(&ccd,reply)<0) // First pause, then abort... weird
      return CMD_ERR;

    if (AbortExposure(&ccd,reply)<0) // If we get errors, abort may have failed
      return CMD_ERR;
    
    break;

  case PAUSE:  // Exposure PAUSEd already, so we can send AbortExposure directly
    if (AbortExposure(&ccd,reply)<0) // If we get errors, abort may have failed
      return CMD_ERR;
    break;

  case IDLE:  // Cannot abort when idle, gripe
    strcpy(reply,"Cannot Abort, no exposure in progress");
    return CMD_ERR;
    break;

  case READOUT:  // Cannot abort during readout
    strcpy(reply,"Readout in Progress, Cannot Abort");
    return CMD_ERR;
    break;

  default:  // Don't know the state, cannot abort
    strcpy(reply,"AzCamServer state is Unknown, Cannot Abort");
    return CMD_ERR;
    break;

  }

  // We were able to send the Abort, so set the CCD state to IDLE and
  // set the Abort flag

  ccd.State = IDLE;
  ccd.Abort = 1;  

  strcpy(reply,"Exposure Aborted");
  NotifyClient(&obs,reply,DONE);  // Abort is always "done"
  return CMD_OK;
}

/*!  
  \brief CLEANUP command - Clean up the runtime state after serious errors
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  cleanup

  Attempts to clean up after an abort or other serious errors where the
  runtime state can be left in something of a mess.  It performs the
  following cleanup tasks
  <ol>
  <li>Sends an AbortExposure command to make sure no image integration 
      continues.
  <li>Sends a CloseShutter command to make sure the shutter is closed.
  <li>Sets the ccd.State flag to IDLE
  <li>Sets the ccd.Abort flag to 0
  <li>Queries to reset the readout pixel counters (GetDetPars())
  <li>Instructs the server to clear the CCD array
  <li>Gets the current CCD and Dewar temperatures
  </ol>
  It's here not because the routines that normally handle errors don't
  do a good job, but because sometimes things get ugly.  If cleanup
  doesn't clear up problems, the only recourse is to do a full restart
  of the system (and it can get that bad sometimes).

*/

int
cmd_cleanup(char *args, MsgType msgtype, char *reply)
{

  // check the file descriptor and make sure we have an active connection

  if (ccd.FD<0) {
    strcpy(reply,"No azcam server connection active");
    return CMD_ERR;
  }
  
  ccd.State = IDLE;
  ccd.Abort = 0;
  CloseShutter(&ccd,reply);
  GetPixelCount(&ccd,reply);
  ClearArray(&ccd,reply);
  GetTemp(&ccd,reply);

  strcpy(reply,"Cleanup Completed");
  return CMD_OK;
}

/*!  
  \brief WRITE command - Write an image to disk
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  write [image]

  Instructs the azcam server to write the current contents of the
  image buffer to disk.  This is usually used to recover "by hand"
  when a regular post-exposure write fails (most often because the
  azcam server refuses to overwrite images on its data disks).

  If given with no arguments, the the current filename (as defined by
  PATH, FILENAME and EXPNUM commands) will be used and the file
  counter will be advanced.

  If the optional argument is given, it overrides the current file
  pattern except for the path on the server.  The file counter will
  not be changed.

  In either case, this command should be followed with a PROCESS
  command to initiate any offline post-processing that might normally
  follow a proper readout.

*/

int
cmd_writeim(char *args, MsgType msgtype, char *reply)
{
  char argbuf[64];
  char msgstr[256];
  char newfile[256];

  // check the file descriptor and make sure we have an active connection

  if (ccd.FD<0) {
    strcpy(reply,"No azcam server connection active");
    return CMD_ERR;
  }

  memset(newfile,0,sizeof(newfile));

  // If we have an argument, use that for the filename, but
  // use the default path.

  if (strlen(args)>0) {
    GetArg(args,1,argbuf);

    // If the .fits extension is not part of the new filename, add it

    if (strstr(argbuf,".fits"))
      sprintf(newfile,"%s\\%s",ccd.filePath,argbuf);
    else
      sprintf(newfile,"%s\\%s.fits",ccd.filePath,argbuf);

    // write it 

    if (WriteImage(&ccd,newfile,msgstr)<0) {
      sprintf(reply,"Write failed for %s - %s",newfile,msgstr);
      return CMD_ERR;
    }
    strcpy(ccd.LastFile,argbuf);
    sprintf(reply,"LastFile=%s written to disk",argbuf);
  }
  else {  // write using the default pattern
    if (WriteCCDImage(&ccd,&obs,reply)<0)
      return CMD_ERR;
  }

  return CMD_OK;

}

/*!  
  \brief PROCESS command - Upload image info to the data manager for post-processing
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  process [image]

  Uploads information about the image to the Data Manager (if enabled)
  to initiate any post-processing of the image (e.g., transfer to
  a Linux disk from the azcam server, display, archiving, logging,
  etc).  This processing is handled off-line.

  This command is usually used to recover "by hand" when a regular
  post-exposure write fails (most often because the azcam server
  refuses to overwrite images on its data disks).  The PROCESS
  command is usually preceded by a WRITE command to first write
  out the image to the azcam server disks, then PROCESS is used
  to initiate post-processing.

  If given with no arguments, the current last filename generated by
  the previous write (either automatic or via the WRITE command) 
  is used.

  If the optional argument is given, it overrides the current file
  pattern.  Note that image must not contain any path information,
  the Data Manager has the correct info (if it does not, the user
  has done something wrong...).

*/

int
cmd_process(char *args, MsgType msgtype, char *reply)
{
  char argbuf[64];
  char fname[256];

  // check the file descriptor and make sure we have an active connection

  if (!dm.useDM) {
    strcpy(reply,"No Data Manager agent link is active");
    return CMD_ERR;
  }

  memset(fname,0,sizeof(fname));

  // If we have an argument, use that for the filename, but
  // use the default path.

  if (strlen(args)>0) {
    GetArg(args,1,argbuf);

    // If the .fits extension is not part of the new filename, add it

    if (strstr(argbuf,".fits"))
      strcpy(fname,argbuf);
    else
      sprintf(fname,"%s.fits",argbuf);

  }
  else {
    strcpy(fname,ccd.LastFile);
  }

  // upload it to DataMan for post-processing

  if (ProcessCCDImage(&ccd,&obs,fname,reply)<0)
    return CMD_ERR;

  return CMD_OK;

}

/*!  
  \brief CCDINIT command - (Re)Initialize the CCD Controller
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  ccdinit

  ReInitializes the azcam server CCD controller configuration by executing the
  InitCCDConfig() function used at the start of the session.

*/

int
cmd_ccdinit(char *args, MsgType msgtype, char *reply)
{
  char msgstr[256];

  // check the file descriptor and make sure we have an active connection

  if (ccd.FD<0) {
    strcpy(reply,"No azcam server connection active");
    return CMD_ERR;
  }

  memset(msgstr,0,sizeof(msgstr));
  if (InitCCDConfig(&ccd,msgstr)<0) {
    sprintf(reply,"CCDINIT Failed - %s",msgstr);
    return CMD_ERR;
  }

  strcat(reply,msgstr);
  return CMD_OK;

}

//----------------------------------------------------------------
//
// Filter Wheel Commands
//

/*!
  \brief FWINIT - Initialize the Filter Wheel Interface

  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  fwinit 

  Initializes the filter wheel interface.  Usually used to restart the
  interface if there have been problems, as the filter wheel is usually
  initialized at client startup time.  Function calls FWStartup().

*/

int
cmd_fwinit(char *args, MsgType msgtype, char *reply)
{

  if (fw.useFW) {
    if (FWStartup(&fw)<0) {
      strcpy(reply,"Filter Wheel Init Failed FW=Disabled - see console for details");
      return CMD_ERR;
    }
    else {
      strcpy(reply,"Filter Wheel Initialized FW=Enabled");
      return CMD_OK;
    }
  }
  else {
    strcpy(reply,"Filter Wheel no enabled for use, cannot init");
    return CMD_ERR;
  }

}

/*!
  \brief FILTER - Set/Query the filter in the beam position

  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  filter [n]

  Puts filter n into the beam, or if given with no arguments queries
  the current filter position.  Also saves the current filter info in
  the #obspars::Filter and #obspars::FilterID data members

*/

int
cmd_filter(char *args, MsgType msgtype, char *reply)
{
  char argbuf[64];
  int reqfilt;
  char keyval[16];

  // Make sure the filter wheel is enabled...

  if (!fw.useFW) {
    strcpy(reply,"No Filter Wheel Enabled by this Configuration");
    return CMD_ERR;
  }

  // And if enabled, active

  if (!fw.Link) {
    strcpy(reply,"Filter Wheel Offline");
    return CMD_ERR;
  }

  // If we have no arguments, this is just a query

  if (strlen(args)>0) {
    GetArg(args,1,argbuf);
    reqfilt = atoi(argbuf);
    if (reqfilt < 1 || reqfilt > fw.npos) {
      sprintf(reply,"Invalid filter requested '%s' must be 1..%d",
	      argbuf,fw.npos);
      return CMD_ERR;
    }
  }
  else {
    reqfilt = 0;  // query position only
  }

  // Set the filter or just query as required

  if (FWSelect(&fw,reqfilt) < 0) {
    sprintf(reply,"Could not set the filter, Filter=0 FilterID=(Unknown) - see console for details");
    fw.beampos = 0;
    obs.Filter = 0;
    strcpy(obs.FilterID,"UNKNOWN");
    return CMD_ERR;
  }
  sprintf(reply,"Filter=%d FilterID=(%s)",
	  fw.beampos,(fw.FilterID)[fw.beampos-1]);
  obs.Filter = fw.beampos;
  strcpy(obs.FilterID,(fw.FilterID)[fw.beampos-1]);

  return CMD_OK;

}
/*!
  \brief LOADFILT - Set/Query the filter in the load position

  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  loadfilt [n]

  Moves filter n to the load port.  If this command is given with no
  arguments it will return the filter currently at the load port.  Used
  to move filters into the load port when adding or removing filters
  from the filter wheel.

*/

int
cmd_loadfilt(char *args, MsgType msgtype, char *reply)
{
  char argbuf[64];
  int reqfilt;
  char keyval[16];

  // Make sure the filter wheel is enabled...

  if (!fw.useFW) {
    strcpy(reply,"No Filter Wheel Enabled by this Configuration");
    return CMD_ERR;
  }

  // And if enabled, active

  if (!fw.Link) {
    strcpy(reply,"Filter Wheel Offline");
    return CMD_ERR;
  }

  // If we have no arguments, this is just a query

  if (strlen(args)>0) {
    GetArg(args,1,argbuf);
    reqfilt = atoi(argbuf);
    if (reqfilt < 1 || reqfilt > fw.npos) {
      sprintf(reply,"Invalid filter requested '%s' must be 1..%d",
	      argbuf,fw.npos);
      return CMD_ERR;
    }
  }
  else {
    reqfilt = 0;  // query position only
  }

  // Set the filter or just query as required

  if (FWLoadFilt(&fw,reqfilt) < 0) {
    sprintf(reply,"Could not move the filter wheel - see console for details");
    fw.beampos = 0;
    obs.Filter = 0;
    strcpy(obs.FilterID,"UNKNOWN");
    return CMD_ERR;
  }
  sprintf(reply,"Load=%d LoadID=(%s)",
	  fw.loadpos,(fw.FilterID)[fw.loadpos-1]);

  // store the current in-beam filter info as well in the obs struct

  obs.Filter = fw.beampos;
  strcpy(obs.FilterID,(fw.FilterID)[fw.beampos-1]);

  return CMD_OK;

}

/*!
  \brief FILTERID - Set/Query a Filter ID

  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  filterid [n [IDstr]]

  Set the ID of filter n.  If IDstr is absent, reports the ID of filter
  n, or if given with no arguments at all, reports all filter IDs.

  Requires that a filter wheel be initialized and a filter table loaded.
*/

int
cmd_filterid(char *args, MsgType msgtype, char *reply)
{
  char argbuf[64];
  char idstr[64];
  int reqfilt;
  int nargs;
  int i;
  char temp[64];

  // Make sure the filter wheel is enabled (it need not be online)

  if (!fw.useFW) {
    strcpy(reply,"No Filter Wheel Enabled by this Configuration");
    return CMD_ERR;
  }

  // If we have no arguments, this is just a query

  if (strlen(args)>0) {  
    nargs = sscanf(args,"%d %[^\n]",&reqfilt,idstr);
    if (nargs <= 0) {
       sprintf(reply,"Illegal command arguments '%s'",args);
       return CMD_ERR;
    }

    // make sure the requested filter is valid

    if (reqfilt < 1 || reqfilt > fw.npos) {
      sprintf(reply,"Invalid filter position '%s', must be 1..%d",
	      argbuf,fw.npos);
      return CMD_ERR;
    }

    // We only got a filter number (e.g., filterid 1), return its ID

    if (nargs==1) {
      sprintf(reply,"%d=(%s)",reqfilt,(fw.FilterID)[reqfilt-1]);
      return CMD_OK;
    }

    // We got 2 arguments, redefine the filter ID associated with reqfilt

    strcpy((fw.FilterID)[reqfilt-1],idstr);
    sprintf(reply,"%d=(%s)",reqfilt,idstr);

  }

  // No arguments, return all filter IDs

  else {
    for (i=0;i<fw.npos;i++) {
      sprintf(temp, "%d=(%s) ",(i+1),(fw.FilterID)[i]);
      strcat(reply,temp);
    }
  }

  return CMD_OK;

}

/*!
  \brief FWTABLE - Load/Query the Filter ID Table

  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  fwtable [file|reload]

  Loads the named filter wheel ID table.  If given with no arguments, 
  returns the name of the filter wheel table in use.  If the "reload"
  keyword is given in place of a filename, it reloads the filter
  table given by the #fwheel_params::FWTable string.

*/

int
cmd_fwtable(char *args, MsgType msgtype, char *reply)
{

  // Make sure the filter wheel is enabled (it need not be online)

  if (!fw.useFW) {
    strcpy(reply,"No Filter Wheel Enabled by this Configuration");
    return CMD_ERR;
  }

  // If we have no arguments, this is just a query

  if (strlen(args)>0) {
    if (strcasecmp(args,"RELOAD")==0) { // reload the current table
      if (FWGetIDs(&fw,reply)<0)
	return CMD_ERR;
    }
    else { // load the named table
      strcpy(fw.FWTable,args);
      if (FWGetIDs(&fw,reply)<0)
	return CMD_ERR;
    }
  }
  else { 
    // just a query, tell them which table we are using
    sprintf(reply,"FWTable=%s",fw.FWTable);
  }

  return CMD_OK;

}

/*!
  \brief ISTATUS - Report the Instrument Status

  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  istatus

  Reports the instrument status.

*/

int
cmd_istatus(char *args, MsgType msgtype, char *reply)
{

  // What is the filter wheel state and current beam position?

  if (fw.beampos > 0)
    sprintf(reply,"FW=%s Filter=%d FilterID=(%s)",
	  ((fw.useFW) ? "Enabled" : "Disabled"),
	    fw.beampos,(fw.FilterID)[fw.beampos-1]);
  else
    sprintf(reply,"FW=%s Filter=0 FilterID=(UNKNOWN)",
	    ((fw.useFW) ? "Enabled" : "Disabled"));

  // The CCD shutter and temperatures are part of the "instrument" status

  sprintf(reply,"%s Shutter=%d SetPoint=%.1f CCDTemp=%.1f DewarTemp=%.1f",
	  reply,ccd.Shutter,ccd.SetPoint,ccd.CCDTemp,ccd.DewarTemp);

  return CMD_OK;

}

/*!
  \brief FWINFO - Print the current filter wheel configuration (engineering)

  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  fwinfo

  Prints the current filter wheel configuration info to the console.  This
  command is used for engineering purposes only and is not available to
  remote commands.


*/

int
cmd_fwinfo(char *args, MsgType msgtype, char *reply)
{
  if (msgtype == EXEC) {
    FWInfo(&fw);
  }
  else {
    strcpy(reply,"Cannot execute FWINFO command except as EXEC:");
    return CMD_ERR;
  }
  strcpy(reply,"FWINFO dump complete.");
  return CMD_OK;
}

//----------------------------------------------------------------
//
// PC-TCS Commands
//

/*!
  \brief TCINIT - Initialize the PC-TCS link

  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  tcinit

  Initializes the link to the PC-TCS agent.  This is typically done
  to re-initialize the link after errors, as the application usually
  initializes the link at startup time.

  \sa cmd_tcclose()
*/

int
cmd_tcinit(char *args, MsgType msgtype, char *reply)
{

  if (!client.useISIS) {
    ClosePCTCS(&tcs); // close any delinquent connections if standalone

    if (OpenPCTCS(&tcs,0)<0) {
      sprintf(reply,"Could not connect to the PC-TCS Agent - %s=Disabled",
	      tcs.ID);
      return CMD_ERR;
    }
  }

  // Try to get TCS info

  if (GetPCTCSInfo(&tcs,reply)<0) {
    sprintf(reply,"PC-TCS init query failed - %s - %s=Disabled",reply,tcs.ID);
    return CMD_ERR;
  }

  sprintf(reply,"PC-TCS Interface Initialized - %s=Enabled",tcs.ID);

  return CMD_OK;
}

/*!
  \brief TCCLOSE - Close the PC-TCS link

  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  tcclose

  Closes the link to the PC-TCS agent and resets the interface state
  flags.

  \sa cmd_tcinit()
*/

int
cmd_tcclose(char *args, MsgType msgtype, char *reply)
{
  if (client.useISIS)
    tcs.Link = 0;
  else
    ClosePCTCS(&tcs);

  sprintf(reply,"PC-TCS Interface Closed - %s=Disabled",tcs.ID);
  return CMD_OK;
}

/*!
  \brief TCSTATUS - Query the PC-TCS for current status info

  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  tcstatus

  Queries the PC-TCS for current telescope status information.  If the link
  is active, it returns an IMPv2 message with the current pointing information
  (Date, Time, JD, RA, Dec, Equinox, HA, LST, and SecZ), and the telescope
  focus and temperature (if available).

  If the link is down, it returns at least the date and time, and sets the
  #pctcs_config::Link flag false so the header knows that the TCS is down
  and only reports time/date info as provided by the azcam server.

*/

int
cmd_tcstatus(char *args, MsgType msgtype, char *reply)
{
  
  if (GetPCTCSInfo(&tcs,reply)<0)
    return CMD_ERR;

  if (client.Debug)
    printf("PC-TCS returned '%s'\n",reply);

  // We got info, print a message in IMPv2 Format

  if (tcs.Link) {
    sprintf(reply,"TCSLink=UP DATE-OBS=%s TIME-OBS=%s JD=%s RA=%s DEC=%s EQUINOX=%s HA=%s LST=%s SECZ=%s TELFOCUS=%s TELTEMP=%s",
	    tcs.DateObs,tcs.TimeObs,tcs.JD,tcs.RA,tcs.Dec,
	    tcs.Equinox,tcs.HA,tcs.LST,tcs.SecZ,tcs.TelFocus,
	    tcs.TelTemp);
    
  } 
  else {
    sprintf(reply,"TCSLink=DOWN DATE-OBS=%s TIME-OBS=%s",
	    tcs.DateObs,tcs.TimeObs);
  }

  // redundant copies of date/time info for obspars_t

  strcpy(obs.DateObs,tcs.DateObs);
  strcpy(obs.TimeObs,tcs.TimeObs);

  return CMD_OK;
}

int
cmd_tcinfo(char *args, MsgType msgtype, char *reply)
{
  if (msgtype == EXEC) {
    PCTCSInfo(&tcs);
  }
  else {
    strcpy(reply,"Cannot execute TCINFO command except as EXEC:");
    return CMD_ERR;
  }
  strcpy(reply,"TCINFO dump complete.");
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
  char reply[BIG_STR_SIZE];     // command reply buffer

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
      if (client.isVerbose) {
	msg[strlen(msg)-1]='\0';
	printf("OUT: %s\n",msg);
      }
    }
    else
      printf("No ISIS server active, > command unavailable\n");
    
  }
  
  // All other commands use the cmd_xxx() action calls

  else { 

    // If the command is GO or FOCUS, record the host requesting it
    
    if (strcasecmp(cmd,"GO")==0 || strcasecmp(cmd,"FOCUS")==0)
      strcpy(obs.expHost,"console");

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

    /* old dangerous stuff - send any non-command verb to azcam server as-is
    if (nfound == 0) { // Send to the comm port as-is if non-blank
      if (strlen(cmd)>0) {
        memset(msgbody,0,sizeof(msgbody));
	if (strlen(args)>0) 
	   sprintf(msgbody,"%s %s",cmd,args);
	else	
	   strcpy(msgbody,cmd);
	if (msgbody[strlen(msgbody)-1] == '\n') msgbody[strlen(msgbody)-1] = '\0';   
	if (client.Debug) printf(">%s: '%s'\n",ccd.Port,msgbody);
	strcat(msgbody,"\n");
	WriteAzCam(&ccd,msgbody);
      }
    }
    */

    // If unknown command, gripe, otherwise do it

    if (nfound == 0) {
      if (strlen(cmd)>0) 
        printf("ERROR: unknown command - %s\n",cmd);
    }
    else {
	
      // All console keyboard are treated as EXEC: type messages
	
      switch (cmdtab[icmd].action(args,EXEC,reply)) {
	
      case CMD_ERR:
	printf("ERROR: %s\n",reply);
	break;
	
      case CMD_OK:
	printf("DONE: %s\n",reply);
	break;
	
      case CMD_NOOP:
      default:
	break;
	
      } // end of switch()
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
  char reply[BIG_STR_SIZE];     // command reply string

  // Other working variables

  int i;
  int nfound=0;
  int icmd=-1;

  // Some simple initializations

  memset(reply,0,sizeof(reply));
  memset(args,0,sizeof(args));
  memset(cmd,0,sizeof(cmd));

  // Split the ISIS format message into components

  if (SplitMessage(buf,srcID,destID,&msgtype,msgbody)<0) {
    if (client.isVerbose)
      printf("\nISIS IN: %s\n",buf);
    return;
  }
        
  // Immediate action depends on the type of message received as
  // recorded by the msgtype code.

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

    // If the command is GO or FOCUS, record the host requesting it
    
    if (strcasecmp(cmd,"GO")==0 || strcasecmp(cmd,"FOCUS")==0)
      strcpy(obs.expHost,srcID);

    // Traverse the command table, exact case-insensitive match required

    nfound = 0;
    for (i=0; i<NumCommands; i++) {
      if (strcasecmp(cmdtab[i].cmd,cmd)==0) { 
	nfound++;
	icmd=i;
	break;
      }
    }

    /* old stuff - sent whatever else we got to the server as-is
    if (nfound == 0) {
      if (strlen(msgbody)>0) {
	if (client.Debug) printf("AzCam> '%s'\n",msgbody);
	strcat(msgbody,"\n");
	WriteAzCam(&ccd,msgbody);
      }
    }
    */
    
    // Unknown command, gripe back to the sender, otherwise try to do it

    if (nfound == 0) {
      sprintf(msg,"%s>%s ERROR: Unknown command - %s\n",
              client.ID,srcID,msgbody);
    }
    else {
      switch(cmdtab[icmd].action(args,msgtype,reply)) {

      case CMD_ERR: // command generated an error
	sprintf(msg,"%s>%s ERROR: %s\n",client.ID,srcID,reply);
	break;

      case CMD_NOOP: // command is a no-op, debug/verbose output only
	if (client.isVerbose)
	  printf("IN: %s from ISIS node %s\n",msgbody,srcID);
	break;

      case CMD_OK:  // command executed OK, return reply
      default:
	sprintf(msg,"%s>%s DONE: %s\n",client.ID,srcID,reply);
	break;
	
      } // end of switch on cmdtab.action()
    }

    // An incoming PING requires special handling - it is an exception
    // to the usual messaging syntax since PONG is sent in reply 

    if (strcasecmp(cmd,"PING") == 0)
      sprintf(msg,"%s>%s %s\n",client.ID,srcID,reply);
      
    break;

  default:  // we don't know what we got, print for debugging purposes
    printf("Malformed message received on client port: %s\n",buf);
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

    if (client.isVerbose) {
      msg[strlen(msg)-1]='\0';
      printf("OUT: %s\n",msg);
    }
  } // end of reply handling

}

