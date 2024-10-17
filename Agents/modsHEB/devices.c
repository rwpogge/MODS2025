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

// GLOBAL TABLE WITH DEVICE DATA -------------------------------

device_t deviceTable[] = {
  {"QuadCell0", "Qcl0", "Reading from the quadcell sensor.",   lbto::tel::unit::volt(),    0,   PROCESS_AS_QUADCELL },
  {"QuadCell1", "Qcl1", "Reading from the quadcell sensor.",   lbto::tel::unit::volt(),    1,   PROCESS_AS_QUADCELL },
  {"QuadCell2", "Qcl2", "Reading from the quadcell sensor.",   lbto::tel::unit::volt(),    2,   PROCESS_AS_QUADCELL },
  {"QuadCell3", "Qcl3", "Reading from the quadcell sensor.",   lbto::tel::unit::volt(),    3,   PROCESS_AS_QUADCELL },
  {"Rtd",       "AmbT", "Temperature from the in-box sensor.", lbto::tel::unit::celsius(), 4,   PROCESS_AS_RTD      },
  {"ArchonS",   "ArcS", "The power status of the Archon.",     lbto::tel::unit::none(),    0,   PROCESS_AS_DO       },
  {"BogS",      "BogS", "The power status of the BOG heater.", lbto::tel::unit::none(),    1,   PROCESS_AS_DO       },
  {"IonS",      "IonS", "The power status of the ion-gauge.",  lbto::tel::unit::none(),    2,   PROCESS_AS_DO       }
};

const int NUM_DEVICES = sizeof(deviceTable)/sizeof(device_t); //The number of entries in the table above.


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
  \brief Initialize the envdata struct variables that depend on the connected devices.

  \param envi pointer to an #envdata_t data structure

  Initializes the enviromental sensor data structure variables that depend on connected devices.
  Called from initEnvData.

  Variables are freed using the freeDeviceData function.
*/
void initDeviceData(envdata_t *envi){
  //Dynamically creating the device data array.
  //This code is primarily C code, so malloc and calloc are prefered for memory allocation.
  envi->deviceData = (float*) calloc(NUM_DEVICES, sizeof(float));
  memset(envi->deviceData, 0, NUM_DEVICES*sizeof(float));

  //Dynamically creating the HDF telemetry array.
  //The objected oriented nature of the telemetry library makes the C++ new/delete keywords preferable here.
  envi->floatMeasures = new lbto::tel::float_measure::buf_proxy[NUM_DEVICES];
}

/*!
  \brief Correctly releases memory allocated for device data in an #envdata_t structure.

  \param envi pointer to an #envdata_t data structure

  Frees dynamically allocated memory needed for device data.
*/
void freeDeviceData(envdata_t *envi){
  //If the deviceData array has not already been freed, do so now.
  if(envi->deviceData != NULL){
    free(envi->deviceData);
    envi->deviceData = NULL;
  } 
  
  //If the floatMeasures array has not already been freed, do so now.
  if(envi->floatMeasures != NULL){
    delete[] envi->floatMeasures;
    envi->floatMeasures = NULL;
  } 
}

/*!
  \brief Get enviromental sensor data

  \param envi pointer to an #envdata_t struct

  \return 0 on success, -1 on errors.  

  Gets device data (enviromental sensor data) and loads the results into the enviromental data structure.
*/
int getDeviceData(envdata_t *envi) {
  //Query WAGOs and collect data here.
  uint16_t rawHebData[5];
  wagoSetGet(0, envi->hebAddr, 0, 5, rawHebData);

  uint16_t rawHebDoData;
  wagoSetGet(0, envi->hebAddr, 512, 1, &rawHebDoData);

  //Set the ENV data.

  //Querying every device in the table.
  for(int i=0; i<NUM_DEVICES; i++){
    device_t* device = deviceTable+i;

    switch(device->processingType){
      case PROCESS_AS_DO:
        *(envi->deviceData+i) = ((rawHebDoData & (1 << device->wagoAddress)) == (1 << device->wagoAddress));
        break;
      case PROCESS_AS_RTD:
        ptRTD2C(rawHebData+device->wagoAddress, envi->deviceData+i);
        break;
      case PROCESS_AS_QUADCELL:
        qc2vdc(rawHebData+device->wagoAddress, envi->deviceData+i);
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

int setDigitalOutputs(envdata_t *envi, int num, int baseAddress, int* states){
  uint16_t state = 0;

  for(int i=num-1; i>=0; i--){
    state = state << 1;
    if(states[i] == 1) state += 1;
  }

  return wagoSetGet(1, envi->hebAddr, baseAddress, 1, &state);
}