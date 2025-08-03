/*!
  \mainpage azcamUtils - azcam client utility library

  \author  R. Pogge, OSU Astronomy Dept. (pogge.1@osu.edu)
  \date 2025 July 23

  \section Overview

  In 2025 we replaced the 2005-vintage OSU CCD controllers with
  next-generation STA Archon controllers. To operate the Archons,
  we've adopted the azcam software developed by Prof. Michael Lesser
  at the University of Arizona.  LBTO uses azcam for all of the guide
  and wavefront sensor cameras and would like to try to adopt it more
  broadly for current and future instruments where practical.
  
  The main program is the azcam application that runs on the same
  physical host with a dedicated gigabit ethernet fiber interface
  to the Archon.  Written in python, azcam provides a TCP socket
  command interface that allows client applications to execute
  server commands to control the CCD system.  The current generation
  is written in python 3 and runs on Linux and Windowsservers.

  For MODS we are using a customized version of the Linux python
  azcam server, azcam-mods, that overloads the core azcam server
  with MODS-specific function.
  
  This library is used to create an interface agent that replaces
  the old OSU DOS "IC" detector control computers and BASIC software
  with an ISIS client that emulates all the IC functions so we
  don't need to rewrite most of the MODS data-taking system to
  interface with the Archons (that might become an option in the
  future...)

  \section Components

  The OSU azcam client library consists of these components:
  <pre>
     server.c     - azcam server control and database functions
     image.c      - image writing functions
     exposure.c   - exposure and detector control functions
     ccdtemp.c    - temperature control functions
     azcamutils.c - additional utility functions
     iosubs.c     - TCP socket routines providing a common communication layer
     azcam.h      - azcam client library header file
  </pre>

  The client functions encapsulate most of the basic functions of the
  azcam server, hiding the socket communications layer from any applications
  using the function.  It knows how to process coded status return from
  the azcam server and translate it into useful data for the calling client
  application.

  The iosubs.c functions provide the TCP socket communications layer,
  providing commands for initializing and closing a socket connection,
  read, writing, and handling command transmission, including
  communication timeout handling.

  The main camera parameters are stored in an instance of an #azcam
  struct which provide all of the information needed to interact with
  the server, and carries all of the basic camera parameters.

  \section Make Compilation and Dependencies

  The Make for this library compiles the code using g++.  There are no
  external libraries required to link with libazcam.a in its current
  version.

  \section Notes

  \section Modification History
  <pre>
  2005 May 6 - initial development [rwp/osu]
  2005 May 13 - first field-test version [rwp/osu]
  2005 May 17 - final organization of the API [rwp/osu]
  </pre>
*/

//---------------------------------------------------------------------------

/*!
  \file azcamutils.c
  \brief azcam utility functions

  The following are a suite of additional utility functions that provide
  additional functionality to an application using this client API.

  \author R. Pogge, OSU Astronomy Dept. (pogge.1@osu.edu)
  \original 2005 May 17
  \date 2025 July 26
  
*/

#include "azcam.h" // azcam client API header 

         
/*!
  \brief Initialize an #azcam data structure

  \param cam pointer to an #azcam struct with the server parameters

  Initialize the azcam data structure to default values.  Many detector
  values take 0, which means "I don't know" to other routines in this
  API.

  This function should be called to clear out the #azcam struct before
  filling it with valid data (e.g., from a setup file loaded by the
  application).

*/

void
initAzCam(azcam_t *cam)
{

  // Check the file descriptor, we only change these if we're not already live
  
  if (cam->FD < 0) {
    strcpy(cam->Host,"localhost"); // default is the computer we're on
    cam->Port = 2402;              // default azcam command server socket
    cam->FD = -1;                  // -1 means no socket yet
  }
  cam->Timeout = 10L;   // default timeout is 10 seconds

  strcpy(cam->iniFile,"NONE");

  // Default state flags

  cam->State = IDLE;
  cam->Shutter = SH_CLOSED;
  cam->Abort = 0;

  // Default temperature info

  cam->setPoint = 0.0;
  cam->ccdTemp =  0.0;
  cam->baseTemp = 0.0;

  // Default ROI: 0 for <axis>Bin means not defined

  cam->firstCol = 0;
  cam->lastCol = 0;
  cam->colBin = 0;
  cam->firstRow = 0;
  cam->lastRow = 0;
  cam->rowBin = 0;

  // Default Detector Format.  0 for all means not defined

  cam->NCtotal = 0;
  cam->NCpredark = 0;
  cam->NCunderscan = 0;
  cam->NCoverscan = 0;
  cam->NRtotal = 0;
  cam->NRpredark = 0;
  cam->NRunderscan = 0;
  cam->NRoverscan = 0;
  cam->NRframexfer = 0;

  // Default readout pixel counts (0)

  cam->Ncols = 0;
  cam->Nrows = 0;
  cam->Npixels = 0;
  cam->pixLeft = 0;
  cam->Nread = 0;

  // Default Exposure and File info

  cam->expTime = 0.0;
  cam->timeLeft = 0;
  strcpy(cam->filePath,"NONE");
  strcpy(cam->fileName,"NONE");
  cam->fileNum = 1;
  cam->fileFormat = MEF;
  strcpy(cam->lastFile,"NONE");

}

/*!
  \brief Print the contents of the struct on stdout
  \param cam pointer to an #azcam struct with the server parameters

  Prints the contents of the #azcam struct on stdout in human-readable
  form.

*/
  
void
azcamInfo(azcam_t *cam)
{
  printf("azcam Server Interface Configuration:\n");

  printf("Communications:\n");
  if (cam->FD < 0) 
    printf("  TCP Socket CLOSED\n");
  else
    printf("  TCP Socket OPEN\n");

  printf("  azcam Server: %s:%d\n",cam->Host,cam->Port);
  printf("  Comm Timeout: %d seconds\n",cam->Timeout);
  switch(cam->State) {
  case IDLE:
    printf("  Exposure State: IDLE\n");
    break;
  case EXPOSING:
    printf("  Exposure State: Integrating\n");
    break;
  case ABORT:
    printf("  Exposure State: Exposure Aborting\n");
    break;
  case PAUSE:
    printf("  Exposure State: Exposure Paused\n");
    break;
  case RESUME:
    printf("  Exposure State: Integration Resumed\n");
    break;
  case READ:
    printf("  Exposure State: Readout complete, write pending\n");
    break;
  case READOUT:
    printf("  Exposure State: Readout in progress\n");
    break;
  case SETUP:
    printf("  Exposure State: Exposure is being initialized\n");
    break;
  case WRITING:
    printf("  Exposure State: Image file is being written\n");
    break;
  default:
    printf("  Exposure State: UNKNOWN (state code %d)\n",cam->State);
    break;
  }

  printf("Camera Configuration:\n");
  printf("  CCD Config File: %s\n",cam->iniFile);

  printf("  CCD ROI: [%d:%d,%d:%d]  Binning: %dx%d\n",
	 cam->firstCol,cam->lastCol,
	 cam->firstRow,cam->lastRow,
	 cam->colBin,cam->rowBin);

  /*
  printf("  CCD Format: %d %d %d %d %d %d %d %d %d\n",
	 cam->NCtotal,cam->NCpredark,cam->NCunderscan,cam->NCoverscan,
	 cam->NRtotal,cam->NRpredark,cam->NRunderscan,cam->NRoverscan,
	 cam->NRframexfer);
  */
  printf("  Next Readout: %d x %d pixels (%d pixels total)\n",
	 cam->Ncols,cam->Nrows,cam->Npixels);

  printf("CCD Temperature Control:\n");
  printf("    CCD Detector Temperature: %.1f C\n",cam->ccdTemp);
  printf("  CCD Mount Base Temperature: %.1f C\n",cam->baseTemp);
  // printf("   CCD Temperature Set Point: %.1f C\n",cam->setPoint);
  
  printf("Exposure and File Information:\n");
  printf("  Exposure Time: %.3f sec\n",cam->expTime);
  printf("    Data Folder: %s\n",cam->filePath);
  printf("  Next Filename: %s\n",cam->fileName);
  printf("  Last Filename: %s\n",cam->lastFile);
  switch(cam->fileFormat) {
  case STDFITS:
    printf("  File Format: Standard FITS\n");
    break;
  case MEF:
    printf("  File Format: Multi-Extension FITS (MEF)\n");
    break;
  case BINARY:
    printf("  File Format: Raw Binary\n");
    break;
  default:
    printf("  File Format: UNKNOWN\n");
    break;
  }

  printf("\n");

}

