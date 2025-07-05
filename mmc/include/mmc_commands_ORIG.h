#ifndef MMC_COMMANDS_H
#define MMC_COMMANDS_H

/*!
  \file mmc_commands.h
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
int cmd_quit   (char *, MsgType, char *); // terminate MicroLynx Services
int cmd_info   (char *, MsgType, char *); // return client application info
int cmd_version(char *, MsgType, char *); // return version info
int cmd_verbose(char *, MsgType, char *); // toggle verbose mode
int cmd_debug  (char *, MsgType, char *); // toggle debug (superverbose) mode
int cmd_help   (char *, MsgType, char *); // show command help
int cmd_status (char *, MsgType, char *); // MMC Service Status
int cmd_mstatus(char *, MsgType, char *); // Mechanism Status
int cmd_istatus(char *, MsgType, char *); // Instrument General Status
int cmd_pstatus(char *, MsgType, char *); // Instrument Power Status
int cmd_estatus(char *, MsgType, char *); // MODS Temps and Pressures Status
int cmd_loadplc(char *, MsgType, char *); // Load a MicroLynx controller
int cmd_util    (char *, MsgType, char *); // Lamps and Lasers Box controller
int cmd_llb    (char *, MsgType, char *); // Lamps and Lasers Box controller
int cmd_startup(char *, MsgType, char *); // General MicroLynx controller
int cmd_stow   (char *, MsgType, char *); // General MicroLynx controller
int cmd_shutdown(char *, MsgType, char *); // General MicroLynx controller
int cmd_sleep  (char *, MsgType, char *); // General MicroLynx controller
int cmd_wake   (char *, MsgType, char *); // General MicroLynx controller
int cmd_saveconfig(char *, MsgType, char *); // General MicroLynx controller
int cmd_panic  (char *, MsgType, char *); // General MicroLynx controller
int cmd_abort  (char *, MsgType, char *); // General MicroLynx controller

// App-specific commands

// MODS Mechanism Controller Service(s)
int cmd_hatch    (char *, MsgType, char *);
int cmd_calib    (char *, MsgType, char *);
int cmd_agw      (char *, MsgType, char *);
int cmd_agwy     (char *, MsgType, char *);
int cmd_agwx     (char *, MsgType, char *);
int cmd_agwfoc   (char *, MsgType, char *);
int cmd_agwfilt  (char *, MsgType, char *);
int cmd_gprobe   (char *, MsgType, char *);
int cmd_gpfocus  (char *, MsgType, char *);
int cmd_gpoffset (char *, MsgType, char *);
int cmd_minsert  (char *, MsgType, char *); // initialize/stow a mask 
int cmd_slitmask (char *, MsgType, char *); // select a mask 
int cmd_mselect  (char *, MsgType, char *); // slit mask select with slit mask
// int cmd_readmask (char *, MsgType, char *); // read barcode on mask - retired
int cmd_dichroic (char *, MsgType, char *); // dichroic turret
int cmd_colttf   (char *, MsgType, char *); // Red Collimator tip
int cmd_rcolttfa (char *, MsgType, char *); // Red Collimator tip
int cmd_bcolttfa (char *, MsgType, char *); // Blue Collimator tip
int cmd_rcolttfb (char *, MsgType, char *); // Red Collimator tilt
int cmd_bcolttfb (char *, MsgType, char *); // Blue Collimator tilt
int cmd_rcolttfc (char *, MsgType, char *); // Red Collimator tilt
int cmd_bcolttfc (char *, MsgType, char *); // Blue Collimator tilt
int cmd_colfoc   (char *, MsgType, char *); // Red Collimator focus
int cmd_rcolfoc  (char *, MsgType, char *); // Red Collimator focus
int cmd_bcolfoc  (char *, MsgType, char *); // Blue Collimator focus
int cmd_colstep  (char *, MsgType, char *); // Red Collimator Increment
int cmd_rcolstep (char *, MsgType, char *); // Red Collimator Increment
int cmd_bcolstep (char *, MsgType, char *); // Blue Collimator Increment
int cmd_grating  (char *, MsgType, char *); // Red Grating select
int cmd_rgrating (char *, MsgType, char *); // Red Grating select
int cmd_bgrating (char *, MsgType, char *); // Blue Grating select
int cmd_grtilt   (char *, MsgType, char *); // Red Grating tilt
int cmd_rgrtilt1 (char *, MsgType, char *); // Red Grating tilt
int cmd_bgrtilt1 (char *, MsgType, char *); // Blue Grating tilt
int cmd_rgrtilt2 (char *, MsgType, char *); // Red Grating tilt
int cmd_bgrtilt2 (char *, MsgType, char *); // Blue Grating tilt
int cmd_rgrtilt3 (char *, MsgType, char *); // Red Grating tilt
int cmd_bgrtilt3 (char *, MsgType, char *); // Blue Grating tilt
int cmd_rgrtilt4 (char *, MsgType, char *); // Red Grating tilt
int cmd_bgrtilt4 (char *, MsgType, char *); // Blue Grating tilt
int cmd_grstep   (char *, MsgType, char *); // Red Grating Increment
int cmd_rgrstep1 (char *, MsgType, char *); // Red Grating Increment
int cmd_bgrstep1 (char *, MsgType, char *); // Blue Grating Increment
int cmd_rgrstep2 (char *, MsgType, char *); // Red Grating Increment
int cmd_bgrstep2 (char *, MsgType, char *); // Blue Grating Increment
int cmd_rgrstep3 (char *, MsgType, char *); // Red Grating Increment
int cmd_bgrstep3 (char *, MsgType, char *); // Blue Grating Increment
int cmd_rgrstep4 (char *, MsgType, char *); // Red Grating Increment
int cmd_bgrstep4 (char *, MsgType, char *); // Blue Grating Increment
int cmd_shutter  (char *, MsgType, char *); // shutter
int cmd_rshutter (char *, MsgType, char *); // Red camera shutter
int cmd_bshutter (char *, MsgType, char *); // Blue camera shutter
int cmd_filter   (char *, MsgType, char *); // Filter Wheel
int cmd_rfilter  (char *, MsgType, char *); // Red Filter Wheel
int cmd_bfilter  (char *, MsgType, char *); // Blue Filter Wheel
int cmd_camfoc   (char *, MsgType, char *); // Red camera focus
int cmd_rcamfoc  (char *, MsgType, char *); // Red camera focus
int cmd_bcamfoc  (char *, MsgType, char *); // Blue camera focus
int cmd_camstep  (char *, MsgType, char *); // Red camera increment focus
int cmd_rcamstep (char *, MsgType, char *); // Red camera increment focus
int cmd_bcamstep (char *, MsgType, char *); // Blue camera increment focus
int cmd_lamp     (char *, MsgType, char *); // Calibration Lamps
int cmd_irlaser  (char *, MsgType, char *); // IR laser
int cmd_vislaser (char *, MsgType, char *); // VISABLE laser
int cmd_ieb      (char *, MsgType, char *); // IEB command
int cmd_close    (char *, MsgType, char *); // close port
int cmd_open     (char *, MsgType, char *); // open the comm port
int cmd_ports    (char *, MsgType, char *); // comm port info
int cmd_proc     (char *, MsgType, char *); // procedure source code
int cmd_reset    (char *, MsgType, char *); // reset session
int cmd_setport  (char *, MsgType, char *); // set/query comm port attributes
int cmd_sy       (char *, MsgType, char *); // exec a shell command 
int cmd_moverel  (char *, MsgType, char *); // relative move
int cmd_moveabs  (char *, MsgType, char *); // absolute move
int cmd_imcs     (char *, MsgType, char *); // absolute move
int cmd_bimcs    (char *, MsgType, char *); // absolute move
int cmd_rimcs    (char *, MsgType, char *); // absolute move

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

    \arg \c #CMD_FERR command execution resulted in a fatal error, \p reply contains
    the error message to return to the caller.

    \arg \c #CMD_WARN command execution resulted in a warning, \p reply contains
    the error message to return to the caller.

    See the individual command action functions implemented in commands.c
    for details.
  */

  int(* action)(char *args, MsgType msgtype, char *reply);  

  char *usage;        //!< command usage syntax, for the help facility and error messages
  char *description;  //!< brief, 1-line command description for the help facility
}
cmdtab[] = {   //!< global scope command table for this application
  {"hatch",    cmd_hatch,    "hatch [open|close]","Command the darkslide"},
  {"calib",    cmd_calib,    "calib [out|in]","Command the Calibration Tower"},
  {"agw",      cmd_agw,      "agw [xpos ypos fpos|home]","Command the AGW X,Y,Focus"},
  {"agwy",     cmd_agwy,     "agwy [ypos|home]","Command the AGW Y stage"},
  {"agwx",     cmd_agwx,     "agwx [xpos|home]","Command the AGW X stage"},
  {"agwfoc",   cmd_agwfoc,   "agwfoc [fpos|home]","Command the AGW Focus"},
  {"agwfilt",  cmd_agwfilt,  "agwfilt [n|ID]","Command the AGW Filter Wheel"},
  {"gprobe",   cmd_gprobe,   "gprobe [Xgp Ygp]","Command the AGW Guide Probe"},
  {"gpfocus",  cmd_gpfocus,  "gpfocus [focus]","Offset the AGW Guide Probe"},
  {"gpoffset", cmd_gpoffset, "gpoffset [dXs dYs]","Offset the AGW Guide Probe"},
  {"minsert",  cmd_minsert,  "minsert [in|stow]","Query/Set Mask in,out or scan"},
  {"slitmask", cmd_slitmask, "slitmask [n|brace]","Query/Set Mask in load position"},
  {"mselect",  cmd_mselect,  "mselect [n|brace]","Query/Set select a mask"},
  //  {"readmask", cmd_readmask, "readmask","Read barcode ID on slit mask"},
  {"dichroic", cmd_dichroic, "dichroic [n|blue|red|both]", "Select/Query the dichroic beam selector position"},
  {"rcolttfa", cmd_colttf,   "rcolttfa [aPos|step +/-aPos]", "Tip/Query Red TTFA Collimator mirror actuator"},
  {"bcolttfa", cmd_colttf,   "bcolttfa [bPos|step +/-bPos]", "Tip/Query Blue TTFA Collimator mirror actuator"},
  {"rcolttfb", cmd_colttf,   "rcolttfb [bPos|step +/-bPos]", "Tilt/Query Red TTFB Collimator mirror actuator"},
  {"bcolttfb", cmd_colttf,   "bcolttfb [aPos|step +/-aPos]", "Tilt/Query Blue TTFB collimator mirror actuator"},
  {"rcolttfc", cmd_colttf,   "rcolttfc [bPos|step +/-bPos]", "Tilt/Query Red TTFC Collimator mirror actuator"},
  {"bcolttfc", cmd_colttf,   "bcolttfc [cPos|step +/-cPos]", "Tilt/Query Blue TTFC collimator mirror actuator"},
  {"rcolfoc",  cmd_colfoc,   "rcolfoc [aPos bPos cPos|step +/-aPos +/-bPos +/-cPos]", "Focus/Query Red TTF Focus Collimator mirror to absolute position"},
  {"bcolfoc",  cmd_colfoc,   "bcolfoc [aPos bPos cPos|step +/-aPos +/-bPos +/-cPos]", "Focus/Query Blue TTF Focus Collimator mirror to absolute position"},
  {"rcolstep", cmd_colstep,  "rcolstep [dfoc|dfa dfb dfc]", "Increment(step)/Query Red TTF Collimator mirror to focus position"},
  {"bcolstep", cmd_colstep,  "bcolstep [dfoc|dfa dfb dfc]", "Set/Query Blue Collimator mirror focus position"},
  {"rgrating", cmd_grating,  "rgrating [n|ID]", "Select/Query grating in the Red grating turret"},
  {"bgrating", cmd_grating,  "bgrating [n|ID]", "Select/Query grating in the Blue grating turret"},
  {"rgrtilt1", cmd_grtilt,   "rgrtilt1 [tPos|step +/-tPos]", "Tilt/Query Red grating 1 to a given absolute tilt"},
  {"bgrtilt1", cmd_grtilt,   "bgrtilt1 [tPos|step +/tPos]", "Tilt/Query Blue grating 1 to a given absolute tilt"},
  {"rgrtilt2", cmd_grtilt,   "rgrtilt2 [tPos|step +/tPos]", "Tilt/Query Red grating 2 to a given absolute tilt"},
  {"bgrtilt2", cmd_grtilt,   "bgrtilt2 [tPos|step +/-tPos]", "Tilt/Query Blue grating 2 to a given absolute tilt"},
  {"rgrtilt3", cmd_grtilt,   "rgrtilt3 [tPos|step +/-tPos]", "Tilt/Query Red grating 3 to a given absolute tilt"},
  {"bgrtilt3", cmd_grtilt,   "bgrtilt3 [tilt|home]", "Tilt/Query Blue grating 3 to a given absolute tilt"},
  {"rgrtilt4", cmd_grtilt,   "rgrtilt4 [tilt|home]", "Tilt/Query Red grating 4 to a given absolute tilt"},
  {"bgrtilt4", cmd_grtilt,   "bgrtilt4 [tilt|home]", "Tilt/Query Blue grating 4 to a given absolute tilt"},
  {"rgrstep1", cmd_grstep,   "rgrstep1 [dt]", "Increment/Query tilt of Red grating"},
  {"bgrstep1", cmd_grstep,   "bgrstep1 [dt]", "Increment/Query tilt of Blue grating"},
  {"rgrstep2", cmd_grstep,   "rgrstep2 [dt]", "Increment/Query tilt of Red grating"},
  {"bgrstep2", cmd_grstep,   "bgrstep2 [dt]", "Increment/Query tilt of Blue grating"},
  {"rgrstep3", cmd_grstep,   "rgrstep3 [dt]", "Increment/Query tilt of Red grating"},
  {"bgrstep3", cmd_grstep,   "bgrstep3 [dt]", "Increment/Query tilt of Blue grating"},
  {"rgrstep4", cmd_grstep,   "rgrstep4 [dt]", "Increment/Query tilt of Red grating"},
  {"bgrstep4", cmd_grstep,   "bgrstep4 [dt]", "Increment/Query tilt of Blue grating"},
  {"rshutter", cmd_shutter,  "rshutter [open|close]", "Open/Close/Query Red camera shtter"},
  {"bshutter", cmd_shutter,  "bshutter [open|close]", "Open/Close/Query Blue camera shtter"},
  {"rfilter",  cmd_filter,   "rfilter [n|ID] [Load]", "Set/Query/Load Red Filter Wheel"},
  {"bfilter",  cmd_filter,   "bfilter [n|ID] [Load]", "Set/Query/Load Blue Filter Wheel"},
  {"rcamfoc",  cmd_camfoc,   "rcamfoc [foc|home]", "Focus/Query Red camera primary mirror"},
  {"bcamfoc",  cmd_camfoc,   "bcamfoc [foc|home]", "Focus/Query Blue camera primary mirror"},
  {"rcamstep", cmd_camstep,  "rcamstep [dfoc]", "Increment(step)/Query Red camera primary mirror"},
  {"bcamstep", cmd_camstep,  "bcamstep [dfoc]", "Increment(step)/Query Blue camera primary mirror"},
  {"lamp",     cmd_lamp,     "lamp [lamp1 lamp2 ...] [on|off|info]", "Operate Calibration Lamps"},
  {"irlaser",  cmd_irlaser,  "irlaser [pon|poff] [enable|disable] [power [lev]] [temp [Tset]] [status|info]", "Operate IMCS Infrared Reference Laser"},
  {"vislaser", cmd_vislaser, "vislaser [pon|poff] [enable|disable] [power [lev]] [status|info]", "Operate Visible-light Alignment Laser"},
  {"ieb",      cmd_ieb,      "ieb [n] [status|power|temp|mlc] [n] [on|off]", "IEB Interface"},
  {"close",    cmd_close,    "close","Close an open comm port"},
  {"debug",    cmd_debug,    "debug","Toggle super-verbose debugging output mode on/off"},
  {"open",     cmd_open,     "open <port>","Open a comm port, /dev/ttyX or host:port"},
  {"ports",    cmd_ports,    "ports","List information about the active comm ports"},
  {"reset",    cmd_reset,    "reset","Reload the runtime config file and re-initialize the comm port"},
  {"setport",  cmd_setport,  "setport <speed> <data> <stop> <parity>","Set/Query TTY port attributes"},
  {"sy",       cmd_sy,       "sy <UNIX command>","execute a shell command"},
  {"abort",    cmd_reset,    "abort [mechanism|all]","Abort mechanism motion(s)"},
  {"moverel",  cmd_moverel,  "moverel","execute a relative move"},
  {"moveabs",  cmd_moveabs,  "moveabs","execute an absolute move"},
  {"verbose",  cmd_verbose,  "verbose","Toggle verbose output mode on/off"},
  {"version",  cmd_version,  "version","Report the application version and compilation time"},
  {"ping",     cmd_ping,"",""},
  {"pong",     cmd_pong,"",""},   
  {"quit",     cmd_quit,     "quit","Quit MicroLynx Services (exit mmcService)"},   
  //  {"history" ,cmd_history ,"history","Show the command history"},
  {"info",     cmd_info,     "info","Report application runtime info"},
  {"help",     cmd_help,     "help <cmd>","Help command (alias: ? <cmd>)"},
  {"status",   cmd_info,     "status","MMC Service Status"},
  {"mstatus",  cmd_mstatus,  "mstatus <who>","mechanism status command"},
  {"istatus",  cmd_istatus,  "istatus ","Query Instrument General Status"},
  {"pstatus",  cmd_pstatus,  "pstatus ","Query Instrument Power Status"},
  {"bimcs",    cmd_imcs,  "bimcs [start|stop|qcells|freq|rate] [dfoc]", "BLUE IMCS"},
  {"rimcs",    cmd_imcs,  "rimcs [start|stop|qcells|freq|rate] [dfoc]", "RED IMCS"},
  {"loadplc",  cmd_loadplc,  "loadplc [mechanism file]","Load a MicroLynx PLC code"},
  {"util",     cmd_reset,    "util [status][service1,service2.. [status|on|off]][all off] ","Utility Box control"},
  {"llb",      cmd_reset,    "llb reset","Turn off all lamps and lasers"},
  {"startup",  cmd_reset,    "startup [IEBn|warm]","Cold/Warm all MODS units"},
  {"stow",     cmd_reset,    "stow","Safely stow all MODS mechanisms"},
  {"shutdown", cmd_reset,    "shutdown","Shutoff MODS power"},
  {"wake",     cmd_reset,    "wake","Restore Power - Wakeup MODS"},
  {"sleep",    cmd_reset,    "sleep","Put MODS into low-power standby mode"},
  {"saveconfig",cmd_reset,   "saveconfig","Save all MODS state"},
  {"panic",    cmd_reset,    "panic","Emergency Stop"},
  {"estatus",  cmd_reset,  "estatus ","Query Instrument and Glycol Temperatures and Pressures Status"},
  {"calmode",  cmd_reset,    "calmode","Calibration mode"},
  {"obsmode",  cmd_reset,    "obsmode","Observing mode"},
  {"?",        cmd_help,"",""}  // "" excludes from help
};

// Number of commands defined (so we don't have to count correctly)
  
int NumCommands = sizeof(cmdtab)/sizeof(struct Commands);  //!< number of commands defined

// command function return codes

#define CMD_OK   0   //!< Command executed OK, return completion status
#define CMD_ERR -1   //!< Command execution resulted in an error
#define CMD_NOOP 1   //!< Command execution requires no further action (no-op)
#define CMD_FERR 2   //!< Command execution resulted in a fatal error
#define CMD_WARN 3   //!< Command execution resulted in a warning

#endif // COMMANDS_H
