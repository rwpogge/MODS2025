#include "IIFServerI.hpp"
#include <sys/stat.h>
#include <iostream>
#include <sys/types.h>
#include "GshmVar.hpp"

using namespace lbto;
using namespace std;

//! The class's parameterized Constructor
IIFServerI::IIFServerI(Ice::CommunicatorPtr comm, IIF * _anIIF)
{
  // Initializations
  __cmds = 0;
  btarget = false;
  bguidestars = false;
  bhotspot = false;
  boffset = false;
  bnonSidereal = false;
  breference = false;
  anIIF = _anIIF;
  __comm = comm;

  if (anIIF) proxyInfo = anIIF->proxy_info_string();

  //read the config.server file and get all its properties.
  __props = __comm->getProperties();
}

IIFServerI::~IIFServerI()
{
  if (anIIF) { delete anIIF; }
}

//! Check authorization for control of associated telescope sides
result
IIFServerI::Authorize(const Ice::Current &)
{
  if (iif.ICE.logging)
    {
      Ice::Print info(__comm->getLogger());
      stringstream oss;
      oss << "[]";
      info << "{proxy: " << proxyInfo << ", cmd: " << "Authorize" << ", args: " << oss.str() << "}";
    }

  result tmp;

  bool res = anIIF->Authorize();

  if (res)
    {
      tmp.rescode = EXIT_SUCCESS;
      tmp.resmsg.push_back("Instrument Authorized");
    }

  else
    {
      tmp.rescode = EXIT_FAILURE;
      tmp.resmsg.push_back("Instrument Not Authorized");
    }

  return tmp;
}

//! StepFocus moves the respective focus position, by moving the OPE
//! a given distance in the direction of the telescope’s Z axis.
result
IIFServerI::StepFocus(double relPos, string opetype, string side, const Ice::Current &)
{
  if (iif.ICE.logging)
    {
      Ice::Print info(__comm->getLogger());
      stringstream oss;
      oss << "[" << relPos;
      oss << ", " << opetype;
      oss << ", " << side;
      oss << "]";
      info << "{proxy: " << proxyInfo << ", cmd: " << "StepFocus" << ", args: " << oss.str() << "}";
    }

  bool errorCheck = Configuration::getParameter("IIFIceErrorChecking", true); // 13-Dec-13
//  if(anIIF->getInstrument() == "LUCIFER") errorCheck = false; // 13-Dec-13, disabled 30-Mar-16

  opeType ope = StrToOpe(opetype, errorCheck);

  if (ope < 0)
    {
      result tmp;
      tmp.rescode = EXIT_FAILURE;
      tmp.resmsg.push_back("StepFocus result status: Error");
      tmp.resmsg.push_back("StepFocus Error: ope invalid: " + opetype);
      Ice::Error err(__comm->getLogger());
      err << proxyInfo << " StepFocus Error: ope invalid: " + opetype;
      return tmp;
    }

  Result * aResult = anIIF->StepFocus(relPos, ope, side.c_str());

  return execute(aResult, "StepFocus");
}

//! MoveFocus moves an OPE to a new absolute position z to adjust or to define a new focus position.
result
IIFServerI::MoveFocus(double absPos, string opetype, string side, const Ice::Current &)
{
  if (iif.ICE.logging)
    {
      Ice::Print info(__comm->getLogger());
      stringstream oss;
      oss << "[" << absPos;
      oss << ", " << opetype;
      oss << ", " << side;
      oss << "]";
      info << "{proxy: " << proxyInfo << ", cmd: " << "MoveFocus" << ", args: " << oss.str() << "}";
    }

  bool errorCheck = Configuration::getParameter("IIFIceErrorChecking", true); // 13-Dec-13
//  if(anIIF->getInstrument() == "LUCIFER") errorCheck = false; // 13-Dec-13, disabled 30-Mar-16

  opeType ope = StrToOpe(opetype, errorCheck);

  if (ope < 0)
    {
      result tmp;
      tmp.rescode = EXIT_FAILURE;
      tmp.resmsg.push_back("MoveFocus result status: Error");
      tmp.resmsg.push_back("MoveFocus Error: ope invalid: " + opetype);
      Ice::Error err(__comm->getLogger());
      err << proxyInfo << " MoveFocus Error: ope invalid: " + opetype;
      return tmp;
    }

  Result * aResult = anIIF->MoveFocus(absPos, ope, side.c_str());

  return execute(aResult, "MoveFocus");
}

//! OffsetPointing moves the telescope a small distance, setting the value
//! of the telescope pointing coordinates to the new position.
result
IIFServerI::OffsetPointing(double rotangle, double offx, double offy, string coordsys, string opetype __attribute__((unused)), string np __attribute__((unused)), string movetype, string side, const Ice::Current &)
{
  return OffsetPointing(rotangle, offx, offy, coordsys, movetype, side, "OffsetPointing");
}

//! 25-Mar-16
//! OffsetPointing2 moves the telescope a small distance, setting the value
//! of the telescope pointing coordinates to the new position, without ope and np.
result
IIFServerI::OffsetPointing2(double rotangle, double offx, double offy, string coordsys, string movetype, string side, const Ice::Current &)
{
  return OffsetPointing(rotangle, offx, offy, coordsys, movetype, side, "OffsetPointing2");
}

//! 25-Mar-16
//! The real OffsetPointing method
result
IIFServerI::OffsetPointing(double rotangle, double offx, double offy, string coordsys, string movetype, string side, string cmd)
{
  if (iif.ICE.logging)
    {
      Ice::Print info(__comm->getLogger());
      stringstream oss;
      oss << "[" << rotangle;
      oss << ", " << offx;
      oss << ", " << offy;
      oss << ", " << coordsys;
      oss << ", " << movetype;
      oss << ", " << side;
      oss << "]";
      info << "{proxy: " << proxyInfo << ", cmd: " << cmd << ", args: " << oss.str() << "}";
    }

  result tmp;
  bool bad = false, cs;

  bool errorCheck = Configuration::getParameter("IIFIceErrorChecking", true); // 13-Dec-13
//  if(anIIF->getInstrument() == "LUCIFER") errorCheck = false; // 13-Dec-13, disabled 30-Mar-16

  offsetType off = StrToOffset(coordsys, errorCheck);

  if (off < 0)
    {
      tmp.rescode = EXIT_FAILURE;
      tmp.resmsg.push_back(cmd + " result status: Error");
      tmp.resmsg.push_back(cmd + " Error: coordinate system invalid: " + coordsys);
      Ice::Error err(__comm->getLogger());
      err << proxyInfo << " " << cmd << " Error: coordinate system invalid: " + coordsys;
      bad = true;
    }

  /* check for presence of 'CS' 29-Apr-15 */
  string temp = movetype;
  transform(temp.begin(), temp.end(), temp.begin(), (int(*)(int))toupper);

  if (temp.find("CS") != string::npos)
    {
      cs = true;
      temp.resize(temp.size() - 2); // remove the 'CS'
    }

  else { cs = false; }

  moveType move = StrToMove(temp, errorCheck);

  if (move < 0)
    {
      tmp.rescode = EXIT_FAILURE;

      if (!bad) { tmp.resmsg.push_back(cmd + " result status: Error"); }

      tmp.resmsg.push_back(cmd + " Error: move type invalid: " + movetype);
      Ice::Error err(__comm->getLogger());
      err << proxyInfo << " " << cmd << " Error: move type invalid: " + movetype;
      bad = true;
    }

  if (bad) { return tmp; }

  Offset * offset = new Offset(offx, offy, off, move, cs); // added move, cs 29-Apr-15

  /* pseudo-monocular support 15-Aug-13 */
  /* reworked 15-Mar-16 */
  bool pseudoMonocular = false;
  string currentSide = side;
  transform(currentSide.begin(), currentSide.end(), currentSide.begin(), (int(*)(int))tolower);

  /* set pseudoMonocular and currentSide */
  if (iif.side[0].pseudoMonocular || iif.side[1].pseudoMonocular)
    {
      bool leftSide  = currentSide == "both" || currentSide == "left";
      bool rightSide = currentSide == "both" || currentSide == "right";
      string currentInstrument = anIIF->getInstrument();
      string leftAuthorizedInstrument  = iif.side[0].authorizedInstrument;
      string rightAuthorizedInstrument = iif.side[1].authorizedInstrument;
      string currentFocalStation = anIIF->getFocalStation();
      string leftAuthorizedFocalStation  = iif.side[0].authorizedFocalStation;
      string rightAuthorizedFocalStation = iif.side[1].authorizedFocalStation;

      /* ARGOS support 15-Mar-16 */
      if (currentFocalStation         == "bentGregorianArgos") { currentFocalStation         = "bentGregorianFront"; }

      if (leftAuthorizedFocalStation  == "bentGregorianArgos") { leftAuthorizedFocalStation  = "bentGregorianFront"; }

      if (rightAuthorizedFocalStation == "bentGregorianArgos") { rightAuthorizedFocalStation = "bentGregorianFront"; }

      /* do not allow pseudo-monocular if the active side is not authorized,
       * and restrict the side (no "both") for passive requests
       * there is a side ambiguity here, in that if identical instruments
       * are on both sides, and the passive side sends side "both", it will be interpreted as 'active'
       * note: only one side can be set for pseudoMonocular */
      if (iif.side[0].pseudoMonocular)
        {
          if (leftSide && currentInstrument == leftAuthorizedInstrument && currentFocalStation == leftAuthorizedFocalStation)
            {
              /* active side request */
              pseudoMonocular = true;
              currentSide = "left"; // override possible "both"
            }

          else if (rightSide && currentInstrument == rightAuthorizedInstrument && currentFocalStation == rightAuthorizedFocalStation)
            {
              /* passive side request */
              currentSide = "right"; // override possible "both"
            }
        }

      else
        {
          if (rightSide && currentInstrument == rightAuthorizedInstrument && currentFocalStation == rightAuthorizedFocalStation)
            {
              /* active side request */
              pseudoMonocular = true;
              currentSide = "right"; // override possible "both"
            }

          else if (leftSide && currentInstrument == leftAuthorizedInstrument && currentFocalStation == leftAuthorizedFocalStation)
            {
              /* passive side request */
              currentSide = "left"; // override possible "both"
            }
        }
    }

  /* set up sync offsets for pseudo-monocular 15-Aug-13 */
  /* this code is copied from BinocularControl */
  if (pseudoMonocular)
    {
      iif.side[0].offset.sync = 2;
      iif.side[1].offset.sync = 2;
    }

  /* issue active (or only) offset */
  Result * aResult = anIIF->OffsetPointing(rotangle, offset, currentSide.c_str()); // remove ope, np, movetype 29-Apr-15

  /* pseudo-monocular support 15-Aug-13 */
  /* ignore any errors */
  if (pseudoMonocular)
    {
      result tmp;
      string passiveFocalStation, passiveSide;

      if (currentSide == "left")
        {
          passiveSide = "right";
          passiveFocalStation = iif.side[1].authorizedFocalStation;
        }

      else
        {
          passiveSide = "left";
          passiveFocalStation = iif.side[0].authorizedFocalStation;
        }

      if (passiveFocalStation == "prime")
        {
          Result * bResult = anIIF->OffsetPointing(rotangle, offset, passiveSide.c_str(), true); // send rotation angle 21-Nov-13, remove ope, np, movetype 29-Apr-15
          tmp = execute(bResult, "OffsetPointing");
        }

      else
        {
          Result * bResult = anIIF->OffsetPointing(rotangle, offset, passiveSide.c_str(), true); // remove ope, np, movetype 29-Apr-15
          tmp = execute(bResult, "OffsetPointing");
        }

      /* even if this command fails, the PCS has been given the offset 18-Apr-17*/
    }

  delete offset;
  return execute(aResult, cmd);
}

//!OffsetGuiding moves the telescope a small distance without changing the value of the pointing coordinates.
result
IIFServerI::OffsetGuiding(double rotangle, double offx, double offy, string coordsys, string side, const Ice::Current &)
{
  if (iif.ICE.logging)
    {
      Ice::Print info(__comm->getLogger());
      stringstream oss;
      oss << "[" << rotangle;
      oss << ", " << offx;
      oss << ", " << offy;
      oss << ", " << coordsys;
      oss << ", " << side;
      oss << "]";
      info << "{proxy: " << proxyInfo << ", cmd: " << "OffsetGuiding" << ", args: " << oss.str() << "}";
    }

  bool errorCheck = Configuration::getParameter("IIFIceErrorChecking", true); // 13-Dec-13
//  if(anIIF->getInstrument() == "LUCIFER") errorCheck = false; // 13-Dec-13, disabled 30-Mar-16

  offsetType off = StrToOffset(coordsys, errorCheck);

  if (off < 0)
    {
      result tmp;
      tmp.rescode = EXIT_FAILURE;
      tmp.resmsg.push_back("OffsetGuiding result status: Error");
      tmp.resmsg.push_back("OffsetGuiding Error: coordinate system invalid: " + coordsys);
      Ice::Error err(__comm->getLogger());
      err << proxyInfo << " OffsetGuiding Error: coordinate system invalid: " + coordsys;
      return tmp;
    }

  Offset * offset = new Offset(offx, offy, off);

  Result * aResult = anIIF->OffsetGuiding(rotangle, offset, side.c_str());
  delete offset;
  return execute(aResult, "OffsetGuiding");
}

//! TipTilt moves an OPE in tip or tilt direction, relative to the current position.
result
IIFServerI::TipTilt(double xrot, double yrot, string opetype, string side, const Ice::Current &)
{
  if (iif.ICE.logging)
    {
      Ice::Print info(__comm->getLogger());
      stringstream oss;
      oss << "[" << xrot;
      oss << ", " << yrot;
      oss << ", " << opetype;
      oss << ", " << side;
      oss << "]";
      info << "{proxy: " << proxyInfo << ", cmd: " << "TipTilt" << ", args: " << oss.str() << "}";
    }

  bool errorCheck = Configuration::getParameter("IIFIceErrorChecking", true); // 13-Dec-13
//  if(anIIF->getInstrument() == "LUCIFER") errorCheck = false; // 13-Dec-13, disabled 30-Mar-16

  opeType ope = StrToOpe(opetype, errorCheck);

  if (ope < 0)
    {
      result tmp;
      tmp.rescode = EXIT_FAILURE;
      tmp.resmsg.push_back("TipTilt result status: Error");
      tmp.resmsg.push_back("TipTilt Error: ope invalid: " + opetype);
      Ice::Error err(__comm->getLogger());
      err << proxyInfo << " TipTilt Error: ope invalid: " + opetype;
      return tmp;
    }

  Result * aResult = anIIF->TipTilt(xrot, yrot, ope, side.c_str());

  return execute(aResult, "TipTilt");
}

//! SetPMTerm sets a single term in the pointing model.
result
IIFServerI::SetPMTerm(string name, double value, string side, const Ice::Current &)
{
  return SetPMTerm(name, value, "ABS", side, "SetPMTerm");
}

//! 25-Mar-16
//! SetPMTerm2 sets a single term in the pointing model with moveType
result
IIFServerI::SetPMTerm2(string name, double value, string movetype, string side, const Ice::Current &)
{
  return SetPMTerm(name, value, movetype, side, "SetPMTerm2");
}

//! 25-Mar-16
//! The real SetPMTerm method
result
IIFServerI::SetPMTerm(string name, double value, string movetype, string side, string cmd)
{
  if (iif.ICE.logging)
    {
      Ice::Print info(__comm->getLogger());
      stringstream oss;
      oss << "[" << name;
      oss << ", " << value;
      oss << ", " << movetype;
      oss << ", " << side;
      oss << "]";
      info << "{proxy: " << proxyInfo << ", cmd: " << cmd << ", args: " << oss.str() << "}";
    }

  bool errorCheck = Configuration::getParameter("IIFIceErrorChecking", true);
//  if(anIIF->getInstrument() == "LUCIFER") errorCheck = false;, disabled 30-Mar-16

  moveType move = StrToMove(movetype, errorCheck);

  if (move < 0)
    {
      result tmp;
      tmp.rescode = EXIT_FAILURE;
      tmp.resmsg.push_back(cmd + " result status: Error");
      tmp.resmsg.push_back(cmd + " Error: move type invalid: " + movetype);
      Ice::Error err(__comm->getLogger());
      err << proxyInfo << " " << cmd << " Error: move type invalid: " + movetype;
      return tmp;
    }

  Result * aResult = anIIF->SetPMTerm(name, value, move, side.c_str());

  return execute(aResult, cmd);
}

//! MoveXYZ moves the primary and secondary together in X, Y and Z direction.
result
IIFServerI::MoveXYZ(double xmov, double ymov, double zmov, string side, const Ice::Current &)
{
  if (iif.ICE.logging)
    {
      Ice::Print info(__comm->getLogger());
      stringstream oss;
      oss << "[" << xmov;
      oss << ", " << ymov;
      oss << ", " << zmov;
      oss << ", " << side;
      oss << "]";
      info << "{proxy: " << proxyInfo << ", cmd: " << "MoveXYZ" << ", args: " << oss.str() << "}";
    }

  Result * aResult = anIIF->MoveXYZ(xmov, ymov, zmov, side.c_str());

  return execute(aResult, "MoveXYZ");
}

//! MoveXY moves an OPE in X or Y direction, relative to the current position.
result
IIFServerI::MoveXY(double xmov, double ymov, string opetype, string side, const Ice::Current &)
{
  if (iif.ICE.logging)
    {
      Ice::Print info(__comm->getLogger());
      stringstream oss;
      oss << "[" << xmov;
      oss << ", " << ymov;
      oss << ", " << opetype;
      oss << ", " << side;
      oss << "]";
      info << "{proxy: " << proxyInfo << ", cmd: " << "MoveXY" << ", args: " << oss.str() << "}";
    }

  bool errorCheck = Configuration::getParameter("IIFIceErrorChecking", true); // 13-Dec-13
//  if(anIIF->getInstrument() == "LUCIFER") errorCheck = false; // 13-Dec-13, disabled 30-Mar-16

  opeType ope = StrToOpe(opetype, errorCheck);

  if (ope < 0)
    {
      result tmp;
      tmp.rescode = EXIT_FAILURE;
      tmp.resmsg.push_back("MoveXY result status: Error");
      tmp.resmsg.push_back("MoveXY Error: ope invalid: " + opetype);
      Ice::Error err(__comm->getLogger());
      err << proxyInfo << " MoveXY Error: ope invalid: " + opetype;
      return tmp;
    }

  Result * aResult = anIIF->MoveXY(xmov, ymov, ope, side.c_str());

  return execute(aResult, "MoveXY");
}

//! Rotates the primary and the secondary mirror around a common point.
result
IIFServerI::RotateCommon(double z, double angle, double direction,
                         string side, const Ice::Current &)
{
  if (iif.ICE.logging)
    {
      Ice::Print info(__comm->getLogger());
      stringstream oss;
      oss << "[" << z;
      oss << ", " << angle;
      oss << ", " << direction;
      oss << ", " << side;
      oss << "]";
      info << "{proxy: " << proxyInfo << ", cmd: " << "RotateCommon" << ", args: " << oss.str() << "}";
    }

  Result * aResult = anIIF->RotateCommon(z, angle, direction, side.c_str());

  return execute(aResult, "RotateCommon");
}

//! the tertiary mirror is rotated to adjust the incoming beam angle for
result
IIFServerI::RotateZ(double angle, string movetype, string side, const Ice::Current &)
{
  if (iif.ICE.logging)
    {
      Ice::Print info(__comm->getLogger());
      stringstream oss;
      oss << "[" << angle;
      oss << ", " << movetype;
      oss << ", " << side;
      oss << "]";
      info << "{proxy: " << proxyInfo << ", cmd: " << "RotateZ" << ", args: " << oss.str() << "}";
    }

  bool errorCheck = Configuration::getParameter("IIFIceErrorChecking", true); // 13-Dec-13
//  if(anIIF->getInstrument() == "LUCIFER") errorCheck = false; // 13-Dec-13, disabled 30-Mar-16

  moveType move = StrToMove(movetype, errorCheck);

  if (move < 0)
    {
      result tmp;
      tmp.rescode = EXIT_FAILURE;
      tmp.resmsg.push_back("RotateZ result status: Error");
      tmp.resmsg.push_back("RotateZ Error: move type invalid: " + movetype);
      Ice::Error err(__comm->getLogger());
      err << proxyInfo << " RotateZ Error: move type invalid: " + movetype;
      return tmp;
    }

  Result * aResult = anIIF->RotateZ(angle, move, side.c_str());

  return execute(aResult, "RotateZ");
}

//! This command combines the functionality of MoveXY, TipTilt, StepFocus and MoveFocus.
result
IIFServerI::Move(double x, double y, double z, double rx, double ry, double rz, int dflag, string movetype, string opetype, int time, string side, const Ice::Current &)
{
  if (iif.ICE.logging)
    {
      Ice::Print info(__comm->getLogger());
      stringstream oss;
      oss << "[" << x;
      oss << ", " << y;
      oss << ", " << z;
      oss << ", " << rx;
      oss << ", " << ry;
      oss << ", " << rz;
      oss << ", " << dflag;
      oss << ", " << movetype;
      oss << ", " << opetype;
      oss << ", " << time;
      oss << ", " << side;
      oss << "]";
      info << "{proxy: " << proxyInfo << ", cmd: " << "Move" << ", args: " << oss.str() << "}";
    }

  bool bad = false;
  result tmp;

  bool errorCheck = Configuration::getParameter("IIFIceErrorChecking", true); // 13-Dec-13
//  if(anIIF->getInstrument() == "LUCIFER") errorCheck = false; // 13-Dec-13, disabled 30-Mar-16

  opeType ope = StrToOpe(opetype, errorCheck);

  if (ope < 0)
    {
      tmp.rescode = EXIT_FAILURE;
      tmp.resmsg.push_back("Move result status: Error");
      tmp.resmsg.push_back("Move Error: ope invalid: " + opetype);
      Ice::Error err(__comm->getLogger());
      err << proxyInfo << " Move Error: ope invalid: " + opetype;
      bad = true;
    }

  moveType move = StrToMove(movetype, errorCheck);

  if (move < 0)
    {
      tmp.rescode = EXIT_FAILURE;

      if (!bad) { tmp.resmsg.push_back("Move result status: Error"); }

      tmp.resmsg.push_back("Move Error: move type invalid: " + movetype);
      Ice::Error err(__comm->getLogger());
      err << proxyInfo << " Move Error: move type invalid: " + movetype;
      bad = true;
    }

  Result * aResult = anIIF->Move(x, y, z, rx, ry, rz, dflag, move, ope, time, side.c_str());

  return execute(aResult, "Move");
}

//! Set the hotspot coordinates to be used by PresetTelescope
result
IIFServerI::SetHotspot(double coord1, double coord2, const Ice::Current &)
{
  if (iif.ICE.logging)
    {
      Ice::Print info(__comm->getLogger());
      stringstream oss;
      oss << "[" << coord1;
      oss << ", " << coord2;
      oss << "]";
      info << "{proxy: " << proxyInfo << ", cmd: " << "SetHotspot" << ", args: " << oss.str() << "}";
    }

  result tmp;

  hotspot.Setcoord1(coord1);
  hotspot.Setcoord2(coord2);
  bhotspot = true;
  tmp.rescode = EXIT_SUCCESS;
  tmp.resmsg.push_back("SetHotspot result status: OK");

  if (iif.ICE.logging)
    {
      Ice::Print info(__comm->getLogger());
      info << "SetHotspot OK";
    }

  return tmp;
}

//! Removes from the system the hotspot.
result
IIFServerI::ClearHotspot(const Ice::Current &)
{
  if (iif.ICE.logging)
    {
      Ice::Print info(__comm->getLogger());
      stringstream oss;
      oss << "[]";
      info << "{proxy: " << proxyInfo << ", cmd: " << "ClearHotspot" << ", args: " << oss.str() << "}";
    }

  result tmp;

  bhotspot = false;
  tmp.rescode = EXIT_SUCCESS;
  tmp.resmsg.push_back("ClearHotspot result status: OK");

  if (iif.ICE.logging)
    {
      Ice::Print info(__comm->getLogger());
      info << "ClearHotspot OK";
    }

  return tmp;
}

//! Set the offset coordinates to be used by PresetTelescope
result
IIFServerI::SetOffset(double coord1, double coord2, string coordsys, const Ice::Current &)
{
  return SetOffset(coord1, coord2, coordsys, "", "SetOffset");
}

// 29-Apr-15
//! Set the offset coordinates to be used by PresetTelescope with movetype
//! Deprecated
result
IIFServerI::SetOffsetNew(double coord1, double coord2, string coordsys, string movetype, const Ice::Current &)
{
  return SetOffset(coord1, coord2, coordsys, movetype, "SetOffsetNew");
}

// 25-Mar-16
// 29-Apr-15
//! Set the offset coordinates to be used by PresetTelescope with movetype
result
IIFServerI::SetOffset2(double coord1, double coord2, string coordsys, string movetype, const Ice::Current &)
{
  return SetOffset(coord1, coord2, coordsys, movetype, "SetOffset2");
}

// 25-Mar-16
//! The real SetOffset method
result
IIFServerI::SetOffset(double coord1, double coord2, string coordsys, string movetype, string cmd)
{
  if (iif.ICE.logging)
    {
      Ice::Print info(__comm->getLogger());
      stringstream oss;
      oss << "[" << coord1;
      oss << ", " << coord2;
      oss << ", " << coordsys;
      oss << ", '" << movetype;
      oss << "']";
      info << "{proxy: " << proxyInfo << ", cmd: " << cmd << ", args: " << oss.str() << "}";
    }

  result tmp;
  bool cs;

  bool errorCheck = Configuration::getParameter("IIFIceErrorChecking", true); // 13-Dec-13
//  if(anIIF->getInstrument() == "LUCIFER") errorCheck = false; // 13-Dec-13, disabled 30-Mar-16

  offsetType off = StrToOffset(coordsys, errorCheck);

  if (off < 0)
    {
      tmp.rescode = EXIT_FAILURE;
      tmp.resmsg.push_back(cmd + " result status: Error");
      tmp.resmsg.push_back(cmd + " Error: coordinate system invalid: " + coordsys);
      Ice::Error err(__comm->getLogger());
      err << proxyInfo << " " << cmd << " Error: coordinate system invalid: " + coordsys;
      return tmp;
    }

  string temp = movetype;
  transform(temp.begin(), temp.end(), temp.begin(), (int(*)(int))toupper);
  cs = temp.find("CS") != string::npos;

  offset.Setcoord1(coord1);
  offset.Setcoord2(coord2);
  offset.Setcoordsystem(off);
  offset.SetmoveType(MV_ABS);
  offset.SetCS(cs);

  boffset = true;
  tmp.rescode = EXIT_SUCCESS;
  tmp.resmsg.push_back(cmd + ": Status OK");

  if (iif.ICE.logging)
    {
      Ice::Print info(__comm->getLogger());
      info << cmd + " OK";
    }

  return tmp;
}

//! Removes from the system the offset.
result
IIFServerI::ClearOffset(const Ice::Current &)
{
  if (iif.ICE.logging)
    {
      Ice::Print info(__comm->getLogger());
      stringstream oss;
      oss << "[]";
      info << "{proxy: " << proxyInfo << ", cmd: " << "ClearOffset" << ", args: " << oss.str() << "}";
    }

  result tmp;

  boffset = false;
  tmp.rescode = EXIT_SUCCESS;
  tmp.resmsg.push_back("ClearOffset result status: OK");

  if (iif.ICE.logging)
    {
      Ice::Print info(__comm->getLogger());
      info << "ClearOffset OK";
    }

  return tmp;
}

//! Set the non-sidereal values to be used by PresetTelescope
result
IIFServerI::SetNonSidereal(nonsidereal target, bool override, const Ice::Current &)
{
  if (iif.ICE.logging)
    {
      Ice::Print info(__comm->getLogger());
      stringstream oss;
      oss << "[[" << target.type;
      oss << ", " << target.wavelength;
      oss << ", '" << target.file << "'";
      oss << ", " << target.value0;
      oss << ", " << target.value1;
      oss << ", " << target.value2;
      oss << ", " << target.value3;
      oss << ", " << target.value4;
      oss << "], " << override;
      oss << "]";
      info << "{proxy: " << proxyInfo << ", cmd: " << "SetNonSidereal" << ", args: " << oss.str() << "}";
    }

  result tmp;

  nsType ns = StrToNs(target.type);

  if (ns < 0 || ns == NS_TARGET)  // target cannot be type NS_TARGET
    {
      tmp.resmsg.push_back("SetNonSidereal result status: Error");
      tmp.resmsg.push_back("SetNonSidereal Error: non-siderial type invalid: " + target.type);
      tmp.rescode = EXIT_FAILURE;
      Ice::Error err(__comm->getLogger());
      err << proxyInfo << " SetNonSidereal  Error: :non-sidereal type invalid: " + target.type;
      return tmp;
    }

  nonSidereal.Settype(ns);
  nonSidereal.Setwavelength(target.wavelength);
  nonSidereal.Setfile(target.file);
  nonSidereal.Setvalue(0, target.value0);
  nonSidereal.Setvalue(1, target.value1);
  nonSidereal.Setvalue(2, target.value2);
  nonSidereal.Setvalue(3, target.value3);
  nonSidereal.Setvalue(4, target.value4);
  bnonSidereal = true;

  if (override)
    {
      Result * aResult = anIIF->UpdateNonSiderealTarget(&nonSidereal, OV_SET);
      return execute(aResult, "SetNonSidereal");
    }

  tmp.rescode = EXIT_SUCCESS;
  tmp.resmsg.push_back("SetNonSidereal result status: OK");

  if (iif.ICE.logging)
    {
      Ice::Print info(__comm->getLogger());
      info << "SetNonSidereal OK";
    }

  return tmp;
}

//! Removes from the system the non-sidereal object.
result
IIFServerI::ClearNonSidereal(bool override, const Ice::Current &)
{
  if (iif.ICE.logging)
    {
      Ice::Print info(__comm->getLogger());
      stringstream oss;
      oss << "[" << override << "]";
      info << "{proxy: " << proxyInfo << ", cmd: " << "ClearNonSidereal" << ", args: " << oss.str() << "}";
    }

  result tmp;

  bnonSidereal = false;

  if (override)
    {
      Result * aResult = anIIF->UpdateNonSiderealTarget(&nonSidereal, OV_CLEAR);
      return execute(aResult, "ClearNonSidereal");
    }

  tmp.rescode = EXIT_SUCCESS;
  tmp.resmsg.push_back("ClearNonSidereal result status: OK");

  if (iif.ICE.logging)
    {
      Ice::Print info(__comm->getLogger());
      info << "ClearNonSidereal OK";
    }

  return tmp;
}

//! Update the non-sidereal values
result
IIFServerI::UpdateNonSiderealTarget(double RArate, double DECrate, const Ice::Current &)
{
  if (iif.ICE.logging)
    {
      Ice::Print info(__comm->getLogger());
      stringstream oss;
      oss << "[" << RArate;
      oss << ", " << DECrate;
      oss << "]";
      info << "{proxy: " << proxyInfo << ", cmd: " << "UpdateNonSiderealTarget" << ", args: " << oss.str() << "}";
    }

  nonSidereal.Settype(NS_DIFFERENTIAL);
  nonSidereal.Setwavelength(0.0f);
  nonSidereal.Setfile("");
  nonSidereal.Setvalue(0, 0.0);
  nonSidereal.Setvalue(1, 0.0);
  nonSidereal.Setvalue(2, 0.0);
  nonSidereal.Setvalue(3, RArate);
  nonSidereal.Setvalue(4, DECrate);

  Result * aResult = anIIF->UpdateNonSiderealTarget(&nonSidereal);

  return execute(aResult, "UpdateNonSiderealTarget");
}

//! Set the non-sidereal values for hijacking
result
IIFServerI::SetNonSiderealObject(string objtype, nonsidereal object, bool override, const Ice::Current &)
{
  if (iif.ICE.logging)
    {
      Ice::Print info(__comm->getLogger());
      stringstream oss;
      oss << "[" << objtype;
      oss << ", [" << object.type;
      oss << ", " << object.wavelength;
      oss << ", '" << object.file << "'";
      oss << ", " << object.value0;
      oss << ", " << object.value1;
      oss << ", " << object.value2;
      oss << ", " << object.value3;
      oss << ", " << object.value4;
      oss << "], " << override;
      oss << "]";
      info << "{proxy: " << proxyInfo << ", cmd: " << "SetNonSiderealObject" << ", args: " << oss.str() << "}";
    }

  result tmp;

  objType ob = StrToObj(objtype);

  if (ob < 0)
    {
      tmp.resmsg.push_back("SetNonSiderealObject result status: Error");
      tmp.resmsg.push_back("SetNonSiderealObject Error: non-sidereal object invalid: " + objtype);
      tmp.rescode = EXIT_FAILURE;
      Ice::Error err(__comm->getLogger());
      err << proxyInfo << " SetNonSiderealObject  Error: :non-sidereal object invalid: " + objtype;
      return tmp;
    }

  nsType ns = StrToNs(object.type);

  if (ns < 0 || (ob == OBJ_TARGET && ns == NS_TARGET))  // target cannot be type NS_TARGET
    {
      tmp.resmsg.push_back("SetNonSiderealObject result status: Error");
      tmp.resmsg.push_back("SetNonSiderealObject Error: non-siderial type invalid: " + object.type);
      tmp.rescode = EXIT_FAILURE;
      Ice::Error err(__comm->getLogger());
      err << proxyInfo << " SetNonSiderealObject  Error: :non-sidereal type invalid: " + object.type;
      return tmp;
    }

  NonSidereal nonsidereal;
  nonsidereal.Settype(ns);
  nonsidereal.Setwavelength(object.wavelength);
  nonsidereal.Setfile(object.file);
  nonsidereal.Setvalue(0, object.value0);
  nonsidereal.Setvalue(1, object.value1);
  nonsidereal.Setvalue(2, object.value2);
  nonsidereal.Setvalue(3, object.value3);
  nonsidereal.Setvalue(4, object.value4);

  if (ob == OBJ_TARGET)
    {
      /* same as SetNonSidereal */
      nonSidereal = nonsidereal;
      bnonSidereal = true;
    }

  if (override)
    {
      Result * aResult = anIIF->UpdateNonSiderealObject(ob, &nonsidereal, OV_SET);
      return execute(aResult, "SetNonSiderealObject " + objtype + " override set");
    }

  tmp.rescode = EXIT_SUCCESS;
  tmp.resmsg.push_back("SetNonSiderealObject " + objtype + " result status: OK");

  if (iif.ICE.logging)
    {
      Ice::Print info(__comm->getLogger());
      info << "SetNonSiderealObject " + objtype + " OK";
    }

  return tmp;
}

//! Removes from the system the non-sidereal object.
// 16-Aug-16
result
IIFServerI::ClearNonSiderealObject(string objtype, bool override, const Ice::Current &)
{
  if (iif.ICE.logging)
    {
      Ice::Print info(__comm->getLogger());
      stringstream oss;
      oss << "[" << objtype;
      oss << ", " << override;
      oss << "]";
      info << "{proxy: " << proxyInfo << ", cmd: " << "ClearNonSiderealObject" << ", args: " << oss.str() << "}";
    }

  result tmp;

  objType ob = StrToObj(objtype);

  if (ob < 0)
    {
      tmp.resmsg.push_back("ClearNonSiderealObject result status: Error");
      tmp.resmsg.push_back("ClearNonSiderealObject Error: non-sidereal object invalid: " + objtype);
      tmp.rescode = EXIT_FAILURE;
      Ice::Error err(__comm->getLogger());
      err << proxyInfo << " ClearNonSiderealObject  Error: :non-sidereal object invalid: " + objtype;
      return tmp;
    }

  if (ob == OBJ_TARGET) { bnonSidereal = false; } /* same as ClearNonSidereal */

  if (override)
    {
      NonSidereal nonsidereal;
      nonsidereal.Settype(NS_TARGET); // not used, but must be legal
      Result * aResult = anIIF->UpdateNonSiderealObject(ob, &nonsidereal, OV_CLEAR);
      return execute(aResult, "ClearNonSiderealObject " + objtype + " override set");
    }

  tmp.rescode = EXIT_SUCCESS;
  tmp.resmsg.push_back("ClearNonSiderealObject " + objtype + " override clear: OK");

  if (iif.ICE.logging)
    {
      Ice::Print info(__comm->getLogger());
      info << "ClearNonSiderealObject OK";
    }

  return tmp;
}

//! Update the non-sidereal values
// 16-Aug-16
result
IIFServerI::UpdateNonSiderealObject(string objtype, double RArate, double DECrate, const Ice::Current &)
{
  if (iif.ICE.logging)
    {
      Ice::Print info(__comm->getLogger());
      stringstream oss;
      oss << "[" << objtype;
      oss << ", " << RArate;
      oss << ", " << DECrate;
      oss << "]";
      info << "{proxy: " << proxyInfo << ", cmd: " << "UpdateNonSiderealObject" << ", args: " << oss.str() << "}";
    }

  result tmp;

  objType obj = StrToObj(objtype);

  if (obj < 0)
    {
      tmp.resmsg.push_back("UpdateNonSiderealObject result status: Error");
      tmp.resmsg.push_back("UpdateNonSiderealObject Error: non-siderial object invalid: " + objtype);
      tmp.rescode = EXIT_FAILURE;
      Ice::Error err(__comm->getLogger());
      err << proxyInfo << " UpdateNonSiderealObject  Error: :non-sidereal object invalid: " + objtype;
      return tmp;
    }

  nonSidereal.Settype(NS_DIFFERENTIAL);
  nonSidereal.Setwavelength(0.0f);
  nonSidereal.Setfile("");
  nonSidereal.Setvalue(0, 0.0);
  nonSidereal.Setvalue(1, 0.0);
  nonSidereal.Setvalue(2, 0.0);
  nonSidereal.Setvalue(3, RArate);
  nonSidereal.Setvalue(4, DECrate);

  Result * aResult = anIIF->UpdateNonSiderealObject(obj, &nonSidereal);

  return execute(aResult, "UpdateNonSiderealObject");
}

//! Set the target information to be used by PresetTelescope
result
IIFServerI::SetTarget(double coord1, double coord2, string coordsys, double epoch, float wl, const Ice::Current &)
{

  if (iif.ICE.logging)
    {
      Ice::Print info(__comm->getLogger());
      stringstream oss;
      oss << "[" << coord1;
      oss << ", " << coord2;
      oss << ", " << coordsys;
      oss << ", " << epoch;
      oss << ", " << wl;
      oss << "]";
      info << "{proxy: " << proxyInfo << ", cmd: " << "SetTarget" << ", args: " << oss.str() << "}";
    }

  bool errorCheck = Configuration::getParameter("IIFIceErrorChecking", true); // 13-Dec-13
//  if(anIIF->getInstrument() == "LUCIFER") errorCheck = false; // 13-Dec-13, disabled 30-Mar-16

  coordType coord = StrToCoord(coordsys, errorCheck);

  if (coord < 0)
    {
      result tmp;
      tmp.rescode = EXIT_FAILURE;
      tmp.resmsg.push_back("SetTarget result status: Error");
      tmp.resmsg.push_back("SetTarget Error: coordinate system invalid: " + coordsys);
      Ice::Error err(__comm->getLogger());
      err << proxyInfo << " SetTarget Error: coordinate system invalid: " + coordsys;
      return tmp;
    }

  target.Setcoord1(coord1);
  target.Setcoord2(coord2);
  target.Setcoordsystem(coord);
  target.Setepoch(epoch);
  target.Setwavelength(wl);
  btarget = true;
  result tmp;
  tmp.rescode = EXIT_SUCCESS;
  tmp.resmsg.push_back("SetTarget result status: OK");

  if (iif.ICE.logging)
    {
      Ice::Print info(__comm->getLogger());
      info << "SetTarget OK";
    }

  return tmp;
}

//! PresetTelescope slew the telescope into position in order to begin an observation cycle.
result
IIFServerI::PresetTelescope(double rotangle, string rotmode, string modetype, string side, const Ice::Current &)
{
  if (iif.ICE.logging)
    {
      Ice::Print info(__comm->getLogger());
      stringstream oss;
      oss << "[" << rotangle;
      oss << ", " << rotmode;
      oss << ", " << modetype;
      oss << ", " << side;
      oss << "]";
      info << "{proxy: " << proxyInfo << ", cmd: " << "PresetTelescope" << ", args: " << oss.str() << "}";
    }

  Position currTarget = target; // this may contain non-sidereal data

  if (bnonSidereal)
    {
      /* set the target properties from the non-sidereal object 8-Aug-13 */
      nsType ns = nonSidereal.Gettype();
      currTarget.Settype(ns);
      currTarget.Setcoordsystem(COORD_RADEC);
      currTarget.Setequinox(J2000);
      currTarget.Setepoch(2000.0);
      currTarget.Setwavelength(nonSidereal.Getwavelength());
      currTarget.Setpropermotion(NULL);

      if (ns == NS_FILE) { currTarget.Setfile(nonSidereal.Getfile()); }

      if (ns == NS_DIFFERENTIAL)
        {
          currTarget.Settime(nonSidereal.Getvalue(0));
          currTarget.Setcoord1(nonSidereal.Getvalue(1));
          currTarget.Setcoord2(nonSidereal.Getvalue(2));
          currTarget.SetRARate(nonSidereal.Getvalue(3));
          currTarget.SetDECRate(nonSidereal.Getvalue(4));
        }

      else
        {
          currTarget.Setcoord1(0.0);
          currTarget.Setcoord2(0.0);
        }
    }

  if (!btarget && !bnonSidereal)
    {
      /* return error */
      result t;
      t.resmsg.push_back("PresetTelescope result status: Error");
      t.resmsg.push_back("PresetTelescope Error: No target object defined");
      t.rescode = EXIT_FAILURE;
      Ice::Error err(__comm->getLogger());
      err << proxyInfo << " PresetTelescope Error: No target object defined";
      return t;
    }

  /* call PresetTelescope */
  result tmp;
  bool bad = false;

  bool errorCheck = Configuration::getParameter("IIFIceErrorChecking", true); // 13-Dec-13
//  if(anIIF->getInstrument() == "LUCIFER") errorCheck = false; // 13-Dec-13, disabled 30-Mar-16

  rotatorType rot = StrToRotmode(rotmode, errorCheck);

  if (rot < 0)
    {
      tmp.rescode = EXIT_FAILURE;
      tmp.resmsg.push_back("PresetTelescope result status: Error");
      tmp.resmsg.push_back("PresetTelescope Error: rotator mode invalid: " + rotmode);
      Ice::Error err(__comm->getLogger());
      err << proxyInfo << " PresetTelescope Error: rotator mode invalid: " + rotmode;
      bad = true;
    }

  modeType mode = StrToMode(modetype);

  if (mode < 0)
    {
      tmp.rescode = EXIT_FAILURE;

      if (!bad) { tmp.resmsg.push_back("PresetTelescope result status: Error"); }

      tmp.resmsg.push_back("PresetTelescope Error: mode invalid: " + modetype);
      Ice::Error err(__comm->getLogger());
      err << proxyInfo << " PresetTelescope Error: mode invalid: " + modetype;
      bad = true;
    }

  if (bad) { return tmp; }

  Position ** tgs;
  Offset * off;
  Hotspot * hs;
  Position * ref;

  bguidestars ? tgs = &guidestars[0] : tgs = NULL;
  boffset ? off = &offset : off = NULL;
  bhotspot ? hs = &hotspot : hs = NULL;
  breference ? ref = &reference : ref = NULL;

  /* pseudo-monocular support 15-Aug-13 */
  /* reworked 15-Mar-16 */
  bool pseudoMonocular = false;
  string currentSide = side;
  transform(currentSide.begin(), currentSide.end(), currentSide.begin(), (int(*)(int))tolower);

  /* set pseudoMonocular and currentSide */
  if (iif.side[0].pseudoMonocular || iif.side[1].pseudoMonocular)
    {
      bool leftSide  = currentSide == "both" || currentSide == "left";
      bool rightSide = currentSide == "both" || currentSide == "right";
      string currentInstrument = anIIF->getInstrument();
      string leftAuthorizedInstrument  = iif.side[0].authorizedInstrument;
      string rightAuthorizedInstrument = iif.side[1].authorizedInstrument;
      string currentFocalStation = anIIF->getFocalStation();
      string leftAuthorizedFocalStation  = iif.side[0].authorizedFocalStation;
      string rightAuthorizedFocalStation = iif.side[1].authorizedFocalStation;

      /* ARGOS support 15-Mar-16 */
      if (currentFocalStation         == "bentGregorianArgos") { currentFocalStation         = "bentGregorianFront"; }

      if (leftAuthorizedFocalStation  == "bentGregorianArgos") { leftAuthorizedFocalStation  = "bentGregorianFront"; }

      if (rightAuthorizedFocalStation == "bentGregorianArgos") { rightAuthorizedFocalStation = "bentGregorianFront"; }

      /* do not allow pseudo-monocular if the active side is not authorized,
       * and restrict the side (no "both") for passive requests
       * there is a side ambiguity here, in that if identical instruments
       * are on both sides, and the passive side sends side "both", it will be interpreted as 'active'
       * note: only one side can be set for pseudoMonocular */
      if (iif.side[0].pseudoMonocular)
        {
          if (leftSide && currentInstrument == leftAuthorizedInstrument && currentFocalStation == leftAuthorizedFocalStation)
            {
              /* active side request */
              pseudoMonocular = true;
              currentSide = "left"; // override possible "both"
            }

          else if (rightSide && currentInstrument == rightAuthorizedInstrument && currentFocalStation == rightAuthorizedFocalStation)
            {
              /* passive side request */
              currentSide = "right"; // override possible "both"
            }
        }

      else
        {
          if (rightSide && currentInstrument == rightAuthorizedInstrument && currentFocalStation == rightAuthorizedFocalStation)
            {
              /* active side request */
              pseudoMonocular = true;
              currentSide = "right"; // override possible "both"
            }

          else if (leftSide && currentInstrument == leftAuthorizedInstrument && currentFocalStation == leftAuthorizedFocalStation)
            {
              /* passive side request */
              currentSide = "left"; // override possible "both"
            }
        }
    }

  /* set up sync presets for pseudo-monocular 15-Aug-13 */
  /* this code is copied from BinocularControl */
  if (pseudoMonocular)
    {
      iif.presetClear = false; // added 18-Jul-11
      iif.side[0].preset.sync = 2;
//    iif.side[0].offset.sync = 0; // removed 27-Jul-16
      iif.side[1].preset.sync = 2;
//    iif.side[1].offset.sync = 0; // removed 27-Jul-16
    }

  /* issue active (or only) preset */
  Result * aResult = anIIF->PresetTelescope(rotangle, rot, &currTarget, tgs, mode, currentSide.c_str(), off, hs, ref);

  /* pseudo-monocular support 15-Aug-13 */
  /* ignore any errors */
  if (pseudoMonocular)
    {
      result tmp;
      string passiveFocalStation, passiveSide;

      if (currentSide == "left")
        {
          passiveSide = "right";
          passiveFocalStation = iif.side[1].authorizedFocalStation;
        }

      else
        {
          passiveSide = "left";
          passiveFocalStation = iif.side[0].authorizedFocalStation;
        }

      if (passiveFocalStation == "prime")
        {
          Result * bResult = anIIF->PresetTelescope(0.0, rot, &currTarget, NULL, MODE_TRACK, passiveSide.c_str(), off, hs, NULL, true); // send rotation angle and mode 21-Nov-13, force angle to 0 18-Sep-14
          tmp = execute(bResult, "PresetTelescope");
        }

      else
        {
          Result * bResult = anIIF->PresetTelescope(rotangle, rot, &currTarget, tgs, mode, passiveSide.c_str(), off, hs, ref, true);
          tmp = execute(bResult, "PresetTelescope");
        }

      /* even if this command fails, the PCS has been given the target 18-Apr-17 */
    }

  return execute(aResult, "PresetTelescope");
}

//! BinocularControl conditions the TCS for various binocular operations.
result
IIFServerI::BinocularControl(string flag, const Ice::Current &)
{
  if (iif.ICE.logging)
    {
      Ice::Print info(__comm->getLogger());
      stringstream oss;
      oss << "[" << flag << "]";
      info << "{proxy: " << proxyInfo << ", cmd: " << "BinocularControl" << ", args: " << oss.str() << "}";
    }

  binocularType bin = StrToBinocular(flag);

  if (bin < 0)
    {
      result tmp;
      tmp.rescode = EXIT_FAILURE;
      tmp.resmsg.push_back("BinocularControl result status: Error");
      tmp.resmsg.push_back("BinocularControl Error: flag invalid: " + flag);
      Ice::Error err(__comm->getLogger());
      err << proxyInfo << " BinocularControl Error: flag invalid: " + flag;
      return tmp;
    }

  Result * aResult = anIIF->BinocularControl(bin);

  return execute(aResult, "BinocularControl");
}

//! Reads a block of entries from the data dictionary in one shot.
result
IIFServerI::GetParameter(SeqDD list, const Ice::Current &)
{
  if (iif.ICE.DDlogging)
    {
      Ice::Print info(__comm->getLogger());
      stringstream oss;
      oss << "[";
      for (auto const & e : list) { oss << e.DDname << ", "; }
      oss << "]";
      info << "{proxy: " << proxyInfo << ", cmd: " << "GetParameter" << ", args: " << oss.str() << "}";
    }

  MultiDDEntry multiEntries;

  for (int i = 0; i < (int)list.size(); i++) //fill the IIF::multiEntries object
    {
      multiEntries.PushEntry(list[i].DDname);
    }

  Result * aResult = anIIF->GetMultiParameter(multiEntries);

  return execute(aResult, "GetParameter");
}

//! Sets the values of the specified data dictionary entries on the TCS in one shot.
result
IIFServerI::SetParameter(SeqDD list, const Ice::Current &)
{
  if (iif.ICE.DDlogging)
    {
      Ice::Print info(__comm->getLogger());
      stringstream oss;
      // key-value
      oss << "[{";
      for (auto const & e : list) { oss << e.DDname << ": '" << e.DDkey << "', "; }
      oss << "}]";
      info << "{proxy: " << proxyInfo << ", cmd: " << "SetParameter" << ", args: " << oss.str() << "}";
    }

  MultiDDEntry multiEntries;

  for (int i = 0; i < (int)list.size(); i++) //fill the IIF::multiEntries object
    {
      multiEntries.PushEntry(list[i].DDname, list[i].DDkey);
    }

  Result * aResult = anIIF->SetMultiParameter(multiEntries);

  return execute(aResult, "SetParameter");
}

//! Sends an array of 28 Zernike polynomial coefficients to be compensated by the actuators
//! of the primary or secondary mirror.
result
IIFServerI::SendWavefront(SeqWF wfs, string opetype, string side, const Ice::Current &)
{
  if (iif.ICE.logging)
    {
      Ice::Print info(__comm->getLogger());
      stringstream oss;
      oss << "[" << "wfs_SeqWF_no_translation";
      oss << ", " << opetype;
      oss << ", " << side;
      oss << "]";
      info << "{proxy: " << proxyInfo << ", cmd: " << "SendWavefront" << ", args: " << oss.str() << "}";
    }

  bool errorCheck = Configuration::getParameter("IIFIceErrorChecking", true); // 13-Dec-13
//  if(anIIF->getInstrument() == "LUCIFER") errorCheck = false; // 13-Dec-13, disabled 30-Mar-16

  opeType ope = StrToOpe(opetype, errorCheck);

  if (ope < 0)
    {
      result tmp;
      tmp.rescode = EXIT_FAILURE;
      tmp.resmsg.push_back("SendWavefront result status: Error");
      tmp.resmsg.push_back("SendWavefront Error: flag invalid: " + opetype);
      Ice::Error err(__comm->getLogger());
      err << proxyInfo << " SendWavefront Error: flag invalid: " + opetype;
      return tmp;
    }

  WaveFront * wf = new WaveFront(); // creates dummy WaveFront instance with all 0.0 coefficients

  for (int i = 0; i < (int)wfs.size(); i++) { wf->setCoefficient(wfs[i], i); }

  Result * aResult = anIIF->SendWavefront(wf, ope, side.c_str());
  delete wf;

  return execute(aResult, "SendWavefront");
}

//! Updates the target effective wavelength without issuing a new preset.
result
IIFServerI::UpdateTargetWavelength(float wavelength, string side, const Ice::Current &)
{
  if (iif.ICE.logging)
    {
      Ice::Print info(__comm->getLogger());
      stringstream oss;
      oss << "[" << wavelength;
      oss << ", " << side;
      oss << "]";
      info << "{proxy: " << proxyInfo << ", cmd: " << "UpdateTargetWavelength" << ", args: " << oss.str() << "}";
    }

  Result * aResult = anIIF->UpdateTargetWavelength(wavelength, side.c_str());

  return execute(aResult, "UpdateTargetWavelength");
}

//! Sets the target and guide stars.
result
IIFServerI::SetStars(SeqPos list, const Ice::Current &)
{
  /* generate SeqPos2 */
  SeqPos2 stars(list.size());

  for (int i = 0; i < (int)list.size(); i++)
    {
      stars[i].coord1 = list[i].coord1;
      stars[i].coord2 = list[i].coord2;
      stars[i].system = list[i].system;
      stars[i].epoch = list[i].epoch;
      stars[i].equinox = list[i].equinox;
      stars[i].pmcoord1 = list[i].pmcoord1;
      stars[i].pmcoord2 = list[i].pmcoord2;
      stars[i].apparentMagnitude = list[i].apparentMagnitude;
      stars[i].filter = list[i].filter;
      stars[i].color = list[i].color;
      stars[i].colorType = list[i].colorType;
      stars[i].wavelength = list[i].wavelength;
      stars[i].type = "SIDEREAL";
      stars[i].file = "";
      stars[i].time = 0.0;
      stars[i].RARate = 0.0;
      stars[i].DECRate = 0.0;
    }

  return SetStars(stars, "SetStars");
}

//! Sets the target and guide stars.
//! Deprecated; use SetStars2 25-Mar-16
result
IIFServerI::SetStarsNew(SeqNewPos list, const Ice::Current &)
{
  /* generate SeqPos2 */
  SeqPos2 stars(list.size());

  for (int i = 0; i < (int)list.size(); i++)
    {
      stars[i].coord1 = list[i].coord1;
      stars[i].coord2 = list[i].coord2;
      stars[i].system = list[i].system;
      stars[i].epoch = list[i].epoch;
      stars[i].equinox = list[i].equinox;
      stars[i].pmcoord1 = list[i].pmcoord1;
      stars[i].pmcoord2 = list[i].pmcoord2;
      stars[i].apparentMagnitude = list[i].apparentMagnitude;
      stars[i].filter = list[i].filter;
      stars[i].color = list[i].color;
      stars[i].colorType = list[i].colorType;
      stars[i].wavelength = list[i].wavelength;
      stars[i].type = list[i].type;
      stars[i].file = list[i].file;
      stars[i].time = list[i].time;
      stars[i].RARate = list[i].RARate;
      stars[i].DECRate = list[i].DECRate;
    }

  return SetStars(stars, "SetStarsNew");
}

//! Sets the target and guide stars.
result
IIFServerI::SetStars2(SeqPos2 list, const Ice::Current &)
{
  return SetStars(list, "SetStars2");
}

//! 25-Mar-16
//! The real SetStars method.
result
IIFServerI::SetStars(SeqPos2 list, string cmd)
{
  if (iif.ICE.logging)
    {
      Ice::Print info(__comm->getLogger());
      stringstream oss;
      oss << "[[";
      for (auto const & e : list) { oss << "[" << e.type << ", " << e.coord1 << ", " << e.coord2 << ", " << e.system << ", " << e.epoch << ", " << e.equinox << ", " << e.pmcoord1 << ", " << e.pmcoord2 << ", " << e.apparentMagnitude << ", '" << e.filter << "', " << e.color << ", '" << e.colorType << "', " << e.wavelength << ", '" << e.file << "', " << e.time << ", " << e.RARate << ", " << e.DECRate << "], "; }
      oss << "]]";
      info << "{proxy: " << proxyInfo << ", cmd: " << cmd << ", args: " << oss.str() << "}";
    }

  result tmp;
  bool bad = false;

  if (list.size() == 0)
    {
      tmp.rescode = EXIT_FAILURE;
      tmp.resmsg.push_back(cmd + " result status: Error");
      tmp.resmsg.push_back(cmd + " Error: List of stars is empty");
      Ice::Error err(__comm->getLogger());
      err << proxyInfo << " " << cmd << " Error: List of stars is empty";
    }

  else
    {
      bool errorCheck = Configuration::getParameter("IIFIceErrorChecking", true); // 13-Dec-13
//    if(anIIF->getInstrument() == "LUCIFER") errorCheck = false; // 13-Dec-13, disabled 30-Mar-16

      nsType type = StrToNs(list[0].type);

      if (type < 0 || type == NS_TARGET)  // target cannot be type NS_TARGET
        {
          tmp.rescode = EXIT_FAILURE;
          tmp.resmsg.push_back(cmd + " result status: Error");
          tmp.resmsg.push_back(cmd + " Error: type invalid: " + list[0].type);
          Ice::Error err(__comm->getLogger());
          err << proxyInfo << " " << cmd << " Error: type invalid: " + list[0].type;
          bad = true;
        }

      coordType coord = StrToCoord(list[0].system, errorCheck);

      if (coord < 0)
        {
          tmp.rescode = EXIT_FAILURE;

          if (!bad) { tmp.resmsg.push_back(cmd + " result status: Error"); }

          tmp.resmsg.push_back(cmd + " Error: target coordinate system invalid: " + list[0].system);
          Ice::Error err(__comm->getLogger());
          err << proxyInfo << " " << cmd << " Error: target coordinate system invalid: " + list[0].system;
          bad = true;
        }

      equinoxType eq = StrToEquinox(list[0].equinox, errorCheck);

      if (eq < 0)
        {
          tmp.rescode = EXIT_FAILURE;

          if (!bad) { tmp.resmsg.push_back(cmd + " result status: Error"); }

          tmp.resmsg.push_back(cmd + " Error: target equinox invalid: " + list[0].equinox);
          Ice::Error err(__comm->getLogger());
          err << proxyInfo << " " << cmd << " Error: target equinox invalid: " + list[0].equinox;
          bad = true;
        }

      filterType filter = StrToFilter(list[0].filter, errorCheck);

      if (filter < 0)
        {
          tmp.rescode = EXIT_FAILURE;

          if (!bad) { tmp.resmsg.push_back(cmd + " result status: Error"); }

          tmp.resmsg.push_back(cmd + " Error: target filter type invalid: " + list[0].filter);
          Ice::Error err(__comm->getLogger());
          err << proxyInfo << " " << cmd << " Error: target filter type invalid: " + list[0].filter;
          bad = true;
        }

      colorType color = StrToColor(list[0].colorType, errorCheck);

      if (color < 0)
        {
          tmp.rescode = EXIT_FAILURE;

          if (!bad) { tmp.resmsg.push_back(cmd + " result status: Error"); }

          tmp.resmsg.push_back(cmd + " Error: target color type invalid: " + list[0].colorType);
          Ice::Error err(__comm->getLogger());
          err << proxyInfo << " " << cmd << " Error: target color type invalid: " + list[0].colorType;
          bad = true;
        }

      if (bad) { return tmp; }

      guidestars.resize(list.size());

      //read the target, which is the first position object(index 0).
      target.Settype(type);
      target.Setcoord1(list[0].coord1);
      target.Setcoord2(list[0].coord2);
      target.Setcoordsystem(coord);
      target.Setepoch(list[0].epoch);
      target.Setequinox(eq);
      ProperMotionType propm = {list[0].pmcoord1, list[0].pmcoord2};
      MagnitudeType magnitude = {list[0].apparentMagnitude, filter, list[0].color, color};
      target.Setmagnitude(&magnitude);
      target.Setpropermotion(&propm);
      target.Setwavelength(list[0].wavelength);
      target.Setfile(list[0].file);
      target.Settime(list[0].time);
      target.SetRARate(list[0].RARate);
      target.SetDECRate(list[0].DECRate);
      btarget = true;

      for (int i = 1; i < (int)list.size(); i++)
        {
          ostringstream oss;
          oss.str("");
          oss << i;
          nsType type = StrToNs(list[i].type);

          if (type < 0 || type == NS_TARGET)  // guide star cannot be type NS_TARGET 16-Aug-16
            {
              tmp.rescode = EXIT_FAILURE;
              tmp.resmsg.push_back(cmd + " result status: Error");
              tmp.resmsg.push_back(cmd + " Error: star " + oss.str() + " type invalid: " + list[i].type);
              Ice::Error err(__comm->getLogger());
              err << proxyInfo << " " << cmd << " Error: star " + oss.str() + " type invalid: " + list[i].type;
              bad = true;
            }

          coordType coord = StrToCoord(list[i].system, errorCheck);

          if (coord < 0)
            {
              tmp.rescode = EXIT_FAILURE;

              if (!bad) { tmp.resmsg.push_back(cmd + " result status: Error"); }

              tmp.resmsg.push_back(cmd + " Error: guide star " + oss.str() + " coordinate system invalid: " + list[i].system);
              Ice::Error err(__comm->getLogger());
              err << proxyInfo << " " << cmd << " Error: guide star " + oss.str() + " coordinate system invalid: " + list[i].system;
              bad = true;
            }

          equinoxType eq = StrToEquinox(list[i].equinox, errorCheck);

          if (eq < 0)
            {
              tmp.rescode = EXIT_FAILURE;

              if (!bad) { tmp.resmsg.push_back(cmd + " result status: Error"); }

              tmp.resmsg.push_back(cmd + " Error: guide star " + oss.str() + " equinox invalid: " + list[i].equinox);
              Ice::Error err(__comm->getLogger());
              err << proxyInfo << " " << cmd << " Error: guide star " + oss.str() + " equinox invalid: " + list[i].equinox;
              bad = true;
            }

          filterType filter = StrToFilter(list[i].filter, errorCheck);

          if (filter < 0)
            {
              tmp.rescode = EXIT_FAILURE;

              if (!bad) { tmp.resmsg.push_back(cmd + " result status: Error"); }

              tmp.resmsg.push_back(cmd + " Error: guide star " + oss.str() + " filter type invalid: " + list[i].filter);
              Ice::Error err(__comm->getLogger());
              err << proxyInfo << " " << cmd << " Error: guide star " + oss.str() + " filter type invalid: " + list[i].filter;
              bad = true;
            }

          colorType color = StrToColor(list[i].colorType, errorCheck);

          if (color < 0)
            {
              tmp.rescode = EXIT_FAILURE;

              if (!bad) { tmp.resmsg.push_back(cmd + " result status: Error"); }

              tmp.resmsg.push_back(cmd + " Error: guide star " + oss.str() + " color type invalid: " + list[i].colorType);
              Ice::Error err(__comm->getLogger());
              err << proxyInfo << " " << cmd << " Error: guide star " + oss.str() + " color type invalid: " + list[i].colorType;
              bad = true;
            }

          if (bad) { return tmp; }

          guidestars[i - 1] = new Position();

          guidestars[i - 1]->Settype(type);
          guidestars[i - 1]->Setcoord1(list[i].coord1);
          guidestars[i - 1]->Setcoord2(list[i].coord2);
          guidestars[i - 1]->Setcoordsystem(coord);
          guidestars[i - 1]->Setepoch(list[i].epoch);
          guidestars[i - 1]->Setequinox(eq);
          // deal with the proper motion
          propm.coord1 = list[i].pmcoord1;
          propm.coord2 = list[i].pmcoord2;
          // Deal with the Apparent Magnitude
          magnitude.apparentMagnitude = list[i].apparentMagnitude;
          magnitude.filter = filter;
          magnitude.color = list[i].color;
          magnitude.colorType = color;
          guidestars[i - 1]->Setmagnitude(&magnitude);
          guidestars[i - 1]->Setpropermotion(&propm);
          guidestars[i - 1]->Setwavelength(list[i].wavelength);
          guidestars[i - 1]->Setfile(list[i].file);
          guidestars[i - 1]->Settime(list[i].time);
          guidestars[i - 1]->SetRARate(list[i].RARate);
          guidestars[i - 1]->SetDECRate(list[i].DECRate);
        }

      guidestars[list.size() - 1] = (Position *) NULL;
      bguidestars = true;
      tmp.rescode = EXIT_SUCCESS;
      tmp.resmsg.push_back(cmd + " result status: OK");

      if (iif.ICE.logging)
        {
          Ice::Print info(__comm->getLogger());
          info << cmd << " OK";
        }
    }

  return tmp;
}

//! Removes from the system the selected target and guide stars.
result
IIFServerI::ClearStars(const Ice::Current &)
{
  if (iif.ICE.logging)
    {
      Ice::Print info(__comm->getLogger());
      stringstream oss;
      oss << "[]";
      info << "{proxy: " << proxyInfo << ", cmd: " << "ClearStars" << ", args: " << oss.str() << "}";
    }

  result tmp;

  btarget = false;
  bguidestars = false;
  tmp.rescode = EXIT_SUCCESS;
  tmp.resmsg.push_back("ClearStars result status: OK");

  if (iif.ICE.logging)
    {
      Ice::Print info(__comm->getLogger());
      info << "ClearStars OK";
    }

  return tmp;
}

//! Readies the cable wrap and rotator.
result
IIFServerI::RotReady(bool enable, string side, const Ice::Current &)
{
  if (iif.ICE.logging)
    {
      Ice::Print info(__comm->getLogger());
      stringstream oss;
      oss << "[" << enable;
      oss << ", " << side;
      oss << "]";
      info << "{proxy: " << proxyInfo << ", cmd: " << "RotReady" << ", args: " << oss.str() << "}";
    }

  Result * aResult = anIIF->RotReady(enable, side.c_str());

  return execute(aResult, "RotReady");
}

//! If the rotator is tracking or slewing, this command makes it stop moving and hold the position.
result
IIFServerI::RotHold(string side, const Ice::Current &)
{
  if (iif.ICE.logging)
    {
      Ice::Print info(__comm->getLogger());
      stringstream oss;
      oss << "[" << side << "]";
      info << "{proxy: " << proxyInfo << ", cmd: " << "RotHold" << ", args: " << oss.str() << "}";
    }

  Result * aResult = anIIF->RotHold(side.c_str());

  return execute(aResult, "RotHold");
}

//! Makes rotator begin tracking according to the polynomial stream it is currently receiving from the PCS.
result
IIFServerI::RotTrack(string side, const Ice::Current &)
{
  if (iif.ICE.logging)
    {
      Ice::Print info(__comm->getLogger());
      stringstream oss;
      oss << "[" << side << "]";
      info << "{proxy: " << proxyInfo << ", cmd: " << "RotTrack" << ", args: " << oss.str() << "}";
    }

  Result * aResult = anIIF->RotTrack(side.c_str());

  return execute(aResult, "RotTrack");
}

//! This command is to provide some control over the use of the rotator cablewrap.
result
IIFServerI::MaximizeWrapTime(bool azflag, bool rotflag, string side, const Ice::Current &)
{
  if (iif.ICE.logging)
    {
      Ice::Print info(__comm->getLogger());
      stringstream oss;
      oss << "[" << azflag;
      oss << ", " << rotflag;
      oss << ", " << side;
      oss << "]";
      info << "{proxy: " << proxyInfo << ", cmd: " << "MaximizeWrapTime" << ", args: " << oss.str() << "}";
    }

  Result * aResult = anIIF->MaximizeWrapTime(azflag, rotflag, side.c_str());

  return execute(aResult, "MaximizeWrapTime");
}

//! Makes the rotator move to the specified angle in the specified coordinate frame and hold at that position.
result
IIFServerI::RotServicePosition(double angle, string side, const Ice::Current &)
{
  if (iif.ICE.logging)
    {
      Ice::Print info(__comm->getLogger());
      stringstream oss;
      oss << "[" << angle;
      oss << ", " << side;
      oss << "]";
      info << "{proxy: " << proxyInfo << ", cmd: " << "RotServicePosition" << ", args: " << oss.str() << "}";
    }

  Result * aResult = anIIF->RotServicePosition(angle, side.c_str());

  return execute(aResult, "RotServicePosition");
}

//! Temporarily suspends the current guiding operation.
result
IIFServerI::PauseGuiding(string side, const Ice::Current &)
{
  if (iif.ICE.logging)
    {
      Ice::Print info(__comm->getLogger());
      stringstream oss;
      oss << "[" << side << "]";
      info << "{proxy: " << proxyInfo << ", cmd: " << "PauseGuiding" << ", args: " << oss.str() << "}";
    }

  Result * aResult = anIIF->PauseGuiding(side.c_str());

  return execute(aResult, "PauseGuiding");
}

//! Resumes suspended guiding operation.
result
IIFServerI::ResumeGuiding(string side, const Ice::Current &)
{
  if (iif.ICE.logging)
    {
      Ice::Print info(__comm->getLogger());
      stringstream oss;
      oss << "[" << side << "]";
      info << "{proxy: " << proxyInfo << ", cmd: " << "ResumeGuiding" << ", args: " << oss.str() << "}";
    }

  Result * aResult = anIIF->ResumeGuiding(side.c_str());

  return execute(aResult, "ResumeGuiding");
}

//! Stops the current guiding operation.
result
IIFServerI::StopGuiding(string side, const Ice::Current &)
{
  if (iif.ICE.logging)
    {
      Ice::Print info(__comm->getLogger());
      stringstream oss;
      oss << "[" << side << "]";
      info << "{proxy: " << proxyInfo << ", cmd: " << "StopGuiding" << ", args: " << oss.str() << "}";
    }

  Result * aResult = anIIF->StopGuiding(side.c_str());

  return execute(aResult, "StopGuiding");
}

//! Sets the guider hotspot.
result
IIFServerI::SetGuidingHotspot(double coord1, double coord2, string movetype, string side, const Ice::Current &)
{
  if (iif.ICE.logging)
    {
      Ice::Print info(__comm->getLogger());
      stringstream oss;
      oss << "[" << coord1;
      oss << ", " << coord2;
      oss << ", " << movetype;
      oss << ", " << side;
      oss << "]";
      info << "{proxy: " << proxyInfo << ", cmd: " << "SetGuidingHotspot" << ", args: " << oss.str() << "}";
    }

  bool errorCheck = Configuration::getParameter("IIFIceErrorChecking", true); // 13-Dec-13
//  if(anIIF->getInstrument() == "LUCIFER") errorCheck = false; // 13-Dec-13, disabled 30-Mar-16

  moveType move = StrToMove(movetype, errorCheck);

  if (move < 0)
    {
      result tmp;
      tmp.rescode = EXIT_FAILURE;
      tmp.resmsg.push_back("SetGuidingHotspot result status: Error");
      tmp.resmsg.push_back("SetGuidingHotspot Error: move type invalid: " + movetype);
      Ice::Error err(__comm->getLogger());
      err << proxyInfo << " SetGuidingHotspot Error: move type invalid: " + movetype;
      return tmp;
    }

  Result * aResult = anIIF->SetGuidingHotspot(coord1, coord2, move, side.c_str());

  return execute(aResult, "SetGuidingHotspot");
}

//! SetAGWFilter sets the AGw filter.
result
IIFServerI::SetAGWFilter(int filterNumber, string side, const Ice::Current &)
{
  if (iif.ICE.logging)
    {
      Ice::Print info(__comm->getLogger());
      stringstream oss;
      oss << "[" << filterNumber;
      oss << ", " << side;
      oss << "]";
      info << "{proxy: " << proxyInfo << ", cmd: " << "SetAGWFilter" << ", args: " << oss.str() << "}";
    }

  Result * aResult = anIIF->SetAGWFilter(filterNumber, side.c_str());

  return execute(aResult, "SetAGWFilter");
}

//! SetGuidingBinning sets the guider binning.
result
IIFServerI::SetGuidingBinning(int factor, string side, const Ice::Current &)
{
  if (iif.ICE.logging)
    {
      Ice::Print info(__comm->getLogger());
      stringstream oss;
      oss << "[" << factor;
      oss << ", " << side;
      oss << "]";
      info << "{proxy: " << proxyInfo << ", cmd: " << "SetGuidingBinning" << ", args: " << oss.str() << "}";
    }

  Result * aResult = anIIF->SetGuidingBinning(factor, side.c_str());

  return execute(aResult, "SetGuidingBinning");
}

//! Temporarily suspends the current AO operation.
result
IIFServerI::PauseAO(string side, const Ice::Current &)
{
  if (iif.ICE.logging)
    {
      Ice::Print info(__comm->getLogger());
      stringstream oss;
      oss << "[" << side << "]";
      info << "{proxy: " << proxyInfo << ", cmd: " << "PauseAO" << ", args: " << oss.str() << "}";
    }

  Result * aResult = anIIF->PauseAO(side.c_str());

  return execute(aResult, "PauseAO");
}

//! Resumes suspended operation after a PauseAO.
result
IIFServerI::ResumeAO(string side, const Ice::Current &)
{
  if (iif.ICE.logging)
    {
      Ice::Print info(__comm->getLogger());
      stringstream oss;
      oss << "[" << side << "]";
      info << "{proxy: " << proxyInfo << ", cmd: " << "ResumeAO" << ", args: " << oss.str() << "}";
    }

  Result * aResult = anIIF->ResumeAO(side.c_str());

  return execute(aResult, "ResumeAO");
}

//! Start AO operation.
result
IIFServerI::StartAO(string side, const Ice::Current &)
{
  if (iif.ICE.logging)
    {
      Ice::Print info(__comm->getLogger());
      stringstream oss;
      oss << "[" << side << "]";
      info << "{proxy: " << proxyInfo << ", cmd: " << "StartAO" << ", args: " << oss.str() << "}";
    }

  Result * aResult = anIIF->StartAO(side.c_str());

  return execute(aResult, "StartAO");
}

//! Stop AO operation.
result
IIFServerI::StopAO(string reason, string side, const Ice::Current &)
{
  if (iif.ICE.logging)
    {
      Ice::Print info(__comm->getLogger());
      stringstream oss;
      oss << "['" << reason << "'";
      oss << ", " << side;
      oss << "]";
      info << "{proxy: " << proxyInfo << ", cmd: " << "StopAO" << ", args: " << oss.str() << "}";
    }

  Result * aResult = anIIF->StopAO(reason, side.c_str());

  return execute(aResult, "StopAO");
}

//! Acquire the reference star.
result
IIFServerI::AcquireRefAO(bool repointFlag, string side, const Ice::Current &)
{
  if (iif.ICE.logging)
    {
      Ice::Print info(__comm->getLogger());
      stringstream oss;
      oss << "[" << repointFlag;
      oss << ", " << side;
      oss << "]";
      info << "{proxy: " << proxyInfo << ", cmd: " << "AcquireRefAO" << ", args: " << oss.str() << "}";
    }

  Result * aResult = anIIF->AcquireRefAO(repointFlag, side.c_str());

  return execute(aResult, "AcquireRefAO");
}

//! Check the reference star position.
result
IIFServerI::CheckRefAO(string side, const Ice::Current &)
{
  if (iif.ICE.logging)
    {
      Ice::Print info(__comm->getLogger());
      stringstream oss;
      oss << "[" << side << "]";
      info << "{proxy: " << proxyInfo << ", cmd: " << "CheckRefAO" << ", args: " << oss.str() << "}";
    }

  Result * aResult = anIIF->CheckRefAO(side.c_str());

  return execute(aResult, "CheckRefAO");
}

//! Refine the AO operation.
result
IIFServerI::RefineAO(string method, string side, const Ice::Current &)
{
  if (iif.ICE.logging)
    {
      Ice::Print info(__comm->getLogger());
      stringstream oss;
      oss << "['" << method << "'";
      oss << ", " << side;
      oss << "]";
      info << "{proxy: " << proxyInfo << ", cmd: " << "RefineAO" << ", args: " << oss.str() << "}";
    }

  Result * aResult = anIIF->RefineAO(method, side.c_str());

  return execute(aResult, "RefineAO");
}

//! Do AcquireRefAO, StartAO.
result
IIFServerI::RunAO(string type, string side, const Ice::Current &)
{
  if (iif.ICE.logging)
    {
      Ice::Print info(__comm->getLogger());
      stringstream oss;
      oss << "['" << type << "'";
      oss << ", " << side;
      oss << "]";
      info << "{proxy: " << proxyInfo << ", cmd: " << "RunAO" << ", args: " << oss.str() << "}";
    }

  raoType rao = StrToRao(type);

  if (rao < 0)
    {
      result tmp;
      tmp.rescode = EXIT_FAILURE;
      tmp.resmsg.push_back("RunAo result status: Error");
      tmp.resmsg.push_back("RunAo Error: type invalid: " + type);
      Ice::Error err(__comm->getLogger());
      err << proxyInfo << " RunAo Error: type invalid: " + type;
      return tmp;
    }

  Result * aResult = anIIF->RunAO(rao, side.c_str());

  return execute(aResult, "RunAO");
}

//! Request AO flat.
result
IIFServerI::PresetFlatAO(string flat, string side, const Ice::Current &)
{
  if (iif.ICE.logging)
    {
      Ice::Print info(__comm->getLogger());
      stringstream oss;
      oss << "['" << flat << "'";
      oss << ", " << side;
      oss << "]";
      info << "{proxy: " << proxyInfo << ", cmd: " << "PresetFlatAO" << ", args: " << oss.str() << "}";
    }

  Result * aResult = anIIF->PresetFlatAO(flat, side.c_str());

  return execute(aResult, "PresetFlatAO");
}

//! Correct AO modes.
result
IIFServerI::CorrectModesAO(SeqModes modes, string side, const Ice::Current &)
{
  if (iif.ICE.logging)
    {
      Ice::Print info(__comm->getLogger());
      stringstream oss;
      oss << "[" << "modes_SeqModes_no_translation";
      oss << ", " << side;
      oss << "]";
      info << "{proxy: " << proxyInfo << ", cmd: " << "CorrectModesAO" << ", args: " << oss.str() << "}";
    }

  double __modes[672];

  memset(__modes, 0, sizeof(__modes));

  int n = modes.size() > 672 ? 672 : modes.size();

  for (int i = 0; i < n; i++) { __modes[i] = modes[i]; }

  Result * aResult = anIIF->CorrectModesAO(__modes, side.c_str());

  return execute(aResult, "CorrectModesAO");
}

//! Apply Waffle AO.
result
IIFServerI::ApplyWaffleAO(SeqModes modes, string side, const Ice::Current &)
{
  if (iif.ICE.logging)
    {
      Ice::Print info(__comm->getLogger());
      stringstream oss;
      oss << "[" << "modes_SeqModes_no_translation";
      oss << ", " << side;
      oss << "]";
      info << "{proxy: " << proxyInfo << ", cmd: " << "ApplyWaffleAO" << ", args: " << oss.str() << "}";
    }

  double __modes[672];

  memset(__modes, 0, sizeof(__modes));

  int n = modes.size() > 672 ? 672 : modes.size();

  for (int i = 0; i < n; i++) { __modes[i] = modes[i]; }

  Result * aResult = anIIF->ApplyWaffleAO(__modes, side.c_str());

  return execute(aResult, "ApplyWaffleAO");
}

//! Modify the AO operation.
result
IIFServerI::ModifyAO(int NModes, double Freq, int Nbins, double TTMod, string F1spec, string F2spec, string side, const Ice::Current &)
{
  if (iif.ICE.logging)
    {
      Ice::Print info(__comm->getLogger());
      stringstream oss;
      oss << "[" << NModes;
      oss << ", " << Freq;
      oss << ", " << Nbins;
      oss << ", " << TTMod;
      oss << ", '" << F1spec << "'";
      oss << ", '" << F2spec << "'";
      oss << ", " << side;
      oss << "]";
      info << "{proxy: " << proxyInfo << ", cmd: " << "ModifyAO" << ", args: " << oss.str() << "}";
    }

  Result * aResult = anIIF->ModifyAO(NModes, Freq, Nbins, TTMod, F1spec, F2spec, side.c_str());

  return execute(aResult, "ModifyAO");
}

//! Preset the AO operation.
result
IIFServerI::PresetAO(string AOMode, string side, const Ice::Current &)
{
  if (iif.ICE.logging)
    {
      Ice::Print info(__comm->getLogger());
      stringstream oss;
      oss << "['" << AOMode << "'";
      oss << ", " << side;
      oss << "]";
      info << "{proxy: " << proxyInfo << ", cmd: " << "PresetAO" << ", args: " << oss.str() << "}";
    }

  if (breference) //if the reference has been defined, call PresetAO
    {
      Result * aResult = anIIF->PresetAO(AOMode, &reference, side.c_str());

      return execute(aResult, "PresetAO");
    }

  else
    {
      result t;
      t.resmsg.push_back("PresetAO result status: Error");
      t.resmsg.push_back("PresetAO Error: No reference defined");
      t.rescode = EXIT_FAILURE;
      Ice::Error err(__comm->getLogger());
      err << proxyInfo << " PresetAO Error: No reference defined";
      return t;
    }
}

//! Offset the AO stage in XY.
result
IIFServerI::OffsetXYAO(double DeltaX, double DeltaY, string side, const Ice::Current &)
{
  if (iif.ICE.logging)
    {
      Ice::Print info(__comm->getLogger());
      stringstream oss;
      oss << "[" << DeltaX;
      oss << ", " << DeltaY;
      oss << ", " << side;
      oss << "]";
      info << "{proxy: " << proxyInfo << ", cmd: " << "OffsetXYAO" << ", args: " << oss.str() << "}";
    }

  Result * aResult = anIIF->OffsetXYAO(DeltaX, DeltaY, side.c_str());

  return execute(aResult, "OffsetXYAO");
}

//! Offset the AO stage in Z.
result
IIFServerI::OffsetZAO(double DeltaZ, string side, const Ice::Current &)
{
  if (iif.ICE.logging)
    {
      Ice::Print info(__comm->getLogger());
      stringstream oss;
      oss << "[" << DeltaZ;
      oss << ", " << side;
      oss << "]";
      info << "{proxy: " << proxyInfo << ", cmd: " << "OffsetZAO" << ", args: " << oss.str() << "}";
    }

  Result * aResult = anIIF->OffsetZAO(DeltaZ, side.c_str());

  return execute(aResult, "OffsetZAO");
}

//! Sets the AO reference star(s).
result
IIFServerI::SetReference(SeqPos list, const Ice::Current &)
{
  /* generate SeqPos2 */
  SeqPos2 stars(list.size());

  for (int i = 0; i < (int)list.size(); i++)
    {
      stars[i].coord1 = list[i].coord1;
      stars[i].coord2 = list[i].coord2;
      stars[i].system = list[i].system;
      stars[i].epoch = list[i].epoch;
      stars[i].equinox = list[i].equinox;
      stars[i].pmcoord1 = list[i].pmcoord1;
      stars[i].pmcoord2 = list[i].pmcoord2;
      stars[i].apparentMagnitude = list[i].apparentMagnitude;
      stars[i].filter = list[i].filter;
      stars[i].color = list[i].color;
      stars[i].colorType = list[i].colorType;
      stars[i].wavelength = list[i].wavelength;
      stars[i].type = "SIDEREAL";
      stars[i].file = "";
      stars[i].time = 0.0;
      stars[i].RARate = 0.0;
      stars[i].DECRate = 0.0;
    }

  return SetReference(stars, "SetReference");
}

//! Sets the AO reference star(s).
//! Deprecated; use SetReference2 25-Mar-16
result
IIFServerI::SetReferenceNew(SeqNewPos list, const Ice::Current &)
{
  /* generate SeqPos2 */
  SeqPos2 stars(list.size());

  for (int i = 0; i < (int)list.size(); i++)
    {
      stars[i].coord1 = list[i].coord1;
      stars[i].coord2 = list[i].coord2;
      stars[i].system = list[i].system;
      stars[i].epoch = list[i].epoch;
      stars[i].equinox = list[i].equinox;
      stars[i].pmcoord1 = list[i].pmcoord1;
      stars[i].pmcoord2 = list[i].pmcoord2;
      stars[i].apparentMagnitude = list[i].apparentMagnitude;
      stars[i].filter = list[i].filter;
      stars[i].color = list[i].color;
      stars[i].colorType = list[i].colorType;
      stars[i].wavelength = list[i].wavelength;
      stars[i].type = list[i].type;
      stars[i].file = list[i].file;
      stars[i].time = list[i].time;
      stars[i].RARate = list[i].RARate;
      stars[i].DECRate = list[i].DECRate;
    }

  return SetReference(stars, "SetReferenceNew");
}

//! Sets the AO reference star(s).
result
IIFServerI::SetReference2(SeqPos2 list, const Ice::Current &)
{
  return SetReference(list, "SetReference2");
}

//! 25-Mar-16
//! The real SetReference method.
result
IIFServerI::SetReference(SeqPos2 list, string cmd)
{
  if (iif.ICE.logging)
    {
      Ice::Print info(__comm->getLogger());
      stringstream oss;
      oss << "[[";
      for (auto const & e : list) { oss << "[" << e.type << ", " << e.coord1 << ", " << e.coord2 << ", " << e.system << ", " << e.epoch << ", " << e.equinox << ", " << e.pmcoord1 << ", " << e.pmcoord2 << ", " << e.apparentMagnitude << ", '" << e.filter << "', " << e.color << ", '" << e.colorType << "', " << e.wavelength << ", '" << e.file << "', " << e.time << ", " << e.RARate << ", " << e.DECRate << "], "; }
      oss << "]]";
      info << "{proxy: " << proxyInfo << ", cmd: " << cmd << ", args: " << oss.str() << "}";
    }

  result tmp;
  bool bad = false;

  if (list.size() == 0)
    {
      tmp.rescode = EXIT_FAILURE;
      tmp.resmsg.push_back(cmd + " result status: Error");
      tmp.resmsg.push_back(cmd + " Error: List of stars is empty");
      Ice::Error err(__comm->getLogger());
      err << proxyInfo << " " << cmd << " Error: List of stars is empty";
    }

  else
    {
      bool errorCheck = Configuration::getParameter("IIFIceErrorChecking", true); // 13-Dec-13
//    if(anIIF->getInstrument() == "LUCIFER") errorCheck = false; // 13-Dec-13, disabled 30-Mar-16

      nsType type = StrToNs(list[0].type);

      if (type < 0)  // allow any type 16-Aug-16
        {
          tmp.rescode = EXIT_FAILURE;
          tmp.resmsg.push_back(cmd + " result status: Error");
          tmp.resmsg.push_back(cmd + " Error: type invalid: " + list[0].type);
          Ice::Error err(__comm->getLogger());
          err << proxyInfo << " " << cmd << " Error: type invalid: " + list[0].type;
          bad = true;
        }

      coordType coord = StrToCoord(list[0].system, errorCheck);

      if (coord < 0)
        {
          tmp.rescode = EXIT_FAILURE;

          if (!bad) { tmp.resmsg.push_back(cmd + " result status: Error"); }

          tmp.resmsg.push_back(cmd + " Error: coordinate system invalid: " + list[0].system);
          Ice::Error err(__comm->getLogger());
          err << proxyInfo << " " << cmd << " Error: coordinate system invalid: " + list[0].system;
          bad = true;
        }

      equinoxType eq = StrToEquinox(list[0].equinox, errorCheck);

      if (eq < 0)
        {
          tmp.rescode = EXIT_FAILURE;

          if (!bad) { tmp.resmsg.push_back(cmd + " result status: Error"); }

          tmp.resmsg.push_back(cmd + " Error: equinox invalid: " + list[0].equinox);
          Ice::Error err(__comm->getLogger());
          err << proxyInfo << " " << cmd << " Error: equinox invalid: " + list[0].equinox;
          bad = true;
        }

      filterType filter = StrToFilter(list[0].filter, errorCheck);

      if (filter < 0)
        {
          tmp.rescode = EXIT_FAILURE;

          if (!bad) { tmp.resmsg.push_back(cmd + " result status: Error"); }

          tmp.resmsg.push_back(cmd + " Error: filter type invalid: " + list[0].filter);
          Ice::Error err(__comm->getLogger());
          err << proxyInfo << " " << cmd << " Error: filter type invalid: " + list[0].filter;
          bad = true;
        }

      colorType color = StrToColor(list[0].colorType, errorCheck);

      if (color < 0)
        {
          tmp.rescode = EXIT_FAILURE;

          if (!bad) { tmp.resmsg.push_back(cmd + " result status: Error"); }

          tmp.resmsg.push_back(cmd + " Error: color type invalid: " + list[0].colorType);
          Ice::Error err(__comm->getLogger());
          err << proxyInfo << " " << cmd << " Error: color type invalid: " + list[0].colorType;
          bad = true;
        }

      if (bad) { return tmp; }

      ProperMotionType propm;
      MagnitudeType magnitude;
      reference.Settype(type);
      reference.Setcoord1(list[0].coord1);
      reference.Setcoord2(list[0].coord2);
      reference.Setcoordsystem(coord);
      reference.Setepoch(list[0].epoch);
      reference.Setequinox(eq);
      propm.coord1 = list[0].pmcoord1;
      propm.coord2 = list[0].pmcoord2;
      magnitude.apparentMagnitude = list[0].apparentMagnitude;
      magnitude.filter = filter;
      magnitude.color = list[0].color;
      magnitude.colorType = color;
      reference.Setmagnitude(&magnitude);
      reference.Setpropermotion(&propm);
      reference.Setwavelength(list[0].wavelength);
      reference.Setfile(list[0].file);
      reference.Settime(list[0].time);
      reference.SetRARate(list[0].RARate);
      reference.SetDECRate(list[0].DECRate);
      LNReference.resize(list.size());

      for (int i = 0; i < (int)list.size(); i++)
        {
          ostringstream oss;
          oss.str("");
          oss << i;
          nsType type = StrToNs(list[i].type);

          if (type < 0)
            {
              tmp.rescode = EXIT_FAILURE;
              tmp.resmsg.push_back(cmd + " result status: Error");
              tmp.resmsg.push_back(cmd + " Error: star " + oss.str() + " type invalid: " + list[i].type);
              Ice::Error err(__comm->getLogger());
              err << proxyInfo << " " << cmd << " Error: star " + oss.str() + " type invalid: " + list[i].type;
              bad = true;
            }

          coordType coord = StrToCoord(list[i].system, errorCheck);

          if (coord < 0)
            {
              tmp.rescode = EXIT_FAILURE;

              if (!bad) { tmp.resmsg.push_back(cmd + " result status: Error"); }

              tmp.resmsg.push_back(cmd + " Error: star " + oss.str() + " coordinate system invalid: " + list[i].system);
              Ice::Error err(__comm->getLogger());
              err << proxyInfo << " " << cmd << " Error: star " + oss.str() + " coordinate system invalid: " + list[i].system;
              bad = true;
            }

          equinoxType eq = StrToEquinox(list[i].equinox, errorCheck);

          if (eq < 0)
            {
              tmp.rescode = EXIT_FAILURE;

              if (!bad) { tmp.resmsg.push_back(cmd + " result status: Error"); }

              tmp.resmsg.push_back(cmd + " Error: star " + oss.str() + " equinox invalid: " + list[i].equinox);
              Ice::Error err(__comm->getLogger());
              err << proxyInfo << " " << cmd << " Error: star " + oss.str() + " equinox invalid: " + list[i].equinox;
              bad = true;
            }

          filterType filter = StrToFilter(list[i].filter, errorCheck);

          if (filter < 0)
            {
              tmp.rescode = EXIT_FAILURE;

              if (!bad) { tmp.resmsg.push_back(cmd + " result status: Error"); }

              tmp.resmsg.push_back(cmd + " Error: star " + oss.str() + " filter type invalid: " + list[i].filter);
              Ice::Error err(__comm->getLogger());
              err << proxyInfo << " " << cmd << " Error: star " + oss.str() + " filter type invalid: " + list[i].filter;
              bad = true;
            }

          colorType color = StrToColor(list[i].colorType, errorCheck);

          if (color < 0)
            {
              tmp.rescode = EXIT_FAILURE;

              if (!bad) { tmp.resmsg.push_back(cmd + " result status: Error"); }

              tmp.resmsg.push_back(cmd + " Error: star " + oss.str() + " color type invalid: " + list[i].colorType);
              Ice::Error err(__comm->getLogger());
              err << proxyInfo << " " << cmd << " Error: star " + oss.str() + " color type invalid: " + list[i].colorType;
              bad = true;
            }

          if (bad) { return tmp; }

          LNReference[i].Settype(type);
          LNReference[i].Setcoord1(list[i].coord1);
          LNReference[i].Setcoord2(list[i].coord2);
          LNReference[i].Setcoordsystem(coord);
          LNReference[i].Setepoch(list[i].epoch);
          LNReference[i].Setequinox(eq);
          propm.coord1 = list[i].pmcoord1;
          propm.coord2 = list[i].pmcoord2;
          LNReference[i].Setpropermotion(&propm);
          magnitude.apparentMagnitude = list[i].apparentMagnitude;
          magnitude.filter = filter;
          magnitude.color = list[i].color;
          magnitude.colorType = color;
          LNReference[i].Setmagnitude(&magnitude);
          LNReference[i].Setwavelength(list[i].wavelength);
          LNReference[i].Setfile(list[i].file);
          LNReference[i].Settime(list[i].time);
          LNReference[i].SetRARate(list[i].RARate);
          LNReference[i].SetDECRate(list[i].DECRate);
        }

      breference = true;

      tmp.rescode = EXIT_SUCCESS;
      tmp.resmsg.push_back(cmd + " result status: OK");

      if (iif.ICE.logging)
        {
          Ice::Print info(__comm->getLogger());
          info << cmd << " OK";
        }
    }

  return tmp;
}

//! Removes from the system the AO reference star.
result
IIFServerI::ClearReference(const Ice::Current &)
{
  if (iif.ICE.logging)
    {
      Ice::Print info(__comm->getLogger());
      stringstream oss;
      oss << "[]";
      info << "{proxy: " << proxyInfo << ", cmd: " << "ClearReference" << ", args: " << oss.str() << "}";
    }

  result tmp;

  breference = false;
  tmp.rescode = EXIT_SUCCESS;
  tmp.resmsg.push_back("ClearReference result status: OK");

  if (iif.ICE.logging)
    {
      Ice::Print info(__comm->getLogger());
      info << "ClearReference OK";
    }

  return tmp;
}

//! This command allows the user to use the current sky or focal plane location achieved via the
//! OffsetPointing command and update the previous values;
result
IIFServerI::UpdatePointingReference(string offset_coord, string side, const Ice::Current &)
{
  if (iif.ICE.logging)
    {
      Ice::Print info(__comm->getLogger());
      stringstream oss;
      oss << "[" << offset_coord;
      oss << ", " << side;
      oss << "]";
      info << "{proxy: " << proxyInfo << ", cmd: " << "UpdatePointingReference" << ", args: " << oss.str() << "}";
    }

  bool errorCheck = Configuration::getParameter("IIFIceErrorChecking", true); // 13-Dec-13
//  if(anIIF->getInstrument() == "LUCIFER") errorCheck = false; // 13-Dec-13, disabled 30-Mar-16

  offsetType off = StrToOffset(offset_coord, errorCheck);

  if (off < 0)
    {
      result tmp;
      tmp.rescode = EXIT_FAILURE;
      tmp.resmsg.push_back("UpdatePointing result status: Error");
      tmp.resmsg.push_back("UpdatePointing Error: coordinate system invalid: " + offset_coord);
      Ice::Error err(__comm->getLogger());
      err << proxyInfo << " UpdatePointing Error: coordinate system invalid: " + offset_coord;
      return tmp;
    }

  Result * aResult = anIIF->UpdatePointingReference(off, side.c_str());

  return execute(aResult, "UpdatePointingReference");
}

//! Use this command to log a message inside the TCS's logging system.
result
IIFServerI::LogEvent(string event, string description, const Ice::Current &)
{
  if (iif.ICE.logging)
    {
      Ice::Print info(__comm->getLogger());
      stringstream oss;
      oss << "['" << event  << "'";
      oss << ", '" << description  << "'";
      oss << "]";      
      info << "{proxy: " << proxyInfo << ", cmd: " << "LogEvent" << ", args: " << oss.str() << "}";
    }

  Result * aResult = anIIF->LogEvent(event.c_str(), description.c_str());

  return execute(aResult, "LogEvent");
}

//! Use this command to get the LBC rotator trajectory.
result
IIFServerI::GetRotatorTrajectory(double seconds, double interval, double start, string side, const Ice::Current &)
{
  if (iif.ICE.logging)
    {
      Ice::Print info(__comm->getLogger());
      stringstream oss;
      oss << "[" << seconds;
      oss << ", " << interval;
      oss << ", " << start;
      oss << ", " << side;
      oss << "]";
      info << "{proxy: " << proxyInfo << ", cmd: " << "GetRotatorTrajectory" << ", args: " << oss.str() << "}";
    }

  Result * aResult = anIIF->GetRotatorTrajectory(seconds, interval, start, side.c_str());

  return execute(aResult, "GetRotatorTrajectory");
}

//! Use this command to get a rotator trajectory.
result
IIFServerI::GetRotatorPolynomials(double start, int count, double interval, int order, string side, const Ice::Current &)
{
  if (iif.ICE.logging)
    {
      Ice::Print info(__comm->getLogger());
      stringstream oss;
      oss << "[" << start;
      oss << ", " << count;
      oss << ", " << interval;
      oss << ", " << order;
      oss << ", " << side;
      oss << "]";
      info << "{proxy: " << proxyInfo << ", cmd: " << "GetRotatorPolynomials" << ", args: " << oss.str() << "}";
    }

  __order = order; // save the order for later
  Result * aResult = anIIF->GetRotatorPolynomials(start, count, interval, order, side.c_str());

  return execute(aResult, "GetRotatorPolynomials");
}

//! Use this command to set the standby level.
result
IIFServerI::Standby(int level, string side, const Ice::Current &)
{
  if (iif.ICE.logging)
    {
      Ice::Print info(__comm->getLogger());
      stringstream oss;
      oss << "[" << level;
      oss << ", " << side;
      oss << "]";
      info << "{proxy: " << proxyInfo << ", cmd: " << "Standby" << ", args: " << oss.str() << "}";
    }

  Result * aResult = anIIF->Standby(level, side.c_str());

  return execute(aResult, "Standby");
}

//! Use this command to rotate the primary about a point on axis.
result
IIFServerI::RotatePrimary(double distance, double angle, double direction, string side, const Ice::Current &)
{
  if (iif.ICE.logging)
    {
      Ice::Print info(__comm->getLogger());
      stringstream oss;
      oss << "[" << distance;
      oss << ", " << angle;
      oss << ", " << direction;
      oss << ", " << side;
      oss << "]";
      info << "{proxy: " << proxyInfo << ", cmd: " << "RotatePrimary" << ", args: " << oss.str() << "}";
    }

  Result * aResult = anIIF->RotatePrimary(distance, angle, direction, side.c_str());

  return execute(aResult, "RotatePrimary");
}

//! Gets focal plane coordinates for LN AO reference stars.
result
IIFServerI::GetKFPCoordinates(SeqPos list, string side, const Ice::Current &)
{
  /* generate SeqPos2 */
  SeqPos2 stars(list.size());

  for (int i = 0; i < (int)list.size(); i++)
    {
      stars[i].coord1 = list[i].coord1;
      stars[i].coord2 = list[i].coord2;
      stars[i].system = list[i].system;
      stars[i].epoch = list[i].epoch;
      stars[i].equinox = list[i].equinox;
      stars[i].pmcoord1 = list[i].pmcoord1;
      stars[i].pmcoord2 = list[i].pmcoord2;
      stars[i].apparentMagnitude = list[i].apparentMagnitude;
      stars[i].filter = list[i].filter;
      stars[i].color = list[i].color;
      stars[i].colorType = list[i].colorType;
      stars[i].wavelength = list[i].wavelength;
      stars[i].type = "SIDEREAL";
      stars[i].file = "";
      stars[i].time = 0.0;
      stars[i].RARate = 0.0;
      stars[i].DECRate = 0.0;
    }

  return GetKFPCoordinates(stars, side, "GetKFPCoordinates");
}

//! Gets focal plane coordinates for LN AO reference stars.
//! Deprecated; use GetKFPCoordinates2 25-Mar-16
result
IIFServerI::GetKFPCoordinatesNew(SeqNewPos list, string side, const Ice::Current &)
{
  /* generate SeqPos2 */
  SeqPos2 stars(list.size());

  for (int i = 0; i < (int)list.size(); i++)
    {
      stars[i].coord1 = list[i].coord1;
      stars[i].coord2 = list[i].coord2;
      stars[i].system = list[i].system;
      stars[i].epoch = list[i].epoch;
      stars[i].equinox = list[i].equinox;
      stars[i].pmcoord1 = list[i].pmcoord1;
      stars[i].pmcoord2 = list[i].pmcoord2;
      stars[i].apparentMagnitude = list[i].apparentMagnitude;
      stars[i].filter = list[i].filter;
      stars[i].color = list[i].color;
      stars[i].colorType = list[i].colorType;
      stars[i].wavelength = list[i].wavelength;
      stars[i].type = list[i].type;
      stars[i].file = list[i].file;
      stars[i].time = list[i].time;
      stars[i].RARate = list[i].RARate;
      stars[i].DECRate = list[i].DECRate;
    }

  return GetKFPCoordinates(stars, side, "GetKFPCoordinatesNew");
}

//! Gets focal plane coordinates for LN AO reference stars.
result
IIFServerI::GetKFPCoordinates2(SeqPos2 list, string side, const Ice::Current &)
{
  return GetKFPCoordinates(list, side, "GetKFPCoordinates2");
}

//! 25-Mar-16
//! The real GetKFPCoordinates method.
result
IIFServerI::GetKFPCoordinates(SeqPos2 list, string side, string cmd)
{
  if (iif.ICE.logging)
    {
      Ice::Print info(__comm->getLogger());
      stringstream oss;
      oss << "[[";
      for (auto const & e : list) { oss << "[" << e.type << ", " << e.coord1 << ", " << e.coord2 << ", " << e.system << ", " << e.epoch << ", " << e.equinox << ", " << e.pmcoord1 << ", " << e.pmcoord2 << ", " << e.apparentMagnitude << ", '" << e.filter << "', " << e.color << ", '" << e.colorType << "', " << e.wavelength << ", '" << e.file << "', " << e.time << ", " << e.RARate << ", " << e.DECRate << "], "; }
      oss << "]]";
      oss << ", " << side;
      info << "{proxy: " << proxyInfo << ", cmd: " << cmd << ", args: " << oss.str() << "}";
    }

  if (list.size() > 0)
    {
      result tmp;
      bool bad = false;

      bool errorCheck = Configuration::getParameter("IIFIceErrorChecking", true); // 13-Dec-13
//    if(anIIF->getInstrument() == "LUCIFER") errorCheck = false; // 13-Dec-13, disabled 30-Mar-16

      vector<Position> LNstars(list.size());
      ProperMotionType propm;
      MagnitudeType magnitude;

      for (int i = 0; i < (int)list.size(); i++)
        {
          ostringstream oss;
          oss.str("");
          oss << i;
          nsType type = StrToNs(list[0].type);

          if (type != NS_TARGET && type != NS_SIDEREAL)
            {
              tmp.rescode = EXIT_FAILURE;
              tmp.resmsg.push_back(cmd + " result status: Error");
              tmp.resmsg.push_back(cmd + " Error: type invalid: " + list[0].type);
              Ice::Error err(__comm->getLogger());
              err << proxyInfo << " " << cmd << " Error: type invalid: " + list[0].type;
              bad = true;
            }

          coordType coord = StrToCoord(list[i].system, errorCheck);

          if (coord < 0)
            {
              tmp.rescode = EXIT_FAILURE;

              if (!bad) { tmp.resmsg.push_back(cmd + " result status: Error"); }

              tmp.resmsg.push_back(cmd + " Error: star " + oss.str() + " coordinate system invalid: " + list[i].system);
              Ice::Error err(__comm->getLogger());
              err << proxyInfo << " " << cmd << " Error: star " + oss.str() + " coordinate system invalid: " + list[i].system;
              bad = true;
            }

          equinoxType eq = StrToEquinox(list[i].equinox, errorCheck);

          if (eq < 0)
            {
              tmp.rescode = EXIT_FAILURE;

              if (!bad) { tmp.resmsg.push_back(cmd + " result status: Error"); }

              tmp.resmsg.push_back(cmd + " Error: star " + oss.str() + " equinox invalid: " + list[i].equinox);
              Ice::Error err(__comm->getLogger());
              err << proxyInfo << " " << cmd << " Error: star " + oss.str() + " equinox invalid: " + list[i].equinox;
              bad = true;
            }

          filterType filter = StrToFilter(list[i].filter, errorCheck);

          if (filter < 0)
            {
              tmp.rescode = EXIT_FAILURE;

              if (!bad) { tmp.resmsg.push_back(cmd + " result status: Error"); }

              tmp.resmsg.push_back(cmd + " Error: star " + oss.str() + " filter type invalid: " + list[i].filter);
              Ice::Error err(__comm->getLogger());
              err << proxyInfo << " " << cmd << " Error: star " + oss.str() + " filter type invalid: " + list[i].filter;
              bad = true;
            }

          colorType color = StrToColor(list[i].colorType, errorCheck);

          if (color < 0)
            {
              tmp.rescode = EXIT_FAILURE;

              if (!bad) { tmp.resmsg.push_back(cmd + " result status: Error"); }

              tmp.resmsg.push_back(cmd + " Error: star " + oss.str() + " color type invalid: " + list[i].colorType);
              Ice::Error err(__comm->getLogger());
              err << proxyInfo << " " << cmd << " Error: star " + oss.str() + " color type invalid: " + list[i].colorType;
              bad = true;
            }

          if (bad) { return tmp; }

          LNstars[i].Settype(type);
          LNstars[i].Setcoord1(list[i].coord1);
          LNstars[i].Setcoord2(list[i].coord2);
          LNstars[i].Setcoordsystem(coord);
          LNstars[i].Setepoch(list[i].epoch);
          LNstars[i].Setequinox(eq);
          // deal with the proper motion
          propm.coord1 = list[i].pmcoord1;
          propm.coord2 = list[i].pmcoord2;
          LNstars[i].Setpropermotion(&propm);
          // deal with the Apparent Magnitude
          magnitude.apparentMagnitude = list[i].apparentMagnitude;
          magnitude.filter = filter;
          magnitude.color = list[i].color;
          magnitude.colorType = color;
          LNstars[i].Setmagnitude(&magnitude);
          LNstars[i].Setwavelength(list[i].wavelength);
          LNstars[i].Setfile(list[i].file);
          LNstars[i].Settime(list[i].time);
          LNstars[i].SetRARate(list[i].RARate);
          LNstars[i].SetDECRate(list[i].DECRate);
        }

      Result * aResult = anIIF->GetKFPCoordinates(LNstars, side.c_str());
      return execute(aResult, cmd);
    }

  else if (breference)
    {
      Result * aResult = anIIF->GetKFPCoordinates(LNReference, side.c_str());
      return execute(aResult, cmd);
    }

  else
    {
      result tmp;
      tmp.rescode = EXIT_FAILURE;
      tmp.resmsg.push_back(cmd + " result status: Error");
      tmp.resmsg.push_back(cmd + " Error: List of stars is empty");
      Ice::Error err(__comm->getLogger());
      err << proxyInfo << " " << cmd << " Error: List of stars is empty";
      return tmp;
    }
}

//! 14-Jul-16
result
IIFServerI::NSQuery(string targetID, string startDate, string startTime, string endDate, string endTime, string interval, string fileName, const Ice::Current &)
{
  if (iif.ICE.logging)
    {
      Ice::Print info(__comm->getLogger());
      stringstream oss;
      oss << "['" << targetID << "'";
      oss << ", '" << startDate << "'";
      oss << ", '" << startTime << "'";
      oss << ", '" << endDate << "'";
      oss << ", '" << endTime << "'";
      oss << ", '" << interval << "'";
      oss << ", '" << fileName << "'";
      oss << "]";
      info << "{proxy: " << proxyInfo << ", cmd: " << "NSQuery" << ", args: " << oss.str() << "}";
    }

  Result * aResult = anIIF->NSQuery(targetID, startDate, startTime, endDate, endTime, interval, fileName);

  return execute(aResult, "NSQuery");
}

//! Remove the IIF objects.
void
IIFServerI::remove(const Ice::Current & c __attribute__((unused)))
{
  if (iif.ICE.logging)
    {
      Ice::Print info(__comm->getLogger());
      info << "Remove OK";
    }

  //Protect this section with a mutex
  IceUtil::LockT < IceUtil::RecMutex > guard(__cmdsMutex);
  btarget = bguidestars = bhotspot = boffset = bnonSidereal = breference = false;
}

//! Executes the whole bunch of IIF steps involved during command execution and returns a IIFServerI Result object
result
IIFServerI::execute(Result * aResult, string cmd)
{
  //Debug information
  Ice::Print info(__comm->getLogger());

  result tmp;

  bool logging_flag = iif.ICE.logging;
  bool dd_log_flag = iif.ICE.DDlogging;
  bool debug_flag = iif.ICE.debug;

  //increase the command execution counter, protected by mutex
  if (debug_flag) { info << "Increasing command counter"; info.flush();}

  {
    IceUtil::LockT < IceUtil::RecMutex > guard(__cmdsMutex);
    __cmds++;
  }

  if (aResult->GetresultCode() != RESCODE_FAIL)
    {
      if (debug_flag) {info << "Calling "  << cmd << " IIF::GetCommandStatus()"; info.flush();}

      StatusInfo * statusInf = anIIF->GetCommandStatus(aResult->GetcommandHandle());

      if (debug_flag) { info << "Calling "  << cmd << " IIF::statusInf->Block()"; info.flush();}

      statusInf->Block();

      CommandReturn cmdRet;

      if (debug_flag) { info << "Deserializing "  << cmd; info.flush();}

      cmdRet.deserialize(statusInf->GetCommandResult());

      if (cmdRet.isError())
        {
          tmp.resmsg.push_back(cmd + " result status: Error");
          tmp.rescode = EXIT_FAILURE;
        }

      else if (cmdRet.isWarning())
        {
          tmp.resmsg.push_back(cmd + " result status: Warning");
          tmp.rescode = EXIT_WARNING;
        }

      else
        {
          tmp.resmsg.push_back(cmd + " result status: OK");
          tmp.rescode = EXIT_SUCCESS;
        }

      for (int i = 1; i <= cmdRet.getResultCount(); i++)
        {
          tmp.resmsg.push_back(cmdRet.getResultDescription(i));

          if (debug_flag) { info << tmp.resmsg.back(); info.flush();}
        }

      /* added 19-Jul-11 */
      if (cmd.find("GetRotatorTrajectory") != string::npos)
        {
          double time, theta;

          for (int i = 0; i < cmdRet.getNumberOfArgs() / 2; i++)
            {
              cmdRet.getArgument(&time);
              cmdRet.getArgument(&theta);
              stringstream pair;
              pair.precision(16);
              pair << time << " " << theta;
              tmp.resmsg.push_back(pair.str());
            }
        }

      /* added 3-Aug-12 */
      if (cmd.find("GetRotatorPolynomials") != string::npos)
        {
          double time, theta0, theta1, theta2;

          for (int i = 0; i < cmdRet.getNumberOfArgs() / (__order + 2); i++)
            {
              stringstream pair;
              pair.precision(16);
              cmdRet.getArgument(&time);
              cmdRet.getArgument(&theta0);
              pair << time << " " << theta0;

              if (__order > 0)
                {
                  cmdRet.getArgument(&theta1);
                  pair << " " << theta1;
                }

              if (__order > 1)
                {
                  cmdRet.getArgument(&theta2);
                  pair << " " << theta2;
                }

              tmp.resmsg.push_back(pair.str());
            }
        }

      /* added 24-Sep-12 */
      if (cmd.find("GetKFPCoordinates") != string::npos)
        {
          double x, y;

          for (int i = 0; i < cmdRet.getNumberOfArgs() / 2; i++)
            {
              stringstream pair;
              pair.precision(16);
              cmdRet.getArgument(&x);
              cmdRet.getArgument(&y);
              pair << x << " " << y;
              tmp.resmsg.push_back(pair.str());
            }
        }

      if (logging_flag)
        {
          if (dd_log_flag || (cmd != "GetParameter" && cmd != "SetParameter"))  // test added 17-Feb-15
            {
              info << cmd << " command status: " << statusInf->GetCommandStatus();
              info.flush();
            }
        }

      delete statusInf;
    }

  else
    {
      tmp.resmsg.push_back(cmd + " execution failed: " + aResult->GetresultString());
      tmp.rescode = EXIT_FAILURE;
      info << tmp.resmsg.front();
      info.flush();
    }

  delete aResult;

  //decrease the command execution counter, protected by mutex
  if (debug_flag) { info << "Decreasing command counter"; info.flush();}

  {
    IceUtil::LockT < IceUtil::RecMutex > guard(__cmdsMutex);
    __cmds--;
  }

  //Debug information
  if (debug_flag) { info << cmd << " execution done"; info.flush();}

  return tmp;
}
