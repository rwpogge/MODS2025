#ifndef MODS_H
#define MODS_H

//
// Parameters of the MODS User Interface
//

#define MODS_SESSION_NAME "MODS1 Control Panel" //!< Window Name for banner
#define MODS_REV_NUMBER   "v1.26.1-bino"       //!< MODS UI Revision number
#define MODS_REV_DATE     "2015 Dec 31"        //!< Revision date (UTC)

// Runtime files and paths

#define MODS_BASE_PATH    "/home/dts/"     //!< Runtime base path
#define MODS_INI_FILE     "modsUI.ini"     //!< default MODS ini file in MODS_BASE_PATH

// MODS dual-instrument array-indexes (convenience parameters)

#define MODS1 0  //!< Dual-instrument array-index for MODS1
#define MODS2 1  //!< Dual-instrument array-index for MODS2

// Control Panel Defaults

#define MODS_USE_MODS1     true      //!< Use MODS1 by default
#define MODS1_SIDE         "Left"    //!< MODS1 at the LBT SX (left) side
#define MODS_USE_MODS2     true      //!< Use MODS2 by default
#define MODS2_SIDE         "Right"   //!< MODS2 at the LBT DX (right) side
#define MODS_USE_HKM       false     //!< Use the MODS Housekeeping Manager
#define MODS_USE_IIF       true      //!< Use the MODS IIF agent
#define MODS_USE_ITERM     false     //!< Enable the iTerm console by default
#define MODS_USE_UM        false     //!< Enable the Utilities Manager by default

// ISIS Server and Client info defaults

#define MODS_ISIS_SERVER_ID     "IS" //!< Default ISIS server ID
#define MODS_ISIS_SERVER_HOST "192.168.139.131" //!< Default ISIS server Host (mods1data)
#define MODS_ISIS_SERVER_PORT   6600 //!< Default ISIS server Port
#define MODS_BASEPORT          10901 //!< Base port (will use BASEPORT+x, x=0..n-1

// Instrument Data-Taking System Base Host IDs

#define MODS_IE_HOST    "IE"  //!< Instrument Electronics (IE) host ID
#define MODS_RCCD_HOST  "RC"  //!< Red CCD controller host ID (IC for Red)
#define MODS_RDM_HOST   "RD"  //!< Red CCD data manager host ID (CB for Red)
#define MODS_BCCD_HOST  "BC"  //!< Blue CCD controller host ID (IC for Blue)
#define MODS_BDM_HOST   "BD"  //!< Blue CCD data manager host ID (CB for Blue)
#define MODS_ENV_HOST   "IM"  //!< Instrument Environmental Monitor host

// Default Instrument Services Agent Host Names

#define MODS_IIF_HOST  "TC"   //!< MODS TCS Interface (aka "IIF") host ID
#define MODS_HKM_HOST  "HK"   //!< MODS housekeeping manager host ID
#define MODS_ITERM_ID  "MT"   //!< MODS engineering console ("ITerm") ID
#define MODS_UTILS_ID  "UM"   //!< MODS utilites manager console ID
#define MODS_CONFIG_ID "CM"   //!< MODS setup/configuration manager console ID

// Observatory Site Parameters

#define LBT_LATITUDE    32.701308 //!< Site Latitude in decimal degrees North
#define LBT_LONGITUDE -109.889064 //!< Site Longitude in decimal degrees East
#define LBT_ELEVATION      3221.0 //!< Site Elevation in meters

// LBT Focal Station Parameters

#define LBT_DGF_SCALE      0.600 //!< LBT Direct Gregorian Focus image scale (mm/arcsec)

// MODS CCD Detector Parameters

#define MODS_CCD_FORMAT  "8Kx3K" //!< MODS e2v CCD231-68 full-frame unbinned pixel format XxY
#define MODS_CCD_XSIZE      8288 //!< MODS e2v CCD231-68 X-axis full-frame unbinned size (pixels)
#define MODS_CCD_YSIZE      3088 //!< MODS e2v CCD231-68 Y-axis full-frame unbinned size (pixels)

// Default User Interface Time intervals - some can be overridden by
// the runtime initialization file

#define MODS_UPDATE_TIME      500 //!< Onboard Clock update cadence in milliseconds
#define MODS_STATUS_TIME    10000 //!< Status query cadence in milliseconds
#define MODS_KEEPALIVE      60000 //!< Keep-Alive signal cadence in milliseconds
#define MODS_HEARTBEAT_TIME   120 //!< MODS ISIS client default heartbeat interval in seconds
#define MODS_DEADMAN_TIME    3600 //!< Dead-Man Switch Timeout in seconds
#define MODS_STATUSMSG_TIME    20 //!< Status message persistence in time seconds
#define MODS_BUSYTICK_TIME 100000 //!< Busy-wait tick interval in microseconds
#define MODS_ESTATUS_TIME     120 //!< Environmental status query time in seconds
#define MODS_MAX_LAMPTIME    3600 //!< Maximum time for a lamp to be lit in seconds
#define MODS_IMCS_CYCLETIME  2500 //!< Typical IMCS cycle time in milleseconds
#define MODS_IMCS_LOCKTIME    120 //!< Default wait-for-lock time in seconds
#define MODS_IMCS_MINWAIT       5 //!< Minimum IMCS wait-for-lock time in seconds
#define MODS_IMCS_NUMLOCK       3 //!< Number of cycles the IMCS must be locked to be "converged"

// CCD filename parameters

#define MODS_MAX_FILENUM     9999 //!< Maximum file counter (runs 1..MODS_MAX_FILENUM)

// IMCS parameters 

#define MODS_IMCS_MINEXP       10 //!< Minimum exposure time for IMCS use in seconds

// Type of Command Request (immediate, queued, or block for time)

#define MODS_IMMEDIATE_REQUEST 0 //!< Send the command/query immediately
#define MODS_WAIT_REQUEST   2000 //!< Send the command/query and block for time in msec
#define MODS_QUEUE_REQUEST    -1 //!< Queue the command/query to the message dispatcher
#define MODS_QUEUE_CADENCE   500 //!< Message dispatcher queue cadence in msec

// MODS Information

#define MODS_URL_WEB      "coming soon..." //!< MODS Web Page
#define MODS_URL_WIKI     "wiki.lbto.arizona.edu/twiki/bin/view/Instrumentation/SciInstrMODS"      //!< MODS Wiki

// Common Mechanism and Integration Parameter Limits

#define MODS_MAX_EXPTIME 3600.0 //!< Maximum exposure time in seconds (must be float)
#define MODS_MAX_NUMEXP     999 //!< Maximum number of exposures in a single GO sequence
#define MODS_GPX_MIN      -92.0 //!< Minimum guide probe X position in millimeters
#define MODS_GPX_MAX       92.0 //!< Maximum guide probe X position in millimeters
#define MODS_GPY_MIN     -204.0 //!< Minimum guide probe Y position in millimeters
#define MODS_GPY_MAX        0.0 //!< Maximum guide probe Y position in millimeters
#define MODS_GPFOC_MIN     -2.0 //!< Minimum guide probe focus position in millimeters
#define MODS_GPFOC_MAX      2.0 //!< Maximum guide probe focus position in millimeters
#define MODS_CAMFOC_MIN     0.0 //!< Minimum camera primary mirror focus position in microns
#define MODS_CAMFOC_MAX  4600.0 //!< Maximum camera primary mirror focus position in microns
#define MODS_COLTTF_MIN     0.0 //!< Minimum collimator mirror TTF actuator position in microns
#define MODS_COLTTF_MAX 32000.0 //!< Maximum collimator mirror TTF actuator position in microns
#define MODS_GRTILT_MIN     0.0 //!< Minimum grating tilt in revs
#define MODS_GRTILT_MAX 30000.0 //!< Maximum grating tilt in revs
#define MODS_DICHROIC_MIN     1 //!< Minimum Dichroic Drum position
#define MODS_DICHROIC_MAX     3 //!< Maximum Dichroic Drum position
#define MODS_SLITMASK_MIN     1 //!< Minimum Slit Mask selector position
#define MODS_SLITMASK_MAX    24 //!< Maximum Slit Mask selector position
#define MODS_CAMFILT_MIN      1 //!< Minimum Camera filter wheel position
#define MODS_CAMFILT_MAX      8 //!< Maximum Camera filter wheel position
#define MODS_GRATING_MIN      1 //!< Minimum Grating turret position
#define MODS_GRATING_MAX      4 //!< Maximum Grating turret position
#define MODS_AGWFILT_MIN      1 //!< Minimum AGW filter wheel position
#define MODS_AGWFILT_MAX      4 //!< Maximum AGW filter wheel position

// ThorLabs IR Laser Controller Parameters

#define MODS_IR_MINPOWER    0.0 //!< IMCS IR Laser Minimum output power [mW]
#define MODS_IR_MAXPOWER    2.2 //!< IMCS IR Laser Maximum output power [mW]
#define MODS_IR_PRESET      1.0 //!< IMCS IR Laser preset operating power [mW]
#define MODS_IR_MINTEMP    19.0 //!< IMCS IR Laser minimum laser head temperature [C]
#define MODS_IR_MAXTEMP    29.0 //!< IMCS IR Laser maximum laser head temperature [C]

// ThorLabs Visible Laser Controller Parameters

#define MODS_VIS_MINPOWER    0.0 //!< Visible Laser minimum output power [mW]
#define MODS_VIS_MAXPOWER    2.3 //!< Visible Laser maximum output power [mW]
#define MODS_VIS_PRESET      0.0 //!< Visible Laser preset operating power [mW]

// Variable-intensity flat-field lamp controller parameters

#define MODS_VFLAT_MIN      0.0 //!< Minimum variable-intensity lamp voltage
#define MODS_VFLAT_MAX     11.0 //!< Maximum variable-intensity lamp voltage

// Telescope Interface Parameters

#define MODS_MIN_ROTPA     -180.0 //!< Minimum rotator position angle [degrees N thru E]
#define MODS_MAX_ROTPA      180.0 //!< Maximum rotator position angle [degrees N thru E]
#define MODS_CATALOG_NCOLS     17 //!< Maximum number of columns in a target catalog

// Housekeeping Monitor Parameters

#define MODS_MAX_MEASURES  32000 //!< Maximum number of measurments for housekeeping monitor data
#define MODS_MAX_BOXTEMP    32.0 //!< Maximum box temperature before triggering cooling alarms [deg C]
#define MODS_MAX_DEWPRES 1.0e-03 //!< Maximum dewar pressure before triggering alarms [torr]
#define MODS_MAX_CCDTEMP   -90.0 //!< Maximum CCD temperature before triggering alarms [deg C]
#define MODS_MAX_DEWTEMP  -165.0 //!< Maximum LN2 dewar tank temperature before triggering alarms [deg C]
#define MODS_MIN_GLYPRES     1.0 //!< Minimum pressure of the glycol cooling supply/return [psi-g]
#define MODS_ALARM_REPEAT      5 //!< Suppress repeat alarm messages every N times

// Application Default Base Fonts

#define MODS_BASE_FONT "Helvetica" //!< Base font size in points
#define MODS_BASE_FONTSIZE      12 //!< Base font size in points
#define MODS_MED_FONTSIZE       14 //!< Medium font size in points

// Runtime Flags - place to enable/disable features

#undef  MODS_DEBUG_MODE

// Globals - use very sparingly!

extern int appFontSize;
extern bool useExpBell;

// Global IMPv2 Host Table

extern QString modsIEHost[2];  //!< Instrument Electronics (IE) server host
extern QString modsBCHost[2];  //!< Blue CCD host
extern QString modsBDHost[2];  //!< Blue Data Manager host
extern QString modsRCHost[2];  //!< Red CCD host
extern QString modsRDHost[2];  //!< Red Data Manager host
extern QString modsTCHost[2];  //!< LBT Telescope interface host
extern QString modsIMHost[2];  //!< Instrument Monitor host

// Global telescope side

extern QString modsSide[2];    //!< LBT side the MODS instance mounted on?

// Global instrument mode parameter

extern QString instMode[2];    //!< Instrument instance mode (dual|red|blue)

#endif
