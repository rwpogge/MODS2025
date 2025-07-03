#ifndef __modsIIFUtils_h__
#define __modsIIFUtils_h__

//
// mods_iifutils.h - MODS LBT DD parameter struct
//

/*!
  \file mods_iifutils.h
  \brief Place holder for Data Dictionary parameters

  \author Staff, OSU Dept. of Astronomy (rgonzale@astronomy.ohio-state.edu)
  \date 2010 March 9 [rdg/rwp]

  \par Modification History:
<pre>
  2010 Oct 20 - new entries for LBT after the first MODS1 commissioning run
                as additions to the lbtData struct [rwp/osu]
</pre>
*/

/*
// Ice and IIF include files from LBT.
*/
#include <iostream>
#include <fstream>
#include <sstream>
#include <Ice/Ice.h>
#include "Factory.h"
#include "IIFServer.h"


/*
// Namespaces used by the LBTO software.
*/
using namespace std;
using namespace lbto;


// Communicator and pointer to ZeroICE and IIF functions

extern Ice::CommunicatorPtr communicator; // Ice communicator
extern lbto::result res; // result object
extern lbto::IIFServerPrx iif;
extern lbto::FactoryPrx factory;

//----------------------------------------------------------------
//
// Data Dictionary Get/Set Parameters
//
/*!
  \brief MODS DD parameters
*/

typedef struct lbtData {
  string authInst;     //!< Name of the authorized instrument
  // Telescope Pointing Information
  string telUTC;       //!< UTC time of the telescope query
  string telLST;       //!< Local Sidereal Time
  double telRA;        //!< Telescope RA in radians
  double telDec;       //!< Telescope Dec in radians
  double telAz;        //!< Telescope azimuth encoder value in arcsec
  double telEl;        //!< Telescope azimuth encoder value in arcsec
  double telRot;       //!< Telescope direct gregorian rotator angle in arcsec 
  string rotMode;      //!< Focal station rotator mode (POSITION, PARALLACTIC, NATIVE)
  double parAngle;     //!< Parallactic angle if rotMode=POSITION, offset if rotMode=PARALLACTIC in radians
  double posAngle;     //!< Celestial position angle in radians
  double rotAngle;     //!< Instrument Rotator angle in radians
  // AGw Stage and Guide Star PCS Coordinates (engineering)
  double guiRA;        //!< Guide object RA in radians
  double guiDec;       //!< Guide object Dec in radians
  double reqPCSX;      //!< Requested PCS_X AGw Position in mm
  double reqPCSY;      //!< Requested PCS_Y AGw Position in mm
  double actPCSX;      //!< Actual (achieved) PCS_X AGw Position in mm
  double actPCSY;      //!< Actual (achieved) PCS_Y AGw Position in mm
  double gsPredX;      //!< Predicted Guide Star PCS_X in mm
  double gsPredY;      //!< Predicted Guide Star PCS_Y in mm
  double gsMeasX;      //!< Measured Guide Star PCS_X in mm
  double gsMeasY;      //!< Measured Guide Star PCS_Y in mm
  double gsDeltaX;     //!< Guide Star X Offset from Predicted in mm
  double gsDeltaY;     //!< Guide Star Y Offset from Predicted in mm
  // Primary and Secondary Mirror Collimation Positions
  float m1PosX;        //!< Primary Mirror X position in mm
  float m1PosY;        //!< Primary Mirror Y position in mm
  float m1PosZ;        //!< Primary Mirror Z position in mm
  float m1RotX;        //!< Primary Mirror RX rotation in arcsec
  float m1RotY;        //!< Primary Mirror RY rotation in arcsec
  float m1RotZ;        //!< Primary Mirror RZ rotation in arcsec
  float m1CTemp;       //!< Primary Mirror Temperature in degrees Celsius
  float m1ATemp;       //!< Primary Mirror Ambient Air Temperature in degrees Celsius
  float m2PosX;        //!< Secondary Mirror X position in mm
  float m2PosY;        //!< Secondary Mirror Y position in mm
  float m2PosZ;        //!< Secondary Mirror Z position in mm
  float m2RotX;        //!< Secondary Mirror RX rotation in arcsec
  float m2RotY;        //!< Secondary Mirror RY rotation in arcsec
  float m2RotZ;        //!< Secondary Mirror RZ rotation in arcsec
  float m2CTemp;       //!< Secondary Mirror collimation Temperature in degrees Celsius
  // LBT Structure and Air Temperature Sensors
  float ttemp201;      //!< Middle C-Ring Steel Temperature
  float ttemp202;      //!< Middle C-Ring Air Temperature
  float ttemp203;      //!< Upper C-Ring Steel Temperature
  float ttemp204;      //!< Upper C-Ring Air Temperature
  float ttemp205;      //!< Lower C-Ring Steel Temperature
  float ttemp206;      //!< Lower C-Ring Air Temperature
  float ttemp207;      //!< Right Lower Wind Brace Steel Temperature
  float ttemp208;      //!< Right Lower Wind Brace Air Temperature
  float ttemp209;      //!< Right Mirror-Cell Brace Steel Temperature
  float ttemp210;      //!< Right Mirror-Cell Brace Air Temperature
  float ttemp301;      //!< Right C-Ring Extension Steel Temperature
  float ttemp302;      //!< Right C-Ring Extension Air Temperature
  float ttemp303;      //!< Left C-Ring Extension Steel Temperature
  float ttemp304;      //!< Left C-Ring Extension Air Temperature
  float ttemp305;      //!< Upper Wind Brace Steel Temperature
  float ttemp306;      //!< Upper Wind Brace Air Temperature
  float ttemp307;      //!< Right Middle Wind Brace Steel Temperature
  float ttemp308;      //!< Right Middle Wind Brace Air Temperature
  float ttemp309;      //!< Left Middle Wind Brace Steel Temperature
  float ttemp310;      //!< Left Middle Wind Brace Air Temperature
  // LBT Weather Station data
  int lbtLink;         //!< Is the LBT weather station link active?
  float lbtPres;       //!< Air pressure at the LBT in hPa
  float lbtTemp;       //!< Ambient air temperature at the LBT in degree Celsius
  float lbtHum;        //!< Relative humidity at the LBT in percent
  float lbtDwpt;       //!< Dew-Point temperature at the LBT in degrees Celsius
} lbtData_t;

/*
  This data structure holds the MODS data dictionary parameters described 
  in document OSUMODS-2009-00x (2009 Nov 19). Note that the call to 
  mods_SetParameter() will create the necessary time stamps.
*/
typedef struct modsData {
  string modsName; // MODS instrument ID
  string powerState; // Instrument power status
  string hatch; // Instrument dark hatch status
  string imcsLaser; // IMCS IR laser power status
  float glycolSupplyPressure; // glycol supply pressure & temp
  float glycolSupplyTemp;
  float glycolReturnPressure; // glycol return pressure & temp
  float glycolReturnTemp;
  double blueDewarPressure; // Blue CCD dewar pressure & temp
  double blueDewarTemp;
  double blueCCDTemp;
  double redDewarPressure; // Red CCD dewar pressure & temp
  double redDewarTemp;
  double redCCDTemp;
  double blueIEBTemp; // Instrument Box Temperatures
  double redIEBTemp;
  double blueHEBTemp;
  double redHEBTemp;
  double utilBoxTemp;
} modsData_t;

//
// The tables below give the data dictionary entries corresponding to 
// the entries in the lbtData struct. The index [side] is either 
// 0=Left (SX) or 1=Right (DX). These are the DD names that would be 
// passed into the SeqDD entry in the GetParameter() call.
//
// Telescope Pointing Information
//

extern int side;

// variable Data Dictionary Parameter Name type units

enum { TELRA, TELDEC, TELUTC, TELLST, PARANGLE, POSANGLE, ROTANGLE, ROTMODE, 
       GUIRA, GUIDEC, REQPCSX, REQPCSY, ACTPCSX, ACTPCSY, 
       GSPREDX, GSPREDY, GSMEASX, GSMEASY, GSDELTAX, GSDELTAY,
       TELAZ, TELEL, TELROT,
       AUTHINST, 
       M1POSX, M1POSY , M1POSZ, M1ROTX, M1ROTY, M1ROTZ, M1CTEMP, M1ATEMP,
       M2POSX, M2POSY , M2POSZ, M2ROTX, M2ROTY, M2ROTZ, M2CTEMP,
       TTEMP201, TTEMP202, TTEMP203, TTEMP204, TTEMP205,
       TTEMP206, TTEMP207, TTEMP208, TTEMP209, TTEMP210,
       TTEMP301, TTEMP302, TTEMP303, TTEMP304, TTEMP305,
       TTEMP306, TTEMP307, TTEMP308, TTEMP309, TTEMP310,
       LBTLINK, LBTPRES, LBTTEMP, LBTHUM, LBTDWPT
};

extern char clientProxy_Name[80];
extern char instrument_Name[80];
extern char focalStation_Name[80];

extern char iifmsgs[200][100];
extern int iifcount;

#endif  // modsIIFUtils_h
