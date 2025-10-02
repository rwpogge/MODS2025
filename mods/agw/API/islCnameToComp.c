// islCnameToComp.c

#include "islapi.h"

//-----------------------------------------------------------------------
//  islCnameToComp - look up a host by name and return its IP address
//
//  modified for 64-bit (Archon update)
//  note that gethostbyname() is deprecated, should replace with 
//  getaddrinfo() eventually
//
//-----------------------------------------------------------------------

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

  // old test of h-addrtype vs. sizeof(islcomp) breaks on 64-bit machines
  //if (hp->h_addrtype != AF_INET || hp->h_length != sizeof(islcomp)) {

  if (hp->h_addrtype != AF_INET) {
    pthread_mutex_unlock(&cname_mutex);
    return -1;
  }
	  
  c = *((long *) hp->h_addr);
  pthread_mutex_unlock(&cname_mutex);
  return c;
}
