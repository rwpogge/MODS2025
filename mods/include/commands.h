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

// Filter Wheel commands

int cmd_ports  (char *, MsgType, char *); // show current mechanism comm ports
int cmd_rdpos  (char *, MsgType, char *); // read position encoders
int cmd_setcur (char *, MsgType, char *); // set motor current (amps)
int cmd_setsp  (char *, MsgType, char *); // set move speed (rev/s)
int cmd_setacc (char *, MsgType, char *); // set move acceleration (rev/s/s)
int cmd_setdec (char *, MsgType, char *); // set move deceleration (rev/s/s)
int cmd_setstep(char *, MsgType, char *); // set steps/position
int cmd_setto  (char *, MsgType, char *); // set timeout interval (sec)
int cmd_mstatus(char *, MsgType, char *); // print microstep drive status
int cmd_idle   (char *, MsgType, char *); // disable motor (idle to 0 amps)
int cmd_send   (char *, MsgType, char *); // send a raw command 
int cmd_query  (char *, MsgType, char *); // send a raw query command
int cmd_reset  (char *, MsgType, char *); // reset runtime & controller params
int cmd_init   (char *, MsgType, char *); // initialize the microstep drive
int cmd_abort  (char *, MsgType, char *); // abort/stop microstep drive
int cmd_fpars  (char *, MsgType, char *); // report filter parameters

// High-level "user" commands

int cmd_status (char *, MsgType, char *); // client STATUS command
int cmd_filter (char *, MsgType, char *); // user FILTER command
int cmd_load   (char *, MsgType, char *); // user LOAD command
int cmd_findpos(char *, MsgType, char *); // seek valid filter position

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
}
cmdtab[] = {   //!< global scope command table for this application
  {"quit"    ,cmd_quit    },
  {"reset"   ,cmd_reset   },
  {"filter"  ,cmd_filter  },
  {"findpos" ,cmd_findpos },
  {"load"    ,cmd_load    },
  {"status"  ,cmd_status  },
  {"init"    ,cmd_init    },  
  {"restore" ,cmd_init    },  // restore is an alias for init
  {"abort"   ,cmd_abort   },
  {"stop"    ,cmd_abort   },
  {"rdpos"   ,cmd_rdpos   },
  {"steps"   ,cmd_setstep },
  {"speed"   ,cmd_setsp   },
  {"current" ,cmd_setcur  },
  {"accel"   ,cmd_setacc  },
  {"decel"   ,cmd_setdec  },
  {"timeout" ,cmd_setto   },
  {"mstatus" ,cmd_mstatus },
  {"fpars"   ,cmd_fpars   },
  {"idle"    ,cmd_idle    },
  {"ports"   ,cmd_ports   },
  {"send"    ,cmd_send    },
  {"query"   ,cmd_query   },
  {"info"    ,cmd_info    },
  {"verbose" ,cmd_verbose },
  {"version" ,cmd_version },
  {"debug"   ,cmd_debug   },
  {"help"    ,cmd_help    },
  {"?"       ,cmd_help    },  // ? is an alias for help
  {"history" ,cmd_history },
  {"ping"    ,cmd_ping    },
  {"pong"    ,cmd_pong    }
};

// Number of commands defined (so we don't have to count correctly)
  
int NumCommands = sizeof(cmdtab)/sizeof(struct Commands);  //!< number of commands defined

// command function return codes

#define CMD_OK   0   //!< Command executed OK, return completion status
#define CMD_ERR -1   //!< Command execution resulted in an error
#define CMD_NOOP 1   //!< Command execution requires no further action (no-op)
#define CMD_FERR 2   //!< Command execution resulted in an error
#define CMD_WARN 3   //!< Command execution resulted in an error

#endif // COMMANDS_H
