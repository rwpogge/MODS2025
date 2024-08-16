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
*/
void initEnvData(envdata_t *envi){
  strcpy(envi->modsID,"None");
  envi->cadence = DEFAULT_CADENCE;  // default monitoring cadence (see client.h)
  envi->pause = 0;                  // start running (no pause)

  envi->doLogging = 1;              // enable enviromental data logging by default
  envi->useHdf5 = 0;                // default: do not output enviornmental data to Hdf5
  envi->hdfInitalized = 0;
  strcpy(envi->logRoot,ENV_LOGS);
  strcpy(envi->hdfRoot,HDF_LOGS);
  strcpy(envi->leapSecondsFile,LEAP_SECONDS_FILE);
  strcpy(envi->logFile,"");
  strcpy(envi->lastDate,"");
  strcpy(envi->utcDate,"");
  if (envi->logFD > 0) close(envi->logFD);  // just in case...
  envi->logFD = 0;
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

  printf("\n NOTE: Data above could be stale, note the date/time above\n");
  printf("\n");
}

//Converts an array of raw RTD data into its equivalent Temperature.
void ptRTD2C(uint16_t* rawData, float* outputData, int numRtds){
  float tempRes = 0.1;
  float tempMax = 850.0;
  float wrapT = tempRes*(pow(2.0, 16)-1);
    
  for(int i=0; i<numRtds; i++){
      float temp = tempRes*rawData[i];
      if (temp > tempMax)
        temp -= wrapT;
      
      outputData[i] = temp;
  }
}

//Converts an array of raw quadcell data into its equivalent DC voltage.
void qc2vdc(uint16_t* rawData, float* outputData){
  for(int i=0; i<4; i++){
    int posMax = pow(2, 15) - 1;
    int negMin = pow(2, 16) - 2;
    
    if(rawData[i] > posMax)
        outputData[i] = 10.0*((rawData[i]-negMin)/posMax);
    else
        outputData[i] = 10.0*((float)rawData[i]/posMax);
  }
}

/*!
  \brief Get enviromental sensor data

  \param envi pointer to an #envdata_t struct

  \return 0 on success, -1 on errors.  

  Gets data from the instrument enviromental sensors and loads the results into the enviromental data 
  structure.
*/
int getEnvData(envdata_t *envi) {
  int ierr;

  //Query WAGOs and collect data here.
  uint16_t rawHebData[5];
  wagoSetGet(0, "192.168.139.135", 0, 5, rawHebData);
  ptRTD2C(rawHebData+4, envi->rtdData, 1);
  qc2vdc(rawHebData, envi->quadcellData);

  // Get the UTC date/time of the query (ISIS client utility routine)
  strcpy(envi->utcDate,ISODate());

  // All done (logging is done by the calling program)
  return 0;
}