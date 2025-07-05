// islAPIInit.c - islAPIInit

#include "islapi.h"
#include <stdio.h>

pthread_mutex_t	islWaitContact_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t cname_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t	appname_mutex = PTHREAD_MUTEX_INITIALIZER;

int initapi = 0;

// -----------------------------------------------------------------------
// islAPIInit - initialize the library. Initialize and create mutexes.
//
// Note: All ISLAPI functions call islAPIInit first to ensure that
// the library is initialized. This is done for easier use.
//
//
// ** Having the functions call islAPIInit first creates
// ** a race condition in multi-threaded applications.
// ** If your application is multithreaded, call islAPIInit explicitly
// ** before you spawn any threads.
//
void
islAPIInit(void)
{
  if (initapi != 0)
    return;
  initapi = 1;
}
