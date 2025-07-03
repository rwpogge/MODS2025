#ifndef IIFSERVER_ICE
#define IIFSERVER_ICE

module lbto
{
/** @brief Structure to allocate the DD data.*/
struct DDstruct
{
        string DDname;
        string DDkey;
        string dataType;
        string comment;
};
sequence<DDstruct> SeqDD;

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
sequence<newposition> SeqNewPos;

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

sequence<double> SeqWF;

sequence<double> SeqModes;

sequence<string> SeqRes;

struct result
{
        int rescode;
        SeqRes resmsg;
};

interface IIFServer
{
        ["ami"] result Authorize();
        ["ami"] result StepFocus(double relPos, string ope, string side);
        ["ami"] result MoveFocus(double absPos, string ope, string side);
        ["ami"] result OffsetPointing(double rotangle, double offx, double offy, string coordsys, string ope, string np, string movetype, string side);
        ["ami"] result OffsetGuiding(double rotangle, double offx, double offy, string coordsys, string side);
        ["ami"] result UpdatePointingReference(string offsetcoord, string side);
	["ami"] result TipTilt(double xrot, double yrot, string ope, string side);
	["ami"] result SetPMTerm(string name, double value, string side);
        ["ami"] result MoveXYZ(double xmov, double ymov, double zmov, string side);
        ["ami"] result MoveXY(double xmov, double ymov, string ope, string side);
        ["ami"] result Move(double x, double y, double z, double rx, double ry, double rz, int dflag, string movetype, string OPE, int time, string side);
        ["ami"] result SetTarget(double coord1, double coord2, string system, double epoch, float wavelength);
        ["ami"] result SetStars(SeqPos stars);
        ["ami"] result SetStarsNew(SeqNewPos stars);
        ["ami"] result ClearStars();
        ["ami"] result SetHotspot(double coord1, double coord2);
        ["ami"] result ClearHotspot();
        ["ami"] result SetOffset(double coord1, double coord2, string system);
        ["ami"] result ClearOffset();
        ["ami"] result SetNonSidereal(nonsidereal target, bool override);
        ["ami"] result ClearNonSidereal(bool override);
        ["ami"] result UpdateNonSiderealTarget(double RArate, double DECrate);
        ["ami"] result PresetTelescope(double rotangle, string rotmode, string mode, string side);
        ["ami"] result BinocularControl(string flag);
        ["ami"] result GetParameter(SeqDD list);
        ["ami"] result SetParameter(SeqDD list);
        ["ami"] result RotateCommon(double z, double angle, double direction, string side);
        ["ami"] result RotateZ(double angle, string movetype, string side);
        ["ami"] result RotatePrimary(double distance, double angle, double direction, string side);
        ["ami"] result SendWavefront(SeqWF wfs, string OPE, string side);
        ["ami"] result UpdateTargetWavelength(float wavelength, string side);
	["ami"] result LogEvent(string event, string description);
	["ami"] result GetRotatorTrajectory(double seconds, double interval, double start, string side);
	["ami"] result GetRotatorPolynomials(double start, int count, double interval, int order, string side);
	["ami"] result Standby(int level, string side);
        ["ami"] result MaximizeWrapTime(bool azflag, bool rotflag, string side);
        ["ami"] result PauseGuiding(string side);
        ["ami"] result ResumeGuiding(string side);
        ["ami"] result SetGuidingHotspot(double coord1, double coord2, string movetype, string side);

        ["ami"] result RotReady(bool enable, string side);
        ["ami"] result RotHold(string side);
        ["ami"] result RotTrack(string side);
        ["ami"] result RotServicePosition(double angle, string side);

        ["ami"] result PauseAO(string side);
        ["ami"] result ResumeAO(string side);
        ["ami"] result StartAO(string side);
        ["ami"] result StopAO(string reason, string side);
        ["ami"] result AcquireRefAO(bool repointFlag, string side);
        ["ami"] result CheckRefAO(string side);
        ["ami"] result RefineAO(string method, string side);
        ["ami"] result RunAO(string type, string side);
        ["ami"] result PresetFlatAO(string flat, string side);
        ["ami"] result CorrectModesAO(SeqModes modes, string side);
        ["ami"] result ModifyAO(int NModes, double Freq, int Nbins, double TTMod, string F1spec, string F2spec, string side);
        ["ami"] result PresetAO(string AOMode, string side);
        ["ami"] result OffsetXYAO(double DeltaX, double DeltaY, string side);
        ["ami"] result OffsetZAO(double DeltaZ, string side);
        ["ami"] result SetReference(SeqPos stars);
        ["ami"] result SetReferenceNew(SeqNewPos stars);
        ["ami"] result ClearReference();
        ["ami"] result GetKFPCoordinates(SeqPos stars, string side);
        ["ami"] result GetKFPCoordinatesNew(SeqNewPos stars, string side);
        ["ami"] result SetAGWFilter(int filterNumber, string side);

        void remove();
};

};
#endif
