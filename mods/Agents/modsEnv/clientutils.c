//
// clientutils - client app utilities go here
//

/*!
  \file clientutils.c
  \brief Client application utility functions

  \author R. Pogge, OSU Astronomy Dept. (pogge.1@osu.edu)
  \date 2010 June 21

  \date 2025 Aug 17 - updates for Archon HEB and LBTO telemetry [rwp/osu]
  
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
void
initEnvData(envdata_t *envi)
{
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

  strcpy(envi->hebB_Addr,"");
  envi->hebB_AirTemp = 0.0;
  envi->blueDewTemp = 0.0;
  envi->blueArchon = 0;
  envi->blueIonGauge = 0;
  
  strcpy(envi->hebR_Addr,"");
  envi->hebR_AirTemp = 0.0;
  envi->redDewTemp = 0.0;
  envi->redArchon = 0;
  envi->redIonGauge = 0;

  strcpy(envi->blueIG_Addr,"");
  envi->blueIG_Port = 8018;
  envi->blueIG_Chan = 5;
  envi->blueDewPres = 0.0;
  
  strcpy(envi->redIG_Addr,"");
  envi->redIG_Port = 8018;
  envi->redIG_Chan = 5;
  envi->redDewPres = 0.0; 
  
  envi->doLogging = 1;                      // enable enviromental data logging by default
  envi->useHdf5 = 1;                        // default: output environmental data to Hdf5
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
void
printEnvData(envdata_t *envi)
{
  if (!useCLI) return;

  printf("Environmental Monitor Agent Info:\n");
  printf("  Instrument: %s\n",envi->modsID);
  printf("       IUB WAGO IP Address: %s\n",envi->iub_Addr);
  printf("  Blue IEB WAGO IP Address: %s\n",envi->iebB_Addr);
  printf("   Red IEB WAGO IP Address: %s\n",envi->iebR_Addr);
  printf("  Blue HEB WAGO IP Address: %s\n",envi->hebB_Addr);
  printf("   Red HEB WAGO IP Address: %s\n",envi->hebR_Addr);
  printf("      Blue Dewar Ion Gauge: %s:%04d channel %02d\n",envi->blueIG_Addr,envi->blueIG_Port,envi->blueIG_Chan);
  printf("       Red Dewar Ion Gauge: %s:%04d channel %02d\n",envi->redIG_Addr,envi->redIG_Port,envi->redIG_Chan);
  printf("  Monitor Status: %s\n",(envi->pause) ? "PAUSED" : "Active");
  printf("  Sampling Cadence: %d seconds\n",envi->cadence);
  printf("  Data Logging: %s\n",(envi->doLogging) ? "Enabled" : "Disabled");
  envi->logFD == 0 ? printf("  Data Log File: CLOSED\n") : printf("  Data Log File: %s\n",envi->logFile);
  printf("  HDF Logging: %s\n",(envi->useHdf5) ? "Enabled" : "Disabled");
  envi->useHdf5 == 0 ? printf("  Hdf Log Directory: CLOSED\n") : printf("  Hdf Log Directory: %s\n",envi->hdfRoot);
  printf("\nMost Recent Sensor Data:\n");
  printf("  Date/Time: UTC %s\n",envi->utcDate);
  printf("  Ambient Temp: %.1f C\n",envi->ambientTemp);
  printf("  Glycol Supply: P=%.2f psi-g  T=%.1f C\n",
	 envi->glycolSupplyPres,envi->glycolSupplyTemp);
  printf("         Return: P=%.2f psi-g  T=%.1f C\n",
	 envi->glycolReturnPres,envi->glycolReturnTemp);
  printf("  IUB Air: %.1f C   HeatSink: %.1f C\n",
	 envi->utilBoxTemp, envi->agwHSTemp);
  printf("  IEB Air: Red=%.1f Blue=%.1f C\n",
	 envi->iebR_AirTemp, envi->iebB_AirTemp);
  printf("   Glycol: Red=%.1f Blue=%.1f C\n",
	 envi->iebR_ReturnTemp, envi->iebB_ReturnTemp);
  printf("  HEB Air: Red=%.1f Blue=%.1f C\n",
	 envi->hebR_AirTemp, envi->hebB_AirTemp);
  printf("  Dewar:\n");
  printf("     Red: T=%1.f C  P=%8.2e torr \n",envi->redDewTemp,envi->redDewPres);
  printf("    Blue: T=%1.f C  P=%8.2e torr \n",envi->blueDewTemp,envi->blueDewPres);
  printf("  Instrument:\n");
  printf("      Air Temp: Top=%.1f Bot=%.1f C\n",envi->airTopTemp, envi->airBotTemp);
  printf("    Truss Temp: Top=%.1f Bot=%.1f C\n",envi->trussTopTemp, envi->trussBotTemp);
  printf("\nClient Info:\n");
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
int
getEnvData(envdata_t *envi)
{
  int ierr;
  uint16_t iubData[10];   // raw IUB WAGO data array
  uint16_t iebRData[10];  // raw Red IEB WAGO data array
  uint16_t iebBData[10];  // raw Blue IEB WAGO data array
  uint16_t llbData[10];   // raw Blue IEB WAGO data array
  uint16_t hebRData[4];   // raw Red HEB WAGO data array
  uint16_t hebBData[4];   // raw Blue HEB WAGO data array
  
  int iubPower   = 0;     // IUB AC power relay status word
  int iubBreaker = 0;     // IUB AC ciruit breaker status word
  int hebRPower = 0;      // Red HEB power relay status word
  int hebBPower = 0;      // Blue HEB power relay status word
  
  //
  // IUB WAGO register addresses - Aug 2025
  //   IUB pressure and temperature sensors: 0
  //   Circuit breaker sensors: 10
  //   Power relay digital out: 512
  //
  
  // Get data from the IUB environmental sensors

  ierr = wagoSetGetRegisters(0,envi->iub_Addr,0,10,iubData);

  // If we cannot read the IUB, it means probably everything else is off as well.
  // since all AC power goes through the IUB wago
  
  if (ierr != 0) {
    if (useCLI) printf("WARNING: %s IUB WAGO read error\n",envi->modsID);
    return ierr;

    // if IUB is off, everything else is also probably off
    
    shm_addr->MODS.utilState = 0;
    shm_addr->MODS.llbState = 0;
    shm_addr->MODS.blueIEBState = 0;
    shm_addr->MODS.redIEBState = 0;
    shm_addr->MODS.blueHEBState = 0;
    shm_addr->MODS.redHEBState = 0;
    shm_addr->MODS.guideCamState = 0;
    shm_addr->MODS.wfsCamState = 0;
  }
  else {
    envi->glycolSupplyPres = (float)iubData[0]/327.64;
    envi->glycolReturnPres = (float)iubData[1]/327.64;
    envi->glycolSupplyTemp = ptRTD2C(iubData[4]);
    envi->glycolReturnTemp = ptRTD2C(iubData[5]);
    envi->agwHSTemp   = ptRTD2C(iubData[6]);
    envi->utilBoxTemp = ptRTD2C(iubData[7]);
    envi->ambientTemp = ptRTD2C(iubData[8]);

    shm_addr->MODS.utilState = 1;
    shm_addr->MODS.glycolSupplyPressure = envi->glycolSupplyPres;
    shm_addr->MODS.glycolReturnPressure = envi->glycolReturnPres;
    shm_addr->MODS.glycolSupplyTemperature = envi->glycolSupplyTemp;
    shm_addr->MODS.glycolReturnTemperature = envi->glycolReturnTemp;
    shm_addr->MODS.utilBoxAirTemperature = envi->utilBoxTemp;
    shm_addr->MODS.outsideAirTemperature = envi->ambientTemp;
    shm_addr->MODS.agwHeatSinkTemperature = envi->agwHSTemp;
  }
  
  // Get the IUB AC power control status data

  ierr = wagoSetGetRegisters(0,envi->iub_Addr,512,1,iubData);
  if (ierr < 0) {
    if (useCLI) printf("WARNING: %s IUB WAGO error reading power status word\n",envi->modsID);
  }
  else {
    iubPower = iubData[0];
  
    // On/Off state depends on the wiring of the AC power switching
    // relay: normally-open (OFF on mains power up) or normally-closed
    // (ON on mains power up)
    envi->iebR_Switch = ((iubPower & IEB_R_POWER) != IEB_R_POWER); // normally closed
    envi->iebB_Switch = ((iubPower & IEB_B_POWER) != IEB_B_POWER); // normally closed
    envi->hebR_Switch = ((iubPower & HEB_R_POWER) != HEB_R_POWER); // normally closed - changed Aug 2025
    envi->hebB_Switch = ((iubPower & HEB_B_POWER) != HEB_B_POWER); // normally closed - changed Aug 2025
    envi->gcam_Switch = ((iubPower & GCAM_POWER)  == GCAM_POWER);  // normally OPEN
    envi->wfs_Switch  = ((iubPower & WFS_POWER)   == WFS_POWER);   // normally OPEN
    envi->llb_Switch  = ((iubPower & LLB_POWER)   != LLB_POWER);   // normally closed

    shm_addr->MODS.utilState = 1;
    shm_addr->MODS.llbState = envi->llb_Switch;
    shm_addr->MODS.blueIEBState = envi->iebB_Switch;
    shm_addr->MODS.redIEBState = envi->iebR_Switch;
    shm_addr->MODS.blueHEBState = envi->hebB_Switch;
    shm_addr->MODS.redHEBState = envi->hebR_Switch;
    shm_addr->MODS.guideCamState = envi->gcam_Switch;
    shm_addr->MODS.wfsCamState = envi->wfs_Switch;
  }
  
  // Get the IUB AC power breaker output side current sensor data
  
  ierr = wagoSetGetRegisters(0,envi->iub_Addr,10,1,iubData);
  if (ierr < 0) {
    if (useCLI) printf("WARNING: %s cannot read IUB breaker output status\n",envi->modsID);
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

  // Red and Blue IEB WAGO RTD temperature sensor register base address: 0

  // Get data from the Red IEB environmental sensors
  
  ierr = wagoSetGetRegisters(0,envi->iebR_Addr,0,10,iebRData);
  if (ierr < 0) {
    if (useCLI) printf("WARNING: %s Red IEB WAGO read error\n",envi->modsID);

    shm_addr->MODS.redIEBState = 0;
  }
  else {
    envi->iebR_AirTemp = ptRTD2C(iebRData[4]);
    envi->iebR_ReturnTemp = ptRTD2C(iebRData[5]);
    envi->airTopTemp = ptRTD2C(iebRData[6]);
    envi->airBotTemp = ptRTD2C(iebRData[7]);

    shm_addr->MODS.redIEBState = 1;
    shm_addr->MODS.redTemperature[0] = envi->iebR_AirTemp;
    shm_addr->MODS.redTemperature[1] = envi->iebR_ReturnTemp;
    shm_addr->MODS.redTemperature[2] = envi->airTopTemp;
    shm_addr->MODS.redTemperature[3] = envi->airBotTemp;
  }
  
  // Get data from the Blue IEB environmental sensors
  
  ierr = wagoSetGetRegisters(0,envi->iebB_Addr,0,10,iebBData);
  if (ierr < 0) {
    if (useCLI) printf("WARNING: %s Blue IEB WAGO read error\n",envi->modsID);

    shm_addr->MODS.blueIEBState = 0;
  }
  else {
    envi->iebB_AirTemp = ptRTD2C(iebBData[4]);
    envi->iebB_ReturnTemp = ptRTD2C(iebBData[5]);
    envi->trussTopTemp = ptRTD2C(iebBData[6]);
    envi->trussBotTemp = ptRTD2C(iebBData[7]);

    shm_addr->MODS.blueIEBState = 1;
    shm_addr->MODS.blueTemperature[0] = envi->iebB_AirTemp;
    shm_addr->MODS.blueTemperature[1] = envi->iebB_ReturnTemp;
    shm_addr->MODS.blueTemperature[2] = envi->trussTopTemp;
    shm_addr->MODS.blueTemperature[3] = envi->trussBotTemp;
  }

  // Red and Blue HEB power relays and sensor register addresses:
  //   RTD module: 4
  //   Power relays: 512

  // Red HEB power control status - both are normally open relays

  ierr = wagoSetGetRegisters(0,envi->hebR_Addr,512,1,hebRData);
  if (ierr < 0) {
    if (useCLI) printf("WARNING: %s Red HEB WAGO error reading power relay status word\n",envi->modsID);

    // If an HEB is off, so are systems powered through it

    shm_addr->MODS.redHEBState = 0;
    shm_addr->MODS.redArchonState = 0;
    shm_addr->MODS.redIonGaugeState = 0;
  }
  else {
    hebRPower = hebRData[0];
    envi->redArchon = ((hebRPower & ARCHON_POWER) == ARCHON_POWER);
    envi->redIonGauge = ((hebRPower & IG_POWER) == IG_POWER);

    shm_addr->MODS.redHEBState = 1;
    shm_addr->MODS.redArchonState = envi->redArchon;
    shm_addr->MODS.redIonGaugeState = envi->redIonGauge;
  }

  // Blue HEB power control status - both are normally open relays

  ierr = wagoSetGetRegisters(0,envi->hebR_Addr,512,1,hebBData);
  if (ierr < 0) {
    if (useCLI) printf("WARNING: %s Blue HEB WAGO error reading power relay status word\n",envi->modsID);

    // If an HEB is off, so are systems powered through it
    
    shm_addr->MODS.blueHEBState = 0;
    shm_addr->MODS.blueArchonState = 0;
    shm_addr->MODS.blueIonGaugeState = 0;
  }
  else {
    hebBPower = hebBData[0];
    envi->blueArchon = ((hebBPower & ARCHON_POWER) == ARCHON_POWER);
    envi->blueIonGauge = ((hebBPower & IG_POWER) == IG_POWER);

    shm_addr->MODS.blueHEBState = 1;
    shm_addr->MODS.blueArchonState = envi->blueArchon;
    shm_addr->MODS.blueIonGaugeState = envi->blueIonGauge;
  }

  // Red HEB temperature measurements

  ierr = wagoSetGetRegisters(0,envi->hebR_Addr,4,2,hebRData);
  if (ierr < 0) {
    if (useCLI) printf("WARNING: %s Red HEB WAGO RTD sensor read error\n",envi->modsID);

    shm_addr->MODS.redHEBState = 0;
    shm_addr->MODS.redHEBTemperature = nanf();
    shm_addr->MODS.redDewarTemperature = nanf();
  }
  else {
    envi->hebR_AirTemp = ptRTD2C(hebRData[0]);
    envi->redDewTemp = ptRTD2C(hebRData[1]);

    shm_addr->MODS.redHEBState = 1;
    shm_addr->MODS.redHEBTemperature = envi->hebR_AirTemp;
    shm_addr->MODS.redDewarTemperature = envi->redDewTemp;
  }
  
  // Blue HEB temperature measurements

  ierr = wagoSetGetRegisters(0,envi->hebB_Addr,4,2,hebBData);
  if (ierr < 0) {
    if (useCLI) printf("WARNING: %s Blue HEB WAGO RTD sensor read error\n",envi->modsID);

    shm_addr->MODS.blueHEBState = 0;
    shm_addr->MODS.blueHEBTemperature = nanf();
    shm_addr->MODS.blueDewarTemperature = nanf();
  }
  else {
    envi->hebB_AirTemp = ptRTD2C(hebBData[0]);
    envi->blueDewTemp = ptRTD2C(hebBData[1]);

    shm_addr->MODS.blueHEBState = 1;
    shm_addr->MODS.blueHEBTemperature = envi->hebB_AirTemp;
    shm_addr->MODS.blueDewarTemperature = envi->blueDewTemp;
  }

  // Read the red and blue ionization gauges here (TCP/IP socket to IEB 2-channel Comtrols)

  // Red dewar ion gauge
  
  envi->redDewPres = getIonPressure(envi->redIG_Addr, envi->redIG_Port, envi->redIG_Chan, ION_TIMEOUT_LENGTH);
  shm_addr->MODS.redDewarPressure = envi->redDewPres;

  // Blue dewar ion gauge
  
  envi->blueDewPres = getIonPressure(envi->blueIG_Addr, envi->blueIG_Port, envi->blueIG_Chan, ION_TIMEOUT_LENGTH);
  shm_addr->MODS.blueDewarPressure = envi->blueDewPres;

  //...
  
  // IR laser power and status complicated, maybe someday...
  
  // Get the UTC date/time of the query (ISIS client utility routine)

  strcpy(envi->utcDate,ISODate());

  // All done (logging is done by the calling program)

  return 0;
}

//---------------------------------------------------------------------------
//
// ptRTD2C() - convert WAGO Pt RTD sensor raw data to degrees C
//
// Arguments:
//   short rawRTD - raw RTD datum
//
// Description:
//   Converts WAGO RTD module raw integer datum into degrees C float.
//
//   The WAGO module has a temperature resolution of 0.1C and a temperature
//   range of -273 to 850C. If temp in >850.0, short integer raw data are
//   wrapped on 2^16-1 ADU.  This is how it reports negative temperatures
//   with a 16-bit unsigned integer.
//
//   Author:
//     R. Pogge, OSU Astronomy Dept
//     pogge.1@osu.edu
//     2025 July 17
//
//---------------------------------------------------------------------------

float
ptRTD2C(short rawData)
{
  float tempMax = 850.0; // max temperature reported
  float tempRes = 0.1;   // temperature resolution 0.1C
  float temp, wrapT;
  
  wrapT = tempRes*(pow(2.0,16)-1);
  temp = tempRes*(float)rawData;
  if (temp > tempMax)
    temp -= wrapT;

  return temp;
}
