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

  Initializes the enviromental sensor data structure with sensible
  defaults.  This ensures that we have some way to avoid data
  structures with delinquent information.

*/
void initEnvData(envdata_t *envi){
  strcpy(envi->modsID,"None");
  envi->cadence = DEFAULT_CADENCE;  // default monitoring cadence (see client.h)
  envi->pause = 0;                  // start running (no pause)
  strcpy(envi->heb_Addr,"");
  envi->ambientTemp = 0.0;
  envi->quadcell[0] = 0.0;
  envi->quadcell[1] = 0.0;
  envi->quadcell[2] = 0.0;
  envi->quadcell[3] = 0.0;

  envi->doLogging = 1;                      // enable enviromental data logging by default
  envi->useHdf5 = 0;                        // default: do not output enviornmental data to Hdf5
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
  \brief Initialize the telemetry data structures used for HDF5 output.

  \param envi pointer to an #envdata_t data structure

  \return 0 on success, -1 on errors.

  Initializes the lib-telemetry structures used for outputing telemetry data to an 
  HDF5 file. If HDF5 is being used, then this function is called on initalization.

  This function can only be called once.

*/
int initTelemetryData(envdata_t* envi){
  try{
    //Initalizing telemetry.
    std::shared_ptr<lbto::tel::ambassador> callBack(new TelemetryCallback());
    lbto::tel::collection_manager::init(callBack);

    //Defining a telemetry stream with modlue name "mods" and stream name "modsenv".
    lbto::tel::telemeter_definer modsDefiner = lbto::tel::collection_manager::instance().make_telemeter_definer(
      lbto::tel::system(lbto::tel::name("tel")), lbto::tel::name("mods")
    );

    //Adding measures to the telemetry.
    modsDefiner.add_child(lbto::tel::float_measure(
      envi->ambientTempMeasure, 
      lbto::tel::unit::celsius(), 
      lbto::tel::name("ambientTemp"),
      lbto::tel::description("Temperature from the in-box sensor.")
    ));
    modsDefiner.add_child(lbto::tel::float_measure(
      envi->quadcellMeasure[0], 
      lbto::tel::unit::volt(), 
      lbto::tel::name("quadcellreading0"),
      lbto::tel::description("Readings from the quadcell.")
    ));
    modsDefiner.add_child(lbto::tel::float_measure(
      envi->quadcellMeasure[1], 
      lbto::tel::unit::volt(), 
      lbto::tel::name("quadcellreading1"),
      lbto::tel::description("Readings from the quadcell.")
    ));
    modsDefiner.add_child(lbto::tel::float_measure(
      envi->quadcellMeasure[2], 
      lbto::tel::unit::volt(), 
      lbto::tel::name("quadcellreading2"),
      lbto::tel::description("Readings from the quadcell.")
    ));
    modsDefiner.add_child(lbto::tel::float_measure(
      envi->quadcellMeasure[3], 
      lbto::tel::unit::volt(), 
      lbto::tel::name("quadcellreading3"),
      lbto::tel::description("Readings from the quadcell.")
    ));

    //Adding this definition to the telemetry store.
    envi->modsCollector.reset(new lbto::tel::collector(modsDefiner.make_definition(), MAX_TELEMETRY_BUFFER_BYTES, false));


  //Catching exceptions thrown while initalizing lib-telemetry data.
  }catch(std::exception const& exn){
      printf("initTelemetryData() exception: %s\n", exn.what());
      return -1;
  }catch(...){
      printf("initTelemetryData() unknown exception.\n");
      return -1;
  }

  return 0;
}

/*!
  \brief Cleanly closes the telemetry data structures used for HDF5 output.

  \param envi pointer to an #envdata_t data structure

  If telemetry is being used this function should be called once before the program 
  terminates.
*/
void closeTelemetryData(envdata_t* envi){
  while(envi->modsCollector->count_buffered_samples() != 0){
    sleep(1);
  }

  envi->modsCollector.reset();
}

/*!
  \brief Print the contents of the envData struct

  \param envi pointer to an #envdata_t data structure

  Prints the contents of the #envdata_t data structure describing the
  raw contents of the last TCS query.  The Date/Time give when
  the query occurred.  

*/
void printEnvData(envdata_t *envi){
  if (!useCLI) return;

  printf("Enviromental Monitor Agent Info:\n");
  printf("  Instrument: %s\n",envi->modsID);
  printf("      HEB WAGO IP Address: %s\n",envi->heb_Addr);
  printf("  Monitor Status: %s\n",(envi->pause) ? "PAUSED" : "Active");
  printf("  Sampling Cadence: %d seconds\n",envi->cadence);
  printf("  Data Logging: %s\n",(envi->doLogging) ? "Enabled" : "Disabled");
  envi->logFD == 0 ? printf("  Data Log File: CLOSED\n") : printf("  Data Log File: %s\n",envi->logFile);
  printf("Most Recent Sensor Data:\n");
  printf("  Date/Time: UTC %s\n",envi->utcDate);
  printf("  Ambient Temp: %.1f C\n",envi->ambientTemp);
  printf("  Quadcell: 1:%f 2:%f 3:%f 4:%f\n", envi->quadcell[0], envi->quadcell[1], envi->quadcell[2], envi->quadcell[3]);
  printf("Client Info:\n");
  printf("  Mode: %s\n",(client.useISIS) ? "ISIS client" : "Standalone");
  printf("  ISIS Host: %s (%s:%d)\n",client.ID,client.Host,client.Port);
  printf("  Config File: %s\n",client.rcFile);
  printf("  %s\n",(client.isVerbose) ? "Verbose" : "Concise");

  printf("\n NOTE: Data above could be stale, note the date/time above\n");
  printf("\n");
}

/*!
  \brief Get enviromental sensor data

  \param envi pointer to an #envdata_t struct

  \return 0 on success, -1 on errors.  

  Gets data from the instrument enviromental sensors and loads
  the results into the enviromental data structure

*/
int getEnvData(envdata_t *envi) {
  int ierr;
  uint16_t hebData[5];  // raw HEB WAGO data array

  // Get data from the HEB environmental sensors
  ierr = wagoSetGet(0,envi->heb_Addr,0,5,hebData);
  if (ierr != 0) {
    if (useCLI) printf("WARNING: %s HEB WAGO read error",envi->modsID);
    return ierr;
  }

  qc2vdc(hebData, envi->quadcell);
  ptRTD2C(hebData+4, &(envi->ambientTemp), 1);
  
  // Get the UTC date/time of the query (ISIS client utility routine)
  strcpy(envi->utcDate,ISODate());

  // All done (logging is done by the calling program)
  return 0;
}

/*!
  \brief Initialize the environmental data log

  \param envi pointer to an #envdata_t data structure

  \return 0 on success, -1 on errors

  Opens the environmental data log with the appropriate UTC date tag.
  Log entries are created by the logEnvData() function.

*/
int initEnvLog(envdata_t *envi){
  int ierr;
  char logStr[MED_STR_SIZE];
  int exists;

  // Get the UTC Date in CCYYMMDD format
  strcpy(envi->lastDate,UTCDateTag());
  
  // Build the log file name from the /path/rootname provided
  // at runtime, and see if it already exists.
  sprintf(envi->logFile,"%s.%s.log",envi->logRoot,envi->lastDate);
  exists = fileExists(envi->logFile);

  // Open the new runtime log file, append to it if it exists 
  if (envi->logFD > 0) close(envi->logFD);

  envi->logFD = open(envi->logFile,(O_WRONLY|O_CREAT|O_APPEND));

  if (envi->logFD == -1) {
    printf("ERROR: cannot open environmental data log file %s - %s\n",
	    envi->logFile,strerror(errno));
    envi->doLogging == 0; // disable logging to prevent noise...
    return -1;
  }
  chmod(envi->logFile,0666);

  // If this is the first time the file has been opened, print the
  // log header, otherwise just note the log has been restarted
  if (exists)
    logMessage(envi,"Monitor agent re-started");
  else {
    // This is the first time we've had this file open, write the
    // detailed file header.
    memset(logStr,0,sizeof(logStr));
    sprintf(logStr,"#\n# %s Environmental Sensor Data Log\n#\n",envi->modsID);
    ierr = write(envi->logFD,logStr,strlen(logStr));

    memset(logStr,0,sizeof(logStr));
    sprintf(logStr,"# Started: UTC %s\n# Sampling Cadence: %d seconds\n#\n",
	    ISODate(),envi->cadence);
    ierr = write(envi->logFD,logStr,strlen(logStr));

    memset(logStr,0,sizeof(logStr));
    sprintf(logStr,"# UTC Date/Time      Tamb Qcl0     Qcl1     Qcl2     Qcl3\n");
    ierr = write(envi->logFD,logStr,strlen(logStr));
  }
  return 0;
}

/*!
  \brief Append data to the environmental data log

  \param envi pointer to an #envdata_t data structure

  \return 0 on success, -1 on errors

  Appends the most recent environmental sensor readings to the current
  data log.  This routine checks the time and rolls over the data log
  at midnight UTC.

  At present (v2.x) we do not log AC power status sensor data, only
  the data from temperature and pressure sensors.

*/
int logEnvData(envdata_t *envi){
  int ierr;
  char dateTag[MED_STR_SIZE];  
  char logStr[BIG_STR_SIZE];

  // If logging is disabled, return now
  if (!envi->doLogging) return 0;

  // Did we cross over into the next day?  If so, we need to close the
  // current log file and start a new one.
  strcpy(dateTag,UTCDateTag());
  if (strcasecmp(dateTag,envi->lastDate) != 0)
    if (initEnvLog(envi)<0) return -1;

  // Append the current enviromental sensor data to the data log
  memset(logStr,0,sizeof(logStr));
  sprintf(logStr,"%s %5.1f %5.5f, %5.5f, %5.5f, %5.5f\n", 
    envi->utcDate, envi->ambientTemp, envi->quadcell[0], envi->quadcell[1], envi->quadcell[2], envi->quadcell[3]
  );
  ierr = write(envi->logFD,logStr,strlen(logStr));
  
  return 0;
}

/*!
  \brief Append a message to the environmental data log

  \param envi pointer to an #envdata_t data structure
  \param msgStr string with the message to append

  \return 0 on success, -1 on errors

  Appends a message to the current data log. The message is offset
  by a # to mark it as a "comment", and tagged with the UTC date/time.
  This function is used, for example, to note changes in the sampling
  cadence.

  \sa initEnvLog(), logEnvData()
*/
int logMessage(envdata_t *envi, char *msgStr){
  int ierr;
  char logStr[BIG_STR_SIZE];

  // If logging is disabled, return now
  if (!envi->doLogging) return 0;

  // If the message string is blank, return now
  if (strlen(msgStr) == 0) return 0;

  // Append the message as a comment to the enviromental sensor data log
  memset(logStr,0,sizeof(logStr));
  sprintf(logStr,"# %s %s\n",ISODate(),msgStr);
  ierr = write(envi->logFD,logStr,strlen(logStr));
  
  return 0;
}

/*!
  \brief Test to see if a file exists

  \param fileName string with the full filename

  \return 1 if it exists, 0 if not, -1 on errors

  Tests to see if a file exists.  This is a utility which will let us
  avoid overwriting a header file in a data log, for example, or messing
  with a non-regular file.

*/
int fileExists(char *fileName){
  struct stat statBuf;
  int itype;

  if (strlen(fileName)==0) return -1;
  if (stat(fileName,&statBuf)<0) {
    switch (errno) {
    case ENOENT:
      return 0;
      break;
    default:
      return -1;
      break;
    }
  }

  // It exists, now check its type.  If not a regular file, return the
  // type code as -(itype)

  itype = (statBuf.st_mode & S_IFMT);

  if (itype == S_IFREG)
    return 1;
  else
    return -itype;
}

/*!
  \brief Append data to the HDF5 telemetry log.

  \param envi pointer to an #envdata_t data structure

  \return 0 on success, -1 on errors

  Appends the most recent environmental sensor readings to the current
  HDF5 stream.

  At present (v2.x) we do not log AC power status sensor data, only
  the data from temperature and pressure sensors.

*/
int logTelemetryData(envdata_t *envi){
  time_t commitTime = time(NULL);

  try{    
    //Storing variable data in the streams.
    envi->ambientTempMeasure.store(envi->ambientTemp);
    for(int i=0; i<4; i++) envi->quadcellMeasure[i].store(envi->quadcell[i]);

    //Commiting the data to the HDF5 file.
    envi->modsCollector->commit_sample(lbto::tel::date::from_posix_utc_s(commitTime));    //TODO: Fix time stamps.

  //Catching exceptions thrown while sending lib-telemetry data to an output stream.
  }catch(lbto::tel::sample_dropped const& exn){
    printf("Sample Dropped: %s\n", exn.what());
    return -1;
  }catch(std::exception const& exn){
    printf("logTelemetryData() exception: %s\n", exn.what());
    return -1;
  }catch (...){
    printf("logTelemetryData() unknown exception.\n");
    return -1;
  }

  return 0;
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