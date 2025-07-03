#include "./modsIIFUtils.h"    // MODS IIF utility structs

#define __PAUSERESUME
#define EXIT_WARNING 2

// Save the result messages coming from TCS in iifmsgs[]

void showResults(const lbto::result _res, const string _cmd)
{
  unsigned int i;
  iifcount=0;
  memset(iifmsgs,0,sizeof(iifmsgs));
  if (_res.rescode == 0) {
    strcpy(iifmsgs[0],"SUCCESS");
    for (i=1; i< _res.resmsg.size(); i++)
      strcpy(iifmsgs[i],&_res.resmsg[i][0]); 
  }
  else {
    for (i=0; i< _res.resmsg.size(); i++)
      strcpy(iifmsgs[i],&_res.resmsg[i][0]);
  }
  iifcount = i;
}

//----------------------------------------------------------------
//
// mods_IIFClose() - Closes the ICE IFF instance.
//
// See Also: mods_IIFInit()
//

int
mods_IIFClose()
{

  if (!factory) throw "Invalid proxy: IIF Server not found.";

  if (iif>0) factory->destroy(iif);
 
  sprintf(iifmsgs[0],"Closing proxy '%s' for '%s' on '%s'", 
	  clientProxy_Name, focalStation_Name, instrument_Name);

  res.rescode = EXIT_SUCCESS;
  res.resmsg.clear();

  return EXIT_SUCCESS;
}

//----------------------------------------------------------------
//
// mods_OpenIceComm() - Open IIF/Ice communications
//
// returns error or success
//
// Uses the error message(s) returned in the result structure
// described in 481s013s Section 7.1
//
// Changes here to make the path/name configurable, either at compile
// time with APP_CFGDIR or at runtime using the MODS_CONFIG
// environment variable.
//

int
mods_OpenIceComm(char *propsRoot, int argc, char *argv[])
{
  // Set up the configuration file. Look in the default configuration
  // directory defined at compile-time and if not found, check the
  // environment variable MODS_CONFIG, or fail.

  char cfgFile[256];  // full qualfied configuration filename

  // Build the full client properties filename

  if (strlen(propsRoot)<=0)
    sprintf(cfgFile,"%s/config.client",APP_CFGDIR);
  else
    sprintf(cfgFile,"%s/%s.client",APP_CFGDIR,propsRoot);

  char * config_file = cfgFile; 

  // If this does not exist, see if MODS_CONFIG is defined in the
  // environment

  if (access(config_file,R_OK) == -1) {
    config_file = getenv("MODS_CONFIG");
    if (access(config_file,R_OK) == -1) {
      sprintf(iifmsgs[0],"ICE/IIF properties file %s not found",
	      cfgFile);
      iifcount=1;
      return EXIT_FAILURE;
    }
  }

  Ice::PropertiesPtr props = Ice::createProperties();
  props->load(config_file);
  Ice::InitializationData id;
  id.properties = props;
  
  communicator = Ice::initialize(argc, argv, id); //Connect with IIF Factory
  factory = FactoryPrx::checkedCast(communicator->propertyToProxy("Factory.Proxy"));
  string proxyName = props->getPropertyWithDefault("MODS.ProxyName","MODS1");
  string instrumentID = props->getPropertyWithDefault("MODS.InstrumentID","MODS");
  string focalStation = props->getPropertyWithDefault("MODS.FocalStation","directGregorian left");

  strcpy(clientProxy_Name,&proxyName[0]);
  strcpy(focalStation_Name,&focalStation[0]);
  strcpy(instrument_Name,&instrumentID[0]);

  return EXIT_SUCCESS;
}

//----------------------------------------------------------------
//
// mods_error() - return the most recent error message generated 
//                by a function call. 
//
// If a function does not result in an error, this should return a
// blank (NULL) string.
//
// Uses the error message(s) returned in the result structure
// described in the 481s013s Section 7.1
//
// This should concatenate all the messages into one long string,
// separated by comments for readability.
//

char
*mods_error()
{
  char errMsg[1024];
  if (iifcount > 0) {
    strcpy(errMsg,iifmsgs[0]);
    for (int i=1;i<iifcount;i++)
      sprintf(errMsg,"%s, %s",errMsg,iifmsgs[i]);
  }
  else
    strcpy(errMsg,"none");

  return(errMsg);
}

//----------------------------------------------------------------
//
// mods_IIFInit() - Initialize the ICE communicator and connect 
//                  to the IIF server. 
//
// Returns EXIT_SUCCESS if the connection is successfully initialized,
// or EXIT_FAILURE if not, with diagnostic info retrievalbe with the
// mods_error() method.
//
// Valid values for instID and focStation are defined in Table 6.1 in
// the ICE Instrument ICD (481s013s).
//
// We are free to define the proxyName as we need, but it must be
// stored and returned with the mods_proxy() method.
//
// Example: mods_IIFInit("MODS1 left","MODS","directGregorian left")
//
// See Also: mods_Authorize(), mods_error(), mods_proxy()
//

int
mods_IIFInit(char *proxyName, char *focalStation, char *instrID)
{
  char *commandSide;   // "right"
  int status = EXIT_SUCCESS;
  int ierr = 0;

  try
    {
      if (!factory) throw "Invalid proxy: IIF Server not found.";

      // Get a proxy object for this instrument. If the proxy already exists, 
      // the arbitrator will link the proper IIF instance with it.
      // Otherwise, it will create a new IIF instance for this client.

      iif = factory->create(clientProxy_Name, focalStation_Name,instrument_Name);
      if (!iif)
	sprintf(iifmsgs[0],"Invalid proxy: IIF Factory failed to create a new instance.");
      else {
	res.rescode = EXIT_SUCCESS;
	res.resmsg.clear();
      }
    }
  catch(const Ice::Exception& ex)
    {
      sprintf(iifmsgs[0],"%s\n", &ex);
      //cerr << ex << endl;
      status = EXIT_FAILURE;
    }
  
  catch (const char* msg)
    {
      sprintf(iifmsgs[0],"%s\n",msg);
      status = EXIT_FAILURE;
    }
  
  catch (string msg)
    {
      sprintf(iifmsgs[0],"%s\n",&msg[0]);
      status = EXIT_FAILURE;
    }

  return status;
}

//----------------------------------------------------------------
//
// mods_Authorize() - Return the authorization status of the 
//                    ICE IIF instance.
//
// Implements the Authorize() function described in 481s013s section
// 8.2
//
// Returns 1 if authorized ("true"), 0 if not authorized ("false")
//
// See Also: mods_IIFInit()
//

int
mods_Authorize()
{
  iif = factory->create(clientProxy_Name, focalStation_Name,instrument_Name);

  if (!iif)
    sprintf(iifmsgs[0],"Invalid proxy: IIF Factory failed to create a new instance.");
  
  res = iif->Authorize();
  showResults(res,"Authorize");
  if (res.rescode != EXIT_SUCCESS) {
    sprintf(iifmsgs[0],"'%s' Instrument _NOT_ Authorized, Need to IIFInit communications",instrument_Name);
    return 0;
  } 
  sprintf(iifmsgs[0],"'%s' Instrument Authorized with '%s'",instrument_Name,focalStation_Name);
  return 1;
}

//----------------------------------------------------------------
//
// mods_proxy() - return the name of the client proxy used by this
//                ICE IIF instance.
//
// Returns the client proxy name defined by a previous successful call
// to mods_IIFInit().  It should return NULL if mods_IIFInit() failed
// to create an ICE IIF instance.
//
// See Also: mods_IIFInit()
//

char 
*mods_proxy()
{
  return clientProxy_Name;
}

//----------------------------------------------------------------
//
// mods_ClearStars() - remove target and guide stars defined
//                     by a previous SetStars() call.
//
// Implements the ClearStars() function (481s013s Section 8.9)
//
// If an error occurs, return EXIT_FAILURE and set mods_error() with
// the error message(s) returned.
//

int 
mods_ClearStars()
{
  iif = factory->create(clientProxy_Name, focalStation_Name,instrument_Name) ;
  if (!iif)
    sprintf(iifmsgs[0],"Invalid proxy: IIF Factory failed to create a new instance.");
  
  res = iif->ClearStars();
  showResults(res,"ClearStars");
  if (res.rescode != EXIT_SUCCESS) {
    if (iifcount == 0)
      strcpy(iifmsgs[0],"Error: ClearStars Failed");
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

//----------------------------------------------------------------
//
// mods_LogEvent() - send message to the TCS event logging system
//
// Implements the LogEvent() function (481s013s Section 8.16), sending
// the eventName and eventDescription strings given. If an error
// occurs, return EXIT_FAILURE, the reason can be retrieved using
// mods_error().
//
//
// Example: mods_LogEvent("MODS1_Oops","We have no idea what just happened");
//

int 
mods_LogEvent(char *eventName, char *eventDescription)
{
  iif = factory->create(clientProxy_Name, focalStation_Name,instrument_Name) ;
  if (!iif)
    sprintf(iifmsgs[0],"Invalid proxy: IIF Factory failed to create a new instance.");
    
  res = iif->LogEvent((string)eventName,(string)eventDescription);
  showResults(res,"LogEvent");
  if (res.rescode != EXIT_SUCCESS) {
    if (iifcount == 0)
      strcpy(iifmsgs[0],"Error: LogEvent Failed");
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

//----------------------------------------------------------------
//
// mods_MaximizeWrapTime() - control behavior of azimuth and rotator
//                           cable wraps on the next preset
//
// Implements the MaximizeWrapTime() function (481s013s Section 8.17),
// sending the arguments given. If an error occurs, return
// EXIT_FAILURE and set mods_error().
//
// Example: mods_MaximizeWrapTime(false,false,"left");
//
// NOTE: The current IIF Support Status table on the LBTO wiki
// indicates that MaximizeWrapTime() is "operational but not
// supported".  It is implemented here as a placeholder for when (or
// if) they get around to supporting this function.
//

int 
mods_MaximizeWrapTime(bool azFlag, bool rotFlag, char *side)
{
  iif = factory->create(clientProxy_Name, focalStation_Name,instrument_Name) ;
  if (!iif)
    sprintf(iifmsgs[0],"Invalid proxy: IIF Factory failed to create a new instance.");

  res = iif->MaximizeWrapTime(azFlag, rotFlag, (string)side);
  showResults(res,"MaximizeWrapTime");
  if (res.rescode != EXIT_SUCCESS) {
    if (iifcount == 0)
      strcpy(iifmsgs[0],"Error: MaximizeWrapTime Failed");
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

//--------------------------------------------------------------------------
//
// mods_OffsetPointing() - offset the telescope
//
// Implements the OffsetPointing() function (418s013p section 8.23),
// sending the arguments given to make either a pointing and/or
// rotator offset.  dPA are in radians, offX and offY are either in
// radians (if coordSys=RADEC) or millimeters (if coordSys=DETXY).
// coordSys must be one of "RADEC" or "DETXY" and moveType is one of
// "REL" or "ABS".
//
// If an error occurs, return EXIT_FAILURE and set mods_error().
//
// We only implement offsets in the RADEC and DETXY (aka PCS_XY)
// coordinate systems, and we impose OPE="DEFAULT", and
// NEW_POSITION="true".
//
// Examples: mods_OffsetPointing(0.123,-0.005,0.0,"RADEC","ABS","left");
//           mods_OffsetPointing(0.0,0.0015,-0.0023,"DETXY","REL","right");
//
// See Also: mods_PresetTelescope()
//
// Added trap for EXIT_WARNING, but note that warnings are not always
// consistently implemented in all IIF functions.
//

int 
mods_OffsetPointing(double dPA, double offX, double offY, char *coordSys,
		    char *moveType, char *side)
{
  iif = factory->create(clientProxy_Name, focalStation_Name,instrument_Name) ;
  if (!iif)
    sprintf(iifmsgs[0],"Invalid proxy: IIF Factory failed to create a new instance.");

  res = iif->OffsetPointing(dPA,offX,offY,(string)coordSys,"DEFAULT","true",
			    (string)moveType,(string)side);

  showResults(res,"OffsetPointing");
  if (res.rescode != EXIT_SUCCESS) {
    if (res.rescode == EXIT_WARNING) {
      if (iifcount == 0)
	strcpy(iifmsgs[0],"WARNING: OffsetPointing returned warning");
      return EXIT_SUCCESS;
    }
    else {
      if (iifcount == 0)
	strcpy(iifmsgs[0],"Error: OffsetPointing Failed");
      return EXIT_FAILURE;
    }
  }
  return EXIT_SUCCESS;
}

//----------------------------------------------------------------
//
// mods_PauseGuiding() - pause autoguiding if active
//
// Implements the PauseGuiding() function (481s013s Section 8.28).  If
// an error occurs, return EXIT_FAILURE and set mods_error().
//
// Example: mods_PauseGuiding("left");
//
// See Also: mods_ResumeGuiding()
//

int 
mods_PauseGuiding(char *side)
{
  iif = factory->create(clientProxy_Name, focalStation_Name,instrument_Name) ;
  if (!iif)
    sprintf(iifmsgs[0],"Invalid proxy: IIF Factory failed to create a new instance.");

#ifdef __PAUSERESUME  
  res = iif->PauseGuiding((string)side);
  showResults(res,"PauseGuiding");
#endif
  if (res.rescode != EXIT_SUCCESS) {
    if (iifcount == 0)
      strcpy(iifmsgs[0],"Error: PauseGuiding Failed");
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

//----------------------------------------------------------------
//
// mods_PresetTelescope() - preset the telescope to a new target
//
// Implements the PresetTelescope() function (481s013s Section 8.31),
// sending the arguments given. rotPA is in radians, rotMode and
// opMode are as described in Section 8.27. Generally we use
//    rotMode = "POSITION"
//     opMode = {"STATIC","ACTIVE","GUIDE","TRACK","ACQUIRE"}.
//
// The target and guide star celestial coordinates must have been
// defined previously by mods_SetTarget() and mods_SetStars() calls.
//
// If an error occurs, return EXIT_FAILURE and set mods_error().
//
// Example: mods_PresetTelescope(0.0,"POSITION","GUIDE","left");
//
// See Also: mods_SetTarget(), mods_SetStars(), ...
//

int 
mods_PresetTelescope(double rotPA, char *rotMode, char *opMode, char *side)
{
  iif = factory->create(clientProxy_Name, focalStation_Name,instrument_Name) ;
  if (!iif)
    sprintf(iifmsgs[0],"Invalid proxy: IIF Factory failed to create a new instance.");

  res = iif->PresetTelescope(rotPA, (string)rotMode, (string)opMode, (string)side);
  showResults(res,"PresetTelescope");
  if (res.rescode != EXIT_SUCCESS) {
    if (res.rescode == EXIT_WARNING) {
      if (iifcount == 0)
	strcpy(iifmsgs[0],"WARNING: PresetTelescope returned warning");
      return EXIT_SUCCESS;
    }
    else {
      if (iifcount == 0)
	strcpy(iifmsgs[0],"Error: PresetTelescope Failed");
      return EXIT_FAILURE;
    }
  }
  return EXIT_SUCCESS;
}

//----------------------------------------------------------------
//
// mods_Remove() - remove target, offset, and hotspot coords
//
// Implements the Remove() function (481s013s Section 8.33) to remove
// target, offset, and hotspot coordinates.
//
// If an error occurs, return EXIT_FAILURE and set mods_error().
//
// Note: This was remove() - lowercase - in older versions of the IIF
//      but became Remove() - uppercase - later.
//
// See Also: mods_SetTarget(), mods_SetStars(), mods_PresetTelescope()
//

int 
mods_Remove()
{
  iif = factory->create(clientProxy_Name, focalStation_Name,instrument_Name);
  if (!iif)
    sprintf(iifmsgs[0],"Invalid proxy: IIF Factory failed to create a new instance.");

  iif->remove();  // was iif->Remove() in earlier builds [rwp/osu]
  showResults(res,"remove");
  if (res.rescode != EXIT_SUCCESS) {
    if (iifcount == 0)
      sprintf(iifmsgs[0],"[%d]Error: PresetTelescope Failed",
	      res.rescode);
    return EXIT_FAILURE;
  }
  strcpy(iifmsgs[0],"Removed Target,Hotspot,and Offset objects");
  return EXIT_SUCCESS;
}

//----------------------------------------------------------------
//
// mods_ResumeGuiding() - Resume paused autoguiding
//
// Implements the ResumeGuiding() function (481s013s Section 8.35).  A
// call to mods_ResumeGuiding() must follow mods_PauseGuiding().  If
// an error occurs, return EXIT_FAILURE and set mods_error().
//
// Example: mods_ResumeGuiding("left");
//
// See Also: mods_PauseGuiding()
//

int 
mods_ResumeGuiding(char *side)
{
  iif = factory->create(clientProxy_Name, focalStation_Name,instrument_Name) ;
  if (!iif)
    sprintf(iifmsgs[0],"Invalid proxy: IIF Factory failed to create a new instance.");

#ifdef __PAUSERESUME  
  res = iif->ResumeGuiding((string)side);
  showResults(res,"ResumeGuiding");
#endif
  if (res.rescode != EXIT_SUCCESS) {
    if (iifcount == 0)
      strcpy(iifmsgs[0],"Error: ResumeGuiding Failed ");
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

//----------------------------------------------------------------
//
// mods_RotReady() - enable/disable the instrument rotator
//
// Implements the RotReady() function (481s013s Section 8.40).  A call
// to mods_RotReady() must preceed and follow a call to the
// mod_RotServicePosition() function to enable and disable the rotator
// for this maintenance function which allows us to rotate the
// instrument to a service position even when it is not the authorized
// beamreceiving instrument.
//
// If an error occurs, return EXIT_FAILURE and set mods_error().
//
// Example: mods_RotReady(true,"left")
//
// See Also: mods_RotServicePosition(), mods_RotHold()
//

int 
mods_RotReady(bool enable, char *side)
{
  iif = factory->create(clientProxy_Name, focalStation_Name,instrument_Name) ;
  if (!iif)
    sprintf(iifmsgs[0],"Invalid proxy: IIF Factory failed to create a new instance.");

  res = iif->RotReady(enable, (string)side);
  showResults(res,"RotReady");
  if (res.rescode != EXIT_SUCCESS) {
    if (iifcount == 0)
      strcpy(iifmsgs[0],"Error: RotReady Failed");
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

//----------------------------------------------------------------
//
// mods_RotServicePosition() - raw instrument rotator move
//
// Implements the RotServicePosition() function (481s013s Section
// 8.41).  rotAngle is in degrees (note difference from PA given in
// radians).  This function must be preceeded by a mods_RotReady()
// call.
//
// If an error occurs, return EXIT_FAILURE and set mods_error().
//
// Example: mods_RotServicePosition(90.5,"left");
//
// See Also: mods_RotReady(), mods_RotHold()
//

int 
mods_RotServicePosition(double rotAngle, char *side)
{
  iif = factory->create(clientProxy_Name, focalStation_Name,instrument_Name) ;
  if (!iif)
    sprintf(iifmsgs[0],"Invalid proxy: IIF Factory failed to create a new instance.");
  res = iif->RotServicePosition(rotAngle, (string)side);
  showResults(res,"RotServicePosition");
  if (res.rescode != EXIT_SUCCESS) {
    if (iifcount == 0)
      strcpy(iifmsgs[0],"Error: RotServicePosition Failed");
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

//----------------------------------------------------------------
//
// mods_RotHold - stop a moving instrument rotator
//
// Implements the RotHold() function (481s013s Section 8.39) Argument
// is the side (left|right|both).  It stops the named rotator if it is
// moving, otherwise it has no effect.
//
// Implemented for completeness with the other RotXYZ functions.
//
// Example: mods_RotHold("left");
//
// See Also: mods_RotReady(), mods_RotServicePosition()
//

int
mods_RotHold (char *side)
{
  iif = factory->create(clientProxy_Name, focalStation_Name,instrument_Name) ;
  if (!iif)
    sprintf(iifmsgs[0],"Invalid proxy: IIF Factory failed to create a new instance.");
  res = iif->RotHold((string)side);
  showResults(res,"RotHold");
  if (res.rescode != EXIT_SUCCESS) {
    if (iifcount == 0)
      strcpy(iifmsgs[0],"Error: RotHold Failed");
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

//----------------------------------------------------------------
//
// mods_SetStars() - set target and guide star coordinates for
//                   the next preset
//
// Implements the SetStars() function (481s013s Section 8.55).
//
// RA and Dec are in radians, Epoch in decimal years, proper motion RA
// and proper motion Dec are in mas/year, gsMag is in decimal
// magnitude, lambda in decimal microns.
//
// We always set COORDSYS="RADEC" and Equinox to "J2000" internally.
// Normally, the proper motions are 0.0, but there may be instances
// where these are non-zero an important.
//
// If an error occurs, return EXIT_FAILURE and set mods_error().  A
// call to mods_SetTarget() or mods_SetStars() must preceed a
// mods_PresetTelescope() call.
//
// If no guide star is required, e.g., for a TRACK or STATIC mode
// preset, send gsName="" or gsName="none" (case insensitive) to
// disable the guide star parameter set.
//
// Note that the placeholder values for the color type and filter type
// fields must be "NONE" if no value is used.  We now default these to
// "NONE" since neither matters for MODS operations.  This corrects a
// problem in 2013 Sept (actually seen earlier in May 2013 but not
// reported to us by LBTO) in which the TCS build changed to no longer
// support " " as the no-value argument.  See 481s013s section 7.3.1
// and 7.3.2 for details.
//
// Example:
//  mods_SetStars("NGC1068",raRad,decRad,2000.0,0.0,0.0,
//  "GS123456+0012.5",raGSRad,decGSRad,2000.0,0.0,0.0,17.5,0.56)
//
// See Also: mods_PresetTelescope(), mods_SetTarget()
//

int 
mods_SetStars(char *targName, double targRA, double targDec, double targEpoch,
	      double targPMRA, double targPMDec, 
	      char *gsName, double gsRA, double gsDec, double gsEpoch, 
	      double gsPMRA, double gsPMDec, double gsMag, float lambda)
{
  iif = factory->create(clientProxy_Name, focalStation_Name, instrument_Name);
  SeqPos stars;

  // We always require a target

  position target = {targRA,targDec,"RADEC",targEpoch,"J2000",targPMRA,
		     targPMDec,0,"NONE",0,"NONE",lambda,targName};
  stars.push_back(target);

  // We don't send a guidestar position object to SetStars() unless we
  // have a guide star, as, for example, during a TRACK preset that
  // only requires target coordinates.  We tell this by looking at the
  // gsName string - if null or none, there is no guide star

  if (strlen(gsName)==0 || strcasecmp(gsName,"none")==0);
  else {
    position guidestar = {gsRA,gsDec,"RADEC",gsEpoch,"J2000",gsPMRA,gsPMDec,
			  gsMag,"NONE",0,"NONE",lambda,gsName};
    stars.push_back(guidestar);
  }

  res = iif->SetStars(stars);
  showResults(res,"SetStars");
  if (res.rescode != EXIT_SUCCESS) {
    if (iifcount == 0)
      strcpy(iifmsgs[0],"Error: SetStars Failed");
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

//----------------------------------------------------------------
//
// mods_UpdatePointingRefererence() - Set the (0,0) pointing reference
//                                    position for absolute offsets.
//
// Implements the UpdatePointingReference() function (481s013s Section
// 8.64).  
//
// This affects subsequent absolute MOVE_TYPE=ABS OffsetPointing
// requests, as it tells the LBT that the current position on this
// side is now (0,0) for absolute offsets.  In practice
// UpdatePointingReference is invoked after doing offsets to center a
// target in the MODS slit(s), so that any subsequent offset is
// relative to the in-slit position, not the position the preset left
// the telescope in after moving to the field and locking on the guide
// star.
//
// If an error occurs, return EXIT_FAILURE and set mods_error().
//
// Example: mods_UpdatePointingReference("RADEC","left")
//
// See Also: mods_PresetTelescope(), mods_SetStars()
//

int 
mods_UpdatePointingReference(char *coordSys, char *side)
{
  iif = factory->create(clientProxy_Name, focalStation_Name,instrument_Name) ;
  if (!iif)
    sprintf(iifmsgs[0],"Invalid proxy: IIF Factory failed to create a new instance.");

  res = iif->UpdatePointingReference((string)coordSys, (string)side);
  showResults(res,"UpdatePointingReference");
  if (res.rescode != EXIT_SUCCESS) {
    if (iifcount == 0)
      strcpy(iifmsgs[0],"Error: UpdatePointingReference Failed");
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

//----------------------------------------------------------------
//
// mods_UpdateTargetWavelength() - set the target wavelength
//
// Implements the UpdateTargetWavelength() function (481s013s Section
// 8.65).
//
// For MODS there are three typical wavelength values employed,
// depending on the mode:
//    Dichroic Mode: 0.56 um
//    Blue Direct Mode: 0.45 um
//    Red Direct Mode: 0.65 um
//
// If an error occurs, return EXIT_FAILURE and set mods_error().
//
// Example: mods_UpdateTargetWavelength(0.56,"left")
//
// See Also: mods_PresetTelescope(), mods_SetStars(), mods_SetTarget()
//

int 
mods_UpdateTargetWavelength(float lambda, char *side)
{
  iif = factory->create(clientProxy_Name, focalStation_Name,instrument_Name) ;
  if (!iif)
    sprintf(iifmsgs[0],"Invalid proxy: IIF Factory failed to create a new instance.");

  res = iif->UpdateTargetWavelength(lambda, (string)side);
  showResults(res,"UpdateTargetWavelength");
  if (res.rescode != EXIT_SUCCESS) {
    if (iifcount == 0)
      strcpy(iifmsgs[0],"Error: UpdateTargetWavelength");
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

//----------------------------------------------------------------
//
// mods_GetMODSData() - read MODS-specific data dictionary parameters
//
// Convenience function that implements the GetParameter() function
// (481s013s Section 8.13), to read only the MODS-specific variables
// from the data dictionary and load them into into the modsData
// struct (the first argument is a pointer to the modsData struct
// defined by the client program).
//
// This data structure is then used by the client program to extract
// the information it needs from the various data-dictionary
// entries. See mods_iifutils.h for a description of the lbtData
// struct.
//
// If an error occurs, return EXIT_FAILURE and set mods_error().
//
// Example: mods_GetMODSData(&modsSX,"left")
//

int 
mods_GetMODSData(modsData_t &modsdd, char *side)
{
  int len;
  string state_copy;
  char filename[80];

  iif = factory->create(clientProxy_Name, focalStation_Name, instrument_Name);

  if (!iif)
    sprintf(iifmsgs[0],"Invalid proxy: IIF Factory failed to create a new instance.");

  SeqDD ddentries;
  DDstruct dd;

  // Construct the filename, the path is defined at compile-time

  sprintf(filename,"%s/modsDD.%s",APP_CFGDIR,side);
  ifstream files_data (filename);

  for(int i=0;i<200 && !files_data.eof();i++) {
    getline(files_data, state_copy);
    len=strlen(&state_copy[0]);
    state_copy[len]='\0';
    if (state_copy[0]=='#');
    else {
      dd.DDname = (string)state_copy;
      ddentries.push_back(dd);
    }
  }
  files_data.close();

  res = iif->GetParameter(ddentries);
  showResults(res,"GetParameter");
  if (res.rescode != EXIT_SUCCESS) {
    if (iifcount == 0) 
      strcpy(iifmsgs[0],"Error: mods_GetMODSData Failed");
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

//----------------------------------------------------------------
//
// mods_GetTCSData() - read TCS data dictionary parameters
//
// Convenience function that implements the GetParameter() function
// (481s013s Section 8.13) to read a large block of TCS-specific
// data-dictionary entries in order to query the telescope status and
// load them into the lbtData struct (the first argument is a pointer
// to the lbtData struct defined by the client program).
//
// This data structure is then used by the client program to extract
// the information it needs from the various data-dictionary
// entries. See mods_iifutils.h for a description of the lbtData
// struct.
//
// If an error occurs, return EXIT_FAILURE and set mods_error().
//
// Example: mods_GetTCSData(&lbtSX,"left")
//

int 
mods_GetTCSData(lbtData_t &lbt, char *side)
{
  int len;
  string state_copy;
  char filename[256];

  int numNames;
  char ddName[200][100];

  iif = factory->create(clientProxy_Name, focalStation_Name, instrument_Name);

  if (!iif)
    sprintf(iifmsgs[0],"Invalid proxy: IIF Factory failed to create a new instance.");

  SeqDD ddentries;
  DDstruct dd;

  sprintf(filename,"%s/iifDD.%s",APP_CFGDIR,side);
  ifstream files_data (filename);

  numNames = 0;
  for (int i=0;i<200 && !files_data.eof();i++) {
    getline(files_data, state_copy);
    len=strlen(&state_copy[0]);
    if (len > 0) {
      state_copy[len]='\0';
      if (state_copy[0]=='#');
      else {
	dd.DDname = (string)state_copy;
	ddentries.push_back(dd);
	strcpy(ddName[numNames],&state_copy[0]);
	numNames++;
      }
    }
  }
  files_data.close();

  res = iif->GetParameter(ddentries);
  showResults(res,"GetParameter");
  if (res.rescode != EXIT_SUCCESS) {
    if (iifcount == 0) 
      strcpy(iifmsgs[0],"Error: mods_GetTCSData Failed");
    return EXIT_FAILURE;
  }

  // Process the DD entries we just read.  We still retain the ability
  // to read old-style fully-qualified DD names and new-style DD
  // Variable names (embedded DD aliases), the former just in case we
  // go back to fully qualifed DD names in the future.

  for (int i=0;i<numNames;i++) {
    // printf("  %s=%s\n",ddName[i],iifmsgs[i+1]);

    if (strcasecmp(ddName[i],"pcs.pointingStatus.time_UT.UTString")==0 ||
	strcasecmp(ddName[i],"UTC")==0)
      lbt.telUTC = (string)iifmsgs[i+1];
    else if (strcasecmp(ddName[i],"pcs.pointingStatus.time_SD.SDString")==0 ||
	     strcasecmp(ddName[i],"LST")==0)
      lbt.telLST = (string)iifmsgs[i+1];

    // Mount Control System (MCS) Azimuth, Elevation, and Rotator Encoder data
    // (note: rotator[5] = Direct Gregorian)

    else if (strcasecmp(ddName[i],"mcs.azDrive.position")==0 ||
	     strcasecmp(ddName[i],"AZPosition")==0)
      lbt.telAz = atof(iifmsgs[i+1]);
    else if (strcasecmp(ddName[i],"mcs.elDrive.position")==0 ||
	     strcasecmp(ddName[i],"ELPosition")==0)
      lbt.telEl = atof(iifmsgs[i+1]);
    else if (strcasecmp(ddName[i],"mcs.rotatorSide[0].rotators[5].actualPositionAsec")==0 ||
	     strcasecmp(ddName[i],"mcs.rotatorSide[1].rotators[5].actualPositionAsec")==0 ||
	     strcasecmp(ddName[i],"L_DGRPosition")==0 ||
	     strcasecmp(ddName[i],"R_DGRPosition")==0)
      lbt.telRot = atof(iifmsgs[i+1]);

    // LBT Telescope Temperature Sensor Web data

    else if (strcasecmp(ddName[i],"ecs.instrCool.tempChan201")==0 ||
	     strcasecmp(ddName[i],"Temp201")==0)
      lbt.ttemp201 = atof(iifmsgs[i+1]);
    else if (strcasecmp(ddName[i],"ecs.instrCool.tempChan202")==0 ||
	     strcasecmp(ddName[i],"Temp202")==0)
      lbt.ttemp202 = atof(iifmsgs[i+1]);
    else if (strcasecmp(ddName[i],"ecs.instrCool.tempChan203")==0 ||
	     strcasecmp(ddName[i],"Temp203")==0)
      lbt.ttemp203 = atof(iifmsgs[i+1]);
    else if (strcasecmp(ddName[i],"ecs.instrCool.tempChan204")==0 ||
	     strcasecmp(ddName[i],"Temp204")==0)
      lbt.ttemp204 = atof(iifmsgs[i+1]);
    else if (strcasecmp(ddName[i],"ecs.instrCool.tempChan205")==0 ||
	     strcasecmp(ddName[i],"Temp205")==0)
      lbt.ttemp205 = atof(iifmsgs[i+1]);
    else if (strcasecmp(ddName[i],"ecs.instrCool.tempChan206")==0 ||
	     strcasecmp(ddName[i],"Temp206")==0)
      lbt.ttemp206 = atof(iifmsgs[i+1]);
    else if (strcasecmp(ddName[i],"ecs.instrCool.tempChan207")==0 ||
	     strcasecmp(ddName[i],"Temp207")==0)
      lbt.ttemp207 = atof(iifmsgs[i+1]);
    else if (strcasecmp(ddName[i],"ecs.instrCool.tempChan208")==0 ||
	     strcasecmp(ddName[i],"Temp208")==0)
      lbt.ttemp208 = atof(iifmsgs[i+1]);
    else if (strcasecmp(ddName[i],"ecs.instrCool.tempChan209")==0 ||
	     strcasecmp(ddName[i],"Temp209")==0)
      lbt.ttemp209 = atof(iifmsgs[i+1]);
    else if (strcasecmp(ddName[i],"ecs.instrCool.tempChan210")==0 ||
	     strcasecmp(ddName[i],"Temp210")==0)
      lbt.ttemp210 = atof(iifmsgs[i+1]);
    else if (strcasecmp(ddName[i],"ecs.instrCool.tempChan301")==0 ||
	     strcasecmp(ddName[i],"Temp301")==0)
      lbt.ttemp301 = atof(iifmsgs[i+1]);
    else if (strcasecmp(ddName[i],"ecs.instrCool.tempChan302")==0 ||
	     strcasecmp(ddName[i],"Temp302")==0)
      lbt.ttemp302 = atof(iifmsgs[i+1]);
    else if (strcasecmp(ddName[i],"ecs.instrCool.tempChan303")==0 ||
	     strcasecmp(ddName[i],"Temp303")==0)
      lbt.ttemp303 = atof(iifmsgs[i+1]);
    else if (strcasecmp(ddName[i],"ecs.instrCool.tempChan304")==0 ||
	     strcasecmp(ddName[i],"Temp304")==0)
      lbt.ttemp304 = atof(iifmsgs[i+1]);
    else if (strcasecmp(ddName[i],"ecs.instrCool.tempChan305")==0 ||
	     strcasecmp(ddName[i],"Temp305")==0)
      lbt.ttemp305 = atof(iifmsgs[i+1]);
    else if (strcasecmp(ddName[i],"ecs.instrCool.tempChan306")==0 ||
	     strcasecmp(ddName[i],"Temp306")==0)
      lbt.ttemp306 = atof(iifmsgs[i+1]);
    else if (strcasecmp(ddName[i],"ecs.instrCool.tempChan307")==0 ||
	     strcasecmp(ddName[i],"Temp307")==0)
      lbt.ttemp307 = atof(iifmsgs[i+1]);
    else if (strcasecmp(ddName[i],"ecs.instrCool.tempChan308")==0 ||
	     strcasecmp(ddName[i],"Temp308")==0)
      lbt.ttemp308 = atof(iifmsgs[i+1]);
    else if (strcasecmp(ddName[i],"ecs.instrCool.tempChan309")==0 ||
	     strcasecmp(ddName[i],"Temp309")==0)
      lbt.ttemp309 = atof(iifmsgs[i+1]);
    else if (strcasecmp(ddName[i],"ecs.instrCool.tempChan310")==0 ||
	     strcasecmp(ddName[i],"Temp310")==0)
      lbt.ttemp310 = atof(iifmsgs[i+1]);

    // LBT Weather Station data

    else if (strcasecmp(ddName[i],"env.weather.lbt.alive")==0 ||
	     strcasecmp(ddName[i],"LBTWeatherAlive")==0)
      lbt.lbtLink = atoi(iifmsgs[i+1]);
    else if (strcasecmp(ddName[i],"env.weather.lbt.pressure")==0 ||
	     strcasecmp(ddName[i],"LBTPressure")==0)
      lbt.lbtPres = atof(iifmsgs[i+1]);
    else if (strcasecmp(ddName[i],"env.weather.lbt.temperature")==0 ||
	     strcasecmp(ddName[i],"LBTTemp")==0)
      lbt.lbtTemp = atof(iifmsgs[i+1]);
    else if (strcasecmp(ddName[i],"env.weather.lbt.humidity")==0 ||
	     strcasecmp(ddName[i],"LBTHumidity")==0)
      lbt.lbtHum = atof(iifmsgs[i+1]);
    else if (strcasecmp(ddName[i],"env.weather.lbt.dewPoint")==0 ||
	     strcasecmp(ddName[i],"LBTDewPoint")==0)
      lbt.lbtDwpt = atof(iifmsgs[i+1]);

    // Instrument Authorization Status

    else if (strcasecmp(ddName[i],"iif.side[0].authorizedInstrument")==0 ||
	     strcasecmp(ddName[i],"iif.side[1].authorizedInstrument")==0 ||
	     strcasecmp(ddName[i],"L_Instrument")==0 ||
	     strcasecmp(ddName[i],"R_Instrument")==0)
	lbt.authInst = (string)iifmsgs[i+1];

    // Pointing Control System (PCS) data

    else if (strcasecmp(ddName[i],"pcs.side[0].pointingStatus.rotatorMode")==0 ||
	     strcasecmp(ddName[i],"pcs.side[1].pointingStatus.rotatorMode")==0 ||
	     strcasecmp(ddName[i],"L_RotMode")==0 || 
	     strcasecmp(ddName[i],"R_RotMode")==0) 
	lbt.rotMode = (string)iifmsgs[i+1];
    else if (strcasecmp(ddName[i],"pcs.side[0].pointingStatus.rotatorAngle")==0 ||
	     strcasecmp(ddName[i],"pcs.side[1].pointingStatus.rotatorAngle")==0 ||
	     strcasecmp(ddName[i],"L_RotAngle")==0 || 
	     strcasecmp(ddName[i],"R_RotAngle")==0)
      lbt.rotAngle = atof(iifmsgs[i+1]);
    else if (strcasecmp(ddName[i],"pcs.side[0].pointingStatus.positionAngle")==0 ||
	     strcasecmp(ddName[i],"pcs.side[1].pointingStatus.positionAngle")==0 ||
	     strcasecmp(ddName[i],"L_PosAngle")==0 || 
	     strcasecmp(ddName[i],"R_PosAngle")==0)
      lbt.posAngle = atof(iifmsgs[i+1]);
    else if (strcasecmp(ddName[i],"pcs.pointingStatus.achieved.parallacticAngle")==0 ||
	     strcasecmp(ddName[i],"ParAngle")==0)
      lbt.parAngle = atof(iifmsgs[i+1]);
    else if (strcasecmp(ddName[i],"pcs.side[0].pointingStatus.rotator.rotCenter_RA.Radians")==0 ||
	     strcasecmp(ddName[i],"pcs.side[1].pointingStatus.rotator.rotCenter_RA.Radians")==0 ||
	     strcasecmp(ddName[i],"L_RotCenterRA")==0 || 
	     strcasecmp(ddName[i],"R_RotCenterRA")==0)
      lbt.telRA = atof(iifmsgs[i+1]);
    else if (strcasecmp(ddName[i],"pcs.side[0].pointingStatus.achieved.achieved_RA.Radians")==0 ||
	     strcasecmp(ddName[i],"pcs.side[1].pointingStatus.achieved.achieved_RA.Radians")==0 ||
	     strcasecmp(ddName[i],"L_AchievedRA")==0 || 
	     strcasecmp(ddName[i],"R_AchievedRA")==0)
      lbt.telRA = atof(iifmsgs[i+1]);
    else if (strcasecmp(ddName[i],"pcs.side[0].pointingStatus.rotator.rotCenter_DEC.Radians")==0 ||
	     strcasecmp(ddName[i],"pcs.side[1].pointingStatus.rotator.rotCenter_DEC.Radians")==0 ||
	     strcasecmp(ddName[i],"L_RotCenterDEC")==0 || 
	     strcasecmp(ddName[i],"R_RotCenterDEC")==0)
      lbt.telDec = atof(iifmsgs[i+1]);
    else if (strcasecmp(ddName[i],"pcs.side[0].pointingStatus.achieved.achieved_DEC.Radians")==0 ||
	     strcasecmp(ddName[i],"pcs.side[1].pointingStatus.achieved.achieved_DEC.Radians")==0 ||
	     strcasecmp(ddName[i],"L_AchievedDEC")==0 || 
	     strcasecmp(ddName[i],"R_AchievedDEC")==0)
      lbt.telDec = atof(iifmsgs[i+1]);

    // GCS Guide Star Data (mostly engineering)

    else if (strcasecmp(ddName[i],"gcs.side[0].GuideCam.GS_RA")==0 ||
	     strcasecmp(ddName[i],"gcs.side[1].GuideCam.GS_RA")==0 ||
	     strcasecmp(ddName[i],"L_GuideRA")==0 || 
	     strcasecmp(ddName[i],"R_GuideRA")==0)
      lbt.guiRA = atof(iifmsgs[i+1]);
    else if (strcasecmp(ddName[i],"gcs.side[0].GuideCam.GS_DEC")==0 ||
	     strcasecmp(ddName[i],"gcs.side[1].GuideCam.GS_DEC")==0 ||
	     strcasecmp(ddName[i],"L_GuideDEC")==0 || 
	     strcasecmp(ddName[i],"R_GuideDEC")==0)
      lbt.guiDec = atof(iifmsgs[i+1]);
    else if (strcasecmp(ddName[i],"gcs.side[0].AGW.SFPrequested_X")==0 ||
	     strcasecmp(ddName[i],"gcs.side[1].AGW.SFPrequested_X")==0 ||
	     strcasecmp(ddName[i],"L_AGWKFPReqX")==0 || 
	     strcasecmp(ddName[i],"R_AGWKFPReqX")==0)
      lbt.reqPCSX = atof(iifmsgs[i+1]);
    else if (strcasecmp(ddName[i],"gcs.side[0].AGW.SFPrequested_Y")==0 ||
	     strcasecmp(ddName[i],"gcs.side[1].AGW.SFPrequested_Y")==0 ||
	     strcasecmp(ddName[i],"L_AGWKFPReqY")==0 || 
	     strcasecmp(ddName[i],"R_AGWKFPReqY")==0)
      lbt.reqPCSY = atof(iifmsgs[i+1]);
    else if (strcasecmp(ddName[i],"gcs.side[0].AGW.SFPachieved_X")==0 ||
	     strcasecmp(ddName[i],"gcs.side[1].AGW.SFPachieved_X")==0 ||
	     strcasecmp(ddName[i],"L_AGWKFPPosX")==0 || 
	     strcasecmp(ddName[i],"R_AGWKFPPosX")==0)
      lbt.actPCSX = atof(iifmsgs[i+1]);
    else if (strcasecmp(ddName[i],"gcs.side[0].AGW.SFPachieved_Y")==0 ||
	     strcasecmp(ddName[i],"gcs.side[1].AGW.SFPachieved_Y")==0 ||
	     strcasecmp(ddName[i],"L_AGWKFPPosY")==0 || 
	     strcasecmp(ddName[i],"R_AGWKFPPosY")==0)
      lbt.actPCSY = atof(iifmsgs[i+1]);
    else if (strcasecmp(ddName[i],"pcs.side[0].pointingStatus.guider.predicted_X")==0 ||
	     strcasecmp(ddName[i],"pcs.side[1].pointingStatus.guider.predicted_X")==0 ||
	     strcasecmp(ddName[i],"L_GSXPredicted")==0 || 
	     strcasecmp(ddName[i],"R_GSXPredicted")==0)
      lbt.gsPredX = atof(iifmsgs[i+1]);
    else if (strcasecmp(ddName[i],"pcs.side[0].pointingStatus.guider.predicted_Y")==0 ||
	     strcasecmp(ddName[i],"pcs.side[1].pointingStatus.guider.predicted_Y")==0 ||
	     strcasecmp(ddName[i],"L_GSYPredicted")==0 || 
	     strcasecmp(ddName[i],"R_GSYPredicted")==0)
      lbt.gsPredY = atof(iifmsgs[i+1]);
    else if (strcasecmp(ddName[i],"pcs.side[0].pointingStatus.guider.measured_X")==0 ||
	     strcasecmp(ddName[i],"pcs.side[1].pointingStatus.guider.measured_X")==0 ||
	     strcasecmp(ddName[i],"L_GSXMeasured")==0 || 
	     strcasecmp(ddName[i],"R_GSXMeasured")==0)
      lbt.gsMeasX = atof(iifmsgs[i+1]);
    else if (strcasecmp(ddName[i],"pcs.side[0].pointingStatus.guider.measured_Y")==0 ||
	     strcasecmp(ddName[i],"pcs.side[1].pointingStatus.guider.measured_Y")==0 ||
	     strcasecmp(ddName[i],"L_GSYMeasured")==0 || 
	     strcasecmp(ddName[i],"R_GSYMeasured")==0)
      lbt.gsMeasY = atof(iifmsgs[i+1]);
    else if (strcasecmp(ddName[i],"pcs.side[0].pointingStatus.guider.delta_X")==0 ||
	     strcasecmp(ddName[i],"pcs.side[1].pointingStatus.guider.delta_X")==0 ||
	     strcasecmp(ddName[i],"L_GSCumXPredmMeas")==0 || 
	     strcasecmp(ddName[i],"R_GSCumXPredmMeas")==0)
      lbt.gsDeltaX = atof(iifmsgs[i+1]);
    else if (strcasecmp(ddName[i],"pcs.side[0].pointingStatus.guider.delta_Y")==0 ||
	     strcasecmp(ddName[i],"pcs.side[1].pointingStatus.guider.delta_Y")==0 ||
	     strcasecmp(ddName[i],"L_GSCumYPredmMeas")==0 || 
	     strcasecmp(ddName[i],"R_GSCumYPredmMeas")==0)
      lbt.gsDeltaY = atof(iifmsgs[i+1]);

    // Primary Mirror Data

    else if (strcasecmp(ddName[i],"psf.side[0].primaryMirror.collimation.collimation[0]")==0 ||
	     strcasecmp(ddName[i],"psf.side[1].primaryMirror.collimation.collimation[0]")==0 ||
	     strcasecmp(ddName[i],"L_M1CollX")==0 || 
	     strcasecmp(ddName[i],"R_M1CollX")==0)
      lbt.m1PosX = atof(iifmsgs[i+1]);
    else if (strcasecmp(ddName[i],"psf.side[0].primaryMirror.collimation.collimation[1]")==0 ||
	     strcasecmp(ddName[i],"psf.side[1].primaryMirror.collimation.collimation[1]")==0 ||
	     strcasecmp(ddName[i],"L_M1CollY")==0 || 
	     strcasecmp(ddName[i],"R_M1CollY")==0)
      lbt.m1PosY = atof(iifmsgs[i+1]);
    else if (strcasecmp(ddName[i],"psf.side[0].primaryMirror.collimation.collimation[2]")==0 ||
	     strcasecmp(ddName[i],"psf.side[1].primaryMirror.collimation.collimation[2]")==0 ||
	     strcasecmp(ddName[i],"L_M1CollZ")==0 || 
	     strcasecmp(ddName[i],"R_M1CollZ")==0)
      lbt.m1PosZ = atof(iifmsgs[i+1]);
    else if (strcasecmp(ddName[i],"psf.side[0].primaryMirror.collimation.collimation[3]")==0 ||
	     strcasecmp(ddName[i],"psf.side[1].primaryMirror.collimation.collimation[3]")==0 ||
	     strcasecmp(ddName[i],"L_M1CollRx")==0 || 
	     strcasecmp(ddName[i],"R_M1CollRx")==0)
      lbt.m1RotX = atof(iifmsgs[i+1]);
    else if (strcasecmp(ddName[i],"psf.side[0].primaryMirror.collimation.collimation[4]")==0 ||
	     strcasecmp(ddName[i],"psf.side[1].primaryMirror.collimation.collimation[4]")==0 ||
	     strcasecmp(ddName[i],"L_M1CollRy")==0 || 
	     strcasecmp(ddName[i],"R_M1CollRy")==0)
      lbt.m1RotY = atof(iifmsgs[i+1]);
    else if (strcasecmp(ddName[i],"psf.side[0].primaryMirror.collimation.collimation[5]")==0 ||
	     strcasecmp(ddName[i],"psf.side[1].primaryMirror.collimation.collimation[5]")==0 ||
	     strcasecmp(ddName[i],"L_M1CollRz")==0 || 
	     strcasecmp(ddName[i],"R_M1CollRz")==0)
      lbt.m1RotZ = atof(iifmsgs[i+1]);
    else if (strcasecmp(ddName[i],"psf.side[0].primaryMirror.collimation.temperature")==0 ||
	     strcasecmp(ddName[i],"psf.side[1].primaryMirror.collimation.temperature")==0 ||
	     strcasecmp(ddName[i],"L_M1CollTemp")==0 || 
	     strcasecmp(ddName[i],"R_M1CollTemp")==0)
      lbt.m1CTemp = atof(iifmsgs[i+1]);
    else if (strcasecmp(ddName[i],"pmc.side[0].thermalRpt.ambientThermAvg")==0 ||
	     strcasecmp(ddName[i],"pmc.side[1].thermalRpt.ambientThermAvg")==0 ||
	     strcasecmp(ddName[i],"L_M1AirTemp")==0 || 
	     strcasecmp(ddName[i],"R_M1AirTemp")==0)
      lbt.m1ATemp = atof(iifmsgs[i+1]);

    // Secondary Mirror Data

    else if (strcasecmp(ddName[i],"psf.side[0].secondaryMirror.collimation.collimation[0]")==0 ||
	     strcasecmp(ddName[i],"psf.side[1].secondaryMirror.collimation.collimation[0]")==0 ||
	     strcasecmp(ddName[i],"L_M2CollX")==0 || 
	     strcasecmp(ddName[i],"R_M2CollX")==0)
      lbt.m2PosX = atof(iifmsgs[i+1]);
    else if (strcasecmp(ddName[i],"psf.side[0].secondaryMirror.collimation.collimation[1]")==0 ||
	     strcasecmp(ddName[i],"psf.side[1].secondaryMirror.collimation.collimation[1]")==0 ||
	     strcasecmp(ddName[i],"L_M2CollY")==0 || 
	     strcasecmp(ddName[i],"R_M2CollY")==0)
      lbt.m2PosY = atof(iifmsgs[i+1]);
    else if (strcasecmp(ddName[i],"psf.side[0].secondaryMirror.collimation.collimation[2]")==0 ||
	     strcasecmp(ddName[i],"psf.side[1].secondaryMirror.collimation.collimation[2]")==0 ||
	     strcasecmp(ddName[i],"L_M2CollZ")==0 || 
	     strcasecmp(ddName[i],"R_M2CollZ")==0)
      lbt.m2PosZ = atof(iifmsgs[i+1]);
    else if (strcasecmp(ddName[i],"psf.side[0].secondaryMirror.collimation.collimation[3]")==0 ||
	     strcasecmp(ddName[i],"psf.side[1].secondaryMirror.collimation.collimation[3]")==0 ||
	     strcasecmp(ddName[i],"L_M2CollRx")==0 || 
	     strcasecmp(ddName[i],"R_M2CollRx")==0)
      lbt.m2RotX = atof(iifmsgs[i+1]);
    else if (strcasecmp(ddName[i],"psf.side[0].secondaryMirror.collimation.collimation[4]")==0 ||
	     strcasecmp(ddName[i],"psf.side[1].secondaryMirror.collimation.collimation[4]")==0 ||
	     strcasecmp(ddName[i],"L_M2CollRy")==0 || 
	     strcasecmp(ddName[i],"R_M2CollRy")==0)
      lbt.m2RotY = atof(iifmsgs[i+1]);
    else if (strcasecmp(ddName[i],"psf.side[0].secondaryMirror.collimation.collimation[5]")==0 ||
	     strcasecmp(ddName[i],"psf.side[1].secondaryMirror.collimation.collimation[5]")==0 ||
	     strcasecmp(ddName[i],"L_M2CollRz")==0 || 
	     strcasecmp(ddName[i],"R_M2CollRz")==0)
      lbt.m2RotZ = atof(iifmsgs[i+1]);
    else if (strcasecmp(ddName[i],"psf.side[0].secondaryMirror.collimation.temperature")==0 ||
	     strcasecmp(ddName[i],"psf.side[1].secondaryMirror.collimation.temperature")==0 ||
	     strcasecmp(ddName[i],"L_M2CollTemp")==0 || 
	     strcasecmp(ddName[i],"R_M2CollTemp")==0)
      lbt.m2CTemp = atof(iifmsgs[i+1]);

  }
  
  // All done...
    
  return EXIT_SUCCESS;
}

//----------------------------------------------------------------
//
// mods_SetMODSData - write MODS-specific data dictionary parameters
//
// Implements the SetParameter() function (481s013s Section 8.51),
// sending relevant MODS parameters contained in the modsPar struct.
//
// During lab testing, the modsPar struct contains the standard dummy
// int, float, and string test parameters.
//
// This convenience function writes only the MODS-specific Data
// Dictionary parameters.
//
// If an error occurs, return EXIT_FAILURE and set mods_error().
//
// Example: mods_SetMODSPars(&lbtSX,"left")
//

int 
mods_SetMODSData(modsData_t &modsdd, char *side)
{
  int len;
  string state_copy;
  char filename[80];
  string modsdata_file;
  string cside;
  string temp;
  SeqDD ddentries;
  DDstruct dd;

  iif = factory->create(clientProxy_Name, focalStation_Name, instrument_Name);

  if (!iif)
    sprintf(iifmsgs[0],"Invalid proxy: IIF Factory failed to create a new instance.");

  if (!strcasecmp(side,"RIGHT")) cside="1";
  else if (!strcasecmp(side,"LEFT")) cside="0";
  
  /*
  dd.DDname = "side["+ cside +"].modsName";
  dd.DDkey = modsdd.modsName;
  dd.DDkey = "MODS1";
  ddentries.push_back(dd);
  dd.DDname = "side[" + cside + "].powerState";
  dd.DDkey = modsdd.powerState;
  dd.DDkey = "ENABLE";
  ddentries.push_back(dd);

  dd.DDname = "side[" + cside +"].hatch";
  dd.DDkey = modsdd.hatch;
  dd.DDkey = "CLOSED";
  ddentries.push_back(dd);

  dd.DDname = "side[" + cside + "0].imcsLaser";
  dd.DDkey = modsdd.imcsLaser;
  ddentries.push_back(dd);
  
  dd.DDname = "side[" + cside + "].glycolSupplyPressure";
  sprintf(&temp[0],"%f",modsdd.glycolSupplyPressure);
  dd.DDkey = temp;
  ddentries.push_back(dd);

  dd.DDname = "side[" + cside + "].glycolSupplyTemp";
  sprintf(&temp[0],"%f",modsdd.glycolSupplyTemp);
  dd.DDkey = temp;
  ddentries.push_back(dd);

  dd.DDname = "side[" + cside + "].glycolReturnPressure";
  sprintf(&temp[0],"%f",modsdd.glycolReturnPressure);
  dd.DDkey = temp;
  ddentries.push_back(dd);

  dd.DDname = "side[" + cside + "].glycolReturnTemp";
  sprintf(&temp[0],"%f",modsdd.glycolReturnTemp);
  dd.DDkey = temp;
  ddentries.push_back(dd);

  dd.DDname = "side[" + cside + "].blueDewarPressure";
  sprintf(&temp[0],"%f",modsdd.blueDewarPressure);
  dd.DDkey = temp;
  ddentries.push_back(dd);

  dd.DDname = "side[" + cside + "].blueDewarTemp";
  sprintf(&temp[0],"%f",modsdd.blueDewarTemp);
  dd.DDkey = temp;
  ddentries.push_back(dd);

  dd.DDname = "side[" + cside + "].blueCCDTemp";
  sprintf(&temp[0],"%f",modsdd.blueCCDTemp);
  dd.DDkey = temp;
  ddentries.push_back(dd);

  dd.DDname = "side[" + cside + "].redDewarPressure";
  sprintf(&temp[0],"%f",modsdd.redDewarPressure);
  dd.DDkey = temp;
  ddentries.push_back(dd);

  dd.DDname = "side[" + cside + "].redDewarTemp";
  sprintf(&temp[0],"%f",modsdd.redDewarTemp);
  dd.DDkey = temp;
  ddentries.push_back(dd);

  dd.DDname = "side[" + cside + "].redCCDTemp";
  sprintf(&temp[0],"%f",modsdd.redCCDTemp);
  dd.DDkey = temp;
  ddentries.push_back(dd);

  dd.DDname = "side[" + cside + "].blueIEBTemp";
  sprintf(&temp[0],"%f",modsdd.blueIEBTemp);
  dd.DDkey = temp;
  ddentries.push_back(dd);

  dd.DDname = "side[" + cside + "].redIEBTemp";
  sprintf(&temp[0],"%f",modsdd.redIEBTemp);
  dd.DDkey = temp;
  ddentries.push_back(dd);

  dd.DDname = "side[" + cside + "].blueHEBTemp";
  sprintf(&temp[0],"%f",modsdd.blueHEBTemp);
  dd.DDkey = temp;
  ddentries.push_back(dd);

  dd.DDname = "side[" + cside + "].redHEBTemp";
  sprintf(&temp[0],"%f",modsdd.redHEBTemp);
  dd.DDkey = temp;
  ddentries.push_back(dd);

  dd.DDname = "side[" + cside + "].utilBoxTemp";
  sprintf(&temp[0],"%f",modsdd.utilBoxTemp);
  dd.DDkey = temp;
  ddentries.push_back(dd);
  */
  res = iif->SetParameter(ddentries);
  showResults(res,"SetParameter");
  if (res.rescode != EXIT_SUCCESS) {
    if (iifcount == 0) 
      strcpy(iifmsgs[0],"Error: mods_SetParameter Failed");
    return EXIT_FAILURE;
  }
  strcpy(iifmsgs[0],"mods_SetMODSData _NOT_ Available");
  return EXIT_SUCCESS;
}

//----------------------------------------------------------------
//
// mods_SetParameter() - set a MODS-specific data-dictionary value
//
// Implements the SetParameter() function (481s013s Section 8.51),
// to set a single MODS-specific data-dictionary parameter.
//
// If an error occurs, return EXIT_FAILURE and set mods_error().
//
// Example: mods_SetParameter(&mods1Par,"left");
//
// See Also: mods_SetMODSData(), mods_GetMODSData()
//

int 
mods_SetParameter(char *DDName, char *DDKey)
{
  iif = factory->create(clientProxy_Name, focalStation_Name, instrument_Name);

  if (!iif)
    sprintf(iifmsgs[0],"Invalid proxy: IIF Factory failed to create a new instance.");

  SeqDD ddentries;
  DDstruct dd;

  dd.DDname = DDName;
  dd.DDkey = DDKey;
  ddentries.push_back(dd);

  res = iif->SetParameter(ddentries);
  showResults(res,"SetParameter");
  if (res.rescode != EXIT_SUCCESS) {
    if (iifcount == 0)
      strcpy(iifmsgs[0],"Error: mods_SetParameter Failed");
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

//----------------------------------------------------------------
//
// mods_GetParameter() - get a single data-dictionary parameter
//
// Implements the GetParameter() function (481s013s Section
// 8.13), to retrieve a single data dictionary entry by name.
//
// Example: mods_GetParameter("gcs.side[0].isCollimated,foo);
//
// See Also: mods_GetMODSData(), mods_GetTCSData()
//

int 
mods_GetParameter(char *DDName,char *DDkey)
{

  iif = factory->create(clientProxy_Name, focalStation_Name, instrument_Name);

  if (!iif)
    sprintf(iifmsgs[0],"Invalid proxy: IIF Factory failed to create a new instance.");

  SeqDD ddentries;
  DDstruct dd;

  dd.DDname = (string)DDName;
  ddentries.push_back(dd);

  res = iif->GetParameter(ddentries);
  showResults(res,"GetParameter");
  if (res.rescode != EXIT_SUCCESS) {
    if (iifcount == 0) 
      strcpy(iifmsgs[0],"Error: mods_GetParameter Failed");
    return EXIT_FAILURE;
  }
  strcpy(DDkey,iifmsgs[1]); // returned value is in iifmsgs[1].
  return EXIT_SUCCESS;
}

//----------------------------------------------------------------
//
// mods_BinocularControl() - send a binocular control command
//
// Implements the BinocularControl() function (481s013s Section 8.3),
// sending the binoCmd string given.
//
// If an error occurs, return EXIT_FAILURE, the reason can be
// retrieved using mods_error().
//
// Example: mods_BinocularControl("SYNCPRESET");
//

int 
mods_BinocularControl(char *binoCmd)
{
  iif = factory->create(clientProxy_Name, focalStation_Name,instrument_Name) ;
  if (!iif)
    sprintf(iifmsgs[0],"Invalid proxy: IIF Factory failed to create a new instance.");
    
  res = iif->BinocularControl((string)binoCmd);
  showResults(res,"BinocularControl");
  if (res.rescode != EXIT_SUCCESS) {
    if (iifcount == 0)
      strcpy(iifmsgs[0],"Error: BinocularControl Command Failed");
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

//----------------------------------------------------------------
//
// mods_ClearNonSidereal() - remove non-sidereal objects set by
//                           a previous mods_SetNonSidereal() call
//
// Implements the ClearNonSidereal() function (481s013s Section 8.6),
// removing any non-sidereal objects set by a previous
// mods_SetNonSidereal() function call.
//
// If an error occurs, return EXIT_FAILURE and set mods_error().
//
// Note that the boolean false argument is hardwired, since any
// non-false argument is reserved for the IRTC and irrelevant for
// MODS.
//
// Example: mods_ClearNonSidereal();
//

int 
mods_ClearNonSidereal()
{
  iif = factory->create(clientProxy_Name, focalStation_Name,instrument_Name) ;
  if (!iif)
    sprintf(iifmsgs[0],"Invalid proxy: IIF Factory failed to create a new instance.");

  res = iif->ClearNonSidereal(false);
  showResults(res,"ClearNonSidereal");
  if (res.rescode != EXIT_SUCCESS) {
    if (iifcount == 0)
      strcpy(iifmsgs[0],"Error: ClearNonSidereal Failed");
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

//----------------------------------------------------------------
//
// mods_SetNonSidereal() - set non-sidereal target parameters
//
// Implements the SetNonSidereal() function (481s013s Section 8.48).
//
// This function sets the non-sidereal target information,
// particularly the target type, any JPL Horizons database ephemeris
// file to use, and the differential tracking rate information.
//
// Builds the nonsidereal data structure using these arguments:
//   nsType (string) - non-sideral object type = name,"FILE","DIFFERENTIAL"
//   effWave (float) - target effective wavelength in microns
//   fileName (string) - file name for nsType=FILE or "" if no file
//   dMJD (double) - differential MJD in UT days
//   dRA (double) - differential RA in radians
//   dDec (double) - differential Dec in radians
//   raRate (double) - differential RA track rate, dRA/dt*cos(DEC) in radians/day
//   decRate (double) - differential Dec track rate, dDec/dt in radians/day
//
// See 481s013s Section 7.5 for details of the nonsidereal data
// structure.
//
// Example:
//
// See Also: mods_ClearNonSidereal(), mods_SetStars(), 
//           mods_UpdateNonSidereal()
//

int 
mods_SetNonSidereal(char *nsType, float effWave, char *fileName,
		    double dMJD, double dRA, double dDec,
		    double raRate, double decRate)
{
  iif = factory->create(clientProxy_Name, focalStation_Name, instrument_Name);

  nonsidereal ns = {nsType,effWave,fileName,dMJD,dRA,dDec,raRate,decRate};

  res = iif->SetNonSidereal(ns,false);
  showResults(res,"SetNonSidereal");
  if (res.rescode != EXIT_SUCCESS) {
    if (iifcount == 0)
      strcpy(iifmsgs[0],"Error: SetNonSidereal Failed");
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

//----------------------------------------------------------------
//
// mods_UpdateNonSidereal() - modify the non-sidereal tracking rate
//
// Implements the UpdateNonSidereal() function (481s013s Section 8.63),
// allowing the user to change the current non-sidereal tracking rates
// on the fly.
//
// Arguments:
//   raRate (double): RA differential tracking rate in radians/day
//  decRate (double): Dec differential tracking rate in radians/day
//
// If an error occurs, return EXIT_FAILURE and set mods_error().
//
// Example: mods_UpdateNonSidereal(12.123,-10.002);
//

int 
mods_UpdateNonSidereal(double raRate, double decRate)
{
  iif = factory->create(clientProxy_Name, focalStation_Name,instrument_Name) ;
  if (!iif)
    sprintf(iifmsgs[0],"Invalid proxy: IIF Factory failed to create a new instance.");

  res = iif->UpdateNonSiderealTarget(raRate,decRate);
  showResults(res,"UpdateNonSidereal");
  if (res.rescode != EXIT_SUCCESS) {
    if (iifcount == 0)
      strcpy(iifmsgs[0],"Error: UpdateNonSidereal Failed");
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

//----------------------------------------------------------------
//
// mods_SetAGWFilter() - change the MODS AGw unit guider filter
//
// Implements the SetAGWFilter() function (481s013s Section 8.45),
// allowing us to change the AGw unit guide camera filter through
// the IIF instead of directly with the MODS instrument server.
//
// Arguments:
//   agwFilt (int): AGw filter number, 1..4
//   side (char *): telescope side = {left,right,both}
//
// If an error occurs, return EXIT_FAILURE and set mods_error().
//
// Example: mods_SetAGWFilter(1);
//

int 
mods_SetAGWFilter(int agwFilt, char* side)
{
  iif = factory->create(clientProxy_Name, focalStation_Name,instrument_Name) ;
  if (!iif)
    sprintf(iifmsgs[0],"Invalid proxy: IIF Factory failed to create a new instance.");

  res = iif->SetAGWFilter(agwFilt, (string)side);
  showResults(res,"SetAGWFilter");
  if (res.rescode != EXIT_SUCCESS) {
    if (iifcount == 0)
      strcpy(iifmsgs[0],"Error: SetAGWFilter Failed");
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

