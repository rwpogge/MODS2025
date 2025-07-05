// pname, putpname
// Put/get program name for logging 
//
#include <string.h>
static char sname[7]={' ',' ',' ',' ',' ',' ',' '};

void putpname(char *name)
{
  memcpy(sname,name,7);
}

void pname(char *name)
{
  memcpy(name,sname,7);
}

