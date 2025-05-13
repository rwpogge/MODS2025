/*!
  \file loadconfig.c
  \brief Load/Parse ISIS client's runtime configuration file.

  ISIS-style runtime configuration files (e.g., named myclient.ini,
  .myclientrc, whatever) contain simple Keyword-Value pairs that are
  parsed into global-scope variables for the client and its various
  subroutines to use.
 
  The # is used as a comment character, making a comment line when it
  appears as the first character in a line by itself.  Inline comments
  are not supported by this simple parser. We adopt the convention that 
  keywords and values are case insensitive, to remove any ambiguity.
 
  \author R. Pogge, OSU Astronomy Dept. (pogge@astronomy.ohio-state.edu)
  \author X. Carroll
  \date 2024 August 14
*/

#include "client.h"   // Custom client application header file


// UTILITY FUNCTIONS -----------------------------------------------

/*!
  \brief Ensure defaults are set for most values in the env data struct.
  \param cfgFP A file pointer to an open configuration file.
  
  If we need to initialize any default parameter values, do it here.
  Note that as-written these variables have been defined in global scope
  for the entire client application, e.g., in main.c for the
  application.
*/
void loadDefaultConfigs(char *cfgfile){
  // Record the runtime config file in use.
  strcpy(client.rcFile,cfgfile);

  // ISIS server information (Defaults defined in the client.h header):
  client.useISIS = 0;  // default: STANDALONE mode rather than an ISIS client
  strcpy(client.isisHost,DEFAULT_ISISHOST); 
  client.isisPort = DEFAULT_ISISPORT;       
  strcpy(client.isisID,DEFAULT_ISISID);     

  // Client information (defaults in client.h):
  strcpy(client.ID,DEFAULT_MYID);   // client default ISIS node name
  client.Port = DEFAULT_MYPORT;     // client default port number
  
  // Client hostname
  gethostname(client.Host,sizeof(client.Host));   

  // Client runtime parameters
  client.doLogging = 0;            // default: runtime logging enabled 
  strcpy(client.logFile,ENV_LOGS); // default client runtime log filename

  client.isVerbose = 0;            // default: not verbose (concise)
  client.Debug = 0;                // default: no debugging

  // Reset the client global data structures to the compile-time defaults
  initEnvData(&env);

  // default: interactive shell enabled (also set in main)
  useCLI = 1;
}

/*!
  \brief Determines how many WAGO modules are defined in the config file
  \param cfgFP A file pointer to an open configuration file.
  \return The number of wago modules in the file.

  This function will reset the file pointer to the start of the file upon completion.
*/
int numWagoModules(FILE* cfgFP){
  char keyword[MAXCFGLINE];  // File is organized into KEYWORD VALUE pairs
  char argList[MAXCFGLINE];  // Generic argument list string
  char inStr[MAXCFGLINE];    // Generic input string
  int numModules = 0;
  
  // Do a pass of the file looking for occourences of the WAGO keyword
  while(fgets(inStr, MAXCFGLINE, cfgFP)) {
    // Skip comments (#) and blank lines
    if ((inStr[0]!='#') && (inStr[0]!='\n') && inStr[0]!='\0'){
      inStr[MAXCFGLINE] ='\0';
      sscanf(inStr,"%s %[^\n]",keyword,argList);

      if (strcasecmp(keyword,"WAGO")==0){
        numModules++;
      }
    }
	}

  //Reset the file pointer to the start of the file.
  rewind(cfgFP);

  //Returning the number of modules present.
  return numModules;
}

/*!
  \brief Should be used instead of atoi.
  \param str The string which should be converted to an integer.
  \param resultInt A pointer to the integer which will be set as a result of this function.
  \return 0 on success, <0 on failure.

  This function will convert a string value to an integer value, handling errors. 
  This function should be used instead of atoi, but note the different usage. 
  strToInt returns an error code, and takes an additional parameter for the resulting int value.
*/
int strToInt(char* str, int* resultInt){
  char* end;
  long resultLong;
  errno = 0;

  // This function uses strtol rather than atoi, because it has superior error handling.
  // But, strtol returns a long value, so it needs casted to an integer.

  // Getting the long value of the string.
  resultLong = strtol(str, &end, 10);

  // Checking for errors.
  if(end == str || *end != '\0') return -1;                             // Not a number.
  else if(resultLong > INT_MAX || resultLong < INT_MIN) return -2;      // Outside of Int range.

  // Setting the result integer.
  *resultInt = (int)resultLong;

  return 0;
}

/*!
  \brief Acts as an alias for printing an error message and closing a file. 
  \param cfgFp The file to close.
*/
void warnAndClose(const char* key, char* argStr, FILE* cfgFP){
  printf("ERROR: %s option '%s' unrecognized\n",key, argStr);
  printf("Aborting - fix the config file (%s) and try again\n",client.rcFile);
	if (cfgFP !=0) fclose(cfgFP);
}


/*!
  \brief Used to parse lines that start with WAGO, and the DEVICE lines that come after.
  \param cfgFP A pointer the config file.
  \param inStr The input string from the loadConfig loop.
  \param argStr The argument token string from the loadConfig loop.
  \param moduleIndex The current module index (number of times the WAGO string has appeared so far).
  \param maxDeviceOffset The current highest offset seen on any device.
  \return 0 on success, <0 on failure.

  This function will parse a WAGO module that is in the following format:
    WAGO <ModuleName> <ModuleType> <BaseAddress> <NumDevices>
    DEVICE <Name1> <DeviceOfset1> <ShouldLog1>
    DEVICE <Name2> <DeviceOfset2> <ShouldLog2>
    ...
  
  If the module is of type DO, you also need to include if the device is NO/NC:
    WAGO <ModuleName> DO <BaseAddress> <NumDevices>
    DEVICE <Name1> <DeviceOfset1> <NO/NC> <ShouldLog1>
    DEVICE <Name2> <DeviceOfset2> <NO/NC> <ShouldLog2>
    ...

  An example module is:
    WAGO Power DO 512 3
    DEVICE archon  0 no Y
    DEVICE bog     1 nc Y
    DEVICE ion     2 no N
*/
int parseWagoModule(FILE* cfgFP, char* inStr, char* argStr, int* moduleIndex, int* maxDeviceOffset){
  int errValue; // Used to convert strings to integers
  int i;        // Loop index

  // The current module list object
  device_module_t* currentModule = env.modules+(*moduleIndex);

  // The module name
  GetArg(inStr,2,currentModule->name);

  // The type of module (DO, AI, RTD, etc.)
  GetArg(inStr,3,argStr);
  currentModule->processingType = strToProcessType(argStr);
  if(currentModule->processingType == -1){
    warnAndClose("ProcessType", argStr, cfgFP);
    return -2;
  }

  // The base address of the module
  GetArg(inStr,4,argStr);
  errValue = strToInt(argStr, &(currentModule->baseAddress));
  if(errValue){
    warnAndClose("BaseAddress", argStr, cfgFP);
    return -2;
  } 

  // The number of conneted devices
  GetArg(inStr,5,argStr);
  errValue = strToInt(argStr, &(currentModule->numDevices));
  if(errValue){
    warnAndClose("NumDevices", argStr, cfgFP);
    return -2;
  }

  // Dynamically allocating memory for the devices
  if(currentModule->devices == NULL){
    // NOTE: The C++ 'new' keyword is used instead of 'malloc()' here, because our struct 
    // contains C++ objects which need to be initialized.
    currentModule->devices = new device_t[currentModule->numDevices];
  }

  // For every connected device, there should be a line with additional information.
  for(i=0; (i<currentModule->numDevices && fgets(inStr, MAXCFGLINE, cfgFP)); i++){
    // Skipping blank lines and lines prefixed with the '#' character.
    if ((inStr[0]=='#') || (inStr[0]=='\n')){
      i--;        //This isn't a device.
      continue;   //Get the next line.
    } 

    // Parsing the line
    inStr[MAXCFGLINE] ='\0';

    // The device keyword
    GetArg(inStr,2,argStr);

    if(strcasecmp(argStr,"DEVICE") != 0){
      printf("ERROR: couldn't find the next device in the %s module\n", currentModule->name);
      printf("Aborting - fix the config file (%s) and try again\n", client.rcFile);
      if (cfgFP !=0) fclose(cfgFP);
      return -2;
    }
    
    // The current device list object
    device_t* currentDevice = currentModule->devices+i;

    // The device name
    GetArg(inStr,3,currentDevice->name); 

    // The device offset
    GetArg(inStr,4,argStr);
    errValue = strToInt(argStr, &(currentDevice->address));
    if(errValue){
      warnAndClose("DeviceOffset", argStr, cfgFP);
      return -2;
    }

    // Updating the maximum device offset for this module.
    if(currentDevice->address > currentModule->maxOffset) currentModule->maxOffset = currentDevice->address;

    // Updating the maximum device offset for the whole file.
    if(currentModule->maxOffset > *maxDeviceOffset) *maxDeviceOffset = currentModule->maxOffset;

    int logParam = 5;
    if(currentModule->processingType == DO){
      //We should check for logging one parameter later.
      logParam++;

      //Determine if the device is NO (Normally Open) or NC (Normally Closed).
      currentDevice->nc = 0;
      
      GetArg(inStr,5,argStr);
      if (strcasecmp(argStr,"NC")==0) {
        currentDevice->nc = 1;
      }else if(strcasecmp(argStr,"NO")!=0){
        warnAndClose("DeviceNO/NC", argStr, cfgFP);
        return -2;
      }
    }
    
    // The device logging status
    GetArg(inStr,logParam,argStr);
    if (strcasecmp(argStr,"T")==0 || strcasecmp(argStr,"Y")==0) {
      currentDevice->logEntry = 1;
    }else if (strcasecmp(argStr,"F")==0 || strcasecmp(argStr,"N")==0 || strcasecmp(argStr,"")==0) {
      currentDevice->logEntry = 0;
    }else{
      warnAndClose("DeviceLogging", argStr, cfgFP);
      return -2;
    }
  }

  //If we hit the end of the file without finding enough devices, error out.
  if(i != currentModule->numDevices){
    printf("ERROR: there were not enough devices in the %s module\n",currentModule->name);
    printf("Aborting - fix the config file (%s) and try again\n",client.rcFile);
    if (cfgFP !=0) fclose(cfgFP);
    return -2;
  }

  (*moduleIndex)++;

  return 0;
}

// PRIMARY FUNCTION -----------------------------------------------

/*!
  \brief Load/Parse ISIS client's runtime configuration file.
  \param cfgfile Path/name of the client runtime configuration file
  \return 0 if success, <0 if failure.  All error message are printed 
  to the client's console.

  The precise actions of LoadConfig() are tailored to the client application.
*/
int loadConfig(char *cfgfile){
  char keyword[MAXCFGLINE]; // File is organized into KEYWORD VALUE pairs
  char argList[MAXCFGLINE]; // Generic argument list string
  char argStr[MAXCFGLINE];  // Generic argument token string
  char inStr[MAXCFGLINE];   // Generic input string

  FILE *cfgFP;              // Configuration file pointer
  int errValue;             // Used to convert strings to integers 

  int moduleIndex = 0;      // Used to count the number of device modules which have been added
  int maxDeviceOffset = 0;  // Used to find the maximum offset of any device

  //Loading sensible default config options.
  loadDefaultConfigs(cfgfile);

  // Now open the config file, if not, gripe and return -1.  Opening the
  // file here ensures that sensible defaults are set even if the config
  // file stuff was in error.
  if (!(cfgFP=fopen(cfgfile,"r"))) {
    printf("ERROR: Cannot open runtime configuration file %s\n",cfgfile);
    printf("       %s\n",strerror(errno));
    return -1;
  }

  //Dynamically allocating memory for the device modules.
  if(env.modules == NULL){
    env.numModules = numWagoModules(cfgFP);
    if(env.numModules == 0){
      printf("ERROR: No WAGO modules were found. Are you using an old configuration file?\n");
      printf("Aborting - fix the config file (%s) and try again\n",client.rcFile);
      return -2;
    }

    // NOTE: The C++ 'new' keyword is used instead of 'malloc()' here, because our struct 
    // contains C++ objects which need to be initialized.
    env.modules = new device_module_t[env.numModules];
  }

  //----------------------------------------------------------------

  // Config file parser loop. Read in each line of the config file and process it 
  while(fgets(inStr, MAXCFGLINE, cfgFP)) {

    // Skip comments (#) and blank lines
    if ((inStr[0]!='#') && (inStr[0]!='\n') && inStr[0]!='\0'){
      inStr[MAXCFGLINE] ='\0';

      sscanf(inStr,"%s %[^\n]",keyword,argList);

      // KEYWORD PAIRS ------------------------------

      // Mode: the application's operating mode.  2 options:
      //       STANDALONE: no ISIS server present
      //       ISISClient: we're an ISIS client
      if (strcasecmp(keyword,"MODE")==0) {
	      GetArg(inStr,2,argStr);
	  
        if (strcasecmp(argStr,"STANDALONE")==0){
	        client.useISIS = 0;
	      }else if (strcasecmp(argStr,"ISISCLIENT")==0){
	        client.useISIS = 1;
	      }else{
	        warnAndClose("Mode", argStr, cfgFP);
	        return -1;
	      }
      }

      // ID: IMPv2 node name of this client 
      else if (strcasecmp(keyword,"ID")==0) {
	      GetArg(inStr,2,argStr);
	      strcpy(client.ID,argStr);
      }

      // Port: network UDP socket port number of this client.  Host is
      //       assumed to be localhost (since it can't be anything else)
      else if (strcasecmp(keyword,"PORT")==0) {
	      GetArg(inStr, 2, argStr);
	      errValue = strToInt(argStr, &(client.Port));
        if(errValue){
          warnAndClose("Port", argStr, cfgFP);
          return -1;
        } 
      }

      // ISISID: IMPv2 Host name of the ISIS server.
      else if (strcasecmp(keyword,"ISISID")==0) {
	      GetArg(inStr, 2, argStr);
	      strcpy(client.isisID, argStr);
      }

      // ISISHost: Hostname of the machine running the ISIS server.
      //           May be a resolvable name or an IP address.
      else if (strcasecmp(keyword,"ISISHOST")==0) {
	      GetArg(inStr,2,argStr);
	      strcpy(client.isisHost,argStr);
      }
	
      // ISISPort: network UDP socket port number used by the ISIS server 
      //           running on ISISHost				  
      else if (strcasecmp(keyword,"ISISPORT")==0) {
	      GetArg(inStr, 2, argStr);
        errValue = strToInt(argStr, &(client.isisPort));
        if(errValue){
          warnAndClose("ISISPORT", argStr, cfgFP);
          return -1;
        } 
      }

      // InstID: Formal ID of the MODS we're monitoring (e.g., MODS1)
      else if (strcasecmp(keyword,"INSTID")==0) {
	      GetArg(inStr, 2, argStr);
	      strcpy(env.modsID,argStr);
      }

      // UseTTY: enable/disable the interactive command shell
      //    Usage: UseTTY [T|F]  --> recognizes aliases T=Y and F=N
      else if (strcasecmp(keyword,"USETTY")==0) {
	      GetArg(inStr,2,argStr);

	      if (strcasecmp(argStr,"T")==0 || strcasecmp(argStr,"Y")==0) {
	        useCLI = 1;
	      }else if (strcasecmp(argStr,"F")==0 || strcasecmp(argStr,"N")==0) {
	        useCLI = 0;
	      }else{
          warnAndClose("UseTTY", argStr, cfgFP);
          return -1;
        }
      }

      // Cadence: Instrument environmental sensor sampling cadence in seconds
      //    Nothing changes fast, so keep it slow
      else if (strcasecmp(keyword,"CADENCE")==0) {
	      GetArg(inStr,2,argStr);

        int dt;
        errValue = strToInt(argStr, &(dt));
        if(errValue || dt <= 0){
          warnAndClose("Cadence", argStr, cfgFP);
          return -1;
        }

	      env.cadence = (long) dt;
      }

      // HEB: IP address of the Head Electronic Box WAGO FieldBus controller
      else if (strcasecmp(keyword,"HEB")==0) {
	      GetArg(inStr,2,argStr);
	      strcpy(env.hebAddr,argStr);
      }

      // DataLog: Full path and rootname of the environmental data log
      //    The agent will append the UTC CCYYMMDD date tag and .log extension
      //    to create the full qualified name /path/rootname.CCYYMMDD.log
      else if (strcasecmp(keyword,"DATALOG")==0) {
	      GetArg(inStr,2,argStr);
	      strcpy(env.logRoot,argStr);
	      env.doLogging = 1;
      }

      // hdfLog: Path of the HDF data log
      else if (strcasecmp(keyword,"HDFLOG")==0) {
	      GetArg(inStr,2,argStr);
	      strcpy(env.hdfRoot,argStr);
      }

      // leapSecondsFile: Full path and name of the leap-seconds.list file.
      else if (strcasecmp(keyword,"LEAPSECONDSFILE")==0) {
	      GetArg(inStr,2,argStr);
	      strcpy(env.leapSecondsFile,argStr);
      }


      // KEYWORD FLAGS ------------------------------

      // NoLog: Explicitly disable ascii data logging
      else if (strcasecmp(keyword,"NOLOG")==0) {
	      env.doLogging = 0;
      }
      
      // HDF5 Output: Explicitly disable hdf5 data logging
      else if (strcasecmp(keyword,"NOHDF5")==0) {
	      env.useHdf5 = 0;
      }

      // Verbose: Enable verbose output mode (e.g., for debugging)
      else if (strcasecmp(keyword,"VERBOSE")==0) {
	      client.isVerbose = 1;
      }

      // Debug: Enable runtime debugging out (superverbose mode)
      else if (strcasecmp(keyword,"DEBUG")==0) {
	      client.Debug = 1;
      }


      // KEYWORD MODULE -------------------------------

      // WAGO: The start of a WAGO device module. A set of devices should be listed on the following lines.
      else if (strcasecmp(keyword,"WAGO")==0){
        int result = parseWagoModule(cfgFP, inStr, argStr, &moduleIndex, &maxDeviceOffset);
        if(result != 0) return result;
      }


      // KEYWORD JUNK ------------------------------

      // We found a device outside of a module. That is a problem.
      else if (strcasecmp(keyword,"DEVICE")==0) {
	      printf("ERROR: A device was found outside of a module. You might have too many devices.\n");
        printf("Aborting - fix the config file (%s) and try again\n",client.rcFile);
        return -2;
      }
      
      // Gripe if junk is in the config file
      else {
        printf("Ignoring unrecognized config file entry - %s", inStr);
      }
    }

    memset(inStr,0,sizeof(inStr)); 
  }

  //----------------------------------------------------------------

  // Now that we know the maximum device offset, dynamically allocate memory for data collection.
  if(env.rawWagoData == NULL){
    env.rawWagoData = (uint16_t*) malloc((maxDeviceOffset+1)*sizeof(uint16_t));
    memset(env.rawWagoData, 0, (maxDeviceOffset+1)*sizeof(uint16_t));
  }
  
  // all done, close the config file and return
  if (cfgFP!=0) fclose(cfgFP);
  return 0;
}
