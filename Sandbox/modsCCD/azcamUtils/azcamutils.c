/*!
  \mainpage azcamUtils - azcam CCD Camera Client API

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

  \section Parts API Components

  The OSU azcam client API consists of two components:
  <pre>
     server.c     - Server Control and Database Functions
     image.c      - Image Writing Functions
     exposure.c   - Exposure and Detector Control Functions
     ccdtemp.c    - Temperature Control Functions
     azcamutils.c - Additional Utility Functions
     iosubs.c     - TCP socket routines providing a common 
                      communication layer
     azcam.h      - API header file
  </pre>

  The API functions encapsulate most of the basic functions
  of the azcam server, hiding the socket communications layer beneath
  the API layer functions, and which know how to process any status
  returns from the commands into data members of the #azcam struct, or
  returned as data to the calling application.  Most API function names
  recapitulate single azcam server commands, while others are "meta"
  commands that incorporate one or more functions.  Not all azcam server
  functions have their own API routines, for example, the ARC Controller
  commands are accessed via the ARCCommand() function.  Defined
  parameters are provided to give "macro" versions of all azcam server
  numerical codes, making programming more straightforward.

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

  \section Mods Modification History
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

  \author R. Pogge, OSU Astronomy Dept. (pogge@astronomy.ohio-state.edu)
  \date 2005 May 17
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
    strcpy(cam->Host,"NONE");
    cam->Port = 0;
    cam->FD = -1;         // -1 means no socket
  }
  cam->Timeout = 10L;   // default timeout is 10 seconds
  strcpy(cam->IniFile,"NONE");
  strcpy(cam->CfgFile,"NONE");

  // Default state flags

  cam->State = IDLE;
  cam->ShutterMode = LIGHT_IMAGE;
  cam->Readout = IMMEDIATE;
  cam->Shutter = SH_CLOSED;
  cam->Abort = 0;

  // Default temperature info

  cam->SetPoint = 0.0;
  cam->CCDTemp =  0.0;
  cam->DewarTemp = 0.0;
  cam->DiodeSensor = DT670;
  cam->CCDSensor = DT670;
  cam->DewarSensor = DT670;

  // Default ROI: 0 for <axis>Bin means not defined

  cam->FirstCol = 0;
  cam->LastCol = 0;
  cam->ColBin = 0;
  cam->FirstRow = 0;
  cam->LastRow = 0;
  cam->RowBin = 0;

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

  // Default Readout Configuration: single amp

  cam->ReadMode = SINGLE_AMP;
  cam->Splits = NO_SPLIT;
  cam->NumDetX = 1;
  cam->NumDetY = 1;
  strcpy(cam->AmpConfig,"0000");

  // Default readout pixel counts (0)

  cam->Ncols = 0;
  cam->Nrows = 0;
  cam->Npixels = 0;

  // Default Exposure and File info

  cam->ExpTime = 0.0;
  cam->Elapsed = 0;
  strcpy(cam->FilePath,"NONE");
  strcpy(cam->FileName,"NONE");
  cam->FileNum = 1;
  cam->FileFormat = EXTFITS;
  strcpy(cam->LastFile,"NONE");

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
  printf("  azcam Client Config File: %s\n",cam->CfgFile);

  printf("Communications:\n");
  if (cam->FD < 0) 
    printf("  TCP Socket CLOSED\n");
  else
    printf("  TCP Socket OPEN\n");

  printf("  azcam Server: %s:%d\n",cam->Host,cam->Port);
  printf("  Comm Timeout: %d seconds\n",cam->Timeout);
  switch(cam->State) {
  case IDLE:
    printf("  Server State: IDLE\n");
    break;
  case EXPOSING:
    printf("  Server State: EXPOSURE in progress\n");
    break;
  case READOUT:
    printf("  Server State: READOUT in progress\n");
    break;
  case PAUSE:
    printf("  Server State: Exposure PAUSEd\n");
    break;
  default:
    printf("  Server State: UNKNOWN\n");
    break;
  }

  printf("Camera Configuration:\n");
  printf("  CCD Config File: %s\n",cam->IniFile);

  switch(cam->ShutterMode) {
  case DARK_IMAGE:
    printf("  Shutter Mode: Always Closed\n");
    break;
  case LIGHT_IMAGE:
    printf("  Shutter Mode: Open During Exposures\n");
    break;
  case TDI:
    printf("  Shutter Mode: Open During Readout\n");
    break;
  default:
    printf("  Shutter Mode: UNKNOWN\n");
    break;
  }
  printf("  Shutter: %s\n",
	 ((cam->Shutter) ? "Open" : "Closed"));
  
  switch (cam->Readout) {
  case IMMEDIATE:
    printf("  Readout Mode: IMMEDIATE\n");
    break;
  case DEFERRED:
    printf("  Readout Mode: DEFERRED\n");
    break;
  default:
    printf("  Readout Mode: UNKNOWN\n");
    break;
  }

  printf("  CCD ROI: [%d:%d,%d:%d]  Binning: %dx%d\n",
	 cam->FirstCol,cam->LastCol,
	 cam->FirstRow,cam->LastRow,
	 cam->ColBin,cam->RowBin);

  printf("  CCD Format: %d %d %d %d %d %d %d %d %d\n",
	 cam->NCtotal,cam->NCpredark,cam->NCunderscan,cam->NCoverscan,
	 cam->NRtotal,cam->NRpredark,cam->NRunderscan,cam->NRoverscan,
	 cam->NRframexfer);

  switch(cam->ReadMode) {
  case SINGLE_AMP:
    printf("  Readout Config: Single Amp\n");
    break;
  case TWO_AMP_PARALLEL:
    printf("  Readout Config: 2-Amp Parallel\n");
    break;
  case TWO_AMP_SERIAL:
    printf("  Readout Config: 2-Amp Serial\n");
    break;
  case FOUR_AMP_QUAD:
    printf("  Readout Config: 4-Amp Quad\n");
    break;
  case MOSAIC:
    printf("  Readout Config: MOSAIC\n");
    break;
  default:
    printf("  Readout Config: UNKNOWN\n");
    break;
  }

  switch(cam->Splits) {
  case NO_SPLIT:
    printf("  No Split Register\n");
    break;
  case SPLIT_SERIAL:
    printf("  Split Serial Registers\n");
    break;
  case SPLIT_PARALLEL:
    printf("  Split Parallel Registers\n");
    break;
  case SPLIT_QUAD:
    printf("  Quad Splits\n");
    break;
  default:
    printf("  Unknown Register Split\n");
    break;
  }

  printf("  Number of Detectors: %d x %d\n",
	 cam->NumDetX, cam->NumDetY);
  printf("  Amplifier Configuration: %s\n",cam->AmpConfig);
  printf("  Next Readout: %d x %d pixels (%d pixels total)\n",
	 cam->Ncols,cam->Nrows,cam->Npixels);

  printf("CCD Temperature Control:\n");
  printf("  SetPoint: %.1f C\n",cam->SetPoint);
  printf("  Last CCD Temperature: %.1f C\n",cam->CCDTemp);
  printf("  Last Dewar Temperature: %.1f C\n",cam->DewarTemp);
  switch(cam->DiodeSensor) {
  case DT670:
    printf("  Diode Sensor: DT670\n");
    break;
  case AD590:
    printf("  Diode Sensor: AD590\n");
    break;
  case N914:
    printf("  Diode Sensor: 1N914\n");
    break;
  default:
    printf("  Diode Sensor: UNKNOWN\n");
    break;
  }
  switch(cam->CCDSensor) {
  case DT670:
    printf("  CCD Sensor: DT670\n");
    break;
  case AD590:
    printf("  CCD Sensor: AD590\n");
    break;
  case N914:
    printf("  CCD Sensor: 1N914\n");
    break;
  default:
    printf("  CCD Sensor: UNKNOWN\n");
    break;
  }
  switch(cam->DewarSensor) {
  case DT670:
    printf("  Dewar Sensor: DT670\n");
    break;
  case AD590:
    printf("  Dewar Sensor: AD590\n");
    break;
  case N914:
    printf("  Dewar Sensor: 1N914\n");
    break;
  default:
    printf("  Dewar Sensor: UNKNOWN\n");
    break;
  }
  printf("Exposure & File Information:\n");
  printf("  Exposure Time: %.3f sec\n",cam->ExpTime);
  printf("  Next Filename: %s/%s.%0.4d.fits\n",
	 cam->FilePath,cam->FileName,cam->FileNum);
  printf("  Last Filename: %s\n",cam->LastFile);
  switch(cam->FileFormat) {
  case STDFITS:
    printf("  File Format: Standard FITS\n");
    break;
  case EXTFITS:
    printf("  File Format: Multi-Extension FITS\n");
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

/*!
  \brief Send a raw command to the Archon controller
  
  \param cam pointer to an #azcam struct with the server parameters
  \param cmdstr command string to send
  \param reply string to contain any reply text
  \return 0 if successful, -1 on errors, with error text in reply

  Sends a raw Archon Controller command to the azcam server.  Command
  strings are sent as-is with no validation, so if there is a syntax
  error on the server side, this function will report an error back to
  the calling application.

  A complete list of valid Archon controller commands may be found
  in the Archon manual (http://www.sta-inc.net/archonmanual) available
  from the STA website.

*/

int
archonCmd(azcam_t *cam, char *cmdstr, char*reply)
{
  char msgstr[128];

  memset(msgstr,0,sizeof(msgstr));

  if (azcamCmd(cam,cmdstr,msgstr)<0) {
    strcpy(reply,msgstr);
    return -1;
  }

  if (strlen(msgstr)==0)
    strcpy(reply,"OK");
  else
    sprintf(reply,"OK %s",msgstr);

  return 0;

}
