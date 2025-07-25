/*!
  \file devices.c
  \brief All of the functions related to collecting enviornmental sensing data.

  \author X. Carroll, OSU.
  \date 2024 August 29
*/

#include "client.h" // custom client application header 

// UTILITY FUNCTIONS -----------------------------------------------

/*!
  \brief Finds the integer value associated with the given process type.

  \param process the string value of the process type.

  \return the integer value associated with the process type. -1 if the string isn't a process type.
*/
int strToProcessType(char* process){
  int i;
  for (i=0; i<PROCESS_MAP_LEN; i++)
    if (!strcasecmp(process, PROCESS_MAP[i].string))
      return PROCESS_MAP[i].value;

  return -1;
}

/*!
  \brief Converts a raw RTD reading into its equivalent temperature.

  \param rawData pointer to an RTD float as collected from the WAGO module.
  \param outputData pointer to the float variable where the converted data will be stored.
*/
void rtd2c(uint16_t* rawData, float* outputData){
  float tempRes = 0.1;
  float tempMax = 850.0;
  float wrapT = tempRes*(pow(2.0, 16)-1);
    
  float temp = tempRes*(*rawData);
  if (temp > tempMax)
    temp -= wrapT;
  
  *outputData = temp;
}

/*!
  \brief Converts a raw analog input reading into its equivalent DC voltage.

  \param rawData pointer to the quadcell variable as collected from the WAGO module.
  \param outputData pointer to the float variable where the converted data will be stored.
*/
void ai2vdc(uint16_t* rawData, float* outputData){
  int posMax = pow(2, 15) - 1;
  int negMin = pow(2, 16) - 2;
  
  if(*rawData > posMax)
      *outputData = 10.0*(((*rawData)-negMin)/posMax);
  else
      *outputData = 10.0*((float)(*rawData)/posMax);
  
}


// INTERACT WITH ENV DATA -----------------------------------------------

/*!
  \brief Turn device data from envdata into a status string.

  \param envi pointer to an #envdata_t struct
  \param moduleIndex the index of the module to be processed.
  \param deviceIndex the index of the device to be processed, or <0 if the whole module should be processed.
  \param buffer a char array to fill with the result string.
  \param bufferSize the size of the supplied buffer in bytes.

  \return 0 on success, -1 on errors.  

  Processes device data in envdata into a status string. i.e. "arcon=on ion=on".
*/
int getDeviceModuleStatus(envdata_t *envi, int moduleIndex, int deviceIndex, char* buffer, int bufferSize){
  device_module_t* module;  // The targeted module.
  device_t* device;         // The targeted device.

  // Get the module while checking for errors.
  if(moduleIndex >= envi->numModules || moduleIndex < 0){
    printf("module index given to getDeviceModuleStatus() is out of bounds.");
    return -1;
  }
  module = env.modules + moduleIndex;

  // Verify that the device is in range.
  if(deviceIndex >= module->numDevices){
    printf("device index given to getDeviceModuleStatus() is out of bounds.");
    return -1;
  }

  // If deviceIndex is positive, we should only print the status of a single device 
  if(deviceIndex >= 0){
    device = module->devices + deviceIndex;
    
    switch(module->processingType){
      case DO: snprintf(buffer, bufferSize, "%s=%s", device->name, device->data ? "ON" : "OFF"); break;
      case RTD: snprintf(buffer, bufferSize, "%s=%.1f", device->name, device->data); break;
      case AI: snprintf(buffer, bufferSize, "%s=%.4f", device->name, device->data); break;
      default: break;
    }

    return 0;
  }

  // If the deviceIndex is negative, we should print the status of every device.
  int stringLength = 0;
  for(int i=0; i< module->numDevices; i++){
    // Get the device at index i.
    device = module->devices + i;

    // Add the device data to the response buffer.
    switch(module->processingType){
      case DO:
        stringLength += snprintf(buffer+stringLength, bufferSize-stringLength, 
          "%s=%s ", device->name, device->data ? "ON" : "OFF"
        );
        break;
      case RTD: 
        stringLength += snprintf(buffer+stringLength, bufferSize-stringLength, 
          "%s=%.1f ", device->name, device->data
        );
        break;
      case AI: 
        stringLength += snprintf(buffer+stringLength, bufferSize-stringLength, 
          "%s=%.4f ", device->name, device->data
        );
        break;
      default:
        break;
    }
  }

  return 0;
}

/*!
  \brief Correctly releases memory allocated for device data in an #envdata_t structure.

  \param envi pointer to an #envdata_t data structure
*/
void freeAllDeviceData(envdata_t *envi){
  // Note: The modules and devices were allocated using the 'new' keyword, not 'malloc()'
  // This is because the structs contain C++ objects which need initalized.

  //If the modules array has not already been freed, do so now.
  if(envi->modules != NULL){
    for(int i=0; i<envi->numModules; i++){
      //Free every device connected to the module
      if(envi->modules[i].devices != NULL){
        delete[] envi->modules[i].devices;
        envi->modules[i].devices = NULL;
      } 
    }

    //Free every module
    delete[] envi->modules;
    envi->modules = NULL;
  }

  //If the WAGO memory has not already been freed, do so now.
  if(envi->rawWagoData != NULL){
    free(envi->rawWagoData);
    envi->rawWagoData = NULL;
  }
}

// INTERACT WITH DEVICES -----------------------------------------------

/*!
  \brief Get enviromental sensor data from every module

  \param envi pointer to an #envdata_t struct
  \param moduleIndex the index of the module to be querried.

  \return 0 on success, -1 on errors.  

  Gets device data (enviromental sensor data) for every module and loads the results into the enviromental data structure.
*/
int getAllDeviceData(envdata_t *envi){
  for(int i=0; i<envi->numModules; i++){
    getDeviceModuleData(envi, i);
  }

  return 0;
}

/*!
  \brief Get enviromental sensor data from a single module

  \param envi pointer to an #envdata_t struct
  \param moduleIndex the index of the module to be querried.

  \return 0 on success, -1 on errors.  

  Gets device data (enviromental sensor data) from one module and loads the results into the enviromental data structure.
*/
int getDeviceModuleData(envdata_t *envi, int moduleIndex) {
  // Query the WAGO based on the processType.
  switch(envi->modules[moduleIndex].processingType){
    //The register types.
    case AI:
    case RTD:
      wagoSetGetRegisters(0, envi->hebAddr, envi->modules[moduleIndex].baseAddress, envi->modules[moduleIndex].maxOffset+1, envi->rawWagoData);
      break;
    //The coil types.
    case DO:
      wagoSetGetCoils(0, envi->hebAddr, envi->modules[moduleIndex].baseAddress, envi->modules[moduleIndex].maxOffset+1, (uint8_t*)envi->rawWagoData);
      break;
    //Default case.
    default:
      break;
  }

  // For every connected device.
  for(int j=0; j<envi->modules[moduleIndex].numDevices; j++){
    device_t* device = envi->modules[moduleIndex].devices+j;

    // Process the WAGO data based on the processType.
    switch(envi->modules[moduleIndex].processingType){
      case DO:
        device->data = ((uint8_t*) envi->rawWagoData)[device->address] ^ device->nc;
        break;
      case RTD: 
        rtd2c(envi->rawWagoData+device->address, &device->data);
        break;
      case AI: 
        ai2vdc(envi->rawWagoData+device->address, &device->data);
        break;
      default:
        break;
    }
  }

  // Get the UTC date/time of the query (ISIS client utility routine)
  strcpy(envi->utcDate,ISODate());

  // All done (logging is done by the calling program)
  return 0;
}
