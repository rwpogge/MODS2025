/* islapi.h */

#ifndef _ISLAPI_H_
#define _ISLAPI_H_

#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h>
#include <unistd.h>
#include <cstring>
#include <sys/types.h>

typedef short	islnum;
typedef long	islcomp;
typedef int	islconn;

struct port2sock {
	short	port;
	int	sock;
};

#define P2S_SIZE 64 // number of entries in port to socket (port2sock) map table
#define LISTEN_Q_LEN 10

islnum  islAppNameToAppNumb(char *islname);
islcomp islCnameToComp(char *islcname);
islconn islWaitContact(islnum a);
islconn islMakeContact(islcomp c, islnum a);
int islSendEOF(islconn c);
void islAPIInit(void);

extern pthread_mutex_t islWaitContact_mutex, cname_mutex, appname_mutex;

#endif // !defined(_ISLAPI_H_)
