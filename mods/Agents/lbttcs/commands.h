#ifndef COMMANDS_H
#define COMMANDS_H

/*!
  \file commands.h
  \brief LBTTCS client application command tree header file

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
int cmd_status (char *, MsgType, char *); // Report the application status

// General TCS Commands

int cmd_tcinit  (char *, MsgType, char *);  // Initialize an TCS client session
int cmd_tcstatus(char *, MsgType, char *);  // Query the TCS for status
int cmd_gettime (char *, MsgType, char *);  // Query time/date only (no TCS info)
int cmd_tcclose (char *, MsgType, char *);  // Close an TCS client session
int cmd_config  (char *, MsgType, char *);  // Print TCS config info (engineering)
int cmd_setturbo(char *, MsgType, char *);  // Set "Turbo" TCS query mode
int cmd_setfull (char *, MsgType, char *);  // [Re]set full TCS query mode (default)
int cmd_authorize(char *, MsgType, char *); // Query the authorization status
int cmd_telpos  (char *, MsgType, char *);  // Just query the current telescope position (subset of tcstatus)

// Preset and Offset Commands

int cmd_clear    (char *, MsgType, char *); // Clear all targeting information (ClearStars)
int cmd_bc       (char *, MsgType, char *); // Send a binocular control command (BinocularControl)
int cmd_objname  (char *, MsgType, char *); // Set/Query the object ID for the preset
int cmd_objcoords(char *, MsgType, char *); // Set/Query the object coords (RA,Dec)
int cmd_guiname  (char *, MsgType, char *); // Set/Query the guide star ID
int cmd_guicoords(char *, MsgType, char *); // Set/Query the guide star coords (RA,Dec)
int cmd_rotator  (char *, MsgType, char *); // Set/Query the telescope rotator angle and mode
int cmd_refwave  (char *, MsgType, char *); // Set/Query the target reference wavelength (microns)
int cmd_side     (char *, MsgType, char *); // Set/Query the telescope "side" to use (left|right|both)
int cmd_preset   (char *, MsgType, char *); // Issue a telescope preset (LBT-ese for "point")
int cmd_offset   (char *, MsgType, char *); // General telescope offset (full IIF OffsetPointing() syntax)
int cmd_skyoffset(char *, MsgType, char *); // Issue a telescope offset in celestial coordinates (RADEC)
int cmd_slitoffset(char *, MsgType, char *); // Issue a telescope offset in slit coordinates (DETXY)
int cmd_rotoffset(char *, MsgType, char *); // Issue a telescope rotator offset in degrees
int cmd_update   (char *, MsgType, char *); // Update the pointing reference
int cmd_remove   (char *, MsgType, char *); // Remove pointing, offset, and hotspot objects

// Other IIF Functions

int cmd_logevent (char *, MsgType, char *); // Log an instrument "event"
int cmd_setparam (char *, MsgType, char *); // Set a parameter in the Data Dictionary
int cmd_getparam (char *, MsgType, char *); // Get a parameter from the Data Dictionary
int cmd_gpause   (char *, MsgType, char *); // Pause Guiding
int cmd_gresume  (char *, MsgType, char *); // Resume Guiding
int cmd_rotready (char *, MsgType, char *); // Enable/Disable the ISS rotator (engineering)
int cmd_rotsvcpos(char *, MsgType, char *); // Move the ISS rotator (engineering)


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
  {"verbose" ,cmd_verbose ,"verbose","Toggle verbose output mode on/off"},
  {"debug"   ,cmd_debug   ,"debug","Toggle super-verbose debugging output mode on/off"},
  {"history" ,cmd_history ,"history","Show the command history (console only)"},
  {"info"    ,cmd_info    ,"info","Report client session runtime info"},
  {"version" ,cmd_version ,"version","Report the client version and compilation time"},
  {"iifinit" ,cmd_tcinit  ,"iifinit <proxy instID focStation side>","Initialize the LBT IIF connection"},
  {"iifclose",cmd_tcclose ,"iifclose","Close the LBT IIF client connection"},
  {"tcstatus",cmd_tcstatus,"tcstatus","Query and report the LBT status"},
  {"gettime" ,cmd_gettime ,"gettime","Query and report the UTC date/time (no TCS query)"},
  {"telpos"  ,cmd_telpos  ,"telpos","Query for current LBT pointing info (subset of tcstatus)"}, 
  {"status"  ,cmd_status  ,"status","Report the current LBT IIF client status"},
  {"+turbo"  ,cmd_setturbo,"+turbo","Setup for \"turbo\" query mode (time-only queries)"},
  {"-turbo"  ,cmd_setfull ,"-turbo","Restore full TCS query mode"},
  {"authorized",cmd_authorize,"authorized","Query the authorization status on the LBT focal station"},
  {"bc"      ,cmd_bc      ,"bc","Send a Binocular Control (BC) command to the TCS"},
  {"clearstars",cmd_clear ,"ClearStars","Clear target and guider coordinates"},
  {"objname" ,cmd_objname ,"objname <string>","Set/Query the target object name"},
  {"objcoords",cmd_objcoords,"objcoords <ra dec <pmr pmd epoch>>",
   "Set/Query the target coordinates (sexagesimal J2000, optional proper motion in mas/year)"},
  {"guiname" ,cmd_guiname ,"guiname <string>","Set/Query the guide object name"},
  {"guicoords",cmd_guicoords,"guicoords <ra dec <pmr pmd epoch>>",
   "Set/Query the guide object coordinates (sexagesimal J2000, optional proper motion in mas/year)"},
  {"rotator" ,cmd_rotator ,"rotator <pa mode>","Set/Query the instrument rotator angle and mode"},
  {"refwave" ,cmd_refwave ,"refwave <lam>","Set/Query the target reference wavelength in microns"},
  {"remove"  ,cmd_remove  ,"remove","Remove target, offset, and hotspot objects from the IIF"},
  {"side"    ,cmd_side    ,"side <left|right|both>","Set/Query the active telescope side"},
  {"preset"  ,cmd_preset  ,"preset [track|guide|active|acquire|static] [side]","Preset (point) the telescope"},
  {"skyoffset",cmd_skyoffset,"skyoffset dRA dDec mode","Offset the telescope dRA, dDec in arcsec, mode=(rel|abs)"},
  {"offset",cmd_skyoffset,"offset dRA dDec mode","Offset the telescope dRA, dDec in arcsec, mode=(rel|abs)"},
  {"slitoffset",cmd_slitoffset,"slitoffset dX dY mode","Offset the telescope in the slit plane, dX, dY in arcsec (dY=slit), mode=(rel|abs)"},
  {"offsetxy",cmd_slitoffset,"offsetXY dX dY mode","Offset the telescope in the slit plane, dX, dY in arcsec (dY=slit), mode=(rel|abs)"},
  {"rotoffset",cmd_rotoffset,"rotoffset dPA","Offset the telescope rotator relative to the current position"},
  {"updatepointing",cmd_update,"UpdatePointing [detxy|radec] [left|right|both]","Update the telescope pointing reference (i.e., absorb an offset)"},
  {"offsetpointing",cmd_offset,"OffsetPointing dPA dRA dDec coordSys offMode side - experts only..."},
  {"logevent",cmd_logevent ,"LogEvent <name descr>","log an instrument event"},
  {"setparameter",cmd_setparam,"SetParameter <DDname value>","Set a parameter in the Data Dictionary"},
  {"getparameter",cmd_getparam,"GetParameter <DDname>","Get a parameter from the Data Dictionary"},
  {"pauseguiding",cmd_gpause,"PauseGuiding [left|right|both]","Pause guiding temporarily"},
  {"resumeguiding",cmd_gresume,"ResumeGuiding [left|right|both]","Resume guiding after PauseGuiding"},
  {"rotready" ,cmd_rotready,"RotReady [enable|disable] [side]","Enable/Disable the ISS rotator [engineering only]"},
  {"rotpos"   ,cmd_rotsvcpos,"RotPos angle [side]","Slew the ISS rotator to raw position angle in degrees [engineering only]"},
  {"tcinit"   ,cmd_tcinit  ,"tcinit"  ,"alias for iifinit"},
  {"tcclose"  ,cmd_tcclose ,"tcclose" ,"alias for iifclose"},
  {"setparam" ,cmd_setparam,"setparam","alias for SetParameter"},
  {"getparam" ,cmd_getparam,"getparam","alias for GetParameter"},
  {"gpause"   ,cmd_gpause  ,"gpause"  ,"alias for PauseGuiding"},
  {"gresume"  ,cmd_gresume ,"gresume" ,"alias for ResumeGuiding"},
  {"update"   ,cmd_update  ,"update"  ,"alias for UpdatePointing"},
  {"clear"    ,cmd_clear   ,"clear"   ,"alias for ClearStars"},
  {"config"   ,cmd_config  ,"config","Report TCS configuration [engineering]"},
  {"help"     ,cmd_help    ,"help <cmd>","Help command (alias: ? <cmd>)"},
  {"?"        ,cmd_help    ,"",""},  // "" excludes from help
  {"ping"     ,cmd_ping    ,"",""},
  {"pong"     ,cmd_pong    ,"",""}   
};

// Number of commands defined (so we don't have to count correctly)
  
int NumCommands = sizeof(cmdtab)/sizeof(struct Commands);  //!< number of commands defined

// command function return codes

#define CMD_OK   0   //!< Command executed OK, return completion status
#define CMD_ERR -1   //!< Command execution resulted in an error
#define CMD_NOOP 1   //!< Command execution requires no further action (no-op)

#endif // COMMANDS_H
