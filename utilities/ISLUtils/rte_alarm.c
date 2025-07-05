#include <stdio.h>
#include <sys/time.h>
#include <stdlib.h>
#include <errno.h>      // error code definition header file
#include <sys/types.h>

int rtalarm();
void exit(int);
void rte_ticks(long);

unsigned rte_alarm(unsigned centisec)
{
    int time;
    struct itimerval value;

    time=centisec;
    if(0>(int)centisec) // correct for unsigned overflows so we at least
        time=((unsigned) ~0)>>1; // won't abend
 

    value.it_interval.tv_sec=0L;
    value.it_interval.tv_usec=0L;
    value.it_value.tv_sec=(long) (time/100);
    value.it_value.tv_usec=(long) ((time%100)*10000);

    if(-1==setitimer(ITIMER_REAL,&value,0)) {
      perror("rte_alarm");
      printf("rte_alarm: error setting alarm, time %d\n",time);
      exit(-1);
    }
    return(0);
}
