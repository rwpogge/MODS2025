#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <sys/sendfile.h>

using namespace std;

#include "instrutils.h"  
#include "isl_funcs.h"  
#include "isl_types.h"  
#include "params.h"      // Common parameters and defines
#include "islcommon.h"   // Common parameters and defines
#include "isl_shmaddr.h" // Shared memory attachment.

int
checkForError(int error_number,char *reply) 
{
  int nread, n;
  char temp[132];   // temp. message buffer
  string messages;
  string who;

  ifstream in("/home2/mods/mmc_3.0.0/mmcServers/MicroLynx_err.txt");
  
  while (getline(in,who)) {
    istringstream s(who); // Get the first integer from the line
    s >> n; // Get the first integer from the line
    if( n == error_number) { // Compare with COMM read.
      strcpy(reply,&who[0]);
      break;
    }
  }
  return n;
}
