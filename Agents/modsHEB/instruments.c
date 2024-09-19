/*!
  \file instruments.c
  \brief All of the functions related to collecting enviornmental sensing data.

  \author X. Carroll, OSU.
  \date 2024 August 29
*/

#include "client.h" // custom client application header 

// CONSTANTS -------------------------------------------------------

#define QUADCELL_PROCESS 0
#define RTD_PROCESS 1

// GLOBAL TABLE WITH INSTRUMENT DATA -------------------------------

instrument_t instrumentTable[] = {
  {"QuadCell0", "Qcl0", "Reading from the quadcell sensor.",   lbto::tel::unit::volt(),    0, QUADCELL_PROCESS },
  {"QuadCell1", "Qcl1", "Reading from the quadcell sensor.",   lbto::tel::unit::volt(),    1, QUADCELL_PROCESS },
  {"QuadCell2", "Qcl2", "Reading from the quadcell sensor.",   lbto::tel::unit::volt(),    2, QUADCELL_PROCESS },
  {"QuadCell3", "Qcl3", "Reading from the quadcell sensor.",   lbto::tel::unit::volt(),    3, QUADCELL_PROCESS },
  {"Rtd",       "AmbT", "Temperature from the in-box sensor.", lbto::tel::unit::celsius(), 4, RTD_PROCESS      }
};

const int NUM_INSTRUMENTS = sizeof(instrumentTable)/sizeof(instrument_t); //The number of entries in the table above.


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
  \brief Initialize the envdata struct variables that depend on the connected instruments.

  \param envi pointer to an #envdata_t data structure

  Initializes the enviromental sensor data structure variables that depend on connected instruments.
  Called from initEnvData.

  Variables are freed using freeEnvData function.
*/
void initInstrumentData(envdata_t *envi){
  //Dynamically creating and clearing the instrument data array.
  envi->instrumentData = (float*) calloc(NUM_INSTRUMENTS, sizeof(float));
  memset(envi->instrumentData, 0, NUM_INSTRUMENTS*sizeof(float));

  //Dynamically creating the HDF telemetry array.
  envi->floatMeasures = new lbto::tel::float_measure::buf_proxy[NUM_INSTRUMENTS];
}

/*!
  \brief Get enviromental sensor data

  \param envi pointer to an #envdata_t struct

  \return 0 on success, -1 on errors.  

  Gets data from the instrument enviromental sensors and loads the results into the enviromental data 
  structure.
*/
int getInstrumentData(envdata_t *envi) {
  //Query WAGOs and collect data here.
  uint16_t rawHebData[5];
  wagoSetGet(0, envi->hebAddr, 0, 5, rawHebData);

  //Set the ENV data.

  //Querying every instrument in the table.
  for(int i=0; i<NUM_INSTRUMENTS; i++){
    instrument_t* inst = instrumentTable+i;

    switch(inst->type){
      case QUADCELL_PROCESS:
        qc2vdc(rawHebData+inst->wagoAddress, envi->instrumentData+i);
        break;
      case RTD_PROCESS:
        ptRTD2C(rawHebData+inst->wagoAddress, envi->instrumentData+i);
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