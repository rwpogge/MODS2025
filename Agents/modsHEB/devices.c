/*!
  \file devices.c
  \brief All of the functions related to collecting enviornmental sensing data.

  \author X. Carroll, OSU.
  \date 2024 August 29
*/

#include "client.h" // custom client application header 

// CONSTANTS -------------------------------------------------------

//An enum to make the integer values of process types more readable.
typedef enum {DO, DO_NC, RTD, AI} PROCESS;
 
//This maps the string values of the enum above to their numerical values.
const static struct{
    PROCESS value;
    const char* string;
}PROCESS_MAP[] = {
    {DO, "DO"},
    {RTD, "RTD"},
    {AI, "AI"}
};
const int PROCESS_MAP_LEN = sizeof(PROCESS_MAP)/sizeof(PROCESS_MAP[0]);


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

  \param rawData pointer to the quadcell varaible as collected from the WAGO module.
  \param outputData pointer to the float varaible where the converted data will be stored.
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
  \brief Correctly releases memory allocated for device data in an #envdata_t structure.

  \param envi pointer to an #envdata_t data structure
*/
void freeDeviceData(envdata_t *envi){
  //If the modules array has not already been freed, do so now.
  if(envi->modules != NULL){
    for(int i=0; i<envi->numModules; i++){
      //Free every device connected to the module
      if(envi->modules[i].devices != NULL) free(envi->modules[i].devices);
      envi->modules[i].devices = NULL;
    }

    //Free every module
    free(envi->modules);
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
  \brief Get enviromental sensor data

  \param envi pointer to an #envdata_t struct

  \return 0 on success, -1 on errors.  

  Gets device data (enviromental sensor data) and loads the results into the enviromental data structure.
*/
int getDeviceData(envdata_t *envi) {
  // For every connected module.
  for(int i=0; i<envi->numModules; i++){
    // Query the WAGO based on the processType.
    switch(envi->modules[i].processingType){
      //The register types.
      case AI:
      case RTD:
        wagoSetGet(0, envi->hebAddr, envi->modules[i].baseAddress, envi->modules[i].numDevices, envi->rawWagoData);
        break;
      //The coil types.
      case DO:
        wagoSetGetCoils(0, envi->hebAddr, envi->modules[i].baseAddress, envi->modules[i].numDevices, envi->rawWagoData);
        break;
      //Default case.
      default:
        break;
    }

    // For every connected device.
    for(int j=0; j<envi->modules[i].numDevices; j++){
      device_t* device = envi->modules[i].devices+j;

      // Process the WAGO data based on the processType.
      switch(envi->modules[i].processingType){
        case DO:  device->data = envi->rawWagoData[device->address];        break;
        case RTD: rtd2c(envi->rawWagoData+device->address, &device->data);  break;
        case AI:  ai2vdc(envi->rawWagoData+device->address, &device->data); break;
        default: break;
      }
    }
  }

  // Get the UTC date/time of the query (ISIS client utility routine)
  strcpy(envi->utcDate,ISODate());

  // All done (logging is done by the calling program)
  return 0;
}