#include "instruments.h"

// GLOBAL TABLE WITH INSTRUMENT DATA -------------------------------

struct InstrumentProfile instrumentTable[] = {
    {"QuadCell0", "Reading from the quadcell sensor.", lbto::tel::unit::volt()},
    {"QuadCell1", "Reading from the quadcell sensor.", lbto::tel::unit::volt()},
    {"QuadCell2", "Reading from the quadcell sensor.", lbto::tel::unit::volt()},
    {"QuadCell3", "Reading from the quadcell sensor.", lbto::tel::unit::volt()},
    {"Rtd", "Temperature from the in-box sensor.", lbto::tel::unit::celsius()}
};


// UTILITY FUNCTIONS -----------------------------------------------

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