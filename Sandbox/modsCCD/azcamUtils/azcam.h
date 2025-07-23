#ifndef AZCAM_H
#define AZCAM_H

//
// azcam.h - AzCam Client API Header
//

/*!
  \file azcam.h
  \brief AzCam Client API Header

  \author R. Pogge, OSU Astronomy Dept. (pogge@astronomy.ohio-state.edu)
  \date 2005 May 5

  \par Modification History:
<pre>
  2005 May 17 - updated and cleaned up Doxygen hooks [rwp/osu]
</pre>
*/

// System header files 

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/times.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/file.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <termios.h>
#include <fcntl.h>
#include <signal.h>
#include <math.h>

//----------------------------------------------------------------
//
// azcam: AzCam server parameter struct
//


/*!
  \brief AzCam Server Parameter Table

  Contains basic information about the AzCam server configuration
  we're using.  It starts simple and will become more complex as 
  required.

*/

typedef struct azcam {

  // Server Info

  char Host[64];     //!< Host name of the AzCam server
  int  Port;         //!< Port number of the AzCam server client socket
  int  FD;           //!< File descriptor of the open client port, -1 = closed
  long Timeout;      //!< Timeout Interval for communications
  char CfgFile[128]; //!< Name of the AzCam client configuration file (on the client)
  char IniFile[128]; //!< Name of the AzCam server initialization file (on the server)

  // System State flags

  int State;       //!< Server state, one of #IDLE, #EXPOSING, #READOUT, or #PAUSE
  int ShutterMode; //!< Shutter mode, one of #DARK_IMAGE or #LIGHT_IMAGE
  int Readout;     //!< Readout mode, one of #IMMEDIATE or #DEFERRED
  int Shutter;     //!< Shutter state, one of #SH_OPEN or #SH_CLOSED
  int Abort;       //!< Abort Flag, if 1 an abort has been requested

  // Temperature Info

  float setPoint;    //!< CCD detector temperature set point in degrees C
  float ccdTemp;     //!< CCD detector temperature in degrees C
  float baseTemp;    //!< CCD mount base temperature in degrees C

  // Detector Region of Interest (ROI) Parameters

  int FirstCol;   //!< First column to be read in unbinned pixels
  int LastCol;    //!< Last column to be read in unbinned pixels
  int ColBin;     //!< Column-axis binning factor
  int FirstRow;   //!< First row to be read in unbinned pixels
  int LastRow;    //!< Last row to be read in unbinned pixels
  int RowBin;     //!< Row-axis binning factor

  // Detector Format Parameters

  int NCtotal;     //!< Total number of device columns (serial pixels)
  int NCpredark;   //!< Number of physical dark prescan columns
  int NCunderscan; //!< Number of underscan columns to read
  int NCoverscan;  //!< Number of overscan columns to read
  int NRtotal;     //!< Total number of device rows (lines)
  int NRpredark;   //!< Number of physical dark prescan rows
  int NRunderscan; //!< Number of underscan rows to read
  int NRoverscan;  //!< Number of overscan rows to read
  int NRframexfer; //!< Number of rows to shift for frame transfer mode

  // Detector Readout Configuration Parameters

  int ReadMode;      //!< Readout mode, one of #SINGLE_AMP, #TWO_AMP_PARALLEL, #TWO_AMP_SERIAL, #FOUR_AMP_QUAD, or #MOSAIC
  int Splits;        //!< Splits flag, one of #NO_SPLIT, #SPLIT_SERIAL, #SPLIT_PARALLEL, or #SPLIT_QUAD
  int NumDetX;       //!< number of detectors in the X (columns) direction in a multi-detector mosaic
  int NumDetY;       //!< number of detectors in the Y (rows) direction in a multi-detector mosaic
  char AmpConfig[6]; //!< Amplifier configuration, one for each amp, coding is 0=no flip, 1=flip cols, 2=flip rows, 3=flip both

  // Detector Readout Pixel Count Parameters

  int Ncols;       //!< Number of columns to be readout
  int Nrows;       //!< Number of rows to be readout
  int Npixels;     //!< Total number of pixels to be readout
  int Nread;       //!< Number of pixels readout so far

  // Other parameters

  float ExpTime;       //!< Exposure (integration) time in decimal seconds
  int   Elapsed;       //!< Elapsed exposure time in milliseconds
  char  FilePath[128]; //!< File path for images (must be valid on the AzCam server machine)
  char  FileName[128]; //!< Rootname of image files
  int   FileNum;       //!< Image sequence number of the next file to be written
  int   FileFormat;    //!< File format, one of #STDFITS, #MEF, #BINARY
  char  LastFile[128]; //!< Full name of the last image file written, including path

} azcam_t;

// Parameter Values

#define SH_OPEN   1   //!< Shutter is open
#define SH_CLOSED 0   //!< Shutter is closed

#define IDLE     0    //!< Server is idle and awaiting commands
#define READOUT  1    //!< Server is busy reading out the detector
#define EXPOSING 2    //!< Server is busy exposing (integrating) the detector
#define PAUSE    3    //!< Server is busy with a paused exposure

#define DT670  0      //!< Temperature sensor is a DT670 diode
#define AD590  1      //!< Temperature sensor is an AD590 IC temperature transducer
#define N914   3      //!< Temperature sensor is a 1N914 diode (weirdness of C, doesn't allow a #def to begin with a number)

#define DARK_IMAGE  0 //!< Dark image, keep shutter closed during integration
#define LIGHT_IMAGE 1 //!< Light image, open shutter during integration
#define TDI         2 //!< TDI mode, shutter open during readout (unsupported, for future expansion)

#define EXP_NOWAIT 0  //!< Do not wait for exposure to complete, return immediately from StartExposure
#define EXP_WAIT   1  //!< Wait for exposure to complete before returning status from StartExposure

#define IMMEDIATE 0   //!< Readout occurs immediately after integration 
#define DEFERRED  1   //!< Readout is deferred until explicit readout request

#define STDFITS   0   //!< Write image as Standard FITS format
#define MEF       1   //!< Write image as Multi-extension FITS (MEF) format
#define BINARY    2   //!< Write image as raw Binary format

#define GUIDER_CLIENT 1  //!< [Open] a persistent guider client socket
#define GUIDER_CLOSE  2  //!< Close a guider client socket
#define MESSAGE_CLIENT 3 //!< [Open] a persistent message client socket
#define MESSAGE_CLOSE  4 //!< Close a message client socket
#define DISPLAY_CLIENT 5 //!< Set hostname/port of a display client socket
#define FILE_CLIENT    6 //!< Set hostname/port of a file client socket

#define SINGLE_AMP       0 //!< Single-amplifier readout mode
#define TWO_AMP_PARALLEL 1 //!< 2-amplifier, split parallels, diagonally opposed corners readout
#define TWO_AMP_SERIAL   2 //!< 2-amplifier, split serials
#define FOUR_AMP_QUAD    3 //!< 4-amplifier, quad radout
#define MOSAIC           6 //!< Multi-detector mosaic

#define NO_SPLIT       0 //!< Detector has no split registers
#define SPLIT_SERIAL   1 //!< Detector has split serial registers (2-amp readout)
#define SPLIT_PARALLEL 2 //!< Detector has split parallel registers (2-amp readout)
#define SPLIT_QUAD     3 //!< Detector has split parallel and serial registers (4-amp readout)

// azcamutils Function Prototypes

int openAzCam(azcam_t *, char *);
void closeAzCam(azcam_t *);
int sendAzCam(azcam_t *, char *);
int readAzCam(azcam_t *, char *);
int azcamCmd(azcam_t *, char *, char *);

// Method function prototypes (implement single or multiple AzCam server
// commands

// Server Control and Database Commands (server.c)

int setKeyword(azcam_t *, char *, char *, char *, char *);
int getKeyword(azcam_t *, char *, char *, char *);
int clearKeywords(azcam_t *, char *, char *); 

// Image Writing Commands (image.c)

int setSocket(azcam_t *, int , char *, int , char *);
int writeImage(azcam_t *, char *, char *);
int sendImage(azcam_t *, char *, char *, char *);

// Exposure Control Commands (exposure.c)

int clearArray(azcam_t *, char *);
int startExposure(azcam_t *, int , char *);
int setExposure(azcam_t *, float , char *);
int readExposure(azcam_t *, char *);
int abortExposure(azcam_t *, char *);
int abortReadout(azcam_t *, char*);
int pauseExposure(azcam_t *, char *);
int resumeExposure(azcam_t *, char *);
int setFormat(azcam_t *, char *);
//int SetConfiguration(azcam_t *, char *); no Archon equivalent
int setROI(azcam_t *, char *);
int openShutter(azcam_t *, char *);
int closeShutter(azcam_t *, char *);
//int RowShift(azcam_t *, int , char *); not implemented for Archon
int getDetPars(azcam_t *, char *);
int getPixelCount(azcam_t *, char *);
int setShutterMode(azcam_t *, int , char *); // ?
int setReadoutMode(azcam_t *, int , char *); // ?

// Temperature Commands (ccdtemp.c)

int getTemp(azcam_t *, char *);
int setTemp(azcam_t *, float , char *);

// Additional Utility Functions (azcamutils.c)

void initAzCam(azcam_t *);
void azcamInfo(azcam_t *);
int archonCmd(azcam_t *, char *, char *);

#endif // AZCAM_H

