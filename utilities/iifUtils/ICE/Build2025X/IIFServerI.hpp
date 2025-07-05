#pragma once

#include <iif/ice/IIFServer.slice.hpp>
#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>
#include <iif/instrument/IIF.hpp>

namespace lbto {

/**
* @class IIFServerI
* @author Jose Borelli <borelli@mpia.de>
* @brief is responsible for the communication with the LBT-TCS via IIF.
*/

#define EXIT_WARNING 2

class IIFServerI : public IIFServer
{
public:
	/** @brief class parameterized constructor
	@param comm The Ice Communicator.
	@param _anIIF A pointer to an IIF object associated with this instance.*/
	IIFServerI(Ice::CommunicatorPtr comm, IIF* _anIIF);

	/** @brief class destructor */
	~IIFServerI();

	/** @brief Check authorization.
	@return Result object */
	result Authorize(const Ice::Current&) override;

	/** @brief StepFocus moves the respective focus position, by moving the OPE 
	@brief a given distance in the direction of of the telescope’s Z axis.
	@param relPos distance to move the OPE [millimeter]
	@param ope Optical element :  M1 or M2 or M3 or M1M2
	@param side the side this command applies to.
	@return Result object. */
	result StepFocus(double relPos, string ope, string side, const Ice::Current&) override;

	/** @brief Moves an OPE to a new absolute position z to adjust or to define a new focus  position.
	@param absPos distance to move the OPE [millimeter]
	@param ope Optical element :  M1 or M2 or M3 or M1M2
	@param side the side this command applies to.
	@return Result object. */
	result MoveFocus(double absPos, string ope, string side, const Ice::Current&) override;

	/** @brief OffsetPointing moves the telescope a small distance, setting the value 
	@brief of the telescope pointing coordinates to the new position.
	@param rotangle Rotator angle [radian]
	@param offx Offset in X [radian/mm]
	@param offy Offset in Y [radian/mm]
	@param coordsys Coordinate system: RADEC, AZALT, DETXY, GALACTIC
	@param ope Optical element : DEFAULT, M1, M2, M3, HEXAPOD, MOUNT
	@param np New position flag
	@param movetype movement type to apply: Relative / Absolute
	@param side the side this command applies to.
	@return Result object. */
	result OffsetPointing(double rotangle, double offx, double offy, string coordsys, string ope, string np, string movetype, string side, const Ice::Current&) override;
	result OffsetPointing2(double rotangle, double offx, double offy, string coordsys, string movetype, string side, const Ice::Current&) override;

	/** @brief OffsetGuiding moves the telescope a small distance without changing 
	@brief the value of the pointing coordinates.
	@param rotangle Rotator angle [radian]
	@param offx Offset in X [radian]
	@param offy Offset in Y [radian]
	@param coordsys Coordinate system: AZALT
	@param side the side this command applies to.
	@return Result object. */
	result OffsetGuiding(double rotangle, double offx, double offy, string coordsys, string side, const Ice::Current&) override;

	/** @brief This command allows the user to use the current sky or focal plane location achieved via the 
	@brief OffsetPointing command and update the previous values; To be used in conjunction with the OffsetPointing cmd.
	@param offset_coord the offset coordinate system.
	@param side the side this command applies to.
	@return Result object */
	result UpdatePointingReference(string offset_coord, string side, const Ice::Current&) override;
	
	/** @brief TipTilt moves an OPE in tip or tilt direction, relative to the current position.
	@param xrot rotation in x [microradian]
	@param yrot rotation in y [microradian]
	@param ope Optical element: M1 or M2 or M3
	@param side the side this command applies to.
	@return Result object. */
	result TipTilt(double xrot, double yrot, string ope, string side, const Ice::Current&) override;

	/** @brief SetPMTerm sets a singel term in the pointing model.
	@param name name of term (only "CA" and "IE" are allowed)
	@param value value of term
	@param side the side this command applies to.
	@param movetype "ABS" or "REL" (incremental).
	@return Result object. */
	result SetPMTerm(string name, double value, string side, const Ice::Current&) override;
	result SetPMTerm2(string name, double value, string movetype, string side, const Ice::Current&) override;

	/** @brief MoveXYZ moves the primary and secondary together in X, Y and Z direction.
	@param xmov movement in x [millimeter]
	@param ymov movement in y [millimeter]
	@param zmov movement in z [millimeter]
	@param side the side this command applies to.
	@return Result object. */
	result MoveXYZ(double xmov, double ymov, double zmov, string side, const Ice::Current&) override;

	/** @brief MoveXY moves an OPE in X or Y direction, relative to the current position.
	@param xmov movement in x [millimeter]
	@param ymov movement in y [millimeter]
	@param ope Optical element: M1 or M2
	@param side the side this command applies to.
	@return Result object. */
	result MoveXY(double xmov, double ymov, string ope, string side, const Ice::Current&) override;

	/** @brief Rotates the primary and the secondary mirror around a common point.
	@param z rotation point [millimeter]
	@param angle rotation angle [radian]
	@param direction direction angle [radian]
	@param side the side this command applies to.
	@return Result object. */ 
	result RotateCommon(double z, double angle, double direction, string side, const Ice::Current&) override;

	/** @brief the tertiary mirror is rotated to adjust the incoming beam angle for  the instrument. 
	@brief movetype determines if the movement is relative or absolute.
	@param angle rotation angle [radian]
	@param movetype movement type: ABS, REL
	@param side the side this command applies to.
	@return Result object. */
	result RotateZ(double angle, string movetype, string side, const Ice::Current&) override;

	/** @brief This command combines the functionality of MoveXY, TipTilt, StepFocus and MoveFocus.
	@param x movement in x [millimeter]
	@param y movement in y [millimeter]
	@param z movement in z [millimeter]
	@param rx rotation in x [microradian]
	@param ry rotation in y [microradian]
	@param rz rotation in z [microradian]
	@param dflag  6 bits with a bit for each of the preceding 6 variables. Bit 0 enables X, bit 1 enables Y, bit 2 enables Z, and so on.
	@param movetype movement type: ABS, REL
	@param OPE Optical element: DEFAULT, M1, M2, M3, M1M2, M1M3, M2M3, M1M2M3
	@param time lookahead time for the collimation correction [sec]
	@param side the side this command applies to.
	@return Result object. */
	result Move(double x, double y, double z, double rx, double ry, double rz, int dflag, string movetype, string OPE, int time, string side, const Ice::Current&) override;

	/** @brief Set the hotspot coordinates to be used by PresetTelescope command
	@param coord1 Coordinate x [mm]
	@param coord2 Coordinate y [mm]
	@return Result object. 
	@note This is not an IIF/TCS command. */
	result SetHotspot(double coord1, double coord2, const Ice::Current&) override;

	/** @brief Removes the hotspot from the system.
	@return Result object. 
	@note This is not an IIF/TCS command. */
	result ClearHotspot(const Ice::Current&) override;

	/** @brief Set the offset coordinates to be used by PresetTelescope
	@param coord1 Coordinate x [radian/mm]
	@param coord2 Coordinate y [radian/mm]
	@param coordsys Coordinate system: RADEC, AZALT, DETXY, GALACTIC.
	@param movetype "CS" for coordinate system offset.
	@return Result object. 
	@note This is not an IIF/TCS command. */
	result SetOffset(double coord1, double coord2, string coordsys, const Ice::Current&) override;
	result SetOffsetNew(double coord1, double coord2, string coordsys, string movetype, const Ice::Current&) override;
	result SetOffset2(double coord1, double coord2, string coordsys, string movetype, const Ice::Current&) override;

	/** @brief Removes the offset from the system.
	@return Result object. 
	@note This is not an IIF/TCS command. */
	result ClearOffset(const Ice::Current&) override;

	/** @brief Set the non-sidereal values to be used by PresetTelescope
	@param target non-sidereal target
	@param override non-sidereal override flag
	@return Result object. 
	@note This is not an IIF/TCS command. */
	result SetNonSidereal(nonsidereal target, bool override, const Ice::Current&) override;

	/** @brief Removes the non-sidereal object from the system.
	@param override non-sidereal override flag
	@return Result object. 
	@note This is not an IIF/TCS command. */
	result ClearNonSidereal(bool override, const Ice::Current&) override;

	/** @brief Update the non-sidereal values
	@param RArate the new RArate
	@param DECrate the new DECrate
	@param side the side this command applies to.
	@return Result object. */
	result UpdateNonSiderealTarget(double RArate, double DECrate, const Ice::Current&) override;

	/** @brief Set the non-sidereal values to be used by PresetTelescope
	@param objtype object type
	@param target non-sidereal target
	@param override non-sidereal override flag
	@return Result object. 
	@note This is not an IIF/TCS command. */
	result SetNonSiderealObject(string objtype, nonsidereal target, bool override, const Ice::Current&) override;

	/** @brief Removes the non-sidereal object from the system.
	@param objtype object type
	@param override non-sidereal override flag
	@return Result object. 
	@note This is not an IIF/TCS command. */
	result ClearNonSiderealObject(string objtype, bool override, const Ice::Current&) override;

	/** @brief Update the non-sidereal values
	@param objtype the object type (target, guide, AOref)
	@param RArate the new RArate
	@param DECrate the new DECrate
	@param side the side this command applies to.
	@return Result object. */
	result UpdateNonSiderealObject(string objtype, double RArate, double DECrate, const Ice::Current&) override;

	/** @brief Set the target information to be used by PresetTelescope
	@param coord1 Coordinate x [radian]
	@param coord2 Coordinate y [radian]
	@param coordsys Coordinate system: RADEC, AZALT, GALACTIC
	@param epoch 
	@param wl Target wavelength
	@return Result object.
	@note This is not an IIF/TCS command and is not completed and must be completed. */
	result SetTarget(double coord1, double coord2, string coordsys, double epoch, float wl, const Ice::Current&) override;

	/** @brief Sets the target, and eventually, the guide stars. (first element on the list is used as target)
	@param list List of objects.
	@return Result object
	@note This is not an IIF/TCS command. */
	result SetStars(SeqPos list, const Ice::Current&) override;
	result SetStarsNew(SeqNewPos list, const Ice::Current&) override;
	result SetStars2(SeqPos2 list, const Ice::Current&) override;

	/** @brief Removes the selected target and guide stars from the system.
	@return Result object 
	@note This is not an IIF/TCS command. */
	result ClearStars(const Ice::Current&) override;

	/** @brief PresetTelescope slew the telescope into position in order to begin an observation cycle.
	@param rotangle initial rotator angle [radian]
	@param rotmode rotator mode: POSITION, PARALLACTIC, NATIVE, GRAVITATIONAL, IDLE.
	@param mode Operation mode: STATIC, TRACK, ACQUIRE, GUIDE, ADAPTIVE, ACTIVE, INTERFEROMETRIC.
	@param side the side this command applies to.
	@return Result object
	@note This is an IIF/TCS command, but the parameters are different. */
	result PresetTelescope(double rotangle, string rotmode, string mode, string side, const Ice::Current& ) override;

	/** @brief BinocularControl clears the target information in the PCS.
	@param flag Preset control flag: CLEARPRESET, SYNCPRESET, SYNCOFFSET, ADJUSTBALANCE.
	@return Result object */
	result BinocularControl(string flag, const Ice::Current&) override;

	/** @brief Reads a block of entries from the data dictionary in one shot. 
	@brief If FITS information is defined here, it will be included in the FITS headers (only used by the IRTC client).
	@param list DD entry list
	@return Result object. */
	result GetParameter(SeqDD list, const Ice::Current&) override;

	/** @brief Sets the values of the specified data dictionary entries on the TCS in one shot.
	@param list DD entry list with entry-value pairs
	@return Result object. */
	result SetParameter(SeqDD list, const Ice::Current&) override;

	/** @brief Sends an array of 28 Zernike polynomial coefficients to be compensated by the actuators 
	@brief of the primary or secondary mirror.
	@param wfs coefficients list
	@param OPE Optical element: M1, M2
	@param side the side this command applies to.
	@return Result object. */
	result SendWavefront(SeqWF wfs, string OPE, string side, const Ice::Current&) override;

	/** @brief Updates the target effective wavelength without issuing a new preset.
	@param wavelength new target wavelength. Range is [0.3, 15.0] [microns]
	@param side the side this command applies to.
	@return Result object. */
	result UpdateTargetWavelength(float wavelength, string side, const Ice::Current&) override;

	/** @brief Readies the cable wrap and rotator.
	@param enable flag to ready/unready the rotator.
	@param side the side this command applies to.
	@return Result object */
	result RotReady(bool enable, string side, const Ice::Current&) override;
	
	/** @brief If the rotator is tracking or slewing, this command makes it stop moving and hold the position.
	@param side the side this command applies to.
	@return Result object */
	result RotHold(string side, const Ice::Current&) override;
	
	/** @brief Makes rotator begin tracking according to the polynomial stream it is currently receiving from the PCS.
	@param side the side this command applies to.
	@return Result object */
	result RotTrack(string side, const Ice::Current&) override;

	/** @brief This command is to provide some control over the use of the rotator cablewrap.
	@param azflag flag to control AZ wrap.
	@param rotflag flag to control rotator wrap.
	@param side the side this command applies to.
	@return Result object */
	result MaximizeWrapTime(bool azflag, bool rotflag, string side, const Ice::Current&) override;
	
	/** @brief Makes the rotator move to the specified angle in the specified coordinate frame and hold at that position.
	@param angle [radian]
	@param side the side this command applies to.
	@return Result object */
	result RotServicePosition(double angle, string side, const Ice::Current&) override;
	
	/** @brief Temporarily suspends the current guiding operation.
	@param side the side this command applies to.
	@return Result object */
	result PauseGuiding(string side, const Ice::Current&) override;

	/** @brief Resumes suspended guiding operation.
	@param side the side this command applies to.
	@return Result object */
	result ResumeGuiding(string side, const Ice::Current&) override;

	/** @brief Stops guiding operation.
	@param side the side this command applies to.
	@return Result object */
	result StopGuiding(string side, const Ice::Current&) override;

	/** @brief Sets guider hotspot.
	@param coord1 coordinate x [pixel].
	@param coord2 coordinate y [pixel].
	@param movetype the move type, REL or ABS.
	@param side the side this command applies to.
	@return Result object */
	result SetGuidingHotspot(double coord1, double coord2, string movetype, string side, const Ice::Current&) override;

	/** @brief Use this command to set the AGw filter.
	@param filterNumber the number of the filter.
	@param side the side this command applies to.
	@return Result object */
	result SetAGWFilter(int filterNumber, string side, const Ice::Current&) override;

	/** @brief Use this command to set the guider binning.
	@param factor the binning factor.
	@param side the side this command applies to.
	@return Result object */
	result SetGuidingBinning(int factor, string side, const Ice::Current&) override;

	/** @brief Temporarily suspends the current AO operation.
	@param side the side this command applies to.
	@return Result object */
	result PauseAO(string side, const Ice::Current&) override;

	/** @brief Resumes suspended operation after a PauseAO.
	@param side the side this command applies to.
	@return Result object */
	result ResumeAO(string side, const Ice::Current&) override;

	/** @brief Start AO operation.
	@param side the side this command applies to.
	@return Result object */
	result StartAO(string side, const Ice::Current&) override;

	/** @brief Stop AO operation.
	@param reason the reason for stopping.
	@param side the side this command applies to.
	@return Result object */
	result StopAO(string reason, string side, const Ice::Current&) override;

	/** @brief Acquire the reference star.
	@param repointFlag flag to request the telescope to repoint.
	@param side the side this command applies to.
	@return Result object */
	result AcquireRefAO(bool repointFlag, string side, const Ice::Current&) override;

	/** @brief Check the reference star position.
	@param side the side this command applies to.
	@return Result object */
	result CheckRefAO(string side, const Ice::Current&) override;

	/** @brief Refine AO.
	@param method the method used for refinement.
	@param side the side this command applies to.
	@return Result object */
	result RefineAO(string method, string side, const Ice::Current&) override;

	/** @brief Do AcquireRefAO, StartAO.
	@param repointFlag flag to request the telescope to repoint in AcquireRefAO.
	@param side the side this command applies to.
	@return Result object */
	result RunAO(string type, string side, const Ice::Current&) override;

	/** @brief Request flat shape for AO.
	@param flat the flat name.
	@param side the side this command applies to.
	@return Result object */
	result PresetFlatAO(string flat, string side, const Ice::Current&) override;

	/** @brief Send 672 mode corrections to AO.
	@param modes modes list
	@param side the side this command applies to.
	@return Result object */
	result CorrectModesAO(SeqModes modes, string side, const Ice::Current&) override;

	result ApplyWaffleAO(SeqModes modes, string side, const Ice::Current&) override;

	/** @brief Modify AO.
	@param NModes the number of corrected modes.
	@param Freq  the CCD integration frequency.
	@param Nbins  the CCD binning.
	@param TTMod  Tip-Tilt internal mirror modulation.
	@param F1spec the selected position of filter wheel #1.
	@param F2spec the selected position of filter wheel #2.
	@param side the side this command applies to.
	@return Result object */
	result ModifyAO(int NModes, double Freq, int Nbins, double TTMod, string F1spec, string F2spec, string side, const Ice::Current&) override;

	/** @brief Preset AO.
	@param AOMode the AO operatin mode.
	@param WFS    the source of WFS data.
	@param S1Null specifies the slope null vector.
	@param side the side this command applies to.
	@return Result object */
	result PresetAO(string AOMode, string side, const Ice::Current&) override;

	/** @brief OffsetXY AO.
	@param DeltaX the X offset.
	@param DeltaY the Y offset.
	@param side the side this command applies to.
	@return Result object */
	result OffsetXYAO(double DeltaX, double DeltaY, string side, const Ice::Current&) override;

	/** @brief OffsetZ AO.
	@param DeltaZ the Z offset.
	@param side the side this command applies to.
	@return Result object */
	result OffsetZAO(double DeltaZ, string side, const Ice::Current&) override;

	/** @brief Sets the AO reference star
	@param list List of objects.
	@return Result object
	@note This is not an IIF/TCS command. */
	result SetReference(SeqPos list, const Ice::Current&) override;
	result SetReferenceNew(SeqNewPos list, const Ice::Current&) override;
	result SetReference2(SeqPos2 list, const Ice::Current&) override;

	/** @brief Removes the AO reference star from the system.
	@return Result object 
	@note This is not an IIF/TCS command. */
	result ClearReference(const Ice::Current&) override;

	/** @brief Use this command to log a message inside the TCS's logging system.
	@param event an event name that the instrument desires to log
	@param description the descriptive text that the instrument desires to log
	@return Result object */
	result LogEvent(string event, string description, const Ice::Current&) override;

	/** @brief Use this command to get the rotator trajectory for the LBC.
	@param seconds the number of seconds in the trajectory
	@param interval the time between points
	@param start the start time in MJD
	@param side the side this command applies to.
	@return Result object */
	result GetRotatorTrajectory(double seconds, double interval, double start, string side, const Ice::Current&) override;

	/** @brief Use this command to get rotator polynomials.
	@param start the start time in MJD (UT days)
	@param count the number of points in the trajectory
	@param interval the time between points (seconds)
	@param order the desired polynomial order (0,1,2)
	@param side the side this command applies to.
	@return Result object */
	result GetRotatorPolynomials(double start, int count, double interval, int order, string side, const Ice::Current&) override;

	/** @brief Use this command to set the standby level.
	@param level the standby level
	@param side the side this command applies to.
	@return Result object */
	result Standby(int level, string side, const Ice::Current&) override;

	/** @brief Use this command to to rotate the primary about a point on axis.
	@param distance the distance above the mirror about which to rotate
	@param angle the rotation angle
	@param direction the direction in shich to rotate
	@param side the side this command applies to.
	@return Result object */
	result RotatePrimary(double distance, double angle, double direction, string side, const Ice::Current&) override;

	/** @brief Use this command to get focal plane positions for LN AO reference objects.
	@param list the list of stars.
	@param side the side this command applies to.
	@return Result object */
	result GetKFPCoordinates(SeqPos list, string side, const Ice::Current&) override;
	result GetKFPCoordinatesNew(SeqNewPos list, string side, const Ice::Current&) override;
	result GetKFPCoordinates2(SeqPos2 list, string side, const Ice::Current&) override;

	/** @brief Use this command to get non-sidereal ephemeris from JPL Horizons.
	@param targetID the JJPL Horizons target ID.
	@param startDate the ephemeris start date.
	@param startTime the ephemeris start time.
	@param endDate the ephemeris end date.
	@param endTime the ephemeris end time.
	@param interval the ephemeris interval.
	@param fileName the ephemeris file name.
	@return Result object */
	result NSQuery(string targetID, string startDate, string startTime, string endDate, string endTime, string interval, string fileName, const Ice::Current&) override;

	//---------------------------------------------------------------------------------------------	
	/** @brief Removes the IIF object */
	void remove(const Ice::Current&) override;
	
private:

	// methods for commands that have multiple versions
	result OffsetPointing(double rotangle, double offx, double offy, string coordsys, string movetype, string side, string cmd); // 25-Mar-16
	result SetPMTerm(string name, double value, string movetype, string side, string cmd); // 25-Mar-16
	result SetOffset(double coord1, double coord2, string coordsys, string movetype, string cmd); // 25-Mar-16
	result SetStars(SeqPos2 list, string cmd); // 25-Mar-16
	result SetReference(SeqPos2 list, string cmd); // 25-Mar-16
	result GetKFPCoordinates(SeqPos2 list, string side, string cmd); // 25-Mar-16

	//!Command counter, increased with every command in execution
	int  __cmds;

	//!Saved order in GetRotatorPolynomials
	int __order;

	//!Recursive Ice mutex to protect the command counter operations.
	IceUtil::RecMutex __cmdsMutex;
	
	//! flags to setup the target, guide stars, hotspot, offset, nonSidereal, and reference.
	bool btarget;
	bool bguidestars;
	bool bhotspot;
	bool boffset;
	bool bnonSidereal;
	bool breference;
	
	//! The IIF object.
	IIF *anIIF;

	//! IIF Hotspot. See IIF Hotspot class. It is an optional parameter of the PresetTelescope command
	Hotspot hotspot;
	
	//! IIF Offset. See IIF Offset class. It is an optional parameter of the PresetTelescope command
	Offset offset;
	
	//! IIF Target. See IIF Position object. Must be defined before to call PresetTelescope.
	Position target;

	//! list of guide stars. See the IIF Position class.
	vector<Position*> guidestars;

	//! non-sidereal object
	NonSidereal nonSidereal; // was pointer 25-Mar-16
	
	//! AO reference star(s)
	Position reference; // was pointer 25-Mar-16
	vector<Position> LNReference;
	
	//! dd list to read each time we take an exposure or we use the GetParameter command.
	SeqDD __ddlist;

	//! The Ice communicator
 	Ice::CommunicatorPtr __comm;
	
	//! The Ice property object. Used to get the properties from the configuration file.
	Ice::PropertiesPtr __props;

	//! The proxy name
	string proxyInfo = "{name: INDEF, instrument: INDEF, focalStation: INDEF, side: INDEF}";
	
	/** @brief Executes the whole bunch of IIF steps involved during command execution and returns a DirtI Result object
	@param aResult IIF Result object 
	@param command String to be used by the logging system 
	@param log_flag Activates/Deactivates the server logging system.
	@return Result object */
 	result execute(Result *aResult, string cmd);
};

} /* namespace lbto */
