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
  2025 July 25 - major overhaul for python azcam server [rwp/osu]
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
  char cfgFile[128]; //!< Name of the AzCam client configuration file (on the client)
  char iniFile[128]; //!< Name of the AzCam server initialization file (on the server)

  // System State flags

  int State;       //!< Server state, one of #IDLE, #EXPOSING, #READOUT, or #PAUSE
  int Shutter;     //!< Shutter state, one of #SH_OPEN or #SH_CLOSED
  int Abort;       //!< Abort Flag, if 1 an abort has been requested

  // Temperature Info

  float setPoint;    //!< CCD detector temperature set point in degrees C
  float ccdTemp;     //!< CCD detector temperature in degrees C
  float baseTemp;    //!< CCD mount base temperature in degrees C
  
  // Detector Region of Interest (ROI) Parameters

  int firstCol;   //!< first column to be read in unbinned pixels
  int lastCol;    //!< last column to be read in unbinned pixels
  int colBin;     //!< Column-axis binning factor
  int firstRow;   //!< first row to be read in unbinned pixels
  int lastRow;    //!< last row to be read in unbinned pixels
  int rowBin;     //!< Row-axis binning factor

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

  // Detector Readout Pixel Count Parameters

  int Ncols;       //!< Number of columns to be readout
  int Nrows;       //!< Number of rows to be readout
  int Npixels;     //!< Total number of pixels to be readout
  int pixLeft;     //!< Number of pixels left to be readout
  int Nread;       //!< Number of pixels readout so far

  // Other parameters

  char  imgType[12];   //!< Image type string (aka IMAGETYP keyword, object,flat,bias,etc.)
  char  imgTitle[64];  //!< Image title string (aka OBJECT keyword)
  float expTime;       //!< Exposure (integration) time in seconds
  float timeLeft;      //!< Time remaining on the current exposure in seconds
  char  filePath[128]; //!< File path for images (must be valid on the AzCam server machine)
  char  fileName[128]; //!< Rootname of image files
  int   fileNum;       //!< Image sequence number of the next file to be written
  int   fileFormat;    //!< File format, one of #STDFITS, #MEF, #BINARY
  char  lastFile[128]; //!< Full name of the last image file written, including path

} azcam_t;

// Parameter Values

#define SH_OPEN   1   //!< Shutter is open
#define SH_CLOSED 0   //!< Shutter is closed

// Exposure Status Codes for azcam = exposure.get_exposureflag
// list is in exposure.exposureflags (on the server not remote)

#define IDLE     0    //!< Server is idle and awaiting commands
#define EXPOSING 1    //!< Exposure in progress
#define ABORT    2    //!< Exposure Abort in progress
#define PAUSE    3    //!< Exposure is paused
#define RESUME   4    //!< Exposure is resuming
#define READ     5    //!< Exposure has readout, pending write
#define READOUT  7    //!< Exposure is being read out
#define SETUP    8    //!< Exposure is being set up
#define WRITING  9    //!< Exposure file being written to disk
#define AZERROR 11    //!< azcam error code

// Image types

#define DARK_IMAGE  0 //!< Dark image, keep shutter closed during integration
#define LIGHT_IMAGE 1 //!< Light image, open shutter during integration
#define TDI         2 //!< TDI mode, shutter open during readout (unsupported, for future expansion)

#define EXP_NOWAIT 0  //!< Do not wait for exposure to complete, return immediately from StartExposure
#define EXP_WAIT   1  //!< Wait for exposure to complete before returning status from StartExposure

#define STDFITS   0   //!< Write image as Standard FITS format
#define MEF       1   //!< Write image as Multi-extension FITS (MEF) format
#define BINARY    2   //!< Write image as raw Binary format

#define GUIDER_CLIENT 1  //!< [Open] a persistent guider client socket
#define GUIDER_CLOSE  2  //!< Close a guider client socket
#define MESSAGE_CLIENT 3 //!< [Open] a persistent message client socket
#define MESSAGE_CLOSE  4 //!< Close a message client socket
#define DISPLAY_CLIENT 5 //!< Set hostname/port of a display client socket
#define FILE_CLIENT    6 //!< Set hostname/port of a file client socket

// azcamutils function prototypes

int openAzCam(azcam_t *, char *);
void closeAzCam(azcam_t *);
int sendAzCam(azcam_t *, char *);
int readAzCam(azcam_t *, char *);
int azcamCmd(azcam_t *, char *, char *);

// Method function prototypes (implement single or multiple azcam server commands

// Server Control and Database Commands (server.c)

int setKeyword(azcam_t *, char *, char *, char *, char *);
int getKeyword(azcam_t *, char *, char *, char *);
int clearKeywords(azcam_t *, char *, char *); 

// Image Writing Commands (image.c)

int imgFilename(azcam_t *, char *, char *);
int imgPath(azcam_t *, char *, char *);
int imgExpNum(azcam_t *, int, char *);
int getLastFile(azcam_t *, char *);

int setSocket(azcam_t *, int , char *, int , char *);
int writeImage(azcam_t *, char *, char *);
int sendImage(azcam_t *, char *, char *, char *);

// Exposure Control Commands (exposure.c)

int clearArray(azcam_t *, char *);

int setImageInfo(azcam_t *, char *, char *, char*);
int setExposure(azcam_t *, float , char *);
int startExposure(azcam_t *, int , char *);
int expStatus(azcam_t *, char *);
int abortExposure(azcam_t *, char *);
int pauseExposure(azcam_t *, char *);
int resumeExposure(azcam_t *, char *);
int abortReadout(azcam_t *, char*);

int getTimeLeft(azcam_t *, char *);
int getPixLeft(azcam_t *, char *);

int openShutter(azcam_t *, char *);
int closeShutter(azcam_t *, char *);

// CCD format, ROI, and on-chip binning

// int setFormat(azcam_t *, char *);  // probably not ever, but could change
int getFormat(azcam_t *, char *);

int setROI(azcam_t *, int, int, int, int, char *);
int getROI(azcam_t *, char *);
int resetROI(azcam_t *, char *);

int setCCDBin(azcam_t *, int, int, char *);
int setXBin(azcam_t *, int, char *);
int setYBin(azcam_t *, int, char *);

// Temperature Commands (ccdtemp.c)

int getTemp(azcam_t *, char *);
int setTemp(azcam_t *, float , char *);

// Additional Utility Functions (azcamutils.c)

void initAzCam(azcam_t *);
void azcamInfo(azcam_t *);

#endif // AZCAM_H
