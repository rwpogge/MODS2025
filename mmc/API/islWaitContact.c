// islWaitContact.c

#include <string.h> // for memset
#include "islapi.h"

// -----------------------------------------------------------------------
// islWaitContact - accept a connection on port a. If no master sock is
// already open for the port, create one and record it in the port-to-
// socket table.
//
islconn
islWaitContact(islnum a)
{
  struct sockaddr_in sockaddr, csockaddr;
  int sock, newsock, i, csockaddrlen;
  static int p2sinit = 0;
  static struct port2sock p2s[P2S_SIZE];
  
  islAPIInit();
	
  if (a == 0)
    return -1;

  pthread_mutex_lock(&islWaitContact_mutex);
  
  if (p2sinit == 0) {
    (void) memset(p2s, 0, sizeof(p2s));
    p2sinit = 1;
  }
  
  // look up master socket in port-to-socket table
  for (sock = -1, i = 0; i < P2S_SIZE; i++) {
    if (p2s[i].port == a) {
      sock = p2s[i].sock;
      break;
    }
  }
  
  if (sock == -1) {
    // open new master socket
    
    // look for room in p2s table
    for (i = 0; i < P2S_SIZE; i++) {
      if (p2s[i].port == 0)
	break;
    }
    if (i == P2S_SIZE) {
      /* no room left in p2s */
      pthread_mutex_unlock(&islWaitContact_mutex);
      return -1;
    }
    
    sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
      pthread_mutex_unlock(&islWaitContact_mutex);
      return -1;
    }

    (void) memset(&sockaddr, 0, sizeof(struct sockaddr_in));
    sockaddr.sin_family = AF_INET;
    sockaddr.sin_port = htons(a);
    sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    
    if (bind(sock, (struct sockaddr *) &sockaddr,
	     sizeof(struct sockaddr_in)) < 0 ||
	listen(sock, LISTEN_Q_LEN) < 0) {
      close(sock);
      pthread_mutex_unlock(&islWaitContact_mutex);
      return -1;
    }
    
    // record master socket in table
    p2s[i].sock = sock;
    p2s[i].port = a;
  }
  pthread_mutex_unlock(&islWaitContact_mutex);
  
  csockaddrlen = sizeof(struct sockaddr_in);
  newsock = accept(sock, (struct sockaddr *) &csockaddr,(socklen_t *) &csockaddrlen);
  return newsock;
}
