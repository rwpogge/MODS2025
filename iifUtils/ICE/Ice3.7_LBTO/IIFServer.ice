#pragma once

module lbto
{
/** @brief Structure to allocate the DD data */
struct DDstruct
{
  string DDname;
  string DDkey;
  string dataType;
  string comment;
};
sequence<DDstruct> SeqDD;

/** @brief structure for abbreviated position object */
struct position
{
  double coord1;
  double coord2;
  string system;
  double epoch;
  string equinox;
  double pmcoord1;
  double pmcoord2;
  double apparentMagnitude;
  string filter;
  double color;
  string colorType;
  float wavelength;
  string targname;
};
sequence<position> SeqPos;

/** @brief structure for complete position object. Deprecated; use position2 25-Mar-16 */
struct newposition
{
  string type;
  double coord1;
  double coord2;
  string system;
  string equinox;
  double epoch;
  double pmcoord1;
  double pmcoord2;
  double apparentMagnitude;
  string filter;
  double color;
  string colorType;
  float wavelength;
  string file;
  double time;
  double RARate;
  double DECRate;
};
sequence<newposition> SeqNewPos; /* Deprecated; use SeqPos2 25-Mar-16 */

/** @brief structure for complete position object */
struct position2 /* changed from 'new' to '2' 25-Mar-16 */
{
  string type;
  double coord1;
  double coord2;
  string system;
  double epoch;
  string equinox;
  double pmcoord1;
  double pmcoord2;
  double apparentMagnitude;
  string filter;
  double color;
  string colorType;
  float wavelength;
  string file;
  double time;
  double RARate;
  double DECRate;
};
sequence<position2> SeqPos2; /* changed from 'new' to '2' 25-Mar-16 */

/** @brief structure for nonsidereal object */
struct nonsidereal
{
  string type;
  float wavelength;
  string file;
  double value0;
  double value1;
  double value2;
  double value3;
  double value4;
};

/** @brief wavefront vector */
sequence<double> SeqWF;

/** @brief AO modes vector */
sequence<double> SeqModes;

/** @brief result vector */
sequence<string> SeqRes;

/** @brief structure for result object */
struct result
{
  int rescode;
  SeqRes resmsg;
};

/** @brief methods implemented */
interface IIFServer
{
  result ApplyWaffleAO(SeqModes modes, string side);
  result Authorize();
  result StepFocus(double relPos, string ope, string side);
  result MoveFocus(double absPos, string ope, string side);
  result OffsetPointing(double rotangle, double offx, double offy, string coordsys, string ope, string np, string movetype, string side);
  result OffsetPointing2(double rotangle, double offx, double offy, string coordsys, string movetype, string side); /* 25-Mar-16 */
  result OffsetGuiding(double rotangle, double offx, double offy, string coordsys, string side);
  result UpdatePointingReference(string offsetcoord, string side);
  result TipTilt(double xrot, double yrot, string ope, string side);
  result SetPMTerm(string name, double value, string side);
  result SetPMTerm2(string name, double value, string movetype, string side); /* 25-Mar-16 */
  result MoveXYZ(double xmov, double ymov, double zmov, string side);
  result MoveXY(double xmov, double ymov, string ope, string side);
  result Move(double x, double y, double z, double rx, double ry, double rz, int dflag, string movetype, string OPE, int time, string side);
  result SetTarget(double coord1, double coord2, string system, double epoch, float wavelength);
  result SetStars(SeqPos stars);
  result SetStarsNew(SeqNewPos stars); /* deprecated; use SetStars2 25-Mar-16 */
  result SetStars2(SeqPos2 stars); /* changed from 'New' to '2' 25-Mar-16 */
  result ClearStars();
  result SetHotspot(double coord1, double coord2);
  result ClearHotspot();
  result SetOffset(double coord1, double coord2, string system);
  result SetOffsetNew(double coord1, double coord2, string system, string movetype); /* deprecated 25-Mar-16 */
  result SetOffset2(double coord1, double coord2, string system, string movetype); /* 29-Apr-15, changed from 'New' to '2' 25-Mar-16 */
  result ClearOffset();
  result SetNonSidereal(nonsidereal target, bool override);
  result ClearNonSidereal(bool override);
  result UpdateNonSiderealTarget(double RArate, double DECrate);
  result SetNonSiderealObject(string objtype, nonsidereal target, bool override);
  result ClearNonSiderealObject(string objtype, bool override);
  result UpdateNonSiderealObject(string objtype, double RArate, double DECrate);
  result PresetTelescope(double rotangle, string rotmode, string mode, string side);
  result BinocularControl(string flag);
  result GetParameter(SeqDD list);
  result SetParameter(SeqDD list);
  result RotateCommon(double z, double angle, double direction, string side);
  result RotateZ(double angle, string movetype, string side);
  result RotatePrimary(double distance, double angle, double direction, string side);
  result SendWavefront(SeqWF wfs, string OPE, string side);
  result UpdateTargetWavelength(float wavelength, string side);
  result LogEvent(string event, string description);
  result GetRotatorTrajectory(double seconds, double interval, double start, string side);
  result GetRotatorPolynomials(double start, int count, double interval, int order, string side);
  result Standby(int level, string side);
  result MaximizeWrapTime(bool azflag, bool rotflag, string side);

  result PauseGuiding(string side);
  result ResumeGuiding(string side);
  result StopGuiding(string side);
  result SetGuidingHotspot(double coord1, double coord2, string movetype, string side);
  result SetAGWFilter(int filterNumber, string side);
  result SetGuidingBinning(int factor, string side);

  result RotReady(bool enable, string side);
  result RotHold(string side);
  result RotTrack(string side);
  result RotServicePosition(double angle, string side);

  result PauseAO(string side);
  result ResumeAO(string side);
  result StartAO(string side);
  result StopAO(string reason, string side);
  result AcquireRefAO(bool repointFlag, string side);
  result CheckRefAO(string side);
  result RefineAO(string method, string side);
  result RunAO(string type, string side);
  result PresetFlatAO(string flat, string side);
  result CorrectModesAO(SeqModes modes, string side);
  result ModifyAO(int NModes, double Freq, int Nbins, double TTMod, string F1spec, string F2spec, string side);
  result PresetAO(string AOMode, string side);
  result OffsetXYAO(double DeltaX, double DeltaY, string side);
  result OffsetZAO(double DeltaZ, string side);
  result SetReference(SeqPos stars);
  result SetReferenceNew(SeqNewPos stars); /* deprecated; use SetReference2 25-Mar-16 */
  result SetReference2(SeqPos2 stars); /* changed from 'New' to '2' 25-Mar-16 */
  result ClearReference();
  result GetKFPCoordinates(SeqPos stars, string side);
  result GetKFPCoordinatesNew(SeqNewPos stars, string side); /* deprecated; use GetKFPCoordinates2 25-Mar-16 */
  result GetKFPCoordinates2(SeqPos2 stars, string side); /* changed from 'New' to '2' 25-Mar-16 */
  result NSQuery(string targenName, string startDate, string startTime, string endDate, string endTime, string interval, string fileName); /* added 14-Jul-16 */

  void remove();
};
};
