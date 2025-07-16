/*!
  \file islcmd.c
  \brief islcmd - ISL system command

  \author Staff, OSU Dept. of Astronomy (rgonzale@astronomy.ohio-state.edu)
  \date 2005 March 20

  \par Usage:
  islcmd(controller,cmd_string,value);

  Get/Command status/motor (good friend of quickC) 

  <pre>
  2005 March 20 - new application [rdg]
  </pre>
*/

#include "instrutils.h"   // utililties function header file
#include "ics_client.h"   // Custom client application header file
#include "isl_funcs.h"    // functions header file
#include "params.h"       // Common parameters and defines
#include "islcommon.h"    // Common parameters and defines
#include "isl_shmaddr.h"  // Shared memory attachment.

//----------------------------------------------------------------
int
islcmd(int controller,char setcmd[],int value)
{
  char sendcmd[64];

  memset(sendcmd,0,sizeof(sendcmd));
  sprintf(sendcmd,"/home2/isl/bin/Comtrol %d %s %d &",controller,setcmd,value);
  sendcmd[strlen(sendcmd)]='\0';
  system(sendcmd);
  return(0);
}
