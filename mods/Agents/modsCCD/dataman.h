#ifndef DATAMAN_H
#define DATAMAN_H

/*!
  \file dataman.h
  \brief Data Manager agent communications header
 
  These functions provide the interface to the Data Manager agent (aka
  DataMan).

  DataMan hooks are required for running y4kcam standalone, otherwise
  the routines will communicate with DataMan via the ISIS network.
  This requires some client awareness here.

  \author R. Pogge, OSU Astronomy Dept. (pogge@astronomy.ohio-state.edu)
  \date 2005 June 7

*/

// system header files 

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <errno.h>
#include <termios.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/times.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/file.h>
#include <netdb.h>

#include "isisclient.h" // ISIS client header, useful utils

#ifndef MAXCFGLINE
#define MAXCFGLINE 128 //!< Maximum characters/line in runtime config files
#endif

//----------------------------------------------------------------
//
// Data Manager Configuration Table
//

/*!
  \brief Data Manager interface configuration data structure

  Information about the interface to a dataman agent application.

*/

typedef struct dmConfig {

  // First things first:

  int  useDM;   //!< 1 = Instrument uses dataman, 0 = no dataman
  char cfgFile[128]; //!< Name of a DataMan configuration file loaded into this struct (if used)

  // IMPv2 Identity of this application

  char Name[9];      //!< IMPv2 node name of this application

  // Basic Parameters of the remote DataMan agent

  char Host[64];    //!< hostname of the DataMan agent application
  int  Port;        //!< Port number of the DataMan agent application
  int  FD;   	    //!< file descriptor for the DataMan agent comm link socket
  long Timeout;     //!< communications timeout interval in seconds
  char ID[9];       //!< IMPv2 node name of the remote DataMan agent

} dataman_t;

//---------------------------------------------------------------------------

// String sizes 

#define DM_MSGSIZE  1024   //!< max message length we expect (overkill)
#define DM_TIMEOUT   30L   //!< DataMan comm timeout interval in seconds

// DataMan agent interface functions (see dataman.c for definitions)

int  openDM(dataman_t *, int);
int  readDM(dataman_t *, char *);
int  writeDM(dataman_t *, char *);
void closeDM(dataman_t *); 
void initDM(dataman_t *);
int  loadDMConfig(dataman_t *, char *, char *);
void dmInfo(dataman_t *);

#endif
