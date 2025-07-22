// rte_cmpt.c - calculate computer time

#include <sys/types.h>
#include <sys/times.h>
#include <sys/time.h>
#include <unistd.h>


#include "instrutils.h"
#include "isl_funcs.h"
#include "params.h"
#include "isl_types.h"
#include "islcommon.h"
#include "isl_shmaddr.h"

void rte_cmpt(time_t *poClock,long *plCentiSec)
{
     struct timeval tv;
     long lRawTime;

     if(0!= gettimeofday(&tv, NULL)) {
       perror("getting timeofday, fatal\n");
       exit(-1);
     }
     *poClock=tv.tv_sec;
     *plCentiSec=(tv.tv_usec+5000)/10000;
     if(*plCentiSec>99) {
       *plCentiSec-=100;
       (*poClock)++;
     }

     return;
}
