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

/** @brief structure for iifres object */
/* changed result -> iifres following exchange with Glenn Eychaner 2025 Jul 03 [rwp/osu] */
struct iifres
{
  int rescode;
  SeqRes resmsg;
};

/** @brief methods implemented */
interface IIFServer
{
  iifres ApplyWaffleAO(SeqModes modes, string side);
  iifres Authorize();
  iifres StepFocus(double relPos, string ope, string side);
  iifres MoveFocus(double absPos, string ope, string side);
  iifres OffsetPointing(double rotangle, double offx, double offy, string coordsys, string ope, string np, string movetype, string side);
  iifres OffsetPointing2(double rotangle, double offx, double offy, string coordsys, string movetype, string side); /* 25-Mar-16 */
  iifres OffsetGuiding(double rotangle, double offx, double offy, string coordsys, string side);
  iifres UpdatePointingReference(string offsetcoord, string side);
  iifres TipTilt(double xrot, double yrot, string ope, string side);
  iifres SetPMTerm(string name, double value, string side);
  iifres SetPMTerm2(string name, double value, string movetype, string side); /* 25-Mar-16 */
  iifres MoveXYZ(double xmov, double ymov, double zmov, string side);
  iifres MoveXY(double xmov, double ymov, string ope, string side);
  iifres Move(double x, double y, double z, double rx, double ry, double rz, int dflag, string movetype, string OPE, int time, string side);
  iifres SetTarget(double coord1, double coord2, string system, double epoch, float wavelength);
  iifres SetStars(SeqPos stars);
  iifres SetStarsNew(SeqNewPos stars); /* deprecated; use SetStars2 25-Mar-16 */
  iifres SetStars2(SeqPos2 stars); /* changed from 'New' to '2' 25-Mar-16 */
  iifres ClearStars();
  iifres SetHotspot(double coord1, double coord2);
  iifres ClearHotspot();
  iifres SetOffset(double coord1, double coord2, string system);
  iifres SetOffsetNew(double coord1, double coord2, string system, string movetype); /* deprecated 25-Mar-16 */
  iifres SetOffset2(double coord1, double coord2, string system, string movetype); /* 29-Apr-15, changed from 'New' to '2' 25-Mar-16 */
  iifres ClearOffset();
  iifres SetNonSidereal(nonsidereal target, bool override);
  iifres ClearNonSidereal(bool override);
  iifres UpdateNonSiderealTarget(double RArate, double DECrate);
  iifres SetNonSiderealObject(string objtype, nonsidereal target, bool override);
  iifres ClearNonSiderealObject(string objtype, bool override);
  iifres UpdateNonSiderealObject(string objtype, double RArate, double DECrate);
  iifres PresetTelescope(double rotangle, string rotmode, string mode, string side);
  iifres BinocularControl(string flag);
  iifres GetParameter(SeqDD list);
  iifres SetParameter(SeqDD list);
  iifres RotateCommon(double z, double angle, double direction, string side);
  iifres RotateZ(double angle, string movetype, string side);
  iifres RotatePrimary(double distance, double angle, double direction, string side);
  iifres SendWavefront(SeqWF wfs, string OPE, string side);
  iifres UpdateTargetWavelength(float wavelength, string side);
  iifres LogEvent(string event, string description);
  iifres GetRotatorTrajectory(double seconds, double interval, double start, string side);
  iifres GetRotatorPolynomials(double start, int count, double interval, int order, string side);
  iifres Standby(int level, string side);
  iifres MaximizeWrapTime(bool azflag, bool rotflag, string side);

  iifres PauseGuiding(string side);
  iifres ResumeGuiding(string side);
  iifres StopGuiding(string side);
  iifres SetGuidingHotspot(double coord1, double coord2, string movetype, string side);
  iifres SetAGWFilter(int filterNumber, string side);
  iifres SetGuidingBinning(int factor, string side);

  iifres RotReady(bool enable, string side);
  iifres RotHold(string side);
  iifres RotTrack(string side);
  iifres RotServicePosition(double angle, string side);

  iifres PauseAO(string side);
  iifres ResumeAO(string side);
  iifres StartAO(string side);
  iifres StopAO(string reason, string side);
  iifres AcquireRefAO(bool repointFlag, string side);
  iifres CheckRefAO(string side);
  iifres RefineAO(string method, string side);
  iifres RunAO(string type, string side);
  iifres PresetFlatAO(string flat, string side);
  iifres CorrectModesAO(SeqModes modes, string side);
  iifres ModifyAO(int NModes, double Freq, int Nbins, double TTMod, string F1spec, string F2spec, string side);
  iifres PresetAO(string AOMode, string side);
  iifres OffsetXYAO(double DeltaX, double DeltaY, string side);
  iifres OffsetZAO(double DeltaZ, string side);
  iifres SetReference(SeqPos stars);
  iifres SetReferenceNew(SeqNewPos stars); /* deprecated; use SetReference2 25-Mar-16 */
  iifres SetReference2(SeqPos2 stars); /* changed from 'New' to '2' 25-Mar-16 */
  iifres ClearReference();
  iifres GetKFPCoordinates(SeqPos stars, string side);
  iifres GetKFPCoordinatesNew(SeqNewPos stars, string side); /* deprecated; use GetKFPCoordinates2 25-Mar-16 */
  iifres GetKFPCoordinates2(SeqPos2 stars, string side); /* changed from 'New' to '2' 25-Mar-16 */
  iifres NSQuery(string targenName, string startDate, string startTime, string endDate, string endTime, string interval, string fileName); /* added 14-Jul-16 */

  void remove();
};
};
