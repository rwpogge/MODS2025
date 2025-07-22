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

2025 June 21 - AlmaLinux 9 port [rwp/osu]
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
int cmd_loadplc(char *, MsgType, char *); // Load MicroLynx controller code
int cmd_abort  (char *, MsgType, char *); // Abort one or all mechanism motions

// App-specific commands

// MODS Microlynx Controller Service(s)
int cmd_hatch    (char *, MsgType, char *); // open/close instrument dark hatch
int cmd_calib    (char *, MsgType, char *); // insert/retract calibration tower
int cmd_agw      (char *, MsgType, char *); // AGW stage xyf position
int cmd_agwy     (char *, MsgType, char *); // AGW stage x-axis actuator position
int cmd_agwx     (char *, MsgType, char *); // AGW stage y-axis actuator position
int cmd_agwfoc   (char *, MsgType, char *); // AGW stage focus actuator position
int cmd_agwfilt  (char *, MsgType, char *); // AGW guide camera filter select
int cmd_gprobe   (char *, MsgType, char *); // AGW guide probe xy position
int cmd_gpfocus  (char *, MsgType, char *); // AGW guide probe focus at fixed gp xy
int cmd_gpoffset (char *, MsgType, char *); // AGW offset guide probe xy position
int cmd_minsert  (char *, MsgType, char *); // insert/stow a slit mask 
int cmd_mselect  (char *, MsgType, char *); // select slit mask cassette position
int cmd_slitmask (char *, MsgType, char *); // select and insert a slit mask
int cmd_dichroic (char *, MsgType, char *); // Dichroic turret position select
int cmd_colttf   (char *, MsgType, char *); // Collimator tip/tilt/focus 
int cmd_colfoc   (char *, MsgType, char *); // Collimator focus 
int cmd_grating  (char *, MsgType, char *); // Grating select
int cmd_grtilt   (char *, MsgType, char *); // Grating tilt
int cmd_shutter  (char *, MsgType, char *); // shutter
int cmd_filter   (char *, MsgType, char *); // Filter Wheel
int cmd_camfoc   (char *, MsgType, char *); // camera focus
int cmd_lamp     (char *, MsgType, char *); // Calibration lamp control
int cmd_irlaser  (char *, MsgType, char *); // IMCS IR laser control
int cmd_vislaser (char *, MsgType, char *); // Alignment visible laser control
int cmd_ieb      (char *, MsgType, char *); // Instrument Electronics Box (IEB) commands
int cmd_close    (char *, MsgType, char *); // close comm port
int cmd_open     (char *, MsgType, char *); // open comm port
int cmd_ports    (char *, MsgType, char *); // comm port info
int cmd_proc     (char *, MsgType, char *); // procedure source code
int cmd_setport  (char *, MsgType, char *); // set/query comm port attributes
int cmd_moverel  (char *, MsgType, char *); // relative move
int cmd_moveabs  (char *, MsgType, char *); // absolute move
int cmd_imcs     (char *, MsgType, char *); // IMCS
int cmd_bimcs    (char *, MsgType, char *); // Blue channel IMCS control
int cmd_rimcs    (char *, MsgType, char *); // Red channel IMCS control
int cmd_misc     (char *, MsgType, char *); // miscellaneous functions (util, llb, calmode, obsmode, estatus)

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
  {"dichroic", cmd_dichroic, "dichroic [n|blue|red|both]", "Select/Query the dichroic beam selector position"},
  {"rcolttfa", cmd_colttf,   "rcolttfa [aPos|step +/-aPos]", "Tip/Query Red TTFA Collimator mirror actuator"},
  {"bcolttfa", cmd_colttf,   "bcolttfa [bPos|step +/-bPos]", "Tip/Query Blue TTFA Collimator mirror actuator"},
  {"rcolttfb", cmd_colttf,   "rcolttfb [bPos|step +/-bPos]", "Tilt/Query Red TTFB Collimator mirror actuator"},
  {"bcolttfb", cmd_colttf,   "bcolttfb [aPos|step +/-aPos]", "Tilt/Query Blue TTFB collimator mirror actuator"},
  {"rcolttfc", cmd_colttf,   "rcolttfc [bPos|step +/-bPos]", "Tilt/Query Red TTFC Collimator mirror actuator"},
  {"bcolttfc", cmd_colttf,   "bcolttfc [cPos|step +/-cPos]", "Tilt/Query Blue TTFC collimator mirror actuator"},
  {"rcolfoc",  cmd_colfoc,   "rcolfoc [aPos bPos cPos|step +/-aPos +/-bPos +/-cPos]", "Focus/Query Red TTF Focus Collimator mirror to absolute position"},
  {"bcolfoc",  cmd_colfoc,   "bcolfoc [aPos bPos cPos|step +/-aPos +/-bPos +/-cPos]", "Focus/Query Blue TTF Focus Collimator mirror to absolute position"},
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
  {"rshutter", cmd_shutter,  "rshutter [open|close]", "Open/Close/Query Red camera shtter"},
  {"bshutter", cmd_shutter,  "bshutter [open|close]", "Open/Close/Query Blue camera shtter"},
  {"rfilter",  cmd_filter,   "rfilter [n|ID] [Load]", "Set/Query/Load Red Filter Wheel"},
  {"bfilter",  cmd_filter,   "bfilter [n|ID] [Load]", "Set/Query/Load Blue Filter Wheel"},
  {"rcamfoc",  cmd_camfoc,   "rcamfoc [foc|home]", "Focus/Query Red camera primary mirror"},
  {"bcamfoc",  cmd_camfoc,   "bcamfoc [foc|home]", "Focus/Query Blue camera primary mirror"},
  {"lamp",     cmd_lamp,     "lamp [lamp1 lamp2 ...] [on|off|info]", "Operate Calibration Lamps"},
  {"irlaser",  cmd_irlaser,  "irlaser [pon|poff] [enable|disable] [power [lev]] [temp [Tset]] [status|info]", "Operate IMCS Infrared Reference Laser"},
  {"vislaser", cmd_vislaser, "vislaser [pon|poff] [enable|disable] [power [lev]] [status|info]", "Operate Visible-light Alignment Laser"},
  {"ieb",      cmd_ieb,      "ieb [n] [status|power|temp|mlc] [n] [on|off]", "IEB Interface"},
  {"close",    cmd_close,    "close","Close an open comm port"},
  {"debug",    cmd_debug,    "debug","Toggle super-verbose debugging output mode on/off"},
  {"open",     cmd_open,     "open <port>","Open a comm port, /dev/ttyX or host:port"},
  {"ports",    cmd_ports,    "ports","List information about the active comm ports"},
  {"setport",  cmd_setport,  "setport <speed> <data> <stop> <parity>","Set/Query TTY port attributes"},
  {"moverel",  cmd_moverel,  "moverel","execute a relative move"},
  {"moveabs",  cmd_moveabs,  "moveabs","execute an absolute move"},
  {"verbose",  cmd_verbose,  "verbose","Toggle verbose output mode on/off"},
  {"version",  cmd_version,  "version","Report the application version and compilation time"},
  {"ping",     cmd_ping,"",""},
  {"pong",     cmd_pong,"",""},   
  {"quit",     cmd_quit,     "quit","Quit MicroLynx Services (exit mmcService)"},   
  {"info",     cmd_info,     "info","Report application runtime info"},
  {"help",     cmd_help,     "help <cmd>","Help command (alias: ? <cmd>)"},
  {"status",   cmd_info,     "status","MMC server status"},
  {"mstatus",  cmd_mstatus,  "mstatus <who>","mechanism status command"},
  {"istatus",  cmd_istatus,  "istatus ","Query instrument configuration status"},
  {"pstatus",  cmd_pstatus,  "pstatus ","Query instrument power status"},
  {"bimcs",    cmd_imcs,     "bimcs [start|stop|qcells|freq|rate] [dfoc]", "BLUE IMCS"},
  {"rimcs",    cmd_imcs,     "rimcs [start|stop|qcells|freq|rate] [dfoc]", "RED IMCS"},
  {"loadplc",  cmd_loadplc,  "loadplc [mechanism file]","Load a MicroLynx PLC code"},
  {"util",     cmd_misc,     "util [status][service1,service2.. [status|on|off]][all off] ","Utility Box control"},
  {"llb",      cmd_misc,     "llb reset","Turn off all lamps and lasers"},
  {"calmode",  cmd_misc,     "calmode","Calibration mode"},
  {"obsmode",  cmd_misc,     "obsmode","Observing mode"},
  {"estatus",  cmd_misc,     "estatus ","Query instrument environmental sensor status"},
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

// moved from hacked isisclient.c (righting old wrong) [rwp/osu - 2025 Jun23]

float findPoly(float, float *, int);
double dfindPoly(double, double *, int);

#endif // COMMANDS_H
