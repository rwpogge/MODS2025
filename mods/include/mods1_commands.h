#ifndef MODS1_COMMANDS_H
#define MODS1_COMMANDS_H

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
int cmd_rinfo  (char *, MsgType, char *); // return client application info
int cmd_info   (char *, MsgType, char *); // return client application info
int cmd_version(char *, MsgType, char *); // return version info
int cmd_verbose(char *, MsgType, char *); // toggle verbose mode
int cmd_debug  (char *, MsgType, char *); // toggle debug (superverbose) mode
int cmd_help   (char *, MsgType, char *); // show command help
int cmd_history(char *, MsgType, char *); // cli history utility
int cmd_loadplc(char *, MsgType, char *); // load a MicroLynx controller
int cmd_proc   (char *, MsgType, char *); // load a procedure file

// App-specific commands

int cmd_status   (char *, MsgType, char *); // query AGW stage status
int cmd_mstatus   (char *, MsgType, char *); // query AGW stage status
int cmd_ports    (char *, MsgType, char *); // comm port info
int cmd_reset    (char *, MsgType, char *); // reset session
int cmd_restart  (char *, MsgType, char *); // restart services
int cmd_abort    (char *, MsgType, char *); // abort mechanism command
int cmd_open     (char *, MsgType, char *); // open the comm port
int cmd_close    (char *, MsgType, char *); // close the comm port
int cmd_setport  (char *, MsgType, char *); // set/query comm port attributes
int cmd_moverel  (char *, MsgType, char *); // move to relative position +/-
int cmd_moveabs  (char *, MsgType, char *); // move to absolute position
int cmd_hatch    (char *, MsgType, char *); // open/close hatch
int cmd_calib    (char *, MsgType, char *);  // MODS1 in/out Calibration Tower
int cmd_1calib   (char *, MsgType, char *); // MODS2 in/out Calibration Tower
int cmd_agw      (char *, MsgType, char *);       // move AGW X,Y,Focus
int cmd_1agw     (char *, MsgType, char *);       // move AGW X,Y,Focus
int cmd_agwx     (char *, MsgType, char *);      // move AGW X
int cmd_1agwx    (char *, MsgType, char *);      // move AGW X
int cmd_agwy     (char *, MsgType, char *);      // move AGW Y
int cmd_1agwy    (char *, MsgType, char *);      // move AGW Y
int cmd_agwfoc   (char *, MsgType, char *);    // move AGW Focus
int cmd_1agwfoc  (char *, MsgType, char *);    // move AGW Focus
int cmd_agwfilt  (char *, MsgType, char *);   // move AGW Filter Wheel
int cmd_1agwfilt (char *, MsgType, char *);   // move AGW Filter Wheel
int cmd_gprobe   (char *, MsgType, char *);    // AGW guide probe
int cmd_1gprobe  (char *, MsgType, char *);    // AGW guide probe
int cmd_gpoffset (char *, MsgType, char *);  // AGW guide probe offset
int cmd_slitmask (char *, MsgType, char *);  // MODS1 Slit Mask
int cmd_1slitmask(char *, MsgType, char *); // MODS2 Slit Mask
int cmd_mselect  (char *, MsgType, char *); //  Load Mask
int cmd_1mselect (char *, MsgType, char *); //  Load Mask
int cmd_minsert  (char *, MsgType, char *); //  Mask
int cmd_1minsert (char *, MsgType, char *); //  Mask
int cmd_readmask (char *, MsgType, char *); //  Mask
int cmd_dichroic (char *, MsgType, char *); //  Dichroic
int cmd_rcolttfa (char *, MsgType, char *); // Red Collimator tip
int cmd_bcolttfa (char *, MsgType, char *); // Blue Collimator tip
int cmd_rcolttfb (char *, MsgType, char *); // Red Collimator tilt
int cmd_bcolttfb (char *, MsgType, char *); // Blue Collimator tilt
int cmd_rcolttfc (char *, MsgType, char *); // Red Collimator tilt
int cmd_bcolttfc (char *, MsgType, char *); // Blue Collimator tilt
int cmd_rcolfoc  (char *, MsgType, char *); // Red Collimator focus
int cmd_bcolfoc  (char *, MsgType, char *); // Blue Collimator focus
int cmd_rcolstep (char *, MsgType, char *); // Red Collimator Increment
int cmd_bcolstep (char *, MsgType, char *); // Blue Collimator Increment
int cmd_rgrating (char *, MsgType, char *); // Red Grating select
int cmd_bgrating (char *, MsgType, char *); // Blue Grating select
int cmd_rgrtilt1 (char *, MsgType, char *); // Red Grating tilt
int cmd_bgrtilt1 (char *, MsgType, char *); // Blue Grating tilt
int cmd_rgrtilt2 (char *, MsgType, char *); // Red Grating tilt
int cmd_bgrtilt2 (char *, MsgType, char *); // Blue Grating tilt
int cmd_rgrstep1 (char *, MsgType, char *); // Red Grating Increment
int cmd_bgrstep1 (char *, MsgType, char *); // Blue Grating Increment
int cmd_rgrstep2 (char *, MsgType, char *); // Red Grating Increment
int cmd_bgrstep2 (char *, MsgType, char *); // Blue Grating Increment
int cmd_rgrstep3 (char *, MsgType, char *); // Red Grating Increment
int cmd_bgrstep3 (char *, MsgType, char *); // Blue Grating Increment
int cmd_rgrstep4 (char *, MsgType, char *); // Red Grating Increment
int cmd_bgrstep4 (char *, MsgType, char *); // Blue Grating Increment
int cmd_rshutter (char *, MsgType, char *); // Red camera shutter
int cmd_bshutter (char *, MsgType, char *); // Blue camera shutter
int cmd_rfilter  (char *, MsgType, char *); // Red camera shutter
int cmd_bfilter  (char *, MsgType, char *); // Blue camera shutter
int cmd_rcamfoc  (char *, MsgType, char *); // Red camera focus
int cmd_bcamfoc  (char *, MsgType, char *); // Blue camera focus
int cmd_rcamstep (char *, MsgType, char *); // Red camera increment focus
int cmd_bcamstep (char *, MsgType, char *); // Blue camera increment focus
int cmd_lamp     (char *, MsgType, char *); // Calibration Lamps.
int cmd_irlaser  (char *, MsgType, char *); // Calibration IR laser
int cmd_vislaser (char *, MsgType, char *); // Calibration Visable laser
int cmd_ieb      (char *, MsgType, char *); // IEB power Interface
int cmd_iebstatus(char *, MsgType, char *); // IEB power Interface
int cmd_iebpower (char *, MsgType, char *); // IEB power Interface
int cmd_iebtemp  (char *, MsgType, char *); // IEB power Interface
int cmd_iebmlc   (char *, MsgType, char *); // IEB power Interface
int cmd_mmcu     (char *, MsgType, char *); // Blue camera increment focus
int cmd_sy       (char *, MsgType, char *); // exec a shell command 

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
  {"restart"   ,cmd_restart   ,"restart","Restart MMC and AGW Services"},
  {"reset"   ,cmd_reset   ,"reset","Reload the runtime config file and re-initialize the comm port"},
  {"abort"   ,cmd_mmcu   ,"abort [name|all]","Abort named or all mechanisms"},
  {"verbose" ,cmd_verbose ,"verbose","Toggle verbose output mode on/off"},
  {"debug"   ,cmd_debug   ,"debug","Toggle super-verbose debugging output mode on/off"},
  {"history" ,cmd_history ,"history","Show the command history"},
  {"reminfo"    ,cmd_mmcu    ,"reminfo","Report application runtime info for Remote Services"},
  {"info"    ,cmd_info    ,"info","Report application runtime mmcService info"},
  {"version" ,cmd_version ,"version","Report the application version and compilation time"},
  {"ports"   ,cmd_mmcu   ,"ports","List information about the active comm ports"},
  {"status"   ,cmd_status,"status","Instrument Electronics Box(IEB) Status"},
  {"mstatus"  ,cmd_mstatus,"mstatus","Mechanism HW Status"},
  {"open",cmd_mmcu,"open [port]","Open a comm port, /dev/ttyX or host:port"},
  {"close"   ,cmd_mmcu   ,"close [port]","Close an open comm port"},
  //  {"setport" ,cmd_setport ,"setport <speed> <data> <stop> <parity>","Set/Query TTY port attributes"},
  {"setport" ,cmd_setport ,"setport [ID]","Set IP port on mechanism (0-31)"},
  {"moverel", cmd_mmcu, "moverel [mechanism +/-value]","send mechanism to relative position"},
  {"moveabs", cmd_mmcu, "moveabs [mechanism value]","send mechanism to absolute position"},
  {"hatch", cmd_mmcu, "hatch [open|close]", "Set/Query hatch"},
  {"calib", cmd_mmcu, "calib [in|out]", "MODS1 Set/Query Calibration Tower"},
  {"1calib", cmd_mmcu, "calib [in|out]", "MODS2 Set/Query Calibration Tower"},
  {"agw", cmd_mmcu, "agw [xpos ypos fpos|home|init]", "Set/Query AGW X,Y,Focus position"},
  {"1agw", cmd_mmcu, "agw [xpos ypos fpos|home|init]", "Set/Query AGW X,Y,Focus position"},
  {"1agwx", cmd_mmcu, "agwx [xpos|home]", "Set/Query AGW X position"},
  {"agwx", cmd_mmcu, "agwx [xpos|home]", "Set/Query AGW X position"},
  {"agwy", cmd_mmcu, "agwy [ypos|home]", "Set/Query AGW Y position"},
  {"1agwy", cmd_mmcu, "agwy [ypos|home]", "Set/Query AGW Y position"},
  {"agwfilt", cmd_mmcu, "agwfilt [n|ID]", "Set/Query AGW filter"},
  {"1agwfilt", cmd_mmcu, "agwfilt [n|ID]", "Set/Query AGW filter"},
  {"agwfoc", cmd_mmcu, "agwfoc [fpos|home]", "Set/Query AGW focus"},
  {"1agwfoc", cmd_mmcu, "agwfoc [fpos|home]", "Set/Query AGW focus"},
  {"gprobe", cmd_mmcu, "gprobe [Xgp Ygp|home]", "Set/Query AGW Guide Probe"},
  {"1gprobe", cmd_mmcu, "gprobe [Xgp Ygp|home]", "Set/Query AGW Guide Probe"},
  {"gpoffset", cmd_mmcu, "gpoffset [dXgp dYgp|home]", "Offset/Query AGW Guide Probe"},
  {"slitmask", cmd_mmcu, "slitmask [n|ID] [in|out]", "MODS1 Select/Query mask in the slit mask cassette and deploy"},
  {"1slitmask", cmd_mmcu, "1slitmask [n|ID] [in|out]", "MODS2 Select/Query mask in the slit mask cassette and deploy"},
  {"mselect", cmd_mmcu, "mselect [n|brace]", "Select/Query mask in the slit mask cassette for loading"},
  {"1mselect", cmd_mmcu, "mselect [n|brace]", "Select/Query mask in the slit mask cassette for loading"},
  {"minsert", cmd_mmcu, "minsert [in|out]", "Insert/Retract/Scan/Query a slit mask into the focal plane"},
  {"1minsert", cmd_mmcu, "minsert [in|out]", "Insert/Retract/Scan/Query a slit mask into the focal plane"},
  {"readmask", cmd_mmcu, "readmask", "Read/Query the barcode label on the slit mask"},
  {"dichroic", cmd_mmcu, "dichroic [n|blue|red|both]", "Select/Query the dichroic beam selector position"},
  {"rcolttfa", cmd_mmcu, "rcolttfa [pos|home]", "Tip/Query Red Collimator mirror actuator"},
  {"bcolttfa", cmd_mmcu, "bcolttfa [pos|home]", "Tip/Query Blue Collimator mirror actuator"},
  {"rcolttfb", cmd_mmcu, "rcolttfc [pos|home]", "Tilt/Query Red Collimator mirror actuator"},
  {"bcolttfb", cmd_mmcu, "bcolttfc [pos|home]", "Tilt/Query Blue collimator mirror actuator"},
  {"rcolttfc", cmd_mmcu, "rcolttfc [pos|home]", "Tilt/Query Red Collimator mirror actuator"},
  {"bcolttfc", cmd_mmcu, "bcolttfc [pos|home]", "Tilt/Query Blue collimator mirror actuator"},
  {"rcolfoc", cmd_mmcu, "rcolfoc [pos|home]", "Focus/Query Red Collimator mirror to absolute position"},
  {"bcolfoc", cmd_mmcu, "bcolfoc [pos|home]", "Focus/Query Blue Collimator mirror to absolute position"},
  {"rcolstep", cmd_mmcu, "rcolstep [dfoc|df1 df2 df3]", "Increment(step)/Query Red Collimator mirror to focus position"},
  {"bcolstep", cmd_mmcu, "bcolstep [dfoc|df1 df2 df3]", "Set/Query Blue Collimator mirror focus position"},

  {"rgrating", cmd_mmcu, "rgrating [n|ID]", "Select/Query grating in the Red grating turret"},
  {"bgrating", cmd_mmcu, "bgrating [n|ID]", "Select/Query grating in the Blue grating turret"},

  {"rgrtilt1", cmd_mmcu, "rgrtilt1 [tilt|home]", "Tilt/Query Red grating 1 to a given absolute tilt"},
  {"bgrtilt1", cmd_mmcu, "bgrtilt1 [tilt|home]", "Tilt/Query Blue grating 1 to a given absolute tilt"},
  {"rgrtilt2", cmd_mmcu, "rgrtilt2 [tilt|home]", "Tilt/Query Red grating 2 to a given absolute tilt"},
  {"bgrtilt2", cmd_mmcu, "bgrtilt2 [tilt|home]", "Tilt/Query Blue grating 2 to a given absolute tilt"},
  {"rgrtilt3", cmd_mmcu, "rgrtilt3 [tilt|home]", "Tilt/Query Red grating 3 to a given absolute tilt"},
  {"bgrtilt3", cmd_mmcu, "bgrtilt3 [tilt|home]", "Tilt/Query Blue grating 3 to a given absolute tilt"},
  {"rgrtilt4", cmd_mmcu, "rgrtilt4 [tilt|home]", "Tilt/Query Red grating 4 to a given absolute tilt"},
  {"bgrtilt4", cmd_mmcu, "bgrtilt4 [tilt|home]", "Tilt/Query Blue grating 4 to a given absolute tilt"},
  {"rgrstep1", cmd_mmcu, "rgrstep1 [dt]", "Increment/Query tilt of Red grating"},
  {"bgrstep1", cmd_mmcu, "bgrstep1 [dt]", "Increment/Query tilt of Blue grating"},
  {"rgrstep2", cmd_mmcu, "rgrstep2 [dt]", "Increment/Query tilt of Red grating"},
  {"bgrstep2", cmd_mmcu, "bgrstep2 [dt]", "Increment/Query tilt of Blue grating"},
  {"rgrstep3", cmd_mmcu, "rgrstep3 [dt]", "Increment/Query tilt of Red grating"},
  {"bgrstep3", cmd_mmcu, "bgrstep3 [dt]", "Increment/Query tilt of Blue grating"},
  {"rgrstep4", cmd_mmcu, "rgrstep4 [dt]", "Increment/Query tilt of Red grating"},
  {"bgrstep4", cmd_mmcu, "bgrstep4 [dt]", "Increment/Query tilt of Blue grating"},
  {"rshutter", cmd_mmcu, "rshutter [open|close]", "Open/Close/Query Red camera shtter"},
  {"bshutter", cmd_mmcu, "bshutter [open|close]", "Open/Close/Query Blue camera shtter"},
  {"rfilter", cmd_mmcu, "rfilter [n|ID] [Load]", "Set/Query/Load Red camera Filter Wheel"},
  {"bfilter", cmd_mmcu, "bfilter [n|ID] [Load]", "Set/Query/Load Blue camera Filter Wheel"},
  {"rcamfoc", cmd_mmcu, "rcamfoc [foc|home]", "Focus/Query Red camera primary mirror"},
  {"bcamfoc", cmd_mmcu, "bcamfoc [foc|home]", "Focus/Query Blue camera primary mirror"},
  {"rcamstep", cmd_mmcu, "rcamstep [dfoc]", "Increment(step)/Query Red camera primary mirror"},
  {"bcamstep", cmd_mmcu, "bcamstep [dfoc]", "Increment(step)/Query Blue camera primary mirror"},
  {"lamp", cmd_mmcu, "lamp [lamp1 lamp2...] [on|off|info]", "Operate the Calibration Lamps"},
  {"irlaser", cmd_mmcu, "irlaser [pon|poff] [enable|disable] [power [lev]] [temp [Tset]] [status|info]", "Operate IMCS Infrared Reference Laser"},
  {"vislaser", cmd_mmcu, "vislaser [pon|poff] [enable|disable] [power [lev]] [status|info]", "Operate Visible-light Alignment Laser"},
  {"ieb", cmd_mmcu, "ieb [n] [power|temp|status|{mlc [m]}] [on|off]", "IEB interface"},
  {"iebstatus", cmd_mmcu, "iebstatus [n]", "IEB power interface"},
  {"iebpower", cmd_mmcu, "iebpower [n] [on|off]", "IEB power interface"},
  {"iebtemp", cmd_mmcu, "iebtemp [n]", "IEB power interface"},
  {"iebmlc", cmd_mmcu, "iebmlc# [n] [on|off|reset|status]", "IEB power interface"},
  {"mmcu", cmd_mmcu, "mmcu", "microLynx controller executive command"},
  {"loadplc", cmd_mmcu, "loadplc [mechanism file]", "Load microLynx Controller with PLC code"},
  {"proc", cmd_proc, "proc [procedure file]", "Load file without .prc extension"},
  {"sy", cmd_sy, "sy [function]", "execute a shell command"},
  {"help", cmd_help, "help [cmd]", "Help command (alias: ? [cmd])"},
  {"?", cmd_help, "", ""},  // "" excludes from help
  {"ping", cmd_ping, "", ""},
  {"pong", cmd_pong, "", ""}   
};

// Number of commands defined (so we don't have to count correctly)
  
int NumCommands = sizeof(cmdtab)/sizeof(struct Commands);  //!< number of commands defined

// command function return codes

#define CMD_OK   0   //!< Command executed OK, return completion status
#define CMD_ERR -1   //!< Command execution resulted in an error
#define CMD_NOOP 1   //!< Command execution requires no further action (no-op)
#define CMD_FERR 2   //!< Command execution resulted in a fatal error
#define CMD_WARN 3   //!< Command execution resulted in a warning

#endif // MODS1_COMMANDS_H
