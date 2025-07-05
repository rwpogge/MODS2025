#ifndef COORDTEST_H
#define COORDTEST_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "MODSAGWCoords.h" // MODS AGW coordinate transform header

// convenience macros

#ifndef PI
#define PI              3.141592653589793
#endif

// angular unit conversions

#define DegToRad(x)    ((x)*PI/180.0)
#define RadToDeg(x)    ((x)*180./PI)
#define HoursToDeg(x)  ((x)*15.)
#define DegToHours(x)  ((x)/15.)
#define HoursToRad(x)  DegToRad(HoursToDeg(x))
#define RadToHours(x)  DegToHours(RadToDeg(x))
#define RadToArcsec(x) (3600.0*RadToDeg(x))
#define ArcsecToRad(x) (DegToRad(x)/3600.0)

#endif // COORDTEST_H
