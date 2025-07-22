/** \file getCurrentTime.c
 * \brief This module implements time functions, e.g. receiving the actual time
 */

#include <sys/types.h>
#include <sys/times.h>
#include <sys/time.h>
#include <time.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "mods_timefuncts.h"

void delay(long milliseconds)
{
//   	struct timeval timeout;

//   	timeout.tv_sec = milliseconds / 1000000L;
//   	timeout.tv_usec = milliseconds % 1000000L;
//   	select(0, NULL, NULL, NULL, &timeout);
//
	struct timespec s;
	s.tv_sec = milliseconds / 1000;
	s.tv_nsec = (milliseconds - (s.tv_sec * 1000)) * 1000000;
	nanosleep(&s, NULL);
}

//int getCurrentTime(char* timestring, char* formatstring)
char * getCurrentTime(char* formatstring)
{
	time_t now;
	struct tm *tm;
	struct timeval *tv=(struct timeval*)malloc(sizeof(struct timeval));
	struct timezone *tz=(struct timezone*)malloc(sizeof(struct timezone));
	int usec;
	char usecstr[4];
	char fillzeros[4];
        char timestring[256];

	time(&now);
	tm=(struct tm*)localtime(&now);
	gettimeofday(tv,tz);
	usec=(int)tv->tv_usec/1000;
	if(usec<10)strcpy(fillzeros,"00");
	else if(usec<100)strcpy(fillzeros,"0");
	else strcpy(fillzeros,"");
	strftime(timestring,255,formatstring,tm);
	
	strcat(timestring,".");
	strcat(timestring,fillzeros);
	
	sprintf(usecstr,"%i",usec);
	strcat(timestring,usecstr);

	//printf("formatsting:%s\ntimestring:%s\n", formatstring, timestring);

	//	return(0);
	return(timestring);
}
