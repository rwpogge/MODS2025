//
// commands.c - application command interpreter
//

/*!
  \file commands.c 
  \brief modsheb Client application command interpreter functions.

  This module contains the command "action" functions called to
  service modsheb client commands.  These consist of a suite of \arg
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
  \date 2010 June 21
*/

#include "client.h"     // Custom client application header file
#include "commands.h"   // Command action functions header file

#include <readline/readline.h>  // Gnu readline utility
#include <readline/history.h>   // Gnu history utility


//***************************************************************************
//
// Common client commands
//
// Commands common to most ISIS client applications are defined here.
//

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
int cmd_quit(char *args, MsgType msgtype, char *reply) {
  if (msgtype == EXEC) {
    client.KeepGoing=0;
    sprintf(reply,"%s=DISABLED MODE=OFFLINE",client.ID);
  }else{
    strcpy(reply,"Cannot execute a remote quit command - operation disallowed");
    return CMD_ERR;
  }

  return CMD_OK;
}

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
int cmd_ping(char *args, MsgType msgtype, char *reply) {
  strcpy(reply,"PONG");
  return CMD_OK;
}

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
int cmd_pong(char *args, MsgType msgtype, char *reply) {
  if (client.isVerbose && useCLI) printf("PONG received\n");
  return CMD_NOOP;
}

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

int cmd_version(char *args, MsgType msgtype, char *reply) {
  sprintf(reply,"modsheb Version=%s CompileDate=%s CompileTime=%s",APP_VERSION,APP_COMPDATE,APP_COMPTIME);
  return CMD_OK;
}
 
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
int cmd_verbose(char *args, MsgType msgtype, char *reply) {
  if (client.isVerbose) {
    client.isVerbose = 0;
    sprintf(reply,"verbose mode disabled");
  }else{
    client.isVerbose = 1;
    sprintf(reply,"verbose mode enabled");
  }

  return CMD_OK;
}

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

  DEBUG may or may not do much depending on the application...

  \sa cmd_verbose
*/
int cmd_debug(char *args, MsgType msgtype, char *reply) {
  if (client.Debug) {
    client.Debug = 0;
    sprintf(reply,"debugging output disabled");
  }else{
    client.Debug = 1;
    sprintf(reply,"debugging output enabled");
  }

  return CMD_OK;
}

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
int cmd_info(char *args, MsgType msgtype, char *reply) {
  int i;

  // Start with the node ID, and host info
  sprintf(reply,"HostID=%s HostAddr=%s:%d",
	  client.ID, client.Host, client.Port);

  // If configured as an ISIS client, report this and the ISIS host:port info,
  // otherwise if standalone, report that, and the host:port of the last
  // remote host to send us something, if known.
  if (client.useISIS) {
    sprintf(reply,"%s Mode=ISISClient ISIS=%s ISISHost=%s:%d",reply,client.isisID,client.isisHost,client.isisPort);
  }else{
    if (strlen(client.remHost)>0)
      sprintf(reply,"%s Mode=STANDALONE RemHost=%s:%d",reply,client.remHost,client.remPort);
    else
      strcat(reply," Mode=STANDALONE");
  }

  // Environmental Monitor information
  sprintf(reply,"%s InstID=%s Cadence=%d runState=%s Logging=%s logFile=%s HdfLogging=%s HdfLogDir=%s",
	  reply, env.modsID, env.cadence, ((env.pause) ? "Paused" : "Active"), ((env.doLogging) ? "Enabled" : "Disabled"),
	  env.logFile, ((env.useHdf5) ? "Enabled" : "Disabled"), env.hdfRoot
  );

  // Report application runtime flags
  sprintf(reply,"%s %s %s",reply,
	  ((client.isVerbose) ? "Verbose" : "Concise"),
	  ((client.Debug) ? "+DEBUG" : "-DEBUG")
  );
	 
  // Finally, report the application's runtime config info as required
  sprintf(reply,"%s rcfile=%s",reply,client.rcFile);

  return CMD_OK;
}

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
int cmd_help(char *args, MsgType msgtype, char *reply) {
  int i, icmd, found;
  int ls;
  char argbuf[32];
  
  if (!useCLI) return 0;

  if (msgtype!=EXEC) {
    strcpy(reply,"Cannot exec help command - remote operation not allowed");
    return CMD_ERR;
  }

  // we are being asked for help on a specific command
  if (strlen(args)>0) {  
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
    }else{
      printf("Unknown Command '%s' (type 'help' to list all commands)\n",argbuf);
    }
  }
  
  // no arguments, print the whole command list
  else{
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
int cmd_history(char *args, MsgType msgtype, char *reply) {
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
  \brief cadence - set/query the enviromental sensor sampling cadence
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  cadence [seconds]
  
  Sets the instrument sensor sampling cadence in seconds.

  If no arguments are given, it reports the current sampling cadence.

  If a cadence less than or equal to zero is given, no change occurs.
  To pause monitoring, use the PAUSE command.

  \sa cmd_pause(), cmd_resume()
*/
int cmd_cadence(char *args, MsgType msgtype, char *reply) {
  char argbuf[32];
  int tcad;

  // if there are command arguments, parse them now
  if (strlen(args)>0) {
    GetArg(args,1,argbuf);
    tcad = atoi(argbuf);

    if (tcad >= 0) {
      env.cadence = tcad;
      sprintf(reply,"Cadence=%d seconds",env.cadence);
      if (env.doLogging) logMessage(&env,reply);
      return CMD_OK;
    }else{
      sprintf(reply,"Invalid cadence '%s', must be >0 seconds",args);
      return CMD_ERR;
    }
  }

  // Query, return the current value
  sprintf(reply,"Cadence=%d seconds",env.cadence);
  return CMD_OK;
}

/*!  
  \brief pause - pause monitoring
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  pause
  
  Pauses monitoring of enviromental sensors.  If already paused, the pause
  continues. To resume monitoring, use the RESUME command.

  \sa cmd_resume()
*/
int cmd_pause(char *args, MsgType msgtype, char *reply) {
  env.pause = 1;
  strcpy(reply,"Monitoring Paused runState=Paused");
  if (env.doLogging) logMessage(&env,"Monitoring Paused");

  return CMD_OK;
}

/*!  
  \brief resume - resume monitoring
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  resume
  
  Resumes monitoring of enviromental sensors at the current cadence after a
  PAUSE.  If already running, it has no effect.

  \sa cmd_pause()
*/
int cmd_resume(char *args, MsgType msgtype, char *reply) {
  env.pause = 0;
  strcpy(reply,"Monitoring Resumed runState=Active");
  if (env.doLogging) logMessage(&env,"Monitoring Resumed");

  return CMD_OK;
}

/*!  
  \brief estatus command - read the instrument sensors return the data
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  estatus
  
  Reads the instrument's enviromental and AC power state sensors and
  returns the data as keyword=value pairs.  It also logs the
  temperature and pressure sensor measurements. If a remote host makes
  a number of estatus queries, it could result in the logs showing a
  stuttered cadence.  A consistent monitoring cadence is maintained
  only when the agent is not otherwise occupied handling command
  requests.

  Reports basic power state information, but not the details of the
  requested switch and breaker sensor states.  For that info see
  pstatus (which, in turns, does *not* evaluate the sensors in terms
  off on/off/fault power state).

  The ESTATUS command ignores the RunState of the agent, so that a
  remote host can read the environmental sensors even when the monitor
  is paused.  It does, however, obey the logging state, and will not
  update the log if logging is disabled.

  \sa cmd_cadence(), cmd_pause(), cmd_resume(), cmd_pstatus()
*/
int cmd_estatus(char *args, MsgType msgtype, char *reply) {
  int ierr;

  // Read the enviromental sensors
  ierr = getInstrumentData(&env);
  if (ierr != 0) {
    strcpy(reply,"Cannot read the enviromental sensors");
    return CMD_ERR;
  }

  //Log to ASCII and HDF5, if enabled.
  if (env.doLogging) logEnvData(&env);    
  if (env.useHdf5) logTelemetryData(&env);

  // Craft the reply string...
  sprintf(reply,"COMMAND NOT FINISHED YET");

  return CMD_OK;
}

/*!  
  \brief pstatus command - read just the instrument AC power status
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  pstatus
  
  Reads the instrument's AC power control status sensors and returns
  the current power state of the instrument.  This is raw, unfiltered
  output for diagnostic purposes as a more readable subset of the data
  returned by the full estatus command.

  This command only reports the switch and breaker state sensors, it does
  not evaluate the end power state in terms of On/Off/Fault conditions.
  See the estatus command for that.

  \sa cmd_cadence(), cmd_pause(), cmd_resume(), cmd_estatus
*/
int cmd_pstatus(char *args, MsgType msgtype, char *reply) {
  int ierr;

  // Read the WAGOs
  ierr = getInstrumentData(&env);
  if (ierr != 0) {
    strcpy(reply,"Cannot read the enviromental sensors");
    return CMD_ERR;
  }

  // Log to ASCII and HDF5, if enabled
  if (env.doLogging) logEnvData(&env);
  if (env.useHdf5) logTelemetryData(&env);

  // Craft the reply string...
  sprintf(reply, "COMMAND NOT FINISHED YET");

  return CMD_OK;
}

/*!  
  \brief CONFIG command - report the agent configuration [engineering]
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  config

  Engineering command that dumps what we know about the current
  configuration to the console.  It is meant for engineering purposes.
  The remote equivalent is the INFO command.

  \sa cmd_info()
*/
int cmd_config(char *args, MsgType msgtype, char *reply) {
  if (msgtype == EXEC){
    printEnvData(&env);
  }else{
    strcpy(reply,"Cannot execute config remotely - use info instead");
    return CMD_ERR;
  }

  return CMD_OK;
}

/*!  
  \brief log command - enable/disable data logging
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  log [enable|disable]

  Enables or disabled instrument sensor data logging.  If given with
  no arguments, it reports the current logging state.  Note that the
  logging state does not affect the pause/resume run state of the data
  monitor.

  We recognize ON and OFF as aliases for ENABLE and DISABLE.

  \sa cmd_pause(), cmd_resume()
*/
int cmd_logging(char *args, MsgType msgtype, char *reply) {
  char argBuf[32];

  if (strlen(args) > 0) {
    GetArg(args,1,argBuf);
    if (strcasecmp(argBuf,"ENABLE")==0 || strcasecmp(argBuf,"ON")==0) {
      env.doLogging = 1;
      logMessage(&env,"Sensor data logging enabled");
    }else if (strcasecmp(argBuf,"DISABLE")==0 || strcasecmp(argBuf,"OFF")==0) {
      logMessage(&env,"Sensor data logging disabled");
      env.doLogging = 0;
    }else{
      sprintf("Unrecognized argument '%s' - usage: logging [enable|disable]",args);
      return CMD_ERR;
    }
  }

  sprintf(reply,"Logging=%s logFile=%s runState=%s", 
    ((env.doLogging) ? "Enabled" : "Disabled"), env.logFile, ((env.pause) ? "Paused" : "Active")
  );

  return CMD_OK;
}

/*!  
  \brief hdflog command - enable/disable hdf data logging
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  hdflog [enable|disable]

  Enables or disabled instrument sensor data logging in the hdf format.  
  If given with no arguments, it reports the current hdf logging state.  
  Note that the logging state does not affect the pause/resume run state 
  of the data monitor.

  We recognize ON and OFF as aliases for ENABLE and DISABLE.

  \sa cmd_pause(), cmd_resume()
*/
int cmd_hdf_logging(char *args, MsgType msgtype, char *reply) {
  char argBuf[32];

  if (strlen(args) > 0) {
    GetArg(args,1,argBuf);
    if (strcasecmp(argBuf,"ENABLE")==0 || strcasecmp(argBuf,"ON")==0) {
      logMessage(&env,"Hdf sensor data logging enabled");
      env.useHdf5 = 1;
    }else if (strcasecmp(argBuf,"DISABLE")==0 || strcasecmp(argBuf,"OFF")==0) {
      logMessage(&env,"Hdf sensor data logging disabled");
      env.useHdf5 = 0;
    }else{
      sprintf("Unrecognized argument '%s' - usage: logging [enable|disable]", args);
      return CMD_ERR;
    }
  }

  sprintf(reply,"HdfLogging=%s HdflogDir=%s runState=%s",
	  ((env.useHdf5) ? "Enabled" : "Disabled"), env.hdfRoot,((env.pause) ? "Paused" : "Active")
  );

  return CMD_OK;
}

/*!  
  \brief cmd_set_all_do_true command - set all DOs to be true
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

*/
int cmd_set_all_do_true(char *args, MsgType msgtype, char *reply) {
  int ierr;

  // Set the registers
  int states[3] = {1,1,1};
  if (setDigitalOutputs(&env, 3, 512, states) != 0){
    sprintf(reply, "Error reading from HEB wago.");
    return CMD_ERR;
  }

  sprintf(reply, "All DOs set to true.");
  return CMD_OK;
}

/*!  
  \brief comment command - append a comment to the environmental log
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  comment string with comment text

  Inserts a comment into the instrument sensor log stream.  This is useful
  for annotating the log during testing (e.g., when doing experiments in
  which one or more systems are powered on or off and the sensor response
  is being measured).
*/
int cmd_comment(char *args, MsgType msgtype, char *reply) {
  char argBuf[32];

  if (strlen(args) > 0) {
    logMessage(&env,args);
    sprintf(reply,"Appended comment '%s' to the log", args);
    return CMD_OK;
  }

  strcpy(reply,"Cannot append a blank comment");
  return CMD_ERR;
}

//***************************************************************************
//
// Command Interpreter I/O Handlers
//

//---------------------------------------------------------------------------
//
// KeyboardCommand() - process a command from the keyboard
//

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
void KeyboardCommand(char *line) {
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
      if (client.isVerbose) {
	      msg[strlen(msg)-1]='\0';
	      printf("OUT: %s\n",msg);
      }
    }else{
      printf("No ISIS server active, > command unavailable\n");
    }
  }
  
  // All other commands use the cmd_xxx() action calls
  else{ 

    // Traverse the command table, matches are case-insensitive, but must be exact word matches (no abbreviations or aliases)
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
    }else{
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
      if (client.Debug) printf("   %s command execution time %.3f seconds\n",cmd,dt);
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
void SocketCommand(char *buf) {

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
      sprintf(msg,"%s>%s ERROR: Unknown command - %s\n",client.ID,srcID,msgbody);
    }else{
      t0 = SysTimestamp();

      switch(cmdtab[icmd].action(args,msgtype,reply)) {
      case CMD_ERR: // command generated an error
	      sprintf(msg,"%s>%s ERROR: %s %s\n",client.ID,srcID,cmd,reply);
	      break;
      case CMD_NOOP: // command is a no-op, debug/verbose output only
	      if (client.isVerbose && useCLI)
	        printf("IN: %s from ISIS node %s\n",msgbody,srcID);
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
    if (useCLI) 
      printf(" [Malformed message received on client port: '%s']\n",buf);
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

