// islCnameToComp.c

#include "islapi.h"

/*-----------------------------------------------------------------------
 * islCnameToComp - look up a host by name and return its IP address
 *-----------------------------------------------------------------------
 */
islcomp
islCnameToComp(char *cname)
{
  islcomp c;
  struct hostent *hp;
  
  islAPIInit();
  
  pthread_mutex_lock(&cname_mutex);

  hp = gethostbyname(cname);
  if (hp == NULL) {
    pthread_mutex_unlock(&cname_mutex);
    return -1;
  }
	
  if (hp->h_addrtype != AF_INET ||
      hp->h_length != sizeof(islcomp)) {
    pthread_mutex_unlock(&cname_mutex);
    return -1;
  }
	  
  c = *((long *) hp->h_addr);
  pthread_mutex_unlock(&cname_mutex);
  return c;
}
