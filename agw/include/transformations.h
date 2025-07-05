/** \file transformations.h
 * \brief This Header defines the prototypes for the coordinate system
 * transformation functions.
 */

#ifndef TRANSFORMATIONS_H_
#define TRANSFORMATIONS_H_

#include "agwcomm.h"

#ifdef __cplusplus
extern "C" {

// int SFP2AGWgeneric(char* host, int agw_no, spos_xy tcssfp, angle pa, spos_xy *agwgeneric, angle *gprot);
int mods_SFP2AGWgeneric(char* host, int agwno, spos_xy sfp, angle pa, spos_xy *agw);    // removed the gprot as there is no change in rotation for MODS, TL 10/19/09

int mods_AGWgeneric2SFP(char* host, int agwno, spos_xy agw, angle pa, spos_xy *sfp);

int mods_SFP2CCDgeneric(char* host, int agwno, spos_xy tcssfp, spos_xy ccdref, angle pa, angle gprot, spos_xy *ccdgeneric);

int mods_CCDgeneric2SFP(char* host, int agwno, spos_xy ccd, spos_xy ccdref, angle pa, spos_xy *sfp);

/*
int WFSgeneric2SFP(char* host, int agw_no, spos_xy wfsgeneric, spos_xy ccdref, spos_xy *tcssfp);
*/

int mods_getFocusOffset(char* host, int agw_no, spos_xy agwgeneric, int filterid, double *foffset);

int mods_getPupilCenter(char* host, int agw_no, spos_xy agwgeneric, angle gprot, spos_xy *pupil);

int mods_getPupilRotation(char* host, int agw_no, angle gprot, angle derot, angle *mlarot);

}
#endif

#endif

