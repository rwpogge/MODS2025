/* -------------------------------------------------------------------------
//
// commands.c - application command interpreter
//
*/
/*!
  \file commands.c 
  \brief MODS mechanism control server (mmcServer, aka Mn.IE host) commands

  This module contains the command "action" functions called to service
  the MODS IE commands.  These consist of a suite of \arg "common" client
  action functions common to most ISIS clients \arg client-specific
  functions that perform the client's particular tasks \arg common
  interface routines for keyboard and ISIS socket interfaces.

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
  \date 2003 October 13
  \date 2005 March 20 Many features added. (rdg)
  \date 2009 April/May - refinements during MODS1 lab integration and testing (rdg/tja/rwp)
  \date 2010 & 2011 - bug fixes during MODS1 at LBT commissioning [rwp/rdg]
  \date 2012 Feb - major bug in OBSMODE that zeroed IMCS laser power fixed [rwp]

  \date 2025 Jun 20 - AlmaLinux 9 port [rwp/osu]
  \date 2025 Jul 16 - changed HEB power status/on/off logic for new WAGO HEB [rwp/osu]
  \date 2025 Jul 20 - power state code updates and clean up shmem interaction [rwp/osu]

  \date 2025 Aug 08 - many changes after live tests with MODS1 at LBT (off telescope) [rwp/osu]

*/
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <sys/wait.h>
#include <time.h>
#include <errno.h>   // system error code

using namespace std;

#include "instrutils.h"   // Custom client application header file
#include "isl_funcs.h"    // Command action functions header file
#include "mmc_client.h"   // Custom MicroLynx app header file
#include "mmc_commands.h" // Command action functions header file
#include "isl_types.h"    // mods data types
#include "isl_shmaddr.h"  // Shared memory header
#include "islcommon.h"    // Shared memory common storage
#include "modscontrol.h"  // MODS function header
#include "mmccontrol.h"   // MMC Service header

// retired fieldtalk for WAGO modbus interface, using open-source libmodbus 

// #include "MbusTcpMasterProtocol.hpp" // fieldtalk package header

// WAGO system function

int
wagoSetGet(int gs, char *host, int slaveAddr, int startRef, short regArr[], int refCnt);

short devOnOff[1];

int itoa(int ,char []);

//2 state, Linear and Indexed mechanisms

#define BI_STATE   0
#define LINEAR     1
#define INDEXED    2

// WAGO Addresses - all had to shift by 1 for libmodbus (0 -> 1) [rwp/osu]

#define VISPOUTWAGO 0
#define IRPOUTWAGO  1
#define IRTSETWAGO  2
#define IRTOUTWAGO  3
#define VISPSETWAGO 512
#define IRPSETWAGO  513
#define VFLATPSET   514
#define OPENADDR    515
#define LLBONOFF    516

// Maximum mumber of characters/line in the program file

#define MAXPGMLINE 80 

char argbuf[MAXPGMLINE]; // Generic argument buffer
int commandID;           // Keeps the command id that identifies the mechanism
 
void StrUpper(char *);
int logLampParam(char *, int [], int[]);
int getCurrentTime(char* , char* );
int mmcLOGGER(char [],char []);

//int mmcGetConfig(char [],char []);  in mmcLOGGER.c, unused.
//int mmcSaveConfig(char [],char []);
extern char *mods_fatalerrormsg[];

static char *lamp_names[9] = 
  {"AR","XE","NE","HG","KR","QTH6V", "QTH1","QTH2","VFLAT"}; // lamp names

static char *util_names[7] = 
  {"IEB_R","IEB_B","HEB_R","HEB_B","LLB","WFS", "AGC"}; // lamp names

static short int lamp_codes[9] = {1,2,4,8,16,32,1024,2048,4096}; // lamp codes

char who_srcID[MAXPGMLINE];    // Generic input buffer

#include "./mlc.c"         // local 'C' functions

// WAGO IDs

int  llbID;  // Lamp/Laser Box
int  utilID; // Instrument Utility Box
int  ieb1ID; // Red Instrument Electronics Box (ieb 1)
int  ieb2ID; // Blue Instrument Electronics Box (ieb 2)
int  rhebID; // Red Head Electronics Box
int  bhebID; // Blue Head Electronics Box

int ierr;                  // Error indicator
int he_token=1;            // Head Electronic is ENABLED/DISABLED
char inbuf[MAXPGMLINE];    // Generic input buffer
FILE *pgmFP;               // Program file pointer

/* ---------------------------------------------------------------------------
//
// Common client commands
//
// Commands common to most ISIS client applications are defined here.
//
//
//---------------------------------------------------------------------------
//
// quit - allowed only from Interface Command System(ICS)
// remote client/server
//
*/

/*!
  \brief QUIT command - quit the client session
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

int
cmd_quit(char *args, MsgType msgtype, char *reply)
{

  if (msgtype == EXEC) {
    client.KeepGoing=0;
    sprintf(reply,"QUIT mmcService=DISABLED MODE=OFFLINE");
  }
  else {
    strcpy(reply,"QUIT Cannot execute quit command - operation not allowed except as EXEC:");
    return CMD_ERR;
  }
  
  return CMD_OK;
}

/* ---------------------------------------------------------------------------
//
// ping - communication handshaking request
//
*/

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

int
cmd_ping(char *args, MsgType msgtype, char *reply)
{
  strcpy(reply,"PONG");
  return CMD_OK;
}

/* ---------------------------------------------------------------------------
//
// pong - communication handshaking acknowledge
//
*/

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

int
cmd_pong(char *args, MsgType msgtype, char *reply)
{
  if (client.isVerbose)
    printf("PONG received\n");
  return CMD_NOOP;
}

/* ---------------------------------------------------------------------------
//
// version - report application version and compilation info
//
*/

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

int
cmd_version(char *args, MsgType msgtype, char *reply)
{
  char cmd_instruction[10];
  char dummy[PAGE_SIZE];
  char who_selected[24];

  strcpy(who_selected,cmdtab[commandID].cmd);
  StrUpper(who_selected);

  GetArg(args,1,cmd_instruction);
  StrUpper(cmd_instruction);

  if(!strcasecmp(cmd_instruction,"AGW")) {
    ierr=agwcu("localhost",0,"version",dummy);
    if(ierr!=0) {
      sprintf(reply,"%s", dummy);
      return CMD_ERR;
    }
    sprintf(reply,"%s agwServer %s",who_selected,&dummy[6]);

  } else {
    sprintf(reply,"%s mmcService Version=%s CompileDate=%s CompileTime=%s",
	    who_selected,APP_VERSION,APP_COMPDATE,APP_COMPTIME);
  }
  return CMD_OK;
}

/* ---------------------------------------------------------------------------
//
// verbose - toggle verbose console output
//
*/

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

int
cmd_verbose(char *args, MsgType msgtype, char *reply)
{
  if (client.isVerbose) {
    client.isVerbose = 0;
    sprintf(reply,"VERBOSE VERBOSE=DISABLED, 'verbose mode disabled");
  }
  else {
    client.isVerbose = 1;
    sprintf(reply,"VERBOSE VERBOSE=ENABLED, 'verbose mode enabled");
  }
  return CMD_OK;
}

/* ---------------------------------------------------------------------------
// 
// debug - toggle debugging output (super-verbose mode)
//
*/

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

  \sa cmd_verbose
*/

int
cmd_debug(char *args, MsgType msgtype, char *reply)
{
  if (client.Debug) {
    client.Debug = 0;
    sprintf(reply,"DEBUG DEBUG=DISABLED, 'debugging output disabled'");
  }
  else {
    client.Debug = 1;
    sprintf(reply,"DEBUG DEBUG=ENABLED, 'debugging output enabled'");
  }
  return CMD_OK;
}

/* ---------------------------------------------------------------------------
//
// info - report application runtime configuration information
//
*/

/*!
  \brief INFO command - report client application runtime information
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on successful creation of the info report, #CMD_ERR
  if errors encountered.

  \par Usage:
  info (alias: status)

  Creates a summary report of the current client application's runtime
  configuration as an IMPv2-compliant message string in which the
  runtime parameters are reported as keyword=value pairs.

  The format of cmd_info should be tailored specifically for the
  particular client application.  If a client controls specific
  instrument or interface functions, the state of those functions should
  be reported in the info string, making it an omnibus "what is your
  status" command.

*/

int
cmd_info(char *args, MsgType msgtype, char *reply)
{
  int i;
  int pnum;
  int len;
  char temp[80];
  char dummy[PAGE_SIZE];
  char who_selected[24];

  strcpy(who_selected,cmdtab[commandID].cmd);
  StrUpper(who_selected);

  // Start with the node ID, and host info

  sprintf(reply,"%s HostID=%s HostAddr=%s:%d ",who_selected,
	  client.ID, client.Host, client.Port);

  // If configured as an ISIS client, report this and the ISIS host:port info,
  // otherwise if standalone, report that, and the host:port of the last
  // remote host to send us something, if known.

  if (client.useISIS) {
    sprintf(reply,"%s Mode=ISISClient ISISID=%s ISISHost=%s:%d ",reply,
	    client.isisID,client.isisHost,client.isisPort);
  }
  else {
    if (strlen(client.remHost)>0)
      sprintf(reply,"%s Mode=STANDALONE RemHost=%s:%d ",reply,
	      client.remHost,client.remPort);
    else
      strcat(reply," Mode=STANDALONE ");
  }
  sprintf(reply,"%s %s %s",reply,
  ((client.isVerbose) ? "Verbose" : "Concise"),
  ((client.Debug) ? "+DEBUG" : "-DEBUG"));
	 
  // Report the application's runtime config info as required

  sprintf(reply,"%s rcFile='%s'",reply,client.rcFile);

  return CMD_OK;

}

/* ---------------------------------------------------------------------------
//
// help - print a list of available commands on the client console
//
*/

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

int
cmd_help(char *args, MsgType msgtype, char *reply)
{
  int i, icmd, found;
  int ls;
  char helper[32];

  GetArg(args,1,argbuf);
  GetArg(args,1,helper);
  StrUpper(helper);

  if(!strcasecmp(helper,"MECH")) {
      sprintf(reply,"HELP HELP=MECH ieb hatch calib agw agwy agwx agwfoc agwfilt gprobe gpoffset minsert slitmask mselect dichroic r/bcolttf# r/bgrating r/bgrtilt# r/bshutter r/bfilter r/bcamfoc abort moverel moveabs mstatus istatus pstatus");
   
  } else if(!strcasecmp(helper,"SYSTEM")) {
      sprintf(reply,"HELP HELP=SYSTEM lamp irlaser vislaser ieb close debug open reset verbose version ping pong info mstatus istatus pstatus startup stow shutdown wake sleep saveconfig panic");
   
  } else if (strlen(args)>0) {  // we are being asked for help on a specific command
    found = 0;
    for (i=0; i<NumCommands; i++) {
      if (strcasecmp(cmdtab[i].cmd,argbuf)==0) {
	found++;
	icmd = i;
	break;
      }
    }
    if (found > 0) {
     //sprintf(reply,"HELP HELP=%s  Usage:%s\n",cmdtab[i].cmd,cmdtab[i].usage);
      sprintf(reply,"HELP HELP=%s  Usage: %s",makeUpper(cmdtab[i].cmd),cmdtab[i].usage);
    }

    if(found==0) {
      sprintf(reply,"HELP HELP=FAULT Invalid help command '%s'",argbuf);
      return CMD_ERR;
    }

  } else {
      //printf("Unknown Command '%s' (type 'help' to list all commands)\n");
    sprintf(reply,"HELP HELP=FAULT Invalid request '%s', valid request, help [cmd_name|mech|system]'",argbuf);
      return CMD_ERR;
  }

  return CMD_OK;
    
}

/* ---------------------------------------------------------------------------
//
// mstatus - client MechanismSTATUS command
//
*/

/*!
  \brief MSTATUS command - report client mechanism status
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

*/
int
cmd_mstatus(char *args, MsgType msgtype, char *reply)
{
  int device;
  int len;
  char who_selected[24];
  char dummy[PAGE_SIZE];
  char cmd_selected[24];
  char EncBits[24];


  if(strlen(args)<=0) {
    sprintf(reply,"%s Invalid use of command, Usage MSTATUS mechanismName",who_selected);
  }

  GetArg(args,1,argbuf);

  strcpy(who_selected,cmdtab[commandID].cmd);
  StrUpper(who_selected);

  device=getMechanismID(argbuf,dummy); // Get mechanism device ID
  sprintf(reply,"%s %s",who_selected,dummy);
  if(device==-1) return CMD_ERR;

  ierr=mlcQuery(device,1,dummy); // Check if the microLynx is ON
  if(ierr!=0) {
    sprintf(reply,"%s MicroLynx for %s is off",who_selected,shm_addr->MODS.who[device]);
    return CMD_ERR;
  }

  // clear out the reply buffer, then build it up piece-by-piece

  memset(reply,0,sizeof(reply));
  rawCommand(device,"PRINT POS",dummy);
  shm_addr->MODS.pos[device]=atof(dummy);

  rawCommand(device,"PRINT IO 20,\" EXTENED=\",IO 30",dummy);
  sprintf(EncBits,"%s",dummy);
  sprintf(reply,"%s %s POS %0.3f ENCBITS %s IP:PORT %s MLID ml%d", 
	  who_selected,shm_addr->MODS.who[device],
	  shm_addr->MODS.pos[device],EncBits,
	  shm_addr->MODS.commport[device].Port,device+1);

  return CMD_OK;
}

/* ---------------------------------------------------------------------------
//
// istatus - Query General Instrument Status
//
*/

/*!
  \brief ISTATUS command - report general status
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage: ISTATUS

  \par Description:
  Returns the current states of all instrument mechanisms, lamps, and 
  lasers providing a comprehensive snapshot of the current instrument
  configuration in the form of FITS-style keyword=value pairs.

  The ICs (red and blue) will issue ISTATUS queries to the IE before each
  exposure to create the instrument-related FITS header entries for that
  exposure, and user interface programs will issue ISTATUS commands to
  update their status displays.

  The main purpose of ISTATUS is to report the positions of all instrument
  mechanisms(filter wheels, focus positions, etc.). These define the most
  important instrument configuration parameters vis-à-vis observing that
  must appear in each image FITS headers.

  ISTATUS returns only the most relevant subset of instrument power state
  keywords,namely the global power state of the IEBs, LLB, AGW controllers,
  and HEBs, and the power states of the calibration lamps (CALLAMPS=) and
  the IMCS IR laser. For detailed power status, use the PSTATUS command.

*/
int
cmd_istatus(char *args, MsgType msgtype, char *reply)
{
  char who_selected[24];
  char Port[10];
  char dummy[PAGE_SIZE];
  char dummy1[PAGE_SIZE];
  char get_buff[PAGE_SIZE];
  char msg[PAGE_SIZE];
  char ttfKeeper[PAGE_SIZE];
  char cmd_instruction[PAGE_SIZE];

  short pressureTemps[10];
  float glycolSupplyPressure;
  float glycolReturnPressure;
  float glycolSupplyTemperature;
  float glycolReturnTemperature;
  float utilBoxAirTemperature;
  float outsideAirTemperature;
  float agwHeatSinkTemperature;
  
  short hebTemps[2];
  float redHEBTemperature;
  float redDewarTemperature;
  float blueHEBTemperature;
  float blueDewarTemperature;

  int agwfiltnum;
  int device;
  int device1;
  int device2;
  int device3;
  int loopcnt;
  int ierr;
  int icnter;
  int retuneMODS;
  float ttffoc;
  float ttfa;
  float ttfb;
  float ttfc;
  float umPerRev; 

  memset(dummy,0,sizeof(dummy));
  memset(cmd_instruction,0,sizeof(cmd_instruction));

  GetArg(args,1,cmd_instruction);
  StrUpper(cmd_instruction);

  strcpy(who_selected,cmdtab[commandID].cmd);
  StrUpper(who_selected);

  sprintf(who_selected,"ISTATUS");

  // WAGO IDs we need

  llbID=getWagoID("llb",dummy); // Get Lamp/Laser Box (LLB) ID
  if(llbID==-1) {
    sprintf(reply,"%s %s",who_selected,dummy);
    return CMD_ERR;
  }

  utilID=getWagoID("util",dummy); // Instrument Utility Box (IUB) ID
  if(utilID==-1) {
    sprintf(reply,"%s %s",who_selected,dummy);
    return CMD_ERR;
  }

  rhebID=getWagoID("rheb",dummy); // Red channel Head Electronicx Box (HEB) ID
  if(rhebID==-1) {
    sprintf(reply,"%s %s",who_selected,dummy);
    return CMD_ERR;
  }

  bhebID=getWagoID("bheb",dummy); // Blue channel Head Electronicx Box (HEB) ID
  if(bhebID==-1) {
    sprintf(reply,"%s %s",who_selected,dummy);
    return CMD_ERR;
  }

  // Interrogate the utility box
  
  KeyCommand("UTIL", dummy);

  if (!strcasecmp(cmd_instruction,"?")) {
     sprintf(reply,"Usage: %s [RESET] - RESET will take some time to complete",
	     who_selected);
     return CMD_OK;
  }

  sprintf(Port,"%d",shm_addr->MODS.modsPorts[2]);

  // Red and Blue channel Instrument Electronics Boxes (IEBs)
  
  if(!strcasecmp(cmd_instruction,"BLUE") || !strcasecmp(cmd_instruction,"RED") || 
     !strcasecmp(cmd_instruction,"R") || !strcasecmp(cmd_instruction,"B")) {

    if(!strcasecmp(cmd_instruction,"BLUE") || !strcasecmp(cmd_instruction,"B")) {
      device1=getMechanismID("bcolttfa",dummy); // TTFA
      device2=getMechanismID("bcolttfb",dummy); // TTFB
      device3=getMechanismID("bcolttfc",dummy); // TTFC
      sprintf(ttfKeeper,"CHANNEL=BLUE");

    } else if(!strcasecmp(cmd_instruction,"RED") || !strcasecmp(cmd_instruction,"R")) {
      device1=getMechanismID("rcolttfa",dummy); // TTFA
      device2=getMechanismID("rcolttfb",dummy); // TTFB
      device3=getMechanismID("rcolttfc",dummy); // TTFC
      sprintf(ttfKeeper,"CHANNEL=RED");

    }
    
    if(!shm_addr->MODS.host[device1]) {
      ttfa=0.0;
      sprintf(ttfKeeper,"%s COLTTFA=NOCOMM",ttfKeeper);
    } else {
      ttfa=shm_addr->MODS.pos[device1]*shm_addr->MODS.convf[device1];
      sprintf(ttfKeeper,"%s COLTTFA=%0.0f",ttfKeeper,ttfa);
    }

    if(!shm_addr->MODS.host[device2]) {
      ttfb=0.0;
      sprintf(ttfKeeper,"%s COLTTFB=NOCOMM",ttfKeeper);
    }  else {
      ttfb=shm_addr->MODS.pos[device2]*shm_addr->MODS.convf[device2];
      sprintf(ttfKeeper,"%s COLTTFB=%0.0f",ttfKeeper,ttfb);
    }

    if(!shm_addr->MODS.host[device3]) {
      ttfc=0.0;
      sprintf(ttfKeeper,"%s COLTTFC=NOCOMM",ttfKeeper);
    } else {
      ttfc=shm_addr->MODS.pos[device3]*shm_addr->MODS.convf[device3];
      sprintf(ttfKeeper,"%s COLTTFC=%0.0f",ttfKeeper,ttfc);
    }

    ttffoc = (ttfa+ttfb+ttfc)/3.0;
    sprintf(ttfKeeper,"%s COLFOCUS=%0.0f",ttfKeeper,ttffoc);
    
    if(!strncasecmp(cmd_instruction,"R",1)) 
      device=getMechanismID("rgrating",dummy);
    else 
      device=getMechanismID("bgrating",dummy);

    if(!shm_addr->MODS.host[device])
      sprintf(ttfKeeper,"%s GRATING=NOCOMM GRATNAME='None' GRATINFO='None'",ttfKeeper);
    else
      sprintf(ttfKeeper,"%s GRATING=%d GRATNAME='%s' GRATINFO='%s'",ttfKeeper,
	      atoi(shm_addr->MODS.state_word[device]),
	      (!strcasecmp(cmd_instruction,"RED") || !strcasecmp(cmd_instruction,"R")) ? \
	      shm_addr->MODS.rgrating[atoi(shm_addr->MODS.state_word[device])] :\
	      shm_addr->MODS.bgrating[atoi(shm_addr->MODS.state_word[device])],
	      (!strcasecmp(cmd_instruction,"RED") || !strcasecmp(cmd_instruction,"R")) ? \
	      shm_addr->MODS.rgratInfo[atoi(shm_addr->MODS.state_word[device])] :\
	      shm_addr->MODS.bgratInfo[atoi(shm_addr->MODS.state_word[device])]);

    if(!strncasecmp(cmd_instruction,"R",1)) 
      device=getMechanismID("rgrtilt1",dummy);
    else 
      device=getMechanismID("bgrtilt1",dummy);

    if(!shm_addr->MODS.host[device])
      sprintf(ttfKeeper,"%s GRATTILT=NOCOMM",ttfKeeper);
    else
      sprintf(ttfKeeper,"%s GRATTILT=%d",ttfKeeper,
	      (int)(shm_addr->MODS.pos[device]*shm_addr->MODS.convf[device]));
    
    if(!strncasecmp(cmd_instruction,"R",1)) 
      device=getMechanismID("rcamfoc",dummy);
    else device=getMechanismID("bcamfoc",dummy);

    if(!shm_addr->MODS.host[device])
      sprintf(ttfKeeper,"%s CAMFOCUS=NOCOMM",ttfKeeper);
    else
      sprintf(ttfKeeper,"%s CAMFOCUS=%0.0f",ttfKeeper,
	      shm_addr->MODS.pos[device]*shm_addr->MODS.convf[device]);

    if(!strncasecmp(cmd_instruction,"R",1)) 
      device=getMechanismID("rfilter",dummy);
    else device=getMechanismID("bfilter",dummy);

    if(!shm_addr->MODS.host[device]) {
      sprintf(ttfKeeper,"%s FILTER=NOCOMM FILTNAME='None' FILTINFO='None'",
	      ttfKeeper);
    } else {
      if(shm_addr->MODS.pos[device]<=0) {
	sprintf(ttfKeeper,"%s FILTER=UNKNOWN FILTNAME='None' FILTINFO='None'",
		ttfKeeper);

      } else {
	sprintf(ttfKeeper,"%s FILTER=%0.0f",ttfKeeper,
		shm_addr->MODS.pos[device]);
	ierr=shm_addr->MODS.pos[device];
	if(!strncasecmp(cmd_instruction,"R",1)) 
	  sprintf(ttfKeeper,"%s FILTNAME='%s' FILTINFO='%s'",ttfKeeper,
		  shm_addr->MODS.rcamfilters[ierr],shm_addr->MODS.rcamfiltInfo[ierr]);
	else
	  sprintf(ttfKeeper,"%s FILTNAME='%s' FILTINFO='%s'",ttfKeeper,
		  shm_addr->MODS.bcamfilters[ierr],shm_addr->MODS.bcamfiltInfo[ierr]);
      }
    }
  } else {
    
    // Blue channel Instrument Electronics Box

    device=getMechanismID("bcolttfa",dummy); // TTFA
    if(!shm_addr->MODS.host[device]) {
      ttfa=0.0;
      sprintf(ttfKeeper,"BCOLTTFA=NOCOMM");
    } else {
      ttfa=shm_addr->MODS.pos[device]*shm_addr->MODS.convf[device];
      sprintf(ttfKeeper,"BCOLTTFA=%0.0f",ttfa);
    }
    
    device=getMechanismID("bcolttfb",dummy); // TTFB
    if(!shm_addr->MODS.host[device]) {
      ttfb=0.0;
      sprintf(ttfKeeper,"%s BCOLTTFB=NOCOMM",ttfKeeper);
    }  else {
      ttfb=shm_addr->MODS.pos[device]*shm_addr->MODS.convf[device];
      sprintf(ttfKeeper,"%s BCOLTTFB=%0.0f",ttfKeeper,ttfb);
    }
    
    device=getMechanismID("bcolttfc",dummy); // TTFC
    if(!shm_addr->MODS.host[device]) {
      ttfc=0.0;
      sprintf(ttfKeeper,"%s BCOLTTFC=NOCOMM",ttfKeeper);
    } else {
      ttfc=shm_addr->MODS.pos[device]*shm_addr->MODS.convf[device];
      sprintf(ttfKeeper,"%s BCOLTTFC=%0.0f",ttfKeeper,ttfc);
    }
    
    ttffoc = (ttfa+ttfb+ttfc)/3.0;
    sprintf(ttfKeeper,"%s BCOLFOC=%0.0f",ttfKeeper,ttffoc);
    
    device=getMechanismID("bgrating",dummy);
    
    if(!shm_addr->MODS.host[device])
      sprintf(ttfKeeper,"%s BGRATING=NOCOMM BGRATID=NOCOMM",ttfKeeper);
    else
      sprintf(ttfKeeper,"%s BGRATING=%d BGRATID='%s'",ttfKeeper,
	      atoi(shm_addr->MODS.state_word[device]),
    	      shm_addr->MODS.bgrating[atoi(shm_addr->MODS.state_word[device])]);

    device=getMechanismID("bgrtilt1",dummy);
    if(!shm_addr->MODS.host[device])
      sprintf(ttfKeeper,"%s BGRTILT1=NOCOMM",ttfKeeper);
    else
      sprintf(ttfKeeper,"%s BGRTILT1=%d",ttfKeeper,
	      (int)(shm_addr->MODS.pos[device]*shm_addr->MODS.convf[device]));
    
    device=getMechanismID("bcamfoc",dummy);
    if(!shm_addr->MODS.host[device])
      sprintf(ttfKeeper,"%s BCAMFOC=NOCOMM",ttfKeeper);
    else
      sprintf(ttfKeeper,"%s BCAMFOC=%0.0f",ttfKeeper,
	      shm_addr->MODS.pos[device]*shm_addr->MODS.convf[device]);
    
    device=getMechanismID("bfilter",dummy);
    if(!shm_addr->MODS.host[device]) {
      sprintf(ttfKeeper,"%s BFILTER=NOCOMM BFILTID'NOCOMM'",ttfKeeper);
    } else {
      if(shm_addr->MODS.pos[device]<=0) {
	sprintf(ttfKeeper,"%s BFILTER=UNKNOWN, BFILTID='None'",ttfKeeper);
      } else {
	sprintf(ttfKeeper,"%s BFILTER=%0.0f BFILTID='%s'",ttfKeeper,
		shm_addr->MODS.pos[device],
		shm_addr->MODS.bcamfilters[(int)shm_addr->MODS.pos[device]]);
      }
    }

    // Red channel Instrument Electronics Box

    device=getMechanismID("rcolttfa",dummy); // TTFA
    if(!shm_addr->MODS.host[device]) {
      ttfa=0.0;
      sprintf(ttfKeeper,"%s RCOLTTFA=NOCOMM",ttfKeeper);
    } else {
      ttfa=shm_addr->MODS.pos[device]*shm_addr->MODS.convf[device];
      sprintf(ttfKeeper,"%s RCOLTTFA=%0.0f",ttfKeeper,ttfa);
    }
    
    device=getMechanismID("rcolttfb",dummy); // TTFB
    if(!shm_addr->MODS.host[device]) {
      ttfb=0.0;
      sprintf(ttfKeeper,"%s RCOLTTFA=NOCOMM",ttfKeeper);
    }  else {
      ttfb=shm_addr->MODS.pos[device]*shm_addr->MODS.convf[device];
      sprintf(ttfKeeper,"%s RCOLTTFB=%0.0f",ttfKeeper,ttfb);
    }
    
    device=getMechanismID("rcolttfc",dummy); // TTFC
    if(!shm_addr->MODS.host[device]) {
      ttfc=0.0;
      sprintf(ttfKeeper,"%s RCOLTTFC=NOCOMM",ttfKeeper);
    } else {
      ttfc=shm_addr->MODS.pos[device]*shm_addr->MODS.convf[device];
      sprintf(ttfKeeper,"%s RCOLTTFC=%0.0f",ttfKeeper,ttfc);
    }
    
    ttffoc = (ttfa+ttfb+ttfc)/3.0;
    sprintf(ttfKeeper,"%s RCOLFOC=%0.0f",ttfKeeper,ttffoc);
    
    device=getMechanismID("rgrating",dummy);
    
    if(!shm_addr->MODS.host[device])
      sprintf(ttfKeeper,"%s RGRATING=NOCOMM RGRATID=NOCOMM",ttfKeeper);
    else 
      sprintf(ttfKeeper,"%s RGRATING=%d RGRATID='%s'",ttfKeeper,
	      atoi(shm_addr->MODS.state_word[device]),
	      shm_addr->MODS.rgrating[atoi(shm_addr->MODS.state_word[device])]);
    
    device=getMechanismID("rgrtilt1",dummy);
    if(!shm_addr->MODS.host[device]) 
      sprintf(ttfKeeper,"%s RGRTILT1=NOCOMM",ttfKeeper);
    else
      sprintf(ttfKeeper,"%s RGRTILT1=%d",ttfKeeper,
	      (int)(shm_addr->MODS.pos[device]*shm_addr->MODS.convf[device]));
    
    device=getMechanismID("rcamfoc",dummy);
    if(!shm_addr->MODS.host[device])
      sprintf(ttfKeeper,"%s RCAMFOC=NOCOMM",ttfKeeper);
    else
      sprintf(ttfKeeper,"%s RCAMFOC=%0.0f",ttfKeeper,
	      shm_addr->MODS.pos[device]*shm_addr->MODS.convf[device]);
    
    device=getMechanismID("rfilter",dummy);
    if(!shm_addr->MODS.host[device]) {
      sprintf(ttfKeeper,"%s RFILTER=NOCOMM RFILTID=NOCOMM",ttfKeeper);
    } else {
      if(shm_addr->MODS.pos[device]<=0) {
	sprintf(ttfKeeper,"%s RFILTER=UNKNOWN RFILTID=NOCOMM",ttfKeeper);
      } else {
	sprintf(ttfKeeper,"%s RFILTER=%0.0f RFILTID='%s'",ttfKeeper,
		shm_addr->MODS.pos[device],
		shm_addr->MODS.rcamfilters[(int)shm_addr->MODS.pos[device]]);
      }
    }
  }

  // Focal plane mechanisms (common to red and blue channels)

  // Dark Hatch
  
  device = getMechanismID("hatch",dummy);
  
  if(!shm_addr->MODS.host[device])
    sprintf(reply,"%s HATCH=NOCOMM", who_selected);
  else {
    if(strlen(shm_addr->MODS.state_word[device])<=0)
      sprintf(reply,"%s HATCH=FAULT", who_selected);
    else
      sprintf(reply,"%s HATCH=%s",
	      who_selected,shm_addr->MODS.state_word[device]);
  }

  // Calibration tower
  
  device=getMechanismID("calib",dummy);
  
  if(!shm_addr->MODS.host[device])
    sprintf(reply,"%s CALIB=NOCOMM", reply, who_selected);
  else 
    if(strlen(shm_addr->MODS.state_word[device])<=0)
      sprintf(reply,"%s CALIB=UNKNOWN",reply);
    else
      sprintf(reply,"%s CALIB=%s",reply,
	      shm_addr->MODS.state_word[device]);

  // Calibration Lamps:  AR|XE|NE|HG|KR|QTH6V|QTH1|QTH2|VFLAT

  ierr=0;
  sprintf(reply,"%s CALLAMPS='",reply);
  if(shm_addr->MODS.lamps.lamp_state[0]) {sprintf(reply,"%sAR ",reply); ierr++;}
  if(shm_addr->MODS.lamps.lamp_state[1]) {sprintf(reply,"%sXE ",reply); ierr++;}
  if(shm_addr->MODS.lamps.lamp_state[2]) {sprintf(reply,"%sNE ",reply); ierr++;}
  if(shm_addr->MODS.lamps.lamp_state[3]) {sprintf(reply,"%sHG ",reply); ierr++;}
  if(shm_addr->MODS.lamps.lamp_state[4]) {sprintf(reply,"%sKR ",reply); ierr++;}
  if(shm_addr->MODS.lamps.lamp_state[5]) {sprintf(reply,"%sQTH6V ",reply); ierr++;}
  if(shm_addr->MODS.lamps.lamp_state[6]) {sprintf(reply,"%sQTH1 ",reply); ierr++;}
  if(shm_addr->MODS.lamps.lamp_state[7]) {sprintf(reply,"%sQTH2 ",reply); ierr++;}
  if(shm_addr->MODS.lamps.lamp_state[8]) {sprintf(reply,"%sVFLAT ",reply); ierr++;}

  if(!ierr)
    sprintf(reply,"%sNone' VFLAT=%0.1f",reply,shm_addr->MODS.vflat_power);
  else
    sprintf(reply,"%s' VFLAT=%0.1f",reply,shm_addr->MODS.vflat_power);

  // Slitmask (select and insert/retract)
  
  device=getMechanismID("mselect",dummy);
  
  if(cmd_instruction[0]=='R' || cmd_instruction[0]=='B') {
    if(!shm_addr->MODS.host[device])

      sprintf(reply,"%s SLITMASK=NOCOMM MASKPOS=NOCOMM MASKNAME='NOCOMM' MASKINFO='None'",reply);
    else {
      sprintf(reply,"%s SLITMASK=%d MASKPOS=%s MASKNAME='%s' MASKINFO='%s'",reply,
	      shm_addr->MODS.active_smask,
	      (shm_addr->MODS.active_smask<=0 ?	"UNKNOWN" : shm_addr->MODS.maskpos),
	      (shm_addr->MODS.active_smask<=0 ?	"UNKNOWN" : shm_addr->MODS.slitmaskName[shm_addr->MODS.active_smask]),
	      shm_addr->MODS.slitmaskInfo[shm_addr->MODS.active_smask]);
    }
  } else {
    if(!shm_addr->MODS.host[device])
      sprintf(reply,"%s SLITMASK=NOCOMM MASKPOS=NOCOMM",reply);
    else {
      sprintf(reply,"%s SLITMASK=%d MASKPOS=%s",reply,
	      shm_addr->MODS.active_smask,
	      (shm_addr->MODS.active_smask<=0 ?	"UNKNOWN" : shm_addr->MODS.maskpos),
	      (shm_addr->MODS.active_smask<=0 ?	"UNKNOWN" : shm_addr->MODS.slitmaskName[shm_addr->MODS.active_smask]));
    }
  }

  MilliSleep(100);
  memset(get_buff,0,sizeof(get_buff));
  ierr=agwcu("localhost",0,"getxy", get_buff);
  sprintf(reply,"%s %s",reply, &get_buff[6]);

  MilliSleep(100);
  memset(get_buff,0,sizeof(get_buff));
  ierr=agwcu("localhost",0,"getfilter ",get_buff);
  if(cmd_instruction[0]=='R' || cmd_instruction[0]=='B') {
    agwfiltnum=atoi(&get_buff[14]);
    sprintf(reply,"%s %s AGWFINFO='%s'", reply, &get_buff[6],
  	    shm_addr->MODS.agwfiltInfo[agwfiltnum]);
  } else
    sprintf(reply,"%s %s", reply, &get_buff[6]);

  // Dichroic beam splitter turret
  
  device=getMechanismID("dichroic",dummy);

  if(cmd_instruction[0]=='R' || cmd_instruction[0]=='B') {
    if(!shm_addr->MODS.host[device])
      sprintf(reply,"%s DICHROIC=NOCOMM DICHNAME='None' DICHINFO='None'",reply);
    else {
      if(shm_addr->MODS.pos[device] < 1.0 || 
	 shm_addr->MODS.pos[device] > 3.0)  
	sprintf(reply,"%s DICHROIC=UNKNOWN DICHNAME='None' DICHINFO='None'",reply);
      else {
	sprintf(reply,"%s DICHROIC=%0.0f DICHNAME='%s' DICHINFO='%s'",reply,
		shm_addr->MODS.pos[device],
		shm_addr->MODS.dichroicName[(int)shm_addr->MODS.pos[device]],
		shm_addr->MODS.dichroicInfo[(int)shm_addr->MODS.pos[device]]);
      }
    }
  } else {
    if(!shm_addr->MODS.host[device])
      sprintf(reply,"%s DICHROIC=NOCOMM DICHNAME=NOCOMM",reply);
    else {
      if(shm_addr->MODS.pos[device] < 1.0 ||shm_addr->MODS.pos[device] > 3.0)
	sprintf(reply,"%s DICHROIC=UNKNOWN DICHNAME=UNKNOWN",reply);
      else
	sprintf(reply,"%s DICHROIC=%0.0f DICHNAME='%s'",reply,
		shm_addr->MODS.pos[device],shm_addr->MODS.dichroicName[device]);
    }
  }

  // IMCS IR Laser Status
  
  ierr = wagoSetGet(0,shm_addr->MODS.WAGOIP[llbID],1,LLBONOFF,devOnOff,1);
  if(ierr==-1) {
    sprintf(reply,"%s IRLASER=OFF",reply);
  } else {
    sprintf(reply,"%s IRLASER=%s IRBEAM=%s IRPSET=%0.1f IRPOUT=%0.1f IRTSET=%0.1f IRTEMP=%0.1f",reply,
	    (shm_addr->MODS.lasers.irlaser_state==1 ? "ON" : "OFF"),
	    (shm_addr->MODS.lasers.irbeam_state==1 ? "ENABLED" : "DISABLED"),
	    shm_addr->MODS.lasers.irlaser_setpoint,
	    shm_addr->MODS.lasers.irlaser_power,
	    shm_addr->MODS.lasers.irlaser_tempSet,
	    shm_addr->MODS.lasers.irlaser_temp);
  }

  sprintf(reply,"%s %s",reply,ttfKeeper);

  //--------------------------------------
  //
  // Temperature and Pressure Sensors
  //

  // IEB temperatures (red and blue)
  
  KeyCommand("ieb r temp", dummy);
  sprintf(reply,"%s %s",reply,&dummy[11]);

  KeyCommand("ieb b temp", dummy);
  sprintf(reply,"%s %s",reply,&dummy[11]);

  // IUB pressure and temperature sensors
  
  ierr = wagoSetGet(0,shm_addr->MODS.WAGOIP[utilID],1,0,pressureTemps,10);

  glycolSupplyPressure = (float)pressureTemps[0]/327.64;
  glycolReturnPressure = (float)pressureTemps[1]/327.64;
  glycolSupplyTemperature = ptRTD2C(pressureTemps[4]);
  glycolReturnTemperature = ptRTD2C(pressureTemps[5]);
  agwHeatSinkTemperature = ptRTD2C(pressureTemps[6]);
  utilBoxAirTemperature = ptRTD2C(pressureTemps[7]);
  outsideAirTemperature = ptRTD2C(pressureTemps[8]);

  shm_addr->MODS.glycolSupplyPressure = glycolSupplyPressure;
  shm_addr->MODS.glycolSupplyTemperature = glycolSupplyTemperature;
  shm_addr->MODS.glycolReturnPressure = glycolReturnPressure;
  shm_addr->MODS.glycolReturnTemperature = glycolReturnTemperature;
  shm_addr->MODS.utilBoxAirTemperature = utilBoxAirTemperature;
  shm_addr->MODS.outsideAirTemperature = outsideAirTemperature;
  shm_addr->MODS.agwHeatSinkTemperature = agwHeatSinkTemperature;
  
  (glycolSupplyPressure>=850.0 ? sprintf(reply,"%s GSPRES=NOCOMM",reply) : 
   sprintf(reply,"%s GSPRES=%0.1f",reply,glycolSupplyPressure));

  (glycolSupplyTemperature>=850.0 ? sprintf(reply,"%s GSTEMP=NOCOMM",reply) : 
   sprintf(reply,"%s GSTEMP=%0.1f",reply,glycolSupplyTemperature));

  (glycolReturnPressure>=850.0 ? sprintf(reply,"%s GRPRES=NOCOMM",reply) : 
   sprintf(reply,"%s GRPRES=%0.1f",reply, glycolReturnPressure));

  (glycolReturnTemperature>=850.0 ? sprintf(reply,"%s GRTEMP=NOCOMM",reply) : 
   sprintf(reply,"%s GRTEMP=%0.1f",reply,glycolReturnTemperature));

  (utilBoxAirTemperature>=850.0 ? sprintf(reply,"%s IUBTAIR=NOCOMM",reply) :
   sprintf(reply,"%s IUBTAIR=%0.1f",reply,utilBoxAirTemperature));

  (outsideAirTemperature>=850.0 ? sprintf(reply,"%s AMBTEMP=NOCOMM",reply) : 
   sprintf(reply,"%s AMBTEMP=%0.1f",reply,outsideAirTemperature));

  (agwHeatSinkTemperature>=850.0 ? sprintf(reply,"%s AGHSTEMP=NOCOMM",reply) : 
   sprintf(reply,"%s AGHSTEMP=%0.1f",reply,agwHeatSinkTemperature));

  // WAGO HEB temperature sensors are on WAGO register addr 5 (4-channel RTD)

  // Red HEB
  
  ierr = wagoSetGet(0,shm_addr->MODS.WAGOIP[rhebID],1,4,hebTemps,2);
  
  redHEBTemperature = ptRTD2C(hebTemps[0]);
  redDewarTemperature = ptRTD2C(hebTemps[1]);
  shm_addr->MODS.redHEBTemperature = redHEBTemperature;
  shm_addr->MODS.redDewarTemperature = redDewarTemperature;

  (redHEBTemperature>=850.0 ? sprintf(reply,"%s RHEBTEMP=NOCOMM",reply) : 
   sprintf(reply,"%s RHEBTEMP=%0.1f",reply,redHEBTemperature));

  (redDewarTemperature>=850.0 ? sprintf(reply,"%s RDEWTEMP=NOCOMM",reply) : 
   sprintf(reply,"%s RDEWTEMP=%0.1f",reply,redDewarTemperature));

  // Blue HEB
  
  ierr=wagoSetGet(0,shm_addr->MODS.WAGOIP[bhebID],1,4,hebTemps,2);
  
  blueHEBTemperature = ptRTD2C(hebTemps[0]);
  blueDewarTemperature = ptRTD2C(hebTemps[1]);
  shm_addr->MODS.blueHEBTemperature = blueHEBTemperature;
  shm_addr->MODS.blueDewarTemperature = blueDewarTemperature;

  (blueHEBTemperature>=850.0 ? sprintf(reply,"%s BHEBTEMP=NOCOMM",reply) : 
   sprintf(reply,"%s BHEBTEMP=%0.1f",reply,blueHEBTemperature));

  (blueDewarTemperature>=850.0 ? sprintf(reply,"%s BDEWTEMP=NOCOMM",reply) : 
   sprintf(reply,"%s BDEWTEMP=%0.1f",reply,blueDewarTemperature));

  
  return CMD_OK;
}

/* ---------------------------------------------------------------------------
//
// pstatus - Query the instrument Power Status
//
*/

/*!
  \brief PSTATUS command - report power status
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \param return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage: PSTATUS

  \par Description:
  PSTATUS returns the current instrument power state including the power
  states of all lamps and lasers controlled by the IE. The command response
  is a combination of the status replies returned by the IEB, LLB, and UTIL
  status queries implemented as a single command. It only returns power
  status variables, no other non-power information (e.g.,report if the IR
  laser AC power is on/off, but do not report beam enable/disable, power
  status, laser head temperatures, etc.).

*/
int
cmd_pstatus(char *args, MsgType msgtype, char *reply)
{
  char who_selected[24];
  char dummy[PAGE_SIZE];

  strcpy(who_selected,cmdtab[commandID].cmd);
  StrUpper(who_selected);

  /* IEB 1 */
  ierr=wagoRW(1,"IEBS",0,0,dummy);             // IEB power
  sprintf(reply,"%s %s",who_selected,dummy);
  
  if(!ierr) {
    sprintf(reply,"%s %s",who_selected,dummy);
    wagoRW(1,"MLCS",0,0,dummy);                // MLC[1-16] power
    sprintf(reply,"%s%s",reply,dummy);

  } else if(ierr==1) {
    sprintf(reply,"%s %s",who_selected,dummy);
    wagoRW(1,"MLCS",0,0,dummy);                // MLC[1-16] power
    sprintf(reply,"%s%s",reply,dummy);

  } else if(ierr==-1) {
    sprintf(reply,"%s %s",who_selected,dummy);

  } else {
    sprintf(reply,"%s IEB_R=UNKNOWN ",who_selected);

  }

  /* IEB 2 */
  ierr=wagoRW(2,"IEBS",0,0,dummy);             // IEB power
  if(!ierr) {
    sprintf(reply,"%s %s",reply,dummy);
    wagoRW(2,"MLCS",0,0,dummy);                // MLC[1-16] power
    sprintf(reply,"%s%s",reply,dummy);

  } else if(ierr==1) {
    sprintf(reply,"%s %s",reply,dummy);
    wagoRW(2,"MLCS",0,0,dummy);                // MLC[1-16] power
    sprintf(reply,"%s%s",reply,dummy);

  } else if(ierr==-1) {
    sprintf(reply,"%s %s",reply,dummy);

  } else {
    sprintf(reply,"%s %s",reply,dummy);

  }

  cmd_lamp("status", EXEC, dummy);
  sprintf(reply,"%s %s UTIL=UNAVAIL",reply,&dummy[8]);

  if(shm_addr->MODS.blueIMCS_OnOff)
    sprintf(reply,"%s BIMCS=ON",reply);
  else
    sprintf(reply,"%s BIMCS=OFF",reply);

  if(shm_addr->MODS.redIMCS_OnOff)
    sprintf(reply,"%s RIMCS=ON",reply);
  else
    sprintf(reply,"%s RIMCS=OFF",reply);
  
  return CMD_OK;
}

/* ***************************************************************************
//
// *** CLIENT-SPECIFIC COMMANDS BEGIN HERE ***
//
//---------------------------------------------------------------------------
//
// ports - report info about open comm ports
//
*/

/*!
  \brief PORTS command - report info about open comm ports
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  ports

  Report information about all comm ports connected to the
  client in IMPv2-compliant format.  Shows their status as follows:
  <pre>
  CommPort=/dev/ttyS0 Open
  </pre>

*/

int
cmd_ports(char *args, MsgType msgtype, char *reply)
{
  char dummy[PAGE_SIZE];
  char who_selected[24];

  strcpy(who_selected,cmdtab[commandID].cmd);
  StrUpper(who_selected);

  //  mlcActivePorts(dummy);   // List active comm port(s)
  //  sprintf(reply,"%s %s",who_selected,dummy);
  sprintf(reply,"%s Command not available for all, Try command_NAME [getip]",who_selected,dummy);
  return CMD_OK;
}

/* ---------------------------------------------------------------------------
//
// misc - miscellaneus commands
//
*/

/*!  
  \brief Miscellaneous commands that don't fit anywhere else
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  cmd_misc() is a place to put commands that don't fit anywhere else,
  or that are experimental before committing to their own cmd_xyz() function.

  calmode - put instrument in calibration mode (AGW out, calib tower in, hatch closed)
 
  obsmode - put instrument in observing mode (calib tower out, AGW in, hatch open)

  util [status] [service1 service2... [on|off]] [all off] [glycol] [temp]
   serviceN - name(s) of switched AC power service [e.g.,IEB_B, LLB ...]
     status - report status
     on|off - power service(s) on or off
    all off - power all services off
     glycol - report glycol supply and return temperature and pressure data
       temp - report Utility Box temperature sensor(RTD) date.

  llb - lamp/laser box services

  heb [chan] [status] [archon igpower [on|off]] [temps]
  
  estatus - environmental sensor status summary

*/

int
cmd_misc(char *args, MsgType msgtype, char *reply)
{
  int  i              = 0; // All purpose counter
  int  ierr           = 0; // Mechanism return -1 is an Error 
  int  unit           = 0; // MicroLynx controller ID
  int  device         = 0; // WAGO controller ID
  int  len            = 0; // Length for building the reply
  int  cmdLen         = 0; // Command length
  int  errorLen       = 0; // Length for building error message

  // Instrument Utility Box power relay digital output channel assignments
  
  int  allUtilPower   = 0; // Utility power state ( Utility box )
  int  iebRedPower    = 0; // Red IEB power state ( IEB1 Instrument box )
  int  iebBluePower   = 0; // Blue IEB power state ( IEB2 Instrument box )
  int  llbPower       = 0; // LLB power state ( Lamp/Laser box )
  int  hebRedPower    = 0; // Red HEB power state ( HEB box )
  int  hebRedEnable   = 0; // Red HEB enable/disable state ( HEB box )
  int  hebBluePower   = 0; // Blue HEB power state ( HEB box )
  int  hebBlueEnable  = 0; // Blue HEB enable/disable state ( HEB box )
  int  agwWFSPower    = 0; // AGw WFS power state
  int  agwGuidePower  = 0; // AGw Guider power state
  int  lampLaserPower = 0; // LLB power state ( Lamp/Laser box )

  // WAGO HEB power relay digital output channel assignments

  int allHEBPower = 0;   // HEB WAGO power relay state (red or blue)
  int archonPower = 0;   // archon power state
  int ionGaugePower = 0; // ionization gauge power state
  
  // Instrument Utility Box Circuit Breaker sense (analog input)

  int  allUtilBreaker; // Utility Breaker
  int  iebRedBreaker;
  int  iebBlueBreaker;
  int  hebRedBreaker;
  int  hebBlueBreaker;
  int  agwWFSBreaker;
  int  agwGuideBreaker;
  int  lampLaserBreaker;

  // Status and working variables
  
  int  agwstatus;
  char hardStart[80];
  char temp[PAGE_SIZE];
  char dummy[PAGE_SIZE];
  char dummy1[PAGE_SIZE];
  char statusMsg[PAGE_SIZE];
  char mmcMsg[PAGE_SIZE];
  char who_selected[24];
  char faultWord[8]="OK";
  short devEnable[1];
  char cmd_instruction[PAGE_SIZE];
  char hebChan;
  int hebWAGO;
  char powerOnOff[4];
  
  // Pressures, temperatures, etc.

  // IEB temperature sensors

  short iebTemps[4];  // 4 RTD channels in each box

  float redIEBAirTemperature;       // Red IEB box air temperature
  float redIEBReturnTemperature;    // Red IEB glycol return temperature
  float TrussTopAirTemperature;     // Collimator Truss top air temperature
  float TrussBottomAirTemperature;  // Collimator Truss bottom air temperature

  float blueIEBAirTemperature;       // Blue IEB box air temperatue
  float blueIEBReturnTemperature;    // Blue IEB glycol return temperature
  float TrussTopTemperature;         // Collimator Truss top steel temperature
  float TrussBottomTemperature;      // Collimator Truss bottom steel temperature
  
  // Utility Box sensors

  short pressureTemps[10];  // pressure and RTD sensors in IUB
  
  float glycolSupplyPressure;
  float glycolReturnPressure;
  float glycolSupplyTemperature;
  float glycolReturnTemperature;
  float utilBoxAirTemperature;
  float outsideAirTemperature;
  float agwHeatSinkTemperature;

  // HEB Sensors - still need to add the dewar vacuum pressures sensors [2025-07-20 rwp]
  
  short hebTemps[2];

  float hebAirTemp;
  float dewarTemp;
  float redHEBTemperature;
  float redDewarTemperature;
  float blueHEBTemperature;
  float blueDewarTemperature;
 
  // CALMODE and OBSMODE error and id checking 

  int calibID        = getMechanismID("calib", dummy);
  int hatchID        = getMechanismID("hatch", dummy);
  int agwYID         = getMechanismID("agwy", dummy);
  int agwXID         = getMechanismID("agwx", dummy);
  int agwFID         = getMechanismID("agwfoc", dummy);
  int mechanismError = 0;
  char errorMsg[PAGE_SIZE]; 
  char lampMsg[PAGE_SIZE]; 

  strcpy(who_selected, cmdtab[commandID].cmd);
  StrUpper(who_selected);

  GetArg (args, 1, argbuf);
  StrUpper(argbuf);

  memset(dummy, 0, sizeof(dummy));
  memset(dummy1, 0, sizeof(dummy1));
  memset(errorMsg, 0, sizeof(errorMsg));

  // Calibration Mode Command
  
  if ( strcasecmp(who_selected, "CALMODE") == 0 ) { // Calibration Mode

    if ( shm_addr->MODS.instrMode == 1 ) {
      len = strlen(dummy1);

      if ( mlcBitsBase10(hatchID, "PRINT IO 22,IO 21", dummy) == 2 ) {
	sprintf(statusMsg,"%s Hatch closed, HATCH=CLOSED", who_selected);
	isisStatusMsg(statusMsg);
	sprintf(dummy1,"HATCH=CLOSED ");
	
      } else {
	if ( mechanismError > 0 ) {
	  if ( mlcBitsBase10(hatchID, "PRINT IO 22,IO 21", dummy) == 1 ) {
	    sprintf(statusMsg,"%s Hatch open, HATCH=OPEN", who_selected);
	    isisStatusMsg(statusMsg);
	    sprintf(&dummy1[len],"HATCH=OPEN ");
	  } else {
	    sprintf(statusMsg,"%s HATCH=FAULT", who_selected);
	    isisStatusMsg(statusMsg);
	    sprintf(&dummy1[len],"HATCH=FAULT ");
	  }

	} else {
	  ierr = Hatch(1, who_selected, errorMsg, dummy1);
	  
	  if ( ierr != 0 ) {
	    shm_addr->MODS.instrMode = -1;
	    mechanismError = ierr;
	  }

	}

      }

      len = strlen(dummy1);
      if ( mlcBitsBase10(calibID, "PRINT IO 22,IO 21", dummy) == 2 ) {
	sprintf(statusMsg,"%s Calibration Tower inserted, CALIB=IN", who_selected);
	isisStatusMsg(statusMsg);
	sprintf(&dummy1[len],"CALIB=IN ");
	ierr = agwcu("localhost", 0, "calib in", dummy);
	
      } else {
	if ( mechanismError > 0 ) {
	  if ( mlcBitsBase10(calibID, "PRINT IO 22,IO 21", dummy) == 1 ) {
	    sprintf(statusMsg,"%s Calibration Tower inserted, CALIB=OUT", who_selected);
	    isisStatusMsg(statusMsg);
	    sprintf(&dummy1[len],"CALIB=OUT ");
	    ierr = agwcu("localhost", 0, "calib out", dummy);

	  } else {
	    sprintf(statusMsg,"%s Calibration Tower, CALIB=FAULT", who_selected);
	    isisStatusMsg(statusMsg);
	    sprintf(&dummy1[len],"CALIB=FAULT ");

	  }

	} else {
	  ierr = Calib(1, who_selected, errorMsg, dummy1);
	  
	  if ( ierr != 0 ) {
	    shm_addr->MODS.instrMode = -1;
	    mechanismError = ierr;
	  } else 
	    ierr = agwcu("localhost", 0, "calib in", dummy);
	  
	}

      }	  
      len = strlen(dummy1);
      
      ierr = agwProbe(1, who_selected, errorMsg, dummy1);
      
      if ( ierr != 0 ) {
	shm_addr->MODS.instrMode = -1;
	mechanismError = ierr;
	
      }
      
      KeyCommand("gprobe", dummy);
      
      cmdLen = strlen("DONE: GPROBE ");
      sprintf(&dummy1[len],&dummy[cmdLen]);
      
      if ( mechanismError > 0 ) {
	sprintf(reply,"%s %s INSTMODE=FAULT %s XGPLAST=%0.3f YGPLAST=%0.3f", who_selected, errorMsg, (shm_addr->MODS.instrMode ? "CAL" : "OBS"), dummy1, shm_addr->MODS.xgpLast, shm_addr->MODS.ygpLast);
	
      } else sprintf(reply,"%s Instrument in Calibration Mode INSTMODE=%s %s XGPLAST=%0.3f YGPLAST=%0.3f", who_selected, (shm_addr->MODS.instrMode ? "CAL" : "OBS"), dummy1, shm_addr->MODS.xgpLast, shm_addr->MODS.ygpLast);
      
      return CMD_OK;
    }
    
    
    //<! ********* SAVE AGw guide gprobe xgb and ygp information
    
    KeyCommand("gprobe", dummy);
    
    for ( i = 0; dummy[i] != '='; i++ ); // Save xgb and ygp
    i++;
    shm_addr->MODS.xgpLast = atof(&dummy[i]); // Save X axis Guide Probe position.
    for (; dummy[i] != '='; i++ );
    i++;
    shm_addr->MODS.ygpLast = atof(&dummy[i]); // Save Y axis Guide Probe position.
    
    sprintf(statusMsg, "%s Saving current AGw guide probe position XGPLAST=%0.3f YGPLAST=%0.3f", who_selected, shm_addr->MODS.xgpLast, shm_addr->MODS.ygpLast);
    isisStatusMsg(statusMsg); // Status message

    //<! ********* START Dark Hatch

    if ( mlcBitsBase10(hatchID, "PRINT IO 22,IO 21", dummy) == 2 ) {
      sprintf(statusMsg,"%s Hatch closed, HATCH=CLOSED", who_selected);
      sprintf(dummy1,"HATCH=CLOSED ");
      isisStatusMsg(statusMsg);
      
    } else {
      if ( mechanismError > 0 ) {
	if ( mlcBitsBase10(hatchID, "PRINT IO 22,IO 21", dummy) == 1 ) {
	  sprintf(statusMsg,"%s Hatch open, HATCH=OPEN", who_selected);
	  sprintf(&dummy1[len],"HATCH=OPEN ");
	} else {
	  sprintf(statusMsg,"%s HATCH=FAULT", who_selected);
	  sprintf(&dummy1[len],"HATCH=FAULT ");
	}
	isisStatusMsg(statusMsg);
	
      } else {
	ierr = Hatch(1, who_selected, errorMsg, dummy1);
	
	if ( ierr != 0 ) {
	  shm_addr->MODS.instrMode = -1;
	  mechanismError = ierr;
	}
      }
    }

    //<! END of Dark Hatch and START AGw guide probe 

    len = strlen(dummy1); // Add a space 
    
    ierr = agwProbe(1, who_selected, errorMsg, dummy1);

    if ( ierr != 0 ) {
      shm_addr->MODS.instrMode = -1;
      mechanismError = ierr;
    }

    memset(dummy, 0, sizeof(dummy));
    memset(temp, 0, sizeof(temp));

    //<! END Retacting the AGw guide probe and START Inserting the Calibration Tower
    len = strlen(dummy1); // Add a space 

    if ( mlcBitsBase10(calibID, "PRINT IO 22,IO 21", dummy) == 2 ) {
      sprintf(statusMsg,"%s Calibration Tower inserted, CALIB=IN", who_selected);
      isisStatusMsg(statusMsg);
      sprintf(&dummy1[len],"CALIB=IN ");
      ierr = agwcu("localhost", 0, "calib in", dummy);
      
    } else {
      if ( mechanismError > 0 ) {
	if ( mlcBitsBase10(calibID, "PRINT IO 22,IO 21", dummy) == 1 ) {
	  sprintf(statusMsg,"%s Calibration Tower inserted, CALIB=OUT", who_selected);
	  isisStatusMsg(statusMsg);
	  sprintf(&dummy1[len],"CALIB=OUT ");
	  ierr = agwcu("localhost", 0, "calib out", dummy);

	} else {
	  sprintf(statusMsg,"%s Calibration Tower, CALIB=FAULT", who_selected);
	  isisStatusMsg(statusMsg);
	  sprintf(&dummy1[len],"CALIB=FAULT ");

	}

      } else {
	
	ierr = Calib(1, who_selected, errorMsg, dummy1);
	
	if ( ierr != 0 ) {
	  shm_addr->MODS.instrMode = -1;
	  mechanismError = ierr;
	} else
	  ierr = agwcu("localhost", 0, "calib in", dummy);
      }

    }
    len = strlen(dummy1); // Add a space 

    KeyCommand("gprobe", dummy);
    cmdLen = strlen("DONE: GPROBE "); // Add a space 

    sprintf(&dummy1[len], "%s", &dummy[cmdLen]);

    //<! END Inserting the Calibration Tower and Check for errors

    if ( mechanismError > 0 ) {
      sprintf(reply,"%s %s INSTMODE=FAULT %s XGPLAST=%0.3f YGPLAST=%0.3f", who_selected, errorMsg, dummy1, shm_addr->MODS.xgpLast, shm_addr->MODS.ygpLast);

      mechanismError = 0;
      shm_addr->MODS.instrMode = -1;
      
      return CMD_ERR;

    } else {

      shm_addr->MODS.instrMode = 1;

      len = strlen(dummy1); // Add an end of line \0
      
      dummy1[len] = '\0';
      
      sprintf(reply,"%s Instrument in Calibration Mode INSTMODE=%s %s XGPLAST=%0.3f YGPLAST=%0.3f", who_selected, (shm_addr->MODS.instrMode ? "CAL" : "OBS"), dummy1, shm_addr->MODS.xgpLast, shm_addr->MODS.ygpLast);
      
      return CMD_OK;
    }

  // OBSMODE command
    
  } else if ( strcasecmp(who_selected, "OBSMODE") == 0 ) { // Observing Mode

    // Make sure all calibration lamps are turned OFF

    llbID = getWagoID("llb", dummy); // Get Lamp/Laser Box(LLB) ID
    shm_addr->MODS.lamps.lamplaser_all[0] &= 0x03C0; 
    ierr = wagoSetGet(1, shm_addr->MODS.WAGOIP[llbID], 1, LLBONOFF, &shm_addr->MODS.lamps.lamplaser_all[0], 1);
    if ( ierr < 0 ) {
      sprintf(statusMsg, "%s could not talk to Lamps, no IP Address, CALLAMPS=FAULT", who_selected);
      if ( mechanismError == 0 ) {
	errorLen = strlen(errorMsg);
	sprintf(&errorMsg[errorLen], "could not talk to Lamps, no IP Address ");
	mechanismError = 1;
      }
      sprintf(lampMsg, "CALLAMPS=FAULT");
    } else {
      for ( i = 0 ; i < 9; i++ ) shm_addr->MODS.lamps.lamp_state[i] = 0;
      // Do not, under any circumstances, ever change the IMCS laser power! [rwp/osu - 2012 Feb 23]
      //ierr = wagoSetGet(1, shm_addr->MODS.WAGOIP[llbID], 1, 513, &shm_addr->MODS.lamps.lamplaser_all[0], 1);
      //ierr = wagoSetGet(1, shm_addr->MODS.WAGOIP[llbID], 1, 514, &shm_addr->MODS.lamps.lamplaser_all[0], 1);
      sprintf(statusMsg,"%s CALLAMPS='None' All lamps have been turned OFF",who_selected);
      sprintf(lampMsg, "CALLAMPS='None' ");
    }      
    isisStatusMsg(statusMsg);

    // Open the Hatch

    len = strlen(dummy1);

    if ( shm_addr->MODS.instrMode == 0 ) {

      if ( mlcBitsBase10(hatchID, "PRINT IO 22,IO 21", dummy) == 1 ) {
	sprintf(statusMsg,"%s Hatch opened, HATCH=OPEN", who_selected);
	sprintf(dummy1,"HATCH=OPEN ");
	isisStatusMsg(statusMsg);
	
      } else {
	if ( mechanismError > 0 ) {
	  if ( mlcBitsBase10(hatchID, "PRINT IO 22,IO 21", dummy) == 2 ) {
	    sprintf(statusMsg,"%s Hatch open, HATCH=CLOSED", who_selected);
	    isisStatusMsg(statusMsg);
	    sprintf(&dummy1[len],"HATCH=CLOSED ");
	  } else {
	    sprintf(statusMsg,"%s HATCH=FAULT", who_selected);
	    isisStatusMsg(statusMsg);
	    sprintf(&dummy1[len],"HATCH=FAULT ");
	  }
	  
	} else {
	  ierr = Hatch(0, who_selected, errorMsg, dummy1);
	  
	  if ( ierr != 0 ) {
	    shm_addr->MODS.instrMode = -1;
	    mechanismError = ierr;
	  }
	}
      }
      
      len = strlen(dummy1);

      // Calibration Tower

      if ( mlcBitsBase10(calibID, "PRINT IO 22,IO 21", dummy) == 1 ) {
	sprintf(statusMsg,"%s Calibration Tower retracted, CALIB=OUT", who_selected);
	isisStatusMsg(statusMsg);
	sprintf(&dummy1[len],"CALIB=OUT ");
	ierr = agwcu("localhost", 0, "calib out", dummy);
	
      } else {
	if ( mechanismError > 0 ) {
	  if ( mlcBitsBase10(calibID, "PRINT IO 22,IO 21", dummy) == 2 ) {
	    sprintf(statusMsg,"%s Calibration Tower inserted, CALIB=IN", who_selected);
	    isisStatusMsg(statusMsg);
	    sprintf(&dummy1[len],"CALIB=IN ");
	    ierr = agwcu("localhost", 0, "calib in", dummy);

	  } else {
	    sprintf(statusMsg,"%s Calibration Tower, CALIB=FAULT", who_selected);
	    isisStatusMsg(statusMsg);
	    sprintf(&dummy1[len],"CALIB=FAULT ");
	  }
	} else {
	  ierr = Calib(0, who_selected, errorMsg, dummy1);
	  
	  if ( ierr != 0 ) {
	    shm_addr->MODS.instrMode = -1;
	    mechanismError = ierr;
	  } else 
	    ierr = agwcu("localhost", 0, "calib out", dummy);

	}

      }

      if ( strcasecmp(argbuf,"RESTORE") == 0 && mechanismError <= 0 ) {
	
	  ierr = agwRestore(0, who_selected, dummy1);
	  
	  if ( ierr != 0 ) {
	    shm_addr->MODS.instrMode = -1;
	    mechanismError = ierr;
	  }
	KeyCommand("gprobe", dummy);
            
	cmdLen = strlen("DONE: GPROBE ");
	sprintf(&dummy1[len],&dummy[cmdLen]);

      } else {

	sprintf(statusMsg,"%s AGw guide probe unchaged", who_selected, &dummy[cmdLen]);
	isisStatusMsg(statusMsg);

	KeyCommand("gprobe", dummy);
            
	cmdLen = strlen("DONE: GPROBE ");
	sprintf(&dummy1[len],&dummy[cmdLen]);

      }

      if ( mechanismError > 0 ) {
	sprintf(reply,"%s %s INSTMODE=FAULT %s %s XGPLAST=%0.3f YGPLAST=%0.3f", who_selected, errorMsg, (shm_addr->MODS.instrMode ? "CAL" : "OBS"), lampMsg, dummy1, shm_addr->MODS.xgpLast, shm_addr->MODS.ygpLast);

      } else
	sprintf(reply,"%s Instrument in Observing Mode INSTMODE=%s %s %s XGPLAST=%0.3f YGPLAST=%0.3f", who_selected, (shm_addr->MODS.instrMode ? "CAL" : "OBS"), lampMsg, dummy1, shm_addr->MODS.xgpLast, shm_addr->MODS.ygpLast);
      
      return CMD_OK;
    }

    //<! START Dark Hatch

    memset(dummy, 0, sizeof(dummy));

    if ( mlcBitsBase10(hatchID, "PRINT IO 22,IO 21", dummy) == 1 ) {
      sprintf(statusMsg,"%s Hatch opened, HATCH=OPEN", who_selected);
      sprintf(dummy1,"HATCH=OPEN ");
      isisStatusMsg(statusMsg);

    } else {
      if ( mechanismError > 0 ) {
	if ( mlcBitsBase10(hatchID, "PRINT IO 22,IO 21", dummy) == 2 ) {
	  sprintf(statusMsg,"%s Hatch open, HATCH=CLOSED", who_selected);
	  isisStatusMsg(statusMsg);
	  sprintf(&dummy1[len],"HATCH=CLOSED ");
	} else {
	  sprintf(statusMsg,"%s HATCH=FAULT", who_selected);
	  isisStatusMsg(statusMsg);
	  sprintf(&dummy1[len],"HATCH=FAULT ");
	}
	
      } else {
	ierr = Hatch(0, who_selected, errorMsg, dummy1);
	
	if ( ierr != 0 ) {
	  shm_addr->MODS.instrMode = -1;
	  mechanismError = ierr;
	}
      }
    }
    
    //<! END of Dark Hatch

    //<! START Retracting the Calibration Tower
    
    len = strlen(dummy1);
    
    if ( mlcBitsBase10(calibID, "PRINT IO 22,IO 21", dummy) == 1 ) {
      sprintf(statusMsg,"%s Calibration Tower retracted, CALIB=OUT", who_selected);
      sprintf(&dummy1[len],"CALIB=OUT ");
      isisStatusMsg(statusMsg);
      ierr = agwcu("localhost", 0, "calib out", dummy);      

    } else {
      if ( mechanismError > 0 ) {
	if ( mlcBitsBase10(calibID, "PRINT IO 22,IO 21", dummy) == 2 ) {
	  sprintf(statusMsg,"%s Calibration Tower inserted, CALIB=IN", who_selected);
	  isisStatusMsg(statusMsg);
	  sprintf(&dummy1[len],"CALIB=IN ");
	  ierr = agwcu("localhost", 0, "calib in", dummy);

	} else {
	  sprintf(statusMsg,"%s Calibration Tower, CALIB=FAULT", who_selected);
	  isisStatusMsg(statusMsg);
	  sprintf(&dummy1[len],"CALIB=FAULT ");

	}

      } else {
	ierr = Calib(0, who_selected, errorMsg, dummy1);
	
	if ( ierr != 0 ) {
	  shm_addr->MODS.instrMode = -1;
	  mechanismError = ierr;

	} else
	  ierr = agwcu("localhost", 0, "calib out", dummy);

      }

    }
    memset(dummy, 0, sizeof(dummy));

    //<! END of Calibration Tower

    //<! START guide probe

    GetArg( args, 1, argbuf);

    if ( strcasecmp(argbuf,"RESTORE") == 0 && mechanismError <= 0 ) {
      
      ierr = agwRestore(0, who_selected, dummy1);
	  
      if ( ierr != 0 ) {
	shm_addr->MODS.instrMode = -1;
	mechanismError = ierr;
      }	  

    } else {
      
      len = strlen(dummy1); // Add a space 
      cmdLen = strlen("DONE: GPROBE ");
      KeyCommand("gprobe", dummy);
      
      sprintf(statusMsg,"%s Leaving AGw guide probe unchanged, %s", who_selected, &dummy[cmdLen]);
      sprintf(&dummy1[len], "%s", &dummy[cmdLen]);

      isisStatusMsg(statusMsg);
      
    }

    memset(dummy, 0, sizeof(dummy));
    memset(temp, 0, sizeof(temp));

    // Save instrument state CALMODE = 1 or OBSMODE = 0

    if ( mechanismError != 0 ) {
      sprintf(reply,"%s %s INSTMODE=FAULT %s %s XGPLAST=%0.3f YGPLAST=%0.3f", who_selected, errorMsg, lampMsg, dummy1, shm_addr->MODS.xgpLast, shm_addr->MODS.ygpLast);

      //      mechanismError = 0;
      shm_addr->MODS.instrMode = -1;
      
      return CMD_ERR;

    } else {

      shm_addr->MODS.instrMode = 0;

      len = strlen(dummy1); // Add an end of line \0

      dummy1[len] = '\0';

      //<! END guide probe
      
      sprintf(reply,"%s Instrument in Observing Mode INSTMODE=%s %s %s XGPLAST=%0.3f YGPLAST=%0.3f", who_selected,
	      (shm_addr->MODS.instrMode ? "CAL" : "OBS"),
	      lampMsg,
	      dummy1,
	      shm_addr->MODS.xgpLast,
	      shm_addr->MODS.ygpLast);

      mechanismError = 0;
      
      return CMD_OK;
      
    }
  }

  // Other commands follow that need to communicate with WAGOs
  // Get their assignment IDs here

  ieb1ID = getWagoID("ieb1",dummy); // Red IEB WAGO ID (IEB1)
  if(ieb1ID==-1) {
    sprintf(reply,"%s %s",who_selected,dummy);
    return CMD_ERR;
  }

  ieb2ID = getWagoID("ieb2",dummy); // Blue IEB WAGO ID (IEB2)
  if(ieb2ID==-1) {
    sprintf(reply,"%s %s",who_selected,dummy);
    return CMD_ERR;
  }

  llbID=getWagoID("llb",dummy); // Get Lamp/Laser Box (LLB) WAGO ID
  if(llbID==-1) {
    sprintf(reply,"%s %s",who_selected,dummy);
    return CMD_ERR;
  }

  utilID=getWagoID("util",dummy); // Get Utility Box (UTIL) WAGO ID
  if(utilID==-1) {
    sprintf(reply,"%s %s",who_selected,dummy);
    return CMD_ERR;
  }

  rhebID = getWagoID("rheb",dummy); // Red WAGO HEB ID
  if(rhebID==-1) {
    sprintf(reply,"%s %s",who_selected,dummy);
    return CMD_ERR;
  }

  bhebID = getWagoID("bheb",dummy); // Blue WAGO HEB ID
  if(rhebID==-1) {
    sprintf(reply,"%s %s",who_selected,dummy);
    return CMD_ERR;
  }

  // Below this point we need the IUB temperatures and pressure readouts

  ierr=wagoSetGet(0,shm_addr->MODS.WAGOIP[utilID],1,0,pressureTemps,10);

  glycolSupplyPressure = (float)pressureTemps[0]/327.64;
  glycolReturnPressure = (float)pressureTemps[1]/327.64;
  glycolSupplyTemperature = ptRTD2C(pressureTemps[4]);
  glycolReturnTemperature = ptRTD2C(pressureTemps[5]);
  agwHeatSinkTemperature = ptRTD2C(pressureTemps[6]);
  utilBoxAirTemperature = ptRTD2C(pressureTemps[7]);
  outsideAirTemperature = ptRTD2C(pressureTemps[8]);

  shm_addr->MODS.glycolSupplyPressure = glycolSupplyPressure;
  shm_addr->MODS.glycolSupplyTemperature = glycolSupplyTemperature;
  shm_addr->MODS.glycolReturnPressure = glycolReturnPressure;
  shm_addr->MODS.glycolReturnTemperature = glycolReturnTemperature;
  shm_addr->MODS.utilBoxAirTemperature = utilBoxAirTemperature;
  shm_addr->MODS.outsideAirTemperature = outsideAirTemperature;
  shm_addr->MODS.agwHeatSinkTemperature = agwHeatSinkTemperature;

  // IEB temperatures, RTD module register base address is 5

  // Red IEB (ieb1)
  
  ierr=wagoSetGet(0,shm_addr->MODS.WAGOIP[ieb1ID],1,4,iebTemps,4);

  redIEBAirTemperature = ptRTD2C(iebTemps[0]);
  redIEBReturnTemperature = ptRTD2C(iebTemps[1]);
  TrussTopAirTemperature = ptRTD2C(iebTemps[2]);
  TrussBottomAirTemperature = ptRTD2C(iebTemps[4]);

  shm_addr->MODS.redTemperature[0] = redIEBAirTemperature;
  shm_addr->MODS.redTemperature[1] = redIEBReturnTemperature;
  shm_addr->MODS.redTemperature[2] = TrussTopAirTemperature;
  shm_addr->MODS.redTemperature[3] = TrussBottomAirTemperature;

  // Blue IEB (ieb2)
  
  ierr=wagoSetGet(0,shm_addr->MODS.WAGOIP[ieb2ID],1,4,iebTemps,4);

  blueIEBAirTemperature = ptRTD2C(iebTemps[0]);
  blueIEBReturnTemperature = ptRTD2C(iebTemps[1]);
  TrussTopTemperature = ptRTD2C(iebTemps[2]);
  TrussBottomTemperature = ptRTD2C(iebTemps[4]);

  shm_addr->MODS.blueTemperature[0] = blueIEBAirTemperature;
  shm_addr->MODS.blueTemperature[1] = blueIEBReturnTemperature;
  shm_addr->MODS.blueTemperature[2] = TrussTopTemperature;
  shm_addr->MODS.blueTemperature[3] = TrussBottomTemperature;

  // HEB temperatures, RTD module register base address is 5
  
  // Red HEB
  
  ierr=wagoSetGet(0,shm_addr->MODS.WAGOIP[rhebID],1,4,hebTemps,2);
  
  redHEBTemperature = ptRTD2C(hebTemps[0]);
  redDewarTemperature = ptRTD2C(hebTemps[1]);
  shm_addr->MODS.redHEBTemperature = redHEBTemperature;
  shm_addr->MODS.redDewarTemperature = redDewarTemperature;

  // Blue HEB
  
  ierr=wagoSetGet(0,shm_addr->MODS.WAGOIP[bhebID],1,4,hebTemps,2);
    
  blueHEBTemperature = ptRTD2C(hebTemps[0]);
  blueDewarTemperature = ptRTD2C(hebTemps[1]);
  shm_addr->MODS.blueHEBTemperature = blueHEBTemperature;
  shm_addr->MODS.blueDewarTemperature = blueDewarTemperature;

  //--------------------------------------------------------
  //
  // UTIL Command - instrument utilities through the IUB
  //
  
  if(!strcasecmp(who_selected,"UTIL")) { // Utility Box control

    if (!strcasecmp(argbuf,"CONFIG")) { // get mechanism ip
      sprintf(reply,"%s IP=%s",who_selected,shm_addr->MODS.WAGOIP[utilID]);
      return CMD_OK;
    }

    // Get the power relay states - if the IUB is off, everybody is off
    // (or the IUB is disconnected from the network, so OFF == UNKNOWN)

    ierr = wagoSetGet(0,shm_addr->MODS.WAGOIP[utilID],1,512,devOnOff,1);
    if (ierr<0) {
      sprintf(reply,"%s %s", who_selected,"UTIL=OFF");
      shm_addr->MODS.utilState = 0;
      shm_addr->MODS.llbState = 0;
      shm_addr->MODS.redIEBState = 0;
      shm_addr->MODS.blueIEBState = 0;
      shm_addr->MODS.redHEBState = 0;
      shm_addr->MODS.blueHEBState = 0;
      shm_addr->MODS.redArchonState = 0;
      shm_addr->MODS.blueArchonState = 0;
      shm_addr->MODS.redIonGaugeState = 0;
      shm_addr->MODS.blueIonGaugeState = 0;
      return CMD_OK;
    }
    shm_addr->MODS.utilState = 1;
    allUtilPower = devOnOff[0];

    // Power control relay states
	
    iebRedPower    = allUtilPower & 1;
    iebBluePower   = allUtilPower & 2;
    hebRedEnable   = allUtilPower & 4; // unused post-2025
    hebRedPower    = allUtilPower & 8;
    hebBlueEnable  = allUtilPower & 16; // unused post-2025
    hebBluePower   = allUtilPower & 32;
    agwWFSPower    = allUtilPower & 64;
    agwGuidePower  = allUtilPower & 128;
    lampLaserPower = allUtilPower & 256;

    /*
    // helpful state info during testing
    printf("\nPower: devOnOff=%d\n",devOnOff[0]);
    printf("  iebR=%d iebB=%d hebR=%d hebB=%d wfs=%d agc=%d llb=%d\n",iebRedPower,
	   iebBluePower,hebRedPower,hebBluePower,agwWFSPower,agwGuidePower,lampLaserPower);
    */
    
    // Get circuit breaker states

    ierr = wagoSetGet(0,shm_addr->MODS.WAGOIP[utilID],1,10,devOnOff,1);
    allUtilBreaker = devOnOff[0];

    // Circuit breaker states (current sense past break)

    iebRedBreaker    = allUtilBreaker & 1;
    iebBlueBreaker   = allUtilBreaker & 2;
    hebRedBreaker    = allUtilBreaker & 4;
    hebBlueBreaker   = allUtilBreaker & 8;
    agwWFSBreaker    = allUtilBreaker & 16;
    agwGuideBreaker  = allUtilBreaker & 32;
    lampLaserBreaker = allUtilBreaker & 64;

    /*
    // helpful state info during testing
    printf("Breakers: devOnOff=%d\n",devOnOff[0]);
    printf("  iebR=%d iebB=%d hebR=%d hebB=%d wfs=%d agc=%d llb=%d\n",iebRedBreaker,
	   iebBlueBreaker,hebRedBreaker,hebBlueBreaker,agwWFSBreaker,
	   agwGuideBreaker,lampLaserBreaker);
    */
    
    // Process the specific UTIL request

    if(!strcasecmp(argbuf,"STATUS") || strlen(args)<=0) {  // STATUS or no-arg is implicit status request

      // Start building the reply
	
      sprintf(reply,"%s",who_selected);

      // Note that the power control relays for the IEBs and LLB are
      // normally closed (immediate power up on application of main AC
      // power), whereas the HEBs and AGw camera power control relays
      // are normally open (these units must be explicitly powered up
      // via the WAGO or the manual-override switches on the IUB patch
      // panel).  That is why the IEB and LLB tests are false=ON,
      // whereas the others are true=ON for the power.

      // IEB power relays are normally closed, so test for ON is !=
	
      if (iebRedPower != 1) {
	if (iebRedBreaker == 1) {
	  sprintf(reply,"%s IEB_R=ON IEB_R_BRK=OK",reply);
	  shm_addr->MODS.redIEBState = 1;
	}
	else {
	  sprintf(reply,"%s IEB_R=ON IEB_R_BRK=FAULT",reply);
	  shm_addr->MODS.redIEBState = -1;
	}
      }
      else {
	sprintf(reply,"%s IEB_R=OFF IEB_R_BRK=UNKNOWN",reply);
	shm_addr->MODS.redIEBState = 0;
      }

      if (iebBluePower != 2) {
	if (iebBlueBreaker == 2) {
	  sprintf(reply,"%s IEB_B=ON IEB_B_BRK=OK",reply);
	  shm_addr->MODS.blueIEBState = 1;
	}
	else {
	  sprintf(reply,"%s IEB_B=ON IEB_B_BRK=FAULT",reply);
	  shm_addr->MODS.blueIEBState = -1;
	}
      }
      else {
	sprintf(reply,"%s IEB_B=OFF IEB_B_BRK=UNKNOWN",reply);
	shm_addr->MODS.blueIEBState = 0;
      }
      
      // Post-2025 WAGO HEB power normally closed so test for ON is != 
      //   Red Power is on coil 4 = decimal 8
      //   Red Breaker is on coil 3 = decimal 4
      
      if (hebRedPower != 8) {
	if (hebRedBreaker == 4) {
	  sprintf(reply,"%s HEB_R=ON HEB_R_BRK=OK",reply);
	  shm_addr->MODS.redHEBState = 1;
	}
	else {
	  sprintf(reply,"%s HEB_R=ON HEB_R_BRK=FAULT",reply);
	  shm_addr->MODS.redHEBState = -1;
	}
      }
      else {
	sprintf(reply,"%s HEB_R=OFF HEB_R_BRK=UNKNOWN",reply);
	shm_addr->MODS.redHEBState = 0;
      }

      // Blue Power is on coil 6 = decimal 32
      // Blue Breaker is on coil 4 = decimal 8
      
      if (hebBluePower != 32) {
	if (hebBlueBreaker == 8) {
	  sprintf(reply,"%s HEB_B=ON HEB_B_BRK=OK",reply);
	  shm_addr->MODS.blueHEBState = 1;
	}
	else {
	  sprintf(reply,"%s HEB_B=ON HEB_B_BRK=FAULT",reply);
	  shm_addr->MODS.blueHEBState = -1;
	}
      }
      else {
	sprintf(reply,"%s HEB_B=OFF HEB_B_BRK=UNKNOWN",reply);
	shm_addr->MODS.blueHEBState = 0;
      }

      // AGw WFS and Guide Camera power relays are normally open so test for power ON is ==
	
      if (agwWFSPower == 64) {
	if (agwWFSBreaker == 16) {
	  sprintf(reply,"%s WFS=ON WFS_BRK=OK",reply);
	  shm_addr->MODS.wfsCamState = 1;
	}
	else {
	  sprintf(reply,"%s WFS=ON WFS_BRK=FAULT",reply);
	  shm_addr->MODS.wfsCamState = -1;
	}
      }
      else {
	sprintf(reply,"%s WFS=OFF WFS_BRK=UNKNOWN",reply);
	shm_addr->MODS.wfsCamState = 0;
      }

      if (agwGuidePower == 128) {
	if (agwGuideBreaker == 32) {
	  sprintf(reply,"%s AGC=ON AGC_BRK=OK",reply);
	  shm_addr->MODS.guideCamState = 1;
	}
	else {
	  sprintf(reply,"%s AGC=ON AGC_BRK=FAULT",reply);
	  shm_addr->MODS.guideCamState = -1;
	}
      }
      else {
	sprintf(reply,"%s AGC=OFF AGC_BRK=UNKNOWN",reply);
	  shm_addr->MODS.guideCamState = 0;
      }

      // LLB power relay is normally closed so test for ON is !=
	
      if (lampLaserPower != 256) {
	if (lampLaserBreaker == 64) {
	  sprintf(reply,"%s LLB=ON LLB_BRK=OK",reply);
	  shm_addr->MODS.llbState = 1;
	}
	else {
	  sprintf(reply,"%s LLB=ON LLB_BRK=FAULT",reply);
	  shm_addr->MODS.llbState = -1;
	}
      }
      else {
	sprintf(reply,"%s LLB=OFF LLB_BRK=UNKNOWN",reply);
	shm_addr->MODS.llbState = 0;
      }
      
      // Glycol supply and return pressures and temperatures
	
      (glycolSupplyPressure>=850.0 ? sprintf(reply,"%s GSPRES=NOCOMM",reply) : 
       sprintf(reply,"%s GSPRES=%0.1f",reply,glycolSupplyPressure));
      
      (glycolReturnPressure>=850.0 ? sprintf(reply,"%s GRPRES=NOCOMM",reply) : 
       sprintf(reply,"%s GRPRES=%0.1f",reply, glycolReturnPressure));
      
      (glycolSupplyTemperature>=850.0 ? sprintf(reply,"%s GSTEMP=NOCOMM",reply) : 
       sprintf(reply,"%s GSTEMP=%0.1f",reply,glycolSupplyTemperature));
      
      (glycolReturnTemperature>=850.0 ? sprintf(reply,"%s GRTEMP=NOCOMM",reply) : 
       sprintf(reply,"%s GRTEMP=%0.1f",reply,glycolReturnTemperature));
      
      // Utility box temperature sensors 

      (utilBoxAirTemperature>=850.0 ? sprintf(reply,"%s IUBTAIR=NOCOMM",reply) :
       sprintf(reply,"%s IUBTAIR=%0.1f",reply,utilBoxAirTemperature));
      
      (outsideAirTemperature>=850.0 ? sprintf(reply,"%s AMBTEMP=NOCOMM",reply) : 
       sprintf(reply,"%s AMBTEMP=%0.1f",reply,outsideAirTemperature));
      
      (agwHeatSinkTemperature>=850.0 ? sprintf(reply,"%s AGHSTEMP=NOCOMM",reply) : 
       sprintf(reply,"%s AGHSTEMP=%0.1f",reply,agwHeatSinkTemperature));
      
    } else if(!strcasecmp(argbuf,"GLYCOL")) {
      
      (glycolSupplyPressure>=850.0 ?  sprintf(reply,"%s GSPRES=NOCOMM",who_selected) : 
       sprintf(reply,"%s GSPRES=%0.1f",who_selected,glycolSupplyPressure));
	
      (glycolSupplyTemperature>=850.0 ? sprintf(reply,"%s GSTEMP=NOCOMM",reply) : 
       sprintf(reply,"%s GSTEMP=%0.1f",reply,glycolSupplyTemperature));
	
      (glycolReturnPressure>=850.0 ? sprintf(reply,"%s GRPRES=NOCOMM",reply) : 
       sprintf(reply,"%s GRPRES=%0.1f",reply, glycolReturnPressure));
	
      (glycolReturnTemperature>=850.0 ? sprintf(reply,"%s GRTEMP=NOCOMM",reply) : 
       sprintf(reply,"%s GRTEMP=%0.1f",reply,glycolReturnTemperature));

      return CMD_OK;

    } else if(!strcasecmp(argbuf,"TEMP")) {

	(glycolSupplyTemperature>=850.0 ? sprintf(reply,"%s GSTEMP=NOCOMM",who_selected) : 
	 sprintf(reply,"%s GSTEMP=%0.1f",who_selected,glycolSupplyTemperature));
	
	(glycolReturnTemperature>=850.0 ? sprintf(reply,"%s GRTEMP=NOCOMM",reply) : 
	 sprintf(reply,"%s GRTEMP=%0.1f",reply,glycolReturnTemperature));

	(utilBoxAirTemperature>=850.0 ? sprintf(reply,"%s IUBTAIR=NOCOMM",reply) :
	 sprintf(reply,"%s IUBTAIR=%0.1f",reply,utilBoxAirTemperature));
	
	(outsideAirTemperature>=850.0 ? sprintf(reply,"%s AMBTEMP=NOCOMM",reply) : 
	 sprintf(reply,"%s AMBTEMP=%0.1f",reply,outsideAirTemperature));
	
	(agwHeatSinkTemperature>=850.0 ? sprintf(reply,"%s AGHSTEMP=NOCOMM",reply) : 
	 sprintf(reply,"%s AGHSTEMP=%0.1f",reply,agwHeatSinkTemperature));

      return CMD_OK;

    } else if(!strcasecmp(argbuf,"ALL")) {
      GetArg(args,2,argbuf);

      if(!strcasecmp(argbuf,"OFF")) {

	KeyCommand("util ieb_r off", dummy);
	isisStatusMsg(dummy);
	mmcLOGGER(shm_addr->MODS.LLOG,dummy);

	MilliSleep(1000);

	KeyCommand("util ieb_b off", dummy);
	isisStatusMsg(dummy);
	mmcLOGGER(shm_addr->MODS.LLOG,dummy);

	MilliSleep(1000);

	KeyCommand("util llb off", dummy);
	isisStatusMsg(dummy);
	mmcLOGGER(shm_addr->MODS.LLOG,dummy);

	MilliSleep(1000);

	KeyCommand("util agc off", dummy);
	isisStatusMsg(dummy);
	mmcLOGGER(shm_addr->MODS.LLOG,dummy);

	MilliSleep(1000);

	KeyCommand("util wfs off", dummy);
	isisStatusMsg(dummy);
	mmcLOGGER(shm_addr->MODS.LLOG,dummy);

	MilliSleep(1000);

	KeyCommand("util heb_b off", dummy);
	isisStatusMsg(dummy);
	mmcLOGGER(shm_addr->MODS.LLOG,dummy);

	MilliSleep(1000);

	KeyCommand("util heb_r off", dummy);
	isisStatusMsg(dummy);
	mmcLOGGER(shm_addr->MODS.LLOG,dummy);

	MilliSleep(1000);
	cmd_istatus("", EXEC,reply);
	
      } else {
	sprintf(reply,"%s Invalid '%s', Usage UTIL all off",who_selected,argbuf); 
	return CMD_ERR;
      }
      return CMD_OK;

      // HEB power control logic changed with the 2025 update:
      //   * no longer using a hold-in/drop-out circuit with current-sensing relay
      //   * HEB boxes are Power ON at startup non OFF, so power relays normally CLOSED
      //   * Power ON state is now datum != state (allUtilPower&int), not ==
      
    } else if(!strcasecmp(argbuf,"HEB_R")) {
      GetArg(args,2,argbuf);
      hebRedPower = allUtilPower & 8;
      hebRedBreaker = allUtilBreaker & 4;
      
      if(!strcasecmp(argbuf,"STATUS") || strlen(argbuf)<=0) {
	if (hebRedPower != 8) {
	  if (hebRedBreaker == 4) {
	    sprintf(reply,"%s HEB_R=ON HEB_R_BRK=OK",who_selected);
	    shm_addr->MODS.redHEBState = 1;
	  }
	  else {
	    sprintf(reply,"%s HEB_R=ON HEB_R_BRK=FAULT",who_selected);
	    shm_addr->MODS.redHEBState = -1;
	  }
	}
	else {
	  sprintf(reply,"%s HEB_R=OFF HEB_R_BRK=UNKNOWN",who_selected);
	  shm_addr->MODS.redHEBState = 0;
	}

      } else if(!strcasecmp(argbuf,"ON")) {
	if(hebRedPower==8) {
	  devOnOff[0]=(short )(allUtilPower ^ 8);
	  ierr = wagoSetGet(1,shm_addr->MODS.WAGOIP[utilID],1,512,devOnOff,1);
	}
	MilliSleep(200);
	KeyCommand("util heb_r", dummy);
	sprintf(reply,"%s",&dummy[6]); 

      } else if(!strcasecmp(argbuf,"OFF")) {
	if(hebRedPower==0) {
	  devOnOff[0]=(short )(allUtilPower | 8);
	  ierr = wagoSetGet(1,shm_addr->MODS.WAGOIP[utilID],1,512,devOnOff,1);
	}
	MilliSleep(200);
	KeyCommand("util heb_r", dummy);
	sprintf(reply,"%s",&dummy[6]); 

      } else {
	sprintf(reply,"%s Invalid '%s', Usage UTIL HEB_R [on|off][status]",who_selected,argbuf); 
	return CMD_ERR;
      }
      
      return CMD_OK;

    } else if(!strcasecmp(argbuf,"HEB_B")) {
      GetArg(args,2,argbuf);

      hebBluePower = allUtilPower & 32;
      hebBlueBreaker = allUtilBreaker & 8; 
      
      if(!strcasecmp(argbuf,"STATUS") || strlen(argbuf)<=0) {
	if (hebBluePower != 32) {
	  if (hebBlueBreaker == 8) {
	    sprintf(reply,"%s HEB_B=ON HEB_B_BRK=OK",who_selected);
	    shm_addr->MODS.blueHEBState = 1;
	  }
	  else {
	    sprintf(reply,"%s HEB_B=ON HEB_B_BRK=FAULT",who_selected);
	    shm_addr->MODS.blueHEBState = -1;
	  }
	}
	else {
	  sprintf(reply,"%s HEB_B=OFF HEB_B_BRK=UNKNOWN",who_selected);
	  shm_addr->MODS.blueHEBState = 0;
	}

      } else if(!strcasecmp(argbuf,"ON")) {
	if(hebBluePower==32) {
	  devOnOff[0]=(short )(allUtilPower ^ 32);
	  ierr = wagoSetGet(1,shm_addr->MODS.WAGOIP[utilID],1,512,devOnOff,1);
	}
	MilliSleep(200);
	KeyCommand("util heb_b", dummy);
	sprintf(reply,"%s",&dummy[6]); 

      } else if(!strcasecmp(argbuf,"OFF")) {
	if(hebBluePower==0) {
	  devOnOff[0]=(short )(allUtilPower | 32);
	  ierr = wagoSetGet(1,shm_addr->MODS.WAGOIP[utilID],1,512,devOnOff,1);
	}
	MilliSleep(200);
	KeyCommand("util heb_b", dummy);
	sprintf(reply,"%s",&dummy[6]); 

      } else {
	sprintf(reply,"%s Invalid '%s', Usage UTIL HEB_B [on|off][status]",who_selected,argbuf); 
	return CMD_ERR;
      }
      
      return CMD_OK;

      // IEBs are normally closed power relay (power on with instrument main)
      
    } else if(!strcasecmp(argbuf,"IEB_R")) {
      GetArg(args,2,argbuf);

      iebRedPower = allUtilPower&1;
      iebRedBreaker = allUtilBreaker&1;

      if(!strcasecmp(argbuf,"STATUS") || strlen(argbuf)<=0) {
	if (iebRedPower != 1) {
	  if (iebRedBreaker == 1) {
	    sprintf(reply,"%s IEB_R=ON IEB_R_BRK=OK",who_selected);
	    shm_addr->MODS.redIEBState = 1;
	  }
	  else {
	    sprintf(reply,"%s IEB_R=ON IEB_R_BRK=FAULT",who_selected);
	    shm_addr->MODS.redIEBState = -1;
	  }
	}
	else {
	  sprintf(reply,"%s IEB_R=OFF IEB_R_BRK=UNKNOWN",who_selected);
	  shm_addr->MODS.redIEBState = 0;
	}

      } else if(!strcasecmp(argbuf,"ON")) {

	if(iebRedPower==1) {
	  devOnOff[0]=(short )(allUtilPower^1);
	  ierr = wagoSetGet(1,shm_addr->MODS.WAGOIP[utilID],1,512,devOnOff,1);
	}
	MilliSleep(200);
	KeyCommand("util ieb_r", dummy);
	sprintf(reply,"%s",&dummy[6]); 

      } else if(!strcasecmp(argbuf,"OFF")) {

	if(iebRedPower==0) {
	  devOnOff[0]=(short )(allUtilPower|1);
	  ierr = wagoSetGet(1,shm_addr->MODS.WAGOIP[utilID],1,512,devOnOff,1);
	}
	MilliSleep(200);
	KeyCommand("util ieb_r", dummy);
	sprintf(reply,"%s",&dummy[6]); 

      } else {
	sprintf(reply,"%s Invalid '%s', Usage UTIL IEB_R [on|off][status]",who_selected,argbuf); 
	return CMD_ERR;
      }

      return CMD_OK;

    } else if(!strcasecmp(argbuf,"IEB_B")) {
      GetArg(args,2,argbuf);

      iebBluePower = allUtilPower&2;
      iebBlueBreaker = allUtilBreaker&2;

      if(!strcasecmp(argbuf,"STATUS") || strlen(argbuf)<=0) {
	if (iebBluePower != 2) {
	  if (iebBlueBreaker == 2) {
	    sprintf(reply,"%s IEB_B=ON IEB_B_BRK=OK",who_selected);
	    shm_addr->MODS.blueIEBState = 1;
	  }
	  else {
	    sprintf(reply,"%s IEB_B=ON IEB_B_BRK=FAULT",who_selected);
	    shm_addr->MODS.blueIEBState = -1;
	  }
	}
	else {
	  sprintf(reply,"%s IEB_B=OFF IEB_B_BRK=UNKNOWN",who_selected);
	  shm_addr->MODS.blueIEBState = 0;
	}

      } else if(!strcasecmp(argbuf,"ON")) {

	if(iebBluePower==2) {
	  devOnOff[0]=(short )(allUtilPower^2);
	  ierr = wagoSetGet(1,shm_addr->MODS.WAGOIP[utilID],1,512,devOnOff,1);
	}
	MilliSleep(200);
	KeyCommand("util ieb_b", dummy);
	sprintf(reply,"%s",&dummy[6]); 

      } else if(!strcasecmp(argbuf,"OFF")) {

	if(iebBluePower==0) {
	  devOnOff[0]=(short )(allUtilPower|2);
	  ierr = wagoSetGet(1,shm_addr->MODS.WAGOIP[utilID],1,512,devOnOff,1);
	}
	MilliSleep(200);
	KeyCommand("util ieb_b", dummy);
	sprintf(reply,"%s",&dummy[6]); 

      } else {
	sprintf(reply,"%s Invalid '%s', Usage UTIL IEB_B [on|off][status]",who_selected,argbuf); 
	return CMD_ERR;
      }

      return CMD_OK;

      // AGw WFS ARC controller power relay is normally OPEN, power OFF at instrument mains ON
      
    } else if(!strcasecmp(argbuf,"WFS")) {
      GetArg(args,2,argbuf);

      agwWFSPower = allUtilPower&64;
      agwWFSBreaker = allUtilBreaker&16;
      
      if(!strcasecmp(argbuf,"STATUS") || strlen(argbuf)<=0) {
	if (agwWFSPower == 64) {
	  if (agwWFSBreaker == 16) {
	    sprintf(reply,"%s WFS=ON WFS_BRK=OK",who_selected);
	    shm_addr->MODS.wfsCamState = 1;
	  }
	  else {
	    sprintf(reply,"%s WFS=ON WFS_BRK=FAULT",who_selected);
	    shm_addr->MODS.wfsCamState = -1;
	  }
	}
	else {
	  sprintf(reply,"%s WFS=OFF WFS_BRK=UNKNOWN",who_selected);
	  shm_addr->MODS.wfsCamState = 0;
	}

      } else if(!strcasecmp(argbuf,"OFF")) {

	if(agwWFSPower==64) {
	  devOnOff[0]=(short )(allUtilPower^64);
	  ierr = wagoSetGet(1,shm_addr->MODS.WAGOIP[utilID],1,512,devOnOff,1);
	}
	MilliSleep(200);
	KeyCommand("util wfs", dummy);
	sprintf(reply,"%s",&dummy[6]); 

      } else if(!strcasecmp(argbuf,"ON")) {

	if(agwWFSPower==0) {
	  devOnOff[0]=(short )(allUtilPower|64);
	  ierr = wagoSetGet(1,shm_addr->MODS.WAGOIP[utilID],1,512,devOnOff,1);
	}
	MilliSleep(200);
	KeyCommand("util wfs", dummy);
	sprintf(reply,"%s",&dummy[6]); 

      } else {
	sprintf(reply,"%s Invalid '%s', Usage UTIL WFS [on|off][status]",who_selected,argbuf); 
	return CMD_ERR;
      }

      return CMD_OK;

      // AGw Acquisition/Guide Camera ARC controller power relay is normally OPEN
      
    } else if(!strcasecmp(argbuf,"AGC")) {
      GetArg(args,2,argbuf);

      agwGuidePower = allUtilPower&128;
      agwGuideBreaker = allUtilBreaker&32;
      if(!strcasecmp(argbuf,"STATUS") || strlen(argbuf)<=0) {
	if (agwGuidePower == 128) {
	  if (agwGuideBreaker == 32) {
	    sprintf(reply,"%s AGC=ON AGC_BRK=OK",who_selected);
	    shm_addr->MODS.guideCamState = 1;
	  }
	  else {
	    sprintf(reply,"%s AGC=ON AGC_BRK=FAULT",who_selected);
	    shm_addr->MODS.guideCamState = -1;
	  }
	}
	else {
	  sprintf(reply,"%s AGC=OFF AGC_BRK=UNKNOWN",who_selected);
	  shm_addr->MODS.guideCamState = 0;
	}

      } else if(!strcasecmp(argbuf,"OFF")) {

	if(agwGuidePower==128) {
	  devOnOff[0]=(short )(allUtilPower^128);
	  ierr = wagoSetGet(1,shm_addr->MODS.WAGOIP[utilID],1,512,devOnOff,1);
	}
	MilliSleep(200);
	KeyCommand("util agc", dummy);
	sprintf(reply,"%s",&dummy[6]); 
	  
      } else if(!strcasecmp(argbuf,"ON")) {

	if(agwGuidePower==0) {
	  devOnOff[0]=(short )(allUtilPower|128);
	  ierr = wagoSetGet(1,shm_addr->MODS.WAGOIP[utilID],1,512,devOnOff,1);
	}
	MilliSleep(200);
	KeyCommand("util agc", dummy);
	sprintf(reply,"%s",&dummy[6]); 
      }

      // Change power state of both AGw unit WFS and AGC
      
    } else if(!strcasecmp(argbuf,"AGW")) {

	agwGuidePower = allUtilPower&128;
	agwGuideBreaker = allUtilBreaker&32;
	if (agwGuidePower == 128) {
	  if (agwGuideBreaker == 32) {
	    sprintf(reply,"%s AGC=ON AGC_BRK=OK",who_selected);
	    shm_addr->MODS.guideCamState = 1;
	  }
	  else {
	    sprintf(reply,"%s AGC=ON AGC_BRK=FAULT",who_selected);
	    shm_addr->MODS.guideCamState = -1;
	  }
	}
	else {
	  sprintf(reply,"%s AGC=OFF AGC_BRK=UNKNOWN",who_selected);
	  shm_addr->MODS.guideCamState = 0;
	}

	agwWFSPower = allUtilPower&64;
	agwWFSBreaker = allUtilBreaker&16;

	if (agwWFSPower == 64) {
	  if (agwWFSBreaker == 16) {
	    sprintf(reply,"%s WFS=ON WFS_BRK=OK",reply);
	    shm_addr->MODS.wfsCamState = 1;
	  }
	  else {
	    sprintf(reply,"%s WFS=ON WFS_BRK=FAULT",reply);
	    shm_addr->MODS.wfsCamState = -1;
	  }
	}
	else {
	  sprintf(reply,"%s WFS=OFF WFS_BRK=UNKNOWN",reply);
	  shm_addr->MODS.wfsCamState = 0;
	}

	if((agwGuidePower==0 && agwGuideBreaker==0) &&
	   (agwWFSPower==0 && agwWFSBreaker==0)) 
	  agwstatus=0;
	else if((agwGuidePower==128 && agwGuideBreaker==32) &&
		(agwWFSPower==64 && agwWFSBreaker==16))
	  agwstatus=1;
	else
	  agwstatus=0x100;

	sprintf(reply,"%s %d",reply, agwstatus);

	return CMD_OK;

	// Lamp/Laser Box power relay is normally CLOSED (power ON with instrument mains)
	
    } else if(!strcasecmp(argbuf,"LLB")) {
      GetArg(args,2,argbuf);

      lampLaserPower = allUtilPower&256;
      lampLaserBreaker = allUtilBreaker&64;
      if(!strcasecmp(argbuf,"STATUS") || strlen(argbuf)<=0) {
	if (lampLaserPower != 256) {
	  if (lampLaserBreaker == 64) {
	    sprintf(reply,"%s LLB=ON LLB_BRK=OK",who_selected);
	    shm_addr->MODS.llbState = 1;
	  }
	  else {
	    sprintf(reply,"%s LLB=ON LLB_BRK=FAULT",who_selected);
	    shm_addr->MODS.llbState = -1;
	  }
	}
	else {
	  sprintf(reply,"%s LLB=OFF LLB_BRK=UNKNOWN",who_selected);
	  shm_addr->MODS.llbState = 0;
	}

      } else if(!strcasecmp(argbuf,"ON")) {

	if(lampLaserPower==256) {
	  devOnOff[0]=(short )(allUtilPower^256);
	  ierr = wagoSetGet(1,shm_addr->MODS.WAGOIP[utilID],1,512,devOnOff,1);
	}
	MilliSleep(200);
	KeyCommand("util llb", dummy);
	sprintf(reply,"%s",&dummy[6]); 

      } else if(!strcasecmp(argbuf,"OFF")) {

	if(lampLaserPower==0) {
	  devOnOff[0]=(short )(allUtilPower|256);
	  ierr = wagoSetGet(1,shm_addr->MODS.WAGOIP[utilID],1,512,devOnOff,1);
	}
	MilliSleep(200);
	KeyCommand("util llb", dummy);
	sprintf(reply,"%s",&dummy[6]); 

      } else {
	sprintf(reply,"%s Invalid '%s', Usage UTIL LLB [on|off][status]",who_selected,argbuf); 
	return CMD_ERR;
      }

    } else {
      sprintf(reply,"%s Invalid '%s', Usage UTIL [service1 service2 ... [on|off][status][all off][glycol][temp]",who_selected,argbuf); 
      return CMD_ERR;
    }

    return CMD_OK;

    //----------------------------------------------------------------
    //
    // Lamp/Laser Box (LLB) Internal control functions
    //
    
  } else if(!strcasecmp(who_selected,"LLB")) { // Lamp Laser Box control

    if(!strcasecmp(argbuf,"RESET")) {
      shm_addr->MODS.lamps.lamplaser_all[0] = 0; // Turn off all lamps and lasers
      ierr = wagoSetGet(1, shm_addr->MODS.WAGOIP[llbID], 1, LLBONOFF, &shm_addr->MODS.lamps.lamplaser_all[0], 1);

      for(i=0;i<9;i++) shm_addr->MODS.lamps.lamp_state[i]=0;
      
      ierr = wagoSetGet(1, shm_addr->MODS.WAGOIP[llbID], 1, 513, &shm_addr->MODS.lamps.lamplaser_all[0], 1);
      
      shm_addr->MODS.lasers.vislaser_state=0;
      shm_addr->MODS.lasers.visbeam_state=0;

      ierr = wagoSetGet(1, shm_addr->MODS.WAGOIP[llbID], 1, 514, &shm_addr->MODS.lamps.lamplaser_all[0], 1);

      shm_addr->MODS.lasers.irlaser_state=0;
      shm_addr->MODS.lasers.irbeam_state=0;

      sprintf(reply,"%s CALLAMPS='None' All lamps have been turned off",who_selected);

    } else {
      sprintf(reply,"LLB Invalid '%s', Usage LLB RESET",argbuf); 
      return CMD_ERR;
    }
    return CMD_OK;

    //------------------------------------------------------
    //
    // ESTATUS - read environmental sensor status, multiple WAGOs
    //
    
  } else if(!strcasecmp(who_selected,"ESTATUS")) { // Environmental sensor status summary

    // 
    // Dirty way to do it... retired 2025 [rwp]
    //
    // KeyCommand("ieb r temp", dummy);
    // sprintf(reply,"%s %s",who_selected,&dummy[11]);
    // KeyCommand("ieb b temp", dummy);
    // sprintf(reply,"%s %s",reply,&dummy[11]);

    // start the reply
    
    sprintf(reply,"%s ",who_selected);  // start the reply

    // IEB temperature sensors

    (redIEBAirTemperature>=850.0 ? sprintf(reply,"%s IEBTEMPR=NOCOMM",reply) : 
     sprintf(reply,"%s IEBTEMPR=%0.1f",reply,redIEBAirTemperature));

    (redIEBReturnTemperature>=850.0 ? sprintf(reply,"%s IEBGRT_R=NOCOMM",reply) : 
     sprintf(reply,"%s IEBGRT_R=%0.1f",reply,redIEBReturnTemperature));

    (blueIEBAirTemperature>=850.0 ? sprintf(reply,"%s IEBTEMPB=NOCOMM",reply) : 
     sprintf(reply,"%s IEBTEMPB=%0.1f",reply,blueIEBAirTemperature));

    (blueIEBReturnTemperature>=850.0 ? sprintf(reply,"%s IEBGRT_B=NOCOMM",reply) : 
     sprintf(reply,"%s IEBGRT_B=%0.1f",reply,blueIEBReturnTemperature));

    (TrussTopAirTemperature>=850.0 ? sprintf(reply,"%s TAIRTOP=NOCOMM",reply) : 
     sprintf(reply,"%s TAIRTOP=%0.1f",reply,TrussTopAirTemperature));

    (TrussBottomAirTemperature>=850.0 ? sprintf(reply,"%s TAIRBOT=NOCOMM",reply) : 
     sprintf(reply,"%s TAIRBOT=%0.1f",reply,TrussBottomAirTemperature));

    (TrussTopTemperature>=850.0 ? sprintf(reply,"%s TCOLLTOP=NOCOMM",reply) : 
     sprintf(reply,"%s TCOLLTOP=%0.1f",reply,TrussTopTemperature));

    (TrussBottomTemperature>=850.0 ? sprintf(reply,"%s TCOLLBOT=NOCOMM",reply) : 
     sprintf(reply,"%s TCOLLBOT=%0.1f",reply,TrussBottomTemperature));

    // Utility Box Temperatures and pressures (readout above)

    (glycolSupplyPressure>=850.0 ? sprintf(reply,"%s GSPRES=NOCOMM",reply) : 
     sprintf(reply,"%s GSPRES=%0.1f",reply,glycolSupplyPressure));
    
    (glycolSupplyTemperature>=850.0 ? sprintf(reply,"%s GSTEMP=NOCOMM",reply) : 
     sprintf(reply,"%s GSTEMP=%0.1f",reply,glycolSupplyTemperature));
    
    (glycolReturnPressure>=850.0 ? sprintf(reply,"%s GRPRES=NOCOMM",reply) : 
     sprintf(reply,"%s GRPRES=%0.1f",reply, glycolReturnPressure));
    
    (glycolReturnTemperature>=850.0 ? sprintf(reply,"%s GRTEMP=NOCOMM",reply) : 
     sprintf(reply,"%s GRTEMP=%0.1f",reply,glycolReturnTemperature));

    (utilBoxAirTemperature>=850.0 ? sprintf(reply,"%s IUBTAIR=NOCOMM",reply) :
     sprintf(reply,"%s IUBTAIR=%0.1f",reply,utilBoxAirTemperature));
    
    (outsideAirTemperature>=850.0 ? sprintf(reply,"%s AMBTEMP=NOCOMM",reply) : 
     sprintf(reply,"%s AMBTEMP=%0.1f",reply,outsideAirTemperature));
    
    (agwHeatSinkTemperature>=850.0 ? sprintf(reply,"%s AGHSTEMP=NOCOMM",reply) : 
     sprintf(reply,"%s AGHSTEMP=%0.1f",reply,agwHeatSinkTemperature));

    // WAGO HEB temperature sensors are on WAGO register addr 5 (4-channel RTD)

    (redHEBTemperature>=850.0 ? sprintf(reply,"%s RHEBTEMP=NOCOMM",reply) : 
     sprintf(reply,"%s RHEBTEMP=%0.1f",reply,redHEBTemperature));

    (redDewarTemperature>=850.0 ? sprintf(reply,"%s RDEWTEMP=NOCOMM",reply) : 
     sprintf(reply,"%s RDEWTEMP=%0.1f",reply,redDewarTemperature));

    (blueHEBTemperature>=850.0 ? sprintf(reply,"%s BHEBTEMP=NOCOMM",reply) : 
     sprintf(reply,"%s BHEBTEMP=%0.1f",reply,blueHEBTemperature));

    (blueDewarTemperature>=850.0 ? sprintf(reply,"%s BDEWTEMP=NOCOMM",reply) : 
     sprintf(reply,"%s BDEWTEMP=%0.1f",reply,blueDewarTemperature));

  } else if (!strcasecmp(who_selected,"HEB")) {

    memset(cmd_instruction,0,sizeof(cmd_instruction));

    GetArg(args,1,cmd_instruction);
    StrUpper(cmd_instruction);

    if (!strcasecmp(cmd_instruction,"RED") || !strcasecmp(cmd_instruction,"R")) {
      hebWAGO = rhebID;
      hebChan='R';
    } else if (!strcasecmp(cmd_instruction,"BLUE") || !strcasecmp(cmd_instruction,"B")) {
      hebWAGO = bhebID;
      hebChan = 'B';
    } else {
      sprintf(reply,"%s Invalid request '%s', Usage: HEB [R|B|RED|BLUE] ...", who_selected,cmd_instruction);
      return CMD_ERR;
    }

    // Read the power relay states. The 8-channel digital output
    // module has registar address 513 If a fault, HEB_x=OFF is the
    // mostly likely reason
    
    ierr = wagoSetGet(0,shm_addr->MODS.WAGOIP[hebWAGO],1,512,devOnOff,1);
    if (ierr<0) {
      sprintf(reply,"%s HEB_%c=OFF", who_selected,hebChan);
      if (hebChan == 'R') {
	shm_addr->MODS.redHEBState = 0; // and all its power must similarly be off
	shm_addr->MODS.redArchonState = 0;
	shm_addr->MODS.redIonGaugeState = 0;
      }
      else {
	shm_addr->MODS.blueHEBState = 0; // and all its power must similarly be off
	shm_addr->MODS.blueArchonState = 0;
	shm_addr->MODS.blueIonGaugeState = 0;
      }
      return CMD_OK;
    }
    allHEBPower = devOnOff[0];
    
    // Power relay states
    
    archonPower  = allHEBPower & 1;
    ionGaugePower = allHEBPower & 2;

    printf("allHEBPower=%d archonPower=%d, ionGaugePower=%d\n",allHEBPower,archonPower,ionGaugePower);
    
    // Temperatures (readout at the top of the command tree)

    if (hebChan == 'R') {
      hebAirTemp = redHEBTemperature;
      dewarTemp = redDewarTemperature;
    } else {
      hebAirTemp = blueHEBTemperature;
      dewarTemp = blueDewarTemperature;
    }

    // printf("hebAir=%.2f dewTemp=%.2f\n",hebAirTemp,dewarTemp);
    
    // second argument tells us what to do.  If missing it is an implicit STATUS query

    if (strlen(args) > 1) {
      GetArg(args,2,argbuf);
      StrUpper(argbuf);
    }
    else { 
      strcpy(argbuf,"STATUS");
    }
    
    if (!strcasecmp(argbuf,"STATUS") || strlen(args) <=1) {

      sprintf(reply,"%s",who_selected); // start reply

      // Archon controller power relay is normally open (off at instrument mains on)

      if (archonPower == 1) {
	sprintf(reply,"%s ARCHON_%c=ON",reply,hebChan);
	if (hebChan == 'R')
	  shm_addr->MODS.redArchonState = 1;
	else
	  shm_addr->MODS.blueArchonState = 1;
      }
      else {
	sprintf(reply,"%s ARCHON_%c=OFF",reply,hebChan);
	if (hebChan == 'R')
	  shm_addr->MODS.redArchonState = 0;
	else
	  shm_addr->MODS.blueArchonState = 0;
      }

      printf("reply=%s\n",reply);

      // Ion Gauge power relay is normally open
      
      if (ionGaugePower == 2) {
	sprintf(reply,"%s IGPOWER_%c=ON",reply,hebChan);
	if (hebChan == 'R')
	  shm_addr->MODS.redIonGaugeState = 1;
	else
	  shm_addr->MODS.blueIonGaugeState = 1;
      }
      else {
	sprintf(reply,"%s IGPOWER_%c=OFF",reply,hebChan);
	if (hebChan == 'R')
	  shm_addr->MODS.redIonGaugeState = 0;
	else
	  shm_addr->MODS.blueIonGaugeState = 0;
      }
      
      printf("reply=%s\n",reply);

      (hebAirTemp>=850.0 ? sprintf(reply,"%s %cHEBTEMP=NOCOMM",reply,hebChan) : 
       sprintf(reply,"%s HEBTEMP_%c=%0.1f",reply,hebChan,hebAirTemp));

      (dewarTemp>=850.0 ? sprintf(reply,"%s %cDEWTEMP=NOCOMM",reply,hebChan) : 
       sprintf(reply,"%s DEWTEMP_%c=%0.1f",reply,hebChan,dewarTemp));
      
      printf("reply=%s\n",reply);

      return CMD_OK;
      
    } else if (!strcasecmp(argbuf,"ARCHON")) {

      // Archon power relay is normally open
      
      sprintf(reply,"%s",who_selected);

      // If no additional arguments, implicit power status query
      // otherwise ON or OFF allowed
      
      if (strlen(args) > 2) {
	GetArg(args,3,argbuf);
	ierr = wagoSetGet(0,shm_addr->MODS.WAGOIP[hebWAGO],1,512,devOnOff,1);
	allHEBPower = devOnOff[0];
	if (!strcasecmp(argbuf,"ON")) {
	  devOnOff[0]=(short)(allHEBPower | 1);
	  ierr = wagoSetGet(1,shm_addr->MODS.WAGOIP[hebWAGO],1,512,devOnOff,1);
	}
	else if (!strcasecmp(argbuf,"OFF")) {
	  devOnOff[0]=(short)(allHEBPower ^ 1);
	  ierr = wagoSetGet(1,shm_addr->MODS.WAGOIP[hebWAGO],1,512,devOnOff,1);
	}
	else {
	  sprintf(reply,"%s unrecognized power state %s - must be ON or OFF",reply,argbuf);
	  return CMD_ERR;
	}
	MilliSleep(200);
      }

      ierr = wagoSetGet(0,shm_addr->MODS.WAGOIP[hebWAGO],1,512,devOnOff,1);
      allHEBPower = devOnOff[0];
      archonPower  = allHEBPower & 1;
      ionGaugePower = allHEBPower & 2;

      if (archonPower == 1) {
	sprintf(reply,"%s ARCHON_%c=ON",reply,hebChan);
	if (hebChan == 'R')
	  shm_addr->MODS.redArchonState = 1;
	else
	  shm_addr->MODS.blueArchonState = 1;
      }
      else {
	sprintf(reply,"%s ARCHON_%c=OFF",reply,hebChan);
	if (hebChan == 'R')
	  shm_addr->MODS.redArchonState = 0;
	else
	  shm_addr->MODS.blueArchonState = 0;
      }

    } else if (!strcasecmp(argbuf,"IGPOWER")) {
      
      // Ionization gauge power relay is normally open
      
      sprintf(reply,"%s",who_selected);

      // If no additional arguments, implicit power status query otherwise ON or OFF allowed
      
      if (strlen(args) > 2) {
	GetArg(args,3,argbuf);
	ierr = wagoSetGet(0,shm_addr->MODS.WAGOIP[hebWAGO],1,512,devOnOff,1);
	allHEBPower = devOnOff[0];
	if (!strcasecmp(argbuf,"ON")) {
	  devOnOff[0]=(short)(allHEBPower | 2);
	  ierr = wagoSetGet(1,shm_addr->MODS.WAGOIP[hebWAGO],1,512,devOnOff,1);
	}
	else if (!strcasecmp(argbuf,"OFF")) {
	  devOnOff[0]=(short)(allHEBPower ^ 2);
	  ierr = wagoSetGet(1,shm_addr->MODS.WAGOIP[hebWAGO],1,512,devOnOff,1);
	}
	else {
	  sprintf(reply,"%s unrecognized power state %s - must be ON or OFF",reply,argbuf);
	  return CMD_ERR;
	}
	MilliSleep(200);
      }

      ierr = wagoSetGet(0,shm_addr->MODS.WAGOIP[hebWAGO],1,512,devOnOff,1);
      allHEBPower = devOnOff[0];
      archonPower  = allHEBPower & 1;
      ionGaugePower = allHEBPower & 2;

      if (ionGaugePower == 2) {
	sprintf(reply,"%s IGPOWER_%c=ON",reply,hebChan);
	if (hebChan == 'R')
	  shm_addr->MODS.redIonGaugeState = 1;
	else
	  shm_addr->MODS.blueIonGaugeState = 1;
      }
      else {
	sprintf(reply,"%s IGPOWER_%c=OFF",reply,hebChan);
	if (hebChan == 'R')
	  shm_addr->MODS.redIonGaugeState = 0;
	else
	  shm_addr->MODS.blueIonGaugeState = 0;
      }
      
      return CMD_OK;

    } else if (!strcasecmp(argbuf,"TEMP") || !strcasecmp(argbuf,"TEMPS")) {

      sprintf(reply,"%s",who_selected);
      
      (hebAirTemp>=850.0 ? sprintf(reply,"%s %cHEBTEMP=NOCOMM",reply,hebChan) : 
       sprintf(reply,"%s HEBTEMP_%c=%0.1f",reply,hebChan,hebAirTemp));

      (dewarTemp>=850.0 ? sprintf(reply,"%s %cDEWTEMP=NOCOMM",reply,hebChan) : 
       sprintf(reply,"%s DEWTEMP_%c=%0.1f",reply,hebChan,dewarTemp));

      return CMD_OK;

    } else {
      sprintf(reply,"%s invalid option '%s': usage: HEB R|B [[archon|ipower] on|off]",who_selected,argbuf); 
      return CMD_ERR;
      
    }
    
  } else {
    sprintf(reply,"%s Invalid '%s' command",who_selected,argbuf); 
    return CMD_ERR;
  }
 
  return CMD_OK;
}

/* ---------------------------------------------------------------------------
//
// open - Open all or 1 comm port
//
*/

/*!  
  \brief OPEN command - open all or 1 comm port
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  open [n|all]
  
  Opens all or a named comm port.

*/
int
cmd_open(char *args, MsgType msgtype, char *reply)
{
  int ierr;
  int unit;
  char dummy[PAGE_SIZE];
  char who_selected[24];

  strcpy(who_selected,cmdtab[commandID].cmd);
  StrUpper(who_selected);

  GetArg(args,1,argbuf);
  StrUpper(argbuf);

  if(!strcasecmp(argbuf,"ALL")) { // do them all
    for(unit=0;unit<MAX_ML-1;unit++) {
      if(shm_addr->MODS.host[unit]==0 ||
	 !strncasecmp(shm_addr->MODS.who[unit],"rimcs",5) || // for agwServer
	 !strncasecmp(shm_addr->MODS.who[unit],"bimcs",5) || // for agwServer
	 !strncasecmp(shm_addr->MODS.who[unit],"agw",3)) { // for agwServer
	;
      } else {
	if(unit!=MAX_ML-1) {
	  CloseTTYPort(&shm_addr->MODS.commport[unit]); // Close
	  MilliSleep(100);
	  OpenTTYPort(&shm_addr->MODS.commport[unit]); // Open
	}
      }
    }
    strcpy(dummy,"all");  // Reset and Open all AGW COMM ports
    ierr=agwcu("localhost",0,"open ",dummy);
    if(ierr!=0) {
      sprintf(reply,"%s", dummy);
      return CMD_ERR;
    }

    sprintf(reply,"%s All mmcService and agwServer communications are opened",
	    who_selected);

  } else if(!strcasecmp(argbuf,"MMC")) { // do them all

    for(unit=0;unit<MAX_ML-1;unit++) {
      if(shm_addr->MODS.host[unit]==0 ||
	 !strncasecmp(shm_addr->MODS.who[unit],"rimcs",5) || // for agwServer
	 !strncasecmp(shm_addr->MODS.who[unit],"bimcs",5) || // for agwServer
	 !strncasecmp(shm_addr->MODS.who[unit],"agw",3)) { // for agwServer
	;
      } else {
	if(unit!=MAX_ML-1) {
	  CloseTTYPort(&shm_addr->MODS.commport[unit]); // Close
	  MilliSleep(100);
	  OpenTTYPort(&shm_addr->MODS.commport[unit]); // Open
	}
      }
    }
    sprintf(reply,"%s mmcService communications are opened",who_selected);

  } else if(!strcasecmp(argbuf,"AGW")) { // do them all

    strcpy(dummy,"all");  // Reset and Open all AGW COMM ports
    ierr=agwcu("localhost",0,"open ",dummy);
    if(ierr!=0) {
      sprintf(reply,"%s AGW=FAULT %s",who_selected,dummy);
      return CMD_ERR;
    }
    sprintf(reply,"%s AGW=OPEN agwServer communications are opened",who_selected);

  } else {
    GetArg(args,1,argbuf);
    unit=getMechanismID(argbuf,dummy); // Get mechanism device ID

    if(unit==-1) {
      sprintf(reply,"%s %s",who_selected,dummy);
      return CMD_ERR;
    }

    if(!strncasecmp(shm_addr->MODS.who[unit],"agw",3)) { // for agwServer
      strcpy(dummy,argbuf);  // Reset and Open all AGW COMM ports
      ierr=agwcu("localhost",0,"open ",dummy);

      sprintf(reply,"%s",dummy);
      if(ierr!=0) return CMD_ERR;

      return CMD_OK;
    }

    CloseTTYPort(&shm_addr->MODS.commport[unit]);
    MilliSleep(1000);

    if(OpenTTYPort(&shm_addr->MODS.commport[unit])<0) {
      sprintf(reply,"%s Can not open %s COMM port, reason %s",
	      who_selected,shm_addr->MODS.who[unit],strerror(errno));
      return CMD_ERR;
    } else {
      sprintf(reply,"%s Opened %s COMM port",
	      who_selected,shm_addr->MODS.who[unit]);
    }
  }
  return CMD_OK;
}

/* ---------------------------------------------------------------------------
//
// close - Close a comm port
//
*/

/*!  
  \brief CLOSE command - close a comm port
  re-initialize the microstep drive
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  close port
  
  Closes a named comm port.

*/
int
cmd_close(char *args, MsgType msgtype, char *reply)
{
  int ierr;
  int unit;
  char dummy[PAGE_SIZE];
  char who_selected[24];

  strcpy(who_selected,cmdtab[commandID].cmd);
  StrUpper(who_selected);

  GetArg(args,1,argbuf);
  StrUpper(argbuf);

  if(!strcasecmp(argbuf,"ALL")) { // do them all
    for(unit=0;unit<MAX_ML-1;unit++) {
      if(shm_addr->MODS.host[unit]==0 ||
	 !strncasecmp(shm_addr->MODS.who[unit],"agw",3));
      else
	if(unit!=MAX_ML-1)
	  CloseTTYPort(&shm_addr->MODS.commport[unit]);
    }
    strcpy(dummy,"close");  // Reset and Open all AGW COMM ports
    ierr=agwcu("localhost",0,"mmcIC 0 ",dummy);
    if(ierr!=0) {
      sprintf(reply,"%s", dummy);
      return CMD_ERR;
    }

    sprintf(reply,"%s All mmcService communications are closed",who_selected);

  } else {

    GetArg(args,1,argbuf);

    unit=getMechanismID(argbuf,dummy); // Get mechanism device ID

    if(!strncasecmp(shm_addr->MODS.who[unit],"agw",3)) { // for agwServer

      strcpy(dummy,argbuf);  // Reset and Open all AGW COMM ports
      ierr=agwcu("localhost",0,"close ",dummy);

      sprintf(reply,"%s",dummy);
      if(ierr!=0) return CMD_ERR;

      return CMD_OK;
    }

    if(unit==-1) {
      sprintf(reply,"%s %s",who_selected,dummy);
      return CMD_ERR;
    }

    CloseTTYPort(&shm_addr->MODS.commport[unit]);
    sprintf(reply,"%s CLOSED %s COMM closed",who_selected,
	    shm_addr->MODS.who[unit]);
  } 

  return CMD_OK;
}

/* ---------------------------------------------------------------------------
//
// setport - Set/Query comm port attributes
//
*/

/*!  
  \brief SETPORT command - set/query comm port attributes
  re-initialize the microstep drive
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  setport [speed databits stopbits parity]
  
  Sets/Queries the attributes of the open comm port.  Also reports if the commport
  is open or not.

*/
int
cmd_setport(char *args, MsgType msgtype, char *reply)
{
  int i;
  char who_selected[24];

  strcpy(who_selected,cmdtab[commandID].cmd);
  StrUpper(who_selected);

  strcpy(reply,"SETPORT command not yet implemented");
  return CMD_OK;

  for(i=0;i<MAX_ML-1;i++) {
    if (shm_addr->MODS.commport[i].FD>0) {
      sprintf(reply,"%s COMMPORT=%s Open",
	      who_selected,shm_addr->MODS.commport[i].Port);
      switch (shm_addr->MODS.commport[i].Interface) {
      case TTY_SERIAL:
	sprintf(reply,"%s Speed=%d DataBits=%d StopBits=%d Parity=%d",
		reply,shm_addr->MODS.commport[i].Speed,shm_addr->MODS.commport[i].DataBits,
		shm_addr->MODS.commport[i].StopBits,
		shm_addr->MODS.commport[i].Parity);
	break;

      case TTY_NETWORK:
	strcat(reply," Interface=NET");
	break;
	
      default:
	strcat(reply," Interface=UNKNOWN");
	break;
      }
    } 
    else {
      sprintf(reply,"%s %s=CLOSED",who_selected,shm_addr->MODS.commport[i].Port);
    }
  }
  
}

/* ---------------------------------------------------------------------------
//
// moverel - Send a relative move to MicroLYNX controller device.
//
*/

/*!  
  \brief moverel command - relative move command +/-value
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  moverel [mechanism val]

  mechanism - Mechanism name 
 
  val - relative position +/-value

  Send relative move MicroLYNX controller command via network or serial.
  MicroLYNX Controller commands given in MODS Mechanism Commands doc.
  http://www.astronomy.ohio-state.edu/~rgonzale/MODS_Mechanism_Commands.pdf

*/
int
cmd_moverel(char *args, MsgType msgtype, char *reply)
{
  char mechanism_name[20];
  int device;
  int nval;
  double val;
  char ret[24];
  char dummy[PAGE_SIZE];
  char who_selected[24];

  strcpy(who_selected,cmdtab[commandID].cmd);
  StrUpper(who_selected);

  memset(ret,0,sizeof(ret));

  //  StrUpper(args);

  strcpy(ret,"MOVR ");
  nval = sscanf(args,"%s %s",mechanism_name,&ret[5]);
  val=atof(&ret[5]);

  /* Search for the IP and Socket */
  device=getMechanismID(mechanism_name,dummy); // Get mechanism device ID
  if(device==-1) {
    sprintf(reply,"%s %s",who_selected,dummy);
    return CMD_ERR;
  }

  if (nval==1) { // Query when no command is issued
    sprintf(reply,"%s Mechanism at position %0.3f",who_selected,
	    positionToShrMem(device,reply));
    return CMD_OK;
  }

  if(strstr(args,"HELP")) { // ask for help
    sprintf(reply,"%s [mechanism val]",who_selected);
    return CMD_OK;
  }

  sendCommand(device,ret,dummy); //  Send a command
  sprintf(reply,"%s %s",who_selected,dummy);

  return CMD_OK;
}

/* ---------------------------------------------------------------------------
//
// moveabs - Send an absolute move to MicroLYNX controller device.
//
*/

/*!  
  \brief moveabs command - absolute command move
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  moveabs [val]

  mechanism - Mechanism name 

  val - relative position +/-value


  Send absolute move MicroLYNX controller command via network or serial.
  MicroLYNX Controller commands given in MODS Mechanism Commands doc.
  http://www.astronomy.ohio-state.edu/~rgonzale/MODS_Mechanism_Commands.pdf

*/
int
cmd_moveabs(char *args, MsgType msgtype, char *reply)
{
  char mechanism_name[20];
  int device;
  int nval;
  double val;
  char ret[24];
  char dummy[PAGE_SIZE];
  char who_selected[24];

  strcpy(who_selected,cmdtab[commandID].cmd);
  StrUpper(who_selected);

  //  StrUpper(args);

  strcpy(ret,"MOVA ");
  nval = sscanf(args,"%s %s",mechanism_name,&ret[5]);
  val=atof(&ret[5]);

  /* Search for the IP and Socket */
  device=getMechanismID(mechanism_name,dummy); // Get mechanism device ID
  if(device==-1) {
    sprintf(reply,"%s %s",who_selected,dummy);
    return CMD_ERR;
  }

  if (nval==1) { // Query when no command is issued
    sprintf(reply,"%s Mechanism at position %0.3f",who_selected,
	    positionToShrMem(device,reply));
    return CMD_OK;
  }

  if(strstr(args,"HELP")) { // ask for help
    sprintf(reply,"%s [mechanism val]",who_selected);
    return CMD_OK;
  }
  
  sendCommand(device,ret,reply); //  Send a command to the mechanism

  return CMD_OK;
}

//
#if 0 // cmd_linear
int
cmd_linear(char *args, MsgType msgtype, char *reply)
{
  int device;
  int bit_nomove;
  int bit_mask;
  char filter_selected[24];
  int dval;
  char dummy[PAGE_SIZE];
  char cmd_instruction[PAGE_SIZE];
  char who_selected[24];
  char mlccmd[24];

  memset(cmd_instruction,0,sizeof(cmd_instruction));

  strcpy(who_selected,cmdtab[commandID].cmd);
  StrUpper(who_selected);

  bit_nomove=bit_mask=dval=0;
  memset(dummy,0,sizeof(dummy));

  StrUpper(args);
  GetArg(args,1,argbuf);

  /* Search for the IP and Socket */
  device=getMechanismID(cmdtab[commandID].cmd,dummy);// Get mechanism device ID
  if(device==-1) {
    sprintf(reply,"%s %s",who_selected,dummy);
    return CMD_ERR;
  }

  if(!strcasecmp(argbuf,"ABORT")) {
    ierr = mlcStopMechanism(device,dummy); //  STOP the operation
    sprintf(reply,"%s %s",who_selected,dummy);
    return CMD_OK;
  }

  if (mlcBusy(device,dummy)) { // Busy?
    sprintf(reply,"%s %s",who_selected,dummy);
    return CMD_ERR;
  }

  if (strncasecmp(args,"M#",2)) { // check for low-level command
    ierr = mlcCheckBits(device,dummy); // check limit bits 21,22
    if((ierr&3)== 3 || ierr==-1) {
      sprintf(reply,"%s %s",who_selected,dummy);
      return CMD_ERR;
    }
  }

  if (strncasecmp(args,"M#",2)) { // check for low-level command
    memset(dummy,0,sizeof(dummy));
    ierr=checkPower(device,dummy); // check mechanism power 
    if(ierr!=0) {
      sprintf(reply,"%s %s", who_selected,dummy);
      return CMD_ERR;
    }
  }

  if (strlen(args)<=0) {  // Query when no command is issued
    //shm_addr->MODS.pos[device]=positionToShrMem(device,dummy);
    sprintf(reply,"%s=%d",who_selected,(int)shm_addr->MODS.pos[device]);
    return CMD_OK;

  }

  dval=atoi(argbuf)-1;

  if (!strncasecmp(args,"M#",2)) { // check for low-level command

    ierr=mlcTechCmd(device,args,who_selected,reply);
    if(ierr<=-1) return CMD_ERR;
    return CMD_OK;

  } else if(!strcasecmp(argbuf,"RESET")) {

    sendCommand(device,"INITIAL",dummy);

    shm_addr->MODS.pos[device]=positionToShrMem(device,dummy);
    sprintf(reply,"%s=1",who_selected);

  } else if (dval>=0 && dval<8) { 
    strcpy(dummy,args);
    sprintf(filter_selected,"TARGNUM=%d",dval);
    sendTwoCommand(device,filter_selected,"BEGIN",dummy);

    if(ierr!=0) {
      sprintf(reply,"%s",dummy);
      return CMD_ERR;
    }

    ierr = mlcCheckBits(device,dummy); // check limit bits 21,22
    if((ierr&3)== 3 || ierr==-1) {
      sprintf(reply,"%s %s",who_selected,dummy);
      return CMD_ERR;
    }

    sprintf(reply,"%s=%s",who_selected, args);

    shm_addr->MODS.pos[device]=positionToShrMem(device,dummy);
    
  } else {
    sprintf(reply,"%s Invalid request '%s', valid range is %d..%d",
	    who_selected,args,
	    (int)(shm_addr->MODS.min[device]),
	    (int)(shm_addr->MODS.max[device]));
    return CMD_ERR;
  }

  return CMD_OK;
}
#endif // end of cmd_linearMechanism

/* ---------------------------------------------------------------------------
//
// hatch - hatch MicroLYNX controller.
//
*/

/*!  
  \brief hatch command - command the hatch to Open or Close
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  hatch [open|close]

  open - Open the MODS hatch or darkslide and let photons enter\n
  close - Close the MODS hatch or darkslide\n

  \par Description: Opens or closes the instrument dark hatch.

  \par Interactions:
  Requires an executive override to open the dark hatch if any of the 
  calibration lamps are turned on.

  Send hatch MicroLYNX controller command via network or serial.
  MicroLYNX Controller commands given in MODS Mechanism Commands doc.
  http://www.astronomy.ohio-state.edu/~rgonzale/MODS_Mechanism_Commands.pdf

*/

int
cmd_hatch(char *args, MsgType msgtype, char *reply)
{
  int device        = 0;
  int io20_hatch    = 0;
  int bit_nomove    = 0;
  int passive_cmd   = 0;

  char who_selected[24];
  char dummy[PAGE_SIZE]; 
  char cmd_instruction[PAGE_SIZE];

  memset(cmd_instruction, 0, sizeof(cmd_instruction));
  memset(dummy, 0, sizeof(dummy));
  StrUpper(args);

  strcpy(who_selected, cmdtab[commandID].cmd);
  StrUpper(who_selected);

  // Search for the IP and Socket

  device = getMechanismID(cmdtab[commandID].cmd, dummy);

  if ( device == -1 ) {
    sprintf(reply, "%s %s=%s", who_selected, who_selected, dummy);
    return CMD_ERR;
  }

  GetArg(args, 1, cmd_instruction);

  if ( strcasecmp(cmd_instruction,"LOCK") == 0 ) { // Abort process
    shm_addr->MODS.LOCKS[device] = 1;
    sprintf(reply, " %s %s=LOCK mechanism is locked",
	    who_selected, who_selected);
    return CMD_OK;

  } else if ( strcasecmp(cmd_instruction,"UNLOCK") == 0 ) { // Abort process
    sprintf(reply," %s %s=UNLOCK mechanism is unlocked",
	    who_selected,who_selected);

    shm_addr->MODS.LOCKS[device] = 0;
    return CMD_OK;
  
  }

  if( shm_addr->MODS.host[device] == 0 ) {
    sprintf(reply," %s %s=NOCOMM No IP address configured, check mechanisms.ini file", who_selected, who_selected);
    return CMD_ERR;


  } else if ( shm_addr->MODS.LOCKS[device] ) {
    sprintf(reply, " %s %s=FAULT connection LOCKED OUT, '%s UNLOCK' to continue", who_selected, who_selected, who_selected);
    return CMD_ERR;

  }

  // Check if the microLynx is ON

  ierr = mlcQuery(device, 1, dummy); 

  if ( ierr != 0 ) {
    sprintf(reply, "%s %s", who_selected, dummy);
    return CMD_ERR;

  }

 // STOP MicroLynx Controller(MLC) operation

  if ( strcasecmp(cmd_instruction,"ABORT") == 0 ) {
    ierr = mlcStopMechanism(device,dummy);
    sprintf(reply,"%s %s",who_selected,dummy);
    return CMD_OK;

  }

  // Check for Busy
  if ( mlcBusy(device,dummy) ) {
    sprintf(reply, "%s %s", who_selected, dummy);
    return CMD_ERR;

  }

  // Check configuration 

  if ( strcasecmp(cmd_instruction,"CONFIG") == 0 ) {
    mlcMechanismConfig(device, who_selected, dummy);
    sprintf(reply, "%s", dummy);
    return CMD_OK;

  }
  
  io20_hatch = mlcBitsBase10(device, "PRINT IO 22,IO 21", dummy);

  if (strlen(args)<=0) { // Query when no command is issued

    shm_addr->MODS.pos[device] = positionToShrMem(device, dummy);

    if ( io20_hatch == 0 ) {
      sprintf(reply," %s HATCH=AJAR The dark hatch is partially open, or sensor problem, Reset to recover", who_selected);

      sprintf(shm_addr->MODS.state_word[device], "AJAR");
      shm_addr->MODS.pos[device] = positionToShrMem(device, dummy);
      return CMD_ERR;

    } else if ( io20_hatch == 1 ) {
      sprintf(reply, "%s HATCH=OPEN", who_selected);
      shm_addr->MODS.pos[device] = (float)io20_hatch;
      sprintf(shm_addr->MODS.state_word[device], "OPEN");

    } else if(io20_hatch==2) {
      sprintf(reply,"%s HATCH=CLOSED",who_selected);
      shm_addr->MODS.pos[device] = (float)io20_hatch;
      sprintf(shm_addr->MODS.state_word[device], "CLOSED");

    } else {
      sprintf(reply, "%s HATCH=FAULT Sensor Fault, both limits asserted", who_selected);
      sprintf(shm_addr->MODS.state_word[device], "FAULT");
      shm_addr->MODS.pos[device] = (float)io20_hatch;
      return CMD_ERR;

    }

    return CMD_OK;
  }

  // Initialize(reset)  hatch
  
  if ( strcasecmp(cmd_instruction, "RESET") == 0 ) {

    ierr = mlcBitsBase10(device,"PRINT IO 22,IO 21",dummy);

    if ( ierr == 2 ) { // bit 22 is on, hatch already reset and closed

      rawCommand(device, "PWRFAIL=0", dummy);
      sprintf(reply, "%s HATCH=CLOSED", who_selected);
      strcpy(shm_addr->MODS.state_word[device],"CLOSED"); 

      return CMD_OK;
    }
      
    ierr = mlcReset(device,who_selected,BI_STATE, "INITIAL", dummy);
    sprintf(reply, "%s", dummy);

    if ( ierr < 0 ) return CMD_ERR;

    return CMD_OK;
  }

  if ( strncasecmp(args,"M#",2) != 0 ) { // check for low-level command
    ierr = checkPower(device, dummy); // Check mechanism power bit.

    if ( ierr != 0 ) {
      sprintf(reply, "%s %s", who_selected, dummy);
      return CMD_ERR;
    }
  }
  
  if ( strncasecmp(args, "M#", 2) != 0 ) { // check for low-level command
    ierr = mlcBitsBase10(device,"PRINT IO 22,IO 21",dummy);
    if ( ierr == 3 ) {
      sprintf(reply, "%s %s=FAULT Sensor Fault, both limits asserted", who_selected, who_selected);
      return CMD_ERR;
    }
  }

  if ( strcasecmp(cmd_instruction, "?") == 0 ) {
     sprintf(reply, "Usage: %s [reset|step|open|close] ", who_selected);
     return CMD_OK;

  }

  if ( strcasecmp(cmd_instruction, "STEP") == 0 ) {
    // STEP behavior, since this is a two-state system, it swaps
    // back and forth between states. If open, then close.
    // If closed, then open. 
    io20_hatch = mlcBitsBase10(device,"PRINT IO 22,IO 21",dummy);
    shm_addr->MODS.pos[device] = io20_hatch;

    switch ( io20_hatch ) {
    case 0: 

      // send back an error if the hatch is partially ajar

      sprintf(reply, "%s HATCH=AJAR The dark hatch is partially open", who_selected);
      sprintf(shm_addr->MODS.state_word[device], "AJAR");
      shm_addr->MODS.pos[device] = io20_hatch;
      return CMD_ERR;

    case 1: 

      // If the hatch is open, close the hatch

      ierr = sendCommand(device, "CLOSE", dummy);
      sprintf(reply, "%s HATCH=%s", who_selected,dummy);

      if ( ierr != 0 ) return CMD_ERR;
  
      sprintf(shm_addr->MODS.state_word[device], "CLOSED");
      shm_addr->MODS.pos[device] = 2.0;
      break;

    case 2: 

      // If the hatch is closed, then open the hatch 

      ierr = sendCommand(device, "OPEN", dummy);
      sprintf(reply, "%s HATCH=%s", who_selected, dummy);
      if ( ierr != 0 ) return CMD_ERR;

      sprintf(shm_addr->MODS.state_word[device], "OPEN");
      shm_addr->MODS.pos[device] = 1.0;
      break;

    case 3: 
      // Send back a sensor error if this is the case

      sprintf(reply,"%s HATCH=FAULT Sensor Fault, both limits asserted", who_selected);
      sprintf(shm_addr->MODS.state_word[device], "FAULT");
      shm_addr->MODS.pos[device] = 3.0;
      return CMD_ERR;

    default:
      break;
    }
    return CMD_OK;
  }

  if ( strcasecmp(cmd_instruction, "RDBITS") == 0 ) {

    io20_hatch = mlcBitsBase10(device, "PRINT IO 22,IO 21", dummy);

    if ( io20_hatch == 0 ) 
      sprintf(reply, "%s %s=AJAR BITS=%s b22-b21'", who_selected, who_selected, dummy);
    else if ( io20_hatch == 1 ) 
      sprintf(reply, "%s %s=OPEN BITS=%s b22-b21", who_selected, who_selected, dummy);
    else if ( io20_hatch == 2 ) 
      sprintf(reply,"%s %s=CLOSED BITS=%s b22-b21", who_selected, who_selected, dummy);
    else
      sprintf(reply, "%s %s=FAULT BITS=%s b22-b21'", who_selected, who_selected, dummy);

    return CMD_OK;
  }

  GetArg(args, 1, cmd_instruction);

  if ( io20_hatch == 0 ) {
    sprintf(reply, "%s HATCH=AJAR The dark hatch is partially open, or sensor problem, Reset to recover", who_selected);
    return CMD_ERR;
  }

  if ( strcasecmp(cmd_instruction, "OPEN") == 0 ) {
    ierr = sendCommand(device, "OPEN", dummy); //  Send a command to the hatch
    sprintf(reply, "%s HATCH=%s", who_selected, dummy);
    if ( ierr != 0 ) return CMD_ERR;

    sprintf(shm_addr->MODS.state_word[device], "OPEN");
    shm_addr->MODS.pos[device] = io20_hatch;
    return CMD_OK;
    
  } else if ( strcasecmp(cmd_instruction, "CLOSE") == 0 ) {

    ierr = sendCommand(device, "CLOSE", dummy); //  Send a command to the hatch
    sprintf(reply, "%s HATCH=%s", who_selected, dummy);
    if ( ierr != 0 ) return CMD_ERR;

    sprintf(shm_addr->MODS.state_word[device], "CLOSED");
    shm_addr->MODS.pos[device] = io20_hatch;
    return CMD_OK;

  } else if ( strncasecmp(args,"M#",2) == 0 ) { // check for low-level command

    ierr=mlcTechCmd(device, args, who_selected, reply);
    if ( ierr <= -1 ) return CMD_ERR;
    return CMD_OK;

  } else {
    sprintf(reply, "%s Invalid request '%s', Usage: hatch [open|close]", who_selected, cmd_instruction); 
    return CMD_ERR;

  }

  return CMD_OK;
}

/* ---------------------------------------------------------------------------
//
// calib - calib MicroLYNX controller.
//
*/

/*!  
  \brief calib command - command the Calibration Tower
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  calib [in|out]

  in - Move calibration tower into beam\n
  close - Move calibration tower out of beam

  \par Description:
  Inserts the calibration tower into the beam to take calibration data
  (spectral-line or continuum lamps), or retract the tower and stow it 
  out of the beam.
  
  \par Interactions:
  Hardware and software interlocks with the AGW Stage: the AGW Stage X and Y
  positions must be HOME before the calibration tower may be moved into the 
  beam. An executive override is required to retract the calibration tower if
  any of the calibration lamps are still turned on.
  
  Send Calibration Tower MicroLYNX controller command via network or serial.
  MicroLYNX Controller commands given in MODS Mechanism Commands doc.
  http://www.astronomy.ohio-state.edu/~rgonzale/MODS_Mechanism_Commands.pdf

*/
int
cmd_calib(char *args, MsgType msgtype, char *reply)
{
  int len         = 0;            // command string length
  int AGWFD       = 0;            // MicroLynx Controller(MLC) device
  int device      = 0;            // MicroLynx Controller(MLC) device
  int writeErr    = -1;           // could not write to device
  int blimit      = 0;            // Calibration Tower limit switches

  char who_selected[24];
  char temp[79];                   // temp character array
  char dummy[PAGE_SIZE];           // dummy character array
  char cmd_instruction[PAGE_SIZE]; // instruction for MLC command
  char cmd_instruction2[PAGE_SIZE];// 2nd instruction for MLC command

  memset(dummy, 0, sizeof(dummy));   // Empty the character arrays
  memset(cmd_instruction, 0, sizeof(cmd_instruction));

  GetArg(args, 1, cmd_instruction);  // get the first argument in 'args' string
  StrUpper(cmd_instruction);         // Convert to upper case

  strcpy(who_selected, cmdtab[commandID].cmd); // Get key command name
  StrUpper(who_selected);                      // Make sure it is uppercase

  /* Search for the IP and Socket */
  device = getMechanismID(cmdtab[commandID].cmd, dummy); // Get mechanism ID

  if ( device == -1 ) { // Check for mechanism ID errors
    sprintf(reply, "%s %s", who_selected, dummy);
    return CMD_ERR;
  }

  if ( !strcasecmp(cmd_instruction, "ABORT") ) { // Abort process
    ierr = mlcStopMechanism(device, dummy);     // STOP the operation
    sprintf(reply, "%s %s", who_selected, dummy);
    return CMD_OK;
  }

  if ( !strcasecmp(cmd_instruction, "CONFIG") ) { // get mechanism ip

    mlcMechanismConfig(device,who_selected,dummy);
    sprintf(reply,"%s",dummy);
    return CMD_OK;

  }

  ierr = mlcQuery(device, 1, dummy); // Check if the microLynx is ON

  if ( ierr != 0 ) {
    sprintf(reply, "%s %s", who_selected, dummy);
    return CMD_ERR;
  }

  if ( mlcBusy(device,dummy) ) { // Check for busy
    sprintf(reply, "%s %s", who_selected, dummy);
    return CMD_ERR;
  }

  if ( !strcasecmp(cmd_instruction, "LOCK") ) { // Lock the Calibration Tower
    shm_addr->MODS.LOCKS[device] = 1;
    sprintf(reply, "%s %s=LOCK mechanism is locked",
	    who_selected, who_selected);
    return CMD_OK;

  } else if ( !strcasecmp(cmd_instruction, "UNLOCK") ) { // Unlock
    shm_addr->MODS.LOCKS[device] = 0;
    sprintf(reply,"%s %s=UNLOCK mechanism is unlocked",
	    who_selected, who_selected);
    return CMD_OK;

  }

  if ( !shm_addr->MODS.host[device] ) {
    sprintf(reply, " %s %s=NOCOMM No IP address configured, check mechanisms.ini file", who_selected, who_selected);
    return CMD_ERR;

  } else if ( shm_addr->MODS.LOCKS[device] ) {
    sprintf(reply, " %s %s=FAULT connection LOCKED OUT, %s UNLOCK to continue", who_selected, who_selected, who_selected);
    return CMD_ERR;

  }

  if ( strlen(args) <= 0 ) {  // Query when no command is issued

    blimit = mlcBitsBase10(device, "PRINT IO 22,IO 21", dummy); // check bits

    if ( blimit == 1 ) {
      sprintf(reply, "CALIB CALIB=OUT"); 
      strcpy(shm_addr->MODS.state_word[device], "OUT");
      ierr = agwcu("localhost",0,"calib out",dummy); // Send Calibration Tower

    } else if ( blimit == 2 ) {
      sprintf(reply, "CALIB CALIB=IN");
      strcpy(shm_addr->MODS.state_word[device], "IN");
      ierr = agwcu("localhost",0,"calib in",dummy); // Send Calibration Tower

    } else if ( blimit == 3 ) {  // Check limit switches
      sprintf(reply, "CALIB CALIB=UNKNOWN Calibration Tower out-of-position, must be reset to initialize");
      strcpy(shm_addr->MODS.state_word[device], "FAULT");
      return CMD_ERR;

    }
    return CMD_OK;

  }


  if ( strcasecmp(cmd_instruction, "RESET") == 0 ) { // Reset mechanism

    memset(dummy, 0, sizeof(dummy));   // Empty the character arrays
    ierr = mlcBitsBase10(device, "PRINT IO 21", dummy); // Check direction

    if( ierr == 1 ) { // bit 21 is on, calib already reset and out

      rawCommand(device, "PWRFAIL=0", dummy);
      sendCommand(device, "POS=0", dummy);      // Find/define position
      sprintf(reply, "%s %s=OUT", who_selected, who_selected);
      strcpy(shm_addr->MODS.state_word[device], "OUT"); 
      ierr = agwcu("localhost", 0, "calib out", dummy); // Send Calibration Tower

      return CMD_OK;
    }
      
    sendCommand(device, "PWRFAIL=0", dummy);      // Find/define position

    sendCommand(device, "STORAGE", dummy);      // Find/define position
    sprintf(reply, "%s CALIB=%s Reset Successful", who_selected, dummy);

    /* Set Calibration Tower bit OFF */
    ierr=agwcu("localhost",0,"calib out",dummy); // Send Calibration Tower

    if(ierr!=0) {
      sprintf(reply,"%s", dummy);
      return CMD_ERR;

    }

    // clear for AGW server operations
    sprintf(shm_addr->MODS.state_word[device], "OUT");
    shm_addr->MODS.pos[device] = (float)mlcBitsBase10(device, "PRINT IO 22,IO 21", dummy);
    ierr=agwcu("localhost",0,"calib out",dummy); // Send Calibration Tower

    return CMD_OK;

  }

  if ( strncasecmp(args, "M#", 2) ) { // check for low-level command
    ierr = checkPower(device, dummy); // check the mechanisms power

    if ( ierr != 0 ) {
      sprintf(reply, "%s %s", who_selected, dummy);
      return CMD_ERR;
    }
  }

  if ( !strcasecmp(cmd_instruction, "STEP") ) { // Step calibration in rev's
    //GetArg(args, 2, cmd_instruction2);

    sprintf(cmd_instruction, "MOVR %s", cmd_instruction);
    ierr = sendCommand(device, cmd_instruction, dummy);

    shm_addr->MODS.pos[device] = positionToShrMem(device, dummy);
    sprintf(reply,"%s CALIB=%0.3f", who_selected, shm_addr->MODS.pos[device]);
    return CMD_OK;

  } else if ( !strncasecmp(args, "M#", 2) ) { // check for low-level command

    ierr = mlcTechCmd(device,args,who_selected,dummy);
    sprintf(reply, "%s", dummy);

    if ( ierr <= -1 ) return CMD_ERR;
    return CMD_OK;
  }

  // Move Calibration Tower into the beam

  if ( strcasecmp(cmd_instruction, "IN") == 0 ) { 

    blimit = mlcBitsBase10(device, "PRINT IO 22,IO 21", dummy);

    if ( blimit == 2 ) {
      sprintf(reply, "CALIB CALIB=IN");
      ierr = agwcu("localhost",0,"calib in",dummy); // Send Calibration Tower

      return CMD_OK;

    } else if ( blimit == 0 ) {  // Check limit switches
      sprintf(reply, "CALIB CALIB=FAULT Calibration Tower out-of-position, must be reset to initialize");
      strcpy(shm_addr->MODS.state_word[device], "FAULT");
      return CMD_ERR;

    }

    memset(dummy,0,sizeof(dummy));   // Empty the character arrays

    /* Check the AGW Y stage and make sure that Y axis is stowed
    agwcu("localhost", 0, "calib", dummy);
    ierr = atoi(&dummy[6]);

    if ( ierr == 0 ) {
      sprintf(reply, "CALIB AGW Stage is in the science beam, move disallowed");
      return CMD_ERR;

    }
    */

    /* Check the AGW Y stage and make sure that Y axis is stowed */
    strcpy(dummy,"print io 21");
    ierr=agwcu("localhost",0,"mmcIC 2 ",dummy);

    if(ierr) {
      sprintf(reply,"AGWY=UNKNOWN %s",dummy);
      return CMD_ERR;
    }

    strcpy(dummy,"print io 21");
    ierr=agwcu("localhost",0,"mmcIC 2 ",dummy);
    
    if(!atoi(&dummy[5])) {
      sprintf(reply,"CALIB CALIB=OUT AGW Stage is in the science beam, move disallowed");
      return CMD_ERR;
      
    }

    ierr = sendCommand(device, "CALMODE", dummy); //  Science MODE command

    if ( ierr != 0 ) return CMD_ERR;

    blimit = mlcBitsBase10(device, "PRINT IO 22,IO 21", dummy); // get tell bit

    if ( blimit == 0 || blimit == 3 )
      sprintf(reply, "%s CALIB=FAULT Calibration Tower not in position", who_selected);

    strcpy(shm_addr->MODS.state_word[device], "IN");
    shm_addr->MODS.pos[device] = positionToShrMem(device,dummy);

    // Set Calibration Tower bit ON 
    memset(dummy, 0, sizeof(dummy));   // Empty the dummy character array
    agwcu("localhost", 0, "calib in", dummy);

    blimit = mlcBitsBase10(device, "PRINT IO 22,IO 21", dummy); // get tell bit

    if ( blimit == 1 ) sprintf(reply, "%s CALIB=OUT", who_selected);
    else if ( blimit == 2 ) sprintf(reply, "%s CALIB=IN", who_selected);
    
    // Override AGW software protection, move Calibration Tower into the beam

  } else if ( !strcasecmp(cmd_instruction, "IN_OR") ) { // AGW Override

    blimit = mlcBitsBase10(device, "PRINT IO 22,IO 21", dummy); // get tell bit

    if ( blimit == 2 ) {
      sprintf(reply, "CALIB CALIB=IN");
      return CMD_OK;

    } else if ( blimit == 0 ) {  // Check both sensors
      sprintf(reply, "CALIB CALIB=FAULT Calibration Tower out-of-position, must be reset to initialize");
      strcpy(shm_addr->MODS.state_word[device], "FAULT");

      return CMD_ERR;
    }

    ierr = sendCommand(device, "CALMODE", dummy); //  Science MODE command

    if ( ierr != 0 ) return CMD_ERR;

    strcpy(shm_addr->MODS.state_word[device], "IN");
    shm_addr->MODS.pos[device] = positionToShrMem(device,dummy);

    // Set Calibration Tower bit ON

    memset(dummy, 0, sizeof(dummy));   // Empty the dummy character array

    agwcu("localhost",0,"calib in",dummy);
    ierr = atoi(&dummy[6]);

    blimit = mlcBitsBase10(device, "PRINT IO 22,IO 21", dummy); // get tell bit

    if( blimit == 1 ) sprintf(reply, "%s CALIB=OUT", who_selected);
    else if( blimit == 2 ) sprintf(reply, "%s CALIB=IN", who_selected);

    // Move the Calibration Tower out of the beam for Science Mode

  } else if (!strcasecmp(cmd_instruction,"OUT")) {

    blimit = 0;
    blimit = mlcBitsBase10(device, "PRINT IO 22,IO 21", dummy);

    if ( blimit == 1 ) {
      sprintf(reply, "CALIB CALIB=OUT");
      ierr = agwcu("localhost",0,"calib out",dummy); // Send Calibration Tower
      return CMD_OK;

    } else if ( blimit == 0 ) {  // Check limit switches
      sprintf(reply, "CALIB CALIB=FAULT Calibration Tower out-of-position, must be reset to initialize");
      strcpy(shm_addr->MODS.state_word[device], "FAULT");
      return CMD_ERR;

    }

    ierr = sendCommand(device, "SCIMODE", dummy); //  Calibration MODE command

    blimit = 0;
    blimit = mlcBitsBase10(device, "PRINT IO 22,IO 21",dummy);

    if ( blimit != 1 )
      sprintf(reply, "%s CALIB=FAULT Calibration Tower not in position", who_selected);

    strcpy(shm_addr->MODS.state_word[device], "OUT");
    shm_addr->MODS.pos[device] = positionToShrMem(device,dummy);


    /* Set Calibration Tower bit OFF */
    ierr = agwcu("localhost", 0, "calib out", dummy);
    ierr = atoi(&dummy[6]);

    blimit = mlcBitsBase10(device, "PRINT IO 22,IO 21", dummy); // get tell bit

    if ( blimit == 1 ) {
      sprintf(reply, "%s CALIB=OUT", who_selected);
      ierr = agwcu("localhost",0,"calib out",dummy); // Send Calibration Tower

    } else if ( blimit == 2 ) {
      sprintf(reply, "%s CALIB=IN", who_selected);
      ierr = agwcu("localhost",0,"calib in",dummy); // Send Calibration Tower
    }

  } else if ( strcasecmp(cmd_instruction,"RDBITS") == 0 ) {

    blimit = mlcBitsBase10(device, "PRINT IO 22,IO 21", dummy); // get tell bit
    
    if ( blimit == 1 ) 
      sprintf(reply, "%s %s=OUT BITS=%s b22-b21, CW sensor asserted", who_selected, who_selected, dummy);

    else if ( blimit == 2 ) 
      sprintf(reply, "%s %s=IN BITS=%s b22-b21, CCW sensor asserted", who_selected, who_selected, dummy);

    else if ( blimit == 3 ) 
      sprintf(reply, "%s %s=FAULT BITS=%s Both CW and CCW are asserted", who_selected, who_selected, dummy);

    else
      sprintf(reply, "%s %s=UNKNOWN BITS=%s No Sensors asserted", who_selected, who_selected, dummy);

    return CMD_OK;

  } else {
    sprintf(reply, "%s Invalid request '%s', Usage: calib [in|out]", who_selected, cmd_instruction);
    return CMD_ERR;

  }

  return CMD_OK;
}

// cmd_readmask() - barcode reader - retired 2025 June [rwp/osu]

/* ---------------------------------------------------------------------------
//
// minsert - minsert MicroLYNX controller.
//
*/

/*!  
  \brief minsert command - command the Insert Mask mechanism
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  mask [in|out|stow]

  in - extract the slit mask from the cassette and deploy it in the focal plane\n
  out - withdraw the slit mask halfway from the focal plane\n
  stow - withdraw the slit mask from the focal plane an stow it in the cassette\n

  \par Description:
  http://www.astronomy.ohio-state.edu/~rgonzale/MODS_ICSCommands.pdf

  If in, it extracts the currently selected mask from the cassette and 
  deploys it in the MODS focal plane.

  If out, it withdraws the mask from the focal plane and stows it in the mask 
  cassette provided of course that cassette position is unoccupied. 
  Preventing this is one of the duties of the software interlock system.

  MASK does not make a mask selection; see SLITMASK to select a mask from the
  cassette for insertion into the focal plane.

  If there is no mask either deployed at the focal plane or stowed in the mask
  cassette, it returns an error: "No mask available in the cassette to 
  [stow|deploy]".

  \par Interactions: Interlocked with the slit mask cassette mechanism 
  (hardware & software).

  Send a mask MicroLYNX controller command via network or serial.
  MicroLYNX Controller commands given in MODS Mechanism Commands doc.
  http://www.astronomy.ohio-state.edu/~rgonzale/MODS_Mechanism_Commands.pdf

*/
int
cmd_minsert(char *args, MsgType msgtype, char *reply)
{
  int device;
  int device2;
  int device3;
  int smask;
  int queryMinsert;
  int io20_minsert;
  int io30_mselect;
  char who_selected[24];
  char bc_dummy[64];
  char dummy[PAGE_SIZE];
  char cmd_instruction[PAGE_SIZE];

  memset(cmd_instruction,0,sizeof(cmd_instruction));

  StrUpper(args);
  GetArg(args,1,cmd_instruction);
  
  strcpy(who_selected,cmdtab[commandID].cmd);
  StrUpper(who_selected);

  queryMinsert=0;

  /* Search for the IP and Socket */
  device=getMechanismID("minsert",dummy); // Get mechanism device ID
  if(device==-1) {
    sprintf(reply,"%s %s",who_selected,dummy);
    return CMD_ERR;
  }

  if(!strcasecmp(cmd_instruction,"LOCK")) { // Abort process
    shm_addr->MODS.LOCKS[device]=1;
    sprintf(reply," %s %s=LOCK Mechanisms are locked",
	    who_selected,who_selected);
    return CMD_OK;

  } else if(!strcasecmp(cmd_instruction,"UNLOCK")) { // Abort process
    shm_addr->MODS.LOCKS[device]=0;
    sprintf(reply," %s %s=UNLOCK Mechanisms is unlocked",
	    who_selected,who_selected);
    return CMD_OK;
  }

  if(!shm_addr->MODS.host[device]) {
    sprintf(reply," %s %s=NOCOMM No IP address configured, check mechanisms.ini file",who_selected,who_selected);
    return CMD_ERR;
  } else if ( shm_addr->MODS.LOCKS[device] ) {
    sprintf(reply," %s %s=FAULT connection LOCKED OUT, SLITMASK UNLOCK to continue",who_selected,who_selected);

    return CMD_ERR;
  }

  if (!strcasecmp(cmd_instruction,"CONFIG")) { // get mechanism ip
    strcpy(shm_addr->MODS.state_word[device],shm_addr->MODS.maskpos);

    mlcMechanismConfig(device,who_selected,dummy);
    sprintf(reply,"%s",dummy);

    return CMD_OK;
  }

  if(!shm_addr->MODS.host[device]) {
    sprintf(reply," %s %s=NOCOMM - NO COMMunication or not connected",who_selected,who_selected);
    return CMD_ERR;
  }

  if(!strcasecmp(cmd_instruction,"ABORT")) {
    ierr = mlcStopMechanism(device,dummy); //  STOP the operation
    sprintf(reply,"%s %s",who_selected,dummy);
    return CMD_OK;
  }

  if (mlcBusy(device,dummy)) { // Busy?
    sprintf(reply,"%s %s",who_selected,dummy);
    return CMD_ERR;
  }
 // Check if the microLynx is ON
  if(mlcQuery(device,1,dummy)!=0) {
    sprintf(reply,"%s %s",who_selected,dummy);
    return CMD_ERR;
  }
    
  if (strncasecmp(args,"M#",2)) { // check for low-level command
    ierr = mlcCheckBits(device,dummy); // check limit bits 21,22
    if((ierr&3)== 3 || ierr==-1) {
      sprintf(reply,"%s %s",who_selected,dummy);
      return CMD_ERR;
    }
  }

  if(!strcasecmp(cmd_instruction,"RESET")) {

    sendCommand(device,"PWRFAIL=0",dummy); // Reset Mask Insert mechanism

    if(!mlcBitsBase10(device,"PRINT IO 22, IO 21",dummy)) {
      ierr = sendCommand(device,"HOME",dummy); // Reset Mask Insert mechanism
      cmd_minsert("",EXEC,reply);
      strcat(reply," Reset Successful");
      return CMD_OK;
    }

    if(!mlcBitsBase10(device,"PRINT IO 22",dummy)) {
      sprintf(dummy,"Resetting %s",who_selected);
      isisStatusMsg(dummy);
    }
  
    ierr = sendCommand(device,"INITIAL",dummy); // Reset Mask Insert mechanism

    shm_addr->MODS.pos[device]=positionToShrMem(device,dummy);

    cmd_minsert("",EXEC,reply);
    strcat(reply," Reset Successful");

    return CMD_OK;
  }

  if (strncasecmp(args,"M#",2)) { // check for low-level command
    ierr=checkPower(device,dummy); // check the mechanism power 
    if(ierr) {
      sprintf(reply,"%s %s",who_selected,dummy);
      return CMD_ERR;
    }
  }

  device2=getMechanismID("mselect",dummy); // Get mechanism device ID
  if(device2==-1) {
    sprintf(reply,"%s %s",who_selected,dummy);
    return CMD_ERR;
  }

  if (mlcBusy(device2,dummy)) { // Busy?
    sprintf(reply,"%s %s",who_selected,dummy);
    return CMD_ERR;
  }

 // Check if the microLynx is ON

  if(strlen(cmd_instruction)>0) {
    if(mlcQuery(device2,1,dummy)!=0) {
      sprintf(reply,"%s %s",who_selected,dummy);
      return CMD_ERR;
    }

    smask = getMaskNumber(device2,dummy);
    io30_mselect=mlcBitsBase10(device2,"PRINT IO 35,IO 34,IO 33,IO 32,IO 31",
			       dummy);
    if(io30_mselect==12) smask=-1; // make sure that 13 is not used if BRACE
  }
  if (strlen(args)<=0) {  // Query when no command is issued

    strcpy(who_selected,"MINSERT");

    if(!mlcBitsBase10(device2,"PRINT IO 36",dummy)) {
      sprintf(reply,"%s %s=FAULT Mechanism out-of-position, in-position sensor not asserted. mselect reset to recover",who_selected,who_selected);
      return CMD_ERR;
    }

    shm_addr->MODS.pos[device]=positionToShrMem(device,dummy);
    
    io20_minsert=mlcBitsBase10(device,"PRINT IO 21,IO 22,IO 23,IO 24",dummy);
    
    switch (io20_minsert) { // Check all returned bits 
    case -1:
      sprintf(reply,"%s %s",who_selected,dummy);
      return CMD_ERR;

    case 0:
      sprintf(reply,"%s MASKPOS=UNKNOWN GRABBER=UNKNOWN Mask and Grabber Out-of-Position",who_selected);
      strcpy(shm_addr->MODS.maskpos,"UNKNOWN");
      return CMD_ERR;

    case 1:
      sprintf(reply,"%s MASKPOS=STOW GRABBER=UNKNOWN",who_selected);
      strcpy(shm_addr->MODS.maskpos,"STOW");
      break;

    case 2:
      sprintf(reply,"%s MASKPOS=IN GRABBER=UNKNOWN",who_selected);
      strcpy(shm_addr->MODS.maskpos,"IN");
      break;

    case 3:
      sprintf(reply,"%s MASKPOS=FAULT GRABBER=UNKNOWN Mask Sensor Fault",who_selected);
      strcpy(shm_addr->MODS.maskpos,"FAULT");
      return CMD_ERR;

    case 4: // Valid state if not at brace
      if(io30_mselect==12) { // at brace
	sprintf(reply,"%s MASKPOS=STOW GRABBER=STOW Cassette at Brace",who_selected);
	strcpy(shm_addr->MODS.maskpos,"STOW");
	return CMD_ERR;
      }
      sprintf(reply,"%s MASKPOS=EMPTY GRABBER=STOW",who_selected);
      strcpy(shm_addr->MODS.maskpos,"EMPTY");
      break;

    case 5: // valid state if not at brace
      if(io30_mselect==12) { // at brace
	sprintf(reply,"%s MASKPOS=STOW GRABBER=STOW",who_selected);
	strcpy(shm_addr->MODS.maskpos,"STOW");
	return CMD_ERR;
      }
      sprintf(reply,"%s MASKPOS=STOW GRABBER=STOW",who_selected);
      strcpy(shm_addr->MODS.maskpos,"STOW");
      break;

    case 6:
      sprintf(reply,"%s MASKPOS=IN GRABBER=STOW Impossible: Mask in Science Position but Grabber Stowed",who_selected);
      strcpy(shm_addr->MODS.maskpos,"IN");
      return CMD_ERR;

    case 7:
      sprintf(reply,"%s MASKPOS=FAULT GRABBER=STOW Mask Sensor Faults",who_selected);
      strcpy(shm_addr->MODS.maskpos,"FAULT");
      return CMD_ERR;

    case 8:
      sprintf(reply,"%s MASKPOS=UNKNOWN GRABBER=IN",who_selected);
      strcpy(shm_addr->MODS.maskpos,"UNKNOWN");
      return CMD_ERR;
      
    case 9:
      sprintf(reply,"%s MASKPOS=STOW GRABBER=IN Impossible: Mask Stowed but Grabber in Science Position",who_selected);
      strcpy(shm_addr->MODS.maskpos,"STOW");
      return CMD_ERR;

    case 10: // Valid state
      sprintf(reply,"%s MASKPOS=IN GRABBER=IN",who_selected);
      strcpy(shm_addr->MODS.maskpos,"IN");
      break;

    case 11:
      sprintf(reply,"%s MASKPOS=FAULT GRABBER=IN Mask Sensor Fault",who_selected);
      strcpy(shm_addr->MODS.maskpos,"FAULT");
      return CMD_ERR;

    case 12:
      sprintf(reply,"%s MASKPOS=UNKNOWN GRABBER=FAULT Grabber and Mask Sensor Faults",who_selected);
      strcpy(shm_addr->MODS.maskpos,"UNKNOWN");
      return CMD_ERR;

    case 13:
      sprintf(reply,"%s MASKPOS=STOW GRABBER=FAULT Grabber Sensor Fault",who_selected);
      strcpy(shm_addr->MODS.maskpos,"STOW");
      return CMD_ERR;

    case 14:
      sprintf(reply,"%s MASKPOS=IN GRABBER=FAULT Grabber Sensor Fault",who_selected);
      strcpy(shm_addr->MODS.maskpos,"IN");
      return CMD_ERR;

    case 15:
      sprintf(reply,"%s MASKPOS=FAULT GRABBER=FAULT ALL Sensor Fault",who_selected);
      strcpy(shm_addr->MODS.maskpos,"FAULT");
      return CMD_ERR;

    default:
      sprintf(reply,"%s MINSERT=FAULT Unknown bit pattern %d, b21-b24",who_selected,io20_minsert);
      return CMD_ERR;
    }
    return CMD_OK;
  }

  if (!strcasecmp(cmd_instruction,"IN")) {

    if(!mlcBitsBase10(device2,"PRINT IO 36",dummy)) {
      sprintf(reply,"%s %s=FAULT Mechanism out-of-position, in-position sensor not asserted. mselect reset to recover",who_selected,who_selected);
      return CMD_ERR;
    }
		 
    if(mlcBitsBase10(device,"PRINT IO 21",dummy)) {
      cmd_minsert("",EXEC,dummy);
      sprintf(reply,"%s",dummy);
      return CMD_OK;
    }
		 
    if(!mlcBitsBase10(device,"PRINT IO 24",dummy)) {
      cmd_minsert("",EXEC,dummy);
      sprintf(reply,"%s",dummy);
      return CMD_OK;
    }

    if(mlcBitsBase10(device,"PRINT IO 22",dummy)) {
      sprintf(dummy,"%s Inserting mask",who_selected);
      isisStatusMsg(dummy);
    }
		 
    io20_minsert=mlcBitsBase10(device,"PRINT IO 21,IO 22,IO 23,IO 24",
			       dummy);

    if(io20_minsert<=0) {
      sprintf(reply,"%s SLITMASK=%d MASKPOS=UNKNOWN GRABBER=UNKNOWN Mask and Grabber Out-of-Position, reset to initialize",who_selected,smask);
      strcpy(shm_addr->MODS.maskpos,"UNKNOWN");
      return CMD_ERR;
    }

    if(io30_mselect==12) {
      if(io20_minsert==4) {
	sprintf(reply,"%s SLITMASK=BRACE MASKPOS=EMPTY GRABBER=STOW",who_selected);
	strcpy(shm_addr->MODS.maskpos,"EMPTY");
	return CMD_ERR;

      } else {
	sprintf(reply,"%s SLITMASK=BRACE MASKPOS=STOW GRABBER=STOW",who_selected);
	strcpy(shm_addr->MODS.maskpos,"STOW");
	return CMD_ERR;
      }
      
    }

    ierr = sendCommand(device,"HOME",dummy); //  Science Position command

    if(ierr!=0) {
      sprintf(reply,"%s SLITMASK=%d Home failed",who_selected,smask);
      return CMD_ERR;
    }

    shm_addr->MODS.pos[device]=positionToShrMem(device,dummy);

    cmd_minsert("",EXEC,dummy);
    sprintf(reply,"%s",dummy);

    return CMD_OK;

  } else if (!strcasecmp(cmd_instruction,"STOW") || !strcasecmp(cmd_instruction,"OUT")) {

    if(!mlcBitsBase10(device2,"PRINT IO 36",dummy)) {
      sprintf(reply,"%s %s=FAULT Mechanism out-of-position, in-position sensor not asserted. mselect reset to recover",who_selected,who_selected);
      return CMD_ERR;
    }
		 
    if(mlcBitsBase10(device,"PRINT IO 22",dummy)) {
      cmd_minsert("",EXEC,dummy);
      sprintf(reply,"%s",dummy);
      return CMD_OK;
    }
		 
    if(mlcBitsBase10(device,"PRINT IO 24",dummy)) {
      cmd_minsert("",EXEC,dummy);
      sprintf(reply,"%s",dummy);
      return CMD_OK;

    }

    if(mlcBitsBase10(device,"PRINT IO 21",dummy)) {
      sprintf(dummy,"%s Retracting mask",who_selected);
      isisStatusMsg(dummy);
    }

    if(!mlcBitsBase10(device,"PRINT IO 22,IO 21",dummy)) {
      sprintf(reply,"%s SLITMASK=%d MASKPOS=UNKNOWN GRABBER=UNKNOWN Mask and Grabber Out-of-Position, reset to initialize",who_selected,smask);
      strcpy(shm_addr->MODS.maskpos,"UNKNOWN");
      return CMD_ERR;
    }

    ierr = sendCommand(device,"STOW",dummy); //  Stow Position command

    if(ierr!=0) {
      sprintf(reply,"%s SLITMASK=%d %s",who_selected,smask,dummy);
      return CMD_ERR;
    }

    shm_addr->MODS.pos[device]=positionToShrMem(device,dummy);

    cmd_minsert("",EXEC,dummy);
    sprintf(reply,"%s",dummy);

    return CMD_OK;

  } else if (!strcasecmp(cmd_instruction,"RDBITS")) {

    smask = getMaskNumber(device2,dummy);
    rawCommand(device,"PRINT IO 24,IO 23,IO 22,IO 21",dummy);

    sprintf(reply,"%s SLITMASK=%d BITS=%s b24-b21", who_selected,smask,dummy);

  } else if(!strncasecmp(args,"M#",2)) { // check for low-level command

    if(!strncasecmp(args,"M#IN_OR",7)) {

      io20_minsert=mlcBitsBase10(device,"PRINT IO 21,IO 22,IO 23,IO 24",
				 dummy);
      if(io30_mselect==12) {
	if(io20_minsert==4) { // MASK select door open

	  sprintf(reply,"%s SLITMASK=BRACE MASKPOS=FAULT GRABBER=STOW",who_selected);
	  strcpy(shm_addr->MODS.maskpos,"FAULT");

	  return CMD_ERR;
	} else {

	  ierr = sendCommand(device,"HOME",dummy); //  Science Position command

	  sprintf(reply,"%s SLITMASK=BRACE GRABBER=IN Science Position Override",who_selected);

	  shm_addr->MODS.pos[device]=positionToShrMem(device,dummy);
	  
	  if(ierr!=0) return CMD_ERR;

	  return CMD_OK;
	}
      }
      sprintf(reply,"%s SLITMASK=%d, BRACE position is required for Override slitmask brace",who_selected,io30_mselect);

    } else if(!strncasecmp(args,"M#STOW_OR",7)) {

      ierr = sendCommand(device2,"IO36PO",dummy); // look for In Position
      if(atoi(dummy)==0) {
	sprintf(reply,"%s MINSERT=FAILED, Mask Select In Position(IP) bit not asserted",who_selected);
	return CMD_ERR;
      } else {

	/* Stow carefully */
	ierr = sendCommand(device,"OR_STOW",dummy);

	sprintf(reply,"%s SLITMASK=BRACE MINSERT=STOW Stow Override",who_selected);
	
	shm_addr->MODS.pos[device]=positionToShrMem(device,dummy);
      }

    } else {
      ierr=mlcTechCmd(device,args,who_selected,reply);
      if(ierr<=-1) return CMD_ERR;
    }
      return CMD_OK;

  } else {
    sprintf(reply,"%s Invalid request '%s', Usage: minsert [in|stow]",who_selected,args); 
    return CMD_ERR;
  }

  shm_addr->MODS.pos[device]=positionToShrMem(device,dummy);

  return CMD_OK;
}

/* ---------------------------------------------------------------------------
//
// mselect - mselect MicroLYNX controller.
//
*/

/*!  
  \brief mselect command - command the Mask Select mechanism
  \param args string with command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  mselect [n|brace|step +/-n|reset|cwlimit|ccwlimit]

  n - number of the slit mask to select (1..24)
  brace - selects the support brace position (sensor test position)
  step - move +/- number of mask positions
  cwlimit - go to the ClockWise(CW) limit position
  ccwlimit - go to the CounterClockWise(CCW) limit position
  findpos - go to the In Position(IP) bit.

  \par Description:
  http://www.astronomy.ohio-state.edu/~rgonzale/MODS_ICSCommands.pdf

  Move the slit mask cassette to place the mask in cassette slot n into 
  position for insertion into the MODS focal plane.
  
  If given without arguments, it returns the slit mask cassette slot currently
  in position, and indicates whether or not a mask is actually loaded in that 
  slot in the cassette along with its ID if known.

  SLITMASK does not insert the named slit mask into the focal plane, it only 
  positions the mask cassette mechanism to the requested position preparatory 
  to insertion with the MASK command.

  If there is already a slit mask deployed in the focal plane, a request to 
  move the slit mask cassette mechanism will be denied and return an error 
  requesting that the currently deployed mask be retracted first using the 
  "MASK out" command.

  \par Interactions: Interlocked with the slit mask insert/retract mechanism 
  (hardware & software).

  Send a mask MicroLYNX controller command via network or serial.
  MicroLYNX Controller commands given in MODS Mechanism Commands doc.
  http://www.astronomy.ohio-state.edu/~rgonzale/MODS_Mechanism_Commands.pdf

*/

int
cmd_mselect(char *args, MsgType msgtype, char *reply)
{
  int smask;
  int device;
  int device2;
  int masknum;
  int mask_pos;
  int io20_minsert;
  int io20_mselect;
  int io30_mselect;
  char mask_selected[24];
  char who_selected[24];
  char dummy[PAGE_SIZE];
  char cmd_instruction[PAGE_SIZE];
  char mlccmd[24];

  memset(dummy,0,sizeof(dummy));
  memset(cmd_instruction,0,sizeof(cmd_instruction));

  StrUpper(args);
  GetArg(args,1,cmd_instruction);


  strcpy(who_selected,cmdtab[commandID].cmd);
  StrUpper(who_selected);

  device2=getMechanismID("mselect",reply); // Get mechanism device ID
  if(device2==-1) {
    sprintf(reply,"%s %s",who_selected,dummy);
    return CMD_ERR;
  }


  if (!strcasecmp(cmd_instruction,"CONFIG")) { // get mechanism ip
    strcpy(shm_addr->MODS.state_word[device2],shm_addr->MODS.maskpos);
    mlcMechanismConfig(device2,who_selected,dummy);
    sprintf(reply,"%s",dummy);

    return CMD_OK;
  }
  /*
  wagoRW(shm_addr->MODS.ieb_i[device2],"MLCS",0,device2,dummy);
  if(strstr(dummy,"OFF")) {
    if(device2>15)
      sprintf(reply,"%s MLCERR=FAULT %s controller power turned OFF",
	      who_selected,who_selected);
    else sprintf(reply,"%s MLCERR=FAULT %s controller power turned OFF",
		 who_selected,who_selected);
    return CMD_ERR;
  }
  */

  if(!strcasecmp(cmd_instruction,"LOCK")) { // Abort process
    shm_addr->MODS.LOCKS[device2]=1;
    sprintf(reply," %s %s=LOCK Mechanisms are locked",
	    who_selected,who_selected);
    return CMD_OK;

  } else if(!strcasecmp(cmd_instruction,"UNLOCK")) { // Abort process
    shm_addr->MODS.LOCKS[device2]=0;
    sprintf(reply," %s %s=UNLOCK Mechanisms is unlocked",
	    who_selected,who_selected);
    return CMD_OK;
  }

  if(!shm_addr->MODS.host[device2]) {
    sprintf(reply," %s %s=NOCOMM No IP address configured, check mechanisms.ini file",who_selected,who_selected);
    return CMD_ERR;
  } else if ( shm_addr->MODS.LOCKS[device2] ) {
    sprintf(reply," %s %s=FAULT connection LOCKED OUT, SLITMASK UNLOCK to continue",who_selected,who_selected);

    return CMD_ERR;
  }

  if(!strcasecmp(cmd_instruction,"ABORT")) {
    ierr = mlcStopMechanism(device2,dummy); //  STOP the operation
    sprintf(reply,"%s %s",who_selected,dummy);
    return CMD_OK;
  }

  if (mlcBusy(device2,dummy)) { // Busy?
    sprintf(reply,"%s %s",who_selected,dummy);
    return CMD_ERR;
  }

 // Check if the microLynx is ON
  if(mlcQuery(device2,1,dummy)!=0) {
    sprintf(reply,"%s %s",who_selected,dummy);
    return CMD_ERR;
  }

  /* Search for the IP and Socket */
  device=getMechanismID("minsert",dummy); // Get mechanism device ID
  if(device==-1) {
    sprintf(reply,"%s %s",who_selected,dummy);
    return CMD_ERR;
  }

  if(!shm_addr->MODS.host[device]) {
    sprintf(reply," %s %s=NOCOMM No IP address configured, check mechanisms.ini file",who_selected,who_selected);
    return CMD_ERR;
  } else if ( shm_addr->MODS.LOCKS[device] ) {
    sprintf(reply," %s %s=FAULT connection LOCKED OUT, SLITMASK UNLOCK to continue",who_selected,who_selected);

    return CMD_ERR;
  }

  if (mlcBusy(device,dummy)) { // Busy?
    sprintf(reply,"%s %s",who_selected,dummy);
    return CMD_ERR;
  }

 // Check if the microLynx is ON
  if(mlcQuery(device,1,dummy)!=0) {
    sprintf(reply,"%s %s",who_selected,dummy);
    return CMD_ERR;
  }

  if (strncasecmp(args,"M#",2)) { // check for low-level command
    ierr = mlcCheckBits(device,dummy); // check limit bits 21,22
    if((ierr&3)== 3 || ierr==-1) {
      sprintf(reply,"%s %s",who_selected,dummy);
      return CMD_ERR;
    }
  }

  if (!strcasecmp(cmd_instruction,"RESET")) {

    rawCommand(device,"PRINT IO 23",dummy);
    if(mlcBitsBase10(device,"PRINT IO 23",dummy)) {
      ierr=mlcBitsBase10(device2,"PRINT IO 35,IO 34,IO 33,IO 32,IO 31",dummy);
      ierr+=1;

      sprintf(reply,"%s SLITMASK=%d in the Science Field, RESET is disallowed, stow mask first",who_selected,ierr);
      return CMD_OK;
    }

    if(!mlcBitsBase10(device,"PRINT IO 22, IO 21",dummy)) {
      sprintf(reply,"%s MINSERT=UNKNOWN, GRABBER=UNKNOWN Grabber not in stow or science position, Reset to recover",who_selected);
      return CMD_ERR;
    }

    sprintf(dummy,"%s Resetting",who_selected);
    isisStatusMsg(dummy);

    ierr = sendCommand(device2,"M_RESET",dummy); // Initialize Mask Select

    ierr = sendCommand(device2,"IO36PO",dummy); // In Position?
    if(atoi(dummy)==0) {
      sprintf(reply,"%s MSELECT=UNKNOWN, in-position bit not asserted, Try again",who_selected);
      return CMD_ERR;
    }
    smask=shm_addr->MODS.active_smask=atoi(dummy)+1;
    cmd_mselect("",EXEC,reply);

    return CMD_OK;
  }

  if (strncasecmp(args,"M#",2)) { // check for low-level command
    ierr=checkPower(device2,dummy); // check the mechanism power 
    if(ierr) {
      sprintf(reply,"%s %s",who_selected,dummy);
      return CMD_ERR;
    }
  }
  
  io20_minsert=mlcBitsBase10(device,"PRINT IO 21,IO 22,IO 23,IO 24",
			     dummy);
  memset(dummy,0,sizeof(dummy));
  smask = getMaskNumber(device2,dummy);

  io30_mselect=mlcBitsBase10(device2,"PRINT IO 35,IO 34,IO 33,IO 32,IO 31",
			       dummy);
  if(io30_mselect==12) smask=-1; // make sure that 13 is not used if BRACE

  if (strlen(args)<=0) {  // Query when no command is issued
    strcpy(who_selected,"MSELECT");

    ierr=checkPower(device2,dummy); // check the mechanism power 
    if(ierr) {
      sprintf(reply,"%s %s",who_selected,dummy);
      return CMD_ERR;
    }

    if(!mlcBitsBase10(device,"PRINT IO 22, IO 21",dummy)) {
      sprintf(reply,"%s MINSERT=UNKNOWN, GRABBER=UNKNOWN Grabber not in stow or science position, Reset to recover",who_selected);
      return CMD_ERR;
    }
    
    if(!mlcBitsBase10(device2,"PRINT IO 36",dummy)) {
      sprintf(reply,"%s %s=FAULT Mechanism out-of-position, in-position sensor not asserted. Reset %s to recover",who_selected,who_selected,who_selected);
      return CMD_ERR;
    }

    switch (io20_minsert) {
    case 3: case 7: case 11:
      sprintf(reply,"%s MINSERT='FAULT' Sensor Fault, both positions asserted",who_selected);
      return CMD_ERR;

    case 4:
      if(io30_mselect==12) {
	sprintf(reply,"%s SLITMASK='BRACE' MASKPOS=EMPTY MASKNAME='BRACE'",who_selected);
	strcpy(shm_addr->MODS.maskpos,"EMPTY");
	return CMD_OK;
      }
      sprintf(reply,"%s SLITMASK=%d MASKPOS=EMPTY MASKNAME='%s'",who_selected,smask, shm_addr->MODS.slitmaskName[smask]);
      strcpy(shm_addr->MODS.maskpos,"EMPTY");
      shm_addr->MODS.active_smask=smask;
      break;

    case 5:
      if(io30_mselect==12) {
	sprintf(reply,"%s SLITMASK=BRACE MASKPOS=STOW MASKNAME='BRACE'",who_selected);
	strcpy(shm_addr->MODS.maskpos,"STOW");
      }
      sprintf(reply,"%s SLITMASK=%d MASKPOS=STOW MASKNAME='%s'",who_selected,smask, shm_addr->MODS.slitmaskName[smask]);
      strcpy(shm_addr->MODS.maskpos,"STOW");
      shm_addr->MODS.active_smask=smask;
      break;

    case 8:
      sprintf(reply,"%s SLITMASK=%d MASKPOS=EMPTY MASKNAME='%s'",who_selected,smask, shm_addr->MODS.slitmaskName[smask]);
      strcpy(shm_addr->MODS.maskpos,"EMPTY");
      shm_addr->MODS.active_smask=smask;
      break;

    case 10:
      ierr=mlcBitsBase10(device2,"PRINT IO 35,IO 34,IO 33,IO 32,IO 31",
				 dummy);

      sprintf(reply,"%s SLITMASK=%d MASKPOS=IN MASKNAME='%s'",who_selected,smask, shm_addr->MODS.slitmaskName[smask]);
      strcpy(shm_addr->MODS.maskpos,"IN");
      shm_addr->MODS.active_smask=smask;
      break;

    case 12: case 13: case 14:
      sprintf(reply,"%s MINSERT='FAULT' Sensor Fault, both limits asserted",who_selected);
      return CMD_ERR;

    case 15:
      sprintf(reply,"%s MINSERT='FAULT' All Sensor Fault, all limits and positions asserted");
      return CMD_ERR;

    default:
      break;
    }
    return CMD_OK;
  }


  if (!strncasecmp(args,"M#",2)) { // check for low-level command
    ierr=mlcTechCmd(device2,args,who_selected,reply);
    if(ierr<=-1) return CMD_ERR;

    return CMD_OK;

  } else if ((atoi(args)>0 && atoi(args)<25)) { 

    if(checkPower(device2,dummy)) {
      sprintf(reply,"%s %s",who_selected,dummy);
      return CMD_ERR;
    }

    if(!mlcBitsBase10(device2,"PRINT IO 36",dummy)) {
      sprintf(reply,"%s %s=FAULT Mechanism out-of-position, in-position sensor not asserted. Reset %s to recover",who_selected,who_selected,who_selected);
      return CMD_ERR;
    }

    if(!mlcBitsBase10(device,"PRINT IO 22, IO 21",dummy)) {
      sprintf(reply,"%s MINSERT=UNKNOWN, GRABBER=UNKNOWN Grabber not in stow or science position, Reset to recover",who_selected);
      return CMD_ERR;
    }

    sprintf(dummy,"%s Selecting mask %d",who_selected,atoi(args));
    isisStatusMsg(dummy);

    io20_minsert=mlcBitsBase10(device,"PRINT IO 21,IO 22,IO 23,IO 24",
			       dummy);

    switch (io20_minsert) {
    case 3: case 7: case 11:
      sprintf(reply,"%s MINSERT=FAULT Sensor Fault, both positions asserted",who_selected);
      return CMD_ERR;

    case 15:
      sprintf(reply,"%s MINSERT=FAULT All Sensor Fault, all limits and positions asserted",who_selected);
      return CMD_ERR;

    case 12: case 13: case 14:
      sprintf(reply,"%s MINSERT=FAULT Sensor Fault, both limits asserted",who_selected);
      return CMD_ERR;

    case 10:
      sprintf(reply,"%s SLITMASK=%d in the Science Field, MSELECT disallowed, stow mask first",who_selected,smask);
      shm_addr->MODS.active_smask=smask;
      break;

    case 8:
      sprintf(reply,"%s MINSERT=FAULT in Science Field without Mask, MSELECT disallowed, stow carriage first, MASKPOS=EMPTY",who_selected);
      strcpy(shm_addr->MODS.maskpos,"EMPTY");
      return CMD_ERR;

    default:
      
      memset(dummy,0,sizeof(dummy));
      shm_addr->MODS.reqpos[device2]=atof(cmd_instruction);
      smask=atoi(cmd_instruction);

      if(smask>=12) smask++;

      sprintf(mask_selected,"MASKNUM=%d",atoi(cmd_instruction));
      ierr = sendTwoCommand(device2,mask_selected,"MASKR",dummy);
      MilliSleep(100);

      smask=mlcBitsBase10(device2,"PRINT IO 35,IO 34,IO 33,IO 32,IO 31",
			  dummy)+1;

      if(smask>12) smask--;
      shm_addr->MODS.reqpos[device2]=atof(args);

      if(smask!=(int)shm_addr->MODS.reqpos[device2]) {
	sprintf(reply,"%s %s=%d Move Fault, position at end of move %d but requested position %f",who_selected,who_selected,smask,smask,shm_addr->MODS.reqpos[device2]);
	return CMD_ERR;
      }

      shm_addr->MODS.pos[device2]=(float)smask;
      shm_addr->MODS.active_smask=smask;

      cmd_mselect("", EXEC, reply);
    }
    return CMD_OK;

  } else if (!strcasecmp(cmd_instruction,"STEP")) {

    ierr=checkPower(device2,dummy); // check the mechanism power 
    if(ierr) {
      sprintf(reply,"%s %s",who_selected,dummy);
      return CMD_ERR;
    }

    rawCommand(device,"PRINT IO 23",dummy);
    if(atoi(dummy)) {
      sprintf(reply,"%s SLITMASK=%d in the Science Field, STEP is disallowed, stow mask first",who_selected,smask);
      return CMD_OK;
      //return CMD_ERR;
    }

    GetArg(args,2,argbuf); // Get step count

    sprintf(mask_selected,"STEPNUM=%s",argbuf);
    ierr = sendTwoCommand(device2,mask_selected,"STEP",dummy);

    cmd_mselect("", EXEC,dummy);
    sprintf(reply,"%s %s",who_selected,dummy);

  } else if (!strcasecmp(cmd_instruction,"RDBITS")) {

    smask = getMaskNumber(device2,dummy);
    rawCommand(device2,"PRINT IO 36,IO 35,IO 34,IO 33,IO 32,IO 31",dummy);

    sprintf(reply,"%s SLITMASK=%d BITS=%s b36-b31", who_selected,smask,dummy);

  } else if (!strcasecmp(cmd_instruction,"BRACE")) {

    sprintf(mask_selected,"BRACE");
    ierr = sendCommand(device2,mask_selected,dummy);

    smask=shm_addr->MODS.active_smask=-1;

    if(!mlcBitsBase10(device,"PRINT IO 24",dummy)) {
      sprintf(reply,"%s SLITMASK=BRACE stow-position bit not asserted",who_selected);
      return CMD_ERR;
    }
    //    sprintf(reply,"%s %s",who_selected,dummy);
    cmd_mselect("",EXEC,reply);

  } else if (!strcasecmp(cmd_instruction,"FINDPOS")) {

    memset(dummy,0,sizeof(dummy));
    io20_mselect=mlcBitsBase10(device2,"PRINT IO 21,IO 22", dummy);
    io20_mselect&=0x3;

    if(io20_mselect==2) {
      ierr = sendCommand(device2,"CENTER",dummy);
      sprintf(reply,"%s SLITMASK=24 In first position from CW",who_selected);

    } else if(io20_mselect==1) {
      ierr = sendCommand(device2,"FINDPOS",dummy);
      sprintf(reply,"%s SLITMASK=1 In first position from CCW",who_selected);

    } else if(io20_mselect==0) {
      ierr = sendCommand(device2,"FINDPOS",dummy);

      if(ierr!=0) {
	sprintf(reply,"%s MSELECT=UNKNOWN %s",who_selected,dummy);
	return CMD_ERR;
      }

      memset(dummy,0,sizeof(dummy));
      smask = getMaskNumber(device2,dummy);

      io30_mselect=mlcBitsBase10(device2,"PRINT IO 35,IO 34,IO 33,IO 32,IO 31",
				 dummy);
      if(smask<=0) {
	sprintf(reply,"%s MSELECT=UNKNOWN %s",who_selected,dummy);
	return CMD_ERR;
      }

      sprintf(reply,"%s SLITMASK=%d In Position(IP) bit asserted",who_selected,smask);
    }

  } else if (!strcasecmp(cmd_instruction,"CWLIMIT")) {

    ierr = sendCommand(device2,"CWLIMIT",dummy);
    sprintf(reply,"%s %s",who_selected,dummy);

  } else if (!strcasecmp(cmd_instruction,"CCWLIMIT")) {

    ierr = sendCommand(device2,"CCWLIMIT",dummy);
    sprintf(reply,"%s %s",who_selected,dummy);

  } else {
    sprintf(reply,"%s Invalid request '%s', valid range 1..24 or BRACE",
	    who_selected,cmd_instruction);

    return CMD_ERR;
  }

  return CMD_OK;
}

/* ---------------------------------------------------------------------------
//
// slitmask - slitmask MicroLYNX controller.
//
*/

/*!  
  \brief slitmask command - command the slit mask for deployment
  \param args string with command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  slitmask [n|ID] [in|out]

  n - number of the slit mask to select (1..24)
  ID - select a slitmask by name instead of storage cassette location number
  in - insert the mask into the science beam
  out - remove the mask from the science beam (stow in the cassette)

  \par Range: 1..24

  \par Description:
  http://www.astronomy.ohio-state.edu/~rgonzale/MODS_ICSCommands.pdf

  Move the slit mask cassette to place the mask in cassette slot n into 
  position for insertion into the MODS focal plane.
  
  If given without arguments, it returns the slit mask cassette slot currently
  in position, and indicates whether or not a mask is actually loaded in that 
  slot in the cassette along with its ID if known.

  SLITMASK does not insert the named slit mask into the focal plane, it only 
  positions the mask cassette mechanism to the requested position preparatory 
  to insertion with the MASK command.

  If there is already a slit mask deployed in the focal plane, a request to 
  move the slit mask cassette mechanism will be denied and return an error 
  requesting that the currently deployed mask be retracted first using the 
  "MASK out" command.

  \par Interactions: Interlocked with the slit mask insert/retract mechanism 
  (hardware & software).

  Send a mask MicroLYNX controller command via network or serial.
  MicroLYNX Controller commands given in MODS Mechanism Commands doc.
  http://www.astronomy.ohio-state.edu/~rgonzale/MODS_Mechanism_Commands.pdf

*/

int
cmd_slitmask(char *args, MsgType msgtype, char *reply)
{
  int i;
  int smask;
  int device;
  int device2;
  int device3;
  int io20_minsert;
  int io20_mselect;
  int io30_mselect;
  char who_selected[24];
  char bc_dummy[64];
  char bit_nomove[79];
  char mask_selected[24];
  char mask_list[512];
  char dummy[PAGE_SIZE];
  char cmd_instruction[PAGE_SIZE];

  memset(dummy,0,sizeof(dummy));
  memset(cmd_instruction,0,sizeof(cmd_instruction));

  StrUpper(args);
  GetArg(args,1,cmd_instruction);

  strcpy(who_selected,cmdtab[commandID].cmd);
  StrUpper(who_selected);

  if(!strcasecmp(cmd_instruction,"RDTAB")) { // re-read the slitmask table
    system("/usr/local/bin/mlcRecover slitmask");
    sprintf(reply,"%s %s table read",who_selected,cmd_instruction);
    return CMD_OK;
  }

  /* Search for the IP and Socket */
  device=getMechanismID("minsert",dummy); // Get mechanism device ID
  if(device==-1) {
    sprintf(reply,"%s %s",who_selected,dummy);
    return CMD_ERR;
  }

  device2=getMechanismID("mselect",dummy); // Get mechanism device ID
  if(device2==-1) {
    sprintf(reply,"%s %s",who_selected,dummy);
    return CMD_ERR;
  }

  if(!strcasecmp(cmd_instruction,"LOCK")) { // Abort process
    shm_addr->MODS.LOCKS[device]=1;
    shm_addr->MODS.LOCKS[device2]=1;
    sprintf(reply," %s %s=LOCK Mechanisms are locked",
	    who_selected,who_selected);
    return CMD_OK;

  } else if(!strcasecmp(cmd_instruction,"UNLOCK")) { // Abort process
    shm_addr->MODS.LOCKS[device]=0;
    shm_addr->MODS.LOCKS[device2]=0;
    sprintf(reply," %s %s=UNLOCK Mechanisms is unlocked",
	    who_selected,who_selected);
    return CMD_OK;
  }

  if(!shm_addr->MODS.host[device]) {
    sprintf(reply," %s %s=NOCOMM No IP address configured, check mechanisms.ini file",who_selected,who_selected);
    return CMD_ERR;
  } else if ( shm_addr->MODS.LOCKS[device] ) {
    sprintf(reply," %s %s=FAULT connection LOCKED OUT, %s UNLOCK to continue",who_selected,who_selected,who_selected);

    return CMD_ERR;
  }

  if(!strcasecmp(cmd_instruction,"ABORT")) {

    /* abort the mask insert */
    ierr = mlcStopMechanism(device,dummy); //  STOP the operation

    /* abort the mask select */
    ierr = mlcStopMechanism(device2,dummy); //  STOP the operation

    sprintf(reply,"%s MINSERT=ABORT MSELECT=ABORT");

    return CMD_OK;
  }

  if (mlcBusy(device,dummy) || mlcBusy(device2,dummy)) {
    sprintf(reply,"%s %s",who_selected,dummy);
    return CMD_ERR; // Busy?
  }

 // Check if the microLynx is ON
  if(mlcQuery(device,1,dummy)!=0) {
    sprintf(reply,"%s %s",who_selected,dummy);
    return CMD_ERR;
  }

 // Check if the microLynx is ON
  if(mlcQuery(device2,1,dummy)!=0) {
    sprintf(reply,"%s %s",who_selected,dummy);
    return CMD_ERR;
  }

  if (strncasecmp(args,"M#",2)) { // check for low-level command
    if(mlcBitsBase10(device,"PRINT IO 22,IO 21",dummy)==3 ||
       mlcBitsBase10(device2,"PRINT IO 22,IO 21",dummy)==3) {
      sprintf(reply,"%s",who_selected);
      if(mlcBitsBase10(device,"PRINT IO 22,IO 21",dummy)==3)
	sprintf(reply,"%s MINSERT=FAULT",reply);
      if(mlcBitsBase10(device2,"PRINT IO 22,IO 21",dummy)==3)
	sprintf(reply,"%s MSELECT=FAULT",reply);
      sprintf(reply,"%s cable disconnected or sensor fault",reply);
      return CMD_ERR;
    }
  }

  if (!strcasecmp(cmd_instruction,"CONFIG")) { // Reset mechanism

    sprintf(reply,"%s Use mselect config and/or minsert config",who_selected);
    return CMD_OK;

  } else if (!strcasecmp(cmd_instruction,"RESET")) { // RESET the slitmask couple
    rawCommand(device,"PWRFAIL=0",dummy); // reset minsert power bit
    rawCommand(device2,"PWRFAIL=0",dummy); // reset mselect power bit

    if(!mlcBitsBase10(device,"PRINT IO 22",dummy))
      sendCommand(device,"HOME",dummy); // RESET Home Mask Insert

    if(!mlcBitsBase10(device,"PRINT IO 21",dummy) && !mlcBitsBase10(device,"PRINT IO 22",dummy))
      ierr = sendCommand(device,"OR_STOW",dummy); // RESET Home Mask Insert
    else
      ierr = sendCommand(device,"STOW",dummy); // RESET Stow Mask Insert
    MilliSleep(1000);

    if(!mlcBitsBase10(device,"PRINT IO 22",dummy)) {
      sprintf(reply,"%s MINSERT=UNKNOWN, GRABBER=UNKNOWN Grabber not in stow position",who_selected);
      return CMD_ERR;
    }
    
    if(!mlcBitsBase10(device2,"PRINT IO 36",dummy)) {
      ierr=mlcBitsBase10(device2,"PRINT IO 22,IO 21",dummy);
      if(ierr!=0) {
	isisStatusMsg("SLITMASK Recovering from MSELECT limit fault");
	if(ierr==1) rawCommand(device2,"MOVR -1",dummy);
	else if(ierr==2) rawCommand(device2,"MOVR 1",dummy);
	isisStatusMsg("SLITMASK standard reset continues");
      }
      ierr = sendCommand(device2,"M_RESET",dummy); // Initialize Mask Select
      isisStatusMsg("SLITMASK checking for brace");
    }

    ierr = sendCommand(device2,"BRACE",dummy); // Go to BRACE to check STOW Position(STOWP) bit

    ierr=mlcBitsBase10(device2,"PRINT IO 35,IO 34,IO 33,IO 32,IO 31",dummy);
    if (ierr!=12) {
      sprintf(reply,"%s SLITMASK=%d Move fault Slitmask reset reqested the BRACE position but moved to position %d, Check position sensors with MSLECET RDBITS",who_selected,ierr,ierr);
      return CMD_ERR;
    }

    if(!mlcBitsBase10(device,"PRINT IO 24",dummy)) {
      sprintf(reply,"%s SLITMASK=BRACE stow-position bit not asserted",who_selected);
      return CMD_ERR;
    }

    ierr = sendCommand(device2,"M_RESET",dummy); // RESET Mask Select to first position
    if(ierr!=0) {
      sprintf(reply,"%s MSELECT=UNKNOWN[%d] %s",who_selected,ierr,dummy);
      return CMD_ERR;
    }

    ierr=mlcBitsBase10(device2,"PRINT IO 35,IO 34,IO 33,IO 32,IO 31",dummy);
    smask=shm_addr->MODS.active_smask=atoi(dummy)+1;
    shm_addr->MODS.pos[device]=positionToShrMem(device,dummy);
    shm_addr->MODS.pos[device2]=positionToShrMem(device2,dummy);

    io20_minsert=mlcBitsBase10(device,"PRINT IO 21,IO 22,IO 23,IO 24",dummy);
    switch (io20_minsert) {
    case 0:
      strcpy(shm_addr->MODS.maskpos,"UNKNOWN");
      break;
    case 4: case 8:
      strcpy(shm_addr->MODS.maskpos,"EMPTY");
      break;
    case 5:
      strcpy(shm_addr->MODS.maskpos,"STOW");
      break;
    case 10:
      strcpy(shm_addr->MODS.maskpos,"IN");
      break;
    default:
      strcpy(shm_addr->MODS.maskpos,"UNKNOWN");
    }
    
    // Moved reply string creation to the end so we can report maskpos
    // as well, giving the most relevant state info [rwp/osu]

    sprintf(reply,"%s Reset Successful SLITMASK=%d MASKPOS=%s",who_selected,
	    smask,shm_addr->MODS.maskpos);

    return CMD_OK;
  }

  if (strncasecmp(args,"M#",2)) { // check for low-level command
    ierr=checkPower(device,dummy); // check the mechanism power 
    if(ierr) {
      sprintf(reply,"%s %s",who_selected,dummy);
      return CMD_ERR;
    }
    ierr=checkPower(device2,dummy); // check the mechanism power 
    if(ierr) {
      sprintf(reply,"%s %s",who_selected,dummy);
      return CMD_ERR;
    }
  }

  io20_minsert=mlcBitsBase10(device,"PRINT IO 21,IO 22,IO 23,IO 24",
			     dummy);

  memset(dummy,0,sizeof(dummy));
  smask = getMaskNumber(device2,dummy);

  io30_mselect=mlcBitsBase10(device2,"PRINT IO 35,IO 34,IO 33,IO 32,IO 31",
  			     dummy);

  if(io30_mselect==12) smask=-1; // make sure that 13 is not used if BRACE

  if (strlen(args)<=0) {  // Query when no command is issued
    strcpy(who_selected,"SLITMASK");

    if(!mlcBitsBase10(device2,"PRINT IO 36",dummy)) {
      sprintf(reply,"%s %s=FAULT Mechanism out-of-position, in-position sensor not asserted. Reset %s to recover",who_selected,who_selected,who_selected);
      return CMD_ERR;
    }

    io20_minsert=mlcBitsBase10(device,"PRINT IO 21,IO 22,IO 23,IO 24",dummy);
    if(io20_minsert<=0) {
      sprintf(reply,"%s SLITMASK=%d MASKPOS=UNKNOWN GRABBER=UNKNOWN Mask and Grabber Out-of-Position, reset to initialize",who_selected,smask);
      strcpy(shm_addr->MODS.maskpos,"UNKNOWN");
      return CMD_ERR;
    }

    shm_addr->MODS.pos[device]=positionToShrMem(device,dummy);
    shm_addr->MODS.pos[device2]=positionToShrMem(device2,dummy);

    /* Check all returned bits */
    if(io20_mselect==3) {
      sprintf(reply,"%s MSELECT=FAULT Sensor Faults",who_selected);
      return CMD_ERR;

    }
    /* Check all returned bits */
    if(io20_minsert==12) { 
      sprintf(reply,"%s SLITMASK=%d Grabber Sensor Fault, MASKPOS=UNKNOWN GRABBER=FAULT",who_selected,smask);
      strcpy(shm_addr->MODS.maskpos,"UNKNOWN");
      return CMD_ERR;

    } else if (io20_minsert==14) {
      sprintf(reply,"%s SLITMASK=%d Grabber Sensor Fault, MASKPOS=IN GRABBER=FAULT",who_selected,smask);
      strcpy(shm_addr->MODS.maskpos,"IN");
      return CMD_ERR;

    } else if (io20_minsert==13) {
      sprintf(reply,"%s SLITMASK=%d Grabber Sensor Fault, MASKPOS=STOW GRABBER=FAULT",who_selected,smask);
      strcpy(shm_addr->MODS.maskpos,"STOW");
      return CMD_ERR;

    } else if (io20_minsert==15) {
      sprintf(reply,"%s SLITMASK=%d ALL Sensor Faults, MASKPOS=FAULT GRABBER=FAULT",who_selected,smask);
      strcpy(shm_addr->MODS.maskpos,"FAULT");
      return CMD_ERR;

    } else if (io20_minsert==3) {
      sprintf(reply,"%s SLITMASK=%d Mask Sensor Fault, MASKPOS=FAULT GRABBER=UNKNOWN",who_selected,smask);
      strcpy(shm_addr->MODS.maskpos,"FAULT");
      return CMD_ERR;

    } else if (io20_minsert==7) {
      sprintf(reply,"%s SLITMASK=%d Mask Sensor Fault, MASKPOS=FAULT GRABBER=STOW",who_selected,smask);
      strcpy(shm_addr->MODS.maskpos,"FAULT");
      return CMD_ERR;

    } else if (io20_minsert==11) {
      if(smask==-1) {
	sprintf(reply,"%s SLITMASK=BRACE Mask Sensor Fault, MASKPOS=FAULT GRABBER=IN",who_selected,smask);
	strcpy(shm_addr->MODS.maskpos,"IN");
	shm_addr->MODS.active_smask=smask;
      } else {
	sprintf(reply,"%s SLITMASK=%d Mask Sensor Fault, MASKPOS=FAULT GRABBER=IN",who_selected,smask);
	strcpy(shm_addr->MODS.maskpos,"FAULT");
	return CMD_ERR;
      }

    } else if (io20_minsert==0) {
      sprintf(reply,"%s SLITMASK=%d Mask Sensor Fault, MASKPOS=UNKNOWN GRABBER=UNKNOWN",who_selected,smask);
      strcpy(shm_addr->MODS.maskpos,"UNKNOWN");
      return CMD_ERR;

    } else if (io20_minsert==9) {
      sprintf(reply,"%s SLITMASK=%d Impossible: Mask Stowed but Grabber in Science Position MASKPOS=STOW GRABBER=IN",who_selected,smask);
      strcpy(shm_addr->MODS.maskpos,"STOW");
      return CMD_ERR;

    } else if (io20_minsert==6) {
      sprintf(reply,"%s SLITMASK=%d Impossible: Mask in Science Position but Grabber Stowed MASKPOS=IN GRABBER=STOW",who_selected,smask);
      strcpy(shm_addr->MODS.maskpos,"IN");
      return CMD_ERR;
      
    } else if (io20_minsert==10) {
      sprintf(reply,"%s SLITMASK=%d MASKPOS=IN MASKNAME='%s'",who_selected,smask,shm_addr->MODS.slitmaskName[smask]);
      strcpy(shm_addr->MODS.maskpos,"IN");
      shm_addr->MODS.active_smask=smask;

    } else if (io20_minsert==5) {
      if(io30_mselect==12) {
	sprintf(reply,"%s SLITMASK=BRACE MASKPOS=STOW MASKNAME='BRACE'",who_selected);
	strcpy(shm_addr->MODS.maskpos,"STOW");
	//return CMD_ERR;

      } else {
	sprintf(reply,"%s SLITMASK=%d MASKPOS=STOW MASKNAME='%s'",who_selected,smask,shm_addr->MODS.slitmaskName[smask]);
	strcpy(shm_addr->MODS.maskpos,"STOW");
	shm_addr->MODS.active_smask=smask;
      }
      
    } else if (io20_minsert==4) {
      strcpy(shm_addr->MODS.maskpos,"EMPTY");
      if(io30_mselect==12) {
	sprintf(reply,"%s SLITMASK=BRACE MASKPOS=EMPTY MASKNAME='BRACE'",who_selected);

      } else {
	sprintf(reply,"%s SLITMASK=%d MASKPOS=EMPTY MASKNAME='%s'",who_selected,smask,shm_addr->MODS.slitmaskName[smask]);
	shm_addr->MODS.active_smask=smask;
      }

    } else {
      sprintf(reply,"%s MINSERT=UNKNOWN",who_selected);
      strcpy(shm_addr->MODS.maskpos,"UNKNOWN");

    }

    return CMD_OK;
  }

  if (!strncasecmp(args,"M#",2)) { // check for low-level command
    ierr=mlcTechCmd(device2,args,who_selected,reply);
    if(ierr<=-1) return CMD_ERR;

  } else if (!strcasecmp(cmd_instruction,"IN")) { // Send MASK to Focal Plane(FP)
    io20_minsert=mlcBitsBase10(device,"PRINT IO 21,IO 22,IO 23,IO 24",dummy);
    if(io20_minsert<=0) {
      sprintf(reply,"%s SLITMASK=%d MASKPOS=UNKNOWN GRABBER=UNKNOWN Mask and Grabber Out-of-Position, reset to initialize",who_selected,smask);
      strcpy(shm_addr->MODS.maskpos,"UNKNOWN");
      return CMD_ERR;
    }

    if(io20_minsert==5 || io20_minsert!=10) {
      if(io20_minsert!=4) {
	ierr = sendCommand(device,"HOME",dummy);
	sprintf(reply,"%s SLITMASK=%d MASKPOS=IN MASKNAME='%s'",who_selected,smask, shm_addr->MODS.slitmaskName[smask]);
	strcpy(shm_addr->MODS.maskpos,"IN");
	shm_addr->MODS.active_smask=smask;
      } else {
	sprintf(reply,"%s SLITMASK=%d MASKPOS=EMPTY MASKNAME='%s'",who_selected,smask,shm_addr->MODS.slitmaskName[smask]);
	strcpy(shm_addr->MODS.maskpos,"EMPTY");
	shm_addr->MODS.active_smask=smask;
      }
    } else if(io20_minsert==4) {
      sprintf(reply,"%s SLITMASK=%d MASKPOS=EMPTY MASKNAME='%s'",who_selected,smask,shm_addr->MODS.slitmaskName[smask]);
      strcpy(shm_addr->MODS.maskpos,"EMPTY");
      shm_addr->MODS.active_smask=smask;
      return CMD_OK;

    } else {
      sprintf(reply,"%s SLITMASK=%d MASKPOS=IN MASKNAME='%s'",who_selected,smask,shm_addr->MODS.slitmaskName[smask]);
      strcpy(shm_addr->MODS.maskpos,"EMPTY");
      shm_addr->MODS.active_smask=smask;

    }
    shm_addr->MODS.pos[device]=positionToShrMem(device,dummy);
    shm_addr->MODS.pos[device2]=positionToShrMem(device2,dummy);

  } else if (!strcasecmp(cmd_instruction,"OUT") ||
	     !strcasecmp(cmd_instruction,"STOW")) { // STOW or retract the MASK from FP
    io20_minsert=mlcBitsBase10(device,"PRINT IO 21,IO 22,IO 23,IO 24",dummy);
    io30_mselect=mlcBitsBase10(device2,"PRINT IO 35,IO 34,IO 33,IO 32,IO 31",
			       dummy);
    
    if(io30_mselect==12) smask=-1; // make sure that 13 is not used if BRACE

    if(io20_minsert==10 && io20_minsert!=5) {
      ierr = sendCommand(device,"STOW",dummy);
      sprintf(reply,"%s SLITMASK=%d MASKPOS=STOW MASKNAME='%s'",who_selected,smask,shm_addr->MODS.slitmaskName[smask]);
      strcpy(shm_addr->MODS.maskpos,"STOW");
      shm_addr->MODS.active_smask=smask;
      
    } else if(io20_minsert==5) {
      sprintf(reply,"%s SLITMASK=%d MASKPOS=STOW MASKNAME='%s'",who_selected,smask,shm_addr->MODS.slitmaskName[smask]);
      strcpy(shm_addr->MODS.maskpos,"STOW");
      
    } else if(io20_minsert==4) {
      sprintf(reply,"%s SLITMASK=%d MASKPOS=EMPTY MASKNAME='%s'",who_selected,smask,shm_addr->MODS.slitmaskName[smask]);
      strcpy(shm_addr->MODS.maskpos,"EMPTY");
      shm_addr->MODS.active_smask=smask;
      
    } else {
      sprintf(reply,"%s %s=%d MASKPOS=STOW MASKNAME='%s'",who_selected,who_selected,smask,shm_addr->MODS.slitmaskName[smask]);
      strcpy(shm_addr->MODS.maskpos,"STOW");
      shm_addr->MODS.active_smask=smask;
    }
    shm_addr->MODS.pos[device]=positionToShrMem(device,dummy);
    shm_addr->MODS.pos[device2]=positionToShrMem(device2,dummy);
    
  } else if (!strcasecmp(cmd_instruction,"BRACE")) {

    sprintf(mask_selected,"BRACE");
    ierr = sendCommand(device2,mask_selected,dummy);

    smask=shm_addr->MODS.active_smask=-1;
    shm_addr->MODS.pos[device]=positionToShrMem(device,dummy);
    shm_addr->MODS.pos[device2]=positionToShrMem(device2,dummy);
    
    if(!mlcBitsBase10(device,"PRINT IO 24",dummy)) {
      sprintf(reply,"%s SLITMASK=BRACE stow-position bit not asserted",who_selected);
      return CMD_ERR;
    }
    cmd_slitmask("",EXEC,reply);
    return CMD_OK;

  } else if ((atoi(cmd_instruction)>0 && atoi(cmd_instruction)<25)) { 

    ierr=checkPower(device2,dummy); // check the mechanism power 
    if(ierr) {
      sprintf(reply,"%s %s",who_selected,dummy);
      return CMD_ERR;
    }

    if(!mlcBitsBase10(device2,"PRINT IO 36",dummy)) {
      sprintf(reply,"%s %s=FAULT Mechanism out-of-position, in-position sensor not asserted. Reset %s to recover",who_selected,who_selected,who_selected);
      return CMD_ERR;
    }

    io20_minsert=mlcBitsBase10(device,"PRINT IO 21,IO 22,IO 23,IO 24",dummy);
    if(io20_minsert<=0) {
      sprintf(reply,"%s SLITMASK=%d MASKPOS=UNKNOWN GRABBER=UNKNOWN Mask and Grabber Out-of-Position, reset to initialize",who_selected,smask);
      strcpy(shm_addr->MODS.maskpos,"UNKNOWN");
      return CMD_ERR;
    }

    /*
    // If already in requested selected position and is already stowed
    // move mask into Science Position(SP).
    */
    if(smask==atoi(cmd_instruction)) { // same slitmask position requested
      if(io20_minsert==10) { // already in SP
	sprintf(reply,"%s SLITMASK=%d MASKPOS=IN MASKNAME='%s'",who_selected,smask,shm_addr->MODS.slitmaskName[smask]);
	strcpy(shm_addr->MODS.maskpos,"IN");
	shm_addr->MODS.active_smask=smask;

      } else if(io20_minsert==5) {  // are we in a stowed position?
	
	memset(bc_dummy,0,sizeof(bc_dummy));
	memset(dummy,0,sizeof(dummy));

	MilliSleep(200);
	ierr = sendCommand(device,"HOME",dummy); // move to Science Position
	
	if(ierr!=0) {
	  sprintf(reply,"%s SLITMASK=UNKNOWN home failed",who_selected);
	  return CMD_ERR;
	}

	shm_addr->MODS.pos[device]=positionToShrMem(device,dummy);
	shm_addr->MODS.pos[device2]=positionToShrMem(device2,dummy);

	sprintf(reply,"%s SLITMASK=%d MASKPOS=IN MASKNAME='%s'",who_selected,smask,shm_addr->MODS.slitmaskName[smask]); // get current status
	strcpy(shm_addr->MODS.maskpos,"IN");
	shm_addr->MODS.active_smask=smask;
	
      } else if(io20_minsert==4) { // *NO* slitmask in selected position
	sprintf(reply,"%s SLITMASK=%d MASKPOS=EMPTY MASKNAME='%s'",who_selected,smask,shm_addr->MODS.slitmaskName[smask]); // get current status
	strcpy(shm_addr->MODS.maskpos,"EMPTY");
	
      } else {
	sprintf(reply,"%s SLITMASK=%d",who_selected,smask);// slitmask in SP.
	strcpy(shm_addr->MODS.maskpos,"IN");
	return CMD_ERR;

      }

      return CMD_OK;
    }

    // Send a status message
    ierr=mlcBitsBase10(device2,"PRINT IO 35,IO 34,IO 33,IO 32,IO 31",dummy);
    if(ierr<12) ierr++;

    
    if(io20_minsert==10) {
      if(!mlcBitsBase10(device2,"PRINT IO 36",dummy)) {
	sprintf(reply,"%s MSELECT=UNKNOWN, in-position bit not asserted, Reset to recover",who_selected);
	return CMD_ERR;
      }    
      sprintf(dummy,"Retracting %d",ierr);
      isisStatusMsg(dummy);
      ierr = sendCommand(device,"STOW",dummy);
    }

    shm_addr->MODS.reqpos[device2]=atof(cmd_instruction);
    shm_addr->MODS.active_smask=atoi(cmd_instruction);
    smask=atoi(cmd_instruction);

    sprintf(mask_selected,"MASKNUM=%d",smask);
    ierr = sendTwoCommand(device2,mask_selected,"MASKR",dummy);
    MilliSleep(100);  // added 2016-01-24 [rwp/osu], getting race condition between mselect and query

    smask=mlcBitsBase10(device2,"PRINT IO 35,IO 34,IO 33,IO 32,IO 31",dummy)+1;

    if(smask>12) smask--;

    if(!mlcBitsBase10(device2,"PRINT IO 36",dummy)) {
      ierr=mlcBitsBase10(device2,"PRINT IO 22,IO 21",dummy);
      sprintf(reply,"%s MSELECT=FAULT position never reached and asserted limit IO2%d, slitmask reset to recover",who_selected,ierr);
      return CMD_ERR;
    }

    if(smask!=(int)shm_addr->MODS.reqpos[device2]) {
      sprintf(reply,"%s %s=%d Move Fault, position at end of move %d but requested position %0.0f",who_selected,who_selected,smask,smask,shm_addr->MODS.reqpos[device2]);
      return CMD_ERR;
    }
    
    shm_addr->MODS.pos[device2]=(float)smask;
    shm_addr->MODS.active_smask=smask;

    io20_minsert=mlcBitsBase10(device,"PRINT IO 21,IO 22,IO 23,IO 24",
			       dummy);
    if(io20_minsert==5) {

      memset(dummy,0,sizeof(dummy));
      memset(bc_dummy,0,sizeof(bc_dummy));

      sprintf(dummy,"Inserting %d",smask);
      isisStatusMsg(dummy);

      MilliSleep(200);
      ierr = sendCommand(device,"HOME",dummy);
      
      if(ierr!=0) {
      	sprintf(reply,"%s %s=UNKNOWN home failed",who_selected,who_selected);
      	return CMD_ERR;
      }
      
      sprintf(reply,"%s SLITMASK=%d MASKPOS=IN MASKNAME='%s'",who_selected,smask, shm_addr->MODS.slitmaskName[smask]);
      strcpy(shm_addr->MODS.maskpos,"IN");

      shm_addr->MODS.pos[device]=positionToShrMem(device,dummy);
      shm_addr->MODS.pos[device2]=positionToShrMem(device2,dummy);
      shm_addr->MODS.active_smask=smask;

    } else {
      sprintf(reply,"%s MSELECT=%d MASKPOS=EMPTY MASKNAME='%s'",who_selected,smask, shm_addr->MODS.slitmaskName[smask]);
      strcpy(shm_addr->MODS.maskpos,"EMPTY");
      shm_addr->MODS.active_smask=smask;

    }
    if(ierr!=0) { 
      return CMD_ERR;
    }
    
  } else {
    sprintf(reply,"%s Invalid request '%s', valid range 1..24 or BRACE",
	    who_selected, cmd_instruction);
    return CMD_ERR;
  }

  return CMD_OK;
}

/* ---------------------------------------------------------------------------
//
// agw - AGW Stage controls MicroLYNX controller.
//
*/

/*!  
  \brief AGW command - Configure the AGW stage
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  agw [xpos ypos fpos]  
  
  xpos - X-axis position in integer microns from mechanism home\n
  ypos - Y-axis position in integer microns from mechanism home\n
  fpos - focus position in integer microns from mechanism home

  \par Description:
  AGW performs low-level configuration of the AGW stage, combining the 
  operations of AGWX, AGWY, and AGWFS into a single command. If the home 
  keyword is given, it retracts all 3 stage axes to their home 
  (mechanical zero) positions.

  \par Interactions: Interlocked with the Calibration Tower 
  (hardware & software).

  Send AGW Stage commands MicroLYNX controller command via network or serial.

*/
int
cmd_agw(char *args, MsgType msgtype, char *reply)
{
  int device;
  int device1;
  int device2;
  int device3;
  int caldevice;
  int calibErr;
  int fwnum;
  int ierr, len;
  int nofvals;
  int val[3];
  double pos[3];
  char agwcmd[79];
  char who_selected[24];
  char get_buff[512];
  char dummy[PAGE_SIZE];
  char cmd_instruction[PAGE_SIZE];

  memset(cmd_instruction, 0, sizeof(cmd_instruction));
  GetArg(args, 1, cmd_instruction);
  StrUpper(cmd_instruction);

  strcpy(who_selected, cmdtab[commandID].cmd);
  StrUpper(who_selected);
  memset(get_buff, 0, sizeof(get_buff));
  memset(dummy, 0, sizeof(dummy));

  if ( !strcasecmp(cmd_instruction, "RDTAB") ) { // re-read the filter tables
    system("/usr/local/bin/mlcRecover agwfilt");
    sprintf(reply, "%s %s table read", who_selected, cmd_instruction);
    return CMD_OK;

  }

  /*
  // Search for the IP and Socket
  */
  device = getMechanismID("agwx", dummy); // Get mechanism device ID
  if ( device == -1 ) {
    sprintf(reply,"%s %s", who_selected,dummy);  
    return CMD_ERR;

  }

  device1 = getMechanismID("agwy", dummy); // Get mechanism device ID
  if ( device1 == -1 ) {
    sprintf(reply, "%s %s", who_selected, dummy);  
    return CMD_ERR;

  }

  device2 = getMechanismID("agwfoc", dummy); // Get mechanism device ID
  if ( device2 == -1 ) {
    sprintf(reply, "%s %s", who_selected, dummy);  
    return CMD_ERR;

  }

  device3 = getMechanismID("agwfilt", dummy); // Get mechanism device ID
  if ( device3 == -1 ) {
    sprintf(reply, "%s %s", who_selected, dummy);  
    return CMD_ERR;

  }

  if ( strlen(args) <= 0 ) { // Query when no command is issued
    strcpy(who_selected,"AGW");
    
    memset(get_buff, 0, sizeof(get_buff));
    ierr = agwcu("localhost", 0, "getxy", get_buff);

    if ( ierr != 0 ) {
      sprintf(reply,"%s %s", who_selected, &get_buff[6]);
      return CMD_ERR;
    }

    sprintf(reply, "%s %s", who_selected, &get_buff[6]);

    MilliSleep(100);
    memset(get_buff, 0, sizeof(get_buff));
    ierr = agwcu("localhost",0,"getfilter ",get_buff);

    if ( ierr != 0 ) {
      sprintf(reply,"%s %s", who_selected, &get_buff[6]);
      return CMD_ERR;

    }
    sprintf(reply, "%s %s", reply, &get_buff[6]);
    return CMD_OK;

  }

  val[0] = val[1] = val[2] = 0;
  nofvals = sscanf(args,"%d %d %d", &val[0], &val[1], &val[2]);
  memset(dummy, 0, sizeof(dummy));
  memset(get_buff, 0, sizeof(get_buff));

  if ( val[0] > shm_addr->MODS.max[device] || val[0] < 0 ) {
    sprintf(reply, "%s Invalid AGWXS %d request, valid range 0..%0.0f", who_selected, val[0], shm_addr->MODS.max[device]);
    return CMD_ERR;

  } else if ( val[1] > shm_addr->MODS.max[device1] || val[1] < 0) {
    sprintf(reply," %s Invalid AGWYS %d request, valid range 0..%0.0f", who_selected, val[1], shm_addr->MODS.max[device1]);
  return CMD_ERR;

  } else if ( val[2] > shm_addr->MODS.max[device2] || val[2] < 0) {
    sprintf(reply, "%s Invalid AGWFS %d request, valid range 0..%0.0f", who_selected, val[2], shm_addr->MODS.max[device2]);
    return CMD_ERR;

  }

  /* 
  // Search for the IP and Socket
  */
  caldevice = getMechanismID("calib", dummy); // Get mechanism device ID

  if ( caldevice == -1 ) {
    sprintf(reply, "%s %s", who_selected, dummy);  
    return CMD_ERR;

  }

  /*  
  // Check the for Calibration Tower obstruction
  */
  //  ierr = rawCommand(caldevice, "PRINT IO 21", dummy);
  //  calibErr = atoi(dummy);

  if ( !strcasecmp(cmd_instruction, "ABORT") ) {
    sprintf(dummy, "%c", 27);     // Stop Y axis
    ierr = agwcu("localhost", 0, "mmcIC 2 ", dummy);

    sprintf(dummy, "%c", 27);     // Stop X axis
    ierr = agwcu("localhost", 0, "mmcIC 1 ", dummy);

    sprintf(dummy, "%c", 27);   // Stop FOCUS axis
    ierr = agwcu("localhost", 0, "mmcIC 4 ", dummy);

    sprintf(reply, "%s AGW=ABORT operation was aborted", who_selected);
    return CMD_OK;

  }

  if ( !strcasecmp(cmd_instruction, "RESET") ||
       !strcasecmp(cmd_instruction, "HOME") ) {

    memset(dummy, 0, sizeof(dummy));
    if ( mlcBitsBase10(caldevice,"PRINT IO 22,IO 21",dummy) == 1 ) {

      ierr=agwcu("localhost",0,"init",get_buff); // Home AGW

    }

    cmd_agw("", EXEC, reply);

    return CMD_OK;

  } else if ( nofvals == 3 ) {

    if ( mlcBitsBase10(caldevice,"PRINT IO 22,IO 21",dummy) != 1 ) {

      sprintf(reply, "%s Calibration Tower is in the beam, move disallowed,", who_selected);
      return CMD_ERR;
      
    }

    memset(agwcmd,0,sizeof(agwcmd));
    GetArg(args,1,agwcmd); // AGW X Stage
    sprintf(get_buff,"x %s ",agwcmd);
    ierr = agwcu("localhost", 0, "setposition ", get_buff);

    if ( ierr != 0 ) {
      sprintf(reply, "%s %s", who_selected, get_buff);
      return CMD_ERR;

    }

    memset(agwcmd, 0, sizeof(agwcmd));
    GetArg(args, 3, agwcmd); // AGW Focus 
    sprintf(get_buff, "focus %s", agwcmd);
    ierr = agwcu("localhost", 0, "setposition ", get_buff);

    if ( ierr != 0 ) {
      sprintf(reply, "%s %s", who_selected, get_buff);
      return CMD_ERR;

    }

    memset(agwcmd, 0, sizeof(agwcmd));
    GetArg(args, 2, agwcmd); // AGW Y Stage
    sprintf(get_buff, "y %s 4", agwcmd);
    ierr = agwcu("localhost", 0,"setposition ", get_buff);

    if ( ierr != 0 ) {
      sprintf(reply, "%s %s", who_selected, get_buff);
      return CMD_ERR;

    }

    cmd_agw("", EXEC, reply); // Display results

  } else {
    sprintf(reply, "%s Invalid request '%s', Usage: agw [xpos ypos fpos]", who_selected, cmd_instruction);
    return CMD_ERR;

  }

  return CMD_OK;
}

/* ---------------------------------------------------------------------------
//
// agwy - AGW Y MicroLYNX controller.
//
*/

/*!  
  \brief agwY command - AGW Stage Y-axis position
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  agwy  [ypos] 

  ypos - Y-axis position in integer microns from mechanism home\n
  home - retract the AGW Y-axis stage to its home position.\n
  init - init is Y towards the CW limit switch 

  \par Description:
  Move the AGW stage to the specified Y-axis position in integer microns. 
  The Y-axis position is measured relative to the axis home sensor, which is 
  fully retracted towards the motor, which moves it as far away as possible 
  from the instrument focal plane.

  \par Interactions: Interlocked with the Calibration Tower 
  (hardware & software)

  Send AGW X MicroLYNX controller command via network or serial.
  MicroLYNX Controller commands given in MODS Mechanism Commands doc.
  http://www.astronomy.ohio-state.edu/~rgonzale/MODS_Mechanism_Commands.pdf

*/
int
cmd_agwy(char *args, MsgType msgtype, char *reply)
{
  int len;
  int device;
  int caldevice;
  int ierr;
  int bit2122;
  int val=0;
  int validNumber;
  float stepMove;
  char who_selected[24];
  char get_buff[512];
  char dummy[PAGE_SIZE];
  char dummy1[PAGE_SIZE];
  char cmd_instruction[PAGE_SIZE];

  memset(cmd_instruction,0,sizeof(cmd_instruction));
  memset(get_buff,0,sizeof(get_buff));
  memset(dummy,0,sizeof(dummy));

  StrUpper(args);

  GetArg(args,1,cmd_instruction);

  validNumber = sscanf(args,"%d", &val);

  strcpy(who_selected,cmdtab[commandID].cmd);

  StrUpper(who_selected);

  device=getMechanismID(cmdtab[commandID].cmd,dummy);// Get mechanism device ID
  if(device==-1) {
    sprintf(reply,"%s %s",who_selected,dummy);
    return CMD_ERR;
  }

  if(!shm_addr->MODS.host[device]) {
    sprintf(reply," %s %s=NOCOMM - NO COMMunication or not connected",who_selected,who_selected);
    return CMD_ERR;
  }

  /* Search for the IP and Socket */
  caldevice=getMechanismID("calib",dummy); // Get mechanism device ID
  if(caldevice==-1) {
    sprintf(reply,"%s %s",who_selected,dummy);
    return CMD_ERR;
  }

  if(!strcasecmp(cmd_instruction,"ABORT")) {
    sprintf(dummy,"%c",27);     // Stop Y axis
    ierr=agwcu("localhost",0,"mmcIC 2 ",dummy);

    sprintf(reply,"%s AGWYS=ABORT operation was aborted",who_selected);
    return CMD_OK;

  } else  if (!strcasecmp(cmd_instruction,"RDBITS")) {
    
    sprintf(dummy,"M#PRINT IO 22, IO 21",args);
    len=strlen(dummy);
    ierr=agwcu("localhost",0,"mmcIC 2 ",dummy);
    if(ierr!=0) {
      sprintf(reply,"%s", dummy);
      return CMD_ERR;
    }
    if(ierr!=0) {
      sprintf(reply,"%s", dummy);
      return CMD_ERR;
    }
    sprintf(reply,"BITS %s", &dummy[6]);

    return CMD_OK;
  }

  if (strlen(args)<=0) { // Query when no command is issued
    strcpy(who_selected,"AGWY");

    strcpy(get_buff,"y");
    ierr=agwcu("localhost",0,"getposition ",get_buff);
    if(ierr!=0) {
      sprintf(reply,"%s", dummy);
      return CMD_ERR;
    }

    sprintf(reply,"%s AGWYS=%0.0f", who_selected,
	    (atof(&get_buff[12])*shm_addr->MODS.convf[device]));

    return CMD_OK;

  } else if (!strcasecmp(cmd_instruction,"GETIP")) { // Mechanism IP
    sprintf(reply,"%s IEB=%d IP '%s' connection is '%s'",who_selected,
	    shm_addr->MODS.ieb_i[device],
  	    shm_addr->MODS.commport[device].Port,
	    (shm_addr->MODS.host[device]==1 ? "OPEN" : "CLOSED"));

    return CMD_OK;
  }

  if(!strncasecmp(args,"M#",2)) { // check for low-level command
    sprintf(dummy,"%s",args);
    len=strlen(dummy);
    ierr=agwcu("localhost",0,"mmcIC 2 ",dummy);
    if(ierr!=0) {
      sprintf(reply,"%s", dummy);
      return CMD_ERR;
    }

    if(ierr!=0) {
      sprintf(reply,"%s", dummy);
      return CMD_ERR;
    }
    sprintf(reply,"%s", &dummy[6]);
    return CMD_OK;
  }

  if(!strcasecmp(cmd_instruction,"RESET")) {

    memset(dummy, 0, sizeof(dummy));
    if ( mlcBitsBase10(caldevice,"PRINT IO 22,IO 21",dummy) == 1 ) {

      ierr=agwcu("localhost",0,"inity",get_buff); // Home AGW

    }

    cmd_agwy("", EXEC, dummy);

    for(ierr=0;dummy[ierr]!='=';ierr++);
    ierr++;

    sprintf(reply,"%s AGWYS=%0.3f",who_selected,
	    atof(&dummy[ierr])*shm_addr->MODS.convf[device]);

    return CMD_OK;

  } else if(!strcasecmp(cmd_instruction,"STEP")) {
    GetArg(args,2,argbuf); // AGW Y

    memset(get_buff,0,sizeof(get_buff));
    strcpy(get_buff,"y");
    ierr=agwcu("localhost",0,"getposition ",get_buff);
    if(ierr!=0) {
      sprintf(reply,"%s %s", who_selected, get_buff);
      return CMD_ERR;
    }
    stepMove=atof(&get_buff[12])+atof(argbuf);

    if(stepMove < shm_addr->MODS.min[device] || 
       stepMove > shm_addr->MODS.max[device]) {
      
      sprintf(reply,"%s %s=%0.3f Request 'STEP %0.3f', exceeds allowable range of %d..%d",
	      who_selected,who_selected,
	      fabs(shm_addr->MODS.pos[device]),
	      atof(argbuf),
	      (int)(shm_addr->MODS.min[device]),
	      (int)(shm_addr->MODS.max[device]));
      return CMD_ERR;
    }

    sprintf(get_buff,"setposition y %0.3f",fabs(stepMove));
    ierr = agwcu("localhost",0,get_buff,dummy);

    if(ierr!=0) {
      sprintf(reply,"%s %s",who_selected,dummy);
      return CMD_ERR;
    }

    for(ierr=0;dummy[ierr]!='=';ierr++);
    ierr++;

    MilliSleep(10);

    sprintf(reply,"%s AGWYS=%0.3f",who_selected,
	    atof(&dummy[ierr])*shm_addr->MODS.convf[device]);

    return CMD_OK;

  } else if ( validNumber == 1 ) {

    /* Check the for Calibration Tower obstruction */
    if ( mlcBitsBase10(caldevice,"PRINT IO 22,IO 21",dummy) != 1 ) {

      sprintf(reply,"%s Calibration Tower is in the beam, move disallowed,",who_selected);
      return CMD_ERR;

    } 

    GetArg(args,1,cmd_instruction); // AGW Y

    if(val>shm_addr->MODS.max[device] || val<0) {
      sprintf(reply,"%s Invalid AGWY %d request, valid range 0..%0.3f",who_selected,val,shm_addr->MODS.max[device]);
      return CMD_ERR;

    }

    /* 
    // The added 4 option is to block until finished
    // without 4 option the immediate position will be return 
    */
    sprintf(get_buff,"setposition y %s 4",cmd_instruction);

    ierr = agwcu("localhost",0,get_buff,dummy);

    if(ierr!=0) {
      sprintf(reply,"%s %s",who_selected,dummy);
      return CMD_ERR;
    }

    cmd_agwy("", EXEC, reply);

    return CMD_OK;

  } else {
    sprintf(reply,"%s Invalid AGWY request '%s', agwy [ypos]",who_selected,args); 
    return CMD_ERR;
  }

  return CMD_OK;
}

/* ---------------------------------------------------------------------------
//
// agwx - AGW X MicroLYNX controller.
//
*/

/*!  
  \brief agwX command - AGW Stage X-axis position
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  agwx [xpos] 

  xpos - X-axis position in integer microns from mechanism home\n
  home - retract the AGW X-axis stage to its home position.
  init - init is X towards the CW limit switch 

  \par Description:
  Move the AGW stage to the specified X-axis position in integer microns. 
  The X-axis position is measured relative to the axis home sensor, which is 
  fully retracted towards the motor.

  \par Interactions: Interlocked with the Calibration Tower 
  (hardware & software)

  Send AGW X MicroLYNX controller command via network or serial.
  MicroLYNX Controller commands given in MODS Mechanism Commands doc.
  http://www.astronomy.ohio-state.edu/~rgonzale/MODS_Mechanism_Commands.pdf

*/
int
cmd_agwx(char *args, MsgType msgtype, char *reply)
{
  int len;
  int device;
  int caldevice;
  int ierr;
  int bit2122;
  int val=0;
  int validNumber;
  float stepMove;
  char who_selected[24];
  char get_buff[80];
  char dummy[PAGE_SIZE];
  char dummy1[PAGE_SIZE];
  char cmd_instruction[PAGE_SIZE];

  memset(cmd_instruction,0,sizeof(cmd_instruction));
  memset(get_buff,0,sizeof(get_buff));
  memset(dummy,0,sizeof(dummy));
  StrUpper(args);

  GetArg(args,1,cmd_instruction);
  memset(get_buff,0,sizeof(get_buff));

  strcpy(who_selected,cmdtab[commandID].cmd);
  StrUpper(who_selected);

  device=getMechanismID(cmdtab[commandID].cmd,dummy);// Get mechanism device ID

  if(device==-1) {
    sprintf(reply,"%s %s",who_selected,dummy);
    return CMD_ERR;
  }

  if(!shm_addr->MODS.host[device]) {
    sprintf(reply," %s %s=NOCOMM - NO COMMunication or not connected",who_selected,who_selected);
    return CMD_ERR;
  }

  /* Check the for Calibration Tower obstruction */
  caldevice=getMechanismID("calib",dummy); // Get mechanism device ID

  if(caldevice==-1) {
    sprintf(reply,"%s %s",who_selected,dummy);
    return CMD_ERR;
  }

  if(!strcasecmp(cmd_instruction,"ABORT")) {
    sprintf(dummy,"%c",27);     // Stop X axis
    ierr=agwcu("localhost",0,"mmcIC 1 ",dummy);
    sprintf(reply,"%s AGWXS=ABORT operation was aborted",who_selected); 
    return CMD_OK;

  } else if (!strcasecmp(cmd_instruction,"RDBITS")) {

    sprintf(dummy, "M#PRINT IO 22, IO 21", args);
    len=strlen(dummy);
    ierr = agwcu("localhost", 0, "mmcIC 1 ", dummy);

    if ( ierr != 0 ) {
      sprintf(reply, "%s", dummy);
      return CMD_ERR;

    }

    if ( ierr != 0 ) {
      sprintf(reply, "%s", dummy);
      return CMD_ERR;

    }
    sprintf(reply, "BITS %s", &dummy[6]);

    return CMD_OK;
  }

  validNumber = sscanf(cmd_instruction,"%d", &val);

  if (strlen(args)<=0) { // Query when no command is issued
    strcpy(who_selected,"AGWX");
    strcpy(get_buff,"x");
    ierr=agwcu("localhost",0,"getposition ",get_buff);
    if(ierr!=0) {
      sprintf(reply,"%s %s", who_selected,get_buff);
      return CMD_ERR;
    }

    sprintf(reply,"%s AGWXS=%0.3f", who_selected,
	    (atof(&get_buff[12])*shm_addr->MODS.convf[device]));

    return CMD_OK;

  } else if (!strcasecmp(cmd_instruction,"GETIP")) { // Mechanism IP
    sprintf(reply,"%s IEB=%d IP '%s' connection is '%s'",who_selected,
	    shm_addr->MODS.ieb_i[device],
  	    shm_addr->MODS.commport[device].Port,
	    (shm_addr->MODS.host[device]==1 ? "OPEN" : "CLOSED"));

    return CMD_OK;

  } else if(!strncasecmp(args,"M#",2)) { // check for low-level command
    sprintf(dummy,"%s",args);
    len=strlen(dummy);
    ierr=agwcu("localhost",0,"mmcIC 1 ",dummy);

    if(ierr!=0) {
      sprintf(reply,"%s %s", who_selected,dummy);
      return CMD_ERR;
    }

    sprintf(reply,"%s", &dummy[6]);
    return CMD_OK;

  }

  /*
  // Test to see if both limits are on
  */
  agwcu("localhost",0,"mmcIC 1 print io 22",dummy);
  agwcu("localhost",0,"mmcIC 1 print io 21",dummy1);
  sprintf(dummy,"%s%s",&dummy[6],&dummy1[6]);
  
  if((atobase(dummy,2))==3) {
    sprintf(reply,"%s %s=%0.3f Sensor Fault, both limits asserted",
	    who_selected,who_selected,
	    atof(&dummy[ierr])*shm_addr->MODS.convf[device]);
    return CMD_ERR;
    
  }
  
  if(!strcasecmp(cmd_instruction,"RESET")) {

    if ( mlcBitsBase10(caldevice,"PRINT IO 22,IO 21",dummy) == 1 ) {

      memset(dummy, 0, sizeof(dummy));
      ierr = agwcu("localhost", 0, "initx", dummy); // Home AGw X Stage

      if ( ierr != 0 ) {
	sprintf(reply, "%s %s", who_selected, dummy);
	return CMD_ERR;
      }
    }
    cmd_agwx("", EXEC, dummy);

    for(ierr=0;dummy[ierr]!='=';ierr++);
    ierr++;

    sprintf(reply,"%s AGWXS=%0.3f",who_selected,
	    atof(&dummy[ierr])*shm_addr->MODS.convf[device]);

    return CMD_OK;

  } else if(!strcasecmp(cmd_instruction,"STEP")) {
    GetArg(args,2,argbuf); // AGW X

    memset(get_buff,0,sizeof(get_buff));
    strcpy(get_buff,"x");
    ierr=agwcu("localhost",0,"getposition ",get_buff);
    if(ierr!=0) {
      sprintf(reply,"%s %s", who_selected, get_buff);
      return CMD_ERR;
    }
    stepMove=atof(&get_buff[12])+atof(argbuf);

    if(stepMove < shm_addr->MODS.min[device] || 
       stepMove > shm_addr->MODS.max[device]) {
      
      sprintf(reply,"%s %s=%0.3f Request 'STEP %0.3f', exceeds allowable range of %d..%d",
	      who_selected,who_selected,
	      fabs(shm_addr->MODS.pos[device]),
	      atof(argbuf),
	      (int)(shm_addr->MODS.min[device]),
	      (int)(shm_addr->MODS.max[device]));
      
      return CMD_ERR;
    }

    sprintf(get_buff,"setposition x %0.3f",fabs(stepMove));
    ierr=agwcu("localhost",0,get_buff,dummy);

    if(ierr!=0) {
      sprintf(reply,"%s %s",who_selected,dummy);
      return CMD_ERR;
    }
  
    for(ierr=0;dummy[ierr]!='=';ierr++);
    ierr++;
    sprintf(reply,"%s AGWXS=%0.3f",who_selected,
	    atof(&dummy[ierr])*shm_addr->MODS.convf[device]);

    return CMD_OK;

  } else if (validNumber==1) {

    GetArg(args,1,cmd_instruction); // AGW X

    if(val>shm_addr->MODS.max[device] || val<0) {
      sprintf(reply,"%s Invalid request %d, valid range 0..%0.3f",who_selected,val,shm_addr->MODS.max[device]);
      return CMD_ERR;
    }

    /* 
    // The added 4 option is to block until finished
    // without 4 option the immediate position will be return 
    */
    sprintf(get_buff,"setposition x %s 4",cmd_instruction);
    ierr=agwcu("localhost",0,get_buff,dummy);

    if(ierr!=0) {
      sprintf(reply,"%s %s",who_selected,dummy);
      return CMD_ERR;
    }

    cmd_agwx("", EXEC, reply);
    return CMD_OK;

  } else {
    sprintf(reply,"%s Invalid request '%s', Usage: agwx [xpos]",who_selected,args); 
    return CMD_ERR;
  }
  return CMD_OK;

}

/* ---------------------------------------------------------------------------
//
// agwfoc - AGW Focus MicroLYNX controller.
//
*/

/*!  
  \brief agwfoc command - AGW Stage Focus Position
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  agwfoc [fpos] 
  
  fpos - focus position in integer microns from mechanism home\n
  home - retract the AGW focus actuator to its home position.\n
  init - init is Focus towards the CCW limit switch 

  \par Description:
  Move the AGW stage pickoff mirror focus actuator to the specified position 
  in integer microns. The focus position is measured relative to the mechanism 
  home, which is fully retracted towards the motor. The focus motion is in the 
  same axis and sense as the AGWY mechanism.

  \par Interactions: Interlocked with the Calibration Tower 
  (hardware & software)

  Send AGW Focus MicroLYNX controller command via network or serial.
  MicroLYNX Controller commands given in MODS Mechanism Commands doc.
  http://www.astronomy.ohio-state.edu/~rgonzale/MODS_Mechanism_Commands.pdf

*/
int
cmd_agwfoc(char *args, MsgType msgtype, char *reply)
{
  int len;
  int device;
  int caldevice;
  int ierr;
  int bit2122;
  int val=0;
  int validNumber;
  float stepMove;
  char who_selected[24];
  char get_buff[80];
  char dummy[PAGE_SIZE];
  char dummy1[PAGE_SIZE];
  char cmd_instruction[PAGE_SIZE];

  memset(cmd_instruction,0,sizeof(cmd_instruction));
  memset(get_buff,0,sizeof(get_buff));
  memset(dummy,0,sizeof(dummy));
  validNumber = sscanf(args,"%d", &val);

  strcpy(who_selected,cmdtab[commandID].cmd);
  StrUpper(who_selected);

  device=getMechanismID(cmdtab[commandID].cmd,dummy);// Get mechanism device ID
  if(device==-1) {
    sprintf(reply,"%s %s",who_selected,dummy);
    return CMD_ERR;
  }

  caldevice=getMechanismID("calib",dummy); // Get mechanism device ID
  if(caldevice==-1) {
    sprintf(reply,"%s %s",who_selected,dummy);
    return CMD_ERR;
  }

  if(!shm_addr->MODS.host[device]) {
    sprintf(reply," %s %s=NOCOMM - NO COMMunication or not connected",who_selected,who_selected);
    return CMD_ERR;
  }

  StrUpper(args);
  GetArg(args,1,cmd_instruction);

  if(!strcasecmp(cmd_instruction,"ABORT")) {
    sprintf(dummy,"%c",27);     // Stop FOCUS axis
    ierr=agwcu("localhost",0,"mmcIC 4 ",dummy);
    if(ierr!=0) {
      sprintf(reply,"%s %s", who_selected, dummy);
      return CMD_ERR;
    }

    sprintf(reply,"%s AGWFS=ABORT operation was aborted",who_selected);

    return CMD_OK;
  }

  if (strlen(args)<=0) { // Query when no command is issued
    strcpy(who_selected,"AGWFOC");

    memset(dummy,0,sizeof(dummy));
    ierr=agwcu("localhost",0,"getfocus",dummy);
    if(ierr!=0) {
      sprintf(reply,"%s %s", who_selected,dummy);
      return CMD_ERR;
    }

    for(ierr=0;dummy[ierr]!='=';ierr++); // find the first '=' sign.
    ierr++;

    for(;dummy[ierr]!='=';ierr++); // now the second '=' sign.
    ierr++;

    sprintf(reply,"%s AGWFS=%0.3f",who_selected,
	    atof(&dummy[ierr])*shm_addr->MODS.convf[device]);

    return CMD_OK;
  }

  StrUpper(args);
  GetArg(args,1,cmd_instruction);

  if (!strcasecmp(cmd_instruction,"GETIP")) { // Mechanism IP
    sprintf(reply,"%s IEB=%d IP '%s' connection is '%s'",who_selected,
	    shm_addr->MODS.ieb_i[device],
  	    shm_addr->MODS.commport[device].Port,
	    (shm_addr->MODS.host[device]==1 ? "OPEN" : "CLOSED"));

    return CMD_OK;

  } else if (!strcasecmp(cmd_instruction,"RDBITS")) {
    sprintf(dummy, "M#PRINT IO 22, IO 21", args);
    len=strlen(dummy);
    ierr = agwcu("localhost", 0, "mmcIC 4 ", dummy);
    if ( ierr != 0 ) {
      sprintf(reply, "%s", dummy);
      return CMD_ERR;

    }

    if ( ierr != 0 ) {
      sprintf(reply, "%s", dummy);
      return CMD_ERR;

    }
    sprintf(reply, "BITS %s", &dummy[6]);

    return CMD_OK;

  } else if(!strncasecmp(args,"M#",2)) { // check for low-level command
    sprintf(dummy,"%s",args);
    len=strlen(dummy);
    ierr=agwcu("localhost",0,"mmcIC 4 ",dummy);

    if(ierr!=0) {
      sprintf(reply,"%s %s ", who_selected, dummy);
      return CMD_ERR;
    }
    sprintf(reply,"%s", &dummy[6]);

    return CMD_OK;
  }

  if(!strcasecmp(cmd_instruction,"RESET")) {

    /* Check the for Calibration Tower obstruction */
    if ( mlcBitsBase10(caldevice,"PRINT IO 22,IO 21",dummy) == 1 ) {

      ierr = agwcu("localhost", 0, "initfoc", dummy); // Home/Init Focus

      if ( ierr != 0 ) {
	sprintf(reply, "%s %s", who_selected, dummy);
	return CMD_ERR;
      }
    }
    ierr = agwcu("localhost", 0, "getfocus", dummy); // Home/Init Focus
      
    for(ierr=0;dummy[ierr]!='=';ierr++);
    ierr++;

    for(;dummy[ierr]!='=';ierr++); // now the second '=' sign.
    ierr++;

    sprintf(reply,"%s AGWFS=%0.3f",who_selected,
    	    atof(&dummy[ierr])*shm_addr->MODS.convf[device]);

    return CMD_OK;

  } else if(!strcasecmp(cmd_instruction,"STEP")) {
    GetArg(args,2,argbuf); // AGW FOCUS

    memset(get_buff,0,sizeof(get_buff));
    strcpy(get_buff,"focus");
    ierr=agwcu("localhost",0,"getposition ",get_buff);

    if(ierr!=0) {
      sprintf(reply,"%s %s", who_selected, get_buff);
      return CMD_ERR;
    }
    stepMove=atof(&get_buff[12])+atof(argbuf);

    if(stepMove < shm_addr->MODS.min[device] || 
       stepMove > shm_addr->MODS.max[device]) {
      
      sprintf(reply,"%s %s=%0.3f Request 'STEP %0.3f' exceeds allowable range of %d..%d",who_selected,who_selected,
	      fabs(shm_addr->MODS.pos[device]*2.0),
	      atof(argbuf),
	      (int)(shm_addr->MODS.min[device]),
	      (int)(shm_addr->MODS.max[device]));

      return CMD_ERR;
    }

    sprintf(get_buff,"setposition focus %0.3f",fabs(stepMove));
    ierr=agwcu("localhost",0,get_buff,dummy);

    if(ierr!=0) {
      sprintf(reply,"%s %s",who_selected,dummy);
      return CMD_ERR;
    }

    for(ierr=0;dummy[ierr]!='=';ierr++);
    ierr++;
    sprintf(reply,"%s AGWXS=%0.3f",who_selected,
	    atof(&dummy[ierr])*shm_addr->MODS.convf[device]);

    return CMD_OK;

  } else if (validNumber==1) {

    GetArg(args,1,cmd_instruction); // AGW Focus

    if(val>shm_addr->MODS.max[device] || val<0) {
      sprintf(reply,"%s Invalid request %d, valid range 0..%0.3f",who_selected,val,shm_addr->MODS.max[device]);
      return CMD_ERR;
    }

    /* Check for Calibration Tower obstruction */
    if ( mlcBitsBase10(caldevice,"PRINT IO 22,IO 21",dummy) != 1 ) {

      sprintf(reply,"%s Calibration Tower is in the beam, move disallowed,",who_selected);
      return CMD_ERR;
      
    }

    sprintf(get_buff,"setposition focus %s 4",cmd_instruction);
    ierr=agwcu("localhost",0,get_buff,dummy);

    if(ierr!=0) {
      sprintf(reply,"%s %s",who_selected,dummy);
      return CMD_ERR;
    }

    cmd_agwfoc("", EXEC, reply);
    return CMD_OK;

  } else {
    sprintf(reply,"%s Invalid request '%s', Usage: agwfoc [0..%0.3f]",who_selected,args,shm_addr->MODS.max[device]);
    return CMD_ERR;
  }

  return CMD_OK;
}

/* ---------------------------------------------------------------------------
//
// agwfilt - AGW Filter Wheel MicroLYNX controller.
//
*/

/*!  
  \brief agwfilt command - AGW Guide Camera Filter Wheel
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  agwfilt [n|ID]

  n - select AGW filter by position number (1..4)\n
  ID - select AGW filter by name

  \par Description:
  Select the filter to put in front of the acquisition and guide CCD camera. 
  Note that the light going into the off-axis wavefront sensor (WFS) camera 
  is unfiltered.\n
  AGW filter names are stored in an external AGW filter table populated via 
  the MODS Engineering Support Interface (MESI).

  \par Interactions: none

  Send AGW Filter Wheel MicroLYNX controller command via network or serial.
  MicroLYNX Controller commands given in MODS Mechanism Commands doc.
  http://www.astronomy.ohio-state.edu/~rgonzale/MODS_Mechanism_Commands.pdf

*/
int
cmd_agwfilt(char *args, MsgType msgtype, char *reply)
{
  int device;
  int ierr;
  int len;
  int val;
  char strBits[4];
  char who_selected[24];
  char get_buff[80];
  char dummy[PAGE_SIZE];
  char cmd_instruction[PAGE_SIZE];

  memset(cmd_instruction,0,sizeof(cmd_instruction));
  memset(get_buff,0,sizeof(get_buff));
  memset(dummy,0,sizeof(dummy));

  StrUpper(args);
  GetArg(args,1,cmd_instruction);
  ierr = sscanf(cmd_instruction,"%d", &val);

  strcpy(who_selected,cmdtab[commandID].cmd);
  StrUpper(who_selected);

  device=getMechanismID(cmdtab[commandID].cmd,dummy);// Get mechanism device ID
  if(device==-1) {
    sprintf(reply,"%s %s",who_selected,dummy);
    return CMD_ERR;
  }


  if(!shm_addr->MODS.host[device]) {
    sprintf(reply," %s %s=NOCOMM - NO COMMunication or not connected",who_selected,who_selected);
    return CMD_ERR;
  }

  if(!strcasecmp(cmd_instruction,"ABORT")) {
    sprintf(dummy,"%c",27);     // Stop FOCUS axis
    ierr=agwcu("localhost",0,"mmcIC 8 ",dummy);
    if(ierr!=0) {
      sprintf(reply,"%s %s", who_selected, dummy);
      return CMD_ERR;
    }

    sprintf(reply,"%s AGWFILT=ABORT operation was aborted",who_selected);

    return CMD_OK;

  } else if (!strcasecmp(cmd_instruction,"RDBITS")) {

    sprintf(dummy, "M#PRINT IO 22, IO 21", args);
    len=strlen(dummy);
    ierr = agwcu("localhost", 0, "mmcIC 8 ", dummy);
    if ( ierr != 0 ) {
      sprintf(reply, "%s", dummy);
      return CMD_ERR;

    }

    if ( ierr != 0 ) {
      sprintf(reply, "%s", dummy);
      return CMD_ERR;

    }
    sprintf(reply, "BITS %s", &dummy[6]);

    return CMD_OK;
  }

  if (strlen(args)<=0) { // Query when no command is issued
    strcpy(who_selected,"AGWFILT");

    if(shm_addr->MODS.busy[device]==1) {
      sprintf(reply,"%s not finished moving",who_selected);
      return CMD_ERR;
    }

    ierr=agwcu("localhost",0,"getfilter",get_buff);
    if(ierr!=0) {
      sprintf(reply,"%s %s", who_selected, get_buff);
      return CMD_ERR;
    }
    sprintf(reply,"%s %s", who_selected, &get_buff[6]);

    return CMD_OK;

  } else if (!strcasecmp(cmd_instruction,"GETIP")) { // Mechanism IP
    sprintf(reply,"%s IEB=%d IP '%s' connection is '%s'",who_selected,
	    shm_addr->MODS.ieb_i[device],
  	    shm_addr->MODS.commport[device].Port,
	    (shm_addr->MODS.host[device]==1 ? "OPEN" : "CLOSED"));

    return CMD_OK;

  } else if(!strcasecmp(cmd_instruction,"RESET")) {

    if(shm_addr->MODS.busy[device]==1) {
      sprintf(reply,"%s is busy, wait until finished or abort then try again.",who_selected);
      return CMD_ERR;
    }

    cmd_agwfilt("", EXEC, reply);

  } else if(!strncasecmp(args,"M#",2)) { // check for low-level command

    if(shm_addr->MODS.busy[device]==1) {
      sprintf(reply,"%s is busy, wait until finished or abort then try again.",who_selected);
      return CMD_ERR;
    }

    sprintf(dummy,"%s",args);
    len=strlen(dummy);
    ierr=agwcu("localhost",0,"mmcIC 8 ",dummy);

    if(ierr!=0) {
      sprintf(reply,"%s %s", who_selected, dummy);
      return CMD_ERR;
    }

    sprintf(reply,"%s %s", who_selected, &dummy[6]);

  } else if (ierr>0) {
    val=atoi(cmd_instruction);
    if(val<=0 || val>4) {
      sprintf(reply,"%s Invalid request '%s', valid range 1..4",who_selected, args);
      return CMD_ERR;

    }

    if(shm_addr->MODS.busy[device]==1) {
      sprintf(reply,"%s is busy, wait until finished or abort then try again.",who_selected);
      return CMD_ERR;
    }

    sprintf(get_buff,"setfilter %s",cmd_instruction);
    ierr=agwcu("localhost",0,get_buff,dummy);
    if(ierr!=0) {
      sprintf(reply,"%s %s", who_selected,dummy);
      return CMD_ERR;
    }
    cmd_agwfilt("", EXEC, reply);

  } else {
    sprintf(reply,"%s Invalid request '%s', Usage: agwfilt [filter]",who_selected,args); 
    return CMD_ERR;
  }
  return CMD_OK;
}

/* ---------------------------------------------------------------------------
//
// dichroic - dichroic MicroLYNX controller.
//
*/

/*!  
  \brief dichroic command - command the Dichroic
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  dichroic [n|blue|red|both]

  Send Dichroic MicroLYNX controller command via network or serial.
  MicroLYNX Controller commands given in MODS Mechanism Commands doc.
  http://www.astronomy.ohio-state.edu/~rgonzale/MODS_Mechanism_Commands.pdf

*/
int
cmd_dichroic(char *args, MsgType msgtype, char *reply)
{
  int ierr;
  int device;
  int dichnum;
  int dich_pos;
  int inPos;
  char who_selected[24];
  char dichroic_selected[24];
  char cmd_instruction[PAGE_SIZE];
  char dummy[PAGE_SIZE];

  memset(cmd_instruction,0,sizeof(cmd_instruction));
  memset(dummy,0,sizeof(dummy));
  ierr = sscanf(args,"%d", &dichnum);
  StrUpper(args);

  GetArg(args,1,cmd_instruction);

  if(!strcasecmp(cmd_instruction,"RDTAB")) { // re-read the filter tables
    system("/usr/local/bin/mlcRecover dichroic");
    sprintf(reply,"%s %s table read",who_selected,cmd_instruction);
    return CMD_OK;
  }

  strcpy(who_selected,cmdtab[commandID].cmd);
  StrUpper(who_selected);

  /* Search for the IP and Socket */
  device=getMechanismID(cmdtab[commandID].cmd,dummy);// Get mechanism device ID
  if(device==-1) {
    sprintf(reply,"%s %s",who_selected,dummy);
    return CMD_ERR;
  }

  if(!strcasecmp(cmd_instruction,"ABORT")) { // STOP MicroLynx Controller(MLC) operation
    ierr = mlcStopMechanism(device,dummy);
    sprintf(reply,"%s %s",who_selected,dummy);
    return CMD_OK;
  }
  
  if (!strcasecmp(cmd_instruction,"CONFIG")) { // get mechanism ip

    mlcMechanismConfig(device,who_selected,dummy);
    sprintf(reply,"%s",dummy);

    return CMD_OK;
  }

  if (mlcBusy(device,dummy)) { // Busy?
    sprintf(reply,"%s %s",who_selected,dummy);
    return CMD_ERR;
  }
  
  ierr=mlcQuery(device,1,dummy); // Check if the microLynx is ON
  if(ierr!=0) {
    sprintf(reply,"%s %s",who_selected,dummy);
    return CMD_ERR;
  }

  /*
  wagoRW(shm_addr->MODS.ieb_i[device],"MLCS",0,device,dummy);
  if(strstr(dummy,"OFF")) {
    if(device>15)
      sprintf(reply,"%s MLCERR=FAULT %s controller power turned OFF",
	      who_selected,who_selected);
    else sprintf(reply,"%s MLCERR=FAULT %s controller power turned OFF",
		 who_selected,who_selected);
    return CMD_ERR;
  }
  */
  if(!strcasecmp(cmd_instruction,"LOCK")) { // Abort process
    shm_addr->MODS.LOCKS[device]=1;
    sprintf(reply," %s %s=LOCK Mechanism is locked, Check Field Lens Cover remove it before 'dichroic UNLOCK'",
	    who_selected,who_selected);
    return CMD_OK;

  } else if(!strcasecmp(cmd_instruction,"UNLOCK")) { // Abort process
    shm_addr->MODS.LOCKS[device]=0;
    sprintf(reply," %s %s=UNLOCK Mechanism is unlocked",
	    who_selected,who_selected);
    return CMD_OK;
  }


  if(!shm_addr->MODS.host[device]) {
    sprintf(reply," %s %s=NOCOMM No IP address configured, check mechanisms.ini file",who_selected,who_selected);
    return CMD_ERR;
  } else if ( shm_addr->MODS.LOCKS[device] ) {
    sprintf(reply," %s %s=FAULT connection LOCKED OUT, Check Field Lens Cover remove it then '%s UNLOCK' to continue",who_selected,who_selected,who_selected);

    return CMD_ERR;
  }

  if(!strcasecmp(cmd_instruction,"RESET")) {  // initialize

    rawCommand(device,"PWRFAIL=0",dummy);

    rawCommand(device,"INITIAL",dummy);
    MilliSleep(500);

    rawCommand(device,"PRINT IO 24",dummy);
    if(!atoi(dummy)) {
      sprintf(reply,"%s %s=FAULT Mechanism out-of-position, in-position sensor not asserted. Reset %s to recover",who_selected,who_selected,who_selected);
      return CMD_ERR;
    }

    dich_pos=mlcBitsBase10(device,"PRINT IO 22,IO 21",dummy);
    inPos=mlcBitsBase10(device,"PRINT IO 24",dummy);

    if(dich_pos==0) {
      sprintf(reply,"%s %s=FAULT in-position asserted with index sensor fault, Step to recover",who_selected,who_selected,dich_pos);
      return CMD_ERR;
    }      

    shm_addr->MODS.pos[device]=(float)dich_pos;
    shm_addr->MODS.reqpos[device]=(float)dich_pos;
    sprintf(reply,"%s %s=%d DICHNAME='%s'",who_selected,who_selected,dich_pos, shm_addr->MODS.dichroicName[dich_pos]);
    return CMD_OK;

  }

  //  if (strncasecmp(args,"M#",2)) { // check for low-level command
  //    ierr=checkPower(device,dummy); // Check mechanism power bit.
  //    if(ierr!=0) {
  //      sprintf(reply,"%s %s",who_selected,dummy);
  //      return CMD_ERR;
  //    }
  //  }

  if (!strcasecmp(cmd_instruction,"?")) {
     sprintf(reply,"Usage: %s [n|red|blue|both] - select a dichroic turret position", who_selected);
     return CMD_OK;
  }

  if (strlen(args)<=0) {  // Query when no arguments given
    strcpy(who_selected,"DICHROIC");

    rawCommand(device,"PWRFAIL=0",dummy);

    rawCommand(device,"PRINT IO 24",dummy);
    if(!atoi(dummy)) {
      sprintf(reply,"%s %s=FAULT Mechanism out-of-position, in-position sensor not asserted. Reset %s to recover",who_selected,who_selected,who_selected);
      return CMD_ERR;
    }

    dich_pos=mlcBitsBase10(device,"PRINT IO 22,IO 21",dummy);
    inPos=mlcBitsBase10(device,"PRINT IO 24",dummy);

    if(dich_pos==0) {
      if(!inPos)
	sprintf(reply,"%s %s=FAULT Mechanism out-of-position, in-position sensor not asserted. Reset %s to recover",who_selected,who_selected,who_selected);
      else
	sprintf(reply,"%s %s=FAULT 'Sensors in-position with unknown index position'",who_selected,who_selected);
      return CMD_ERR;
    }      

    sprintf(reply,"%s %s=%d DICHNAME='%s'",who_selected,who_selected,dich_pos, shm_addr->MODS.dichroicName[dich_pos]);

    shm_addr->MODS.pos[device]=(float)dich_pos;
    shm_addr->MODS.reqpos[device]=(float)dich_pos;

    return CMD_OK;
  }

  /* dichroic [n|blue|red|both] */
  if(!strncasecmp(args,"M#",2)) { // check for low-level command
    ierr=mlcTechCmd(device,args,who_selected,reply);
    if(ierr<=-1) return CMD_ERR;

  } else if (!strcasecmp(cmd_instruction,"STEP")) {

    GetArg(args,2,argbuf); // Get step count
    rawCommand(device,"DRVEN=1",dummy);
    
    rawCommand(device,"PRINT IO 24",dummy);
    if(!atoi(dummy)) {
      sprintf(reply,"%s %s=FAULT Mechanism out-of-position, in-position sensor not asserted. Reset %s to recover",who_selected,who_selected,who_selected);
      rawCommand(device,"DRVEN=0",dummy);
      return CMD_ERR;
    }

    if(atoi(argbuf)<0)
      sprintf(dichroic_selected,"MOVR -DIST");
    else
      sprintf(dichroic_selected,"MOVR DIST");

    dichnum=abs(atoi(argbuf));

    if(dichnum>1) {
      sprintf(reply,"%s %s=FAULT Invalid %s STEPs are +/-1",
	      who_selected,who_selected,argbuf);
      rawCommand(device,"DRVEN=0",dummy);
      return CMD_ERR;
    }


    dich_pos=mlcBitsBase10(device,"PRINT IO 22,IO 21",dummy);

    //(dich_pos==0) {
    //    sprintf(reply,"%s %s=FAULT starting from invalid position, Reset %s to recover",who_selected,who_selected,who_selected);
    //    rawCommand(device,"DRVEN=0",dummy);
    //    return CMD_ERR;
    //  }

    //if (dich_pos+atoi(argbuf)>3 || dich_pos+atoi(argbuf)<=0) {
    if (dich_pos+atoi(argbuf)>3 || dich_pos+atoi(argbuf)<0) {
      sprintf(reply,"%s %s=FAULT Mechanism will move past allowable range 1..3",who_selected,who_selected,argbuf);
      rawCommand(device,"DRVEN=0",dummy);
      return CMD_ERR;
    }

    sendCommand(device,dichroic_selected,dummy);

    for(int i=0;i<40000;i++) {
      rawCommand(device,"PRINT MVG",dummy);
      if(!strcasecmp(dummy,"FALSE")) break;
      MilliSleep(500);
    }
    
    //sprintf(filter_selected,"TARGNUM=%d",filter_pos);
    //ierr = sendTwoCommand(device,filter_selected,"BEGIN",dummy);
    
    dich_pos=mlcBitsBase10(device,"PRINT IO 22,IO 21",dummy);

    rawCommand(device,"PRINT IO 24",dummy);
    if(!atoi(dummy)) {
      sprintf(reply,"%s %s=FAULT Mechanism out-of-position, in-position sensor not asserted. Reset %s to recover",who_selected,who_selected,who_selected);
      rawCommand(device,"DRVEN=0",dummy);
      return CMD_ERR;
    }

    if(dich_pos==0) {
      sprintf(reply,"%s %s=FAULT Invalid position, Reset %s to recover",who_selected,who_selected,who_selected);
      rawCommand(device,"DRVEN=0",dummy);
      return CMD_ERR;
    }

    shm_addr->MODS.pos[device]=(float)dich_pos;
    shm_addr->MODS.reqpos[device]=(float)dich_pos;

    sprintf(reply,"%s %s=%d DICHNAME='%s'",who_selected,who_selected,dich_pos,shm_addr->MODS.dichroicName[dich_pos]);

    rawCommand(device,"DRVEN=0",dummy);

    return CMD_OK;

  } else if (!strcasecmp(cmd_instruction,"RDBITS")) {

    dich_pos = mlcBitsBase10(device,"PRINT IO 22,IO 21",dummy);
    inPos = mlcBitsBase10(device,"PRINT IO 24",dummy);
    rawCommand(device,"PRINT IO 24,IO 22,IO 21",dummy);

    if(!dich_pos) {
      if(!inPos)
	sprintf(reply,"%s %s=FAULT BITS=%s 'in-position sensor not asserted. Reset %s to recover'",who_selected,who_selected,dummy,who_selected);
      else
	sprintf(reply,"%s %s=FAULT BITS=%s 'in-position sensor asserted, check index sensors b2, b1'", who_selected, who_selected,dummy);
      return CMD_OK;
    } else
      sprintf(reply,"%s %s=%d BITS=%s 'Sensors in-position msb3 index b2,b1 '",
	      who_selected,who_selected,dich_pos,dummy);

    return CMD_OK;
    /*
  } else if(!strcasecmp(cmd_instruction,"RED")) {
    dich_pos=mlcBitsBase10(device,"PRINT IO 22,IO 21",dummy);
    MilliSleep(100);

    rawCommand(device,"PRINT IO 24",dummy);
    if(!atoi(dummy)) {
      sprintf(reply,"%s %s=FAULT Mechanism out-of-position, in-position sensor not asserted. Reset %s to recover",who_selected,who_selected,who_selected);
      return CMD_ERR;
    }

    ierr=mlcBitsBase10(device,"PRINT IO 22,IO 21",dummy);
    if(ierr==1) {
      sprintf(reply,"%s %s=%s Already in position RED",
	      who_selected,who_selected,dummy);
      return CMD_OK;
    }

    sprintf(dichroic_selected,"TARGNUM=1");
    ierr = sendTwoCommand(device,dichroic_selected,"BEGIN",dummy);

    if(ierr!=0) {
      sprintf(reply,"%s %s=%s",who_selected,who_selected,dummy);
      return CMD_ERR;
    }

    dich_pos=mlcBitsBase10(device,"PRINT IO 22,IO 21",dummy);
    sprintf(shm_addr->MODS.state_word[device],"%d",dich_pos); 
    sprintf(reply,"%s %s=%d",who_selected,who_selected,dich_pos);

    shm_addr->MODS.pos[device]=(float)dich_pos;
    shm_addr->MODS.reqpos[device]=1.0;

  } else if(!strcasecmp(cmd_instruction,"BLUE")) { // ask for help
    dich_pos=mlcBitsBase10(device,"PRINT IO 22,IO 21",dummy);
    MilliSleep(100);

    rawCommand(device,"PRINT IO 24",dummy);
    if(!atoi(dummy)) {
      sprintf(reply,"%s %s=FAULT Mechanism out-of-position, in-position sensor not asserted. Reset %s to recover",who_selected,who_selected,who_selected);
      return CMD_ERR;
    }

    ierr=mlcBitsBase10(device,"PRINT IO 22,IO 21",dummy);
    if(ierr==3) {
      sprintf(reply,"%s %s=%d Already in position BLUE",
	      who_selected,who_selected,ierr);
      return CMD_OK;
    }

    sprintf(dichroic_selected,"TARGNUM=3");
    ierr = sendTwoCommand(device,dichroic_selected,"BEGIN",dummy);

    if(ierr!=0) {
      sprintf(reply,"%s %s=%s",who_selected,who_selected,dummy);
      return CMD_ERR;
    }

    dich_pos=mlcBitsBase10(device,"PRINT IO 22,IO 21",dummy);
    sprintf(shm_addr->MODS.state_word[device],"%d",dich_pos); 
    sprintf(reply,"%s %s=%d",who_selected,who_selected,dich_pos);

    shm_addr->MODS.pos[device]=(float)dich_pos;
    shm_addr->MODS.reqpos[device]=3.0;

  } else if(!strcasecmp(cmd_instruction,"BOTH")) { // ask for help
    dich_pos=mlcBitsBase10(device,"PRINT IO 22,IO 21",dummy);
    MilliSleep(100);

    rawCommand(device,"PRINT IO 24",dummy);
    if(!atoi(dummy)) {
      sprintf(reply,"%s %s=FAULT Mechanism out-of-position, in-position sensor not asserted. Reset %s to recover",who_selected,who_selected,who_selected);
      return CMD_ERR;
    }

    ierr=mlcBitsBase10(device,"PRINT IO 22,IO 21",dummy);
    if(ierr==2) {
      sprintf(reply,"%s %s=%s Already in position BOTH",
	      who_selected,who_selected,dummy);
      return CMD_OK;
    }

    sprintf(dichroic_selected,"TARGNUM=2");
    ierr = sendTwoCommand(device,dichroic_selected,"BEGIN",dummy);

    if(ierr!=0) {
      sprintf(reply,"%s %s=%s",who_selected,who_selected,dummy);
      return CMD_ERR;
    }

    dich_pos=mlcBitsBase10(device,"PRINT IO 22,IO 21",dummy);
    sprintf(shm_addr->MODS.state_word[device],"%d",dich_pos); 
    sprintf(reply,"%s %s=%d",who_selected,who_selected,dich_pos);

    shm_addr->MODS.pos[device]=(float)dich_pos;
    shm_addr->MODS.reqpos[device]=2.0;
    */
  } else if (dichnum>=1 && dichnum<4) { 

    rawCommand(device,"PWRFAIL=0",dummy);

    rawCommand(device,"PRINT IO 24",dummy);
    if(!atoi(dummy)) {
      sprintf(reply,"%s %s=FAULT Mechanism out-of-position, in-position sensor not asserted. Reset %s to recover",who_selected,who_selected,who_selected);
      return CMD_ERR;
    }

    dich_pos=mlcBitsBase10(device,"PRINT IO 22,IO 21",dummy);

    if(dich_pos==0) {
      sprintf(reply,"%s %s=FAULT starting from invalid position, Reset or Step %s to recover",who_selected,who_selected,who_selected);
      return CMD_ERR;
    }

    if(dich_pos==dichnum) {

      sprintf(reply,"%s %s=%d DICHNAME='%s'",who_selected,who_selected,dich_pos,shm_addr->MODS.dichroicName[dich_pos]);

      return CMD_OK;
    }

    memset(dummy,0,sizeof(dummy));
    sprintf(dichroic_selected,"TARGNUM=%d",dichnum);
    ierr = sendTwoCommand(device,dichroic_selected,"BEGIN",dummy);

    dich_pos=mlcBitsBase10(device,"PRINT IO 22,IO 21",dummy);
    inPos=mlcBitsBase10(device,"PRINT IO 24",dummy);

    if(!dich_pos) {
      if(!inPos)
      sprintf(reply,"%s %s=FAULT Mechanism out-of-position, in-position sensor not asserted. Reset %s to recover",who_selected,who_selected,who_selected);
      else
	sprintf(reply,"%s %s=FAULT 'Sensors in-position with unknown index position'",who_selected,who_selected);
      return CMD_ERR;

    } else if(dichnum!=dich_pos) {
      sprintf(reply,"%s %s=%d Move Fault, position at end of move %d but requested position %d",who_selected,who_selected,dich_pos,dich_pos,dichnum);
      return CMD_ERR;

    } else
      sprintf(reply,"%s %s=%d DICHNAME='%s'",who_selected,who_selected,dich_pos,shm_addr->MODS.dichroicName[dich_pos]);

    shm_addr->MODS.pos[device]=(float)dich_pos;
    shm_addr->MODS.reqpos[device]=(float)dichnum;
    sprintf(shm_addr->MODS.state_word[device],"%d",dich_pos); 

  } else {
    sprintf(reply,"%s Invalid request '%s', Usage: dichroic [1..3|blue|red|both]",who_selected,args); 
    return CMD_ERR;
  }

  return CMD_OK;

}

/* ---------------------------------------------------------------------------
//
// gprobe - AGW Probe MicroLYNX controller.
//
*/

/*!  
  \brief gprobe command - Move AGW guide probe to an absolute X,Y position
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  gprobe [Ygp Xgp|reset]

  XGP - guide probe field center X-axis position in millimeters\n
  YGP - guide probe field center Y-axis position in millimeters\n
  home - home the guide probe

  \par Description:
  Move the AGW stage pickoff mirror center to the specified absolute (X,Y) 
  position in the MODS science focal plane, including autofocus adjustment for 
  the curved focal plane. Coordinates are given in millimeters from the center 
  of the science field, as described in the MODS AGW Specification document. 
  When the instrument PA is 0deg, +XGP is East, -XGP is West, +YGP is North, 
  and -YGP is South. The origin (XGP,YGP)=(0,0) is the center of the guider 
  camera field of view at the MODS science field center (the optical axis of
  the focus).

  If the home keyword is given, it retracts the guide probe to its home 
  (mechanical zero) position, out of the science beam. This is equivalent to 
  giving the individual commands "AGWX home", "AGWY home", and "AGWFS home".

  Both XGP and YGP must be given device coordinates and then executes 
  equivalent AGWX, AGWY, and AGWFS motions.

  \par Interactions: Interlocked with the Calibration Tower 
  (hardware & software).

  Send Guide Probe MicroLYNX controller command via network or serial.
  MicroLYNX Controller commands given in MODS Mechanism Commands doc.
  http://www.astronomy.ohio-state.edu/~rgonzale/MODS_Mechanism_Commands.pdf

*/
int
cmd_gprobe(char *args, MsgType msgtype, char *reply)
{
  int ierr;
  int device;
  int device2;
  int caldevice;
  char Xval[10];
  char Yval[10];
  char get_buff[PAGE_SIZE];
  char dummy[PAGE_SIZE];
  char who_selected[24];
  char cmd_instruction[PAGE_SIZE];

  memset(cmd_instruction, 0, sizeof(cmd_instruction));
  memset(get_buff, 0, sizeof(get_buff));
  memset(dummy, 0, sizeof(dummy));

  strcpy(who_selected, cmdtab[commandID].cmd);
  StrUpper(who_selected);

  caldevice = getMechanismID("calib", dummy); // Get mechanism device ID
  if ( caldevice == -1 ) {
    sprintf(reply,"%s %s", who_selected, dummy);
    return CMD_ERR;

  }

  StrUpper(args);
  GetArg(args, 1, cmd_instruction);

  /* Search for the IP and Socket */
  device = getMechanismID("agwy", dummy); // Get mechanism device ID
  if ( device == -1 ) {
    sprintf(reply, "%s %s", who_selected, dummy);
    return CMD_ERR;

  }

  if( shm_addr->MODS.host[device] == 0 ) {
    sprintf(reply, " %s AGWY=NOCOMM - NO COMMunication or not connected", who_selected);
    return CMD_ERR;

  }

  //<! Search for the IP and Socket

  device2 = getMechanismID("agwx", dummy); // Get mechanism device ID

  if ( device2 == -1 ) {
    sprintf(reply, "%s %s", who_selected, dummy);
    return CMD_ERR;

  }

  if(!shm_addr->MODS.host[device2]) {
    sprintf(reply," %s AGWXS=NOCOMM - NO COMMunication or not connected",who_selected);
    return CMD_ERR;
  }

  ierr = sscanf(args, "%s %s", Xval,Yval);

  if ( strlen(args) <= 0 ) { // Query when no command is issued

    memset(get_buff, 0, sizeof(get_buff));

    ierr = agwcu("localhost", 0, "getxy", get_buff);
    if ( ierr != 0 ) {
      sprintf(reply,"%s %s", who_selected, get_buff);
      return CMD_ERR;

    }

    sprintf(reply,"%s %s", who_selected, &get_buff[6]);

    return CMD_OK;

  } else if ( strcasecmp(cmd_instruction,"RESET") == 0 ||
	      strcasecmp(cmd_instruction,"HOME") == 0 ) {

    //<! Check the for Calibration Tower obstruction

    GetArg(dummy, 1, argbuf);

    if ( mlcBitsBase10(caldevice, "PRINT IO 22, IO 21", dummy) == 1 ) {
      ierr = agwcu("localhost", 0, "init", get_buff); // Home AGW

    }

    memset(get_buff,0,sizeof(get_buff));

    ierr=agwcu("localhost",0,"getxy", get_buff);

    if ( ierr != 0 ) {
      sprintf(reply, "%s %s", who_selected, get_buff);
      return CMD_ERR;
      
    }
    
    sprintf(reply,"%s %s",who_selected, &get_buff[6]);

    return CMD_OK;

  } else if ( ierr == 2 ) {

    /* 
    // Check the for Calibration Tower obstruction
    */
    GetArg(dummy, 1, argbuf);

    if ( ierr = mlcBitsBase10(caldevice, "PRINT IO 22, IO 21", dummy) == 2 ) {
      sprintf(reply, "%s Calibration Tower is in the beam, move disallowed[%d][%s]", who_selected, ierr, dummy);
      return CMD_ERR;

    }

    ierr = mods_setxy("localhost", 0, atof(Xval), atof(Yval), 0.0, 0);

    if ( ierr != 0 ) {
      sprintf(reply, "%s %s", who_selected, mods_fatalerrormsg[ierr]);
      return CMD_ERR;

    }

    MilliSleep(100); // Give 100 millisec to settle

    memset(get_buff,0,sizeof(get_buff));

    ierr = agwcu("localhost", 0, "getxy", get_buff);

    if ( ierr != 0 ) {
      sprintf(reply, "%s %s", who_selected, get_buff);
      return CMD_ERR;

    }

    sprintf(reply, "%s %s", who_selected, &get_buff[6]);
    return CMD_OK;

  } else {
    sprintf(reply,"%s Invalid request '%s', Usage: gprobe [Xgp Ygp]",who_selected,args); 
    return CMD_ERR;
  }
  return CMD_OK;

}

/* ---------------------------------------------------------------------------
//
// gpoffset - AGW Probe Offset MicroLYNX controller.
//
*/

/*!  
  \brief gpoffset command - Offset the AGW guide probe deltaX, deltaY
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage: GPOFFSET [dXGP dYGP]
  
  deltaXGP - guide probe X-axis offset in mm\n
  deltaYGP - guide probe Y-axis offset in mm\n

  \par Description:
  Offset (increment) the AGW stage pickoff mirror center by the specified 
  relative amount in millimeters. When the instrument PA is 0deg,
  +deltaXGP is East, -deltaXGP is West, +deltaYGP is North, and 
  -deltaYGP is South.

  Both deltaXGP and deltaYGP must be given. If no offset is required along a 
  given axis, 0 must be given as a placeholder.

  GPOFFSET does a relative move, whereas GPROBE does an absolute "move to". 
  Like GPROBE, this command is a user-level meta command that transforms 
  guide-probe focal plane coordinates into device coordinates then executes 
  equivalent AGWX, AGWY, and AGWFS motions.

  \par Interactions: Interlocked with the Calibration Tower (hardware & software).

  Send Guide Probe Offset MicroLYNX controller command via network or serial.
  MicroLYNX Controller commands given in MODS Mechanism Commands doc.
  http://www.astronomy.ohio-state.edu/~rgonzale/MODS_Mechanism_Commands.pdf

*/
int
cmd_gpoffset(char *args, MsgType msgtype, char *reply)
{
  int ierr, len;
  double Xval;
  double Yval;
  char xoff[20];
  char yoff[20];
  char get_buff[128];
  char who_selected[24];

  strcpy(who_selected,cmdtab[commandID].cmd);
  StrUpper(who_selected);

  Xval=0.0;
  Yval=0.0;
  ierr=0;
  ierr = sscanf(args,"%s %s",xoff,yoff);
  memset(get_buff,0,sizeof(get_buff));

  if (strlen(args)<=0) { // Query when no command is issued
    strcpy(who_selected,"GPOFFSET");

    ierr=agwcu("localhost",0,"getxy", get_buff);

    if(ierr!=0) {
      sprintf(reply,"%s %s", who_selected, get_buff);
      return CMD_ERR;
    }
    sprintf(reply,"%s %s",who_selected,&get_buff[6]);

    return CMD_OK;

  } else if(ierr==2) {

    sprintf(get_buff,"movr %0.1f",atof(xoff));
    ierr=agwcu("localhost",0,"mmcIC 1 ",get_buff);
    if(ierr!=0) {
      sprintf(reply,"%s %s", who_selected, get_buff);
      return CMD_ERR;
    }

    memset(get_buff,0,sizeof(get_buff));
    sprintf(get_buff,"movr %0.1f",atof(yoff));
    ierr=agwcu("localhost",0,"mmcIC 2 ",get_buff);

    if(ierr!=0) {
      sprintf(reply,"%s %s", who_selected, get_buff);
      return CMD_ERR;
    }

    cmd_gpoffset("", EXEC, reply);

  } else sprintf(reply,"Request <gpoffset %s> is unreasonable. gpoffset [deltaXgp deltaYgp]",args); 

  return CMD_OK;

}

/* ---------------------------------------------------------------------------
//
// gpfocus - AGW Focus MicroLYNX controller.
//
*/

/*!  
  \brief gpfocus command - AGW Probe Focus Position
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  gpfocus [foc] 
  
  foc  - guide probe focus position in millimeters
  home - retract the AGW focus actuator to its home position.\n
  init - init is Focus towards the CCW limit switch 

  \par Description:
  Changes the focus of the guide probe to the specified absolute position. 
  For relative focus changes, use the STEP keyword (e.g., GPFOCUS STEP -5). 
  When the guider focus is changed, the Y stage is back-driven in reflex to
  preserve the absolute guide probe (XGP,YGP) position.

  \par Interactions: Interlocked with the Calibration Tower 
  (hardware & software)

  Send AGW Probe Focus MicroLYNX controller command via network or serial.
  MicroLYNX Controller commands given in MODS Mechanism Commands doc.
  http://www.astronomy.ohio-state.edu/~rgonzale/MODS_Mechanism_Commands.pdf

*/
int
cmd_gpfocus(char *args, MsgType msgtype, char *reply)
{
  int len;
  int device;
  int caldevice;
  int ierr;
  int bit2122;
  int val=0;
  int validNumber;
  float stepMove;
  char who_selected[24];
  char get_buff[PAGE_SIZE];
  char dummy[PAGE_SIZE];
  char cmd_instruction[PAGE_SIZE];

  memset(cmd_instruction,0,sizeof(cmd_instruction));
  memset(get_buff,0,sizeof(get_buff));
  memset(dummy,0,sizeof(dummy));
  validNumber = sscanf(args,"%d", &val);

  strcpy(who_selected,cmdtab[commandID].cmd);
  StrUpper(who_selected);

  device=getMechanismID("agwfoc",dummy);// Get mechanism device ID
  if(device==-1) {
    sprintf(reply,"%s %s",who_selected,dummy);
    return CMD_ERR;
  }

  caldevice=getMechanismID("calib",dummy); // Get mechanism device ID
  if(caldevice==-1) {
    sprintf(reply,"%s %s",who_selected,dummy);
    return CMD_ERR;
  }

  if(!shm_addr->MODS.host[device]) {
    sprintf(reply," %s %s=NOCOMM - NO COMMunication or not connected",who_selected,who_selected);
    return CMD_ERR;
  }

  StrUpper(args);
  GetArg(args,1,cmd_instruction);

  if(!strcasecmp(cmd_instruction,"ABORT")) {
    sprintf(dummy,"%c",27);     // Stop FOCUS axis
    ierr=agwcu("localhost",0,"mmcIC 4 ",dummy);

    sprintf(reply,"%s AGWFS=ABORT operation was aborted",who_selected);

    return CMD_OK;
  }

  if (strlen(args)<=0) { // Query when no command is issued
    strcpy(who_selected,"GPFOCUS");

    memset(dummy,0,sizeof(dummy));

    ierr=agwcu("localhost",0,"getfocus", dummy);

    sprintf(reply,"%s %s", who_selected, &dummy[6]);

    memset(dummy,0,sizeof(dummy));
    strcpy(dummy,"y");

    ierr=agwcu("localhost",0,"getposition ",dummy);

    if(ierr!=0) {
      sprintf(reply,"%s %s", who_selected, dummy);
      return CMD_ERR;
    }

    sprintf(reply,"%s %s", reply, &dummy[6]);

    return CMD_OK;
  }

  StrUpper(args);
  GetArg(args,1,cmd_instruction);

  if (!strcasecmp(cmd_instruction,"GETIP")) { // Mechanism IP
    sprintf(reply,"%s IEB=%d IP '%s' connection is '%s'",who_selected,
	    shm_addr->MODS.ieb_i[device],
  	    shm_addr->MODS.commport[device].Port,
	    (shm_addr->MODS.host[device]==1 ? "OPEN" : "CLOSED"));

    return CMD_OK;

  } else if (!strcasecmp(cmd_instruction,"RDBITS")) {

    ierr=agwcu("localhost",0,"agwfoc -c rdbits",dummy);

    if(ierr!=0) {
      sprintf(reply,"%s", who_selected, &dummy[6]);
      return CMD_ERR;
    }

    sprintf(reply,"%s %s",who_selected, &dummy[6]);

    return CMD_OK;

  } else if(!strncasecmp(args,"M#",2)) { // check for low-level command
    sprintf(dummy,"%s",args);
    len=strlen(dummy);
    ierr=agwcu("localhost",0,"mmcIC 4 ",dummy);

    if(ierr!=0) {
      sprintf(reply,"%s %s", who_selected, dummy);
      return CMD_ERR;
    }
    sprintf(reply,"%s %s", who_selected, &dummy[6]);

    return CMD_OK;
  }

  ierr=agwcu("localhost",0,"mmcIC 4 print io 21",dummy);
  bit2122=atoi(&dummy[6]);
  ierr=agwcu("localhost",0,"mmcIC 4 print io 22",dummy);
  bit2122+=atoi(&dummy[6]);

  if((bit2122)==2) {
    sprintf(reply,"%s AGWFOCUS=FAULT Sensor Fault, both limits asserted",who_selected,who_selected);
    return CMD_ERR;
  }
  
  if(!strcasecmp(cmd_instruction,"RESET")) {

    /* Check the for Calibration Tower obstruction */
    if ( mlcBitsBase10(device,"PRINT IO 22,IO 21",dummy) == 1 ) {
      ierr = agwcu("localhost", 0,"initfoc", dummy); // Home/Init Focus

    } else {

      ierr = agwcu("localhost", 0, "getfocus", dummy);

      sprintf(reply,"%s", &dummy[6]);

      //    memset(dummy,0,sizeof(dummy));
      //    strcpy(dummy,"y");
      //    ierr=agwcu("localhost",0,"getposition ",dummy);

      if(ierr!=0) {
	sprintf(reply,"%s", dummy);
	return CMD_ERR;

      }

    }
    sprintf(reply,"%s %s",reply ,&dummy[6]);

    return CMD_OK;

  } else if(!strcasecmp(cmd_instruction,"STEP")) {
    GetArg(args,2,argbuf); // AGW FOCUS

    memset(get_buff,0,sizeof(get_buff));
    strcpy(get_buff,"focus");
    ierr=agwcu("localhost",0,"getfocus ",get_buff);
    stepMove=atof(&get_buff[12])+atof(argbuf);

    if(stepMove < shm_addr->MODS.min[device] || 
       stepMove > shm_addr->MODS.max[device]) {
      
      sprintf(reply,"%s %s=%0.3f Request 'STEP %0.3f' exceeds allowable range of %d..%d",
	      who_selected,who_selected,
	      fabs(shm_addr->MODS.pos[device]*2.0),
	      atof(argbuf),
	      (int)(shm_addr->MODS.min[device]),
	      (int)(shm_addr->MODS.max[device]));

      return CMD_ERR;
    }

    memset(get_buff,0,sizeof(get_buff));
    sprintf(get_buff,"setfocus %0.3f",fabs(stepMove));
    ierr=agwcu("localhost",0,get_buff,dummy);

    memset(dummy,0,sizeof(dummy));
    ierr=agwcu("localhost",0,"getfocus",dummy);
    sprintf(reply,"%s", &dummy[6]);

    memset(dummy,0,sizeof(dummy));
    strcpy(dummy,"y");
    ierr=agwcu("localhost",0,"getposition ",dummy);

    if(ierr!=0) {
      sprintf(reply,"%s", dummy);
      return CMD_ERR;
    }

    sprintf(reply,"%s %s",reply ,&dummy[6]);

    return CMD_OK;

  } else if ( validNumber == 1 ) {

    GetArg(args, 1, cmd_instruction); // AGW Focus

    /* Check the for Calibration Tower obstruction */
    if ( ierr = mlcBitsBase10(caldevice, "PRINT IO 22,IO 21", dummy) == 2 ) {
      sprintf(reply, "%s Calibration Tower is in the beam, move disallowed[%d][%s]", who_selected, ierr, dummy);
      return CMD_ERR;
      
    }

    memset(get_buff, 0, sizeof(get_buff));
    sprintf(get_buff, "setfocus %s", cmd_instruction);
    ierr = agwcu("localhost", 0, get_buff, dummy);

    if ( ierr != 0 ) {
      sprintf(reply,"%s AGWFOCUS=%0.3f %s", who_selected,
	      atof(&dummy[ierr])*shm_addr->MODS.convf[device],dummy);
      return CMD_ERR;

    }

    memset(dummy, 0, sizeof(dummy));
    ierr = agwcu("localhost", 0, "getfocus", dummy);
    sprintf(reply,"%s", &dummy[6]);

    memset(dummy, 0, sizeof(dummy));
    strcpy(dummy,"y");
    ierr = agwcu("localhost", 0, "getposition ", dummy);

    if ( ierr != 0 ) {
      sprintf(reply, "%s %s", who_selected, dummy);
      return CMD_ERR;
  
    }

    sprintf(reply,"%s %s", reply ,&dummy[6]);

    return CMD_OK;

  } else {
    sprintf(reply,"%s Invalid request '%s', Usage: gpfocus [0..%0.3f]",who_selected,args,shm_addr->MODS.convf[device]);
    return CMD_ERR;
  }

  return CMD_OK;
}

/* ---------------------------------------------------------------------------
//
// colttf - Send Collimator mirror commands to the MicroLYNX controller.
//
*/

/*!  
  \brief colttf command - Set a collimator mirror tip/tilt/focus actuator
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage: cCOLTTFx [pos]

  c    - channel (B or R)\n
  x    - actuator ID, x={A,B,C}\n
  pos  - linear position measured in microns from device home
  
  \par Description:

  Moves the named collimator mirror tip/tilt/focus (TTF) actuator to
  the specified position in units of microns from the device home
  position. Increasing TTF position moves that part of the collimator
  mirror closer to the slit.  There are 3 instances of this mechanism
  per channel.

  If given without arguments, the command returns the current position of 
  that actuator.

  To move an actuator + or - relative to the current position, use the STEP
  keyword.

  To set/query all 3 actuators at once, see the BCOLFOC and RCOLFOC commands.

  \par Interactions: none

  Send Collimator offsets MicroLYNX controller command via network or serial.
  MicroLYNX Controller commands given in MODS Mechanism Commands doc.
  http://www.astronomy.ohio-state.edu/~rgonzale/MODS_Mechanism_Commands.pdf

*/

int
cmd_colttf(char *args, MsgType msgtype, char *reply)
{
  int i;
  int device;
  int argcnt;
  int nval;
  int len;
  int bit2122;
  float ttfval;
  float validMove;
  char dummy[PAGE_SIZE];
  char who_selected[24];
  char cmd_instruction[PAGE_SIZE];

  memset(cmd_instruction,0,sizeof(cmd_instruction));
  strcpy(who_selected,cmdtab[commandID].cmd);
  StrUpper(who_selected);

  GetArg(args,1,cmd_instruction);

  // Was the command given with a single argument or STEP + argument? 

  len=strlen(args);
  if(len) 
    for(i=0,argcnt=1;i<len;i++)
      if(args[i]==' ') argcnt++;

  if (argcnt==1) 
    nval=sscanf(args,"%f",&ttfval); // user passes one argument
  else if (argcnt==2) 
    nval=sscanf(args,"%s %f",cmd_instruction,&ttfval);  // user passes STEP + argument

  StrUpper(cmd_instruction);
  
  // Search for the IP and Socket

  device=getMechanismID(cmdtab[commandID].cmd,dummy);// Get mechanism device ID
  if(device==-1) {
    sprintf(reply,"%s %s",who_selected,dummy); // Error?
    return CMD_ERR;
  }

  if (!strcasecmp(cmd_instruction,"CONFIG")) { // get mechanism ip

    mlcMechanismConfig(device,who_selected,dummy);
    sprintf(reply,"%s",dummy);

    return CMD_OK;
  }

  if(!strcasecmp(cmd_instruction,"ABORT")) { // STOP the mechanism/operation
    shm_addr->MODS.qued[device]=1;
    ierr = mlcStopMechanism(device,dummy);
    sprintf(reply,"%s %s",who_selected,dummy);
    return CMD_OK;
  }

  if (mlcBusy(device,dummy)) { // Busy?
    sprintf(reply,"%s %s",who_selected,dummy);
    return CMD_ERR;
  }

  ierr=mlcQuery(device,1,dummy); // Check if the microLynx is ON
  if(ierr!=0) {
    sprintf(reply,"%s %s",who_selected,dummy);
    return CMD_ERR;
  }

  if(!shm_addr->MODS.host[device]) {
    sprintf(reply," %s %s=NOCOMM - NO COMMunication or not connected",who_selected,who_selected);
    return CMD_ERR;
  }

  if (strncasecmp(args,"M#",2)) { // check for low-level command
    ierr = mlcCheckBits(device,dummy); // check limit bits 21,22
    if((ierr&3)== 3 || ierr==-1) {
      sprintf(reply,"%s %s",who_selected,dummy);
      return CMD_ERR;
    }
  }

  if (strncasecmp(args,"M#",2)) { // check for low-level command
    rawCommand(device,"PRINT IO 22,IO 21",dummy);
    if(atoi(dummy)==11) {
      sprintf(reply,"%s %s=FAULT Both limits asserted, check cable",who_selected,who_selected);
      return CMD_ERR;
    }
  }

  if (!strcasecmp(cmd_instruction,"RDBITS")) {
    rawCommand(device,"PRINT POS",dummy);

    shm_addr->MODS.pos[device] = fabs(atof(dummy));
 
    rawCommand(device,"PRINT IO 22,IO 21",dummy);
    sprintf(reply,"%s %s=%0.3f BITS=%s b22-b21",who_selected,who_selected,
	    shm_addr->MODS.pos[device]*shm_addr->MODS.convf[device],dummy);

    return CMD_OK;

  } else if (!strcasecmp(cmd_instruction,"HOME") ||
	     !strcasecmp(cmd_instruction,"RESET")) {

    rawCommand(device,"INITIAL",dummy);

    rawCommand(device,"PRINT POS",dummy);

    shm_addr->MODS.pos[device] = fabs(atof(dummy));

    sprintf(reply,"%s %s=%0.1f actuator %s completed",
	    who_selected,who_selected,
	    shm_addr->MODS.pos[device]*shm_addr->MODS.convf[device],
	    cmd_instruction);

    return CMD_OK;
  }

  if (strncasecmp(args,"M#",2)) { // check for low-level command
    ierr=checkPower(device,dummy); // check the mechanism power 
    if(ierr!=0) {
      sprintf(reply,"%s %s", who_selected,dummy);
      return CMD_ERR;
    }
  }

  /*
  // shm_addr->MODS.convf[device] is in the shared memory and is the
  // conversion factor from mechanisms.ini in the 8th column that
  // tells us how to convert MLCx units to physical units
  */

  if (strlen(args)<=0) { // no arguments, device query...

    rawCommand(device,"PRINT POS",dummy);
    shm_addr->MODS.pos[device]=fabs(atof(dummy));
    sprintf(reply,"%s %s=%0.1f",who_selected,who_selected,
	    shm_addr->MODS.pos[device]*shm_addr->MODS.convf[device]);

  } else if(!strcasecmp(cmd_instruction,"STEP")) { // STEP keyword 

    validMove=(shm_addr->MODS.pos[device]*shm_addr->MODS.convf[device])+ttfval;
    /* test code */
    if(validMove < shm_addr->MODS.min[device] || 
       validMove > shm_addr->MODS.max[device]) {

      sprintf(reply,"%s %s=%0.1f Request '%0.1f', will exceed actuator range. %d..%d[%0.1f]", who_selected,who_selected,
	      shm_addr->MODS.pos[device]*shm_addr->MODS.convf[device],
	      ttfval,
	      (int)(shm_addr->MODS.min[device]),
	      (int)(shm_addr->MODS.max[device]),validMove);
      
      return CMD_ERR;
    }

    ierr = mlcStep(device,who_selected,LINEAR,-1.0*ttfval,dummy);

    MilliSleep(1000);
    for(int i=0;i<40000;i++) {
      rawCommand(device,"PRINT MVG",dummy);
      if(!strcasecmp(dummy,"FALSE")) break;

      if(shm_addr->MODS.qued[device]==1) break;
      MilliSleep(500);
    }
    shm_addr->MODS.qued[device]=0;


    rawCommand(device,"PRINT POS",dummy);
    shm_addr->MODS.pos[device]=fabs(atof(dummy));

    /*
    // Test to see if we smacked into a limit switch

    if(mlcBitsBase10(device,"PRINT IO 22,IO 21",dummy)==1) {
      sprintf(reply,"%s %s=%0.1f actuator relative move asserted CW limit",
	      who_selected,who_selected,
	      shm_addr->MODS.pos[device]*shm_addr->MODS.convf[device]);
      return CMD_ERR;

    } else if(mlcBitsBase10(device,"PRINT IO 22,IO 21",dummy)==2) {
      sprintf(reply,"%s %s=%0.1f actuator relative move asserted CCW limit",
	      who_selected,who_selected,
	      shm_addr->MODS.pos[device]*shm_addr->MODS.convf[device]);
      return CMD_ERR;

    } else if(mlcBitsBase10(device,"PRINT IO 22,IO 21",dummy)==3) {
      sprintf(reply,"%s %s=%0.1f Sensor Fault, both limits asserted",
	      who_selected,who_selected,
	      shm_addr->MODS.pos[device]*shm_addr->MODS.convf[device]);
      return CMD_ERR;
    }
    */
    sprintf(reply,"%s %s=%0.1f",who_selected,who_selected,
	    shm_addr->MODS.pos[device]*shm_addr->MODS.convf[device]);
 
  } else if (!strcasecmp(cmd_instruction,"?")) {
     sprintf(reply,"Usage: %s [pos] - move collimator TTF actuator %s in microns, range %d..%d", 
	     who_selected, who_selected[strlen(who_selected)-1],
             (int)(shm_addr->MODS.min[device]),
             (int)(shm_addr->MODS.max[device]));
  
  } else if(!strncasecmp(args,"M#",2)) { // check for low-level command
    ierr=mlcTechCmd(device,args,who_selected,reply);
    if(ierr<=-1) return CMD_ERR;
    return CMD_OK;

  } else if (nval) {
    if(ttfval<shm_addr->MODS.min[device] || ttfval>shm_addr->MODS.max[device]) {
      sprintf(reply,"%s Invalid request '%s', Valid %s range %d..%d microns",
	      who_selected,args,who_selected,
	      (int)(shm_addr->MODS.min[device]),
	      (int)(shm_addr->MODS.max[device]));
      return CMD_ERR;
    }

    ttfval=-((ttfval/shm_addr->MODS.convf[device]));
    sprintf(cmd_instruction,"MOVA %f",ttfval); 
    rawCommand(device,cmd_instruction,dummy);

    for(int i=0;i<40000;i++) {
      rawCommand(device,"PRINT MVG",dummy);
      if(!strcasecmp(dummy,"FALSE")) break;

      if(shm_addr->MODS.qued[device]==1) break;
      MilliSleep(500);
    }
    shm_addr->MODS.qued[device]=0;

    rawCommand(device,"PRINT POS",dummy);
    shm_addr->MODS.pos[device]=fabs(atof(dummy));

    MilliSleep(10);
    /*
    // Test to see if we smacked into a limit switch
    */
    
    bit2122=mlcBitsBase10(device,"PRINT IO 22,IO 21",dummy);


    if(bit2122==1) {
      sprintf(reply,"%s %s=%0.1f CW limit asserted",
	      who_selected,who_selected,
	      shm_addr->MODS.pos[device]*shm_addr->MODS.convf[device]);
      return CMD_ERR;

    } else if(bit2122==2) {
      sprintf(reply,"%s %s=%0.1f CCW limit asserted",
	      who_selected,who_selected,
	      shm_addr->MODS.pos[device]*shm_addr->MODS.convf[device]);
      return CMD_ERR;

    } else if(bit2122==3) {
      sprintf(reply,"%s %s=%0.1f Sensor Fault, both limits asserted",
	      who_selected,who_selected,
	      shm_addr->MODS.pos[device]*shm_addr->MODS.convf[device]);
      return CMD_ERR;

    }
    
    sprintf(reply,"%s %s=%0.1f",who_selected,who_selected,
	    shm_addr->MODS.pos[device]*shm_addr->MODS.convf[device]);

  } else {
    sprintf(reply,"%s Invalid request '%s', Usage: %s [pos|home]",
	    who_selected,args,who_selected);
    return CMD_ERR;

  }

  return CMD_OK;
}

/* ---------------------------------------------------------------------------
//
// colfoc - Send Collimator focus commands to the MicroLYNX controller.
//
*/

/*!  
  \brief colttf command - Set the collimator mirror absolute focus position
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage: cCOLFOC [step] [focval|ttfA ttfB ttfC]

  c    - channel (B or R)\n
  foc  - focus position (piston) of the mirror in microns from device home\n
  ttf# - positions for each of the 3 TTF actuators (#={A.B.C})
  
  \par Description:
       Pistons the named collimator mirror by moving all 3 tip/tilt/focus (TTF)
       actuators by the same amount.  Increasing focus position moves the 
       collimator mirror closer to the slit. 
 
       Alternatively, the command can be used to drive the mirror back to its 
       mechanical home position (hence away from the slit), or to move each 
       TTF actuator by a different amount.

       The focus motion specified by the single "foc" value maintains the 
       relative positions of the 3 actuators, and is defined as the mean of 
       the current actuator positions:  foc = (ttfA+ttfB+ttfC)/3

       This ensures that as the focus is changed, the relative alignment of 
       the collimator mirror with the optical axis will stay unchanged.

       An alternative is to give 3 numerical arguments for the
       collimator TTF actuators ttfA, ttfB, and ttfC to set each TTF
       actuator separately.  All 3 must be given.  This has the effect
       of changing the focus (piston) and relative aspects (tip/tilt)
       simultaneously.

       When homing the collimator mirror with the home keyword, all 3 TTF 
       actuators are driven to their mechanical home positions regardless of 
       their relative aspects before homing was requested.

       If the requested focus motion would drive any of the actuators out of 
       range, an error is reported and the motion request denied.

       If given without arguments, the command returns the current focus 
       position, as well as the corresponding positions of the individual TTF 
       actuators.

       To set/query an individual actuator independently while leaving the 
       others unchanged, see the cCOLTTFn commands.

       To make incremental +/- changes to the collimator mirror focus, 
       use the STEP keyword

  \par Interactions: none

  Send Collimator focus MicroLYNX controller command via network or serial.
  MicroLYNX Controller commands given in MODS Mechanism Commands doc.
  http://www.astronomy.ohio-state.edu/~rgonzale/MODS_Mechanism_Commands.pdf

*/
int
cmd_colfoc(char *args, MsgType msgtype, char *reply)
{
  int i;
  int len;
  int break_cnt;
  int nval;
  int ttfA;
  int ttfB;
  int ttfC;
  int abortall;
  int argcnt;
  int posValid;
  float ttfvals[3];
  float validMoveTTFA;
  float validMoveTTFB;
  float validMoveTTFC;
  float colFocus;  // Global collimator focus "piston" position,
                   // colFocus=(ttfA+ttfB+ttfC)/3
  float umPerRev;  // microns per revolution conversion factor
  float minFoc;    // minimum focus value for ttfA..ttfC
  float maxFoc;    // maximum focus value for ttfA..ttfC
  float posA;    // maximum focus value for ttfA..ttfC
  float posB;    // maximum focus value for ttfA..ttfC
  float posC;    // maximum focus value for ttfA..ttfC
  char dummy[PAGE_SIZE];
  char temp[PAGE_SIZE];
  char colID[24];
  char ABCmsg[80];
  char ABCval[40];
  char who_selected[24];
  char cmd_instruction[PAGE_SIZE]; // MLC command

  memset(cmd_instruction,0,sizeof(cmd_instruction));
  memset(dummy,0,sizeof(dummy));
  memset(who_selected,0,sizeof(who_selected));

  strcpy(who_selected,cmdtab[commandID].cmd);
  StrUpper(who_selected);

  GetArg(args,1,cmd_instruction);

  /*
  // Command may be given with (a) a single argument (home, reset, focus, etc.)
  // Triplet of TTF actuator positions: STEP + focus, or STEP + a TTF triplet.
  */
  len=strlen(args);
  if(len>0)
    for(i=0,argcnt=1;i<len;i++)
      if(args[i]==' ') argcnt++;

  if(argcnt==1) nval=sscanf(args,"%f",&ttfvals[0]);
  else if(argcnt==3) nval=sscanf(args,"%f %f %f",
				 &ttfvals[0],&ttfvals[1],&ttfvals[2]);
  else if(argcnt==2) nval=sscanf(args,"%s %f", cmd_instruction, &ttfvals[0]); 
  else if(argcnt==4) nval=sscanf(args,"%s %f %f %f",cmd_instruction, 
				 &ttfvals[0],&ttfvals[1],&ttfvals[2]);

  StrUpper(cmd_instruction);

  /*
  // Search for the IP and Socket addresses for the collimator TTF Actuators
  */
  if(who_selected[0]=='R') {
    ttfA=getMechanismID("rcolttfa",dummy);// Get mechanism device ID
    if(ttfA==-1) {
      sprintf(reply,"%s %s",who_selected,dummy);
      return CMD_ERR;

    }

    ttfB=getMechanismID("rcolttfb",dummy);// Get mechanism device ID
    if(ttfB==-1) {
      sprintf(reply,"%s %s",who_selected,dummy);
      return CMD_ERR;
    }

    ttfC=getMechanismID("rcolttfc",dummy);// Get mechanism device ID
    if(ttfC==-1) {
      sprintf(reply,"%s %s",who_selected,dummy);
      return CMD_ERR;
    }

  } else if(who_selected[0]=='B') {

    ttfA=getMechanismID("bcolttfa",dummy);// Get mechanism device ID
    if(ttfA==-1) {
      sprintf(reply,"%s %s",who_selected,dummy);
      return CMD_ERR;
    }

    ttfB=getMechanismID("bcolttfb",dummy);// Get mechanism device ID
    if(ttfB==-1) {
      sprintf(reply,"%s %s",who_selected,dummy);
      return CMD_ERR;
    }

    ttfC=getMechanismID("bcolttfc",dummy);// Get mechanism device ID
    if(ttfC==-1) {
      sprintf(reply,"%s %s",who_selected,dummy);
      return CMD_ERR;
    }

  } else {
    sprintf(reply,"%s Invalid request '%s', Usage: %s [pos|home]",who_selected,args,who_selected);
    return CMD_ERR;
  }

  if(!strcasecmp(cmd_instruction,"ABORT")) {
    
    shm_addr->MODS.qued[ttfA]=1;
    ierr = mlcStopMechanism(ttfA,dummy); //  STOP ttfa operation
    ierr = mlcStopMechanism(ttfB,dummy); //  STOP ttfb operation
    ierr = mlcStopMechanism(ttfC,dummy); //  STOP ttfc operation
    
    sprintf(reply,"%s BCOLTTFA=ABORT BCOLTTFB=ABORT BCOLTTFC=ABORT ",
	    who_selected);
    
    return CMD_OK;
  }
  umPerRev = shm_addr->MODS.convf[ttfA]; 

  if(shm_addr->MODS.busy[ttfA]==1 ||
     shm_addr->MODS.busy[ttfB]==1 || 
     shm_addr->MODS.busy[ttfC]==1) {

    colFocus = umPerRev*(shm_addr->MODS.pos[ttfA]+shm_addr->MODS.pos[ttfB]+shm_addr->MODS.pos[ttfC])/3.0;

    sprintf(reply,"%s %cCOLFOC=%0.1f %cCOLTTFA=%0.1f %cCOLTTFB=%0.1f %cCOLTTFC=%0.1f One of more mechanism(s) are busy",
	    who_selected,
	    who_selected[0],
	    colFocus,  // Total focus "piston" value = A+B+C/3
	    who_selected[0],
	    shm_addr->MODS.pos[ttfA]*shm_addr->MODS.convf[ttfA], // TTFA
	    who_selected[0],
	    shm_addr->MODS.pos[ttfB]*shm_addr->MODS.convf[ttfB], // TTFB
	    who_selected[0],
	    shm_addr->MODS.pos[ttfC]*shm_addr->MODS.convf[ttfC]); // TTFC
    return CMD_OK;
  }

  /* 
  // All three(3) collimators need to be on line for xCOLFOC command
  */
  ierr=mlcQuery(ttfA,1,dummy); // Check if the TTFA microLynx is ON
  if(ierr!=0) {
    sprintf(reply,"%s %s All three collimator mechanisms need to be online",who_selected,dummy);
    return CMD_ERR;
  }
  ierr=mlcQuery(ttfB,1,dummy); // Check if the TTFB microLynx is ON
  if(ierr!=0) {
    sprintf(reply,"%s %s All three collimator mechanisms need to be online",who_selected,dummy);
    return CMD_ERR;
  }
  ierr=mlcQuery(ttfC,1,dummy); // Check if the TTFC microLynx is ON
  if(ierr!=0) {
    sprintf(reply,"%s %s All three collimator mechanisms need to be online",who_selected,dummy);
    return CMD_ERR;
  }

  if(!strcasecmp(cmd_instruction,"RESET")) {

    rawCommand(ttfB,"PRINT IO 21",dummy);
    if(!atoi(dummy)) ierr=rawCommand(ttfB,"MOVA 1000",dummy);
    MilliSleep(100);

    rawCommand(ttfC,"PRINT IO 21",dummy);
    if(!atoi(dummy)) ierr=rawCommand(ttfC,"MOVA 1000",dummy);

    MilliSleep(1000); // Give TTFB and TTFC enough of a headstart.
    rawCommand(ttfA,"PRINT IO 21",dummy);
    if(!atoi(dummy)) ierr=rawCommand(ttfA,"MOVA 1000",dummy);

    for(i=0,break_cnt=0;i<40000;i++) {
      if(break_cnt==0) {
	rawCommand(ttfB,"PRINT MVG",dummy);
	if(!strcasecmp(dummy,"FALSE")) break_cnt++;
      } else if(break_cnt==1) {
	rawCommand(ttfC,"PRINT MVG",dummy);
	if(!strcasecmp(dummy,"FALSE")) break_cnt++;
      } else if(break_cnt==2) {
	rawCommand(ttfA,"PRINT MVG",dummy);
	if(!strcasecmp(dummy,"FALSE")) break_cnt++;
      } else if(break_cnt==3) break;

      if(shm_addr->MODS.qued[ttfA]==1) break;
      MilliSleep(500);
    }

    if(shm_addr->MODS.qued[ttfA]==0) {
      rawCommand(ttfB,"INITIAL",dummy);
      MilliSleep(100);
      rawCommand(ttfC,"INITIAL",dummy);
      MilliSleep(100);
      rawCommand(ttfA,"INITIAL",dummy);
    }

    MilliSleep(100);
    rawCommand(ttfA,"PRINT POS",dummy);
    posValid = sscanf(dummy,"%f",&posA);
    if(posValid!=0) shm_addr->MODS.pos[ttfA]=fabs(posA);

    rawCommand(ttfB,"PRINT POS",dummy);
    posValid = sscanf(dummy,"%f",&posB);
    if(posValid!=0) shm_addr->MODS.pos[ttfB]=fabs(posB);
    
    rawCommand(ttfC,"PRINT POS",dummy);
    posValid = sscanf(dummy,"%f",&posC);
    if(posValid!=0) shm_addr->MODS.pos[ttfC]=fabs(posC);

    colFocus = umPerRev*(shm_addr->MODS.pos[ttfA]+shm_addr->MODS.pos[ttfB]+shm_addr->MODS.pos[ttfC])/3.0;
    
    if(shm_addr->MODS.qued[ttfA]==1) {
      sprintf(reply,"%s %cCOLFOC=%0.1f %cCOLTTFA=%0.1f %cCOLTTFB=%0.1f %cCOLTTFC=%0.1f %s reset did not complete",
	      who_selected,
	      who_selected[0],
	      colFocus,  // Total focus "piston" value = A+B+C/3
	      who_selected[0],
	      shm_addr->MODS.pos[ttfA]*shm_addr->MODS.convf[ttfA], // TTFA
	      who_selected[0],
	      shm_addr->MODS.pos[ttfB]*shm_addr->MODS.convf[ttfB], // TTFB
	      who_selected[0],
	      shm_addr->MODS.pos[ttfC]*shm_addr->MODS.convf[ttfC], // TTFC
	      cmd_instruction); 
    } else {
      sprintf(reply,"%s %cCOLFOC=%0.1f %cCOLTTFA=%0.1f %cCOLTTFB=%0.1f %cCOLTTFC=%0.1f %s request completed",
	      who_selected,
	      who_selected[0],
	      colFocus,  // Total focus "piston" value = A+B+C/3
	      who_selected[0],
	      shm_addr->MODS.pos[ttfA]*shm_addr->MODS.convf[ttfA], // TTFA
	      who_selected[0],
	      shm_addr->MODS.pos[ttfB]*shm_addr->MODS.convf[ttfB], // TTFB
	      who_selected[0],
	      shm_addr->MODS.pos[ttfC]*shm_addr->MODS.convf[ttfC], // TTFC
	      cmd_instruction); 
    }

    shm_addr->MODS.qued[ttfA]=0;
    
    return CMD_OK;
  }

  /*
  // check Power for all 3 mechanisms
  */
  ierr=checkPower(ttfA,dummy);
  if(ierr) {
    sprintf(reply,"%s %s", who_selected,dummy);
    return CMD_ERR;
  }
    
  ierr=checkPower(ttfB,dummy);
  if(ierr) {
    sprintf(reply,"%s %s", who_selected,dummy);
    return CMD_ERR;
  }
  
  ierr=checkPower(ttfC,dummy);
  if(ierr) {
    sprintf(reply,"%s %s", who_selected,dummy);
    return CMD_ERR;
  }

  /*
  // check bits 21,22 for cable melfunction for all 3 mechanisms
  */
  if(mlcBitsBase10(ttfA,"PRINT IO 22,IO 21",dummy)==3) { 
    sprintf(reply,"%s %s",who_selected,dummy);
    return CMD_ERR;
  }
  
  if(mlcBitsBase10(ttfB,"PRINT IO 22,IO 21",dummy)==3) {
    sprintf(reply,"%s %s",who_selected,dummy);
    return CMD_ERR;
  }
  
  if(mlcBitsBase10(ttfC,"PRINT IO 22,IO 21",dummy)==3) {
    sprintf(reply,"%s %s",who_selected,dummy);
    return CMD_ERR;
  }

  /*
  // Conversion factor for all moves.  We adopt the value for TTFA as
  // all of the actuators are built with identical stepper motors,
  // reduction gears, and lead screw pitches.
  */
  umPerRev = shm_addr->MODS.convf[ttfA]; 

  /*
  // Minimum and Maximum range of travel.  We adopt the largest/smallest of
  // the min/max ranges for each of the 3 actuators for safety.
  */  
  minFoc = shm_addr->MODS.min[ttfA];
  if (shm_addr->MODS.min[ttfB]>minFoc) minFoc = shm_addr->MODS.min[ttfB];
  if (shm_addr->MODS.min[ttfC]>minFoc) minFoc = shm_addr->MODS.min[ttfC];

  maxFoc = shm_addr->MODS.max[ttfA];
  if (shm_addr->MODS.max[ttfB]<maxFoc) maxFoc = shm_addr->MODS.max[ttfB];
  if (shm_addr->MODS.max[ttfC]<maxFoc) maxFoc = shm_addr->MODS.max[ttfC];

  /*
  // Process the command arguments.
  */
  if(strlen(args)<=0) {  // position query

    rawCommand(ttfA,"PRINT POS",dummy);
    posValid = sscanf(dummy,"%f",&posA);
    if(posValid!=0) shm_addr->MODS.pos[ttfA]=fabs(posA);
      
    rawCommand(ttfB,"PRINT POS",dummy);
    posValid = sscanf(dummy,"%f",&posB);
    if(posValid!=0) shm_addr->MODS.pos[ttfB]=fabs(posB);
    
    rawCommand(ttfC,"PRINT POS",dummy);
    posValid = sscanf(dummy,"%f",&posC);
    if(posValid!=0) shm_addr->MODS.pos[ttfC]=fabs(posC);

    colFocus = umPerRev*(shm_addr->MODS.pos[ttfA]+shm_addr->MODS.pos[ttfB]+shm_addr->MODS.pos[ttfC])/3.0;

    sprintf(reply,"%s %cCOLFOC=%0.1f %cCOLTTFA=%0.1f %cCOLTTFB=%0.1f %cCOLTTFC=%0.1f",
	    who_selected,
	    who_selected[0],
	    colFocus,  // Total focus "piston" value = A+B+C/3
	    who_selected[0],
	    shm_addr->MODS.pos[ttfA]*shm_addr->MODS.convf[ttfA],  // TTFA
	    who_selected[0],
	    shm_addr->MODS.pos[ttfB]*shm_addr->MODS.convf[ttfB],  // TTFB
	    who_selected[0],
	    shm_addr->MODS.pos[ttfC]*shm_addr->MODS.convf[ttfC]); // TTFC
    return CMD_OK;
  }

  if (!strcasecmp(cmd_instruction,"RDBITS")) {

    sprintf(reply,"%s %s not available with %s",
	    who_selected,cmd_instruction,who_selected);

    return CMD_OK;

  } else if(!strcasecmp(cmd_instruction,"STEP")) {

    if (argcnt==2) {
      validMoveTTFA=(shm_addr->MODS.pos[ttfA]*shm_addr->MODS.convf[ttfA])+ttfvals[0];
      validMoveTTFB=(shm_addr->MODS.pos[ttfB]*shm_addr->MODS.convf[ttfB])+ttfvals[0];
      validMoveTTFC=(shm_addr->MODS.pos[ttfC]*shm_addr->MODS.convf[ttfC])+ttfvals[0];

      /* 
      // test for valid TTFA Step
      */
      if(validMoveTTFA < shm_addr->MODS.min[ttfA] || 
	 validMoveTTFA > shm_addr->MODS.max[ttfA]) {
      
	sprintf(reply,"%s %cCOLTTFA=%0.1f Request '%0.1f', will exceed actuator range. %d..%d[%0.1f]0", who_selected,who_selected[0],
		shm_addr->MODS.pos[ttfA]*shm_addr->MODS.convf[ttfA],
		ttfvals[0],
		(int)(shm_addr->MODS.min[ttfA]),
		(int)(shm_addr->MODS.max[ttfA]),validMoveTTFA);
	
	return CMD_ERR;
      }

      /*
      // test for valid TTFB Step
      */
      if(validMoveTTFB < shm_addr->MODS.min[ttfB] ||
	 validMoveTTFB > shm_addr->MODS.max[ttfB]) {
	
	sprintf(reply,"%s %cCOLTTFB=%0.1f Request '%0.1f', will exceed actuator range. %d..%d", who_selected,who_selected[0],
		shm_addr->MODS.pos[ttfB]*shm_addr->MODS.convf[ttfB],
		ttfvals[0],
		(int)(shm_addr->MODS.min[ttfB]),
		(int)(shm_addr->MODS.max[ttfB]));
	
	return CMD_ERR;
      }
      
      /* 
      // test for valid TTFC Step 
      */
      if(validMoveTTFC < shm_addr->MODS.min[ttfC] || 
	 validMoveTTFC > shm_addr->MODS.max[ttfC]) {
	
	sprintf(reply,"%s %cCOLTTFC=%0.1f Request '%0.1f', will exceed actuator range. %d..%d", who_selected,who_selected[0],
		shm_addr->MODS.pos[ttfC]*shm_addr->MODS.convf[ttfC],
		ttfvals[0],
		(int)(shm_addr->MODS.min[ttfC]),
		(int)(shm_addr->MODS.max[ttfC]));
	
	return CMD_ERR;
      }

      ierr = mlcStep(ttfB,who_selected,LINEAR,-1.0*ttfvals[0],dummy);
      ierr = mlcStep(ttfC,who_selected,LINEAR,-1.0*ttfvals[0],dummy);
      ierr = mlcStep(ttfA,who_selected,LINEAR,-1.0*ttfvals[0],dummy);

    } else if (argcnt==4) {    

      validMoveTTFA=(shm_addr->MODS.pos[ttfA]*shm_addr->MODS.convf[ttfA])+ttfvals[0];
      validMoveTTFB=(shm_addr->MODS.pos[ttfB]*shm_addr->MODS.convf[ttfB])+ttfvals[1];
      validMoveTTFC=(shm_addr->MODS.pos[ttfC]*shm_addr->MODS.convf[ttfC])+ttfvals[2];

      /* 
      // test for valid TTFA Step 
      */
      if(validMoveTTFA < shm_addr->MODS.min[ttfA] || 
	 validMoveTTFA > shm_addr->MODS.max[ttfA]) {
      
	sprintf(reply,"%s %cCOLTTFA=%0.1f Request '%0.1f', will exceed actuator range. %d..%d[%0.1f]", who_selected,who_selected[0],
		shm_addr->MODS.pos[ttfA]*shm_addr->MODS.convf[ttfA],
		ttfvals[0],
		(int)(shm_addr->MODS.min[ttfA]),
		(int)(shm_addr->MODS.max[ttfA]),validMoveTTFA);
	
	return CMD_ERR;
      }
      
      /* 
      // test for valid TTFB Step
      */
      if(validMoveTTFB < shm_addr->MODS.min[ttfB] || 
	 validMoveTTFB > shm_addr->MODS.max[ttfB]) {
	
	sprintf(reply,"%s %cCOLTTFB=%0.1f Request '%0.1f', will exceed actuator range. %d..%d", who_selected,who_selected[0],
		shm_addr->MODS.pos[ttfB]*shm_addr->MODS.convf[ttfB],
		ttfvals[1],
		(int)(shm_addr->MODS.min[ttfB]),
		(int)(shm_addr->MODS.max[ttfB]));
	
	return CMD_ERR;
      }
      
      /* 
      // test for valid TTFC Step
      */
      if(validMoveTTFC < shm_addr->MODS.min[ttfC] || 
	 validMoveTTFC > shm_addr->MODS.max[ttfC]) {
	
	sprintf(reply,"%s %cCOLTTFC=%0.1f Request '%0.1f', will exceed actuator range. %d..%d", who_selected,who_selected[0],
		shm_addr->MODS.pos[ttfC]*shm_addr->MODS.convf[ttfC],
		ttfvals[2],
		(int)(shm_addr->MODS.min[ttfC]),
		(int)(shm_addr->MODS.max[ttfC]));
	
	return CMD_ERR;
      }
      
      ierr = mlcStep(ttfB,who_selected,LINEAR,-1.0*ttfvals[1],dummy);
      ierr = mlcStep(ttfC,who_selected,LINEAR,-1.0*ttfvals[2],dummy);
      ierr = mlcStep(ttfA,who_selected,LINEAR,-1.0*ttfvals[0],dummy);
    }

    for(i=0,break_cnt=0;i<40000;i++) {
      if(break_cnt==0) {
	rawCommand(ttfB,"PRINT MVG",dummy);
	if(!strcasecmp(dummy,"FALSE")) break_cnt++;
      } else if(break_cnt==1) {
	rawCommand(ttfC,"PRINT MVG",dummy);
	if(!strcasecmp(dummy,"FALSE")) break_cnt++;
      } else if(break_cnt==2) {
	rawCommand(ttfA,"PRINT MVG",dummy);
	if(!strcasecmp(dummy,"FALSE")) break_cnt++;
      } else if(break_cnt==3) break;
      
      if(shm_addr->MODS.qued[ttfA]==1) break;
      MilliSleep(10);
    }

    shm_addr->MODS.qued[ttfA]=0;
    
    MilliSleep(100);

    rawCommand(ttfA,"PRINT POS",dummy);
    posValid = sscanf(dummy,"%f",&posA);
    if(posValid!=0) shm_addr->MODS.pos[ttfA]=fabs(posA);
    
    rawCommand(ttfB,"PRINT POS",dummy);
    posValid = sscanf(dummy,"%f",&posB);
    if(posValid!=0) shm_addr->MODS.pos[ttfB]=fabs(posB);
    
    rawCommand(ttfC,"PRINT POS",dummy);
    posValid = sscanf(dummy,"%f",&posC);
    if(posValid!=0) shm_addr->MODS.pos[ttfC]=fabs(posC);
    
    colFocus = umPerRev*(shm_addr->MODS.pos[ttfA]+shm_addr->MODS.pos[ttfB]+shm_addr->MODS.pos[ttfC])/3.0;
    
    sprintf(reply,"%s %cCOLFOC=%0.1f %cCOLTTFA=%0.1f %cCOLTTFB=%0.1f %cCOLTTFC=%0.1f",
	    who_selected,
	    who_selected[0],
	    colFocus,  // Total focus "piston" value = A+B+C/3
	    who_selected[0],
	    shm_addr->MODS.pos[ttfA]*shm_addr->MODS.convf[ttfA],  // TTFA
	    who_selected[0],
	    shm_addr->MODS.pos[ttfB]*shm_addr->MODS.convf[ttfB],  // TTFB
	    who_selected[0],
	    shm_addr->MODS.pos[ttfC]*shm_addr->MODS.convf[ttfC]); // TTFC
    
    return CMD_OK;
    
  } else if (!strcasecmp(cmd_instruction,"?")) {
     sprintf(reply,"Usage: %s [pos] - move collimator focus %s in microns, range %d..%d", 
	     who_selected, who_selected,
	     (int)(minFoc),(int)(maxFoc));
     return CMD_OK;
  }
  
  if (!strcasecmp(cmd_instruction,"GETIP")) { // Mechanism IP
    sprintf(reply,"%s IEB=%d GETIP='ttfA %s ttfB %s ttfC %s'",who_selected,
	    shm_addr->MODS.ieb_i[ttfA],
  	    shm_addr->MODS.commport[ttfA].Port,
  	    shm_addr->MODS.commport[ttfB].Port,
  	    shm_addr->MODS.commport[ttfC].Port);
    return CMD_OK;

  } else if(!strcasecmp(cmd_instruction,"HOME")) {

    rawCommand(ttfB,"PRINT IO 21",dummy);
    if(!atoi(dummy)) ierr=rawCommand(ttfB,"MOVA 0",dummy);
    MilliSleep(100);

    rawCommand(ttfC,"PRINT IO 21",dummy);
    if(!atoi(dummy)) ierr=rawCommand(ttfC,"MOVA 0",dummy);

    MilliSleep(1000); // Give TTFB and TTFC enough of a headstart.
    rawCommand(ttfA,"PRINT IO 21",dummy);
    if(!atoi(dummy)) ierr=rawCommand(ttfA,"MOVA 0",dummy);

    for(i=0,break_cnt=0;i<40000;i++) {
      if(break_cnt==0) {
	rawCommand(ttfB,"PRINT MVG",dummy);
	if(!strcasecmp(dummy,"FALSE")) break_cnt++;
      } else if(break_cnt==1) {
	rawCommand(ttfC,"PRINT MVG",dummy);
	if(!strcasecmp(dummy,"FALSE")) break_cnt++;
      } else if(break_cnt==2) {
	rawCommand(ttfA,"PRINT MVG",dummy);
	if(!strcasecmp(dummy,"FALSE")) break_cnt++;
      } else if(break_cnt==3) break;
      
      if(shm_addr->MODS.qued[ttfA]==1) break;
      MilliSleep(100);

    }
    shm_addr->MODS.qued[ttfA]=0;

    MilliSleep(100);
    rawCommand(ttfA,"PRINT POS",dummy);
    posValid = sscanf(dummy,"%f",&posA);
    if(posValid!=0) shm_addr->MODS.pos[ttfA]=fabs(posA);

    rawCommand(ttfB,"PRINT POS",dummy);
    posValid = sscanf(dummy,"%f",&posB);
    if(posValid!=0) shm_addr->MODS.pos[ttfB]=fabs(posB);
    
    rawCommand(ttfC,"PRINT POS",dummy);
    posValid = sscanf(dummy,"%f",&posC);
    if(posValid!=0) shm_addr->MODS.pos[ttfC]=fabs(posC);

    colFocus = umPerRev*(shm_addr->MODS.pos[ttfA]+shm_addr->MODS.pos[ttfB]+shm_addr->MODS.pos[ttfC])/3.0;

    sprintf(reply,"%s %cCOLFOC=%0.1f %cCOLTTFA=%0.1f %cCOLTTFB=%0.1f %cCOLTTFC=%0.1f %s request completed",
	    who_selected,
	    who_selected[0],
	    colFocus,  // Total focus "piston" value = A+B+C/3
	    who_selected[0],
	    shm_addr->MODS.pos[ttfA]*shm_addr->MODS.convf[ttfA], // TTFA
	    who_selected[0],
	    shm_addr->MODS.pos[ttfB]*shm_addr->MODS.convf[ttfB], // TTFB
	    who_selected[0],
	    shm_addr->MODS.pos[ttfC]*shm_addr->MODS.convf[ttfC], // TTFC
	    cmd_instruction); 
    
    return CMD_OK;
  }

  if(ttfA==-1 || ttfB==-1  || ttfC==-1) { 
    sprintf(reply,"%s",who_selected);
    if(ttfA==-1) sprintf(reply,"%s %cCOLTTFA=NOTAVAIL",reply,who_selected[0]);
    if(ttfB==-1) sprintf(reply,"%s %cCOLTTFB=NOTAVAIL",reply,who_selected[0]);
    if(ttfC==-1) sprintf(reply,"%s %cCOLTTFC=NOTAVAIL",reply,who_selected[0]);
    return CMD_ERR; 
  }

  if(!shm_addr->MODS.host[ttfA] || 
     !shm_addr->MODS.host[ttfB] || 
     !shm_addr->MODS.host[ttfC]) { 
    sprintf(reply,"%s",who_selected);
    if(!shm_addr->MODS.host[ttfA]) sprintf(reply,"%s %cCOLTTFA=NOCOMM",reply,who_selected[0]);
    if(!shm_addr->MODS.host[ttfB]) sprintf(reply,"%s %cCOLTTFB=NOCOMM",reply,who_selected[0]);
    if(!shm_addr->MODS.host[ttfC]) sprintf(reply,"%s %cCOLTTFC=NOCOMM",reply,who_selected[0]);
    strcat(reply," NO COMMunication or not connected");
    return CMD_ERR;
  }

  if(nval==1) {
    if(ttfvals[0]<minFoc || ttfvals[0]>maxFoc) {
      sprintf(reply,"%s Invalid request '%s', Valid %s focus range is %d..%d microns",
	      who_selected,args,who_selected,(int)(minFoc),(int)(maxFoc));
      return CMD_ERR;
    }

    ttfvals[0]=-((ttfvals[0]/shm_addr->MODS.convf[ttfA]));
    sprintf(ABCval,"MOVA %f",ttfvals[0]);
    ierr=rawCommand(ttfB,ABCval,dummy);
    MilliSleep(100);
    if(ierr==-1) sprintf(ABCmsg,"%cCOLTTFB=NOCOMM",who_selected[0]);
    MilliSleep(100);
    ierr=rawCommand(ttfC,ABCval,dummy);
    if(ierr==-1) sprintf(ABCmsg,"%s %cCOLTTFC=NOCOMM",ABCmsg,who_selected[0]);
    MilliSleep(100);
    ierr=rawCommand(ttfA,ABCval,dummy);
    if(ierr==-1) sprintf(ABCmsg,"%s %cCOLTTFA=NOCOMM",ABCmsg,who_selected[0]);
      
    if(ierr<0) {
      sprintf(reply,"%s %s",who_selected,ABCmsg);
      return CMD_ERR;
    }
    
    for(i=0,break_cnt=0;i<40000;i++) {
      if(break_cnt==0) {
	rawCommand(ttfB,"PRINT MVG",dummy);
	if(!strcasecmp(dummy,"FALSE")) break_cnt++;
      } else if(break_cnt==1) {
	rawCommand(ttfC,"PRINT MVG",dummy);
	if(!strcasecmp(dummy,"FALSE")) break_cnt++;
      } else if(break_cnt==2) {
	rawCommand(ttfA,"PRINT MVG",dummy);
	if(!strcasecmp(dummy,"FALSE")) break_cnt++;
      } else if(break_cnt==3) break;

      if(shm_addr->MODS.qued[ttfA]==1) break;
      MilliSleep(100);
    }
    shm_addr->MODS.qued[ttfA]=0;
    
    MilliSleep(100);
    rawCommand(ttfA,"PRINT POS",dummy);
    posValid = sscanf(dummy,"%f",&posA);
    if(posValid!=0) shm_addr->MODS.pos[ttfA]=fabs(posA);

    rawCommand(ttfB,"PRINT POS",dummy);
    posValid = sscanf(dummy,"%f",&posB);
    if(posValid!=0) shm_addr->MODS.pos[ttfB]=fabs(posB);
    
    rawCommand(ttfC,"PRINT POS",dummy);
    posValid = sscanf(dummy,"%f",&posC);
    if(posValid!=0) shm_addr->MODS.pos[ttfC]=fabs(posC);

    colFocus = umPerRev*(shm_addr->MODS.pos[ttfA]+shm_addr->MODS.pos[ttfB]+shm_addr->MODS.pos[ttfC])/3.0;

    sprintf(reply,"%s %cCOLFOC=%0.1f %cCOLTTFA=%0.1f %cCOLTTFB=%0.1f %cCOLTTFC=%0.1f",
	    who_selected,
	    who_selected[0],
	    colFocus,  // Total focus "piston" value = A+B+C/3
	    who_selected[0],
	    shm_addr->MODS.pos[ttfA]*shm_addr->MODS.convf[ttfA], // TTFA
	    who_selected[0],
	    shm_addr->MODS.pos[ttfB]*shm_addr->MODS.convf[ttfB], // TTFB
	    who_selected[0],
	    shm_addr->MODS.pos[ttfC]*shm_addr->MODS.convf[ttfC]); // TTFC

    return CMD_OK;

  } else if(nval==3 || !strcasecmp(cmd_instruction,"NOMINAL")) {

    /* 
    // Here to get nominal values from Shared Memory placed 
    // by the mechanisms.ini config file    	    
    */
    if(!strcasecmp(cmd_instruction,"NOMINAL")) {
      if(who_selected[0]=='R') {
	ttfvals[0]=shm_addr->MODS.r_imcsNominal[0];
	ttfvals[1]=shm_addr->MODS.r_imcsNominal[1];
	ttfvals[2]=shm_addr->MODS.r_imcsNominal[2];
      } else if(who_selected[0]=='B') {
	ttfvals[0]=shm_addr->MODS.b_imcsNominal[0];
	ttfvals[1]=shm_addr->MODS.b_imcsNominal[1];
	ttfvals[2]=shm_addr->MODS.b_imcsNominal[2];
      }
    }

    if(ttfvals[0]<minFoc || ttfvals[1]<minFoc || ttfvals[2]<minFoc) {
      sprintf(reply,"%s Invalid request '%s', Valid Range %d..%d microns", who_selected,args,
	      (int)(minFoc),(int)(maxFoc));
      return CMD_ERR;
    }
    if(ttfvals[0]>maxFoc || ttfvals[1]>maxFoc || ttfvals[2]>maxFoc) {
      sprintf(reply,"%s Invalid request '%s', Valid Range %d..%d microns", who_selected,args,
	      (int)(minFoc),(int)(maxFoc));
      return CMD_ERR;
    }

    ttfvals[1]=-((ttfvals[1]/shm_addr->MODS.convf[ttfB])); // TTFB
    sprintf(ABCval,"MOVA %f",ttfvals[1]);
    ierr=rawCommand(ttfB,ABCval,dummy);
    MilliSleep(100);
    if(ierr==-1) sprintf(ABCmsg,"%cCOLTTFB=NOCOMM",who_selected[0]);

    ttfvals[2]=-((ttfvals[2]/shm_addr->MODS.convf[ttfC])); // TTFC
    sprintf(ABCval,"MOVA %f",ttfvals[2]);
    ierr=rawCommand(ttfC,ABCval,dummy);
    MilliSleep(100);
    if(ierr==-1) sprintf(ABCmsg,"%cCOLTTFC=NOCOMM",ABCmsg,who_selected[0]);
    
    ttfvals[0]=-((ttfvals[0]/shm_addr->MODS.convf[ttfA])); // TTFA
    sprintf(ABCval,"MOVA %f",ttfvals[0]);
    ierr=rawCommand(ttfA,ABCval,dummy);
    MilliSleep(100);
    if(ierr==-1) sprintf(ABCmsg,"%cCOLTTFA=NOCOMM",ABCmsg,who_selected[0]);
    
    if(ierr<0) {
      sprintf(reply,"%s %s",who_selected,ABCmsg);
      
      return CMD_ERR;
    }
    
    for(i=0,break_cnt=0;i<40000;i++) {
      if(break_cnt==0) {
	rawCommand(ttfB,"PRINT MVG",dummy);
	if(!strcasecmp(dummy,"FALSE")) break_cnt++;
      } else if(break_cnt==1) {
	rawCommand(ttfC,"PRINT MVG",dummy);
	if(!strcasecmp(dummy,"FALSE")) break_cnt++;
      } else if(break_cnt==2) {
	rawCommand(ttfA,"PRINT MVG",dummy);
	if(!strcasecmp(dummy,"FALSE")) break_cnt++;
      } else if(break_cnt==3) break;

      if(shm_addr->MODS.qued[ttfA]==1) break;
      MilliSleep(100);
    }
    shm_addr->MODS.qued[ttfA]=0;

    /*
    // Hold until last motor stops
    */
    rawCommand(ttfA,"PRINT POS",dummy);
    posValid = sscanf(dummy,"%f",&posA);
    if(posValid!=0) shm_addr->MODS.pos[ttfA]=fabs(posA);

    rawCommand(ttfB,"PRINT POS",dummy);
    posValid = sscanf(dummy,"%f",&posB);
    if(posValid!=0) shm_addr->MODS.pos[ttfB]=fabs(posB);
    
    rawCommand(ttfC,"PRINT POS",dummy);
    posValid = sscanf(dummy,"%f",&posC);
    if(posValid!=0) shm_addr->MODS.pos[ttfC]=fabs(posC);

    colFocus = umPerRev*(shm_addr->MODS.pos[ttfA]+shm_addr->MODS.pos[ttfB]+shm_addr->MODS.pos[ttfC])/3.0;

    sprintf(reply,"%s %cCOLFOC=%0.1f %cCOLTTFA=%0.1f %cCOLTTFB=%0.1f %cCOLTTFC=%0.1f",
	    who_selected,
	    who_selected[0],
	    colFocus,         // Total focus "piston" value = A+B+C/3
	    who_selected[0],
	    shm_addr->MODS.pos[ttfA]*shm_addr->MODS.convf[ttfA], // TTFA
	    who_selected[0],
	    shm_addr->MODS.pos[ttfB]*shm_addr->MODS.convf[ttfB], // TTFB
	    who_selected[0],
	    shm_addr->MODS.pos[ttfC]*shm_addr->MODS.convf[ttfC]); // TTFC
    
  } else {
    sprintf(reply,"%s Invalid request '%s', Usage: %s [step] [foc|ttfA ttfB ttfC]",
	    who_selected,args,who_selected);
    return CMD_ERR;
    
  }
  
  return CMD_OK;
  }

// cmd_colstep() - retired 2025 June [rwp/osu]

/* ---------------------------------------------------------------------------
//
// grating - Grating Select for Blue and Red
//
*/

/*!  
  \brief grating command - Select a grating in the grating turret 
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage: cGRATING [n|ID] 
  
  c - channel (B or R) 
  n - select grating by number [1..4] 
  ID - select grating by name Range:  1..4 for grating numbers 

  \par Description:
  Select the grating to put in the collimated beam for the named channel. 
  If given without arguments, it returns the current grating position and
  its name.  

  Grating names are stored in an external grating table populated via 
  the MODS Engineering Support Interface (MESI). 
  
  If selecting a grating by name, an exact name match (case insensitive)
  is required. If the grating position also has an associated grating tilt 
  mechanism, it must be tilted separately using the cGRTILTn command, 
  where n corresponds to the grating turret position (1..4), or the tilt 
  incremented (stepped) using cGRSTEPn.

  \par Interactions: none

  Send Blue or Red Grating Select to MicroLYNX controller command via network
  or serial. MicroLYNX Controller commands given in MODS Mechanism Commands 
  documentation.
  http://www.astronomy.ohio-state.edu/~rgonzale/MODS_Mechanism_Commands.pdf

*/

int
cmd_grating(char *args, MsgType msgtype, char *reply)
{
  int device;
  int bit_nomove;
  int bit_mask;
  int argcnt;
  int io20_Grating;
  char grating_selected[24];
  char who_selected[24];
  int dval;
  int i;
  int len;
  char dummy[PAGE_SIZE];
  char cmd_instruction[PAGE_SIZE];

  memset(cmd_instruction,0,sizeof(cmd_instruction));
  bit_nomove=bit_mask=dval=0;
  memset(dummy,0,sizeof(dummy));

  StrUpper(args);
  GetArg(args,1,cmd_instruction);

  len=strlen(args);
  if(len) // is it a single argument, or STEP + argument? 
    for(i=0,argcnt=1;i<len;i++)
      if(args[i]==' ') argcnt++;

  if (argcnt==1) dval=0; 
  else if(argcnt==2) sscanf(args,"%s %d", // user passes STEP + argument
			    cmd_instruction, &dval);

  strcpy(who_selected,cmdtab[commandID].cmd);
  StrUpper(who_selected);

  if(!strcasecmp(cmd_instruction,"RDTAB")) { // re-read the filter tables

    if(who_selected[0]=='R') {
      system("/usr/local/bin/mlcRecover rgrating");
      sprintf(reply,"%s %s table read",who_selected,cmd_instruction);
    } else if(who_selected[0]=='B') {
      system("/usr/local/bin/mlcRecover bgrating");
      sprintf(reply,"%s %s table read",who_selected,cmd_instruction);
    }
    return CMD_OK;
  }

  device=getMechanismID(cmdtab[commandID].cmd,dummy);// Get mechanism device ID
  if(device==-1) {
    sprintf(reply,"%s %s",who_selected,dummy);
    return CMD_ERR;
  }

  if(!strcasecmp(cmd_instruction,"ABORT")) {
    ierr = mlcStopMechanism(device,dummy); //  STOP the operation
      sprintf(reply,"%s %s",who_selected,dummy);
    return CMD_OK;
  }

  if (!strcasecmp(cmd_instruction,"CONFIG")) { // Get config information
    mlcMechanismConfig(device,who_selected,dummy);
    sprintf(reply,"%s",dummy);

    return CMD_OK;
  }


  if (mlcBusy(device,dummy)) { // Busy?
    sprintf(reply,"%s %s",who_selected,dummy);
    return CMD_ERR;
  }

  ierr=mlcQuery(device,1,dummy); // Check if the microLynx is ON
  if(ierr!=0) {
    sprintf(reply,"%s %s",who_selected,dummy);
    return CMD_ERR;
  }

  if(!strcasecmp(cmd_instruction,"LOCK")) { // Lock Grating Turrent
    shm_addr->MODS.LOCKS[device]=1;
    sprintf(reply," %s %s=LOCK Mechanism is locked",
	    who_selected,who_selected);
    return CMD_OK;

  } else if(!strcasecmp(cmd_instruction,"UNLOCK")) { // Unlock Grating Turrent
    shm_addr->MODS.LOCKS[device]=0;
    sprintf(reply," %s %s=UNLOCK Mechanism is unlocked",
	    who_selected,who_selected);
    return CMD_OK;
  }

  if(!shm_addr->MODS.host[device]) {
    sprintf(reply," %s %s=NOCOMM No IP address configured, check mechanisms.ini file",who_selected,who_selected);
    return CMD_ERR;
  } else if ( shm_addr->MODS.LOCKS[device] ) {
    sprintf(reply," %s %s=FAULT connection LOCKED OUT, '%s UNLOCK' to continue",who_selected,who_selected,who_selected);

    return CMD_ERR;
  }

  if (strncasecmp(args,"M#",2)) { // check for low-level command
      ierr = mlcCheckBits(device,dummy);
      if((ierr&3)== 3 || ierr==-1) {
	sprintf(reply,"%s %s",who_selected,dummy);
	return CMD_ERR;
      }
  }

  if(!strcasecmp(cmd_instruction,"RDBITS")) {

    ierr = rawCommand(device,"GSELECT",dummy); // Bits 21-24
    io20_Grating=atoi(dummy);
    ierr = rawCommand(device,"PRINT IO 25,IO 24,IO 23,IO 22,IO 21",dummy);
    sprintf(reply,"%s %s=%d BITS=%s b25-b21",
	    who_selected,who_selected,io20_Grating+1,dummy);
    return CMD_OK;

  }

  if(!strcasecmp(cmd_instruction,"RESET")) {
    
    ierr = sendCommand(device,"INITIAL",dummy);

    if(ierr!=0) {
      sprintf(reply,"%s %s=%s",who_selected,who_selected,dummy);
      return CMD_ERR;
    }

    rawCommand(device,"PRINT IO 25",dummy);
    if(!atoi(dummy)) {
      sprintf(reply,"%s %s=FAULT RESET Failed, seek did not reach valid position, in-position sensor did not assert",who_selected,who_selected);
      return CMD_ERR;
    }

    ierr = rawCommand(device,"GSELECT",dummy); // Bits 21-24
    io20_Grating=atoi(dummy);

    shm_addr->MODS.pos[device]=(int)io20_Grating+1;
    shm_addr->MODS.reqpos[device]=(float)io20_Grating+1;
    sprintf(shm_addr->MODS.state_word[device],"%d",io20_Grating+1);

    if(who_selected[0]=='R') {
      sprintf(reply,"%s %s=%d GRATNAME='%s'", who_selected, who_selected,
	      io20_Grating+1, shm_addr->MODS.rgrating[io20_Grating+1]);

    } else if(who_selected[0]=='B') {
      sprintf(reply,"%s %s=%d GRATNAME='%s'", who_selected, who_selected,
	      io20_Grating+1, shm_addr->MODS.bgrating[io20_Grating+1]);

    }

    return CMD_OK;
  }

  if (strncasecmp(args,"M#",2)) { // check for low-level command
    ierr=checkPower(device,dummy); // check the mechanism power 
    if(ierr!=0) {
      sprintf(reply,"%s %s", who_selected,dummy);
      return CMD_ERR;
    }
  }

  if (!strcasecmp(cmd_instruction,"?")) {
     sprintf(reply,"Usage: %s [n] - select grating n, leaving tilts unchanged - range %d..%d", 
             who_selected,(int)(shm_addr->MODS.min[device]),(int)(shm_addr->MODS.max[device]));
     return CMD_OK;
  }

  if (strlen(args)<=0) {  // Query when no command is issued

    rawCommand(device,"PWRFAIL=0",dummy);
    MilliSleep(100);

    rawCommand(device,"PRINT IO 25",dummy);
    if(!atoi(dummy)) {
      sprintf(reply,"%s %s=FAULT Mechanism out-of-position, in-position sensor not asserted. Reset %s to recover",who_selected,who_selected,who_selected);
      return CMD_ERR;
    }

    rawCommand(device,"GSELECT",dummy); // Bits 21-24
    io20_Grating=atoi(dummy);

    shm_addr->MODS.pos[device]=(float)io20_Grating+1;
    sprintf(shm_addr->MODS.state_word[device],"%d",io20_Grating+1);

    if(who_selected[0]=='R') {
      sprintf(reply,"%s %s=%d GRATNAME='%s'", who_selected, who_selected,
	      io20_Grating+1, shm_addr->MODS.rgrating[io20_Grating+1]);

    } else if(who_selected[0]=='B') {
      sprintf(reply,"%s %s=%d GRATNAME='%s'", who_selected, who_selected,
	      io20_Grating+1, shm_addr->MODS.bgrating[io20_Grating+1]);

    }

    return CMD_OK;
  }

  GetArg(args,1,cmd_instruction);
  if (argcnt==1) dval=atoi(cmd_instruction)-1;

  if(!strncasecmp(cmd_instruction,"M#",2)) { // check for low-level command
    ierr=mlcTechCmd(device,args,who_selected,reply);
    if(ierr<=-1) return CMD_ERR;
    return CMD_OK;

  } else if (!strcasecmp(cmd_instruction,"STEP")) {
     int desiredPosition;

     rawCommand(device,"PRINT IO 25",dummy);
     if(!atoi(dummy)) {
       sprintf(reply,"%s %s=FAULT Mechanism out-of-position, in-position sensor not asserted. Reset %s to recover",who_selected,who_selected,who_selected);
       return CMD_ERR;
     }

     ierr = rawCommand(device,"GSELECT",dummy); // Bits 21-24
     io20_Grating=atoi(dummy);

     if(ierr!=0) {
       sprintf(reply,"%s %s=%s",who_selected,who_selected,dummy);
       return CMD_ERR;
     }
    
     desiredPosition = io20_Grating + dval; // desired position, 0 based list
     if (desiredPosition > 3) desiredPosition = 3; // if out of bounds, fix it
     else if (desiredPosition < 0) desiredPosition = 0;
     
     strcpy(dummy,args); // now move to the desired position
     sprintf(grating_selected,"TARGNUM=%d",desiredPosition);
     ierr=sendTwoCommand(device,grating_selected,"BEGIN",dummy);
     
     if(ierr!=0) {
       sprintf(reply,"%s %s",who_selected,dummy);
       return CMD_ERR;
     }
     
     ierr = rawCommand(device,"GSELECT",dummy); // Bits 21-24
     io20_Grating=atoi(dummy);

     shm_addr->MODS.pos[device]=io20_Grating+1;
     shm_addr->MODS.reqpos[device]=(float)io20_Grating+1;
     sprintf(shm_addr->MODS.state_word[device],"%d",io20_Grating+1);

     if(who_selected[0]=='R') {
       sprintf(reply,"%s %s=%d GRATNAME='%s'", who_selected, who_selected,
	       io20_Grating+1, shm_addr->MODS.rgrating[io20_Grating+1]);
       
     } else if(who_selected[0]=='B') {
       sprintf(reply,"%s %s=%d GRATNAME='%s'", who_selected, who_selected,
	       io20_Grating+1, shm_addr->MODS.bgrating[io20_Grating+1]);
       
     }
     
     return CMD_OK;

  } else if (dval>=0 && dval<4) { 

    rawCommand(device,"PRINT IO 25",dummy);
    if(!atoi(dummy)) {
      sprintf(reply,"%s %s=FAULT Mechanism out-of-position, in-position sensor not asserted. Reset %s to recover",who_selected,who_selected,who_selected);
      return CMD_ERR;
    }
    else {
      rawCommand(device,"PWRFAIL=0",dummy);
      MilliSleep(100);
    }

    strcpy(dummy,args);
    sprintf(grating_selected,"TARGNUM=%d",dval);
    sendTwoCommand(device,grating_selected,"BEGIN",dummy);

    ierr = rawCommand(device,"GSELECT",dummy); // Bits 21-24
    io20_Grating=atoi(dummy);

    if(dval!=io20_Grating) {
      sprintf(reply,"%s %s=%d Move Fault, position at end of move %d but requested position %d",who_selected,who_selected,io20_Grating+1,io20_Grating+1,dval+1);
      shm_addr->MODS.reqpos[device]=(float)dval+1;
      return CMD_ERR;
    } else

     if(who_selected[0]=='R') {
       sprintf(reply,"%s %s=%d GRATNAME='%s'", who_selected, who_selected,
	       io20_Grating+1, shm_addr->MODS.rgrating[io20_Grating+1]);
       
     } else if(who_selected[0]=='B') {
       sprintf(reply,"%s %s=%d GRATNAME='%s'", who_selected, who_selected,
	       io20_Grating+1, shm_addr->MODS.bgrating[io20_Grating+1]);
       
     }

    shm_addr->MODS.pos[device]=(int)io20_Grating+1;
    shm_addr->MODS.reqpos[device]=(float)dval+1;
    sprintf(shm_addr->MODS.state_word[device],"%d",io20_Grating+1);

  } else {
      sprintf(reply,"%s Invalid request '%s', valid range is 1..4",
	      who_selected,args);
      return CMD_ERR;
  }

return CMD_OK;
}

/* ---------------------------------------------------------------------------
//
// grtilt - Grating Tilt for Blue and Red
//
*/

/*!  
  \brief grating command - Tilt grating n to a given absolute tilt 
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage: cGRTILTn [tilt|home] 
  
  c    - channel (B or R) 
  tilt - amount to tilt grating n, in device units 
  n    - grating to tilt (corresponds to position in the turret, 1..4) 
  home - set the grating tilt to the mechanical home position 

  \par Description:
  Tilts the selected grating to the specified position relative to the 
  mechanism home position.  If "home" is given instead of position value, 
  the grating tilt is driven to the mechanical home position.
 
  If given without arguments, it returns the current tilt position. 

  If the grating at that position has no tilt mechanism
  (e.g., the imaging flat or double-pass prisms), an error is returned.

  The grating tilt can be incremented ("stepped") relative to its current 
  position with a corresponding cGRSTEPn command. 

  Note that a grating can be tilted even if it is not selected to be in the
  "in-beam" position by the cGRATING command. 

  \par Interactions: none

  Send Blue or Red Grating Tilt to MicroLYNX controller command via network
  or serial. MicroLYNX Controller commands given in MODS Mechanism Commands 
  documentation.
  http://www.astronomy.ohio-state.edu/~rgonzale/MODS_Mechanism_Commands.pdf

*/

int
cmd_grtilt(char *args, MsgType msgtype, char *reply)
{
  int device;
  int bit_mask;
  int len;
  int i;
  int argcnt;
  int nval;
  char grating_selected[24];
  char who_selected[24];
  float dval;
  float posval;
  char dummy[PAGE_SIZE];
  char dummy1[PAGE_SIZE];
  char cmd_instruction[PAGE_SIZE];

  bit_mask=0;
  dval=0.000;
  memset(dummy,0,sizeof(dummy));
  memset(cmd_instruction,0,sizeof(cmd_instruction));

  GetArg(args,1,cmd_instruction);

  len=strlen(args); // single argument or STEP + argument? 
  if(len) 
    for(i=0,argcnt=1;i<len;i++)
      if(args[i]==' ') argcnt++;

  if (argcnt==1) 
    nval=sscanf(args,"%f",&dval); // user passes one argument
  else if (argcnt==2) 
    nval=sscanf(args,"%s %f",cmd_instruction,&dval);  // STEP + argument
				
  StrUpper(cmd_instruction);

  strcpy(who_selected,cmdtab[commandID].cmd);
  StrUpper(who_selected);

  /* Search for the IP and Socket */
  device=getMechanismID(cmdtab[commandID].cmd,dummy);// Get device ID
  if(device==-1) {
    sprintf(reply,"%s %s",who_selected,dummy);
    return CMD_ERR;
  }

  if (!strcasecmp(cmd_instruction,"CONFIG")) { // get mechanism infomation

    mlcMechanismConfig(device,who_selected,dummy);
    sprintf(reply,"%s",dummy);

    return CMD_OK;
  }

  if (mlcBusy(device,dummy)) { // Busy?
    sprintf(reply,"%s %s",who_selected,dummy);
    return CMD_ERR;
  }

  ierr=mlcQuery(device,1,dummy); // Check if the microLynx is ON
  if(ierr!=0) {
    sprintf(reply,"%s %s",who_selected,dummy);
    return CMD_ERR;
  }

  if(!strcasecmp(cmd_instruction,"ABORT")) {
    ierr = mlcStopMechanism(device,dummy); //  STOP the operation
    sprintf(reply,"%s %s",who_selected,dummy);
    return CMD_OK;
  }

  if(!shm_addr->MODS.host[device]) {
    sprintf(reply," %s %s=NOCOMM - NO COMMunication or not connected",who_selected,who_selected);
    return CMD_ERR;
  }

  if (strncasecmp(args,"M#",2)) { // check for low-level command
    ierr = mlcCheckBits(device,dummy); // check limit bits 21,22
    if((ierr&3)== 3 || ierr==-1) {
      sprintf(reply,"%s %s",who_selected,dummy);
      return CMD_ERR;
    }
  }

  if (!strcasecmp(cmd_instruction,"RDBITS")) {

    rawCommand(device,"PRINT IO 22,IO 21",dummy);

    sprintf(reply,"%s %s=%0.3f BITS=%s b22-b21",who_selected,who_selected,
	    shm_addr->MODS.pos[device]*shm_addr->MODS.convf[device],dummy);

  }

  if (strlen(args)<=0) {  // Query when no command is issued
    rawCommand(device,"PRINT PWRFAIL",dummy);
    ierr=atoi(dummy);

    if(ierr) {
      rawCommand(device,"PWRFAIL=0",dummy); // reset the powerfail
      sprintf(dummy1,"POS=%f",shm_addr->MODS.pos[device]);
      rawCommand(device,dummy1,dummy); // reset microlynx position.

      sprintf(reply,"%s %s=%0.0f Mechanism was power-cycled, using last known position", who_selected, who_selected, shm_addr->MODS.pos[device]*shm_addr->MODS.convf[device]);

      return CMD_WARN;
    }

    shm_addr->MODS.pos[device]=fabs(positionToShrMem(device,dummy));
    sprintf(shm_addr->MODS.state_word[device],"%0.0f",
	    shm_addr->MODS.pos[device]);
    sprintf(reply,"%s %s=%0.0f",who_selected,who_selected,
	    shm_addr->MODS.pos[device]*shm_addr->MODS.convf[device]);

    return CMD_OK;
  }


  if(!strncasecmp(args,"M#",2)) { // check for low-level command
    ierr=rawCommand(device,"DRVEN=1",dummy); // Send a Raw command

    ierr=mlcTechCmd(device,args,who_selected,reply);
    if(ierr<=-1) {
      ierr=rawCommand(device,"DRVEN=0",dummy); // Send a Raw command
      return CMD_ERR;
    }

    ierr=rawCommand(device,"DRVEN=0",dummy); // Send a Raw command

    return CMD_OK;

  } else if(!strcasecmp(cmd_instruction,"RESET")) {
    
    sprintf(reply,"%s %s=%0.0f Invalid %s for Grating Tilt, Requires a HARDRESET or SOFTRESET",who_selected,who_selected,shm_addr->MODS.pos[device]*shm_addr->MODS.convf[device], cmd_instruction);
    return CMD_ERR;

  } else if(!strcasecmp(cmd_instruction,"HARDRESET")) {
    
    sendCommand(device,"INITIAL",dummy);
    
    shm_addr->MODS.pos[device]=fabs(positionToShrMem(device,dummy));
    sprintf(reply,"%s %s=%0.0f Hard Reset Completed",
	    who_selected,who_selected,
	    shm_addr->MODS.pos[device]*shm_addr->MODS.convf[device]);

  } else if(!strcasecmp(cmd_instruction,"SOFTRESET")) {
    
    rawCommand(device,"PRINT PWRFAIL",dummy);
    ierr=atoi(dummy);

    if(ierr) {
      sprintf(dummy1,"POS=%f",shm_addr->MODS.pos[device]);
      rawCommand(device,dummy1,dummy); // reset microlynx position.
      rawCommand(device,"PWRFAIL=0",dummy); // reset the powerfail

      sprintf(reply,"%s %s=%0.0f Mechanism was power-cycled, using last known position", who_selected, who_selected, shm_addr->MODS.pos[device]*shm_addr->MODS.convf[device]);

      return CMD_WARN;

    } else {
      if(shm_addr->MODS.pos[device] != fabs(positionToShrMem(device,dummy)))
	shm_addr->MODS.pos[device]=fabs(positionToShrMem(device,dummy));
	
      sprintf(reply,"%s %s=%0.0f Soft Reset Completed", 
	      who_selected, who_selected, 
	      shm_addr->MODS.pos[device]*shm_addr->MODS.convf[device]);

      return CMD_OK;

    }
  } else if(!strcasecmp(cmd_instruction,"HOME")) {
    rawCommand(device,"PRINT PWRFAIL",dummy);
    ierr=atoi(dummy);

    if(ierr!=0) {
      sprintf(reply,"%s %s=PWRFLR %s has been power cycled and must be HARDRESET or SOFTRESET to initialize",who_selected,who_selected,who_selected);

      return CMD_ERR;

    }      

    sendCommand(device,"HOME",dummy);

    shm_addr->MODS.pos[device]=fabs(positionToShrMem(device,dummy));
    sprintf(reply,"%s %s=%0.0f HOMED to position 0",who_selected,who_selected,
	    shm_addr->MODS.pos[device]*shm_addr->MODS.convf[device]);

  } else if(!strcasecmp(cmd_instruction,"STEP")) {
    rawCommand(device,"PRINT PWRFAIL",dummy);
    ierr=atoi(dummy);

    if(ierr!=0) {
      sprintf(reply,"%s %s=PWRFLR %s has been power cycled and must be HARDRESET or SOFTRESET to initialize",who_selected,who_selected,who_selected);

      return CMD_ERR;

    }      

    sprintf(grating_selected,"TILT=%f",-(dval/shm_addr->MODS.convf[device]));
    sendCommand(device,grating_selected,dummy);

    ierr = sendCommand(device,"ZLTILTR",dummy);

    if(ierr!=0) {
      sprintf(reply,"%s",dummy);
      return CMD_ERR;
    }

    bit_mask = mlcCheckBits(device,dummy); // check limit bits 21,22
    if(bit_mask < 0) {
      sprintf(reply,"%s %s",who_selected,dummy);
      return CMD_ERR;
    }

    shm_addr->MODS.pos[device]=fabs(positionToShrMem(device,dummy));
    sprintf(reply,"%s %s=%0.0f",who_selected,who_selected,
	    shm_addr->MODS.pos[device]*shm_addr->MODS.convf[device]);

    sprintf(dummy,"GTilt1 Step Value: %f, revpos=%f, tiltpos=%f",
	    -(dval/shm_addr->MODS.convf[device]),
	    shm_addr->MODS.pos[device],
	    shm_addr->MODS.pos[device]*shm_addr->MODS.convf[device]);

    mmcLOGGER(shm_addr->MODS.LLOG,dummy); // log values

  } else if (dval>=0.000) { 

    rawCommand(device,"PRINT PWRFAIL",dummy);
    ierr=atoi(dummy);

    if(ierr) {
      rawCommand(device,"PWRFAIL=0",dummy); // reset the powerfail
      sprintf(dummy1,"POS=%f",shm_addr->MODS.pos[device]);
      rawCommand(device,dummy1,dummy); // reset microlynx position.

      sprintf(grating_selected,"TILT=%f",-(dval/shm_addr->MODS.convf[device]));
      sendCommand(device,grating_selected,dummy);

      ierr = sendCommand(device,"ZLTILTA",dummy);


      shm_addr->MODS.pos[device]=fabs(positionToShrMem(device,dummy));
      sprintf(reply,"%s %s=%0.0f Mechanism was power-cycled, using last known position", who_selected, who_selected, shm_addr->MODS.pos[device]*shm_addr->MODS.convf[device]);

      return CMD_WARN;
    }

    sprintf(grating_selected,"TILT=%f",-(dval/shm_addr->MODS.convf[device]));

    sendCommand(device,grating_selected,dummy);

    ierr = sendCommand(device,"ZLTILTA",dummy);

    if(ierr!=0) {
      sprintf(reply,"%s",dummy);
      return CMD_ERR;
    }

    bit_mask = mlcCheckBits(device,dummy); // check limit bits 21,22
    if(bit_mask < 0) {
      sprintf(reply,"%s %s",who_selected,dummy);
      return CMD_ERR;
    }

    shm_addr->MODS.pos[device]=fabs(positionToShrMem(device,dummy));
    sprintf(reply,"%s %s=%0.0f",who_selected,who_selected,
	    shm_addr->MODS.pos[device]*shm_addr->MODS.convf[device]);
    
  } else {
    sprintf(reply,"%s Invalid request '%s', valid range is %0.0f...%0.0f",
	    who_selected,args,
	    shm_addr->MODS.min[device],
	    shm_addr->MODS.max[device]);
    return CMD_ERR;
  }

  return CMD_OK;
}

// cmd_grstep() - retired 2025 June (unused)

/* ---------------------------------------------------------------------------
//
// shutter - Camera Shutter commands for Blue and Red
//
*/

/*!  
  \brief shutter command - Open/Close the camera shutter
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage: cSHUTTER [open|close]
  
  c     - channel (B or R) 
  open  - open the named camera shutter 
  close - close the named camera shutter

  \par Description:
       Opens or closes the shutter on the named camera.  If given without 
       arguments it returns the current shutter status (open or closed) of
       the named camera.

       The shutter will stay open indefinitely after this command, it does not 
       perform a timed shutter cycle (that is done through the detector control
       system).

  \par Interactions: none

  Send Blue or Red Shutter to MicroLYNX controller command via network
  or serial. MicroLYNX Controller commands given in MODS Mechanism Commands 
  documentation.
  http://www.astronomy.ohio-state.edu/~rgonzale/MODS_Mechanism_Commands.pdf

*/
int
cmd_shutter(char *args, MsgType msgtype, char *reply)
{
  int len;                         // command string length
  int device;                      // MicroLynx Controller(MLC) device
  char temp[79];                   // temp character array
  char dummy[PAGE_SIZE];           // dummy character array
  char who_selected[24];
  char cmd_instruction[PAGE_SIZE];        // instruction for MLC command

  memset(cmd_instruction,0,sizeof(cmd_instruction));
  memset(reply,0,sizeof(reply));   // Empty the dummy character array
  sprintf(reply,".");              // Initialize reply
  memset(dummy,0,sizeof(dummy));   // Empty the dummy character array
  StrUpper(args);                  // Convert to upper case
  GetArg(args,1,cmd_instruction);  // get the first argument in 'args' string

  strcpy(who_selected,cmdtab[commandID].cmd);
  StrUpper(who_selected);

  device=getMechanismID(cmdtab[commandID].cmd,dummy);// Get mechanism device ID
  if(device==-1) {
    sprintf(reply,"%s %s",who_selected,dummy);
    return CMD_ERR;
  }

  if (!strcasecmp(cmd_instruction,"CONFIG")) { // get mechanism ip

    mlcMechanismConfig(device,who_selected,dummy);
    sprintf(reply,"%s",dummy);

    return CMD_OK;
  }

  ierr=mlcQuery(device,1,dummy); // Check if the microLynx is ON
  if(ierr!=0) {
    sprintf(reply,"%s %s",who_selected,dummy);
    return CMD_ERR;
  }

  if(!shm_addr->MODS.host[device]) {
    sprintf(reply," %s %s=NOCOMM - NO COMMunication or not connected",who_selected,who_selected);
    return CMD_ERR;
  }

  if(!strcasecmp(cmd_instruction,"ABORT") ||
     !strcasecmp(cmd_instruction,"LOCAL") ||
     !strcasecmp(cmd_instruction,"DISABLE") ) {
    ierr = mlcStopMechanism(device,dummy); //  STOP the operation
    rawCommandOnly(device,"RSTATE=0");// Disable the HE trigger
    sprintf(reply,"%s %s=DISABLED Head Electronics trigger disabled",who_selected,who_selected);
    he_token=0;
    return CMD_OK;
  }

  if(!strcasecmp(cmd_instruction,"ENABLE") ||
     !strcasecmp(cmd_instruction,"REMOTE")) {
    rawCommandOnly(device,"EXEC STARTIT");// Enable the HE trigger
    sprintf(reply,"%s %s=ENABLED Head Electronics trigger enabled",who_selected,who_selected);
    he_token=1;
    return CMD_OK;
  }

  if (he_token) {
    sprintf(reply,"%s %s=ENABLED You must disable to query, enable when done with query",who_selected,who_selected);
    return CMD_ERR;
  }

  //if (mlcBusy(device,dummy)) { // Busy?
  //sprintf(reply,"%s %s",who_selected,dummy);
  //return CMD_ERR;
  //}

  memset(dummy,0,sizeof(dummy));   // Empty the dummy character array
  if (strlen(args)<=0) {  // Query when no command is issued
    ierr = sendCommand(device,"SHUTTER",dummy);    // open and check state
    if(ierr!=0) {
      sprintf(reply,"%s %s=AJAR shutter partially opened",
	      who_selected,who_selected);
      return CMD_ERR;
    }
    sprintf(reply,"%s %s=%s",who_selected,who_selected,dummy);
    return CMD_OK;
  }

  if(!strcasecmp(cmd_instruction,"OPEN")) {         
    ierr = sendCommand(device,"EXEC OPEN",dummy); // Open the shutter
    if(ierr!=0) {
      sprintf(reply,"%s %s=AJAR shutter partially opened",
	      who_selected,who_selected);
      return CMD_ERR;
    }
    sprintf(reply,"%s %s=OPEN",who_selected,who_selected);

  } else if(!strcasecmp(cmd_instruction,"CLOSE")) {
    ierr = sendCommand(device,"EXEC CLOSE",dummy); // Close the shutter
    if(ierr!=0) {
      sprintf(reply,"%s %s=AJAR shutter partially opened",
	      who_selected,who_selected);
      return CMD_ERR;
    }
    sprintf(reply,"%s %s=CLOSED",who_selected,who_selected);

  } else if(!strcasecmp(cmd_instruction,"SNAP")) {  // SNAP Shot
    GetArg(args,2,temp);                 // get timer
    sendCommand(device,"EXEC OPEN",dummy); // Open
    MilliSleep(atoi(temp));              // Milliseconds
    ierr = sendCommand(device,"EXEC CLOSE",dummy);// Close
    if(ierr!=0) {
      sprintf(reply,"%s %s=AJAR shutter partially opened",
	      who_selected,who_selected);
      return CMD_ERR;
    }
    sprintf(reply,"%s %s=%s",who_selected,who_selected,dummy);

  } else if(!strncasecmp(args,"M#",2)) { // check for low-level command

    ierr=mlcTechCmd(device,args,who_selected,reply);
    if(ierr<=-1) return CMD_ERR;
    return CMD_OK;

  } else {
    sprintf(reply,"%s Invalid request '%s', %s [OPEN|CLOSE]",who_selected,cmd_instruction,who_selected);
    return CMD_ERR;
  }

  return CMD_OK;
}

/* ---------------------------------------------------------------------------
//
// camfoc - Camera Focus commands for Blue and Red
//
*/

/*!  
  \brief camfoc command - Focus the camera primary mirror
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage: cCAMFOC [foc|home|step +/-foc]
  
  c     - channel (B or R) 
  foc   - focus position of the camera primary mirror in device units 
  home  - send the camera primary mirror focus to its mechanical home position

  \par Description:
       Sets the named camera primary mirror absolute focus position.  
       Increasing focus position moves the camera primary mirror closer to 
       the CCD detector.

       The home keyword is used to drive the camera primary mirror to the 
       mechanical home position (and away from the CCD).

       If given without arguments, it returns the absolute focus position of 
       the named camera primary mirror.

       To make incremental +/- changes to the camera primary mirror focus, 
       use STEP.

  \par Interactions: none

  Send Blue or Red Focus to MicroLYNX controller command via network
  or serial. MicroLYNX Controller commands given in MODS Mechanism Commands 
  documentation.
  http://www.astronomy.ohio-state.edu/~rgonzale/MODS_Mechanism_Commands.pdf

*/
int
cmd_camfoc(char *args, MsgType msgtype, char *reply)
{
  int  ierr;
  int  cmdlen;
  int  device;
  float fval;
  float validMove;
  char Fval[10];
  char who_selected[24];
  char cmd_instruction[PAGE_SIZE];
  char dummy[PAGE_SIZE];
  char chkbusybit[128];
  int  camfocPos;

  memset(dummy,0,sizeof(dummy));
  memset(cmd_instruction,0,sizeof(cmd_instruction));
  fval=0.0;
  cmdlen=strlen(args);

  StrUpper(args);
  GetArg(args,1,cmd_instruction);

  strcpy(who_selected,cmdtab[commandID].cmd);
  StrUpper(who_selected);

  /* Search for the IP and Socket */
  device=getMechanismID(cmdtab[commandID].cmd,dummy);// Get mechanism device ID
  if(device==-1) {
    sprintf(reply,"%s %s",who_selected,dummy);
    return CMD_ERR;
  }

  if(!shm_addr->MODS.host[device]) {
    sprintf(reply," %s %s=NOCOMM - NO COMMunication or not connected",who_selected,who_selected);
    return CMD_ERR;
  }

  if(!strcasecmp(argbuf,"ABORT")) {
    ierr = mlcStopMechanism(device,dummy); //  STOP the operation
    sprintf(reply,"%s %s",who_selected,dummy);
    return CMD_OK;
  }

  if (mlcBusy(device,dummy)) { // Busy?
    sprintf(reply,"%s %s",who_selected,dummy);
    return CMD_ERR;
  }

  ierr=mlcQuery(device,1,dummy); // Check if the microLynx is ON
  if(ierr!=0) {
    sprintf(reply,"%s %s",who_selected,dummy);
    return CMD_ERR;
  }

  if (!strcasecmp(cmd_instruction,"CONFIG")) { // get mechanism ip
    
    mlcMechanismConfig(device,who_selected,dummy);
    sprintf(reply,"%s",dummy);

    return CMD_OK;
  }

  shm_addr->MODS.mlunit=device; // Selected device

  StrUpper(args);
  GetArg(args,1,cmd_instruction);

  if (!strcasecmp(cmd_instruction,"?")) {
      sprintf(reply,"%s [step|reset] focval, focval in microns",who_selected);
  }

  if(!strcasecmp(cmd_instruction,"STATUS")) {
    rawCommand(device,"PRINT IO 20",dummy);
    if (shm_addr->MODS.pos[device] == 0) 
      camfocPos = 0;
    else
      camfocPos = (int)((shm_addr->MODS.pos[device]*shm_addr->MODS.convf[device]));
    sprintf(reply,"NAME=%s IP:PORT=%s %s=%d RawPos=%f IO20=%s", 
	    shm_addr->MODS.who[device],
            shm_addr->MODS.commport[device].Port,
	    shm_addr->MODS.who[device],
    	    camfocPos,
	    shm_addr->MODS.pos[device],
            dummy);
    if (mlcBusy(device,dummy))
      strcat(reply," Device=BUSY");
    else
      strcat(reply," Device=IDLE");

    return CMD_OK;
  }

  if (strncasecmp(args,"M#",2)) { // check for low-level command
    ierr = mlcCheckBits(device,dummy); // check limit bits 21,22
    if((ierr&3)== 3 || ierr==-1) {
      sprintf(reply,"%s %s",who_selected,dummy);
      return CMD_ERR;
    }
  }

  if(!strcasecmp(cmd_instruction,"RESET")) {
    rawCommand(device,"INITIAL",dummy);
    MilliSleep(200);
    rawCommand(device,"PRINT POS",dummy); // Send a Raw command
    shm_addr->MODS.pos[device]=fabs(positionToShrMem(device,dummy));
    if (shm_addr->MODS.pos[device] == 0) 
      camfocPos = 0;
    else
      camfocPos = (int)((shm_addr->MODS.pos[device]*shm_addr->MODS.convf[device]));
    sprintf(reply,"%s %s=%d",who_selected,who_selected,camfocPos);
    return CMD_OK;
  }

  if (strncasecmp(args,"M#",2)) { // check for low-level command
    ierr=checkPower(device,dummy); // check the mechanism power 
    if(ierr!=0) {
      sprintf(reply,"%s %s", who_selected,dummy);
      return CMD_ERR;
    }
  }

  if (cmdlen<=0) {  // Query when no command is issued
    ierr = rawCommand(device,"PRINT POS",dummy); // Send a Raw command
    ierr = sscanf(dummy,"%f", &fval);
    shm_addr->MODS.pos[device]=fabs(positionToShrMem(device,dummy));
    if (shm_addr->MODS.pos[device] == 0) 
      camfocPos = 0;
    else
      camfocPos = (int)((shm_addr->MODS.pos[device]*shm_addr->MODS.convf[device]));
    sprintf(reply,"%s %s=%d",who_selected,who_selected,camfocPos);
    return CMD_OK;
  }

  if (!strcasecmp(cmd_instruction,"STEP")) {

    GetArg(args,2,cmd_instruction); // Get step count
    ierr = sscanf(cmd_instruction,"%f",&fval);

    validMove=(shm_addr->MODS.pos[device]*shm_addr->MODS.convf[device])+fval;

    if(validMove < shm_addr->MODS.min[device] || 
       validMove > shm_addr->MODS.max[device]) {

      sprintf(reply,"%s %s=%0.0f Request '%0.0f', will exceed actuator range. %d..%d[%0.0f]", who_selected,who_selected,
	      shm_addr->MODS.pos[device]*shm_addr->MODS.convf[device],
	      fval,
	      (int)(shm_addr->MODS.min[device]),
	      (int)(shm_addr->MODS.max[device]),validMove);
      
      return CMD_ERR;
    }

    //GetArg(args,2,cmd_instruction); // Get step count
    //ierr = sscanf(cmd_instruction,"%f",&fval);

    fval=-((fval/shm_addr->MODS.convf[device]));
    sprintf(Fval,"MOVR %f",fval);
    rawCommand(device,Fval,dummy);
    MilliSleep(1000);
    sprintf(Fval,"HOLD 2",fval);
    rawCommand(device,Fval,dummy);

    memset(dummy,0,sizeof(dummy));
    shm_addr->MODS.pos[device]=fabs(positionToShrMem(device,dummy));
    if (shm_addr->MODS.pos[device] == 0) 
      camfocPos = 0;
    else
      camfocPos = (int)((shm_addr->MODS.pos[device]*shm_addr->MODS.convf[device]));

    /*
    // Test to see if we smacked into a limit switch
    */
    if ( mlcBitsBase10(device,"PRINT IO 22,IO 21",dummy) == 1 ) {
      sprintf(reply,"%s %s=%d actuator relative move asserted CW limit",
	      who_selected,who_selected,camfocPos);
      return CMD_ERR;

    } else if ( mlcBitsBase10(device,"PRINT IO 22,IO 21",dummy) == 2 ) {
      sprintf(reply,"%s %s=%d actuator relative move asserted CCW limit",
	      who_selected,who_selected,camfocPos);
      return CMD_ERR;

    } else if ( mlcBitsBase10(device,"PRINT IO 22,IO 21",dummy) == 3 ) {
      sprintf(reply,"%s %s=%d Sensor Fault, both limits asserted",
	      who_selected,who_selected,camfocPos);
      return CMD_ERR;
    }

    sprintf(reply,"%s %s=%d",who_selected,who_selected,camfocPos);

  } else if (!strcasecmp(cmd_instruction,"RDBITS")) {

    rawCommand(device,"PRINT IO 21,IO 22",dummy);
    camfocPos = (int)((shm_addr->MODS.pos[device]*shm_addr->MODS.convf[device]));
    sprintf(reply,"%s %s=%d BITS=%s b22-b21",
	    who_selected,who_selected,camfocPos,dummy);

    return CMD_OK;

  } else if(!strcasecmp(cmd_instruction,"HOME")) {
    
    rawCommand(device,"EXEC HOME",dummy);

    memset(dummy,0,sizeof(dummy));
    shm_addr->MODS.pos[device]=fabs(positionToShrMem(device,dummy));
    if (shm_addr->MODS.pos[device] == 0) 
      camfocPos = 0;
    else
      camfocPos = (int)((shm_addr->MODS.pos[device]*shm_addr->MODS.convf[device]));
    sprintf(reply,"%s %s=%d",who_selected,who_selected,camfocPos);

  } else if(!strcasecmp(cmd_instruction,"CENTER")) {

    sprintf(Fval,"FOCUS=-4.749");
    sendTwoCommand(device,Fval,"CAMFOCUS",dummy);

    memset(dummy,0,sizeof(dummy));
    shm_addr->MODS.pos[device]=fabs(positionToShrMem(device,dummy));
    if (shm_addr->MODS.pos[device] == 0) 
      camfocPos = 0;
    else
      camfocPos = (int)((shm_addr->MODS.pos[device]*shm_addr->MODS.convf[device]));
    sprintf(reply,"%s %s=%d",who_selected,who_selected,camfocPos);

  } else if(!strcasecmp(cmd_instruction,"CCWLIMIT")) {

    sprintf(Fval,"FOCUS=%f",-10.0);
    sendTwoCommand(device,Fval,"CAMFOCUS",dummy);

    MilliSleep(1000);
    rawCommand(device,"PRINT POS",dummy); // Send a Raw command
    shm_addr->MODS.pos[device]=fabs(positionToShrMem(device,dummy));
    if (shm_addr->MODS.pos[device] == 0) 
      camfocPos = 0;
    else
      camfocPos = (int)((shm_addr->MODS.pos[device]*shm_addr->MODS.convf[device]));
    sprintf(reply,"%s %s=%d",who_selected,who_selected,camfocPos);

    
  } else if(!strcasecmp(cmd_instruction,"CWLIMIT")) {

    sprintf(Fval,"FOCUS=%f",10.0);
    sendTwoCommand(device,Fval,"CAMFOCUS",dummy);

    MilliSleep(1000);
    rawCommand(device,"PRINT POS",dummy); // Send a Raw command
    shm_addr->MODS.pos[device]=fabs(positionToShrMem(device,dummy));
    if (shm_addr->MODS.pos[device] == 0) 
      camfocPos = 0;
    else
      camfocPos = (int)((shm_addr->MODS.pos[device]*shm_addr->MODS.convf[device]));
    sprintf(reply,"%s %s=%d",who_selected,who_selected,camfocPos);
    
  } else if(!strncasecmp(args,"M#",2)) { // check for low-level command

    ierr=mlcTechCmd(device,args,who_selected,reply);
    if(ierr<=-1) return CMD_ERR;
    return CMD_OK;

  } else if(!strncasecmp(args,"NOMINAL",2)) { // check for low-level command

    if(who_selected[0]=='R') {
      sprintf(Fval,"FOCUS=%f",(float)shm_addr->MODS.r_camfocNominal);
    } else if(who_selected[0]=='B') {
      sprintf(Fval,"FOCUS=%f",(float)shm_addr->MODS.b_camfocNominal);
    } else
      sprintf(Fval,"FOCUS=0.0");

    sendTwoCommand(device,Fval,"CAMFOCUS",dummy);

    shm_addr->MODS.pos[device]=fabs(positionToShrMem(device,dummy));
    if (shm_addr->MODS.pos[device] == 0) 
      camfocPos = 0;
    else
      camfocPos = (int)((shm_addr->MODS.pos[device]*shm_addr->MODS.convf[device]));
    sprintf(reply,"%s %s=%d",who_selected,who_selected,camfocPos);

  } else {

    ierr = sscanf(args,"%f", &fval);

    if(ierr) {
      if(fval < 0.0 || fval > 4750.0) {
	sprintf(reply,"%s Invalid request '%0.0f', valid range is %d..%d microns", 
		who_selected,fval,
		(int)(shm_addr->MODS.min[device]),
		(int)(shm_addr->MODS.max[device]));
	return CMD_ERR;
      }

      fval=((fval/shm_addr->MODS.convf[device]));
      sprintf(Fval,"FOCUS=%f",-fval);
      sendTwoCommand(device,Fval,"CAMFOCUS",dummy);

      shm_addr->MODS.pos[device]=fabs(positionToShrMem(device,dummy));
      if (shm_addr->MODS.pos[device] == 0) 
	camfocPos = 0;
      else
	camfocPos = (int)((shm_addr->MODS.pos[device]*shm_addr->MODS.convf[device]));

      /*
      // Test to see if we smacked into a limit switch
      */
      if ( mlcBitsBase10(device,"PRINT IO 22,IO 21",dummy) == 1 ) {
	sprintf(reply,"%s %s=%d actuator move asserted CW limit",
		who_selected,who_selected,camfocPos);
	return CMD_ERR;
	
      } else if ( mlcBitsBase10(device,"PRINT IO 22,IO 21",dummy) == 2 ) {
	sprintf(reply,"%s %s=%d actuator move asserted CCW limit",
		who_selected,who_selected,camfocPos);
	return CMD_ERR;
	
      } else if ( mlcBitsBase10(device,"PRINT IO 22,IO 21",dummy) == 3 ) {
	sprintf(reply,"%s %s=%d Sensor Fault, both limits asserted",
		who_selected,who_selected,camfocPos);
	return CMD_ERR;
      }
      
      sprintf(reply,"%s %s=%d",who_selected,who_selected,camfocPos);

    } else {
      sprintf(reply,"%s Invalid request '%s', Usage: %s [step|reset] [focval]",
	      who_selected,args,who_selected);
      return CMD_ERR;
    }
  }

  return CMD_OK;
}

/* ---------------------------------------------------------------------------
//
// filter - Filter Wheel for Blue and Red
//
*/

/*!  
  \brief filter command - Select a filter on the filter wheel 
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage: cFILTER [n|ID] [load] 
  
  c - channel (B or R) 
  n - select filter by number [1..8] 
  ID - select filter by name Range:  1..8 for filter numbers 
  load - select filter position for loading: Range:  1..8 filter position

  \par Description:
  Select the filter to put in front of the CCD in the named camera. 
  If given without arguments, it returns the current filter position and
  its name.  

  Filter names are stored in an external filter table populated via 
  the MODS Engineering Support Interface (MESI). 
  
  If selecting a filter by name, an exact name match (case insensitive)
  is required.

  \par Interactions: none

  Send Blue or Red Filter Wheel to MicroLYNX controller command via network
  or serial. MicroLYNX Controller commands given in MODS Mechanism Commands 
  documentation.
  http://www.astronomy.ohio-state.edu/~rgonzale/MODS_Mechanism_Commands.pdf

*/

int
cmd_filter(char *args, MsgType msgtype, char *reply)
{
  int ierr,i;
  int device;
  int load_filter;
  int filterval;
  int filter_pos;
  int bit_mask;
  int step_count;
  float fval;
  char Fval[10];
  char filter_selected[24];
  char who_selected[24];
  char dummy[PAGE_SIZE];
  char cmd_instruction[PAGE_SIZE];

  bit_mask=filterval=load_filter=0;
  memset(dummy,0,sizeof(dummy));
  memset(cmd_instruction,0,sizeof(cmd_instruction));

  StrUpper(args);
  GetArg(args,1,cmd_instruction);

  strcpy(who_selected,cmdtab[commandID].cmd); // What color am I?
  StrUpper(who_selected);

  if(!strcasecmp(cmd_instruction,"RDTAB")) { // re-read the filter tables
    if(who_selected[0]=='R') {
      system("/usr/local/bin/mlcRecover rfilter");
      sprintf(reply,"%s %s table read",who_selected,cmd_instruction);
    } else if(who_selected[0]=='B') {
      system("/usr/local/bin/mlcRecover bfilter");
      sprintf(reply,"%s %s table read",who_selected,cmd_instruction);
    }
    return CMD_OK;
  }

  device=getMechanismID(cmdtab[commandID].cmd,dummy);// Get mechanism device ID
  if(device==-1) {
    sprintf(reply,"%s %s",who_selected,dummy);
    return CMD_ERR;
  }

  if (!strcasecmp(cmd_instruction,"CONFIG")) { // get mechanism ip
    mlcMechanismConfig(device,who_selected,dummy);
    sprintf(reply,"%s",dummy);

    return CMD_OK;
  }

  if(!shm_addr->MODS.host[device]) {
    sprintf(reply," %s %s=NOCOMM - NO COMMunication or not connected",who_selected,who_selected);
    return CMD_ERR;
  }

  if(!strcasecmp(cmd_instruction,"ABORT")) {
    ierr = mlcStopMechanism(device,dummy); //  STOP the operation
    sprintf(reply,"%s %s",who_selected,dummy);
    return CMD_OK;
  }

  if (mlcBusy(device,dummy)) { // Busy?
    sprintf(reply,"%s %s",who_selected,dummy);
    return CMD_ERR;
  }
  
  ierr=mlcQuery(device,1,dummy); // Check if the microLynx is ON
  if(ierr!=0) {
    sprintf(reply,"%s %s",who_selected,dummy);
    return CMD_ERR;
  }

  if(!strcasecmp(cmd_instruction,"RESET")) {

    rawCommand(device,"INITIAL",dummy);

    rawCommand(device,"PWRFAIL=0",dummy);

    MilliSleep(100);

    rawCommand(device,"PRINT IO 24",dummy);
    if(!atoi(dummy)) {
      filter_pos=mlcBitsBase10(device,"PRINT IO 23,IO 22,IO 21",dummy);
      sprintf(filter_selected,"TARGNUM=%d",filter_pos+1);
      sendTwoCommand(device,filter_selected,"BEGIN",dummy);
    }

    filter_pos=mlcBitsBase10(device,"PRINT IO 23,IO 22,IO 21",dummy)+1;

    rawCommand(device,"PRINT IO 24",dummy);
    if(!atoi(dummy)) { // try again
      sprintf(reply,"%s %s=%d RESET Failed, seek did not reach valid position, in-position sensor did not assert",who_selected,who_selected,filter_pos);
      return CMD_ERR;
    }

    shm_addr->MODS.pos[device]=(float)filter_pos;
    shm_addr->MODS.reqpos[device]=(float)filter_pos;

    if(who_selected[0]=='R') {
      sprintf(reply,"%s %s=%d FILTNAME='%s'",who_selected,who_selected,
	      filter_pos, shm_addr->MODS.rcamfilters[filter_pos]);

    } else if(who_selected[0]=='B') {
      sprintf(reply,"%s %s=%d FILTNAME='%s'",who_selected,who_selected,
	      filter_pos, shm_addr->MODS.bcamfilters[filter_pos]);
    }

    return CMD_OK;
  }

  shm_addr->MODS.mlunit=device; // Selected device

  if (!strcasecmp(args,"?")) {
    sprintf(reply,"%s [load] [n|ID], n=filter# or load filter#",who_selected);
    return CMD_OK;
  }

  if(!strcasecmp(cmd_instruction,"STATUS")) {
    rawCommand(device,"PRINT IO 20",dummy);
    sprintf(reply,"NAME='%s' IP:PORT=%s %s=%d", 
	    shm_addr->MODS.who[device],
            shm_addr->MODS.commport[device].Port,
	    who_selected,
	    filterval+1);
    if (mlcBusy(device,dummy))
      strcat(reply," Device=BUSY");
    else
      strcat(reply," Device=IDLE");
    return CMD_OK;
  }

  filterval=0;

  if (strlen(args)<=0) {  // Query when no command is issued
    rawCommand(device,"PWRFAIL=0",dummy);

    rawCommand(device,"PRINT IO 24",dummy);
    if(!atoi(dummy)) {
      sprintf(reply,"%s %s=FAULT Mechanism out-of-position, in-position sensor not asserted. Reset %s to recover",who_selected,who_selected,who_selected);
      return CMD_ERR;
    }

    filter_pos=mlcBitsBase10(device,"PRINT IO 23,IO 22,IO 21",dummy)+1;

    if(who_selected[0]=='R') {
      sprintf(reply,"%s %s=%d FILTNAME='%s'",who_selected,who_selected,
	      filter_pos, shm_addr->MODS.rcamfilters[filter_pos]);

    } else if(who_selected[0]=='B') {
      sprintf(reply,"%s %s=%d FILTNAME='%s'",who_selected,who_selected,
	      filter_pos, shm_addr->MODS.bcamfilters[filter_pos]);
    }

    shm_addr->MODS.pos[device]=(float)filter_pos;
    shm_addr->MODS.reqpos[device]=(float)filter_pos;

    return CMD_OK;
  }

  load_filter=-1;
  if(!strncasecmp(args,"LOAD",4)) {
    load_filter=1;
    GetArg(args,2,argbuf);

    filterval=atoi(argbuf)-1;
    if(load_filter) {
      if(!strcasecmp(who_selected,"BFILTER")) {
	load_filter=filterval;
	if(filterval==0) filterval=6;
	else if(filterval==1) filterval=7;
	else if(filterval==2) filterval=0;
	else if(filterval==3) filterval=1;
	else if(filterval==4) filterval=2;
	else if(filterval==5) filterval=3;
	else if(filterval==6) filterval=4;
	else if(filterval==7) filterval=5;
	else filterval=-1;
      } else {
	load_filter=filterval;
	if(filterval==0) filterval=2;
	else if(filterval==1) filterval=3;
	else if(filterval==2) filterval=4;
	else if(filterval==3) filterval=5;
	else if(filterval==4) filterval=6;
	else if(filterval==5) filterval=7;
	else if(filterval==6) filterval=0;
	else if(filterval==7) filterval=1;
	else filterval=-1;
      }
    }
  } else  {
    filterval=atoi(cmd_instruction)-1;
  }

  if(!strncasecmp(args,"M#",2)) { // check for low-level command

    ierr=mlcTechCmd(device,args,who_selected,reply);
    if(ierr<=-1) return CMD_ERR;
    return CMD_OK;

  } else if (!strcasecmp(cmd_instruction,"STEP")) {
    //int stepPos;
    GetArg(args,2,argbuf); // Get step count

    rawCommand(device,"DRVEN=1",dummy);

    rawCommand(device,"PRINT IO 24",dummy);
    if(!atoi(dummy)) {
      sprintf(reply,"%s %s=FAULT Mechanism out-of-position, in-position sensor not asserted. Reset %s to recover",who_selected,who_selected,who_selected);
      rawCommand(device,"DRVEN=0",dummy);
      return CMD_ERR;
    }

    if(atoi(argbuf)<0) {
      sprintf(filter_selected,"MOVR -DIST");
    } else {
      sprintf(filter_selected,"MOVR DIST");
    }

    step_count=abs(atoi(argbuf));
    for(int j=1;j<=step_count;j++) {
      ierr =rawCommand(device,filter_selected,dummy);
      for(i=0;i<40000;i++) {
	rawCommand(device,"PRINT MVG",dummy);
	if(!strcasecmp(dummy,"FALSE")) break;
	MilliSleep(100);
      }
    }

    filter_pos=mlcBitsBase10(device,"PRINT IO 23,IO 22,IO 21",dummy)+1;

    shm_addr->MODS.pos[device]=(float)filter_pos;
    shm_addr->MODS.reqpos[device]=(float)filter_pos;

    if(who_selected[0]=='R') {
      sprintf(reply,"%s %s=%d FILTNAME='%s'",who_selected,who_selected,
	      filter_pos, shm_addr->MODS.rcamfilters[filter_pos]);

    } else if(who_selected[0]=='B') {
      sprintf(reply,"%s %s=%d FILTNAME='%s'",who_selected,who_selected,
	      filter_pos, shm_addr->MODS.bcamfilters[filter_pos]);
    }

    rawCommand(device,"DRVEN=0",dummy);

    return CMD_OK;


  } else if (!strcasecmp(cmd_instruction,"RDBITS")) {

    filter_pos=mlcBitsBase10(device,"PRINT IO 23 IO 22,IO 21",dummy)+1;
    rawCommand(device,"PRINT IO 24,IO 23 IO 22,IO 21",dummy);

    sprintf(reply,"%s %s=%d BITS=%s b24-b21",
	    who_selected,who_selected,filter_pos,dummy);

  } else if (filterval>=0 && filterval<8) { 

    rawCommand(device,"PRINT IO 24",dummy);
    if(!atoi(dummy)) {
      sprintf(reply,"%s %s=FAULT Mechanism out-of-position, in-position sensor not asserted. Reset %s to recover",who_selected,who_selected,who_selected);
      return CMD_ERR;
    }

    sprintf(filter_selected,"TARGNUM=%d",filterval);
    ierr = sendTwoCommand(device,filter_selected,"BEGIN",dummy);

    if(ierr!=0) {
      sprintf(reply,"%s %s=%s",who_selected,who_selected,dummy);
      return CMD_ERR;
    }

    MilliSleep(100);

    filter_pos=mlcBitsBase10(device,"PRINT IO 23,IO 22,IO 21",dummy)+1;

    if(load_filter<0)
      if(filterval+1!=filter_pos) {
	sprintf(reply,"%s %s=%d Move Fault, position at end of move %d but requested position %d",who_selected,who_selected,filter_pos,filter_pos,filterval+1);
	shm_addr->MODS.reqpos[device]=(float)filterval+1;
	return CMD_ERR;
      } else {

	if(who_selected[0]=='R') {
	  sprintf(reply,"%s %s=%d FILTNAME='%s'",who_selected,who_selected,
		  filter_pos, shm_addr->MODS.rcamfilters[filter_pos]);
	  
	} else if(who_selected[0]=='B') {
	  sprintf(reply,"%s %s=%d FILTNAME='%s'",who_selected,who_selected,
		  filter_pos, shm_addr->MODS.bcamfilters[filter_pos]);
	}

      } else 
      sprintf(reply,"%s LOAD filter %d",who_selected,load_filter+1);

    if(filter_pos < 0) return CMD_ERR;

    shm_addr->MODS.pos[device]=(float)(filter_pos);
    shm_addr->MODS.reqpos[device]=(float)filterval+1;
    
  } else {
    sprintf(reply,"%s Invalid request '%s', valid range is %d..%d",
	    who_selected,args,
	    (int)(shm_addr->MODS.min[device]),
	    (int)(shm_addr->MODS.max[device]));
    return CMD_ERR;
  }

return CMD_OK;
}

// cmd_camstep() - retired 2025 June

/* ---------------------------------------------------------------------------
//
// lamp - lamp WAGO command
//
*/

/*!  
  \brief lamp command - Operate the calibration lamps
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  LAMP [lamp1 lamp2 ...] [ON|OFF|INFO]

  lampN - name(s) of the calibration lamp(s) to operate

  ON - turn the selected lamp(s) on

  OFF - turn the selected lamp(s) off

  INFO - return information about the named lamp(s)

  \par Description:
  Select one or more calibration lamps and then either turn them on or off or 
  get information about them. The command arguments consist of a list of one 
  or more lamps to be operated, followed by a keyword giving the operation to 
  be performed. The calibration lamp names are as follows:
  
  Flat1 QTH1 flat field (continuum) lamp 1\n
  Flat2 QTH2 flat field (continuum) lamp 2\n
  Flat3 VFLAT flat field (continuum) lamp 3\n
  Ne Neon penray lamp\n
  Xe Xenon penray lamp\n
  Hg Mercury(Argon) penray lamp\n
  Ar Argon penray lamp\n
  Kr Krypton penray lamp

  The operations that can be performed on these lamps are as follows:\n
  ON Turn the named lamp(s) ON. 1 or more lamps must be named.\n
  OFF Turn the named lamp(s) OFF. If no lamps named, turns off all MODS.lamps.\n
  INFO Return information about the named lamps(s)

  If the INFO operation is requested, it returns the following information 
  vector about named lamp "LampID":\n 
  LampID=(State,TimeOn,Age,Cycles)\n

  \par where:
  State current power state of the lamp (On or Off)\n
  TimeOn elapsed time in seconds the lamp has been lit if On, or 0 if Off\n
  Age total time the lamp has been powered on in since installation (seconds)\n
  Cycles number of times the lamp has been power cycled since installation\n
  Both OFF and INFO have implicit "all" options if no lamp names are given 
  on the command line, thus\n 
  
  LAMP OFF turns off all lamps\n
  LAMP INFO returns information about all lamps\n

  Finally, if LAMP is given with no arguments, it reports the names of all 
  lamps that are currently on.

  \par Interactions:
  An executive override is required to turn on any lamp if the calibration 
  tower is out of the beam and/or the dark hatch is open. However, lamps may 
  be turned off and their info queried at any time without regard to the dark 
  hatch or calibration tower status.

*/
int
cmd_lamp(char *args, MsgType msgtype, char *reply)
{
  // b1  AR    ON/OFF  0000000000000001 1       
  // b2  XE    ON/OFF  0000000000000010 2       
  // b3  NE    ON/OFF  0000000000000100 4
  // b4  HG    ON/OFF  0000000000001000 8
  // b5  KR    ON/OFF  0000000000010000 16
  // b6  QTH6V ON/OFF  0000000000100000 32
  // b11 QTH1  ON/OFF  0000010000000000 1024
  // b12 QTH2  ON/OFF  0000100000000000 2048
  // b13 VFLAT ON/OFF  0001000000000000 4096

  int i, j, k, nc, lamp;
  int ierr, lampcnt;
  char temp[9][6];
  char get_buff[80];
  char dummy[PAGE_SIZE];
  char onOff[PAGE_SIZE];
  short val[1];
  short vflatShort;
  time_t t[9]; 
  int numArgs;
  int indexLamp;
  time_t telapse; 
  char who_selected[24];
  char stateOnOff[10];
  char vflatSet[10];
  float vflatVal;
  float tempVal;
  float vflatPower;
  int vfRegOut;
  int vflatPRet;
  float vflatPOut;
  
  strcpy(who_selected,cmdtab[commandID].cmd);
  StrUpper(who_selected);

  GetArg(args,1,stateOnOff);
  StrUpper(stateOnOff);

  numArgs=0;

  llbID=getWagoID("llb",dummy); // Get Lamp/Laser Box(LLB) ID
  if(llbID==-1) {
    sprintf(reply,"%s %s",who_selected,dummy);
    return CMD_ERR;
  }

  if (!strcasecmp(stateOnOff,"CONFIG")) { // get mechanism ip
    sprintf(reply,"%s IP=%s",who_selected,shm_addr->MODS.WAGOIP[llbID]);
    return CMD_OK;
  }

  if(!strcasecmp(stateOnOff,"STATUS")) {
    printf(who_selected,"LAMP");

    ierr = wagoSetGet(0,shm_addr->MODS.WAGOIP[llbID],1,LLBONOFF,devOnOff,1);
    if(ierr==-1) {
      sprintf(reply,"%s LLB=PWROFF",who_selected);

      for(i=0;i<9;i++) {  // Reset Shared Memory address has been cleared
	if(shm_addr->MODS.lamps.lamp_state[i]==1) {
	  shm_addr->MODS.lamps.lamp_state[i]=0; // reset state to zero(0)
	  shm_addr->MODS.lamps.lamp_cycle[i]++; // increment cycle
	}
      }
      return CMD_OK;
    } else 
    sprintf(reply,"%s LLB=ON",who_selected);
    return CMD_OK;
  }

  ierr = wagoSetGet(0,shm_addr->MODS.WAGOIP[llbID],1,LLBONOFF,devOnOff,1);
  if (ierr<0) {
    sprintf(reply,"%s LLB=OFF CALLAMPS='None' MODS LLB is powered off, LAMP command unavailable",who_selected);
    return CMD_ERR;
  } 

  memset(reply,0,sizeof(reply));
  if (strlen(args)<=0) { // Query when no command is issued
    sprintf(who_selected,"LAMP");

    ierr = wagoSetGet(0,shm_addr->MODS.WAGOIP[llbID],1,VFLATPSET,devOnOff,1);
    vflatPRet = devOnOff[0];
    vflatPOut=-2.033972 + (0.000480 * (float)vflatPRet);
    
    if(vflatPOut < 0.0) vflatPOut = 0.0;
    shm_addr->MODS.vflat_power = vflatPOut;

    ierr=wagoSetGet(0,shm_addr->MODS.WAGOIP[llbID],1,LLBONOFF,devOnOff,1);

    if(!ierr) {   // Reset Shared Memory if address has been cleared
      for(i=0;i<9;i++) {
	if(shm_addr->MODS.lamps.lamp_state[i]==1) {
	  shm_addr->MODS.lamps.lamp_state[i]=0; // reset state to zero(0)
	  shm_addr->MODS.lamps.lamp_cycle[i]++; // increment cycle
	}
      }
      sprintf(reply,"%s CALLAMPS='None' VFLAT=%0.1f",who_selected, shm_addr->MODS.vflat_power);
      return CMD_OK;
    }      

    lampcnt=0;
    sprintf(reply,"%s CALLAMPS='",who_selected);	
    for(i=0;i<9;i++)
      if(shm_addr->MODS.lamps.lamp_state[i]==1) {
	strcat(reply,lamp_names[i]);
	strcat(reply," ");
	lampcnt++;
      }

    if(lampcnt==0) {
      sprintf(reply,"%s CALLAMPS='None' VFLAT=%0.1f",who_selected,shm_addr->MODS.vflat_power);
      return CMD_OK;
    }

    sprintf(reply,"%s' VFLAT=%0.1f",reply,shm_addr->MODS.vflat_power);
    return CMD_OK;
  }

  StrUpper(args);

  for(i = 0; i < 9; i++) memset(temp[i],0,sizeof temp[i]);

  for(i = 0, nc = 1, indexLamp = 1; args[i] != NULL; i++) {
    if(args[i]==' ') {
      GetArg(args,nc,temp[indexLamp]);
      if(!strcasecmp(temp[indexLamp],"VFLAT")) {
	nc++;
	GetArg(args,nc,dummy);
	if(strcasecmp(dummy,lamp_names[0]) &&
	   strcasecmp(dummy,lamp_names[1]) &&
	   strcasecmp(dummy,lamp_names[2]) &&
	   strcasecmp(dummy,lamp_names[3]) &&
	   strcasecmp(dummy,lamp_names[4]) &&
	   strcasecmp(dummy,lamp_names[5]) &&
	   strcasecmp(dummy,lamp_names[6]) &&
	   strcasecmp(dummy,lamp_names[7]) &&
	   strcasecmp(dummy,lamp_names[8]) &&
	   strcasecmp(dummy,"ON") &&
	   strcasecmp(dummy,"OFF")) {
	  //if(shm_addr->MODS.lamps.lamp_state[8]==0 && !strstr(args,"ON")) {
	    //sprintf(reply,"LAMP VFLAT intensity changed with VFLAT turned OFF");
	    //return CMD_ERR;
	  //}

	  tempVal=atof(dummy);
	  if (tempVal>11.0 || tempVal < 0.0) {
	    sprintf(reply,"LAMP invalid VFLAT intensity %.1f, must be 0..11, VFLAT=%0.1f",tempVal, shm_addr->MODS.vflat_power);
	    return CMD_ERR;
	  }

	  /*
	  // VFLAT setting register:
	  // vflatPower = 4237.0201 + 2083.1259 * vflatVal;
	  */
	  vflatVal=atof(dummy);

	  if(vflatVal>11) vflatPower=0.0;
	  else {
	    vflatPower=4237.0201 + 2083.1259 * vflatVal;
	    if(vflatVal<=0 || vflatPower<=4237.0) vflatPower=0.0;
	  }

	  vflatShort = (short)vflatPower;
	  ierr=wagoSetGet(1,shm_addr->MODS.WAGOIP[llbID],1,VFLATPSET,&vflatShort,1);
	  nc++;
	  indexLamp++;

	  MilliSleep(1000);
	  /*
	  // VFLAT getting and processing register:
	  // vflatPOut = -2.033972 + (0.000480 * vflatPRet)
	  */
	  ierr = wagoSetGet(0,shm_addr->MODS.WAGOIP[llbID],1,VFLATPSET,devOnOff,1);
	  vflatPRet = devOnOff[0];
	  vflatPOut=-2.033972 + (0.000480 * (float)vflatPRet);

	  if(vflatPOut < 0.0) vflatPOut = 0.0;
	  shm_addr->MODS.vflat_power = vflatPOut;

	  numArgs=2;
	} else {
	  indexLamp++;
	  GetArg(args,nc,temp[indexLamp]);
	  numArgs=1;
	}
      }
      indexLamp++;
      nc++;
    }
  }

  if (numArgs==1) {
    nc--;
    GetArg(args,nc,temp[nc]);
  }
  else if(numArgs==2) { 
    nc-=2;
    GetArg(args,nc,temp[nc]);
  }
  else GetArg(args,nc,temp[nc]);

  if (!strcasecmp(temp[1],"ON")) {
    sprintf(reply,"%s Invalid %s '%s', Usage: %s [AR|XE|NE|HG|KR|QTH6V|QTH1|QTH2|VFLAT|[VFLAT 0..11]] [ON|OFF|INFO]", 
	    who_selected, cmdtab[commandID].cmd, args, cmdtab[commandID].cmd);
    return CMD_ERR;
  }
  else if (!strcasecmp(temp[nc],"ON") || !strcasecmp(temp[nc],"OFF")) {

    if (!strcasecmp(temp[1],"OFF")) {
      shm_addr->MODS.lamps.lamplaser_all[0] &= 0x03C0; // Turn off all lamps
      ierr = wagoSetGet(1, shm_addr->MODS.WAGOIP[llbID], 1, LLBONOFF, &shm_addr->MODS.lamps.lamplaser_all[0], 1);

      for(i=0;i<9;i++) {
	if(shm_addr->MODS.lamps.lamp_state[i]==1) {
	  shm_addr->MODS.lamps.lamp_state[i]=0; // reset state to zero(0)
	  shm_addr->MODS.lamps.lamp_cycle[i]++; // increment cycle
	}
      }
      cmd_lamp("",EXEC,reply);
      return CMD_OK;
    }
    else {

      // Wago Address: old 40515 new 40517 - see LLBONOFF=516
      // LAMPS: (b)its
      // AR(b1), XE(b2), NE(b4), HG(b8), KR=(b16),QTH6V(b32),
      // QTH1(b1024), QTH2(b2048), VFLAT(b4096)
      //
      // LASERS: (b)its
      // VIS(b64)-ENABLE(b256), IR(b128)-ENABLE(b512)
      // Wago Address: VIS Laser Power (40512) was 40513
      // Wago Address: IR Laser Power (40513) was 40514

      for (j=0;j<nc;j++) {
	for (i=0,k=1;i<9;i++,k+=k) {
	  if (!strcasecmp(temp[j],lamp_names[i])) {
	    if (!strcasecmp(temp[nc],"ON")) {
	      // Turn the lamp ON
	      if (i<6) {
		// This is one of the spectral (pen-ray) lamps
		shm_addr->MODS.lamps.lamplaser_all[0] |= k;
		ierr = wagoSetGet(1,shm_addr->MODS.WAGOIP[llbID],1,LLBONOFF,&shm_addr->MODS.lamps.lamplaser_all[0],1);
		shm_addr->MODS.lamps.lamp_state[i]=1;
	      }
	      else {
		// This is one of the flat-field lamps (QTH1, QTH2, and VFLAT)
		if (i!=8) {
		  // one if the QTH lamps, turn on the QTH 6V power supply first
		  if(shm_addr->MODS.lamps.lamp_state[5]==0) {
		    shm_addr->MODS.lamps.lamplaser_all[0] |= 32;
		    ierr = wagoSetGet(1,shm_addr->MODS.WAGOIP[llbID],1,LLBONOFF,&shm_addr->MODS.lamps.lamplaser_all[0],1);
		    shm_addr->MODS.lamps.lamp_state[5]=1;
		  }
		  shm_addr->MODS.lamps.lamplaser_all[0] |= (k*16);
		  ierr=wagoSetGet(1,shm_addr->MODS.WAGOIP[llbID],1,LLBONOFF,&shm_addr->MODS.lamps.lamplaser_all[0],1);
		  shm_addr->MODS.lamps.lamp_state[i]=1;
		}
		else {
		  // This is the VFLAT lamp
		  shm_addr->MODS.lamps.lamplaser_all[0] |= (k*16);
		  ierr=wagoSetGet(1,shm_addr->MODS.WAGOIP[llbID],1,LLBONOFF,&shm_addr->MODS.lamps.lamplaser_all[0],1);
		  shm_addr->MODS.lamps.lamp_state[i]=1;
		}
	      }
	    }
	    else if (!strcasecmp(temp[nc],"OFF")) {
	      // Turn the lamp OFF
	      if (i<6) {
		// This is one of the spectral (pen-ray) lamps:
		shm_addr->MODS.lamps.lamplaser_all[0] &= (0x1FFF-k);
		ierr = wagoSetGet(1,shm_addr->MODS.WAGOIP[llbID],1,LLBONOFF,&shm_addr->MODS.lamps.lamplaser_all[0],1);

		// 6V power supply is turned *OFF*, Turn *OFF* all flats
		if (k==32) {
		  shm_addr->MODS.lamps.lamplaser_all[0] &= (0x1FFF-0x1C00);
		  ierr=wagoSetGet(1,shm_addr->MODS.WAGOIP[llbID],1,LLBONOFF,&shm_addr->MODS.lamps.lamplaser_all[0],1);
		  for(int l=5;l<9;l++) {
		    shm_addr->MODS.lamps.lamp_state[l] = 0;
		    shm_addr->MODS.lamps.lamp_cycle[l]++;
		  }
		}
		else {
		  shm_addr->MODS.lamps.lamp_state[i] = 0;
		  shm_addr->MODS.lamps.lamp_cycle[i]++;
		}
	      }
	      else { 
		// This is for the QTH(s) k needs some adjustment.
		// first turn off the 6 volt power supply if not on already

		if (i!=8) {
		  shm_addr->MODS.lamps.lamplaser_all[0] &= 0x1FFF-(k*16);
		  ierr = wagoSetGet(1,shm_addr->MODS.WAGOIP[llbID],1,LLBONOFF,&shm_addr->MODS.lamps.lamplaser_all[0],1);
		  shm_addr->MODS.lamps.lamp_state[i]=0;
		  shm_addr->MODS.lamps.lamp_cycle[i]++;
		
		  if (shm_addr->MODS.lamps.lamp_state[6]==0 && shm_addr->MODS.lamps.lamp_state[7]==0) {
		    shm_addr->MODS.lamps.lamplaser_all[0] &= 0x1FDF;
		    ierr=wagoSetGet(1,shm_addr->MODS.WAGOIP[llbID],1,LLBONOFF,&shm_addr->MODS.lamps.lamplaser_all[0],1);
		    shm_addr->MODS.lamps.lamp_state[5]=0;
		    shm_addr->MODS.lamps.lamp_cycle[5]++;
		  }
		}
		else {
		  shm_addr->MODS.lamps.lamplaser_all[0] &= 0x1FFF-(k*16);
		  ierr = wagoSetGet(1,shm_addr->MODS.WAGOIP[llbID],1,LLBONOFF,&shm_addr->MODS.lamps.lamplaser_all[0],1);
		  shm_addr->MODS.lamps.lamp_state[i]=0;
		  shm_addr->MODS.lamps.lamp_cycle[i]++;
		}
	      }
	    }
	  }
	}
      }
    }
  }

  if(!strcasecmp(temp[nc],"INFO")) {
    if(nc==1) {
      memset(get_buff,0,sizeof(get_buff));
      for(j=0, i=0;i<9; i++) {
	sprintf(get_buff,"%s %s=(%s,%d,%d,%d) ",who_selected,lamp_names[i],
		(shm_addr->MODS.lamps.lamp_state[i]==1 ? "ON" : "OFF"),
		shm_addr->MODS.lamps.lamp_timeon[i],
		shm_addr->MODS.lamps.lamp_age[i],
		shm_addr->MODS.lamps.lamp_cycle[i]);
	strcat(reply,get_buff);
      }
    } else {
      memset(get_buff,0,sizeof(get_buff));
      for(j=0;j<nc;j++) {
	for(i=0;i<9; i++) {
	  if(!strcasecmp(temp[j],lamp_names[i])) {
	    sprintf(get_buff,"%s %s=(%s,%d,%d,%d) ",who_selected,lamp_names[i],
		    (shm_addr->MODS.lamps.lamp_state[i]==1 ? "ON" : "OFF"),
		    shm_addr->MODS.lamps.lamp_timeon[i],
		    shm_addr->MODS.lamps.lamp_age[i],
		    shm_addr->MODS.lamps.lamp_cycle[i]);
	    strcat(reply,get_buff);
	    j++;
	  }
	}
      }
    }
  } else {
    
    for(i=0; i<9; i++)
      if(!strcasecmp(temp[1],lamp_names[i])) break;
    
    if(i>=9) {
      sprintf(reply,"%s Invalid request '%s', Usage: lamp [AR|XE|NE|HG|KR|QTH6V|QTH1|QTH2|VFLAT|[VFLAT 0..11]] [ON|OFF|INFO]",who_selected, args);
      
      return CMD_ERR;
    }

    //if(!strcasecmp(temp[nc],"ON") || !strcasecmp(temp[nc],"OFF")) {
      
      lampcnt=0;
      sprintf(reply,"%s CALLAMPS='",who_selected);
      for(i=0;i<9;i++) {
	if(shm_addr->MODS.lamps.lamp_state[i]==1) {
	  strcat(reply,lamp_names[i]);
	  strcat(reply," ");
	  lampcnt++;
	}
      }

      if(lampcnt==0) {
	sprintf(reply,"%s CALLAMPS='None' VFLAT=%0.1f",who_selected,shm_addr->MODS.vflat_power);
	return CMD_OK;
      }

      sprintf(reply,"%s' VFLAT=%0.1f",reply,shm_addr->MODS.vflat_power);
  }

  return CMD_OK;
}

/* ---------------------------------------------------------------------------
//
// vislaser - vislaser WAGO command
//
*/

/*!  
  \brief vislaser command -  Operate the IMCS Visible-light Alignment Laser
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  VISLASER [pon|poff] [enable|disable] [power [lev]] [status|info]

        pon|poff - Turn the visible laser unit's main power on or off
  enable|disable - Enable/disable the laser output (beam "on/off")
     power [lev] - Set/query the laser power output level
          status - Query the laser's current operating status
            info - Query laser system information

  \par Description:
  Sets or queries the current state of the visible-light red laser used for 
  optical alignment.  This is an engineering laser that is co-aligned with 
  the Infrared reference laser used by the Image Motion Compensation System
  (IMCS), but otherwise it is not used during normal observing operations.

  As with the IR laser unit, there are two power settings:
  1.	Main power to the laser controller electronics
  2.	Power to the laser head, which enables light output
  These power functions are provided by this command as follows\n\n

  VISLASER [pon|poff]\n

  Turns on the main power to the visible laser control unit.  This powers 
  up the main laser control electronics but does not turn on the laser 
  itself (no light out the fiber).\n

  VISLASER [enable|disable]\n

  Enables or disables laser light output to the fiber.  To turn output off 
  without powering down the laser controller, "VISLASER disable" is used.

  Additional command options are used to change the light output power, 
  change the temperature controller settings, and request status 
  information of the laser system proper.  
  These functions are as follows:\n

  VISLASER power [lev]\n

  Sets the current laser output power to "lev", or returns the current 
  output power setting if given without the lev argument.  If laser output 
  is currently disabled, it will set the power level that will be used when 
  laser output is next enabled.\n

  VISLASER status\n

  Returns a status string with the current status of the laser, giving the 
  power state of the controller, a flag that indicates if the laser is 
  enabled or disabled, and the power level.\n

  Examples:\n
  Command>	VISLASER\n
  Response>	DONE: VISLASER=ON VISBEAM=ENABLED VISPSET=100 VISPOUT=100\n

  This command just reports the current status of the Visible Laser system.  
  See the keyword listing for the meaning of each parameter above.\n

  Command>	VISLASER disable\n
  Response>	DONE: VISLASER=ON VISBEAM=DISABLED ...\n

  This disabled the visible laser output, but keeps the main power on and 
  the various controller settings as is.

  \par Interactions:
  The visible-light laser system is an engineering-only facility, and in 
  normal operation the unit should be always powered off and disabled.\n
  No observer-level commands should be provided for VISLASER control.

*/

int
cmd_vislaser(char *args, MsgType msgtype, char *reply)
{

  /*
  // b7 Vis Laser ON/OFF  0000000001000000 64
  // b9 Vis Laser Enable  0000000100000000 256
  */
  float vispower;
  int what[4];
  char temp[6];
  char get_buff[80];
  char who_selected[24];
  char dummy[PAGE_SIZE];
  int regPSet;
  int regPOut;
  int ierr;
  int numArgs;
  short vispowerShort;
  float reqRegPOut;

  strcpy(who_selected,cmdtab[commandID].cmd);
  StrUpper(who_selected);

  llbID=getWagoID("llb",dummy); // Get Lamp/Laser Box(LLB) ID
  if(llbID==-1) {
    sprintf(reply,"%s %s",who_selected,dummy);
    return CMD_ERR;
  }

  if (strlen(args)<=0) { 
    sprintf(who_selected,"VISLASER");

    /*
    // Query when no command is issued
    // Get value for Visable Laser Power Out
    */
    ierr = wagoSetGet(0,shm_addr->MODS.WAGOIP[llbID],1,VISPOUTWAGO,devOnOff,1);
    regPOut = devOnOff[0];

    ierr=wagoSetGet(0,shm_addr->MODS.WAGOIP[llbID],1,LLBONOFF,devOnOff,1);
    if(!ierr) { // Reset Shared Memory if address has been cleared
      if(shm_addr->MODS.lasers.vislaser_state==1)
	shm_addr->MODS.lasers.vislaser_state=0;
      if(shm_addr->MODS.lasers.visbeam_state==1)
	shm_addr->MODS.lasers.visbeam_state=0;
      shm_addr->MODS.lasers.vislaser_cycle++;
    }
    
    shm_addr->MODS.lasers.vislaser_power = findPoly((float)regPOut, &shm_addr->MODS.lasers.vislaserRegToPOutCoeff[0], shm_addr->MODS.lasers.vislaserRegToPOutNCoeff);

    /* 
    // Get value for Visable Laser Power Set Point
    */
    ierr = wagoSetGet(0,shm_addr->MODS.WAGOIP[llbID],1,VISPSETWAGO,devOnOff,1);
    regPSet = devOnOff[0];
    shm_addr->MODS.lasers.vislaser_setpoint = findPoly((float)regPSet, &shm_addr->MODS.lasers.vislaserRegToPSetCoeff[0], shm_addr->MODS.lasers.vislaserRegToPSetNCoeff);

    if (shm_addr->MODS.lasers.vislaser_power < 0) shm_addr->MODS.lasers.vislaser_power = 0;

    sprintf(reply,"%s VISPSET=%.2f VISPOUT=%.2f VISLASER=%s VISBEAM=%s RAWVISPSET=%d RAWVISPOUT=%d",who_selected,
	    shm_addr->MODS.lasers.vislaser_setpoint,
	    shm_addr->MODS.lasers.vislaser_power,
	    (shm_addr->MODS.lasers.vislaser_state==1 ? "ON" : "OFF"),
	    (shm_addr->MODS.lasers.visbeam_state==1 ? "ENABLED" : "DISABLED"),
	    regPSet, regPOut);

    return CMD_OK;
  }

  numArgs = sscanf(args,"%s %f", temp, &vispower);

  if(!strcasecmp(temp,"ON")) {
    /*
    // if the laser is off, turn it on
    */
    if (shm_addr->MODS.lasers.vislaser_state == 0) {
      shm_addr->MODS.lamps.lamplaser_all[0] |=  64;
      ierr = wagoSetGet(1,shm_addr->MODS.WAGOIP[llbID],1,LLBONOFF,
			&shm_addr->MODS.lamps.lamplaser_all[0],1);
      
      shm_addr->MODS.lasers.vislaser_state=1;
      shm_addr->MODS.lasers.visbeam_state=0;
    }

    /*
    // if the user supplies "vislaser ON pwrLevel" go to power setting 
    // specified.
    */
     if (numArgs == 2) {
       if(vispower >= shm_addr->MODS.lasers.vislaser_minPower && 
          vispower <= shm_addr->MODS.lasers.vislaser_maxPower) {

	 /*
	 // Convert the user-input vispower (in mW) to a register value in DN
	 */
	 reqRegPOut = findPoly(vispower, &(shm_addr->MODS.lasers.vislaserPSetToRCoeff[0]), shm_addr->MODS.lasers.vislaserPSetToRNCoeff);

	 if (reqRegPOut < 0) reqRegPOut = 0;
	 sleep(2);

	 /* 
	 // Convert the returned value, in DN, to a short data type for WAGO
	 // and then send the value to WAGO
	 */
	 vispowerShort = (short)reqRegPOut;
	 ierr = wagoSetGet(1,shm_addr->MODS.WAGOIP[llbID],1,VISPSETWAGO,
			   &vispowerShort,1);
	 
	 sleep(2);

	 /*
	 // Get the reported POut and PSet from the WAGO and store the returned
	 // value, converted to physical units, in the shared memory
	 */
	 ierr = wagoSetGet(0,shm_addr->MODS.WAGOIP[llbID],1,VISPOUTWAGO,devOnOff,1);
	 regPOut = devOnOff[0];
	 shm_addr->MODS.lasers.vislaser_power = findPoly((float)regPOut, &shm_addr->MODS.lasers.vislaserRegToPOutCoeff[0], shm_addr->MODS.lasers.vislaserRegToPOutNCoeff);

	 if (shm_addr->MODS.lasers.vislaser_power < 0) shm_addr->MODS.lasers.vislaser_power = 0;

	 ierr = wagoSetGet(0,shm_addr->MODS.WAGOIP[llbID],1,VISPSETWAGO,devOnOff,1);
	 regPSet = devOnOff[0];
	 shm_addr->MODS.lasers.vislaser_setpoint = findPoly((float)regPSet, &shm_addr->MODS.lasers.vislaserRegToPSetCoeff[0], shm_addr->MODS.lasers.vislaserRegToPSetNCoeff);
	 
       } else {
	 sprintf(reply,"%s Invalid request '%s %.2f', valid range is %.2f to %.2f mW", 
		 who_selected,temp,vispower,
		 shm_addr->MODS.lasers.vislaser_minPower,
		 shm_addr->MODS.lasers.vislaser_maxPower);
	 return CMD_ERR;
       }
     }

     if (shm_addr->MODS.lasers.vislaser_power < 0) shm_addr->MODS.lasers.vislaser_power = 0;

     sprintf(reply,"%s VISLASER=%s VISBEAM=%s VISPSET=%.2f VISPOUT=%.2f",
	     who_selected,
	     (shm_addr->MODS.lasers.vislaser_state==1 ? "ON" : "OFF"),
	     (shm_addr->MODS.lasers.visbeam_state==1 ? "ENABLED" : "DISABLED"),
	     shm_addr->MODS.lasers.vislaser_setpoint,
	     shm_addr->MODS.lasers.vislaser_power);
     
  } else if(!strcasecmp(temp,"OFF")) {
    shm_addr->MODS.lamps.lamplaser_all[0] &= 0x1EBF;
    ierr = wagoSetGet(1,shm_addr->MODS.WAGOIP[llbID],1,LLBONOFF,
		      &shm_addr->MODS.lamps.lamplaser_all[0],1);

    shm_addr->MODS.lasers.vislaser_state=0;
    shm_addr->MODS.lasers.visbeam_state=0;
    shm_addr->MODS.lasers.vislaser_cycle++;
    
    if (shm_addr->MODS.lasers.vislaser_power < 0) shm_addr->MODS.lasers.vislaser_power = 0;
    
    sprintf(reply,"%s VISLASER=%s VISBEAM=%s VISPSET=%.2f VISPOUT=%.2f",
	    who_selected,
	    (shm_addr->MODS.lasers.vislaser_state==1 ? "ON" : "OFF"),
	    (shm_addr->MODS.lasers.visbeam_state==1 ? "ENABLED" : "DISABLED"),
	    shm_addr->MODS.lasers.vislaser_setpoint,
	    shm_addr->MODS.lasers.vislaser_power);
    
  } else if(!strcasecmp(temp,"ENABLE")) {
    if (shm_addr->MODS.lasers.vislaser_state == 1) {
        if (shm_addr->MODS.lasers.visbeam_state == 0) {  

	  /*
	  // 'Press' the enable/disable switch
	  */
	  shm_addr->MODS.lamps.lamplaser_all[0] |= 256;
	  ierr = wagoSetGet(1,shm_addr->MODS.WAGOIP[llbID],1,LLBONOFF,
			    &shm_addr->MODS.lamps.lamplaser_all[0],1);
	  
	  MilliSleep(500); // Hold the switch down for 0.5 sec 

	  /*
	  // 'Release' the enable/disable switch
	  */
	  shm_addr->MODS.lamps.lamplaser_all[0] &= 0x1EFF;
	  ierr = wagoSetGet(1,shm_addr->MODS.WAGOIP[llbID],1,LLBONOFF,
			    &shm_addr->MODS.lamps.lamplaser_all[0],1);
	  /*
	  // Set the Visible Laser Enable/Disable state variable in shared 
	  // memory
	  */
	  shm_addr->MODS.lasers.visbeam_state=1;
	}
	/*
	// if the user supplies "vislaser ENABLE pwrLevel" go to power 
	// setting specified
	*/
	if (numArgs == 2) {
	  if(vispower >= shm_addr->MODS.lasers.vislaser_minPower && 
	     vispower <= shm_addr->MODS.lasers.vislaser_maxPower) {
	    /*
	    // Convert the user-input vispower (in mW) to a register value 
	    // in DN
	    */
	    reqRegPOut = findPoly(vispower, &(shm_addr->MODS.lasers.vislaserPSetToRCoeff[0]), shm_addr->MODS.lasers.vislaserPSetToRNCoeff);

	    if (reqRegPOut < 0) reqRegPOut = 0;
	    sleep(2);
	  
	    /*
	    // Convert the returned value, in DN, to a short data type for WAGO
	    // and then send the value to WAGO
	    */
	    vispowerShort = (short)reqRegPOut;
	    ierr = wagoSetGet(1,shm_addr->MODS.WAGOIP[llbID],1,VISPSETWAGO,
			      &vispowerShort,1);
	    sleep(2);
	    /*
	    // Get the reported POut and PSet from the WAGO and store the
	    // return value, converted to physical units, in the shared memory
	    */
	    ierr = wagoSetGet(0,shm_addr->MODS.WAGOIP[llbID],1,VISPOUTWAGO,devOnOff,1);
	    regPOut = devOnOff[0];
	    shm_addr->MODS.lasers.vislaser_power = findPoly((float)regPOut, &shm_addr->MODS.lasers.vislaserRegToPOutCoeff[0], shm_addr->MODS.lasers.vislaserRegToPOutNCoeff);

	    if (shm_addr->MODS.lasers.vislaser_power < 0) shm_addr->MODS.lasers.vislaser_power = 0;

	    ierr = wagoSetGet(0,shm_addr->MODS.WAGOIP[llbID],1,VISPSETWAGO,devOnOff,1);
	    regPSet = devOnOff[0];
	    shm_addr->MODS.lasers.vislaser_setpoint = findPoly((float)regPSet, &shm_addr->MODS.lasers.vislaserRegToPSetCoeff[0], shm_addr->MODS.lasers.vislaserRegToPSetNCoeff);

	  } else {
	    sprintf(reply,"%s Invalid request '%s %.2f', valid range is %.2f to %.2f mW", 
		    who_selected,temp,vispower,
		    shm_addr->MODS.lasers.vislaser_minPower,
		    shm_addr->MODS.lasers.vislaser_maxPower);
	    return CMD_ERR;
	  }
	}

        if (shm_addr->MODS.lasers.vislaser_power < 0) shm_addr->MODS.lasers.vislaser_power = 0;
	
        sprintf(reply,"%s VISBEAM=%s VISLASER=%s VISPSET=%.2f VISPOUT=%.2f",
		who_selected,
		(shm_addr->MODS.lasers.visbeam_state==1 ? "ENABLED" : "DISABLED"),
		(shm_addr->MODS.lasers.vislaser_state==1 ? "ON" : "OFF"),
		shm_addr->MODS.lasers.vislaser_setpoint,
		shm_addr->MODS.lasers.vislaser_power);
    } else {
      if (shm_addr->MODS.lasers.vislaser_power < 0) shm_addr->MODS.lasers.vislaser_power = 0;
      
      sprintf(reply, "%s Visible Laser is %s, cannot enable the beam VISLASER=%s VISBEAM=%s",who_selected,
	      shm_addr->MODS.lasers.vislaser_state==1 ? "ON" : "OFF",
	      shm_addr->MODS.lasers.vislaser_state==1 ? "ON" : "OFF",
	      shm_addr->MODS.lasers.visbeam_state==1 ? "ENABLED" : "DISABLED");
      return CMD_ERR;
    }
    
  } else if(!strcasecmp(temp,"DISABLE")) {
    if (shm_addr->MODS.lasers.vislaser_state == 1) {
      if (shm_addr->MODS.lasers.visbeam_state == 1) {  
	/*
	// 'Press' the enable/disable switch
	*/
	shm_addr->MODS.lamps.lamplaser_all[0] |= 256;
	ierr = wagoSetGet(1,shm_addr->MODS.WAGOIP[llbID],1,LLBONOFF,
			  &shm_addr->MODS.lamps.lamplaser_all[0],1);

	MilliSleep(500); // Hold the switch down for 0.5 sec 
	/*
	// 'Release' the enable/disable switch
	*/
	shm_addr->MODS.lamps.lamplaser_all[0] &= 0x1EFF;
	ierr = wagoSetGet(1,shm_addr->MODS.WAGOIP[llbID],1,LLBONOFF,
			  &shm_addr->MODS.lamps.lamplaser_all[0],1);
	/*
	// Set the Visible Laser Enable/Disable state variable in shared memory
	*/
	shm_addr->MODS.lasers.visbeam_state=0;
      }
      if (shm_addr->MODS.lasers.vislaser_power < 0) shm_addr->MODS.lasers.vislaser_power = 0;

      sprintf(reply,"%s VISBEAM=%s VISLASER=%s VISPSET=%.2f VISPOUT=%.2f",
	      who_selected,
	      (shm_addr->MODS.lasers.visbeam_state==1 ? "ENABLED" : "DISABLED"),
	      (shm_addr->MODS.lasers.vislaser_state==1 ? "ON" : "OFF"),
	      shm_addr->MODS.lasers.vislaser_setpoint,
	      shm_addr->MODS.lasers.vislaser_power);
    } else {
      if (shm_addr->MODS.lasers.vislaser_power < 0) shm_addr->MODS.lasers.vislaser_power = 0;
      
      sprintf(reply, "%s Visible Laser is %s, cannot disable the beam VISLASER=%s VISBEAM=%s",who_selected,
	      shm_addr->MODS.lasers.vislaser_state==1 ? "ON" : "OFF",
	      shm_addr->MODS.lasers.vislaser_state==1 ? "ON" : "OFF",
	      shm_addr->MODS.lasers.visbeam_state==1 ? "ENABLED" : "DISABLED");
      return CMD_ERR;
    }
    //shm_addr->MODS.lasers.vislaser_cycle++;

  } else if(!strcasecmp(temp,"RAWPOWER") && numArgs == 2) {
    vispowerShort = (short)vispower;
    ierr = wagoSetGet(1,shm_addr->MODS.WAGOIP[llbID],1,VISPSETWAGO,
		      &vispowerShort,1);
    
    sleep(2);
    /*
    // Get the reported POut and PSet from the WAGO and store the returned
    // value, converted to physical units, in the shared memory
    */
    ierr = wagoSetGet(0,shm_addr->MODS.WAGOIP[llbID],1,VISPOUTWAGO,devOnOff,1);
    regPOut = devOnOff[0];
    shm_addr->MODS.lasers.vislaser_power = findPoly((float)regPOut, &shm_addr->MODS.lasers.vislaserRegToPOutCoeff[0], shm_addr->MODS.lasers.vislaserRegToPOutNCoeff);
    
    ierr = wagoSetGet(0,shm_addr->MODS.WAGOIP[llbID],1,VISPSETWAGO,devOnOff,1);
    regPSet = devOnOff[0];
    shm_addr->MODS.lasers.vislaser_setpoint = findPoly((float)regPSet,	&shm_addr->MODS.lasers.vislaserRegToPSetCoeff[0], shm_addr->MODS.lasers.vislaserRegToPSetNCoeff);
    
    if (shm_addr->MODS.lasers.vislaser_power < 0) shm_addr->MODS.lasers.vislaser_power = 0;

    sprintf(reply,"%s VISPSET=%.2f VISPOUT=%.2f VISLASER=%s VISBEAM=%s RAWVISPSET=%d RAWVISPOUT=%d",who_selected,
	    shm_addr->MODS.lasers.vislaser_setpoint,
	    shm_addr->MODS.lasers.vislaser_power,
	    (shm_addr->MODS.lasers.vislaser_state==1 ? "ON" : "OFF"),
	    (shm_addr->MODS.lasers.visbeam_state==1 ? "ENABLED" : "DISABLED"),
	    regPSet, regPOut);
    
  } else if(!strcasecmp(temp,"POWER")) {
    /*
    // if the user writes "vislaser power" without any other argument
    */
    if(numArgs != 2) {
      /*
      // Get value for Visable Laser Power Out
      */
      ierr = wagoSetGet(0,shm_addr->MODS.WAGOIP[llbID],1,VISPOUTWAGO,devOnOff,1);
      regPOut = devOnOff[0];
      shm_addr->MODS.lasers.vislaser_power = findPoly((float)regPOut, &shm_addr->MODS.lasers.vislaserRegToPOutCoeff[0], shm_addr->MODS.lasers.vislaserRegToPOutNCoeff);

      /* 
      // Get value for Visable Laser Power Set Point
      */
      ierr = wagoSetGet(0,shm_addr->MODS.WAGOIP[llbID],1,VISPSETWAGO,devOnOff,1);
      regPSet = devOnOff[0];
      shm_addr->MODS.lasers.vislaser_setpoint = findPoly((float)regPSet, &shm_addr->MODS.lasers.vislaserRegToPSetCoeff[0], shm_addr->MODS.lasers.vislaserRegToPSetNCoeff);
      
      if (shm_addr->MODS.lasers.vislaser_power < 0) shm_addr->MODS.lasers.vislaser_power = 0;
      
      sprintf(reply,"%s VISPSET=%.2f VISPOUT=%.2f VISLASER=%s VISBEAM=%s RAWVISPSET=%d RAWVISPOUT=%d",who_selected,
	      shm_addr->MODS.lasers.vislaser_setpoint,
	      shm_addr->MODS.lasers.vislaser_power,
	      (shm_addr->MODS.lasers.vislaser_state==1 ? "ON" : "OFF"),
	      (shm_addr->MODS.lasers.visbeam_state==1 ? "ENABLED" : "DISABLED"),
	      regPSet, regPOut);
	      
    } else {
      /*
      // if the user writes "vislaser power x", where x is a desired power 
      // level in mW
      */
      if(vispower >= shm_addr->MODS.lasers.vislaser_minPower && 
         vispower <= shm_addr->MODS.lasers.vislaser_maxPower) {
	/*
        // Convert the user-input vispower (in mW) to a register value in DN
	*/
        reqRegPOut = findPoly(vispower,	&(shm_addr->MODS.lasers.vislaserPSetToRCoeff[0]), shm_addr->MODS.lasers.vislaserPSetToRNCoeff);

        if (reqRegPOut < 0) reqRegPOut = 0;
	/*
        // Convert the returned value, in DN, to a short data type for WAGO
        // and then send the value to WAGO
	*/
	vispowerShort = (short)reqRegPOut;
	ierr = wagoSetGet(1,shm_addr->MODS.WAGOIP[llbID],1,VISPSETWAGO,
			  &vispowerShort,1);

	sleep(2);

	/*
        // Get the reported POut and PSet from the WAGO and store the returned
        // value, converted to physical units, in the shared memory
	*/
	ierr = wagoSetGet(0,shm_addr->MODS.WAGOIP[llbID],1,VISPOUTWAGO,devOnOff,1);
	regPOut = devOnOff[0];
	shm_addr->MODS.lasers.vislaser_power = findPoly((float)regPOut, &shm_addr->MODS.lasers.vislaserRegToPOutCoeff[0], shm_addr->MODS.lasers.vislaserRegToPOutNCoeff);

	ierr = wagoSetGet(0,shm_addr->MODS.WAGOIP[llbID],1,VISPSETWAGO,devOnOff,1);
	regPSet = devOnOff[0];
	shm_addr->MODS.lasers.vislaser_setpoint = findPoly((float)regPSet, &shm_addr->MODS.lasers.vislaserRegToPSetCoeff[0], shm_addr->MODS.lasers.vislaserRegToPSetNCoeff);

	if (shm_addr->MODS.lasers.vislaser_power < 0) shm_addr->MODS.lasers.vislaser_power = 0;
	
        sprintf(reply,"%s VISPSET=%.2f VISPOUT=%.2f VISLASER=%s VISBEAM=%s rawVISPSET=%d rawVISPOUT=%d",who_selected,
		shm_addr->MODS.lasers.vislaser_setpoint,
		shm_addr->MODS.lasers.vislaser_power,
		(shm_addr->MODS.lasers.vislaser_state==1 ? "ON" : "OFF"),
		(shm_addr->MODS.lasers.visbeam_state==1 ? "ENABLED" : "DISABLED"),
		regPSet, regPOut);
      } else {
	sprintf(reply,"%s Invalid request '%s %.2f', valid range is %.2f to %.2f mW", 
		who_selected,temp,vispower,
                shm_addr->MODS.lasers.vislaser_minPower,
                shm_addr->MODS.lasers.vislaser_maxPower);
	return CMD_ERR;
      }
      return CMD_OK;
    }
    
  } else if(!strcasecmp(temp,"STATUS")) {
    /* 
    // Get value for Visable Laser Power Out
    */
    ierr = wagoSetGet(0,shm_addr->MODS.WAGOIP[llbID],1,VISPOUTWAGO,devOnOff,1);
    regPOut = devOnOff[0];
    shm_addr->MODS.lasers.vislaser_power = findPoly((float)regPOut, &shm_addr->MODS.lasers.vislaserRegToPOutCoeff[0], shm_addr->MODS.lasers.vislaserRegToPOutNCoeff);

    /* 
    // Get value for Visable Laser Power Set Point
    */
    ierr = wagoSetGet(0,shm_addr->MODS.WAGOIP[llbID],1,VISPSETWAGO,devOnOff,1);
    regPSet = devOnOff[0];
    shm_addr->MODS.lasers.vislaser_setpoint = findPoly((float)regPSet,	&shm_addr->MODS.lasers.vislaserRegToPSetCoeff[0], shm_addr->MODS.lasers.vislaserRegToPSetNCoeff);

    if (shm_addr->MODS.lasers.vislaser_power < 0) shm_addr->MODS.lasers.vislaser_power = 0;

    sprintf(reply,"%s VISPSET=%.2f VISPOUT=%.2f VISLASER=%s VISBEAM=%s RAWVISPSET=%d RAWVISPOUT=%d",who_selected,
	    shm_addr->MODS.lasers.vislaser_setpoint,
	    shm_addr->MODS.lasers.vislaser_power,
	    (shm_addr->MODS.lasers.vislaser_state==1 ? "ON" : "OFF"),
	    (shm_addr->MODS.lasers.visbeam_state==1 ? "ENABLED" : "DISABLED"),
	    regPSet, regPOut);
    
  } else if(!strcasecmp(temp,"?")) {
    sprintf(reply,"%s [ON|OFF|ENABLE|DISABLE|POWER|STATUS|RESET] - Control the status of the %s", who_selected,who_selected);
    
  } else if(!strcasecmp(temp,"RESET")) {
    shm_addr->MODS.lamps.lamplaser_all[0] &= 0x1EBF; // Turn the laser off
    ierr = wagoSetGet(1,shm_addr->MODS.WAGOIP[llbID],1,LLBONOFF,
		      &shm_addr->MODS.lamps.lamplaser_all[0],1);

    shm_addr->MODS.lasers.vislaser_state=0;
    shm_addr->MODS.lasers.visbeam_state=0;
    shm_addr->MODS.lasers.vislaser_cycle++;

    sleep(2);

    shm_addr->MODS.lamps.lamplaser_all[0] |=  64; // Turn the laser on
    ierr = wagoSetGet(1,shm_addr->MODS.WAGOIP[llbID],1,LLBONOFF,
   		      &shm_addr->MODS.lamps.lamplaser_all[0],1);

    shm_addr->MODS.lasers.vislaser_state=1;
    shm_addr->MODS.lasers.visbeam_state=0;
 
    vispower = 0; // Set the power setpoint to zero
    vispowerShort = (short)vispower;
    ierr = wagoSetGet(1,shm_addr->MODS.WAGOIP[llbID],1,VISPSETWAGO,
    		  &vispowerShort,1);

    sleep(2);

    /*
    // Get the reported POut and PSet from the WAGO and store the returned
    // value, converted to physical units, in the shared memory
    */
    ierr = wagoSetGet(0,shm_addr->MODS.WAGOIP[llbID],1,VISPOUTWAGO,devOnOff,1);
    regPOut = devOnOff[0];
    shm_addr->MODS.lasers.vislaser_power = findPoly((float)regPOut, &shm_addr->MODS.lasers.vislaserRegToPOutCoeff[0], shm_addr->MODS.lasers.vislaserRegToPOutNCoeff);

    ierr = wagoSetGet(0,shm_addr->MODS.WAGOIP[llbID],1,VISPSETWAGO,devOnOff,1);
    regPSet = devOnOff[0];
    shm_addr->MODS.lasers.vislaser_setpoint = findPoly((float)regPSet,	&shm_addr->MODS.lasers.vislaserRegToPSetCoeff[0], shm_addr->MODS.lasers.vislaserRegToPSetNCoeff);
 
    if (shm_addr->MODS.lasers.vislaser_power < 0) shm_addr->MODS.lasers.vislaser_power = 0;
    
    sprintf(reply,"%s VISPSET=%.2f VISPOUT=%.2f VISLASER=%s VISBEAM=%s RAWVISPSET=%d RAWVISPOUT=%d",who_selected,
	    shm_addr->MODS.lasers.vislaser_setpoint,
	    shm_addr->MODS.lasers.vislaser_power,
	    (shm_addr->MODS.lasers.vislaser_state==1 ? "ON" : "OFF"),
	    (shm_addr->MODS.lasers.visbeam_state==1 ? "ENABLED" : "DISABLED"),
	    regPSet, regPOut);
    
  } else {
    sprintf(reply,"%s Invalid request '%s', valid requests %s [ON|OFF|ENABLE|DISABLE|POWER|STATUS|RESET]", 
	    who_selected,temp,cmdtab[commandID].cmd);
    return CMD_ERR;
  }
  return CMD_OK;
  
}

/* ---------------------------------------------------------------------------
//
// irlaser - irlaser WAGO command
//
*/

/*!  
  \brief irlaser command - Operate the IMCS Infrared Reference Laser
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  IRLASER [on|off[pwr]] [enable|disable] [power [P]] [temp] [status|info]

        pon|poff - Turn the laser unit's main power on or off, with optional
  enable|disable - Enable/disable the laser output (beam "on/off")
       power [P] - Set/query the laser power output level(mW); P=0..2mW
            temp - Report the laser-head set-point and operating temperature(C)
          status - Query the laser's current operating status
            info - Query laser system information

  \par Description:
  Sets or queries the current state of the infrared InGaAs laser that provides 
  the reference for the Image Motion Compensation System(IMCS).

  \par Interactions:

*/
int
cmd_irlaser(char *args, MsgType msgtype, char *reply)
{
  float irpower;
  int what[4];
  char temp[6];
  int regPSet;
  int regPOut;
  int ierr;
  int numArgs;
  char get_buff[80];
  char who_selected[24];
  char dummy[PAGE_SIZE];
  short irpowerShort;
  float reqRegPOut;

  strcpy(who_selected,cmdtab[commandID].cmd);
  StrUpper(who_selected);

  llbID=getWagoID("llb",dummy); // Get Lamp/Laser Box(LLB) ID
  if(llbID==-1) {
    sprintf(reply,"%s %s",who_selected,dummy);
    return CMD_ERR;
  }

  if (strlen(args)<=0) { 

    sprintf(who_selected,"IRLASER");
    /* 
    // Query when no command is issued
    // Get value for IR Laser Temperature Set Point
    */
    ierr = wagoSetGet(0,shm_addr->MODS.WAGOIP[llbID],1,IRTSETWAGO,devOnOff,1);

     /* 
     // Reset Shared Memory if address has been cleared
     */
     if(!wagoSetGet(0,shm_addr->MODS.WAGOIP[llbID],1,LLBONOFF,devOnOff,1)){
       if(shm_addr->MODS.lasers.irlaser_state==1)
	 shm_addr->MODS.lasers.irlaser_state=0;
       if(shm_addr->MODS.lasers.irbeam_state==1)
	 shm_addr->MODS.lasers.irbeam_state=0;
       shm_addr->MODS.lasers.irlaser_cycle++;
     }

     shm_addr->MODS.lasers.irlaser_tempSet = findPoly((float)ierr, 
     		&shm_addr->MODS.lasers.irlaserRegToTSetCoeff[0], 
     		  shm_addr->MODS.lasers.irlaserRegToTSetNCoeff);
     
     /* 
     // Get value for IR Laser Temperature Out
     */
     ierr = wagoSetGet(0,shm_addr->MODS.WAGOIP[llbID],1,IRTOUTWAGO,devOnOff,1);
     shm_addr->MODS.lasers.irlaser_temp = findPoly((float)ierr, &shm_addr->MODS.lasers.irlaserRegToTOutCoeff[0], shm_addr->MODS.lasers.irlaserRegToTOutNCoeff);

     /* 
     // Get value for IR Laser Power Set Point
     */
     ierr = wagoSetGet(0,shm_addr->MODS.WAGOIP[llbID],1,IRPSETWAGO,devOnOff,1);
     regPSet = devOnOff[0];
     shm_addr->MODS.lasers.irlaser_setpoint = findPoly((float)regPSet,	&shm_addr->MODS.lasers.irlaserRegToPSetCoeff[0], shm_addr->MODS.lasers.irlaserRegToPSetNCoeff);

     /* 
     // Get value for IR Laser Power Out
     */	
     ierr = wagoSetGet(0,shm_addr->MODS.WAGOIP[llbID],1,IRPOUTWAGO,devOnOff,1);
     regPOut = devOnOff[0];
     shm_addr->MODS.lasers.irlaser_power = findPoly((float)regPOut, &shm_addr->MODS.lasers.irlaserRegToPOutCoeff[0], shm_addr->MODS.lasers.irlaserRegToPOutNCoeff);

     if (shm_addr->MODS.lasers.irlaser_power < 0) shm_addr->MODS.lasers.irlaser_power = 0;

     sprintf(reply,"%s IRPSET=%.1f IRPOUT=%.1f IRLASER=%s IRBEAM=%s IRTSET=%.1f IRTEMP=%.1f RAWIRPSET=%d RAWIRPOUT=%d",who_selected,
	     shm_addr->MODS.lasers.irlaser_setpoint,
	     shm_addr->MODS.lasers.irlaser_power,
	     (shm_addr->MODS.lasers.irlaser_state==1 ? "ON" : "OFF"),
	     (shm_addr->MODS.lasers.irbeam_state==1 ? "ENABLED" : "DISABLED"),
	     shm_addr->MODS.lasers.irlaser_tempSet,
	     shm_addr->MODS.lasers.irlaser_temp,
	     regPSet, regPOut);
     
     return CMD_OK;
  }

  numArgs = sscanf(args,"%s %f", temp, &irpower);

  /*
  // b8  IR Laser ON/OFF  0000000010000000 128
  // b10 IR Laser Enable  0000001000000000 512
  */
  StrUpper(temp);
  if(!strcasecmp(temp,"ON")) { // if the laser is off, turn it on
    if (shm_addr->MODS.lasers.irlaser_state == 0) {
        shm_addr->MODS.lamps.lamplaser_all[0] |=  128;
        ierr = wagoSetGet(1,shm_addr->MODS.WAGOIP[llbID],1,LLBONOFF,
            	      &shm_addr->MODS.lamps.lamplaser_all[0],1);

        shm_addr->MODS.lasers.irlaser_state=1;
        shm_addr->MODS.lasers.irbeam_state=0;
    }

    /*
    // if the user supplies "irlaser ON pwrLevel" go to power setting specified
    */
    if (numArgs == 2) {
      if(irpower >= shm_addr->MODS.lasers.irlaser_minPower && 
	 irpower <= shm_addr->MODS.lasers.irlaser_maxPower) {
	/*
	// Convert the user-input vispower (in mW) to a register value in DN
	*/
	reqRegPOut = findPoly(irpower, &(shm_addr->MODS.lasers.irlaserPSetToRCoeff[0]), shm_addr->MODS.lasers.irlaserPSetToRNCoeff);

	if (reqRegPOut < 0) reqRegPOut = 0;
	sleep(2);
	/*  
	// Convert the returned value, in DN, to a short data type for WAGO
	// and then send the value to WAGO
	*/
	irpowerShort = (short)reqRegPOut;
	ierr = wagoSetGet(1,shm_addr->MODS.WAGOIP[llbID],1,IRPSETWAGO,
			  &irpowerShort,1);

	sleep(2);
	/*
	// Get the reported POut and PSet from the WAGO and store the returned
	// value, converted to physical units, in the shared memory
	*/
	ierr = wagoSetGet(0,shm_addr->MODS.WAGOIP[llbID],1,IRPOUTWAGO,devOnOff,1);
	regPOut = devOnOff[0];
	shm_addr->MODS.lasers.irlaser_power = findPoly((float)regPOut,	&shm_addr->MODS.lasers.irlaserRegToPOutCoeff[0], shm_addr->MODS.lasers.irlaserRegToPOutNCoeff);

	ierr = wagoSetGet(0,shm_addr->MODS.WAGOIP[llbID],1,IRPSETWAGO,devOnOff,1);
	regPSet = devOnOff[0];
	shm_addr->MODS.lasers.irlaser_setpoint = findPoly((float)regPSet, &shm_addr->MODS.lasers.irlaserRegToPSetCoeff[0], shm_addr->MODS.lasers.irlaserRegToPSetNCoeff);

      } else {
	sprintf(reply,"%s Invalid request '%s %.2f', valid range is %.2f to %.2f mW", 
	        who_selected,temp,irpower,
		shm_addr->MODS.lasers.irlaser_minPower,
		shm_addr->MODS.lasers.irlaser_maxPower);
	return CMD_ERR;
      }
    }

    if (shm_addr->MODS.lasers.irlaser_power < 0) shm_addr->MODS.lasers.irlaser_power = 0;

    sprintf(reply,"%s IRLASER=%s IRBEAM=%s IRTSET=%.1f IRTEMP=%.1f IRPSET=%.1f IRPOUT=%.1f",who_selected,
	    (shm_addr->MODS.lasers.irlaser_state==1 ? "ON" : "OFF"),
	    (shm_addr->MODS.lasers.irbeam_state==1 ? "ENABLED" : "DISABLED"),
	    shm_addr->MODS.lasers.irlaser_tempSet,
	    shm_addr->MODS.lasers.irlaser_temp,
	    shm_addr->MODS.lasers.irlaser_setpoint,
	    shm_addr->MODS.lasers.irlaser_power);

  } else if(!strcasecmp(temp,"OFF")) {
    shm_addr->MODS.lamps.lamplaser_all[0] &= 0x1D7F;
    ierr = wagoSetGet(1,shm_addr->MODS.WAGOIP[llbID],1,LLBONOFF,
		      &shm_addr->MODS.lamps.lamplaser_all[0],1);

    shm_addr->MODS.lasers.irlaser_state=0;
    shm_addr->MODS.lasers.irbeam_state=0;

    shm_addr->MODS.lasers.irlaser_cycle++;

    if (shm_addr->MODS.lasers.irlaser_power < 0) shm_addr->MODS.lasers.irlaser_power = 0;

    sprintf(reply,"%s IRLASER=%s IRBEAM=%s IRTSET=%.1f IRTEMP=%.1f IRPSET=%.1f IRPOUT=%.1f",who_selected,
	    (shm_addr->MODS.lasers.irlaser_state==1 ? "ON" : "OFF"),
	    (shm_addr->MODS.lasers.irbeam_state==1 ? "ENABLED" : "DISABLED"),
	    shm_addr->MODS.lasers.irlaser_tempSet,
	    shm_addr->MODS.lasers.irlaser_temp,
	    shm_addr->MODS.lasers.irlaser_setpoint,
	    shm_addr->MODS.lasers.irlaser_power);

  } else if(!strcasecmp(temp,"ENABLE")) {
    if (shm_addr->MODS.lasers.irlaser_state == 1) {
      if (shm_addr->MODS.lasers.irbeam_state == 0) {
	/*
	// 'Press' the enable/disable switch
	*/
	shm_addr->MODS.lamps.lamplaser_all[0] |= 512;
	ierr = wagoSetGet(1,shm_addr->MODS.WAGOIP[llbID],1,LLBONOFF,
			  &shm_addr->MODS.lamps.lamplaser_all[0],1);
	
	MilliSleep(500); // Hold the switch down for 0.5 sec 
	/*
	// 'Release' the enable/disable switch
	*/
	shm_addr->MODS.lamps.lamplaser_all[0] &= 0x1DFF;
	ierr = wagoSetGet(1,shm_addr->MODS.WAGOIP[llbID],1,LLBONOFF,
			  &shm_addr->MODS.lamps.lamplaser_all[0],1);

	/*
	// Set the IR Laser Enable/Disable state variable in shared memory
	*/
	shm_addr->MODS.lasers.irbeam_state=1;
      }
      /*
      // if the user supplies "irlaser ENABLE pwrLevel" go to power 
      // setting specified
      */
      if (numArgs == 2) {
	if(irpower >= shm_addr->MODS.lasers.irlaser_minPower && 
	   irpower <= shm_addr->MODS.lasers.irlaser_maxPower) {
	  /*
	  // Convert the user-input vispower (in mW) to a register value in DN
	  */
	  reqRegPOut = findPoly(irpower, &(shm_addr->MODS.lasers.irlaserPSetToRCoeff[0]), shm_addr->MODS.lasers.irlaserPSetToRNCoeff);

	  if (reqRegPOut < 0) reqRegPOut = 0;
	  sleep(2);

	  /*
	  // Convert the returned value, in DN, to a short data type for WAGO
	  // and then send the value to WAGO
	  */
	  irpowerShort = (short)reqRegPOut;
	  ierr = wagoSetGet(1,shm_addr->MODS.WAGOIP[llbID],1,IRPSETWAGO,
			    &irpowerShort,1);
	  sleep(2);
	  /*
	  // Get the reported POut and PSet from the WAGO and store the
	  // returned value, converted to physical units, in the shared 
	  // memory
	  */
	  ierr = wagoSetGet(0,shm_addr->MODS.WAGOIP[llbID],1,IRPOUTWAGO,devOnOff,1);
	  regPOut = devOnOff[0];
	  shm_addr->MODS.lasers.irlaser_power = findPoly((float)regPOut, &shm_addr->MODS.lasers.irlaserRegToPOutCoeff[0], shm_addr->MODS.lasers.irlaserRegToPOutNCoeff);
	  
	  if (shm_addr->MODS.lasers.irlaser_power < 0) shm_addr->MODS.lasers.irlaser_power = 0;

	  ierr = wagoSetGet(0,shm_addr->MODS.WAGOIP[llbID],1,IRPSETWAGO,devOnOff,1);
	  regPSet = devOnOff[0];
	  shm_addr->MODS.lasers.irlaser_setpoint = findPoly((float)regPSet, &shm_addr->MODS.lasers.irlaserRegToPSetCoeff[0], shm_addr->MODS.lasers.irlaserRegToPSetNCoeff);

	} else {
	  sprintf(reply,"%s Invalid request '%s %.2f', valid range is %.2f to %.2f mW", 
		  who_selected,temp,irpower,
		  shm_addr->MODS.lasers.irlaser_minPower,
		  shm_addr->MODS.lasers.irlaser_maxPower);
	  return CMD_ERR;
	}
      }

      if (shm_addr->MODS.lasers.irlaser_power < 0) shm_addr->MODS.lasers.irlaser_power = 0;

      sprintf(reply,"%s IRBEAM=%s IRLASER=%s IRTSET=%.1f IRTEMP=%.1f IRPSET=%.1f IRPOUT=%.1f",who_selected,
	      (shm_addr->MODS.lasers.irbeam_state==1 ? "ENABLED" : "DISABLED"),
	      (shm_addr->MODS.lasers.irlaser_state==1 ? "ON" : "OFF"),
	      shm_addr->MODS.lasers.irlaser_tempSet,
	      shm_addr->MODS.lasers.irlaser_temp,
	      shm_addr->MODS.lasers.irlaser_setpoint,
	      shm_addr->MODS.lasers.irlaser_power);
    } else {
      if (shm_addr->MODS.lasers.irlaser_power < 0) shm_addr->MODS.lasers.irlaser_power = 0;
      
      sprintf(reply, "%s IR Laser is %s, cannot enable the beam IRLASER=%s IRBEAM=%s",who_selected,
	      shm_addr->MODS.lasers.irlaser_state==1 ? "ON" : "OFF",
	      shm_addr->MODS.lasers.irlaser_state==1 ? "ON" : "OFF",
	      shm_addr->MODS.lasers.irbeam_state==1 ? "ENABLED" : "DISABLED");

      return CMD_ERR;
    }

  } else if(!strcasecmp(temp,"DISABLE")) {
    if (shm_addr->MODS.lasers.irlaser_state == 1) {
      if (shm_addr->MODS.lasers.irbeam_state == 1) {
	/*
	// 'Press' the enable/disable switch
	*/
	shm_addr->MODS.lamps.lamplaser_all[0] |= 512;
	ierr = wagoSetGet(1,shm_addr->MODS.WAGOIP[llbID],1,LLBONOFF,
			  &shm_addr->MODS.lamps.lamplaser_all[0],1);

	MilliSleep(500); // Hold the switch down for 0.5 sec 
	/*
	// 'Release' the enable/disable switch
	*/
	shm_addr->MODS.lamps.lamplaser_all[0] &= 0x1DFF;
	ierr = wagoSetGet(1,shm_addr->MODS.WAGOIP[llbID],1,LLBONOFF,
			  &shm_addr->MODS.lamps.lamplaser_all[0],1);
	/*
	// Set the IR Laser Enable/Disable state variable in shared memory
	*/
	shm_addr->MODS.lasers.irbeam_state=0;
      }
      if (shm_addr->MODS.lasers.irlaser_power < 0) shm_addr->MODS.lasers.irlaser_power = 0;

      sprintf(reply,"%s IRBEAM=%s IRLASER=%s IRTSET=%.1f IRTEMP=%.1f IRPSET=%.1f IRPOUT=%.1f",who_selected,
	      (shm_addr->MODS.lasers.irbeam_state==1 ? "ENABLED" : "DISABLED"),
	      (shm_addr->MODS.lasers.irlaser_state==1 ? "ON" : "OFF"),
	      shm_addr->MODS.lasers.irlaser_tempSet,
	      shm_addr->MODS.lasers.irlaser_temp,
	      shm_addr->MODS.lasers.irlaser_setpoint,
	      shm_addr->MODS.lasers.irlaser_power);

    } else {
      if (shm_addr->MODS.lasers.irlaser_power < 0) shm_addr->MODS.lasers.irlaser_power = 0;

      sprintf(reply, "%s IR Laser is %s, cannot disable the beam IRLASER=%s IRBEAM=%s",who_selected,
	      shm_addr->MODS.lasers.irlaser_state==1 ? "ON" : "OFF",
	      shm_addr->MODS.lasers.irlaser_state==1 ? "ON" : "OFF",
	      shm_addr->MODS.lasers.irbeam_state==1 ? "ENABLED" : "DISABLED");

      return CMD_ERR;
    }
    //shm_addr->MODS.lasers.irlaser_cycle++;
    
  } else if(!strcasecmp(temp,"RAWPOWER") && numArgs == 2) {
    /*
    // Convert the returned value, in DN, to a short data type for WAGO
    // and then send the value to WAGO
    */
    irpowerShort = (short)irpower;
    ierr = wagoSetGet(1,shm_addr->MODS.WAGOIP[llbID],1,IRPSETWAGO,
		      &irpowerShort,1);

    sleep(2);
    /*
    // Get the reported POut and PSet from the WAGO and store the returned
    // value, converted to physical units, in the shared memory
    */
    ierr = wagoSetGet(0,shm_addr->MODS.WAGOIP[llbID],1,IRPOUTWAGO,devOnOff,1);
    regPOut = devOnOff[0];
    shm_addr->MODS.lasers.irlaser_power = findPoly((float)regPOut, &(shm_addr->MODS.lasers.irlaserRegToPOutCoeff[0]), shm_addr->MODS.lasers.irlaserRegToPOutNCoeff);

    ierr = wagoSetGet(0,shm_addr->MODS.WAGOIP[llbID],1,IRPSETWAGO,devOnOff,1);
    regPSet = devOnOff[0];
    shm_addr->MODS.lasers.irlaser_setpoint = findPoly((float)regPSet, &(shm_addr->MODS.lasers.irlaserRegToPSetCoeff[0]), shm_addr->MODS.lasers.irlaserRegToPSetNCoeff);

    if (shm_addr->MODS.lasers.irlaser_power < 0) shm_addr->MODS.lasers.irlaser_power = 0;


    sprintf(reply,"%s IRPSET=%.1f IRPOUT=%.1f IRLASER=%s IRBEAM=%s IRTSET=%.1f IRTEMP=%.1f RAWIRPSET=%d RAWIRPOUT=%d",who_selected,
            shm_addr->MODS.lasers.irlaser_setpoint,
	    shm_addr->MODS.lasers.irlaser_power,
	    (shm_addr->MODS.lasers.irlaser_state==1 ? "ON" : "OFF"),
	    (shm_addr->MODS.lasers.irbeam_state==1 ? "ENABLED" : "DISABLED"),
	    shm_addr->MODS.lasers.irlaser_tempSet,
	    shm_addr->MODS.lasers.irlaser_temp,
            regPSet, regPOut);

  } else if(!strcasecmp(temp,"POWER")) {
    /*
    // if the user writes "irlaser power" without any other argument
    */
    if(numArgs != 2) {
      /* 
      // Get value for IR Laser Temperature Set Point
      */
      ierr = wagoSetGet(0,shm_addr->MODS.WAGOIP[llbID],1,IRTSETWAGO,devOnOff,1);
      shm_addr->MODS.lasers.irlaser_tempSet = findPoly((float)ierr, &shm_addr->MODS.lasers.irlaserRegToTSetCoeff[0], shm_addr->MODS.lasers.irlaserRegToTSetNCoeff);
	
      /* 
      // Get value for IR Laser Temperature Out
      */
      ierr = wagoSetGet(0,shm_addr->MODS.WAGOIP[llbID],1,IRTOUTWAGO,devOnOff,1);
      shm_addr->MODS.lasers.irlaser_temp = findPoly((float)ierr, &shm_addr->MODS.lasers.irlaserRegToTOutCoeff[0], shm_addr->MODS.lasers.irlaserRegToTOutNCoeff);

      /* 
      // Get value for IR Laser Power Set Point
      */
      ierr = wagoSetGet(0,shm_addr->MODS.WAGOIP[llbID],1,IRPSETWAGO,devOnOff,1);
      regPSet = devOnOff[0];
      shm_addr->MODS.lasers.irlaser_setpoint = findPoly((float)regPSet, &shm_addr->MODS.lasers.irlaserRegToPSetCoeff[0], shm_addr->MODS.lasers.irlaserRegToPSetNCoeff);

      /* 
      // Get value for IR Laser Power Out
      */	
      ierr = wagoSetGet(0,shm_addr->MODS.WAGOIP[llbID],1,IRPOUTWAGO,devOnOff,1);
      regPOut = devOnOff[0];
      
      shm_addr->MODS.lasers.irlaser_power = findPoly((float)regPOut,	&shm_addr->MODS.lasers.irlaserRegToPOutCoeff[0], shm_addr->MODS.lasers.irlaserRegToPOutNCoeff);
 
      if (shm_addr->MODS.lasers.irlaser_power < 0) shm_addr->MODS.lasers.irlaser_power = 0;
      
      sprintf(reply,"%s IRPSET=%.1f IRPOUT=%.1f IRLASER=%s IRBEAM=%s IRTSET=%.1f IRTEMP=%.1f RAWIRPSET=%d RAWIRPOUT=%d",who_selected,
	      shm_addr->MODS.lasers.irlaser_setpoint,
	      shm_addr->MODS.lasers.irlaser_power,
	      (shm_addr->MODS.lasers.irlaser_state==1 ? "ON" : "OFF"),
	      (shm_addr->MODS.lasers.irbeam_state==1 ? "ENABLED" : "DISABLED"),
	      shm_addr->MODS.lasers.irlaser_tempSet,
	      shm_addr->MODS.lasers.irlaser_temp,
	      regPSet, regPOut);

    } else {
      /*
      // if the user sends "irlaser power x", where x is a desired power 
      // level in mW
      */
      if(irpower >= shm_addr->MODS.lasers.irlaser_minPower && 
         irpower <= shm_addr->MODS.lasers.irlaser_maxPower) {
	/*
        // Convert the user-input irpower (in mW) to a register value in DN
	*/
        irpower = findPoly((float)irpower, &shm_addr->MODS.lasers.irlaserPSetToRCoeff[0], shm_addr->MODS.lasers.irlaserPSetToRNCoeff);

	/*
        // Convert the returned value, in DN, to a short data type for WAGO
        // and then send the value to WAGO
	*/
	irpowerShort = (short)irpower;
	ierr = wagoSetGet(1,shm_addr->MODS.WAGOIP[llbID],1,IRPSETWAGO,
			  &irpowerShort,1);
	sleep(2);

	/*
        // Get the reported POut and PSet from the WAGO and store the returned
        // value, converted to physical units, in the shared memory
	*/
	ierr = wagoSetGet(0,shm_addr->MODS.WAGOIP[llbID],1,IRPOUTWAGO,devOnOff,1);
	regPOut = devOnOff[0];
	shm_addr->MODS.lasers.irlaser_power = findPoly((float)regPOut,	&(shm_addr->MODS.lasers.irlaserRegToPOutCoeff[0]), shm_addr->MODS.lasers.irlaserRegToPOutNCoeff);

	ierr = wagoSetGet(0,shm_addr->MODS.WAGOIP[llbID],1,IRPSETWAGO,devOnOff,1);
	regPSet = devOnOff[0];
	shm_addr->MODS.lasers.irlaser_setpoint = findPoly((float)regPSet, &(shm_addr->MODS.lasers.irlaserRegToPSetCoeff[0]), shm_addr->MODS.lasers.irlaserRegToPSetNCoeff);
 
        if (shm_addr->MODS.lasers.irlaser_power < 0) shm_addr->MODS.lasers.irlaser_power = 0;

        sprintf(reply,"%s IRPSET=%.1f IRPOUT=%.1f IRLASER=%s IRBEAM=%s IRTSET=%.1f IRTEMP=%.1f",who_selected,
		shm_addr->MODS.lasers.irlaser_setpoint,
		shm_addr->MODS.lasers.irlaser_power,
		(shm_addr->MODS.lasers.irlaser_state==1 ? "ON" : "OFF"),
		(shm_addr->MODS.lasers.irbeam_state==1 ? "ENABLED" : "DISABLED"),
		shm_addr->MODS.lasers.irlaser_tempSet,
		shm_addr->MODS.lasers.irlaser_temp);

      } else {
	sprintf(reply,"%s Invalid request '%s %.2f', valid range is %.2f to %.2f mW", 
                who_selected,temp,irpower,
                shm_addr->MODS.lasers.irlaser_minPower,
                shm_addr->MODS.lasers.irlaser_maxPower);

	return CMD_ERR;
      }
    }

  } else if(!strcasecmp(temp,"STATUS")) {
    if (shm_addr->MODS.lasers.irlaser_power < 0) shm_addr->MODS.lasers.irlaser_power = 0;

    sprintf(reply,"%s IRLASER=%s IRBEAM=%s IRTSET=%.1f IRTEMP=%.1f IRPSET=%.1f IRPOUT=%.1f",who_selected,
	    (shm_addr->MODS.lasers.irlaser_state==1 ? "ON" : "OFF"),
	    (shm_addr->MODS.lasers.irbeam_state==1 ? "ENABLED" : "DISABLED"),
	    shm_addr->MODS.lasers.irlaser_tempSet,
	    shm_addr->MODS.lasers.irlaser_temp,
	    shm_addr->MODS.lasers.irlaser_setpoint,
	    shm_addr->MODS.lasers.irlaser_power);

  } else if(!strcasecmp(temp,"?")) {
    sprintf(reply,"%s [ON|OFF|ENABLE|DISABLE|POWER|STATUS|RESET] - Control the status of the %s", who_selected,who_selected);
    
  } else if(!strcasecmp(temp,"RESET")) { 
    shm_addr->MODS.lamps.lamplaser_all[0] &= 0x1D7F; // Turn the laser off
    ierr = wagoSetGet(1,shm_addr->MODS.WAGOIP[llbID],1,LLBONOFF,
		      &shm_addr->MODS.lamps.lamplaser_all[0],1);

    shm_addr->MODS.lasers.irlaser_state=0;
    shm_addr->MODS.lasers.irbeam_state=0;
    
    shm_addr->MODS.lasers.irlaser_cycle++;
    sleep(2);
    
    shm_addr->MODS.lamps.lamplaser_all[0] |=  128; // Turn the laser on
    ierr = wagoSetGet(1,shm_addr->MODS.WAGOIP[llbID],1,LLBONOFF,
        	      &shm_addr->MODS.lamps.lamplaser_all[0],1);

    shm_addr->MODS.lasers.irlaser_state=1;
    shm_addr->MODS.lasers.irbeam_state=0;
    
    
    irpowerShort = (short)irpower; // Set the power setpoint to zero
    ierr = wagoSetGet(1,shm_addr->MODS.WAGOIP[llbID],1,IRPSETWAGO,
		      &irpowerShort,1);
    sleep(2);

    /*
    // Get everything back and report to the user
    // Get value for IR Laser Temperature Set Point
    */
    ierr = wagoSetGet(0,shm_addr->MODS.WAGOIP[llbID],1,IRTSETWAGO,devOnOff,1);
    shm_addr->MODS.lasers.irlaser_tempSet = findPoly((float)ierr, &shm_addr->MODS.lasers.irlaserRegToTSetCoeff[0], shm_addr->MODS.lasers.irlaserRegToTSetNCoeff);
    /*    
    // Get value for IR Laser Temperature Out 
    */
    ierr = wagoSetGet(0,shm_addr->MODS.WAGOIP[llbID],1,IRTOUTWAGO,devOnOff,1);
    shm_addr->MODS.lasers.irlaser_temp = findPoly((float)ierr,	&shm_addr->MODS.lasers.irlaserRegToTOutCoeff[0], shm_addr->MODS.lasers.irlaserRegToTOutNCoeff);

    /*
    // Get value for IR Laser Power Set Point
    */
    ierr = wagoSetGet(0,shm_addr->MODS.WAGOIP[llbID],1,IRPSETWAGO,devOnOff,1);
    regPSet = devOnOff[0];
    shm_addr->MODS.lasers.irlaser_setpoint = findPoly((float)regPSet, &shm_addr->MODS.lasers.irlaserRegToPSetCoeff[0], shm_addr->MODS.lasers.irlaserRegToPSetNCoeff);

    /*
    // Get value for IR Laser Power Out
    */
    ierr = wagoSetGet(0,shm_addr->MODS.WAGOIP[llbID],1,IRPOUTWAGO,devOnOff,1);
    regPOut = devOnOff[0];
    shm_addr->MODS.lasers.irlaser_power = findPoly((float)regPOut, &shm_addr->MODS.lasers.irlaserRegToPOutCoeff[0], shm_addr->MODS.lasers.irlaserRegToPOutNCoeff);

    if (shm_addr->MODS.lasers.irlaser_power < 0) shm_addr->MODS.lasers.irlaser_power = 0;
    
    sprintf(reply,"%s IRPSET=%.1f IRPOUT=%.1f IRLASER=%s IRBEAM=%s IRTSET=%.1f IRTEMP=%.1f RAWIRPSET=%d RAWIRPOUT=%d",who_selected,
	    shm_addr->MODS.lasers.irlaser_setpoint,
	    shm_addr->MODS.lasers.irlaser_power,
	    (shm_addr->MODS.lasers.irlaser_state==1 ? "ON" : "OFF"),
	    (shm_addr->MODS.lasers.irbeam_state==1 ? "ENABLED" : "DISABLED"),
	    shm_addr->MODS.lasers.irlaser_tempSet,
	    shm_addr->MODS.lasers.irlaser_temp,
	    regPSet, regPOut);
    
  } else {
    sprintf(reply,"%s Invalid request '%s', valid requests %s [ON|OFF|ENABLE|DISABLE|POWER|STATUS|RESET]", 
	    who_selected,temp,cmdtab[commandID].cmd);

    return CMD_ERR;
  }
  
  return CMD_OK;
}

// cmd_sy() retired 2025 June [rwp/osu]

/* ---------------------------------------------------------------------------
//
// abort - abort one or all mechanism motion
//
*/

/*!  
  \brief ABORT command - abort all one or more mechanisms
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  abort [who|all]

  Immediately stops one(who) or all mechanism motion and holds the current position
  (?sets the brakes?).


  \par Lock Behavior:
  ABORT - hit the panic button and override.
*/
int
cmd_abort(char *args, MsgType msgtype, char *reply)
{
  int i;
  int len;
  int ierr;
  int device;
  char who_selected[24];
  char dummy[PAGE_SIZE];
  char cmd_instruction[PAGE_SIZE];

  memset(cmd_instruction,0,sizeof(cmd_instruction));

  strcpy(who_selected,cmdtab[commandID].cmd);
  StrUpper(who_selected);

  GetArg(args,1,cmd_instruction);
  device=getMechanismID(cmd_instruction,dummy); // Get mechanism device ID
  if(device==-1) {
    sprintf(reply,"%s %s",who_selected,dummy);
    return CMD_ERR;
  }

  StrUpper(cmd_instruction);

  if(!strcasecmp(cmd_instruction,"XALL")) {
    /* 1-4 are the AGW mechanisms. Do not abort them!*/
    if (shm_addr->MODS.busy[0]==0 && shm_addr->MODS.host[0]==1) {
      ierr = mlcStopMechanism(0,dummy);
    }

    for(i=5;i<MAX_ML-1;i++) {
      if (shm_addr->MODS.busy[0]==0 && shm_addr->MODS.host[0]==1) {
	ierr = mlcStopMechanism(i,dummy);
      }
    }
  sprintf(reply,"MODS=ABORT, An ABORT was issued to all mechanism except AGW!");
  } else {
    ierr = mlcStopMechanism(device,dummy);
    sprintf(reply,"%s %s",cmd_instruction,dummy);
  }

  return CMD_OK;
  
}

/* ---------------------------------------------------------------------------
//
//  power65 - Turn on/off the 65 VDC Power Supply
//
*/

/*!  
  \brief POWER65 command - Turn on/off 65 VDC power supply
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  power65 [ON|OFF|READ]

  Turn off the Power Supply gracefully.

  \par Lock Behavior:
*/

int
cmd_power65(char *args, MsgType msgtype, char *reply)
{
  int ierr;
  int device;
  char who_selected[24];
  char dummy[PAGE_SIZE];

  strcpy(who_selected,cmdtab[commandID].cmd);
  StrUpper(who_selected);

  ieb1ID=getWagoID("ieb1",dummy); // Get Lamp/Laser Box(LLB) ID

  StrUpper(args);

  GetArg(args,1,argbuf);
  
  if(!strcasecmp(argbuf,"READ")) {
    ierr = wagoSetGet(0,shm_addr->MODS.WAGOIP[ieb1ID],1,513,devOnOff,1);
    if(!ierr)
      sprintf(reply,"65 DVC power supply is ON, POWER65=ON");
    else {
      sprintf(reply,"65 DVC power supply is OFF, POWER65=OFF");
      return CMD_ERR;
    }

  } else if(!strcasecmp(argbuf,"OFF")) {
    ierr = wagoSetGet(1,shm_addr->MODS.WAGOIP[ieb1ID],1,513,(short *)1,1);

    sprintf(reply,"[%d]65 DVC power supply is OFF",ierr);

  } else if(!strcasecmp(argbuf,"ON")) {
    ierr = wagoSetGet(1,shm_addr->MODS.WAGOIP[ieb1ID],1,513,(short *)0,1);
    sprintf(reply,"[%d]65 DVC power supply is ON",ierr);

  } else {
    sprintf(reply,"%s Invalid POWER65 '%s', Usage: power65 [on|off|read]",who_selected,ierr);
    return CMD_ERR;
  }

  return CMD_OK;

}

/* ---------------------------------------------------------------------------
//
//  ieb - Instrument Electronix Box(IEB) Interface
//
*/

/*!  
  \brief ieb command - IEB interface
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  ieb [status|power|temp|mlc[1..16]] [n] [on|off]

  status = Read the current status

  power = IEB n power ON full or OFF standby

  temp = Read the current IEB internal temperature

  mlc = MicroLynx Controller 

  n = IEB ID or MicroLynx Controll ID

  on = Turn ON the power

  off = Turn OFF the power

  \par Description:
  IEB Interface

  \par Lock Behavior:
*/

int
cmd_ieb(char *args, MsgType msgtype, char *reply)
{
  int ierr, ierr2;
  int device;
  int ieb_id;
  int cmd;
  int mlcmask=0x1;
  short onoff[1];
  short i;
  char dummy[PAGE_SIZE];
  char cmd_instruction[PAGE_SIZE];
  char iebID;
  int  iebIDval;
  char mlcID[12];
  char power_only[3];
  char whatTemp[3];
  int bits[17]={0,1,2,4,8,16,32,64,128,256,512,1024,2048,4096,8192,16384,32768};
  short int bitval;
  char who_selected[24];

  float vdrive;
  float idrive;
  float vcontrol;

  memset(cmd_instruction,0,sizeof(cmd_instruction));
  strcpy(who_selected,cmdtab[commandID].cmd);
  StrUpper(who_selected);
  memset(dummy,0,sizeof(dummy));

  GetArg(args,1,cmd_instruction);
  StrUpper(cmd_instruction);

  if(!strcasecmp(cmd_instruction,"RED") || !strcasecmp(cmd_instruction,"R") || !strcasecmp(cmd_instruction,"1")) {
    iebIDval=1; 
    iebID='R';

  } else if(!strcasecmp(cmd_instruction,"BLUE") || !strcasecmp(cmd_instruction,"B") || !strcasecmp(cmd_instruction,"2")) {
    iebIDval=2; 
    iebID='B';

  } else {
    sprintf(reply,"%s Invalid request '%s', Usage: IEB [R|B|RED|BLUE|1|2]", who_selected,cmd_instruction);
    return CMD_ERR;
  }

  if(!strcasecmp(who_selected,"IEB")) {
    GetArg(args,2,argbuf);
    StrUpper(argbuf);

    if(iebIDval==0 || iebIDval>2) {
      sprintf(reply,"%s IEB_%c=UNKNOWN no such IEB",who_selected,iebID);
      return CMD_ERR;
    }

    GetArg(args,3,power_only);
    StrUpper(power_only);

    if(!strcasecmp(argbuf,"STATUS") || strlen(argbuf)<=0 ||
       (!strcasecmp(argbuf,"POWER") && strlen(power_only)<=0)) {

      if(strlen(argbuf)<=0) {

	wagoRW(iebIDval,"IEBS",0,0,dummy);
	sprintf(reply,"%s %s",who_selected,dummy);

      } else {
	ierr = wagoRW(iebIDval,"IEBS",0,0,dummy);
	sprintf(reply,"%s %s",who_selected,dummy);

	if(!ierr) {
	  wagoRW(iebIDval,"MLCS",0,0,dummy);
	  sprintf(reply,"%s %s",reply,dummy);
	} else if(ierr==1) {
	  wagoRW(iebIDval,"MLCS",0,0,dummy);
	  sprintf(reply,"%s %s",reply,dummy);
	} else if(ierr==-1) {
	  return CMD_ERR;
	}
	wagoRW(iebIDval,"TEMPS",0,0,dummy);
	sprintf(reply,"%s %s",reply,dummy);

	wagoRW(iebIDval,"IVS",0,0,dummy);
	sprintf(reply,"%s %s",reply,dummy);

	return CMD_OK;
      }
    } else if(!strcasecmp(argbuf,"GLYCOL")) {
      GetArg(args,1,&iebID);
      iebID = toupper(iebID);

      ieb_id=iebIDval-1;

      ierr = wagoSetGet(0,shm_addr->MODS.WAGOIP[ieb_id],1,514,(short *)atoi(argbuf),1);

      if(ierr&=0x2)
	sprintf(reply,"IEB GLYCOL=NOFLOW");
      else
	sprintf(reply,"IEB GLYCOL=FLOW");

    } else if(!strcasecmp(argbuf,"MPOWER")) {
      GetArg(args,3,argbuf);
      StrUpper(argbuf);
      GetArg(args,1,&iebID);
      iebID = toupper(iebID);
      ieb_id=iebIDval-1;

      if(!strcasecmp(argbuf,"ON")) {
	onoff[0]=0; // Turn on the 65V power supply
	onoff[1]=1;
	ierr = wagoSetGet(1,shm_addr->MODS.WAGOIP[ieb_id],1,514,&onoff[0],1);

	ierr = wagoSetGet(0,shm_addr->MODS.WAGOIP[ieb_id],1,514,(short *)atoi(argbuf),1);

	if(!ierr)
	  sprintf(reply,"IEB MPOWER_%c=ON",iebID);
	else
	  sprintf(reply,"IEB MPOWER_%c=OFF",iebID);

      } else if(!strcasecmp(argbuf,"OFF")) {
	onoff[0]=1; // Turn off the 65V power supply
	onoff[1]=1;
	ierr = wagoSetGet(1,shm_addr->MODS.WAGOIP[ieb_id],1,514,&onoff[0],1);

	ierr = wagoSetGet(0,shm_addr->MODS.WAGOIP[ieb_id],1,514,(short *)atoi(argbuf),1);

	if(!ierr)
	  sprintf(reply,"IEB IEB_%c=OFF",iebID);
	else
	  sprintf(reply,"IEB IEB_%c=OFF",iebID);
	
      } else if(!strcasecmp(argbuf,"STATUS") || (strlen(argbuf)<=0)) {
	wagoRW(iebIDval,"MPOWER",0,0,dummy);
	sprintf(reply,"%s%s",who_selected,dummy);

      } else if(!strcasecmp(argbuf,"NSTATUS") || (strlen(argbuf)<=0)) {
	wagoRW(iebIDval,"BYNAME",0,0,dummy);
	sprintf(reply,"%s%s",who_selected,dummy);

      } else {
	sprintf(reply,"%s Invalid request '%s', Usage: ieb [n] power [on|off]",who_selected,args);
      }
    } else if(!strcasecmp(argbuf,"TEMP")) {
      memset(whatTemp,0,sizeof(whatTemp));
      GetArg(args,3,whatTemp);
      StrUpper(whatTemp);

      if(atoi(whatTemp)<=0) {
	wagoRW(iebIDval,"TEMPS",0,0,dummy);
	sprintf(reply,"%s %s",who_selected,dummy);

      } else if(atoi(whatTemp)<=4) {
	wagoRW(iebIDval,"TEMPS",0,atoi(whatTemp),dummy);
	sprintf(reply,"%s %s",who_selected,dummy);

      } else {
	sprintf(reply,"%s Invalid request '%s', Usage: ieb [n] temp [1-4]", who_selected,whatTemp);
	return CMD_ERR;
      }
      return CMD_OK;

    } else if(!strcasecmp(argbuf,"COMTROL")) {
      sprintf(reply,"%s %s Not implemented Yet!",who_selected,argbuf);
      return CMD_OK;

    } else if(!strcasecmp(argbuf,"VDRIVE")) {
      GetArg(args,1,&iebID);
      iebID = toupper(iebID);
      ieb_id=iebIDval-1;
      ierr = wagoSetGet(0,shm_addr->MODS.WAGOIP[ieb_id],1,0,devOnOff,1);
      vdrive = ((float)(devOnOff[0])/pow(2,15)*10)*9.28;
      sprintf(reply,"%s %s_%c=%0.3f",who_selected,argbuf,iebID,vdrive);
      return CMD_OK;

    } else if(!strcasecmp(argbuf,"IDRIVE")) {
      GetArg(args,1,&iebID);
      iebID = toupper(iebID);
      ieb_id=iebIDval-1;
      ierr = wagoSetGet(0,shm_addr->MODS.WAGOIP[ieb_id],1,2,devOnOff,1);
      idrive = ((float)(devOnOff[0])/pow(2,15)*10)*1.25;
      sprintf(reply,"%s %s_%c=%0.3f",who_selected,argbuf,iebID,idrive);
      return CMD_OK;

    } else if(!strcasecmp(argbuf,"VCONTROL")) {
      GetArg(args,1,&iebID);
      iebID = toupper(iebID);
      ieb_id=iebIDval-1;
      ierr = wagoSetGet(0,shm_addr->MODS.WAGOIP[ieb_id],2,0,devOnOff,1);
      vcontrol = ((float)(devOnOff[0])/pow(2,15)*10)*3.12;
      sprintf(reply,"%s %s_%c=%0.3f",who_selected,argbuf,iebID,vcontrol);
      return CMD_OK;

    } else if(!strcasecmp(argbuf,"MLC")) {
      GetArg(args,3,mlcID);

      if(strlen(mlcID)<3) {
	cmd=atoi(mlcID); 
	if(cmd>18) {
	  sprintf(reply,"%s %s_%c=FAULT Invalid request '%d', valid range is 1..16", who_selected,who_selected,iebID,cmd);
	  return CMD_ERR;
	}
      } else {
	cmd=getMechanismID(mlcID,dummy); // Get mechanism device ID
	if(cmd==-1) {
	  sprintf(reply,"%s %s",who_selected,dummy);
	  return CMD_ERR;
	}

	if(iebID=='B' && cmd<18) {
	  sprintf(reply,"%s %s_%c=FAULT Invalid '%s' not found in BLUE IEB, Try the RED IEB",
		  who_selected,who_selected,iebID,mlcID);
	  return CMD_ERR;
	} 
	if(iebID=='R' && cmd>17) {
	  sprintf(reply,"%s %s_%c=FAULT Invalid '%s' not found in RED IEB, Try the BLUE IEB",
		  who_selected,who_selected,iebID,mlcID);
	  return CMD_ERR;
	}
	if(iebID=='R' || (int)iebID==1) cmd++;
	if(iebID=='B' || (int)iebID==2) cmd-=17;
      }
      GetArg(args,4,argbuf);
      StrUpper(argbuf);

      ieb_id=iebIDval-1;

      if(!strcasecmp(argbuf,"RESET")) {
	/* Turn it off */

	ierr = wagoSetGet(0,shm_addr->MODS.WAGOIP[ieb_id],1,512,devOnOff,1);
	onoff[0] = devOnOff[0];
	onoff[0] |= (short )bits[cmd];

	ierr = wagoSetGet(1,shm_addr->MODS.WAGOIP[ieb_id],1,512,onoff,1);

	MilliSleep(500); // Wait 500ms and let it adjust

	/*	
	// Now Turn it on 
	*/

	
	ierr = wagoSetGet(0,shm_addr->MODS.WAGOIP[ieb_id],1,512,devOnOff,1);
	onoff[0] = devOnOff[0];
	onoff[0]&=onoff[0] ^ (short)bits[cmd];
	ierr = wagoSetGet(1,shm_addr->MODS.WAGOIP[ieb_id],1,512,onoff,1);

	if(cmd==0 || cmd>18) {
	  sprintf(reply,"%s",who_selected);
	  for(i=1,ierr=1;ierr<=16;i+=i,ierr++) {
	    sprintf(reply,"%s MLC%d_%c=%s ",reply,ierr,
		    iebID,(!(i&onoff[0]) ? "ON" : "OFF"));
	  }
	}

      } else if(!strcasecmp(argbuf,"STATUS") || cmd==0) {

	if (wagoSetGet(0,shm_addr->MODS.WAGOIP[ieb_id],1,514,(short *)atoi(argbuf),1)) {
	  sprintf(reply,"%s IEB_%c=OFF",who_selected,iebID);
	  return CMD_ERR;
	}

	if(cmd==0) {
	  wagoRW(iebIDval,"MLCS",0,0,dummy);
	  sprintf(reply,"%s %s",who_selected,dummy);
	} else {
	  wagoRW(iebIDval,"MLCS",0,cmd,dummy);
	  sprintf(reply,"%s %s",who_selected,dummy);
	}

	return CMD_OK;

      } else if(!strcasecmp(argbuf,"ON")) {
	if(cmd>18) cmd+=18; // BLUE IEB starts at 18-33 in Shared Memory

	ierr = wagoSetGet(0,shm_addr->MODS.WAGOIP[ieb_id],1,512,devOnOff,1);
	onoff[0] = devOnOff[0];
	onoff[0]&=onoff[0] ^ (short)bits[cmd];
	ierr = wagoSetGet(1,shm_addr->MODS.WAGOIP[ieb_id],1,512,onoff,1);

	sprintf(reply,"IEB MLC%d_%c=ON",cmd,iebID);
	
      } else if(!strcasecmp(argbuf,"OFF")) {
	if(cmd>18) cmd+=18; // BLUE IEB starts at 18-33 in Shared Memory

	ierr = wagoSetGet(0,shm_addr->MODS.WAGOIP[ieb_id],1,512,devOnOff,1);
	onoff[0] = devOnOff[0];

	onoff[0]|=(short)bits[cmd];
	ierr = wagoSetGet(1,shm_addr->MODS.WAGOIP[ieb_id],1,512,onoff,1);

	sprintf(reply,"IEB MLC%d_%c=OFF",cmd,iebID);
	
      } else {
	sprintf(reply,"%s Invalid request '%s', Usage: ieb [n] mlc [m] [ON|OFF|STATUS]", who_selected,args);

	return CMD_ERR;
      }
    } else {
      sprintf(reply,"%s Invalid requset '%s', Usage: ieb [n] mlc [m] [ON|OFF|STATUS]", who_selected,args);
      
      return CMD_ERR;
    }
  }
  return CMD_OK;
}

/*---------------------------------------------------------------------------
//
// imcs - Query/Close/Open loop the imcs quadcells
//
*/

/*!
  \brief IMCS command - report quadcells Open/Close loop
  \param args string with the command-line arguments
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \param return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage: IMCS

  \par Description:

*/
int
cmd_imcs(char *args, MsgType msgtype, char *reply)
{
  char cmd_instruction[PAGE_SIZE];
  char who_selected[24];
  char dummy[PAGE_SIZE];
  int cmd, ind, closeOpen;
  int device;
  int rbcmd;
  int rbrate;
  int ttfA;
  int ttfB;
  int ttfC;

  memset(cmd_instruction,0,sizeof(cmd_instruction));

  strcpy(who_selected,cmdtab[commandID].cmd);
  StrUpper(who_selected);

  GetArg(args,1,cmd_instruction);

  if(who_selected[0]=='B') {

    ttfA=getMechanismID("bcolttfa",dummy); // Get mechanism device ID
    if(ttfA==-1) {
      sprintf(reply,"%s %s",who_selected,dummy);
      return CMD_ERR;
    }
    ttfB=getMechanismID("bcolttfb",dummy); // Get mechanism device ID
    if(ttfB==-1) {
      sprintf(reply,"%s %s",who_selected,dummy);
      return CMD_ERR;
    }
    ttfC=getMechanismID("bcolttfc",dummy); // Get mechanism device ID
    if(ttfC==-1) {
      sprintf(reply,"%s %s",who_selected,dummy);
      return CMD_ERR;
    }
    rbcmd=0;
    rbrate=0;
    ind=0;
    closeOpen=shm_addr->MODS.blueCloseLoop;

  } else if(who_selected[0]=='R') {
    ttfA=getMechanismID("rcolttfa",dummy); // Get mechanism device ID
    if(ttfA==-1) {
      sprintf(reply,"%s %s",who_selected,dummy);
      return CMD_ERR;
    }
    ttfB=getMechanismID("rcolttfb",dummy); // Get mechanism device ID
    if(ttfB==-1) {
      sprintf(reply,"%s %s",who_selected,dummy);
      return CMD_ERR;
    }
    ttfC=getMechanismID("rcolttfc",dummy); // Get mechanism device ID
    if(ttfC==-1) {
      sprintf(reply,"%s %s",who_selected,dummy);
      return CMD_ERR;
    }
    closeOpen=shm_addr->MODS.redCloseLoop;
    rbcmd=0;
    rbrate=1;
    ind=1;
  }

  if(strlen(args)<=0) {
    if(who_selected[0]=='R') {
      sprintf(reply,"RIMCS IMCS_R=%s CLGAIN_R=%0.2f QCMIN_R=%0.2f QCSAMP_R=%d QCNAVG_R=%d TARGET_R=%s QC1_R=%0.3f QC2_R=%0.3f QC3_R=%0.3f QC4_R=%0.3f",
	      closeOpen==0 ? "OPEN" : "CLOSE",
	      shm_addr->MODS.redQC_Gain,
	      shm_addr->MODS.redQC_Threshold[0],
	      shm_addr->MODS.redQC_SampleRate,
	      shm_addr->MODS.redQC_Samples,
	      shm_addr->MODS.redQC_TARGET==1 ? "ON" : "OFF",
	      shm_addr->MODS.redQC_Average[0],
	      shm_addr->MODS.redQC_Average[1],
	      shm_addr->MODS.redQC_Average[2],
	      shm_addr->MODS.redQC_Average[3]);
    } else {
      sprintf(reply,"BIMCS IMCS_B=%s CLGAIN_B=%0.2f QCMIN_B=%0.2f QCSAMP_B=%d QCNAVG_B=%d TARGET_B=%s QC1_B=%0.3f QC2_B=%0.3f QC3_B=%0.3f QC4_B=%0.3f",
	      closeOpen==0 ? "OPEN" : "CLOSE",
	      shm_addr->MODS.blueQC_Gain,
	      shm_addr->MODS.blueQC_Threshold[0],
	      shm_addr->MODS.blueQC_SampleRate,
	      shm_addr->MODS.blueQC_Samples,
	      shm_addr->MODS.blueQC_TARGET==1 ? "ON" : "OFF",
	      shm_addr->MODS.blueQC_Average[0],
	      shm_addr->MODS.blueQC_Average[1],
	      shm_addr->MODS.blueQC_Average[2],
	      shm_addr->MODS.blueQC_Average[3]);
    }
    return CMD_OK;
  }

  if (!strncasecmp(args,"M#",2)) { // check for low-level command

    if(who_selected[0]=='R')
      device=getMechanismID("rimcs",dummy); // Get mechanism device ID
    else if(who_selected[0]=='B')
      device=getMechanismID("bimcs",dummy); // Get mechanism device ID

    if(device==-1) {
      sprintf(reply,"%s %s",who_selected,dummy);
      return CMD_ERR;
    }
    ierr=hebCommand(device,who_selected,reply);

    if(ierr<=-1) return CMD_ERR;

    return CMD_OK;
  }

  if (!strcasecmp(cmd_instruction,"CLOSE")) {
    if(who_selected[0]=='B') {
      shm_addr->MODS.blueCloseLoop=1;
      shm_addr->MODS.blueCloseLoopON=1;
      sprintf(reply,"%s %s=CLOSE Running closed loop",who_selected,
	      who_selected);
    } else if(who_selected[0]=='R') {
      shm_addr->MODS.redCloseLoop=1;
      shm_addr->MODS.redCloseLoopON=1;
      sprintf(reply,"%s %s=CLOSE Running close loop",who_selected,
	      who_selected);
    }

  } else if (!strcasecmp(cmd_instruction,"OPEN")) {
    if(who_selected[0]=='B') {
      shm_addr->MODS.blueCloseLoop=0;
      shm_addr->MODS.blueCloseLoopON=0;
      sprintf(reply,"%s %s=OPEN Running open loop",who_selected,
	      who_selected);

    } else if(who_selected[0]=='R') {
      shm_addr->MODS.redCloseLoop=0;
      shm_addr->MODS.redCloseLoopON=0;
      sprintf(reply,"%s %s=OPEN Running open loop",who_selected,
	      who_selected);
    }

  } else if (!strcasecmp(cmd_instruction,"QCMIN")) {
    GetArg(args,2,argbuf);
    if(who_selected[0]=='B') {
      shm_addr->MODS.blueQC_Threshold[0]=atof(argbuf);
    } else if(who_selected[0]=='R') {
      shm_addr->MODS.redQC_Threshold[0]=atof(argbuf);
    }
    sprintf(reply,"%s QCMIN_%c=%0.2f",who_selected,who_selected[0],atof(argbuf));

  } else if (!strcasecmp(cmd_instruction,"QCSAMP")) {
    GetArg(args,2,argbuf);
    if(who_selected[0]=='B') {
      shm_addr->MODS.blueQC_SampleRate=atoi(argbuf);
    } else if(who_selected[0]=='R') {
      shm_addr->MODS.redQC_SampleRate=atoi(argbuf);
    }
    sprintf(reply,"%s QCSAMP_%c=%d",who_selected,who_selected[0],atoi(argbuf));

  } else if (!strcasecmp(cmd_instruction,"GAIN")) {
    GetArg(args,2,argbuf);
    if(who_selected[0]=='B') {
      shm_addr->MODS.blueQC_Gain=atof(argbuf);
    } else if(who_selected[0]=='R') {
      shm_addr->MODS.redQC_Gain=atof(argbuf);
    }
    sprintf(reply,"%s CLGAIN_%c=%0.2f",who_selected,who_selected[0],atof(argbuf));

  } else if (!strcasecmp(cmd_instruction,"AVERAGE")) {
    GetArg(args,2,argbuf);
    if(who_selected[0]=='B') {
      shm_addr->MODS.blueQC_Samples=atoi(argbuf);
    } else if(who_selected[0]=='R') {
      shm_addr->MODS.redQC_Samples=atoi(argbuf);
    }
    sprintf(reply,"%s QCNAVG_%c=%d",who_selected,who_selected[0],atoi(argbuf));

  } else {
    sprintf(reply,"%s Invalid requset '%s', Usage: cIMCS [close|open][QCMIN lev][QCSAMP msec][AVERAGE n][GAIN g]",
	    who_selected,args);
    return CMD_ERR;
  }

  return CMD_OK;
}

/* ---------------------------------------------------------------------------
//
// loadplc - load MicroLYNX controller with PLC ladder program file.
//
*/

/*!
  \brief LOADPLC command - load MicroLYNX controller with Ladder code
  \param msgtype message type if the command was sent as an IMPv2 message
  \param reply string to contain the command return reply
  \return #CMD_OK on success, #CMD_ERR if errors occurred, reply contains
  an error message.

  \par Usage:
  loadplc

  Send file to MicroLYNX controller via network or serial.
*/
int
cmd_loadplc(char *args, MsgType msgtype, char *reply)
{
  sprintf(reply,"Not ready to use");
}
#if 0
  int i,j,ierr=0;
  int device,len;
  int line_count;
  char inbuf[MAXPGMLINE];
  char actualLine[MAXPGMLINE];
  char mechanism[MAXPGMLINE];
  char plcFile[MAXPGMLINE];
  char who_selected[24];
  char dummy[PAGE_SIZE];
  FILE *fp;

  strcpy(who_selected,cmdtab[commandID].cmd);
  StrUpper(who_selected);
  /*
  // First lets play it safe and clear the buffers.
  */
  memset(mechanism,0,sizeof(mechanism)); 
  memset(plcFile,0,sizeof(plcFile)); 
  memset(inbuf,0,sizeof(inbuf));
  memset(actualLine,0,sizeof(actualLine));

  GetArg(args,1,mechanism);

  device=getMechanismID(mechanism,dummy); // Get mechanism device ID
  if(device==-1) {
    sprintf(reply,"%s %s",who_selected,dummy);
    return CMD_ERR;
  }

  if (mlcBusy(device,dummy)) { // Busy?
    sprintf(reply,"%s %s",who_selected,dummy);
    return CMD_ERR;
  }

  if (!strncasecmp(who_selected,"AGW",3)) {    
    ierr=agwcu("localhost",0,"loadplc ",mechanism);
    if(ierr!=0) {
      sprintf(reply,"%s", dummy);
      return CMD_ERR;
    }
    return CMD_OK;
  }

  sprintf(plcFile,"/home2/mods/plc/%s.plc",mechanism);

  plcFile[strlen(plcFile)]='\0'; // Do not want seg. faults at any time.
  
  /*
  // Now open the program file, return a -9 if file is in error.
  */
  if (!(fp=fopen(plcFile, "r"))) {
    sprintf(reply,"LOADPLC Could not open file %s",plcFile);
    return CMD_ERR;
  }

  /* ----------------------------------------------------------------
  // Config file parser loop
  // 
  // Read in each line of the config file and process it 
  //  cout << "[ML" << device << "]loading: " << plcFile << endl;
  //
  //----------------------------------------------------------------
  // Read in each line of the PLC code and process it 
  //
  */
  shm_addr->MODS.busy[device]=1; // Hold the IP until finished

  ierr=mlcClear(device,dummy); // Clean NVM on device

  if(ierr!=0) sprintf(reply,"LOADPLC MLC=NOLOAD");
  sprintf(reply,"LOADPLC MLC=LOADED");

  line_count=0;
  while (fgets(inbuf, MAXPGMLINE, fp)) {
    line_count++;
    if(strstr(inbuf,"END HEADER")) break;
  }

  while(fgets(inbuf, MAXPGMLINE, fp)) {
    line_count++;
    /*
    // Skip comments (#,*,') and blank lines
    */
    for(i=j=0; inbuf[i]!='\'' &&
	  inbuf[i]!='*' && 
	  inbuf[i]!='\n' && 
	  inbuf[i]!='#' &&
	  inbuf[i]!='\0' && i<80; i++) {
	actualLine[i]=inbuf[i];
    }

    len=strlen(actualLine);
    if (len!=0) {
      actualLine[strlen(actualLine)]='\r';

      if (WriteTTYPort(&shm_addr->MODS.commport[device],actualLine) <0) {
	sprintf(reply,"LOADPLC %s Can *NOT* write to %s", 
		shm_addr->MODS.who[device],
		shm_addr->MODS.commport[device].Port);
	shm_addr->MODS.busy[device]=0;  // Clear the HOST busy bit.
	if (fp!=0)
	  fclose(fp);
	return CMD_ERR;
      }
      
      MilliSleep(100);
      if(ierr=ReadTTYPort(&shm_addr->MODS.commport[device],dummy,10L)<0) {
	sprintf(reply,"LOADPLC %s Can *NOT* read from %s",dummy,
		shm_addr->MODS.who[device],
		shm_addr->MODS.commport[device].Port);
	shm_addr->MODS.busy[device]=0;  // Clear the HOST busy bit.
	if (fp!=0)
	  fclose(fp);
	return CMD_ERR;
      }
      fprintf(stderr,"[%d][%d]%s",len,line_count,dummy);
      
    }
    memset(dummy,0,sizeof(dummy));
    memset(actualLine,0,sizeof(actualLine));
    memset(inbuf,0,sizeof(inbuf));

  }

  MilliSleep(1000);
  WriteTTYPort(&shm_addr->MODS.commport[device],"SAVE\r");
  if(ierr=ReadTTYPort(&shm_addr->MODS.commport[device],dummy,10L)<0) {
    sprintf(reply,"LOADPLC %s Can *NOT* read from %s",dummy,
	    shm_addr->MODS.who[device],
	    shm_addr->MODS.commport[device].Port);
    shm_addr->MODS.busy[device]=0;  // Clear the HOST busy bit.
    if (fp!=0)
      fclose(fp);
    return CMD_ERR;
  }
  shm_addr->MODS.busy[device]=0;  // Clear the HOST busy bit.
  sprintf(reply,"%s, [ML%d]loaded with %s ",reply,device,plcFile);
    
  if (fp!=0)
    fclose(fp);
  
  return CMD_OK;
}
#endif
/* ***************************************************************************
//
// Command Interpreter I/O Handlers
//
//
//---------------------------------------------------------------------------
//
// KeyCommand() - process a command from the keyboard
//
*/

#include <readline/readline.h>  // Gnu readline utility
#include <readline/history.h>   // Gnu history utility

/*!  
  \brief Process a command from the client's console keyboard
  \param line string with the keyboard command
  \param reply string to contain reply

  \sa SocketCommand()
*/

void
KeyCommand(char *line, char reply[])
{
  char cmd[BIG_STR_SIZE];       // command string (oversized)
  char args[BIG_STR_SIZE];      // command-line argument buffer (oversized)

  // ISIS message handling stuff

  char temp[ISIS_MSGSIZE];       // ISIS message buffer
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

  // If line is NULL, we have nothing to do, return

  if (line==NULL) return;

  // Similarly, if line is blank, return

  // Allocate memory for the message buffer and clear it

  message = (char *)malloc((ISIS_MSGSIZE)*sizeof(char));
  memset(message,0,ISIS_MSGSIZE);

  // Copy the keyboard input line into the message buffer 

  strcpy(message,line);

  // We're all done with the original string from readline(), free it

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

      // Keep destID for Status checks
      strcpy(who_srcID,destID);

      // The trick here is that REQ doesn't put anything in the
      // msgtype field, so that whatever msgtype designator is
      // in the message string gets retained.

      strcpy(msg,ISISMessage(client.ID,destID,REQ,msgbody));

      msg[strlen(msg)-1]='\0';
      printf("OUT: %s\n",msg);
      
      SendToISISServer(&client,msg); // and send it off
      
      if (client.isVerbose) {
	msg[strlen(msg)-1]='\0';
	printf("OUT: %s\n",msg);
      }
    }
    else
      printf("No ISIS server active, > command unavailable\n");
    
  } else {   // All other commands use the cmd_xxx() action calls

    // Traverse the command table, matches are case-insensitive, but
    // must be exact word matches (no abbreviations or aliases)
    
    nfound = 0;
    for (i=0; i<NumCommands; i++) {
      if (strcasecmp(cmdtab[i].cmd,cmd)==0) { 
	nfound++;
	icmd=i;
	break;
      }
    }

    if (nfound == 0) { // Send an error.
      sprintf(reply,"ERROR: unknown command - %s",cmd);

    } 
    else { // All console keyboard are treated as EXEC: type messages

      commandID = icmd; // save to identify mechanism name
      switch (cmdtab[icmd].action(args,EXEC,temp)) {
	
      case CMD_ERR:
	sprintf(reply,"ERROR: %s",temp);
	break;
	
      case CMD_OK:
	sprintf(reply,"DONE: %s",temp);
	break;

      case CMD_WARN:
	sprintf(reply,"WARNING: %s",temp);
	break;

      case CMD_FERR:
	sprintf(reply,"FATAL: %s",temp);
	break;
	
      case CMD_NOOP:
      default:
	break;
      } // end of switch()
    }
  }
}

/* ---------------------------------------------------------------------------
//
// SocketCommand() - process a message/command from a remote ISIS server/client
//
*/

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

  \sa KeyCommand()
*/
void
SocketCommand(char *buf)
{

  // ISIS message components 

  char msg[ISIS_MSGSIZE];       // Full ISIS message buffer
  char srcID[ISIS_NODESIZE];    // ISIS message sending node ID
  char destID[ISIS_NODESIZE];   // ISIS message destination node ID
  MsgType msgtype = REQ;        // ISIS message type, defined in isisclient.h
  char msgbody[ISIS_MSGSIZE];   // ISIS message/command body

  // Command components (command args)

  char cmd[BIG_STR_SIZE];       // command string (oversized)
  char args[BIG_STR_SIZE];      // command-line argument buffer (oversized)
  char reply[LONG_STR_SIZE];     // command reply string
  //  char reply[BIG_STR_SIZE];     // command reply string

  // Other working variables

  int i;
  int nfound=0;
  int icmd=-1;

  // Some simple initializations

  memset(reply,0,sizeof(reply));
  memset(cmd,0,sizeof(cmd));
  memset(msg,0,sizeof(msg));
  memset(srcID,0,sizeof(srcID));
  memset(destID,0,sizeof(destID));

    // Split the ISIS format message into components

  if (SplitMessage(buf,srcID,destID,&msgtype,msgbody)<0) {
    if (client.isVerbose)
      printf("\nISIS IN: %s\n",buf);
    return;
  }
  
  // Keep srcID for Status checks
  strcpy(who_srcID,srcID);
      
  // Immediate action depends on the type of message received as
  // recorded by the msgtype code.

  switch(msgtype) {

  case STATUS:  // we've been sent a status message, echo to console
    if (client.isVerbose) printf("%s\n",buf);
    break;
	  
  case DONE:    // command completion message (?), echo to console.
    if (client.isVerbose) printf("%s\n",buf);
    break;
	  
  case ERROR:   // error messages, echo to console, get fancy later
  case WARNING:
  case FATAL:
    if (client.isVerbose) printf("%s\n",buf);
    break;
    
  case REQ:    // implicit command requests
  case EXEC:   // and executive override commands

    memset(msg,0,ISIS_MSGSIZE);
    memset(args,0,sizeof(args));

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

    if (nfound == 0) {
      StrUpper(cmd);
      if(!strcasecmp(cmd,"MSG"))
	sprintf(msg,"%s>%s DONE: MESSAGE %s",client.ID,srcID,args);
      else
	sprintf(msg,"%s>%s ERROR: %s Invalid command, type: help [cmd_name|mech|system] for command list",client.ID,srcID,cmd);

      memset(cmd,0,sizeof(cmd));

    } else {
      commandID = icmd; // save to identify mechanism name
      switch(cmdtab[icmd].action(args,msgtype,reply)) {

      case CMD_ERR: // command generated an error
	sprintf(msg,"%s>%s ERROR: %s",client.ID,srcID,reply);
	break;

      case CMD_NOOP: // command is a no-op, debug/verbose output only
	if (client.isVerbose)
	  printf("IN: %s from ISIS node %s\n",msgbody,srcID);
	break;

      case CMD_OK:  // command executed OK, return reply
      default:
	sprintf(msg,"%s>%s DONE: %s",client.ID,srcID,reply);
	break;
  
      } // end of switch on cmdtab.action()
    }

    // An incoming PING requires special handling - it is an exception
    // to the usual messaging syntax since PONG is sent in reply 

    if (strcasecmp(cmd,"PING") == 0)
      sprintf(msg,"%s>%s %s\r",client.ID,srcID,reply);
      
    break;
  default:  // we don't know what we got, print for debugging purposes
    if (client.isVerbose) 
      printf("Malformed message received on client port: %s\n",buf);
    break;

  } // end of switch(msgtype) -- default falls through with no-op

  // Do we have something to send back? 
  //
  // If we are configured as an ISIS client (client.useISIS=true), send
  // the reply back to the ISIS server for handling with
  // SendToISISServer()
  //
  // If we are configured as standalone (client.useISIS=false), send the
  // reply back to the remote host with SendToHost().

  if (strlen(msg)>0) { // we have something to send
    if (client.useISIS)
      SendToISISServer(&client,msg);
    else 
      ReplyToRemHost(&client,msg);
 
    mmcLOGGER(shm_addr->MODS.LLOG,msg); // mmc mesi log
 
    if (client.isVerbose) {
      msg[strlen(msg)-1]='\0';
      printf("OUT: %s\n",msg);
    }
   } // end of reply handling
}

//-------------------------------------------------------------------------
//
// findPoly() - single-precision polynomial function
//
// Arguments:
//   float x - value to evalute polynomial
//   float *a - pointer to an array of polynomial coefficients
//   int n - number of coefficients in a
//
// Description: 
//   Returns f(x) given x, a, and n using Horner's method to evaluation
//   the polynomial.  Horner's Method is a computationally efficient
//   scheme for computing simple polynomials in monomial form that is
//   faster and has fewer round-off problems than using the pow() math
//   function to compute x^n.
//
// No sensible return value if errors - beware you can segfault if
// you do something stupid.
//
// See Also: dPoly() for a double-precision implementation
//
// Author:
//   R. Pogge, OSU Astronomy Dept.
//   pogge@astronomy.ohio-state.edu
//   2009 April 29
//
// Modification History:
//  
//-------------------------------------------------------------------------

float
findPoly(float x, float *a, int n)
{
  float fval=a[--n];    // start with the highest-order coefficient
  while (--n>=0)  
    fval=x*fval + a[n];
  return fval; 
}

//-------------------------------------------------------------------------
//
// dfindPoly() - double-precision polynomial function
//
// Arguments:
//   double x - value to evalute polynomial
//   double *a - pointer to an array of polynomial coefficients
//   int n - number of coefficients in a
//
// Description: 
//   Returns f(x) given x, a, and n using Horner's method to evaluation
//   the polynomial.  Horner's Method is a computationally efficient
//   scheme for computing simple polynomials in monomial form that is
//   faster and has fewer round-off problems than using the pow() math
//   function to compute x^n.
//
// No sensible return value if errors - beware you can segfault if
// you do something stupid.
//
// See Also: fPoly() for a single-precision implementation
//
// Author:
//   R. Pogge, OSU Astronomy Dept.
//   pogge@astronomy.ohio-state.edu
//   2009 April 29
//
// Modification History:
//  
//-------------------------------------------------------------------------

double
dfindPoly(double x, double *a, int n)
{
  double fval=a[--n];    // start with the highest-order coefficient
  while (--n>=0)  
    fval=x*fval + a[n];
  return fval; 
}

//---------------------------------------------------------------------------
//
// ptRTD2C() - convert WAGO Pt RTD sensor raw data to degrees C
//
// Arguments:
//   short rawRTD - raw RTD datum
//
// Description:
//   Converts WAGO RTD module raw integer datum into degrees C float.
//
//   The WAGO module has a temperature resolution of 0.1C and a temperature
//   range of -273 to 850C. If temp in >850.0, short integer raw data are
//   wrapped on 2^16-1 ADU.  This is how it reports negative temperatures
//   with a 16-bit unsigned integer.
//
//   Author:
//     R. Pogge, OSU Astronomy Dept
//     pogge.1@osu.edu
//     2025 July 17
//
//---------------------------------------------------------------------------

float
ptRTD2C(short rawData)
{
  float tempMax = 850.0; // max temperature reported
  float tempRes = 0.1;   // temperature resolution 0.1C
  float temp, wrapT;
  
  wrapT = tempRes*(pow(2.0,16)-1);
  temp = tempRes*(float)rawData;
  if (temp > tempMax)
    temp -= wrapT;

  return temp;
}
