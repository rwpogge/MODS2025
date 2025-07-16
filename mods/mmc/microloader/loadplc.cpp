/*!
  \file islpp.c
  \brief islpp - SNTools ISLProcedureProcessor.

  \author Staff, OSU Dept. of Astronomy (rgonzale@astronomy.ohio-state.edu)
  \date 2005 March 20 [rdg]

  \par Usage: Unix style
  islpp <filename>.prc  - without extension.

  ISLProcedureProcessor will run a procedure file.

*/
#include <iostream>
#include <sstream>
#include <fstream>
#include <cstring>
using namespace std;

#include "../include/StrConv.h"   // Command action functions header file
#include "../include/instrutils.h"   // Command action functions header file
#include "../include/isl_funcs.h"
#include "../include/isl_types.h"
#include "../include/params.h"      // Common parameters and defines
#include "../include/islcommon.h"     // Common parameters and defines
#include "../include/isl_shmaddr.h"     // Shared memory attachment.

#define NSEMNAME "isl    "
int nsem_test(char []);
//----------------------------------------------------------------
int
main(int argc, char *argv[])
{
  int i,ierr=0;
  int device;
  char inbuf[MAXPGMLINE];
  char mechanism[MAXPGMLINE];
  char plcFile[MAXPGMLINE];
  FILE *fp;

  /*
   * First lets play it safe and clear the buffers.
   */
  memset(mechanism,0,sizeof(mechanism)); 
  memset(plcFile,0,sizeof(plcFile)); 
  memset(inbuf,0,sizeof(inbuf));

  strcpy(mechanism,argv[1]);

  sprintf(plcFile,"/home2/mods/plc/%s.plc",mechanism);

  plcFile[strlen(plcFile)]='\0'; // Do not want seg. faults at any time.
  
  device=getMechanismID(mechanism,reply);
  if(device==-1) return CMD_ERR;

  /*
   * Now open the program file, return a -9 if file is in error.
   */
  if (!(fp=fopen(plcFile, "r"))) {
    sprintf(reply,"Could not open file %s",plcFile);
    return CMD_ERR;
  }

  /* ----------------------------------------------------------------
   * Config file parser loop
   * 
   * Read in each line of the config file and process it 
   */
  //cout << "[ML" << device << "]loading: " << plcFile << endl;

  //----------------------------------------------------------------
  // Read in each line of the PLC code and process it 
  //
  shm_addr->MODS1.busy[device]=1; // Hold the IP until finished

  ierr=mlcCmdOnly(device,reply); // Send a Raw command no reply required
  if(ierr<0) {
    shm_addr->MODS1.busy[device]=0;    // Clear the HOST busy bit.
    return CMD_ERR;
  }
  ierr=rawCommandOnly(device,"IP ALL"); // Send a Raw command
  MilliSleep(100);
  ierr=rawCommandOnly(device,"DVF 0,0,0"); // Send a Raw command
  MilliSleep(100);
  ierr=rawCommandOnly(device,"CP 1,1"); // Send a Raw command
  MilliSleep(1000);
  ierr=rawCommandOnly(device,"SAVE"); // Send a Raw command

  while (fgets(inbuf, MAXPGMLINE, fp)) {
    if(strstr(inbuf,"END HEADER")) break;
  }

  while(fgets(inbuf, MAXPGMLINE, fp)) {
    /*
     * Skip comments (#,*) and blank lines
     */
    if (inbuf[0] != '#' && 
	inbuf[0] != '\n' && 
	inbuf[0] != '*' && 
	inbuf[0] != '\'' && 
	inbuf[0] != '\0') {
      
      inbuf[strlen(inbuf)]='\r';
      ierr=rawCommandOnly(device,inbuf); // Send a Raw command
      fprintf(stderr,"%s",inbuf);

    }
    memset(inbuf,0,sizeof(inbuf));
    TTYMSleep(200); // 100ms gives the port server a chance to complete
  }

  ierr=mlcCmdReply(device,reply); // Send a Raw command
  if(ierr<0) {
    shm_addr->MODS1.busy[device]=0;    // Clear the HOST busy bit.
    return CMD_ERR;
  }

  MilliSleep(1000);
  ierr=rawCommandOnly(device,"SAVE"); // Send a Raw command

  shm_addr->MODS1.busy[device]=0;  // Clear the HOST busy bit.
  sprintf(reply,"[ML%d]loaded %s ",device,plcFile);
    
  if (fp!=0)
    fclose(fp);

  exit(0);
}
