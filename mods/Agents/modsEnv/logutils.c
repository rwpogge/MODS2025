//
// logutils - Functions used to export log files go here.
//

/*!
  \file logutils.c
  \brief Functions used to export ASCII and HDF5 log files go here

  \author R. Pogge, OSU Astronomy Dept. (pogge@astronomy.ohio-state.edu)
  \author X. Carroll, OSU.
  \date 2010 June 21
*/

#include "client.h" // custom client application header 

/*!
  \brief Initialize the telemetry data structures used for HDF5 output.

  \param envi pointer to an #envdata_t data structure

  \return 0 on success, -1 on errors.

  Initializes the lib-telemetry structures used for outputing telemetry data to an 
  HDF5 file. If HDF5 is being used, then this function is called on initalization.
  If HDF5 is activated, then this function is called.

*/
int initTelemetryData(envdata_t* envi){
  if(envi->hdfInitalized) return 0;
  envi->hdfInitalized = 1;

  try{
    //Initalizing telemetry.
    std::shared_ptr<lbto::tel::ambassador> callBack(new TelemetryCallback());
    lbto::tel::collection_manager::init(callBack);

    //Defining a telemetry stream with modlue name "tel" and stream name "modsenv".
    lbto::tel::telemeter_definer modsDefiner = lbto::tel::collection_manager::instance().make_telemeter_definer(
      lbto::tel::system(lbto::tel::name("tel")), lbto::tel::name("modsenv")
    );

    //Adding measures to the telemetry.
    modsDefiner.add_child(lbto::tel::float_measure(
      envi->ambientTempMeasure, 
      lbto::tel::unit::celsius(), 
      lbto::tel::name("ambientTemp"),
      lbto::tel::description("Outside ambient air temperature in degrees C")
    ));
    modsDefiner.add_child(lbto::tel::float_measure(
      envi->glycolSupplyPresMeasure, 
      lbto::tel::unit::kilopascal(), 
      lbto::tel::name("glycolSupplyPres"),
      lbto::tel::description("Instrument glycol supply pressure in psi-g")
    ));
    modsDefiner.add_child(lbto::tel::float_measure(
      envi->glycolReturnPresMeasure, 
      lbto::tel::unit::kilopascal(), 
      lbto::tel::name("glycolReturnPres"),
      lbto::tel::description("Instrument glycol return pressure in psi-g")
    ));
    modsDefiner.add_child(lbto::tel::float_measure(
      envi->glycolSupplyTempMeasure, 
      lbto::tel::unit::celsius(), 
      lbto::tel::name("glycolSupplyTemp"),
      lbto::tel::description("Instrument glycol supply temperature in degrees C")
    ));
    modsDefiner.add_child(lbto::tel::float_measure(
      envi->glycolReturnTempMeasure, 
      lbto::tel::unit::celsius(), 
      lbto::tel::name("glycolReturnTemp"),
      lbto::tel::description("Instrument glycol return temperature in degrees C")
    ));
    modsDefiner.add_child(lbto::tel::float_measure(
      envi->utilBoxTempMeasure, 
      lbto::tel::unit::celsius(), 
      lbto::tel::name("utilBoxTemp"),
      lbto::tel::description("IUB inside-box air temperature in degrees C")
    ));
    modsDefiner.add_child(lbto::tel::float_measure(
      envi->agwHSTempMeasure, 
      lbto::tel::unit::celsius(), 
      lbto::tel::name("agwHSTemp"),
      lbto::tel::description("AGw camera controller heat sink temperature in degrees C")
    ));
    modsDefiner.add_child(lbto::tel::float_measure(
      envi->iebBAirTempMeasure, 
      lbto::tel::unit::celsius(), 
      lbto::tel::name("iebB_AirTemp"),
      lbto::tel::description("Blue IEB box air temperature in degrees C")
    ));
    modsDefiner.add_child(lbto::tel::float_measure(
      envi->iebBReturnTempMeasure, 
      lbto::tel::unit::celsius(), 
      lbto::tel::name("iebB_ReturnTemp"),
      lbto::tel::description("Blue IEB glycol return temperature in degrees C")
    ));
    modsDefiner.add_child(lbto::tel::float_measure(
      envi->iebRAirTempMeasure, 
      lbto::tel::unit::celsius(), 
      lbto::tel::name("iebR_AirTemp"),
      lbto::tel::description("Red IEB box air temperature in degrees C")
    ));
    modsDefiner.add_child(lbto::tel::float_measure(
      envi->iebRReturnTempMeasure, 
      lbto::tel::unit::celsius(), 
      lbto::tel::name("iebR_ReturnTemp"),
      lbto::tel::description("Red IEB glycol return temperature in degrees C")
    ));
    modsDefiner.add_child(lbto::tel::float_measure(
      envi->airTopTempMeasure, 
      lbto::tel::unit::celsius(), 
      lbto::tel::name("airTopTemp"),
      lbto::tel::description("MODS instrument Top inside air temperature in degrees C")
    ));
    modsDefiner.add_child(lbto::tel::float_measure(
      envi->airBotTempMeasure, 
      lbto::tel::unit::celsius(), 
      lbto::tel::name("airBotTemp"),
      lbto::tel::description("MODS instrument Bottom inside air temperature in degrees C")
    ));
    modsDefiner.add_child(lbto::tel::float_measure(
      envi->trussTopTempMeasure, 
      lbto::tel::unit::celsius(), 
      lbto::tel::name("trussTopTemp"),
      lbto::tel::description("MODS collimator truss tube Top temperature in degrees C")
    ));
    modsDefiner.add_child(lbto::tel::float_measure(
      envi->trussBotTempMeasure, 
      lbto::tel::unit::celsius(), 
      lbto::tel::name("trussBotTemp"),
      lbto::tel::description("MODS collimator truss tube Bottom temperature in degrees C")
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
  if(!envi->hdfInitalized) return;
  if(envi->modsCollector == NULL) return;

  while(envi->modsCollector->count_buffered_samples() != 0){
    sleep(1);
  }

  envi->modsCollector.reset();
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
    sprintf(logStr,"# UTC Date/Time      Tamb  Psup  Pret  Tsup  Tret  Tiub"
	    "  Tagw  Bair  Bret  Rair  Rret  AirT  AirB  ColT  ColB\n"
    );
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
  sprintf(logStr,"%s %5.1f %5.1f %5.1f %5.1f %5.1f %5.1f %5.1f %5.1f %5.1f"
	  " %5.1f %5.1f %5.1f %5.1f %5.1f %5.1f\n",
	  envi->utcDate, envi->ambientTemp,
	  envi->glycolSupplyPres, envi->glycolReturnPres,
	  envi->glycolSupplyTemp, envi->glycolReturnTemp,
	  envi->utilBoxTemp, envi->agwHSTemp,
	  envi->iebB_AirTemp, envi->iebB_ReturnTemp,
	  envi->iebR_AirTemp, envi->iebR_ReturnTemp,
	  envi->airTopTemp, envi->airBotTemp,
	  envi->trussTopTemp, envi->trussBotTemp
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

  //Check if telemetry has been initalized.
  if(initTelemetryData(&env) != 0){
    printf("Telemetry could not be started - hdf5 will not be used.\n");
    env.useHdf5 = 0;
  }

  try{    
    //Storing variable data in the streams.
    envi->ambientTempMeasure.store(envi->ambientTemp);
    envi->glycolSupplyPresMeasure.store(envi->glycolSupplyPres);
    envi->glycolReturnPresMeasure.store(envi->glycolReturnPres);
    envi->glycolSupplyTempMeasure.store(envi->glycolSupplyTemp);
    envi->glycolReturnTempMeasure.store(envi->glycolReturnTemp);
    envi->utilBoxTempMeasure.store(envi->utilBoxTemp);
    envi->agwHSTempMeasure.store(envi->agwHSTemp);
    envi->iebBAirTempMeasure.store(envi->iebB_AirTemp);
    envi->iebBReturnTempMeasure.store(envi->iebB_ReturnTemp);
    envi->iebRAirTempMeasure.store(envi->iebR_AirTemp);
    envi->iebRReturnTempMeasure.store(envi->iebR_ReturnTemp);
    envi->airTopTempMeasure.store(envi->airTopTemp);
    envi->airBotTempMeasure.store(envi->airBotTemp);
    envi->trussTopTempMeasure.store(envi->trussTopTemp);
    envi->trussBotTempMeasure.store(envi->trussBotTemp);
    
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