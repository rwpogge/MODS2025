// islMakeContact.c

#include <string.h> // for memset

#include "islapi.h"

/*-----------------------------------------------------------------------
 * islMakeContact - open a new TCP connection to the specified IP address
 * (c) and port e(a).
 *-----------------------------------------------------------------------
 */
islconn
islMakeContact(islcomp c, islnum a)
{
  struct sockaddr_in sockaddr;
  int sock;
  
  islAPIInit();
  
  sock = socket(PF_INET, SOCK_STREAM, 0);
  if (sock < 0)
    return -1;
  
  (void) memset(&sockaddr, 0, sizeof(struct sockaddr_in));
  
  sockaddr.sin_family = AF_INET;
  sockaddr.sin_port = htons(a);
  sockaddr.sin_addr.s_addr = c;
  
  if (connect(sock, (struct sockaddr *) &sockaddr, sizeof(struct sockaddr_in)) < 0) {
    close(sock);
    return -1;
  }  
  return sock;
}
   
