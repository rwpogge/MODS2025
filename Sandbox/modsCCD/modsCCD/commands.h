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

// azcam server commands

int cmd_azcam   (char *, MsgType, char *); // Send a raw AzCam server command 
int cmd_reset   (char *, MsgType, char *); // Reset the AzCam server
int cmd_process (char *, MsgType, char *); // Upload image info to DataMan for post-processing
int cmd_ccdinit (char *, MsgType, char *); // (Re)Initialize the CCD Controller
int cmd_ccdinfo (char *, MsgType, char *); // Print CCD config info
int cmd_cleanup (char *, MsgType, char *); // Clean up after problems

// Observation and project info (required by the LBTO Data Archive)

int cmd_observer(char *, MsgType, char *); // Set/query the observer name(s) 
int cmd_piname   (char *, MsgType, char *); // Set the PI_NAME keyword
int cmd_partner  (char *, MsgType, char *); // Set the PARTNER keyword
int cmd_propid   (char *, MsgType, char *); // Set the PROPID keyword
int cmd_support  (char *, MsgType, char *); // Set the SUPPORT keyword
int cmd_telops   (char *, MsgType, char *); // Set the TELOPS keyword

// Exposure setup

int cmd_exptime (char *, MsgType, char *); // Set/query the exposure time
int cmd_object  (char *, MsgType, char *); // Set image type to object w/optional name
int cmd_bias    (char *, MsgType, char *); // Set image type to bias w/optional name 
int cmd_flat    (char *, MsgType, char *); // Set image type to flat w/optional name 
int cmd_dark    (char *, MsgType, char *); // Set image type to dark w/optional name 
int cmd_comp    (char *, MsgType, char *); // Set image type to comp w/optional name 
int cmd_zero    (char *, MsgType, char *); // Set image type to zero w/optional name
int cmd_std     (char *, MsgType, char *); // Set image type to std w/optional name

// Image filenames and paths

int cmd_filename(char *, MsgType, char *); // Set/Query the raw data file name pattern 
int cmd_path    (char *, MsgType, char *); // Set/Query the raw data file path 
int cmd_expnum  (char *, MsgType, char *); // Set/Query the raw data file counter 
int cmd_lastfile(char *, MsgType, char *); // Query the name of the last file written

// CCD on-chip binning and region-of-interest readout

int cmd_ccdbin  (char *, MsgType, char *); // Set/Query the CCD on-chip binning factors
int cmd_xbin    (char *, MsgType, char *); // Set/Query the CCD on-chip x (column) binning factor
int cmd_ybin    (char *, MsgType, char *); // Set/Query the CCD on-chip y (row) binning factor
int cmd_roi     (char *, MsgType, char *); // Set/Query the CCD readout region-of-interest (ROI)

// CCD controller functions

int cmd_ccdtemp (char *, MsgType, char *); // query the CCD temperatures (detector and base)
int cmd_shopen  (char *, MsgType, char *); // Open the shutter 
int cmd_shclose (char *, MsgType, char *); // Close the shutter 
int cmd_shutter (char *, MsgType, char *); // Alternative: shutter 1|0 (open/close)

// Exposure control commands

int cmd_go      (char *, MsgType, char *); // Start an exposure
int cmd_pause   (char *, MsgType, char *); // Pause an exposure  
int cmd_resume  (char *, MsgType, char *); // Resume a paused exposure  
int cmd_abort   (char *, MsgType, char *); // Abort an exposure or readout

// IC back-compatibility functions

int cmd_config  (char *, MsgType, char *); // Report instrument configuration info
int cmd_status  (char *, MsgType, char *); // Report the CCD system status 
int cmd_saveconf(char *, MsgType, char *); // Save the current configuration (saveconfig)

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
  {"help"    ,cmd_help    ,"help <cmd>","Help command (alias: ? <cmd>)"},
  {"config"  ,cmd_config  ,"config","Report the current instrument configuration (ICIMACS compatibility)"},
  {"status"  ,cmd_status  ,"status","Report the current instrument status"},
  {"reset"   ,cmd_reset   ,"reset","Reset the CCD azcam server"},
  {"go"      ,cmd_go      ,"go","Take an image (uses exptime and the current image type)"},
  {"pause"   ,cmd_pause   ,"pause","Pause an exposure in progress (see RESUME or ABORT)"},
  {"resume"  ,cmd_resume  ,"resume","Resume a paused exposure (see PAUSE)"},
  {"abort"   ,cmd_abort   ,"abort","Abort an image in progress and discard it (or abort paused exposures)"},
  {"exptime" ,cmd_exptime ,"exptime <sec>","Set/query the exposure time in decimal seconds"},
  {"exp"     ,cmd_exptime ,"",""}, // alias for exptime
  {"object"  ,cmd_object  ,"object <name>","Set image type to OBJECT, including optional image name"},
  {"flat"    ,cmd_flat    ,"flat <name>","Set image type to FLAT, including optional image name"},
  {"dark"    ,cmd_dark    ,"dark <name>","Set image type to DARK, including optional image name"},
  {"bias"    ,cmd_bias    ,"bias <name>","Set image type to BIAS, including optional image name"},
  {"zero"    ,cmd_zero    ,"zero <name>","Set image type to ZERO, including optional image name"},
  {"comp"    ,cmd_comp    ,"comp <name>","Set image type to COMP, including optional image name"},
  {"std"     ,cmd_std     ,"std <name>","Set image type to STD, including optional image name"},
  {"observer",cmd_observer,"observer <str>","Set/query the OBSERVER name(s) for the header"},
  {"pi_name" ,cmd_piname  ,"pi_name <str>","Set/query the PI_NAME keyword for the header"},
  {"partner" ,cmd_partner ,"partner <str>","Set/query the PARTNER keyword for the header"},
  {"propid"  ,cmd_propid  ,"propid <ID>","Set/query the PROPID keyword for the header"},
  {"support" ,cmd_support ,"support <str>","Set/query the SUPPORT keyword for the header"},
  {"telops"  ,cmd_telops  ,"telops <str>","Set/query the TELOPS keyword for the header"},
  {"filename",cmd_filename,"filename <root>","Set/query the data file rootname (rootname.nnnn.fits)"},
  {"path"    ,cmd_path    ,"path <C:/>","Set/query the raw data file path on the AzCam server"},
  {"expnum"  ,cmd_expnum  ,"expnum <n>","Set/query the file counter for the *NEXT* image"},
  {"lastfile",cmd_lastfile,"lastfile","Query the name of the last file written to disk"},
  {"write"   ,cmd_writeim ,"write <image>","Write the current image buffer to disk (uses default path)"},
  {"process" ,cmd_process ,"process <image>","Upload image info for post-processing following write"},
  {"shopen"  ,cmd_shopen  ,"shopen","Open the shutter, stays open until shclose"},
  {"shclose" ,cmd_shclose ,"shclose","Close the shutter"},
  {"ccdbin"  ,cmd_ccdbin  ,"ccdbin nx ny","Set/query the CCD on-chip binning factors in x and y"},
  {"ccdtemp" ,cmd_ccdtemp ,"ccdtemp","Query the CCD detector and mount base temperatures in deg C"},
  {"roi"     ,cmd_roi     ,"roi [off|sc ec sr er]","Set/query/reset the CCD readout region-of-interest"},
  {"ccdinit" ,cmd_ccdinit ,"ccdinit","(Re)Initialize the CCD Controller"},
  {"ccdinfo" ,cmd_ccdinfo ,"ccdinfo","Print detailed AzCam Server and CCD config info (engineering)"},
  {"cleanup" ,cmd_cleanup ,"cleanup","Cleanup runtime parameters after serious errors"},
  {"saveconfig",cmd_saveconf,"saveconfig","Save the current runtime configuration (e.g., for restarts)"},
  {"azcam"   ,cmd_azcam   ,"azcam <cmd>","Send a raw command to the AzCam Server"},
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
#define CMD_STATUS 2 //!< Command execution results in a STATUS: message

#endif // COMMANDS_H
