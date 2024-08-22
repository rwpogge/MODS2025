#ifndef INSTRUMENTS_H
#define INSTRUMENTS_H

#include <telcollection.hxx>  // LBTO telemetry library

// How many instruments to store.
#define NUM_INSTRUMENTS 5

// The data structures that will hold the instrument data.
struct InstrumentProfile{
    char* name; 
    char* description;
    lbto::tel::unit units;
    int type;
};

extern struct InstrumentProfile instrumentTable[];


// UTILITY FUNCTIONS -----------------------------------------------

//Converts an array of raw RTD data into its equivalent Temperature.
void ptRTD2C(uint16_t* rawData, float* outputData, int numRtds);

//Converts an array of raw quadcell data into its equivalent DC voltage.
void qc2vdc(uint16_t* rawData, float* outputData);

#endif