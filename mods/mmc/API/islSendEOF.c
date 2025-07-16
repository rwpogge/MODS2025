// islSendEOF.c - islSendEOF

#include "islapi.h"

/*------------------------------------------------------------------------
 * islSendEOF - signal EOF by shutting down send side of connection
 *------------------------------------------------------------------------
 */
int
islSendEOF(islconn conn)
{
  //return shutdown(conn, 1);
  close(conn);
  return(0);
}
