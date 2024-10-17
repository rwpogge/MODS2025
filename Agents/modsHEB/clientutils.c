//
// clientutils - client app utilities go here
//

/*!
  \file clientutils.c
  \brief Client application utility functions

  Because they have to go somewhere...

  \author R. Pogge, OSU Astronomy Dept. (pogge@astronomy.ohio-state.edu)
  \date 2010 June 21
*/

#include "client.h" // custom client application header 

//---------------------------------------------------------------------------

/*!
  \brief Initialize a MODS instrument environmental sensor data structure

  \param envi pointer to an #envdata_t data structure

  Initializes the enviromental sensor data structure with sensible defaults. This ensures 
  that we have some way to avoid data structures with delinquent information. 

  After using this function, the env data structure will need to be closed using freeEnvData().
*/
void initEnvData(envdata_t *envi){
  envi->cadence = DEFAULT_CADENCE;  // default monitoring cadence (see client.h)
  envi->pause = 0;                  // start running (no pause)

  envi->doLogging = 1;              // enable enviromental data logging by default
  envi->useHdf5 = 0;                // default: do not output enviornmental data to Hdf5
  envi->hdfInitalized = 0;

  strcpy(envi->hebAddr,"");

  //Dynamically creating an empty device array.
  initDeviceData(envi);

  //Dynamically creating the HDF telemetry array.
  envi->floatMeasures = new lbto::tel::float_measure::buf_proxy[NUM_DEVICES];

  //Setting all of the strings to their default values.
  strcpy(envi->modsID,"None");
  strcpy(envi->logRoot,ENV_LOGS);
  strcpy(envi->hdfRoot,HDF_LOGS);
  strcpy(envi->leapSecondsFile,LEAP_SECONDS_FILE);
  strcpy(envi->logFile,"");
  strcpy(envi->lastDate,"");
  strcpy(envi->utcDate,"");

  //Clearing the log file variable.
  if (envi->logFD > 0) close(envi->logFD);  // just in case...
  envi->logFD = 0;
}

/*!
  \brief Clears a MODS instrument environmental sensor data structure

  \param envi pointer to an #envdata_t data structure

  Frees all dynamically allocated memory needed by the #envdata_t structure.
*/
void freeEnvData(envdata_t *envi){
  freeDeviceData(envi);
}

/*!
  \brief Print the contents of the envData struct

  \param envi pointer to an #envdata_t data structure

  Prints the contents of the #envdata_t data structure describing the raw contents of the last TCS query.  
  The Date/Time give when the query occurred.  
*/
void printEnvData(envdata_t *envi){
  if (!useCLI) return;

  printf("Enviromental Monitor Agent Info:\n");
  printf("  Instrument: %s\n",envi->modsID);
  printf("  Monitor Status: %s\n",(envi->pause) ? "PAUSED" : "Active");
  printf("  Sampling Cadence: %d seconds\n",envi->cadence);
  printf("  Data Logging: %s\n",(envi->doLogging) ? "Enabled" : "Disabled");
  envi->logFD == 0 ? printf("  Data Log File: CLOSED\n") : printf("  Data Log File: %s\n",envi->logFile);
  printf("  HDF Logging: %s\n",(envi->useHdf5) ? "Enabled" : "Disabled");
  envi->useHdf5 == 0 ? printf("  Hdf Log Directory: CLOSED\n") : printf("  Hdf Log Directory: %s\n",envi->hdfRoot);
  printf("Client Info:\n");
  printf("  Mode: %s\n",(client.useISIS) ? "ISIS client" : "Standalone");
  printf("  ISIS Host: %s (%s:%d)\n",client.ID,client.Host,client.Port);
  printf("  Config File: %s\n",client.rcFile);
  printf("  %s\n",(client.isVerbose) ? "Verbose" : "Concise");
}