#ifndef COMMANDS_H
#define COMMANDS_H

/*!
  \file commands.h
  \brief MODSHeb client application command tree header file

  To add a command, you need to 
  <ol>
  <li>Add a command action function prototype to the code below
  <li>Add the command verb and its function call to the #cmdtab struct
  </ol>
  
  See commands.c for the full implementation details.
*/

// Common interactive client commands 
int cmd_quit   (char *, MsgType, char *); // quit the application
int cmd_ping   (char *, MsgType, char *); // ping (comm handshake request)
int cmd_pong   (char *, MsgType, char *); // pong (comm handshake acknowledge)
int cmd_info   (char *, MsgType, char *); // return application runtime info
int cmd_version(char *, MsgType, char *); // return version info
int cmd_verbose(char *, MsgType, char *); // toggle verbose mode
int cmd_debug  (char *, MsgType, char *); // toggle debug (superverbose) mode
int cmd_help   (char *, MsgType, char *); // show command help
int cmd_history(char *, MsgType, char *); // cli history utility

// Application Commands
int cmd_status (char *, MsgType, char *);   // Query all MODS devices and return the results
int cmd_cadence (char *, MsgType, char *);  // Query/Set the monitoring cadence in seconds
int cmd_pause   (char *, MsgType, char *);  // Pause monitoring
int cmd_resume  (char *, MsgType, char *);  // Resume monitoring
int cmd_logging (char *, MsgType, char *);  // Enable/Disable/Query the data logging state
int cmd_hdf_logging (char *, MsgType, char *); // Enable/Disable/Query the hdf data logging state
int cmd_config  (char *, MsgType, char *);  // Print the agent config info (engineering)
int cmd_comment (char *, MsgType, char *);  // Append a comment to the log
int cmd_device (char*, char*, MsgType, char*); // Commands or interacts with a device


// Application command/action structure

/*!
  \brief Client command action function data structure
*/

struct Commands {
  const char *cmd;  //!< command verb (e.g., read, quit, etc.)

  /*!
    \brief Action function for this command
    \param args command-line arguments (less the command verb)
    \param msgtype ISIS message-type code (see isismessage.h)
    \param reply string with the command return reply (info or error message)

    Command action functions take the form

    int cmd_xxx(char *args, MsgType msgtype, char *reply){}

    They return one of three integer return status values (defined in
    commands.h):

    \arg \c #CMD_OK command execution completed OK, \p reply contains the
    command output string to return to the caller.

    \arg \c #CMD_NOOP command execution OK, but nothing to return to the
    caller (\p reply is empty, command requires no reply service).

    \arg \c #CMD_ERR command execution resulted in an error, \p reply contains
    the error message to return to the caller.

    See the individual command action functions implemented in commands.c
    for details.
  */

  int(* action)(char *args, MsgType msgtype, char *reply);  

  const char *usage;        //!< command usage syntax, for the help facility and error messages
  const char *description;  //!< brief, 1-line command description for the help facility
}
cmdtab[] = {   //!< global scope command table for this application
  {"quit"    ,cmd_quit    ,"quit","Terminate the client session"},
  {"verbose" ,cmd_verbose ,"verbose","Toggle verbose output mode on/off"},
  {"debug"   ,cmd_debug   ,"debug","Toggle super-verbose debugging output mode on/off"},
  {"history" ,cmd_history ,"history","Show the command history (console only)"},
  {"info"    ,cmd_info    ,"info","Report client session runtime info"},
  {"version" ,cmd_version ,"version","Report the client version and compilation time"},
  {"status"  ,cmd_status  ,"status","Report the current status of all connected devices"},
  {"cadence" ,cmd_cadence ,"cadence","Set/Query the monitoring cadence in seconds"},
  {"pause"   ,cmd_pause   ,"pause","Pause monitoring (see RESUME)"},
  {"resume"  ,cmd_resume  ,"resume","Resume monitoring after a PAUSE"},
  {"log"     ,cmd_logging ,"log <enable|disable>","Enable/Disable enviromental sensor data logging"},
  {"logging" ,cmd_logging ,"logging <enable|disable>","Enable/Disable enviromental sensor data logging"},
  {"hdflog"  ,cmd_hdf_logging ,"hdflog <enable|disable>","Enable/Disable hdf formatted telemetry logging"},
  {"hdflogging" ,cmd_hdf_logging ,"hdflogging <enable|disable>","Enable/Disable hdf formatted telemetry logging"},
  {"comment" ,cmd_comment ,"comment <message>","Append a comment to the log"},
  {"config"  ,cmd_config  ,"config","Report agent configuration [engineering]"},
  {"help"    ,cmd_help    ,"help <cmd>","Help command (alias: ? <cmd>)"},
  {"?"       ,cmd_help    ,"",""},  // "" excludes from help
  {"ping"    ,cmd_ping    ,"",""},
  {"pong"    ,cmd_pong    ,"",""}   
};

// Number of commands defined (so we don't have to count correctly)
int NumCommands = sizeof(cmdtab)/sizeof(struct Commands);  //!< number of commands defined

// command function return codes
#define CMD_OK   0   //!< Command executed OK, return completion status
#define CMD_ERR -1   //!< Command execution resulted in an error
#define CMD_NOOP 1   //!< Command execution requires no further action (no-op)

#endif // COMMANDS_H
