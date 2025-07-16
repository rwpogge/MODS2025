#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/times.h>
#include <time.h>
#include "instrutils.h"
#include "isl_funcs.h"
#include "params.h"
#include "isl_types.h"
#include "islcommon.h"
#include "isl_shmaddr.h"


main()
{
  time_t clock1, clock2, clock1_old, clock2_old;
     struct timeval tv;
     long times();
     struct tms buffer;
     long lRawTime,lRawTime_old, tv_sec_old;

     clock1=0;
     clock2=1;
     clock2=time(&clock2)+10;
     printf("clock2 %10ld\n", clock2);
     while(1){
        clock1=time(&clock1);       /* bracket the centi-seconds */
        lRawTime=times(&buffer);
	if(0!= gettimeofday(&tv, NULL)) {
	  perror("getting timeofday, fatal\n");
	  exit(-1);
	}
	//clock2=time(&clock2);
	printf(" clock1 %10ld\n", clock1);
	if (clock2<=clock1) {
	//	if(clock1!=clock1_old||lRawTime!=lRawTime_old ||
	//   tv.tv_sec!=tv_sec_old||clock2!=clock2_old) {
	  printf(" clock1 %10ld raw %10ld secs %10ld usc %10ld clock2 %10ld\n",
		 clock1,lRawTime,tv.tv_sec,tv.tv_usec,clock2);
	  //  clock1_old=clock1;
	  //  lRawTime_old=lRawTime;
	  // tv_sec_old=tv.tv_sec;
	  //  clock2_old=clock2;
	  //	}
	break;
	}
	sleep(1);
     }

}
