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
int cmd_ping   (char *, MsgType, char *); // ping (comm handshake request)
int cmd_pong   (char *, MsgType, char *); // pong (comm handshake acknowledge)
int cmd_quit   (char *, MsgType, char *); // Quit
int cmd_noop   (char *, MsgType, char *); // NOOP 
int cmd_info   (char *, MsgType, char *); // Information
int cmd_help   (char *, MsgType, char *); // help
int cmd_close  (char *, MsgType, char *); // close port
int cmd_open   (char *, MsgType, char *); // open the comm port
int cmd_version(char *, MsgType, char *); // Version
int cmd_mmcIC  (char *, MsgType, char *); // Interrogator

// App-specific commands

// int cmd_hosts  (char *, MsgType, char *);  // remote host info

// AGW Stage commands

int cmd_status  (char *, MsgType, char *); // query AGW stage status
int cmd_home    (char *, MsgType, char *); // home the AGW stage
int cmd_center  (char *, MsgType, char *); // Send the AGW stage to the science field center
int cmd_getpos  (char *, MsgType, char *); // query position of a single axis
int cmd_setpos  (char *, MsgType, char *); // set position of a single axis
int cmd_xy      (char *, MsgType, char *); // set/query absolute X,Y stage position
int cmd_filter  (char *, MsgType, char *); // set/query acquisition/guide camera filter
int cmd_focus   (char *, MsgType, char *); // set/query absolute AGW focus
int cmd_foffset (char *, MsgType, char *); // set/query AGW focus offset
int cmd_agw     (char *, MsgType, char *); // AGW stage
int cmd_agwx    (char *, MsgType, char *); // AGW X stage
int cmd_agwy    (char *, MsgType, char *); // AGW Y stage
int cmd_agwfoc  (char *, MsgType, char *); // AGW FOCUS stage
int cmd_agwfilt (char *, MsgType, char *); // AGW FW stage
int cmd_init    (char *, MsgType, char *); // home the AGW stage
int cmd_inity   (char *, MsgType, char *); // home the AGW stage
int cmd_initx   (char *, MsgType, char *); // home the AGW stage
int cmd_initfoc (char *, MsgType, char *); // home the AGW stage
int cmd_initfilt(char *, MsgType, char *); // home the AGW stage
int cmd_calib   (char *, MsgType, char *); // set/query the calibration tower position
int cmd_lock    (char *, MsgType, char *); // assert a command lock on the client
int cmd_unlock  (char *, MsgType, char *); // release the command lock on the client
int cmd_driftvec(char *, MsgType, char *); // set/query stage drift vector
int cmd_drift   (char *, MsgType, char *); // toggle stage drift on/off
int cmd_autofoc (char *, MsgType, char *); // set/query stage autofocus
int cmd_reset   (char *, MsgType, char *); // reset the AGW (warm restart)
int cmd_abort   (char *, MsgType, char *); // abort all AGW stage motions
int cmd_ccdAgwSfp(char *, MsgType, char *); // AGW,CCD,SFP header function
int cmd_initTrans(char *, MsgType, char *); // inititialize from Config file
int cmd_sfpToAgw (char *, MsgType, char *); // SFP to GP 
int cmd_agwToSfp (char *, MsgType, char *); // AGW to SFP
int cmd_ccdToAgw (char *, MsgType, char *); // CCD to GP
int cmd_ccdToSfp (char *, MsgType, char *); // CCD to SFP

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
  {"quit"       ,cmd_quit   ,"quit","quit "},
  {"info"       ,cmd_info   ,"info","Information on the connection"},
  {"help"       ,cmd_help   ,"help [what]","Help"},
  {"close"      ,cmd_close  ,"close","Close an open comm port"},
  {"open"       ,cmd_open   ,"open <port>","Open a comm port, /dev/ttyX or host:port"},
  {"noop"       ,cmd_noop   ,"noop","NOOP"},
  {"version"    ,cmd_version   ,"version","Compile and Software Version"},
  {"mmcIC"      ,cmd_mmcIC   ,"mmcIC [who command]","Interrogator"},
  {"home"       ,cmd_home    ,"home","Send the AGW stage to its home (stowed) device position."},
  {"center"     ,cmd_center  ,"center","Send the AGW stage to the science field center."},
  {"getdata"    ,cmd_status  ,"getdata","Return all current AGW stage data (full device status)"},
  {"getxy"      ,cmd_xy      ,"getxy","Return the current AG camera field-center position (microns from science field center)"},
  {"getfocus"   ,cmd_focus   ,"getfocus","Return the current AGW camera focus value (microns from home)"},
  {"getfilter"  ,cmd_filter  ,"getfilter","Return the current AG camera filter position"},
  {"getposition",cmd_getpos  ,"getposition [x|y|focus|filter]","Return the specified raw device axis position (microns from home)"},
  {"setxy"      ,cmd_xy      ,"setxy x y","Set the AG camera field center X,Y position (microns from science field center)"},
  {"setfocus"   ,cmd_focus   ,"setfocus f","Set the AGW camera focus position (microns from home)"},
  {"focoffset"  ,cmd_foffset ,"focoffset df","Set the AGW camera focus offset (microns relative to nominal zero focus)"},
  {"setfilter"  ,cmd_filter  ,"setfilter n","Set the AG camera filter (range: 1..4)"},
  {"setposition",cmd_setpos  ,"setposition [x|y|focus|filter] val","Set the specified raw device axis position (microns from home)"},
  {"agw"        ,cmd_agw ,"agw [xpos ypos fpos]","Query/Set AGW stage device position."},
  {"agwx"       ,cmd_agw ,"agwx [xpos]","Query/Set AGW X axis"},
  {"agwy"       ,cmd_agw ,"initx","Query/Set AGW Y axis"},
  {"agwfoc"     ,cmd_agw ,"initfoc","Query/Set AGW Focus axis"},
  {"agwfilt"    ,cmd_agw ,"initfilt","Query/Set AGW Filter Wheel"},
  {"init"       ,cmd_init ,"init","initialize AGW stage (home then center) device position."},
  {"inity"       ,cmd_init ,"inity","initialize AGW Y axis"},
  {"initx"       ,cmd_init ,"initx","initialize AGW X axis"},
  {"initfoc"     ,cmd_init ,"initfoc","initialize AGW Focus axis"},
  {"initfilt"    ,cmd_init ,"initfilt","initialize AGW Filter Wheel"},
  {"calib"      ,cmd_calib ,"calib [in|out]","Set/Query the calibration tower position (in/out of beam)"},
  {"lock"       ,cmd_lock    ,"lock","Request the command lock for the AGW stage, unlock to release"},
  {"unlock"     ,cmd_unlock  ,"unlock","Release the command lock on the AGW stage"},
  {"driftvec"   ,cmd_driftvec,"driftvec xdot ydot","Set/Query the AGW stage drift vector in micron/sec"},
  {"drift"      ,cmd_drift   ,"drift [start|stop]","Start/Stop/Query the drift motion"},
  {"autofocus"  ,cmd_autofoc ,"autofocus [on|off]","Set/Query the autofocus state (on/off)"},
  {"status"     ,cmd_status  ,"status","Return the current MODS AGW stage status (alias for getdata)"},
  {"abort"     ,cmd_abort  ,"abort","Stop all the current MODS AGW stage motion"},
  {"ccdAgwSfp" ,cmd_ccdAgwSfp  ,"ccdAgwSfp","Work with transformation coefficients"},
  {"initTrans" ,cmd_ccdAgwSfp  ,"initTrans","Initialize transforms"},
  {"sfpToAgw" ,cmd_ccdAgwSfp  ,"sfpToAgw","Work with transformation coefficients"},
  {"agwToSfp" ,cmd_ccdAgwSfp  ,"agwToSfp","Work with transformation coefficients"},
  {"ccdToAgw" ,cmd_ccdAgwSfp  ,"ccdToAgw","Work with transformation coefficients"},
  {"ccdToSfp" ,cmd_ccdAgwSfp  ,"ccdToSfp","Work with transformation coefficients"},
  {"ping"    ,cmd_ping    ,"",""},
  {"pong"    ,cmd_pong    ,"",""},
  {"reset"     ,cmd_reset  ,"reset","Reset/initialize MODS AGW stage"}
};

// Number of commands defined (so we don't have to count correctly)
  
int NumCommands = sizeof(cmdtab)/sizeof(struct Commands);  //!< number of commands defined

// command function return codes

#define CMD_OK   0   //!< Command executed OK, return completion status
#define CMD_ERR -1   //!< Command execution resulted in an error
#define CMD_NOOP 1   //!< Command execution requires no further action (no-op)

#endif // COMMANDS_H

