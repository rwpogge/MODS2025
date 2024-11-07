/*!
  \file devices.c
  \brief All of the functions related to collecting enviornmental sensing data.

  \author X. Carroll, OSU.
  \date 2024 August 29
*/

#include "client.h" // custom client application header 

// CONSTANTS -------------------------------------------------------

#define PROCESS_AS_DO 0
#define PROCESS_AS_RTD 1
#define PROCESS_AS_QUADCELL 2

// UTILITY FUNCTIONS -----------------------------------------------

/*!
  \brief Converts a raw RTD reading into its equivalent temperature.

  \param rawData pointer to an RTD float as collected from the WAGO module.
  \param outputData pointer to the float variable where the converted data will be stored.
*/
void ptRTD2C(uint16_t* rawData, float* outputData){
  float tempRes = 0.1;
  float tempMax = 850.0;
  float wrapT = tempRes*(pow(2.0, 16)-1);
    
  float temp = tempRes*(*rawData);
  if (temp > tempMax)
    temp -= wrapT;
  
  *outputData = temp;
}

/*!
  \brief Converts a raw quadcell reading into its equivalent DC voltage.

  \param rawData pointer to the quadcell varaible as collected from the WAGO module.
  \param outputData pointer to the float varaible where the converted data will be stored.
*/
void qc2vdc(uint16_t* rawData, float* outputData){
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

  Frees dynamically allocated memory needed for device data.
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
}

/*!
  \brief Get enviromental sensor data

  \param envi pointer to an #envdata_t struct

  \return 0 on success, -1 on errors.  

  Gets device data (enviromental sensor data) and loads the results into the enviromental data structure.
*/
int getDeviceData(envdata_t *envi) {
  // Get the UTC date/time of the query (ISIS client utility routine)
  strcpy(envi->utcDate,ISODate());

  // All done (logging is done by the calling program)
  return 0;
}