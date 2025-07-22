//
// clientutils - client app utilities go here
//

/*!
  \file clientutils.c
  \brief Client application utility functions

  Because they have to go somewhere...

  \author R. Pogge, OSU Astronomy Dept. (pogge@astronomy.ohio-state.edu)
  \date 2010 June 21
*/

#include "client.h" // custom client application header 

//---------------------------------------------------------------------------

/*!
  \brief Initialize a MODS instrument environmental sensor data structure

  \param envi pointer to an #envdata_t data structure

  Initializes the enviromental sensor data structure with sensible
  defaults.  This ensures that we have some way to avoid data
  structures with delinquent information.

*/
void initEnvData(envdata_t *envi){
  strcpy(envi->modsID,"None");
  envi->cadence = DEFAULT_CADENCE;  // default monitoring cadence (see client.h)
  envi->pause = 0;                  // start running (no pause)
  strcpy(envi->iub_Addr,"");
  envi->ambientTemp = 0.0;
  envi->glycolSupplyPres = 0.0;
  envi->glycolReturnPres = 0.0;
  envi->glycolSupplyTemp = 0.0;
  envi->glycolReturnTemp = 0.0;
  envi->utilBoxTemp = 0.0;     
  envi->agwHSTemp = 0.0;       

  envi->iebB_Switch  = 0;  
  envi->iebB_Breaker = 0;
  envi->iebR_Switch  = 0;  
  envi->iebR_Breaker = 0;
  envi->hebB_Switch  = 0;  
  envi->hebB_Breaker = 0;
  envi->hebR_Switch  = 0;  
  envi->hebR_Breaker = 0;
  envi->llb_Switch   = 0;   
  envi->llb_Breaker  = 0; 
  envi->gcam_Switch  = 0;  
  envi->gcam_Breaker = 0;
  envi->wfs_Switch   = 0;   
  envi->wfs_Breaker  = 0; 

  strcpy(envi->iebR_Addr,"");
  envi->iebR_AirTemp = 0.0;
  envi->iebR_ReturnTemp = 0.0;
  envi->airTopTemp = 0.0; 
  envi->airBotTemp = 0.0;

  strcpy(envi->iebB_Addr,"");
  envi->iebB_AirTemp = 0.0;
  envi->iebB_ReturnTemp = 0.0;
  envi->trussTopTemp = 0.0; 
  envi->trussBotTemp = 0.0;

  strcpy(envi->llb_Addr,"");
  envi->irlaserState = 0;   
  envi->irlaserPowerSet = 0.0;
  envi->irlaserPowerOut = 0.0;
  envi->irlaserBeam = 0;    
  envi->irlaserTemp = 0.0;    
  envi->irlaserTempSet = 0.0; 

  envi->doLogging = 1;                      // enable enviromental data logging by default
  envi->useHdf5 = 1;                        // default: output enviornmental data to Hdf5
  envi->hdfInitalized = 0;
  strcpy(envi->logRoot,ENV_LOGS);
  strcpy(envi->hdfRoot,HDF_LOGS);
  strcpy(envi->leapSecondsFile,LEAP_SECONDS_FILE);
  strcpy(envi->logFile,"");
  strcpy(envi->lastDate,"");
  strcpy(envi->utcDate,"");
  if (envi->logFD > 0) close(envi->logFD);  // just in case...
  envi->logFD = 0;
}

/*!
  \brief Print the contents of the envData struct

  \param envi pointer to an #envdata_t data structure

  Prints the contents of the #envdata_t data structure describing the
  raw contents of the last TCS query.  The Date/Time give when
  the query occurred.  

*/
void printEnvData(envdata_t *envi){
  if (!useCLI) return;

  printf("Enviromental Monitor Agent Info:\n");
  printf("  Instrument: %s\n",envi->modsID);
  printf("  Blue IEB WAGO IP Address: %s\n",envi->iebB_Addr);
  printf("   Red IEB WAGO IP Address: %s\n",envi->iebR_Addr);
  printf("       IUB WAGO IP Address: %s\n",envi->iub_Addr);
  printf("  Monitor Status: %s\n",(envi->pause) ? "PAUSED" : "Active");
  printf("  Sampling Cadence: %d seconds\n",envi->cadence);
  printf("  Data Logging: %s\n",(envi->doLogging) ? "Enabled" : "Disabled");
  envi->logFD == 0 ? printf("  Data Log File: CLOSED\n") : printf("  Data Log File: %s\n",envi->logFile);
  printf("  HDF Logging: %s\n",(envi->useHdf5) ? "Enabled" : "Disabled");
  envi->useHdf5 == 0 ? printf("  Hdf Log Directory: CLOSED\n") : printf("  Hdf Log Directory: %s\n",envi->hdfRoot);
  printf("Most Recent Sensor Data:\n");
  printf("  Date/Time: UTC %s\n",envi->utcDate);
  printf("  Ambient Temp: %.1f C\n",envi->ambientTemp);
  printf("  Glycol Supply: P=%.1f psi-g  T=%.1f C\n",
	 envi->glycolSupplyPres,envi->glycolSupplyTemp);
  printf("         Return: P=%.1f psi-g  T=%.1f C\n",
	 envi->glycolReturnPres,envi->glycolReturnTemp);
  printf("  IEB Air: Red=%.1f Blue=%.1f C\n",
	 envi->iebR_AirTemp, envi->iebB_AirTemp);
  printf("   Glycol: Red=%.1f Blue_B=%.1f C\n",
	 envi->iebR_ReturnTemp, envi->iebB_ReturnTemp);
  printf("  IUB Air: %.1f C   HeatSink: %.1f C\n",
	 envi->utilBoxTemp, envi->agwHSTemp);
  printf("  Instrument Air Temp: Top=%.1f Bot=%.1f C\n",
	 envi->airTopTemp, envi->airBotTemp);
  printf("  Instrument Truss Temp: Top=%.1f Bot=%.1f C\n",
	 envi->trussTopTemp, envi->trussBotTemp);
  printf("Client Info:\n");
  printf("  Mode: %s\n",(client.useISIS) ? "ISIS client" : "Standalone");
  printf("  ISIS Host: %s (%s:%d)\n",client.ID,client.Host,client.Port);
  printf("  Config File: %s\n",client.rcFile);
  printf("  %s\n",(client.isVerbose) ? "Verbose" : "Concise");

  printf("\n NOTE: Data above could be stale, note the date/time above\n");
  printf("\n");
}

/*!
  \brief Get enviromental sensor data

  \param envi pointer to an #envdata_t struct

  \return 0 on success, -1 on errors.  

  Gets data from the instrument enviromental sensors and loads
  the results into the enviromental data structure

*/
int getEnvData(envdata_t *envi) {
  int ierr;
  uint16_t iubData[10];   // raw IUB WAGO data array
  uint16_t iebRData[10];  // raw Red IEB WAGO data array
  uint16_t iebBData[10];  // raw Blue IEB WAGO data array
  uint16_t llbData[10];   // raw Blue IEB WAGO data array
  int iubPower   = 0;     // IUB AC power request status word
  int iubBreaker = 0;     // IUB AC power breaker status word

  // Get data from the IUB environmental sensors
  ierr = wagoSetGet(0,envi->iub_Addr,1,10,iubData);
  if (ierr != 0) {
    if (useCLI) printf("WARNING: %s IUB WAGO read error",envi->modsID);
    return ierr;
  }

  envi->glycolSupplyPres = (float)iubData[0]/327.64;
  envi->glycolReturnPres = (float)iubData[1]/327.64;
  envi->glycolSupplyTemp = (float)iubData[4]/10.0;
  envi->glycolReturnTemp = (float)iubData[5]/10.0;
  envi->agwHSTemp   = (float)iubData[6]/10.0;
  envi->utilBoxTemp = (float)iubData[7]/10.0;
  envi->ambientTemp = (float)iubData[8]/10.0;

  // Get the IUB AC power control status data
  ierr = wagoSetGet(0,envi->iub_Addr,513,1,iubData);
  if (ierr < 0) {
    if (useCLI) printf("WARNING: %s IUB WAGO error reading power status word",envi->modsID);
  }
  else {
    iubPower = iubData[0];
  
    // On/Off state depends on the wiring of the AC power switching
    // relay: normally-open (OFF on mains power up) or normally-closed
    // (ON on mains power up)
    envi->iebR_Switch = ((iubPower & IEB_R_POWER) != IEB_R_POWER); // normally closed
    envi->iebB_Switch = ((iubPower & IEB_B_POWER) != IEB_B_POWER); // normally closed
    envi->hebR_Switch = ((iubPower & HEB_R_POWER) == HEB_R_POWER); // normally OPEN
    envi->hebB_Switch = ((iubPower & HEB_B_POWER) == HEB_B_POWER); // normally OPEN
    envi->gcam_Switch = ((iubPower & GCAM_POWER)  == GCAM_POWER);  // normally OPEN
    envi->wfs_Switch  = ((iubPower & WFS_POWER)   == WFS_POWER);   // normally OPEN
    envi->llb_Switch  = ((iubPower & LLB_POWER)   != LLB_POWER);   // normally closed
  }
  
  // Get the IUB AC power breaker output side current sensor data
  ierr = wagoSetGet(0,envi->iub_Addr,11,1,iubData);
  if (ierr < 0) {
    if (useCLI) printf("WARNING: %s cannot read IUB breaker output status",envi->modsID);
  }
  else {
    iubBreaker = iubData[0];
    envi->iebR_Breaker = ((iubBreaker & IEB_R_BREAKER) == IEB_R_BREAKER);
    envi->iebB_Breaker = ((iubBreaker & IEB_B_BREAKER) == IEB_B_BREAKER);
    envi->hebR_Breaker = ((iubBreaker & HEB_R_BREAKER) == HEB_R_BREAKER);
    envi->hebB_Breaker = ((iubBreaker & HEB_B_BREAKER) == HEB_B_BREAKER);
    envi->gcam_Breaker = ((iubBreaker & GCAM_BREAKER)  == GCAM_BREAKER);
    envi->wfs_Breaker  = ((iubBreaker & WFS_BREAKER)   == WFS_BREAKER);
    envi->llb_Breaker  = ((iubBreaker & LLB_BREAKER)   == LLB_BREAKER);
  }
  
  // Get data from the Red IEB environmental sensors
  ierr = wagoSetGet(0,envi->iebR_Addr,1,10,iebRData);
  if (ierr < 0) {
    if (useCLI) printf("WARNING: %s Red IEB WAGO read error",envi->modsID);
  }
  else {
    envi->iebR_AirTemp = (float)iebRData[4]/10.0;
    envi->iebR_ReturnTemp = (float)iebRData[5]/10.0;
    envi->airTopTemp = (float)iebRData[6]/10.0;
    envi->airBotTemp = (float)iebRData[7]/10.0;
  }
  
  // Get data from the Blue IEB environmental sensors
  ierr = wagoSetGet(0,envi->iebB_Addr,1,10,iebBData);
  if (ierr < 0) {
    if (useCLI) printf("WARNING: %s Blue WAGO IEB read error",envi->modsID);
  }
  else {
    envi->iebB_AirTemp = (float)iebBData[4]/10.0;
    envi->iebB_ReturnTemp = (float)iebBData[5]/10.0;
    envi->trussTopTemp = (float)iebBData[6]/10.0;
    envi->trussBotTemp = (float)iebBData[7]/10.0;
  }
  
  // In the future, might put IR laser state info here, but laser
  // control logic is complicated...
  //
  // Addresses we need:
  //  IR Laser Power Out: 2
  //  IR Laser Temp Set Point: 3
  //  IR Laser Temp: 4
  //  IR Laser Power Set Point: 514
  //  LLBONOFF: 517
  //

  // Get the UTC date/time of the query (ISIS client utility routine)
  strcpy(envi->utcDate,ISODate());

  // All done (logging is done by the calling program)
  return 0;
}