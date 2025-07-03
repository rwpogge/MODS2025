#ifndef __modsIIFGlobals_h__
#define __modsIIFGlobals_h__

// Global Variables used by the MODS ICE/IIF implementation

// Communicator and pointer to ZeroICE and IIF functions

Ice::CommunicatorPtr communicator; // Ice communicator
lbto::iifres res; // IIF result object
lbto::IIFServerPrx iif;
lbto::FactoryPrx factory;

int side;

char clientProxy_Name[80];
char instrument_Name[80];
char focalStation_Name[80];

char iifmsgs[200][100];
int iifcount;

#endif  // modsIIFGlobals_h
