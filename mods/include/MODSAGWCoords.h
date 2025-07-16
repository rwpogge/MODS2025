#ifndef MODSAGWCoords_H
#define MOSDAGWCoords_H

typedef struct spos_xy
{
  double x;  //!< X-axis coordinate
  double y;  //!< Y-axis coordinate
} spos_xy;

typedef double angle;  //!< angular coordinate

#ifdef __cplusplus
extern "C" {

  // Coordinate Transformation Functions

  int mods_SFP2AGWgeneric(char *, int, spos_xy, angle, spos_xy *);
  int SFPToWFS(char *, int, spos_xy, angle, spos_xy *);
  int mods_AGWgeneric2SFP(char *, int, spos_xy, angle, spos_xy *);
  int mods_CCDgeneric2SFP(char *, int, spos_xy, spos_xy, angle, spos_xy *);
  int CCDToAGW(char *, int, spos_xy, spos_xy, angle, spos_xy *);
  int CCDToWFS(char *, int, spos_xy, angle, spos_xy *);
  int mods_getPupilRotation(char *, int, angle, angle *);

  // Coordinate Utility Functions

  int XYInRange(char *, int, spos_xy);

}
#endif

//---------------------------------------------------------------------------
//
// MODS AGW Metric Parameters
//
//   All rotational misalignment angles are measured relative to the
//   direct Gregorian rotator, which in practice is relative to
//   celestial coordinates at rotator PA=0.
//
//   All parities are measured relative to celestial standard
//   coordinates at rotator PA=0.
//
// Update History:
//   2008 Jan 30 - Initial setup [rwp/osu]
//   2008 Aug 18 - New AGW x and y sensor config [rwp/osu]
//
//---------------------------------------------------------------------------

// MODS1 - SX Gregorian Focus

#define MODS1  1

#define MODS1_SFPRADtoMM   123765.76 //!< LBT f/15 SX direct Gregorian Image Scale in mm/radian
#define MODS1_AGWXParity           1 //!< Parity of the MODS1 AGW stage X-axis relative to SFP [+/-1]
#define MODS1_AGWYParity           1 //!< Parity of the MODS1 AGW stage Y-axis relative to SFP [+/-1]
#define MODS1_AGWRot             0.0 //!< Rotational Misalignment of the MODS1 AGW stage in radians
#define MODS1_AGWXMin          -92.0 //!< Minimum MODS1 AGW stage X-axis position in mm
#define MODS1_AGWXMax           92.0 //!< Maximum MODS1 AGW stage X-axis position in mm
#define MODS1_AGWYMin         -184.0 //!< Minimum MODS1 AGW stage Y-axis position in mm
#define MODS1_AGWYMax            0.0 //!< Maximum MODS1 AGW stage Y-axis position in mm
#define MODS1_AGWFMin            0.0 //!< Minimum MODS1 AGW focus probe position in mm
#define MODS1_AGWFMax           64.0 //!< Maximum MODS1 AGW focus probe position in mm

#define MODS1_CCDPIXtoRAD 5.6817E-07 //!< MODS1 Guider CCD Pixel Scale in radians/pixel
#define MODS1_CCDPixSize       0.015 //!< MODS1 Guider CCD Pixel Size in millimeters
#define MODS1_CCDXParity          -1 //!< Parity of the MODS1 Guider CCD X-axis relative to SFP [+/-1]
#define MODS1_CCDYParity           1 //!< Parity of the MODS1 Guider CCD Y-axis relative to SFP [+/-1]
#define MODS1_CCDRot             0.0 //!< Rotational Misalignment of the MODS1 Guider CCD in radians
#define MODS1_CCDXRef          256.0 //!< MODS1 Guider CCD X-axis reference pixel
#define MODS1_CCDYRef          256.0 //!< MODS1 Guider CCD Y-axis reference pixel

#define MODS1_WFSXOffset        0.00 //!< X-axis Offset of the MODS1 WFS pickoff mirror from the guider in millimeters
#define MODS1_WFSYOffset       12.50 //!< Y-axis Offset of the MODS1 WFS pickoff mirror from the guider in millimeters
#define MODS1_WFSXParity          -1 //!< Parity of the MODS1 WFS CCD X-axis relative to SFP [+/-1]
#define MODS1_WFSYParity           1 //!< Parity of the MODS1 WFS CCD Y-axis relative to SFP [+/-1]
#define MODS1_WFSRot             0.0 //!< Rotational Misalignment of the MODS1 WFS CCD in radians

// MODS2 - DX Gregorian Focus

#define MODS2  2

#define MODS2_SFPRADtoMM   123765.76 //!< LBT f/15 DX direct Gregorian Image Scale in mm/radian
#define MODS2_AGWXParity           1 //!< Parity of the MODS2 AGW stage X-axis relative to SFP [+/-1]
#define MODS2_AGWYParity           1 //!< Parity of the MODS2 AGW stage Y-axis relative to SFP [+/-1]
#define MODS2_AGWRot             0.0 //!< Rotational Misalignment of the MODS2 AGW stage in radians
#define MODS2_AGWXMin          -92.0 //!< Minimum MODS2 AGW stage X-axis position in mm
#define MODS2_AGWXMax           92.0 //!< Maximum MODS2 AGW stage X-axis position in mm
#define MODS2_AGWYMin         -184.0 //!< Minimum MODS2 AGW stage Y-axis position in mm
#define MODS2_AGWYMax            0.0 //!< Maximum MODS2 AGW stage Y-axis position in mm
#define MODS2_AGWFMin            0.0 //!< Minimum MODS1 AGW focus probe position in mm
#define MODS2_AGWFMax           64.0 //!< Maximum MODS1 AGW focus probe position in mm

#define MODS2_CCDPIXtoRAD 5.6817E-07 //!< MODS2 Guider CCD Pixel Scale in radians/pixel
#define MODS2_CCDPixSize       0.015 //!< MODS2 Guider CCD Pixel Size in millimeters
#define MODS2_CCDXParity          -1 //!< Parity of the MODS2 Guider CCD X-axis relative to SFP [+/-1]
#define MODS2_CCDYParity           1 //!< Parity of the MODS2 Guider CCD Y-axis relative to SFP [+/-1]
#define MODS2_CCDRot             0.0 //!< Rotational Misalignment of the MODS2 Guider CCD in radians
#define MODS2_CCDXRef          256.0 //!< MODS2 Guider CCD X-axis reference pixel
#define MODS2_CCDYRef          256.0 //!< MODS2 Guider CCD Y-axis reference pixel

#define MODS2_WFSXOffset         0.0 //!< X-axis Offset of the MODS2 WFS pickoff mirror from the guider in millimeters
#define MODS2_WFSYOffset        12.5 //!< Y-axis Offset of the MODS2 WFS pickoff mirror from the guider in millimeters
#define MODS2_WFSXParity          -1 //!< Parity of the MODS2 WFS CCD X-axis relative to SFP [+/-1]
#define MODS2_WFSYParity           1 //!< Parity of the MODS2 WFS CCD Y-axis relative to SFP [+/-1]
#define MODS2_WFSRot             0.0 //!< Rotational Misalignment of the MODS WFS2 CCD in radians


// AGW Coordinate Transformation Function Error Codes

#define ERROR_UNKNOWN_MODSID   215 //!< User requested an unknown MODS instance (must be MODS1 or MODS2)
#define ERROR_GPX_OUTOFRANGE   115 //!< Guide probe X coordinate is out of range
#define ERROR_GPY_OUTOFRANGE   116 //!< Guide probe Y coordinate is out of range

#endif  // MODSAGWCoords_H
