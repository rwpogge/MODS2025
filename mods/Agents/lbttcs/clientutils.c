//
// clientutils - client app utilities go here
//

/*!
  \file clientutils.c
  \brief Client application utility functions

  Because they have to go somewhere...

  \author R. Pogge, OSU Astronomy Dept. (pogge@astronomy.ohio-state.edu)
  \date 2005 July 29
*/

#include "client.h" // custom client application header 

//---------------------------------------------------------------------------

/*!
  \brief Initialize an LBT information data structure

  \param lbti pointer to an #lbtinfo data structure

  Initializes the LBT telescope information data structure
  with sensible defaults/null values.  This ensures that
  we have some way to avoid data structures with delinquent
  information.

*/

void
initLBTInfo(lbtinfo_t *lbti) 
{
  lbti->Link = 0;    
  lbti->opMode = LABSIM;
  lbti->Turbo = 0; 
  strcpy(lbti->telescope,"LBT-SX");
  lbti->Latitude = LBTLAT;
  lbti->Longitude = LBTLON;
  lbti->Altitude = LBTALT;
  strcpy(lbti->utcDate,"CCYY-MM-DDThh:mm:ss");
  strcpy(lbti->utcTime,"hh:mm:ss");
  strcpy(lbti->telRA,"00:00:00.000");  
  strcpy(lbti->telDec,"+00:00:00.000"); 
  strcpy(lbti->objName,"none");
  strcpy(lbti->objRA,"none");
  strcpy(lbti->objDec,"none"); 
  lbti->objPMRA = 0.0;
  lbti->objPMDec = 0.0;
  lbti->objEpoch = 2000.0;
  strcpy(lbti->guiName,"none");
  strcpy(lbti->guiRA,"none");
  strcpy(lbti->guiDec,"none"); 
  lbti->guiPMRA = 0.0;
  lbti->guiPMDec = 0.0;
  lbti->guiEpoch = 2000.0;
  lbti->guiMag = 0.0;
  strcpy(lbti->HA,"cHH:MM:SS");  
  strcpy(lbti->LST,"hh:mm:ss"); 
  lbti->ZD = 0.0;     
  lbti->SecZ = 0.0;   
  lbti->Equinox = 2000.0;
  lbti->JD = 0.0;     
  lbti->MJD = 0.0;    
  lbti->HJD = 0.0;    
  lbti->parAngle = 0.0;
  lbti->sxFocus = 0;
  lbti->sxRotAngle = 0.0;
  lbti->sxPosAngle = 0.0;
  lbti->sxAuthorized=0;
  lbti->dxFocus = 0;
  lbti->dxRotAngle = 0.0;
  lbti->dxPosAngle = 0.0;
  lbti->dxAuthorized=0;
  strcpy(lbti->rotMode,"POSITION");
  strcpy(lbti->presetMode,"ACTIVE");
  lbti->reqPA = 0.0;
  lbti->lambda = 0.565;
  strcpy(lbti->iceProps,"config");  // default ICE properties file root name
  strcpy(lbti->proxy,"MODS_Raw");
  strcpy(lbti->instID,"MODS");
  strcpy(lbti->focStation,"directGregorian");
  strcpy(lbti->side,"LEFT");
  lbti->authorized = 0;
  strcpy(lbti->authInst,"none");
  lbti->objRARad  = -999.9; // clearly invalid...
  lbti->objDecRad = -999.9;
  lbti->guiRARad  = -999.9;
  lbti->guiDecRad = -999.9;

}

/*!
  \brief Print the last LBT TCS Query data

  \param lbti pointer to an #lbtinfo data structure

  Prints the contents of the #lbtinfo data structure describing the
  raw contents of the last TCS query.  The Date/Time give when
  the query occurred.  

*/

void
printLBTInfo(lbtinfo_t *lbti)
{
  if (!useCLI) return;

  printf("LBT TCS Info:\n");
  printf("  Telescope: %s\n",lbti->telescope);
  printf("  IIF Link: %s\n",(lbti->Link) ? "Up" : "Down");
  if (lbti->opMode == LIVE) 
    printf("  TCS Mode: Live LBT TCS\n");
  else
    printf("  TCS Mode: Simulated LBT TCS\n");
  if (lbti->Turbo)
    printf("  TURBO Mode enabled\n");

  printf("Last LBT TCS Query:\n");
  printf("  Date/Time of Query: %s UTC\n",lbti->utcDate);
  printf("  Position:  Equinox=%.1f RA=%s Dec=%s HA=%s LST=%s\n",
	 lbti->Equinox,lbti->telRA,lbti->telDec,lbti->HA,lbti->LST);
  printf("             ZD=%#4.2f SecZ=%#4.2f\n",lbti->ZD,lbti->SecZ);
  printf("  NOTE: Data above could be stale, note the query time...\n");
  printf("\n");
}

/*!
  \brief Initialize an LBT TCS link

  \param reply string with any messages

  \return 0 on success, -1 on errors

  Initializes the LBT telescope IIF instance and loads
  telescope-specific information into the various data structures.

*/

int
initLBTTCS(char *reply) 
{
  char fullFoc[64];

  sprintf(fullFoc,"%s %s",lbt.focStation,lbt.side);

  switch(lbt.opMode) {
  case LIVE:
    if (lbt.Link) mods_IIFClose();
    if (mods_IIFInit(lbt.proxy,fullFoc,lbt.instID)!=0) {
      sprintf(reply,"Cannot initialize LBT IIF instance - %s",mods_error());
      lbt.Link = 0;
      return -1;
    }
    else
      sprintf(reply,"LBT TCS link initialized");

    break;

  case LABSIM:
    sprintf(reply,"LBT IIF SIMULATION MODE initialized");
    lbt.Latitude  = LBTLAT; 
    lbt.Longitude = LBTLON;
    lbt.Altitude  = LBTALT;
    break;
  }
  lbt.Link = 1;
  lbt.Turbo = 0;
  return 0;

}

/*!
  \brief Query TCS info and load into global lbt struct

  \return 0 on success, -1 on errors.  Even if there are errors it
  loads the lbt time/date info.

  Sends and retrieves a TCS information query, putting the relevant
  information in the global lbt data structure.

*/

int
queryLBTTCS() 
{
  char tcsstr[128];
  char csgn;

  // What we do depends on the operations mode, opMode=LABSIM means
  // we return simulated information

  switch (lbt.opMode) {

  case LIVE:
    if (mods_GetTCSData(lbtTCS,lbt.side)!=0) {
      if (useCLI) printf ("Error in TCS query - %s\n",mods_error());
      return -1;
    }

    // Compute HA, ZD, etc. for time=now to remove any query latency

    getUTCTime(&utc);
    HAZDCalc(&lbt,&utc);

    // Convert encoder azimuth, elevation, and rotator angle from
    // arcseconds to degrees

    lbt.telAz = lbtTCS.telAz/3600.0;
    lbt.telEl = lbtTCS.telEl/3600.0;
    lbt.telRot = lbtTCS.telRot/3600.0; 
    
    // We compute ZD and SecZ from the LBT returns

    lbt.ZD = fabs(90.0 - lbt.telEl);
    if (lbt.ZD < 90.0) {
      lbt.SecZ = 1.0/DCOS(lbt.ZD);
      if (lbt.SecZ > 99.99) lbt.SecZ = 99.99; // near-horizon "no info" value
    }
    else
      lbt.SecZ = 99.99;

    // Convert RA and Dec in radians to sexagesimal strings

    SexStr(radhr(lbtTCS.telRA),lbt.telRA);
    SexStr(raddeg(lbtTCS.telDec),lbt.telDec);

    // Convert to degrees from radians as needed

    lbt.rotAngle = raddeg(lbtTCS.rotAngle);
    lbt.posAngle = raddeg(lbtTCS.posAngle);
    lbt.parAngle = lbtTCS.parAngle;  
    if (strlen(&lbtTCS.rotMode[0])==0)
      strcpy(lbt.rotMode,"Unknown");
    else
      strcpy(lbt.rotMode,&lbtTCS.rotMode[0]);

    break;

  case LABSIM: 
    getUTCTime(&utc);  // query current UTC time
    HAZDCalc(&lbt,&utc);
    lbt.Equinox = 2000.0;
    lbt.sxRotAngle = lbt.parAngle - lbt.sxPosAngle;
    lbt.dxRotAngle = lbt.parAngle - lbt.dxPosAngle;
    lbt.JD = utc.JD;
    if (lbt.sxFocus == 0) lbt.sxFocus = 3456;
    if (lbt.dxFocus == 0) lbt.dxFocus = 1234;
    break;

  } // end of the opMode switch

  // Last step, remove any possible query latencies by checking the time now.
  // Supercedes date, time, JD, and MJD reported by TCS

  getUTCTime(&utc);
  strcpy(lbt.utcDate,utc.Date);
  strcpy(lbt.utcTime,utc.Time);
  lbt.JD = utc.JD;
  lbt.MJD = utc.MJD;

  return 0;
  
}

//-------------------------------------------------------------------------

/*!
  \brief Return UTC system time to usec precision in a utcinfo_t struct

  \param utci pointer to a utcinfo_t data structure

  Reads the system's UTC time clock and fills the utcinfo_t struct
  with all the time info we need, including JD and MJD representations.

  Based on gf_time() from Stevens, W.R., 1998, Unix Network Programming,
  Vol 2, Prentice Hall, Figure 15.6. 

  Calls CalToMJD() from the SkyUtils library, which is based on the
  Duffet-Smith algorithms, among others.

*/

void
getUTCTime(utcinfo_t *utci)
{
  struct timeval tv;
  struct tm *gmt;
  double utday;
  double mjd;
  int msec;
  int sec;

  gettimeofday(&tv,NULL);
  gmt = gmtime(&tv.tv_sec);

  // UTC date and time components

  utci->Year = 1900 + gmt->tm_year;
  utci->Month = 1 + gmt->tm_mon;
  utci->Day = gmt->tm_mday;
  utci->Hour = gmt->tm_hour;
  utci->Min = gmt->tm_min;
  sec = gmt->tm_sec;
  msec = (int)(tv.tv_usec/1000);
  utci->Sec = (double)(sec) + ((double)(tv.tv_usec)/1000000.0);

  // UTC data and time string representations (ISO8601)

  sprintf(utci->Date,"%.4i-%.2i-%.2i",utci->Year,utci->Month,utci->Day);
  sprintf(utci->Time,"%.2i:%.2i:%06.3f",utci->Hour,utci->Min,utci->Sec);
  sprintf(utci->ISO,"%sT%s",utci->Date,utci->Time);

  // Decimal time values

  utci->dtime = double(utci->Hour) + ((double)(utci->Min)/60.0) + (utci->Sec/3600.0);

  utday = (double)(utci->Day) + (utci->dtime/24.0);

  // JD and MJD

  CalToMJD(utci->Month,utday,utci->Year,&mjd);
  utci->JD = mjd + MJD0;
  utci->MJD = utci->JD - MJDF0; // standard MJD=JD-2450000.5

}


/*!
  \brief Compute HA, LST, HJD, ZD, and SecZ

  \param lbti pointer to a lbtinfo_t data structure
  \param utci pointer to a utcinfo_t data structure

  \return 0 on success, -1 if there are problems.

  Computes the hour angle, local sidereal time, heliocentric Julian
  Day number, zenith distance, and secant of the zenith distance for
  the current telescope coordinates in the lbtinfo_t data structure at
  the UTC time referenced by the contents of the utcinfo_t data
  structure.  Results are put into the relevant members of the
  lbtinfo_t struct.

  Calls routines from the SkyUtils library, which is based on the
  Duffet-Smith algorithms, among others.

*/

int
HAZDCalc(lbtinfo_t *lbti, utcinfo_t *utci)
{
  int rah, ram;
  double ras;
  double dra;

  char dsign[2];
  int decd, decm;
  double decs;
  double ddec;
  double parity;

  double utday0;
  double utday;
  double mjd0;
  double gmst;
  double lmst;
  double dha;

  // If no current TCS info, leave now

  if (lbti->Link != 1) return -1; 

  // Decompose sexigesimal RA into components and decimal hours

  sscanf(lbti->telRA,"%d:%d:%f",&rah,&ram,&ras);
  dra = (double)(rah) + ((double)(ram)/60.0) + ((double)(ras)/3600.0) ;

  // Decompose sexigesimal Dec into components and decimal degrees.  This
  // takes some extra logic because we may or may not have a sign.

  sscanf(lbti->telDec,"%1s",dsign);
  if (strcasecmp(dsign,"-")==0) {
    parity = -1.0;
    sscanf(lbti->telDec,"%1s%d:%d:%f",dsign,&decd,&decm,&decs);
  }
  else if (strcasecmp(dsign,"+")==0) {
    parity = 1.0;
    sscanf(lbti->telDec,"%1s%d:%d:%f",dsign,&decd,&decm,&decs);
  }
  else { // no sign = implicit positive declination
    parity = 1.0;
    sscanf(lbti->telDec,"%d:%d:%f",dsign,&decd,&decm,&decs);
  }
  ddec = parity*((double)(decd) + ((double)(decm)/60.0) 
		                + ((double)(decs)/3600.0));

  // Get time components

  utday0 = (double)(utci->Day);         // day at UTC 0h
  utday  = utday0 + (utci->dtime/24.0); // day plus fraction to UTC time

  // Compute MJD at UTC midnight on this day

  CalToMJD(utci->Month,utday0,utci->Year,&mjd0);

  // Compute GST in decimal hours

  UTCtoGST(mjd0,utci->dtime,&gmst);

  // Compute LST in decimal hours and sexigesmial equivalent

  lmst = gmst - (lbti->Longitude/15.0);
  Range(&lmst,24.0);
  SexStr(lmst,lbti->LST);

  // Compute the HA in decimal hours and sexigesimal equivalent
  // Make sure HA is -12..+12

  dha = lmst - dra;

  if (dha >  12.0) dha = dha - 24.0;
  if (dha < -12.0) dha = 24.0 + dha;

  // end fixes...

  SexStr(dha,lbti->HA);

  // Compute the zenith distance and secant(ZD) - now reported by LBT proper, so
  // this bit is only done in simulation mode

  Zenith(dha,ddec,lbti->Latitude,&lbti->ZD,&lbti->SecZ);

  // Compute the HJD for the coordinates

  lbti->HJD = JDtoHJD(utci->JD,dra,ddec);

  // Compute elevation, azimuth, and parallactic angle

  AzEl(lbti->Latitude,dha,ddec,&lbti->telAz,&lbti->telEl,&lbti->parAngle);

  // All done

  return 0;
}
