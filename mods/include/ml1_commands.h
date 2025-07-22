#ifndef COMMANDS_H
#define COMMANDS_H

/*!
  \file commands.h
  \brief Client application command tree header file

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
int cmd_info   (char *, MsgType, char *); // return client application info
int cmd_version(char *, MsgType, char *); // return version info
int cmd_verbose(char *, MsgType, char *); // toggle verbose mode
int cmd_debug  (char *, MsgType, char *); // toggle debug (superverbose) mode
int cmd_help   (char *, MsgType, char *); // show command help
int cmd_history(char *, MsgType, char *); // cli history utility

// App-specific commands

int cmd_ports  (char *, MsgType, char *); // comm port info
int cmd_reset  (char *, MsgType, char *); // reset session
int cmd_open   (char *, MsgType, char *); // open the comm port
int cmd_close  (char *, MsgType, char *); // close the comm port
int cmd_setport(char *, MsgType, char *); // set/query comm port attributes
int cmd_loadml (char *, MsgType, char *); // load MicroLYNX source code
int cmd_c (char *, MsgType, char *); // send a command to motor(s)
int cmd_ml2 (char *, MsgType, char *); // MicroLynx controller 2
int cmd_ml3 (char *, MsgType, char *); // MicroLynx controller 3
int cmd_ml4 (char *, MsgType, char *); // MicroLynx controller 4
int cmd_s_pos (char *, MsgType, char *); // a science position to HW
int cmd_getpos (char *, MsgType, char *); // get motor position
int cmd_setpos (char *, MsgType, char *); // set motor position
int cmd_moverel (char *, MsgType, char *); // send motor to relative position
int cmd_moveabs (char *, MsgType, char *); // send motor to absolute position
int cmd_getparam (char *, MsgType, char *); // get device parameter
int cmd_setparam (char *, MsgType, char *); // set device parameter
int cmd_mstate (char *, MsgType, char *); // state of MicroLYNX params
int cmd_fwheel (char *, MsgType, char *); // connect the filter wheel to isl
int cmd_agw (char *, MsgType, char *); // connect the agw to isl
int cmd_sy (char *, MsgType, char *); // exec a shell command 
int cmd_pos_datum (char *, MsgType, char *); // Home target source code

// Application command/action structure

/*!
  \brief Client command action function data structure
*/

struct Commands {
  char *cmd;        //!< command verb (e.g., read, quit, etc.)

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

  char *usage;        //!< command usage syntax, for the help facility and error messages
  char *description;  //!< brief, 1-line command description for the help facility
}
cmdtab[] = {   //!< global scope command table for this application
  {"quit"    ,cmd_quit    ,"quit","Terminate the client session"},
  {"reset"   ,cmd_reset   ,"reset","Reload the runtime config file and re-initialize the comm port"},
  {"verbose" ,cmd_verbose ,"verbose","Toggle verbose output mode on/off"},
  {"debug"   ,cmd_debug   ,"debug","Toggle super-verbose debugging output mode on/off"},
  {"history" ,cmd_history ,"history","Show the command history"},
  {"info"    ,cmd_info    ,"info","Report application runtime info"},
  {"version" ,cmd_version ,"version","Report the application version and compilation time"},
  {"ports"   ,cmd_ports   ,"ports","List information about the active comm ports"},
  {"open"    ,cmd_open    ,"open <port>","Open a comm port, /dev/ttyX or host:port"},
  {"close"   ,cmd_close   ,"close","Close an open comm port"},
  {"setport" ,cmd_setport ,"setport <speed> <data> <stop> <parity>","Set/Query TTY port attributes"},
  {"loadml"  ,cmd_loadml ,"loadml","Load MicroLYNX source code"},
  {"c"  ,cmd_c ,"c","Send a command to motor(s);"},
  {"ml2"  ,cmd_ml2 ,"ml2","MicroLynx controller 2"},
  {"ml3"  ,cmd_ml3 ,"ml3","MicroLynx controller 3"},
  {"ml4"  ,cmd_ml4 ,"ml4","MicroLynx controller 4"},
  {"s_pos"  ,cmd_s_pos ,"s_pos","Science Position command;"},
  {"getpos"  ,cmd_getpos ,"getpos","Get motor position from microLYNX"},
  {"setpos"  ,cmd_setpos ,"setpos","Set motor position in microLYNX"},
  {"moverel"  ,cmd_moverel ,"moverel","Send motor relative position"},
  {"moveabs"  ,cmd_moveabs ,"moveabs","Send motor absolute position"},
  {"getparam",cmd_getparam ,"getparam","get MODS1 device parameter"},
  {"setparam",cmd_setparam ,"setparam","set MODS1 device parameter"},
  {"mstate"  ,cmd_mstate ,"mstate","status command from microLYNX."},
  {"fwheel"  ,cmd_fwheel ,"fwheel","Connect the filter wheel to ISL"},
  {"agw"  ,cmd_agw ,"agw","Connect the agw to ISL"},
  {"sy"  ,cmd_sy ,"sy","execute a shell command"},
  {"pos_datum"  ,cmd_pos_datum ,"pos_datum","Home the target"},
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
