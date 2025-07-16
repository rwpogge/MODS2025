// islAppNameToAppNumb.c

#include "islapi.h"

/*-----------------------------------------------------------------------
 * islAppNameToAppNumb - look up a port by service name
 *-----------------------------------------------------------------------
 */
islnum
islAppNameToAppNumb(char *appname)
{
  
  struct servent		*sp;
  islnum			port;
  
  islAPIInit();
  
  pthread_mutex_lock(&appname_mutex);
  
  sp = getservbyname(appname, "tcp");
  if (sp == NULL) {
    pthread_mutex_unlock(&appname_mutex);
    return -1;
  }
  
  port = ntohs(sp->s_port);
  pthread_mutex_unlock(&appname_mutex);
  return port;
}
