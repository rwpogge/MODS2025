/*!
  \file islsmc.c
  \brief islsmc - Utility for ISL Single Microlynx Command

  \author Staff, OSU Dept. of Astronomy (rgonzale@astronomy.ohio-state.edu)
  \date 2005 March 20 (rdg)

  \par Usage:
  islsmc controller "ascii string" 

  NOTE: /home2/isl/include/instrutil.h file has more information.

  MODS 1 Controllers are:
  0=DARKSLIDE, 1=CALIBRATION, 2=MASKI, 3=MASKS, 4=AGWX, 5=AGWY, 6=AGWPF, 
  7=AGWFW, 8=DICHROIC, 9=TIP, 10=TILT, 11=FOCUS, 12=GT1, 13=GT2, 14=GS, 
  15=SHUTTER, 16=CAMERAPF, 17=CAMERAFW, 18=RED_TIP, 
  19=RED_TILT, 20=RED_FOCUS, 21=RED_GT1, 22=RED_GT2, 23=RED_GS, 
  24=RED_SHUTTER, 25=RED_CAMERAPF, 26=RED_CAMERAFW

  Command motor(s) to open or close controller
*/

#include "instrutils.h"   // Command action functions header file
#include "isisclient.h" // ISIS common client library header
#include "ics_client.h"     // Custom client application header file
#include "isl_funcs.h"
#include "isl_types.h"
#include "params.h"      // Common parameters and defines
#include "islcommon.h"     // Common parameters and defines
#include "isl_shmaddr.h"     // Shared memory attachment.

//extern rmcrlf(char [], char []);
//----------------------------------------------------------------
int
islsmc(int controller,char cmdstr[])
{
  int err;
  char moving[64];
  char monit[20];

  setup_ids();

  if ( shm_addr->MODS.busy[controller] == 1) {
    //fprintf( stderr,"Controller is busy\n");
    //fprintf( stderr,"Ask ISL to give up control");
    //fprintf( stderr,"type: smcrelease <controller id>\n");
    exit( -905);
  } else shm_addr->MODS.busy[controller] = 1;

  // Which mechanism....0-32
  strcpy(shm_addr->MODS.commport[controller].Port,shm_addr->MODS.TTYIP[controller]);

  shm_addr->MODS.modsx=1;
  if ( OpenTTYPort(&shm_addr->MODS.commport[controller]) == -1) {
    logit(NULL,-102,"CP");
    exit(-102);
  }
  StrToUpper(cmdstr);

  if (strstr(cmdstr,"INITIAL")) {
    // Initialize the Controller
    WriteTTYPort(&shm_addr->MODS.commport[controller],"EXEC INITIAL\r");
    sprintf(monit,"smcbusy %d &",controller);

  } else if (strstr(cmdstr,"HOME")) {
    // HOME
    WriteTTYPort(&shm_addr->MODS.commport[controller],"EXEC HOME\r");
    sprintf(monit,"smcbusy %d &",controller);
  
  } else if (strstr(cmdstr,"STOP")) {

    WriteTTYPort(&shm_addr->MODS.commport[controller],"SSTP 1\r\n");

  } else {

    WriteTTYPort(&shm_addr->MODS.commport[controller],cmdstr);
    sprintf(monit,"smcbusy %d &",controller);
  }

  shm_addr->MODS.busy[controller] = 0;  
  CloseTTYPort(&shm_addr->MODS.commport[controller]);
  system(monit);
  return(0);
}
