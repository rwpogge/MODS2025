/*!
  \file instruments.c
  \brief All of the functions related to collecting enviornmental sensing data.

  \author X. Carroll, OSU.
  \date 2024 August 29
*/

#include "client.h" // custom client application header 

// CONSTANT ADDRESSES -------------------------------

#define HEB_ADDRESS "192.168.139.135"


// GLOBAL TABLE WITH INSTRUMENT DATA -------------------------------

instrument_t instrumentTable[] = {
  {"QuadCell0", "Qcl0", "Reading from the quadcell sensor.",   lbto::tel::unit::volt()    },
  {"QuadCell1", "Qcl1", "Reading from the quadcell sensor.",   lbto::tel::unit::volt()    },
  {"QuadCell2", "Qcl2", "Reading from the quadcell sensor.",   lbto::tel::unit::volt()    },
  {"QuadCell3", "Qcl3", "Reading from the quadcell sensor.",   lbto::tel::unit::volt()    },
  {"Rtd",       "AmbT", "Temperature from the in-box sensor.", lbto::tel::unit::celsius() }
};

const int NUM_INSTRUMENTS = sizeof(instrumentTable)/sizeof(instrument_t); //The number of entries in the table above.


// UTILITY FUNCTIONS -----------------------------------------------

/*!
  \brief Converts an array of raw RTD data into its equivalent Temperature.

  \param rawData pointer to the RTD data as collected from the WAGO module.
  \param outputData pointer to a float array where the converted data will be stored.
  \param numRtds the size of the rawData array (the number of RTDs data was collected for).
*/
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

/*!
  \brief Converts an array of raw quadcell data into its equivalent DC voltage.

  \param rawData pointer to the quadcell data as collected from the WAGO module.
  \param outputData pointer to a float array where the converted data will be stored.
*/
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


// INTERACT WITH ENV DATA -----------------------------------------------

/*!
  \brief Initialize the envdata struct variables that depend on the connected instruments.

  \param envi pointer to an #envdata_t data structure

  Initializes the enviromental sensor data structure variables that depend on connected instruments.
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
  wagoSetGet(0, HEB_ADDRESS, 0, 5, rawHebData);

  //Set the ENV data.
  qc2vdc(rawHebData, envi->instrumentData+0);
  ptRTD2C(rawHebData+4, envi->instrumentData+4, 1);

  // Get the UTC date/time of the query (ISIS client utility routine)
  strcpy(envi->utcDate,ISODate());

  // All done (logging is done by the calling program)
  return 0;
}