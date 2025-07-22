// rte_time.c - return rte format time buffer

#include <sys/types.h>
#include <time.h>

#include "instrutils.h"  // ISL Instrument header
#include "params.h"
#include "isl_types.h"
#include "islcommon.h"
#include "isl_shmaddr.h"

void rte_rawt(long *);
void rte_fixt(time_t *, long *);

void rte_time(int it[5],int *it6)
{
     struct tm *ptr;
     time_t clock;
     long centisec;

     rte_rawt(&centisec);  // retrieve the raw time

     rte_fixt(&clock, &centisec); // correct for clock drift model

     ptr=gmtime(&clock);            // store in rte exec(11 time buffer
     it[0] = centisec%100;
     it[1]=ptr->tm_sec;
     it[2]=ptr->tm_min;
     it[3]=ptr->tm_hour;
     it[4]=1+ptr->tm_yday;
     
     *it6=1900+ptr->tm_year;

     return;
}
