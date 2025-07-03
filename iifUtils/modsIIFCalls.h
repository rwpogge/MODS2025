#ifndef MODSIIFCALLS_H
#define MODSIIFCALLS_H

//
// modsIIFCalls.h - MODS IIF function call definitions
//

/*!
  \file modsIIFCalls.h
  \brief Function calls

  \author Staff, OSU Dept. of Astronomy (rgonzale@astronomy.ohio-state.edu)
  \date 2010 Jan 29 [rdg]

  \par Modification History:
<pre>
   2010 Oct 26 - added coordSys to mods_OffsetPointing() [rwp/osu]
   2014 Jul 01 - added binocular and non-sidereal functions with
                 reference to document 481s013p of 2014 Feb 12 [rwp/osu]
</pre>
*/

//void StrUpper(char *);
// void GetArg(char *,int,char *);
// char *getDateTime(void);

char *mods_error();
int mods_IIFInit(char *, char *, char *);
int mods_OpenIceComm(char *, int , char *[]);
int mods_IIFClose();
char *mods_proxy();

// Implementation of IIF functions we need for MODS

int mods_Authorize();

// Binocular control

int mods_BinocularControl(char *);

// Telescope Control (general)

int mods_ClearStars();
int mods_MaximizeWrapTime(bool, bool, char *);
int mods_OffsetPointing(double, double, double, char *, char *, char *);
int mods_PresetTelescope(double, char *, char *, char *);
int mods_SetStars(char *, double, double, double, double, double,
	          char *, double, double, double, double, double, double,
	          float);
int mods_Remove();
int mods_UpdatePointingReference(char *,char *);
int mods_UpdateTargetWavelength(float, char *);
int mods_LogEvent(char *, char *);

// Guide-Collimation System (GCS) Functions

int mods_PauseGuiding(char *);
int mods_ResumeGuiding(char *);
int mods_SetAGWFilter(int, char*);

// Rotator Functions

int mods_RotReady(bool, char *);
int mods_RotServicePosition(double, char *);
int mods_RotHold(char *);

// Non-Sidereal Target Functions

int mods_SetNonSidereal(char *, float, char *, 
			double, double, double, double, double);
int mods_UpdateNonSiderealTarget(double, double);
int mods_ClearNonSidereal();

// Get/Set TCS and MODS Data Dictionary variables

int mods_SetParameter(char *, char *);
int mods_GetParameter(char *,char *);
int mods_GetMODSData(modsData_t &, char *);
int mods_GetTCSData(lbtData_t &, char *);
int mods_SetMODSData(modsData_t *, char *);


#endif  //   MODSIIFCALLS_H
