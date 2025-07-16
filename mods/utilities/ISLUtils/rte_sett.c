// rte_sett.c - set MODS and optionally system time

#include <sys/types.h>
#include <sys/times.h>
#include <time.h>
#include <string.h>


#include "instrutils.h"
#include "isl_funcs.h"
#include "params.h"
#include "isl_types.h"
#include "islcommon.h"
#include "isl_shmaddr.h"

//
// oGClock - Good time clock
// oGHs  - centiseconds to go with oGClock
// lCentiSec - raw cpu time corresponding to (oGClock,oGHs)
// mode - 'cpu' to set cpu time
//        'offset' to reset offset
//        'rate'   to measure rate
//
// ierr non-zero only if mode is 's' and stime fails
//

long rte_secs();

int rte_sett( time_t oGClock, int iGHs, long lCentiSec, char *sMode)
{
    int iIndex, ierr;
    char model;
    int iComputer;

    iIndex = 01 & shm_addr->time.index;
    model = shm_addr->time.model;
    iComputer= shm_addr->time.icomputer[iIndex];

    if (!strcmp(sMode,"cpu")) {
        rte_sleep(25);
        //ierr=stime(&oGClock);
        ierr=time(&oGClock);
        shm_addr->time.secs_off = rte_secs();
    } else {
       time_t oCpuClock;
       int iCpuHs;
       long lEpoch, lOffset, lDiffHs, lSpan;
       float fRate;

       oCpuClock=lCentiSec/100+shm_addr->time.secs_off;
       iCpuHs = lCentiSec % 100;
       lDiffHs = (oGClock-oCpuClock)*100+iGHs-iCpuHs;

       lEpoch = shm_addr->time.epoch[iIndex];
       fRate = shm_addr->time.rate[iIndex];
       lOffset = shm_addr->time.offset[iIndex];
       lSpan = shm_addr->time.span[iIndex];

       if(!strcmp(sMode,"offset")) {
	 // don't update rate, but save the other stuff
         lEpoch=lCentiSec;
         lOffset = lDiffHs;
       } else if (!strcmp(sMode,"adapt") || !strcmp(sMode,"rate") ) {
	 // update the rate
         lSpan = lCentiSec-lEpoch;
         fRate=((double)(lDiffHs-lOffset))/lSpan;
       }

       iIndex = 01 & ~iIndex;
       shm_addr->time.rate[iIndex] = fRate;
       shm_addr->time.span[iIndex] = lSpan;
       shm_addr->time.offset[iIndex] = lOffset;
       shm_addr->time.epoch[iIndex] = lEpoch;
       shm_addr->time.icomputer[iIndex] = iComputer;
       shm_addr->time.index = iIndex;

       ierr=0;
    }

    return ierr;
}
