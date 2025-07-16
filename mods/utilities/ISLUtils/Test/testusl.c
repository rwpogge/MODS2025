#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/times.h>
#include <time.h>
#include <unistd.h>

//void usleep(unsigned int);

main()
{
     clock_t times();
     struct tms buffer;
     long start, end;
     int i;

     for(i=0;i<100;i++){
       start=times(&buffer);
       usleep((unsigned) 1000);
       end=times(&buffer);
       if(end-start<1) printf("short wait\n");
     }
}
