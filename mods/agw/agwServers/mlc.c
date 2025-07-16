#include "ISLSocket.h"        // For Socket and SocketException
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <vector>
#include <cstdlib>            // For atoi()
#include "timer.h" // Timer
#include "islapi.h"

using namespace std;

#define BUFFSIZE		512
const int RCVBUFSIZE = 512;    // Size of receive buffer

extern int reusePort(int);

#define IO20   "PRINT IO 20\r"

static char *check_status[4] =
  {"PRINT BSY\r",
   "PRINT ERR\r",
   "PRINT HELD\r",
   "PRINT MVG\r"
  };
 
int checkForError(int,char *);
char argbuf[32];

//---------------------------------------------------------------------------
// makeUpper() - return all characters in a string in uppercase
//
 
/*!
  \brief Return uppercase string
 
  \param str2 string remains unchanged.
  \param str  returned as uppercase string, 
 
  Helper utility will return requested string in uppercase, and leave
  the requested string unchanged.
 
*/
 
char* makeUpper(char str2[])
{
  int i;
  char str[strlen(str2)+1];

  strcpy(str,str2);

  for(i=0;i<strlen(str);i++)
    if(str[i] >= 'a' && str[i] <= 'z')
      str[i] = toupper(str[i]);

  return str;
}

//---------------------------------------------------------------------------
// StrUpper() - convert all characters in a string to uppercase
//
 
/*!
  \brief Convert all characters in a string to uppercase
 
  \param str String to be converted to uppercase. Changed by this function.
 
  Helper utility to convert all characters in a string to uppercase.
  All commands are required to be in uppercase.
 
*/
 
void
StrUpper(char *str)
{
  int i;
  for(i=0;i<strlen(str);i++)
    if(str[i] >= 'a' && str[i] <= 'z')
      str[i] = toupper(str[i]);
}

//---------------------------------------------------------------------------
//
// openCommand - Open command port to a mechanism
//

/*!
  \brief Open a command port to a mechanism

  \param i - port of mechanism
  \param dummy string to contain reply

  \return Returns CMD_OK on success, CMD_ERR on errors.

  Opens a command port to the named MicroLYNX controller.

*/

int 
openCommand(int i, char dummy[])
{
  int ierr;

  shm_addr->MODS.busy[i]=1;    // Set the HOST busy bit.

  if (OpenTTYPort(&shm_addr->MODS.commport[i]) < 0) {
    memset(dummy,0,sizeof(dummy));
    sprintf(dummy,"%s=OPENERR openCommand: IP:%s NOT found",
	    makeUpper(shm_addr->MODS.who[i]),
	    shm_addr->MODS.commport[i].Port);
    shm_addr->MODS.busy[i]=0;   // Clear the HOST busy bit.
    return CMD_ERR;
  }
  sprintf(dummy,"%s=OPENED IP:%s has been closed by the User",
	  makeUpper(shm_addr->MODS.who[i]),
	  shm_addr->MODS.commport[i].Port);
  shm_addr->MODS.busy[i]=0;   // Clear the HOST busy bit.

  return CMD_OK;
}

//---------------------------------------------------------------------------
//
// closeCommand - Close command port to a mechanism
//

/*!
  \brief Close a command port to a mechanism
  
  \param i - port of mechanism

  \return Returns CMD_OK on success, CMD_ERR on errors.

  Close a command port to the named MicroLYNX controller.

*/

int 
closeCommand(int i, char dummy[])
{
  shm_addr->MODS.busy[i]=1;   // Set HOST busy bit.
  CloseTTYPort(&shm_addr->MODS.commport[i]);
  sprintf(dummy,"%s=CLOSED IP:%s has been closed by the User",
	  makeUpper(shm_addr->MODS.who[i]),
	  shm_addr->MODS.commport[i].Port);
  shm_addr->MODS.busy[i]=0;   // Clear the HOST busy bit.
  return CMD_OK;
}

//---------------------------------------------------------------------------
//
// positionToShrMem() - send position to shared memory and return position
//                      to caller.
//

/*!
  \brief Send position to shared memory and return position to caller

  \param i index of the mechanism
  \param dummy reply
  \return position of the mechanism

  Sends the current position of the indicated mechanism to the correct
  address in device shared memory, and return the same value to
  the caller.

*/

double 
positionToShrMem(int i, char dummy[])
{

  memset(dummy,0,sizeof(dummy));

  shm_addr->MODS.busy[i]=1; // Hold the IP until finished

  if(WriteTTYPort(&shm_addr->MODS.commport[i],"PRINT POS\r")<0) {
    sprintf(dummy,"%s=TIMEOUT positionToShrMem cannot write to %s",
	    makeUpper(shm_addr->MODS.who[i]),
	    shm_addr->MODS.commport[i].Port);
    shm_addr->MODS.busy[i]=0;  // Clear the HOST busy bit.
    return CMD_ERR;
  }

  MilliSleep(200);
  if(ReadTTYPort(&shm_addr->MODS.commport[i],dummy,10L)<0) {
    sprintf(dummy,"%s=TIMEOUT positionToShrMem cannot read from %s",
	    makeUpper(shm_addr->MODS.who[i]),
	    shm_addr->MODS.commport[i].Port);
    shm_addr->MODS.busy[i]=0;  // Clear the HOST busy bit.
    return CMD_ERR;
  }

  rmcrlf(dummy,dummy);
    
  shm_addr->MODS.pos[i]=atof(&dummy[11]);      
  
  shm_addr->MODS.busy[i]=0;  // Clear the HOST busy bit.

  return (atof(&dummy[11]));
}

//---------------------------------------------------------------------------
//
// sendCommand - Send command to a mechanism
//

/*!
  \brief Send a command to a mechanism

  \param i - index of mechanism
  \param cmd - command string
  \param dummy string to contain reply

  \return Returns CMD_OK on success, CMD_ERR on errors.

  Sends a command to the named MicroLYNX controller.

*/

int 
sendCommand(int i, char cmd[], char dummy[])
{
  int ierr;
  char send[64];
  char dummy2[PAGE_SIZE];
  char modserr[512];

  strcpy(send,cmd);
  strcat(send,"\r"); // Add a '\r'. <CR> to satisfy MicroLynx controller

  shm_addr->MODS.busy[i]=1;    // Set the HOST busy bit.

  memset(dummy,0,sizeof(dummy)); // Clear the dummy and start again.

  if(WriteTTYPort(&shm_addr->MODS.commport[i],send)<0) {
    sprintf(dummy,"%s=TIMEOUT sendCommand cannot write to %s",
	    makeUpper(shm_addr->MODS.who[i]),
	    shm_addr->MODS.commport[i].Port);
    shm_addr->MODS.busy[i]=0;    // Clear the HOST busy bit.
    return CMD_ERR;
  }

  MilliSleep(20);
  if(ReadTTYPort(&shm_addr->MODS.commport[i],dummy,
		 shm_addr->MODS.timeout[i]+1L)<0) {
    sprintf(dummy,"%s=TIMEOUT sendCommand cannot read from %s",
	    makeUpper(shm_addr->MODS.who[i]),
	    shm_addr->MODS.commport[i].Port);
    shm_addr->MODS.busy[i]=0;   // Clear the HOST busy bit.
    return CMD_ERR;
  }

  if(strstr(dummy,"?")) { // check for a MicroLynx Controller ERROR '?'
    strcpy(dummy2,&dummy[strlen(send)+3]); // Save message if != standard
                                           // microLynx controller error.
    sprintf(modserr,"%s",dummy);
    if(WriteTTYPort(&shm_addr->MODS.commport[i],"PRINT ERROR\r")<0) {
      sprintf(dummy,"%s=TIMEOUT sendCommand cannot write to %s",
	      makeUpper(shm_addr->MODS.who[i]),
	      shm_addr->MODS.commport[i].Port);
      shm_addr->MODS.busy[i]=0;   // Clear the HOST busy bit.
      return CMD_ERR;
    }
    memset(dummy,0,sizeof(dummy)); // Clear dummy return
      
    MilliSleep(10);
    if(ReadTTYPort(&shm_addr->MODS.commport[i],dummy,10L)<0) {
      sprintf(dummy,"%s=TIMEOUT sendCommand cannot read from %s",
	      makeUpper(shm_addr->MODS.who[i]),
	      shm_addr->MODS.commport[i].Port);
      shm_addr->MODS.busy[i]=0;   // Clear the HOST busy bit.
      return CMD_ERR;
    }
    ierr = atoi(&dummy[13]); // Get error number

    if(ierr==0) { // Check to see if it was a MODS type error.
      GetArg(modserr,3,argbuf);
      ierr=atoi(argbuf);
    }

    checkForError(ierr,&dummy[0]); // message for this error number
    if(strstr(dummy,"0000")) {
      rmcrlf(dummy2,dummy2);
      memset(dummy,0,sizeof(dummy));
      sprintf(dummy,"%s",dummy2); 
      shm_addr->MODS.busy[i]=0;   // Clear the HOST busy bit.
      return CMD_ERR;
    }   

    shm_addr->MODS.busy[i]=0;   // Clear the HOST busy bit.
    return CMD_ERR;
  }

  rmcrlf(dummy,dummy);

  GetArg(dummy,2,argbuf);
  //sprintf(dummy2,"%s",&dummy[strlen(send)+3]);
  sprintf(dummy2,"%s",&dummy[strlen(send)+1]);
  memset(dummy,0,sizeof(dummy)); // Clear dummy return
  sprintf(dummy,"%s",dummy2);
  /* ********* */

  shm_addr->MODS.busy[i]=0;   // Clear the HOST busy bit.
  return (atoi(argbuf)); // CMD_OK;

}

//---------------------------------------------------------------------------
//
// sendMultiCommand - Send Multiple Commands to a mechanism
//

/*!
  \brief Send Multiple Commands to a mechanism

  \param i - index of mechanism
  \param cmd[] - command list
  \param cnt - number of commands sent
  \param dummy - string to contain reply

  \return Returns CMD_OK on success, CMD_ERR on errors.

  Sends Multiple Command to the named MicroLYNX controller.

  EXAMPLE for MODS: if you should ever need it.
  if(!strcasecmp(argbuf,"LIST")) { // ask for help
    char listdum[5][512];
    char listcmds[5][80];
    sprintf(&listcmds[0][0],"PRINT POS");
    sprintf(&listcmds[1][0],"PRINT ACCL");
    sprintf(&listcmds[2][0],"PRINT VM");
    sprintf(&listcmds[3][0],"PRINT TARGNUM");
    sprintf(&listcmds[4][0],"PRINT DICHNUM");
    ierr = sendMultiCommand(device,listcmds,5,listdum);
    sprintf(reply,"%s %s=%s",who_selected,who_selected,listdum[0]);
    for(int i=1;i<5;i++)
      sprintf(reply,"%s\n%s %s=%s",reply,who_selected,who_selected,listdum[i]);

    if(ierr!=0) return CMD_ERR;

    rawCommand(device,"PRINT POS",dummy);
    shm_addr->MODS.pos[device]=atof(dummy);

*/

int 
sendMultiCommand(int i, char cmdlist[][80], int cnt, char dumlist[][512])
{
  int ierr;
  int cmditem;
  char dummy[512];
  char send[64];
  char modserr[512];

  shm_addr->MODS.busy[i]=1;    // Set the HOST busy bit.

  for(cmditem=0;cmditem<cnt;cmditem++) {
    memset(send,0,sizeof(send));
    strcpy(send,cmdlist[cmditem]); // command
    strcat(send,"\r"); // Add a '\r'. <CR> to satisfy MicroLynx controller
    /* Send nth command */
    if(WriteTTYPort(&shm_addr->MODS.commport[i],send)<0) {
      sprintf(dummy,"%s=TIMEOUT sendMultiCommand cannot write to %s",
	      makeUpper(shm_addr->MODS.who[i]),
	      shm_addr->MODS.commport[i].Port);
      shm_addr->MODS.busy[i]=0;    // Clear the HOST busy bit.
      return CMD_ERR;
    }
    MilliSleep(200);
    if(ReadTTYPort(&shm_addr->MODS.commport[i],dumlist[cmditem],
		   shm_addr->MODS.timeout[i]+1L)<0) {
      sprintf(dummy,"%s=TIMEOUT sendMultiCommand cannot read from %s",
	      makeUpper(shm_addr->MODS.who[i]),
	      shm_addr->MODS.commport[i].Port);
      shm_addr->MODS.busy[i]=0;   // Clear the HOST busy bit.
      return CMD_ERR;
    }
    rmcrlf(dumlist[cmditem],dumlist[cmditem]);
    sprintf(dummy,"%s",&dumlist[cmditem][strlen(send)]+1);
    memset(dumlist[cmditem],0,sizeof(dumlist[cmditem]));
    sprintf(dumlist[cmditem],"%s=%s",makeUpper(shm_addr->MODS.who[i]),dummy);
  }

  shm_addr->MODS.busy[i]=0;   // Clear the HOST busy bit.
  return CMD_OK;

}

//---------------------------------------------------------------------------
//
// sendTwoCommand - Send 2 commands to a mechanism
//

/*!
  \brief Send a command to a mechanism

  \param i - index of mechanism
  \param cmd - 1st command string
  \param cmd2 - 2nd command string
  \param dummy string to contain reply

  \return Returns CMD_OK on success, CMD_ERR on errors.

  Sends a command to the named MicroLYNX controller.

*/

int 
sendTwoCommand(int i, char cmd[], char cmd2[], char dummy[])
{
  int ierr;
  char send[64];
  char send2[64];
  char temp[79];
  char dummy2[PAGE_SIZE];
  char modserr[512];

  memset(dummy2,0,sizeof(dummy2)); // Clear dummy2 copy

  strcpy(send,cmd); // 1st command
  strcat(send,"\r"); // Add a '\r'. <CR> to satisfy MicroLynx controller

  strcpy(send2,cmd2); // 2nd command
  strcat(send2,"\r"); // Add a '\r'. <CR> to satisfy MicroLynx controller

  shm_addr->MODS.busy[i]=1;    // Set the HOST busy bit.

  /* Send 1st command */

  if(WriteTTYPort(&shm_addr->MODS.commport[i],send)<0) {
    sprintf(dummy,"%s=TIMEOUT sendTwoCommand cannot write to %s",
	    makeUpper(shm_addr->MODS.who[i]),
	    shm_addr->MODS.commport[i].Port);
    shm_addr->MODS.busy[i]=0;    // Clear the HOST busy bit.
    return CMD_ERR;
  }

  MilliSleep(10);
  if(ReadTTYPort(&shm_addr->MODS.commport[i],temp,1L)<0) {
    sprintf(dummy,"%s=TIMEOUT sendTwoCommand cannot read from %s",
	    makeUpper(shm_addr->MODS.who[i]),
	    shm_addr->MODS.commport[i].Port);
    shm_addr->MODS.busy[i]=0;   // Clear the HOST busy bit.
    return CMD_ERR;
  }

  MilliSleep(50); // Wait 50msec then send the 2nd command
  /* Send 2nd command */

  memset(dummy,0,sizeof(dummy)); // Clear the dummy and start again.
  if(WriteTTYPort(&shm_addr->MODS.commport[i],send2)<0) {
    sprintf(dummy,"%s=TIMEOUT sendTwoCommand cannot write to %s",
	    makeUpper(shm_addr->MODS.who[i]),
	    shm_addr->MODS.commport[i].Port);
    shm_addr->MODS.busy[i]=0;    // Clear the HOST busy bit.
    return CMD_ERR;
  }

  memset(dummy,0,sizeof(dummy)); // Clear the dummy and start again.

  MilliSleep(50);
  if(ReadTTYPort(&shm_addr->MODS.commport[i],dummy,
		 shm_addr->MODS.timeout[i]+1L)<0) {
    sprintf(dummy,"%s=TIMEOUT sendTwoCommand cannot read from %s",
	    makeUpper(shm_addr->MODS.who[i]),
	    shm_addr->MODS.commport[i].Port);
    shm_addr->MODS.busy[i]=0;   // Clear the HOST busy bit.
    return CMD_ERR;
  }

  if(strstr(dummy,"?")) { // check for a MicroLynx Controller ERROR '?'
    if(WriteTTYPort(&shm_addr->MODS.commport[i],"PRINT ERROR\r")<0) {
      sprintf(dummy,"%s=TIMEOUT sendTwoCommand cannot write to %s",
	      makeUpper(shm_addr->MODS.who[i]),
	      shm_addr->MODS.commport[i].Port);
      shm_addr->MODS.busy[i]=0;   // Clear the HOST busy bit.
      return CMD_ERR;
    }
    memset(dummy,0,sizeof(dummy)); // Clear dummy return
      
    MilliSleep(10);
    if(ReadTTYPort(&shm_addr->MODS.commport[i],dummy,10L)<0) {
      sprintf(dummy,"%s=TIMEOUT sendTwoCommand cannot read from %s",
	      makeUpper(shm_addr->MODS.who[i]),
	      shm_addr->MODS.commport[i].Port);
      shm_addr->MODS.busy[i]=0;   // Clear the HOST busy bit.
      return CMD_ERR;
    }

    ierr = atoi(&dummy[13]); // Get error number
  }

  rmcrlf(dummy,dummy);
  /* 
   * Check to see if the MicroLynx has a mechanism ERROR
   * very, very, very much a kludge but what can I say I'll fix it later.
   */
  GetArg(dummy,2,argbuf); // Get the error if their is one
  sprintf(dummy2,"%s",&dummy[strlen(send2)+3]); // Get the message only
  //  sprintf(dummy2,"%s",&dummy[strlen(send)+1]);
  memset(dummy,0,sizeof(dummy)); // Clear dummy return
  sprintf(dummy,"%s",makeUpper(shm_addr->MODS.who[i]),dummy2);
  /* ********* */

  shm_addr->MODS.busy[i]=0;   // Clear the HOST busy bit.
  return (atoi(argbuf)); // CMD_OK;

}

//---------------------------------------------------------------------------
//
// rawCommand - Send a raw microLynx command to a mechanism
//

/*!
  \brief Send a raw microLynx command to a mechanism

  \param i - index of mechanism
  \param cmd - command string
  \param dummy string to contain reply

  \return Returns CMD_OK on success, CMD_ERR on errors.

  Sends a raw microLynx command to the named MicroLYNX controller.

*/
int 
rawCommand(int i, char cmd[], char dummy[])
{
  int ierr;
  char send[64];
  //char dummy2[512];
  char dummy2[PAGE_SIZE];
  char modserr[512];

  strcpy(send,cmd);
  strcat(send,"\r"); // Add a '\r'. <CR> to satisfy MicroLynx controller
  shm_addr->MODS.busy[i]=1;    // Set the HOST busy bit.
  memset(dummy,0,sizeof(dummy)); // Clear the dummy and start again.

  if(WriteTTYPort(&shm_addr->MODS.commport[i],send)<0) {
    sprintf(dummy,"%s=TIMEOUT rawCommand cannot write to %s",
	    makeUpper(shm_addr->MODS.who[i]),
	    shm_addr->MODS.commport[i].Port);
    shm_addr->MODS.busy[i]=0;    // Clear the HOST busy bit.
    return CMD_ERR;
  }

  MilliSleep(100);
  if(ierr=ReadTTYPort(&shm_addr->MODS.commport[i],dummy,60L)<0) {
    sprintf(dummy,"%s=TIMEOUT rawCommand cannot read from %s",
	    makeUpper(shm_addr->MODS.who[i]),
	    shm_addr->MODS.commport[i].Port);
    shm_addr->MODS.busy[i]=0;   // Clear the HOST busy bit.
    return CMD_ERR;
  }

  if(strstr(dummy,"?")) { // check for a MicroLynx Controller ERROR '?'
    sprintf(modserr,"%s",dummy);
    if(WriteTTYPort(&shm_addr->MODS.commport[i],"PRINT ERROR\r")<0) {
      sprintf(dummy,"%s=TIMEOUT rawCommand cannot write to %s",
	      makeUpper(shm_addr->MODS.who[i]),
	      shm_addr->MODS.commport[i].Port);
      shm_addr->MODS.busy[i]=0;   // Clear the HOST busy bit.
      return CMD_ERR;
    }
    memset(dummy,0,sizeof(dummy)); // Clear dummy return
    
    MilliSleep(10);
    if(ReadTTYPort(&shm_addr->MODS.commport[i],dummy,10L)<0) {
      sprintf(dummy,"%s=TIMEOUT rawCommand cannot read from %s",
	      makeUpper(shm_addr->MODS.who[i]),
	      shm_addr->MODS.commport[i].Port);
      shm_addr->MODS.busy[i]=0;   // Clear the HOST busy bit.
      return CMD_ERR;
    }

    ierr = atoi(&dummy[13]); // Get error number

    if(ierr==0) { // Check to see if it was a MODS type error.
      GetArg(modserr,3,argbuf);
      ierr=atoi(argbuf);
    }
    checkForError(ierr,&dummy[0]); // message for this error number
    shm_addr->MODS.busy[i]=0;   // Clear the HOST busy bit.
    return CMD_ERR;
  }

  dummy[512]='\0';
  rmcrlf(dummy,dummy);
  sprintf(dummy2,"%s",&dummy[strlen(send)+1]);
  memset(dummy,0,sizeof(dummy)); // Clear dummy return
  sprintf(dummy,"%s",dummy2); // Return the response ONLY

  shm_addr->MODS.busy[i]=0;   // Clear the HOST busy bit.
  return CMD_OK;

}

//---------------------------------------------------------------------------
//
// rawCommandOnly - Send a raw microLynx command to a mechanism
//

/*!
  \brief Send a raw microLynx command to a mechanism with no reply

  \param i - index of mechanism
  \param cmd - command string
  \param return - 0=successful or 1=failed

  \return Returns CMD_OK on success, CMD_ERR on errors.

  Sends a raw microLynx command with *NO* reply required

*/

int 
rawCommandOnly(int i, char cmd[])
{
  int ierr;
  char send[64];
  //  char dummy[512];
  char dummy[PAGE_SIZE];

  strcpy(send,cmd);
  strcat(send,"\r"); // Add a '\r'. <CR> to satisfy MicroLynx controller
  memset(dummy,0,sizeof(dummy)); // Clear the dummy and start again.

  if(WriteTTYPort(&shm_addr->MODS.commport[i],send)<0) {
    sprintf(dummy,"%s=TIMEOUT rawCommandOnly cannot write %s",
	    makeUpper(shm_addr->MODS.who[i]),
	    shm_addr->MODS.commport[i].Port);
    return CMD_ERR;
  }

  MilliSleep(10);
  ReadTTYPort(&shm_addr->MODS.commport[i],dummy,3L);
  memset(dummy,0,sizeof(dummy)); // Clear the dummy and start again.
  return CMD_OK;
}

//---------------------------------------------------------------------------
//
// mlcCheckBits() - check limit bits and return them to caller.
//

/*!
  \brief Check mechanism limit bits

  \param i index of the mechanism
  \param dummy string to contain reply
  \return The value of the limit bits

  Queries the named mechanism and returns the current set of
  limit bits.
  <pre>
    Bit 26 is the break
    Bits 25 - 23 status
    Bit 22 CCW bit
    Bit 21 CW bit
  </pre>

*/

int 
mlcCheckBits(int i, char dummy[])
{
  int ierr;

  memset(dummy,0,sizeof(dummy));

  shm_addr->MODS.busy[i]=1;    // Set the HOST busy bit.

  memset(dummy,0,sizeof(dummy)); // Clear the dummy and start again.
    
  if(WriteTTYPort(&shm_addr->MODS.commport[i],IO20)<0) {
    sprintf(dummy,"%s=TIMEOUT mlcCheckBits cannot write to %s",
	    makeUpper(shm_addr->MODS.who[i]),
	    shm_addr->MODS.commport[i].Port);
    shm_addr->MODS.busy[i]=0;   // Clear the HOST busy bit.
    return CMD_ERR;
  }
  
  MilliSleep(10);
  if(ReadTTYPort(&shm_addr->MODS.commport[i],dummy,10L)<0) {
    sprintf(dummy,"%s=TIMEOUT mlcCheckBits cannot read from %s",
	    makeUpper(shm_addr->MODS.who[i]),
	    shm_addr->MODS.commport[i].Port);
    shm_addr->MODS.busy[i]=0;   // Clear the HOST busy bit.
    return CMD_ERR;
  }

  ierr=atoi(&dummy[strlen(IO20)+1]);

  if((ierr&0x3) == 3) {
    sprintf(dummy,"%s=FAULT cable disconnected or sensor fault[%d]",
	    makeUpper(shm_addr->MODS.who[i]),ierr);
    shm_addr->MODS.busy[i]=0;    // Clear the HOST busy bit.
    return CMD_ERR;
  }

  shm_addr->MODS.busy[i]=0;    // Clear the HOST busy bit.
  return(ierr);
}

//---------------------------------------------------------------------------
//
// mlcStopMechanism(mechanism) - Stop the operation
//

/*!
  \brief Stop the current operation

  \param i index of the mechanism
  \return The value of success for now (0)

  Stops the named mechanism current operation and returns

  <pre>

  </pre>

*/

int 
mlcStopMechanism(int i, char dummy[])
{
  int ierr;
  char *esc;

  shm_addr->MODS.busy[i]=0;    // Clear the HOST busy bit.

  sprintf(esc,"%c",27);
  WriteTTYPort(&shm_addr->MODS.commport[i],esc);

  MilliSleep(100);
  ierr = ReadTTYPort(&shm_addr->MODS.commport[i],dummy,5L);

  memset(dummy,0,sizeof(dummy)); // Clear the dummy and start again.
  sprintf(dummy,"%s=STOPPED",makeUpper(shm_addr->MODS.who[i]));

  return(0);
}

//---------------------------------------------------------------------------
//
// getMaskNumber(mechanism,reply) - Get Mask ID
//

/*!
  \brief Get the mask ID from the mechanism IO 30 bits

  \param i index of the mechanism
  \param dummy reply
  \return Mask Number or error

  Interogates mechanism IO 30 bitmask for Mask Number

  <pre>
    Bit 36-31
  </pre>

*/

int 
getMaskNumber(int i,char dummy[])
{
  int ierr;
  int masknumber;

  ierr=sendCommand(i,"MASK_ID",dummy); // Get mask current ID
  if(ierr!=0)  return CMD_ERR;

  masknumber=atoi(dummy);
  return(masknumber);
}

//---------------------------------------------------------------------------
//
// mlcActivePorts(reply) - Get Mask ID
//

/*!
  \brief List active Comtrol to MicroLynx ports

  \param reply reply message
  \return 0 or error

  List all Comtrol to MicroLynx Ports.

  <pre>
  </pre>

*/

int 
mlcActivePorts(char dummy[])
{
  int i;
  int pnum;
  int len;
  char temp[80];

  /* List active comm port(s) */
  sprintf(dummy,"%s='%s' ",makeUpper(shm_addr->MODS.who[0]),shm_addr->MODS.commport[0].Port);
  for(i=1;i<MAX_ML;i++) {
    if(shm_addr->MODS.host[i]) {
      sprintf(dummy,"%s %s='%s' ",dummy,makeUpper(shm_addr->MODS.who[i]),shm_addr->MODS.commport[i].Port);
    }
  }
  /**/
  return 0;
}

//---------------------------------------------------------------------------
//
// mlcWhoAmI(request,mlcname) - check names
//

/*!
  \brief Compare names

  \param request request mechanism name
  \param mlcname microLynx name
  \return value 1=error, 0=ok

  Compare requested name with microLynx name. Give an error if
  names do not match.

  <pre>
  </pre>

*/

int 
//mlcWhoAmI(int device, char who[], char mechanism_name[],char dummy[])
mlcWhoAmI(int device,char mechanism_name[],char dummy[])
{
  char dummy2[PAGE_SIZE];

  memset(dummy2,0,sizeof(dummy2));
  rawCommand(device,"WHO",dummy2); // Names request must match name in MLC

  if(atoi(dummy2)>0) {
    sprintf(dummy,"MLCERR=FAULT Microlynx Controller ML%d not preloaded you must use an engineering software program",device);
    return CMD_ERR;
  }

  if(!strstr(shm_addr->MODS.who[device],dummy2)) {
    sprintf(dummy,"%s MLCERR=FAULT requested name '%s' does not match microLynx Controller '%s'",makeUpper(mechanism_name),mechanism_name, dummy2);
    return CMD_ERR;
  }
  memset(dummy,0,sizeof(dummy));

  return 0;
}

//---------------------------------------------------------------------------
//
// getMechanismID(dummy,reply) - Get the mechanism ID from Shared Memory
//

/*!
  \brief Get the mechanism ID from Shared Memory

  \param dummy mechanism
  \param reply reply message
  \return mechanismID  or error

  Get Mechanism ID

  <pre>
  </pre>

*/

int 
getMechanismID(char mechanism_name[], char dummy[])
{
  int dev;
  int len;
  char dummy2[PAGE_SIZE];

  memset(dummy2,0,sizeof(dummy2));

  for(dev=0;
      !strstr(mechanism_name,shm_addr->MODS.who[dev]) && dev<=MAX_ML;
      dev++);

  if(!shm_addr->MODS.host[dev]) {
    sprintf(dummy,"MLCERR=NOCOMM No IP:SOCKET assigned");
    return CMD_ERR;
  }


  if(strstr(mechanism_name,"agwx")) return dev;
  else if(strstr(mechanism_name,"agwy")) return dev;
  else if(strstr(mechanism_name,"agwfoc")) return dev;
  else if(strstr(mechanism_name,"agwfilt")) return dev;

  if(dev<0 || dev>=MAX_ML) {
    sprintf(dummy,"%s=FAULT No mechanism name available",makeUpper(mechanism_name));
    return CMD_ERR;
  }

  if(mlcWhoAmI(dev,mechanism_name,dummy)) {
    return CMD_ERR;
  }

  return dev;
}

//---------------------------------------------------------------------------
//
// checkPower(device,who) - check mechanism power
//

/*!
  \brief Check the mechanism power

  \param device mechanism id
  \param who    mechanism name
  \return power 1=recycled, 0=ok or error

  check mechanism power

  <pre>
  </pre>

*/

int 
checkPower(int device, char dummy[])
{
  int ierr;

  /* Check the Power Failure variable PWRFAIL */
  sendCommand(device,"PRINT PWRFAIL",dummy);
  ierr=atoi(dummy);
  if(atoi(dummy))
      sprintf(dummy,"%s=PWRFLR mechanism has been power cycled and must be reset to initialize",makeUpper(shm_addr->MODS.who[device]));

  return (ierr);

}

//---------------------------------------------------------------------------
//
// mlcBusy(device,who) - check busy
//

/*!
  \brief Check to see if the mechanism is busy

  \param device mechanism id
  \param who    mechanism name
  \return value 1=error, 0=ok

  check if mechanism busy

  <pre>
  </pre>

*/

int 
mlcBusy(int device, char dummy[])
{
  int ierr;

  if(shm_addr->MODS.busy[device]==1) {
    sprintf(dummy,"%s=BUSY",makeUpper(shm_addr->MODS.who[device]));
    return 1;
  }
  return 0;
}

//---------------------------------------------------------------------------
//
// mlcTechCmd(mlccmd,who,dummy) - check mechanism power
//

/*!
  \brief Check the mechanism power

  \param mlccmd command sent to MicroLynx controller
  \param who    mechanism name
  \param dummy  return string informantion
  \return power 1=recycled, 0=ok or error

  check mechanism power

  <pre>
  </pre>

*/

int 
mlcTechCmd(int device, char args[], char who[], char dummy[])
{
  int i,j;
  int ierr;
  int param_cnt;
  int var_cnt;
  int cnt;
  int len,tempoLen;
  char cmd_instruction[PAGE_SIZE];
  char cmd2[PAGE_SIZE]; // to remove any commas
  char mlccmd[79];
  char dummy2[PAGE_SIZE];
  char dummy3[PAGE_SIZE];
  char tempo[200][24];
  char tempo2[24];
  char tempo3[24]; // save the IO parameter label

  memset(dummy2,0,sizeof(dummy2));
  memset(dummy3,0,sizeof(dummy3));
  for(j=0;j<100;j++) memset(tempo[j],0,sizeof(tempo[j]));

  memset(tempo2,0,sizeof(tempo2));
  memset(tempo3,0,sizeof(tempo3));

  sprintf(cmd2,"%s",&args[2]); // Get MicroLynx instruction

  if (strstr(args,"LIST") || strstr(args,"VARS")) {
    sprintf(dummy,"%s MicroLynx Command <%s> not allowed",who,cmd_instruction); 
    return CMD_ERR;
  }

  cmd2[strlen(cmd2)]='\0';
  GetArg(cmd2,1,mlccmd);

  if(strstr(cmd2,"\"")) {
    memset(cmd_instruction,0,sizeof(cmd_instruction));
    for(i=1,j=0;cmd2[i]!='\0';i++) cmd_instruction[j++]=cmd2[i];
    
    len=strlen(cmd_instruction);
    if(cmd_instruction[len-1]=='"') {
      cmd_instruction[len-1]='\0';
    }

    ierr=rawCommand(device,cmd_instruction,dummy2); // Send a Raw command
    if(ierr!=0) {
      sprintf(dummy,"%s",dummy2);
      return CMD_ERR;
    }

    if(strlen(dummy2)<=1 && !strstr(cmd_instruction,"=")) 
       sprintf(dummy,"%s MLCOUT=UNKNOWN mangled command (example: mechanism m#\"print pos accl io 21 \"or\" mechanism m#\"print pos,vm,io 21,io 25\"  ", who);
    else if(strstr(cmd_instruction,"=")); 
    else sprintf(dummy,"%s MLCOUT=%s",who,dummy2); // data fill
    return CMD_OK;
  }

  if(!strcasecmp(mlccmd,"PRINT")) {
    len=strlen(cmd2);
    memset(cmd_instruction,0,sizeof(cmd_instruction));
    for(i=0,j=0;cmd2[i]!='\0';i++) { // Remove any commas that were sent
      if (cmd2[i]==',')
	if(cmd2[i+1]==' ');
	else
	  cmd_instruction[j++]=' ';
      else 
	cmd_instruction[j++]=cmd2[i];
    }
    i=0; len=strlen(cmd_instruction);
    for(param_cnt=0,var_cnt=1;param_cnt<len;param_cnt++) {
      if (cmd_instruction[param_cnt]==' ') {
	var_cnt++;
	GetArg(cmd_instruction,var_cnt,tempo[i]); // get the next parameter
	if(!strcasecmp(tempo[i],"IO")) {
	
	  /* The request was for an IO bit status */
	  strcpy(tempo3,tempo[i]);
	  var_cnt++;
	  len-=3;

	  GetArg(cmd_instruction,var_cnt,tempo[i]); // io parameter
	  if(atoi(tempo[i])<=0) {
	    sprintf(dummy,"%s MLCERR=FAULT Invalid IO request",who);
	    return CMD_ERR;
	  }
	  sprintf(tempo2,"PRINT IO %s",tempo[i]); // add request

	} else {

	  /* The request was for a parameter status */
	    sprintf(tempo2,"PRINT %s",tempo[i]); // add request
	}

	tempoLen=strlen(tempo[i]);
	if(tempoLen!=0)
	  ierr=rawCommand(device,tempo2,dummy2); // Send a Raw command

	//if(ierr<=-1) {
	if(ierr!=0) {
	  sprintf(dummy,"%s MLCERR=%s",who,dummy2); // fill the return
	  return CMD_ERR;
	}

	/* 
	 * Stack the returns into dummy3 until 
	 * all requests have been sent and received 
	 */
	if(!strcasecmp(tempo3,"IO")) {
	  if(tempoLen!=0)
	    //	    sprintf(dummy3,"%s IO%s=%s",dummy3,tempo[i],&dummy2[strlen(who)+1]); // data fill
	    sprintf(dummy3,"%s IO%s=%s",dummy3,tempo[i],dummy2); // data fill
	  memset(tempo3,0,sizeof(tempo3));
	} else 
	  if(tempoLen!=0)
	    //	    sprintf(dummy3,"%s %s=%s",dummy3,tempo[i],&dummy2[strlen(who)+1]); // data fill dummy2); // data fill
	    sprintf(dummy3,"%s %s=%s",dummy3,tempo[i],dummy2); // data fill dummy2); // data fill
	i++;
      }
    }
    sprintf(dummy,"%s%s",who,dummy3); // output data.
    memset(cmd_instruction,0,sizeof(cmd_instruction));
    return CMD_OK;
  } else {
    ierr=rawCommand(device,cmd2,dummy2); // Send a Raw command
    if(ierr!=0) {
      sprintf(dummy,"%s MLCERR=%s",who,dummy2); // fill the return
      return CMD_ERR;
    }
  }
  sprintf(dummy,"%s %s=%s",who,mlccmd,dummy2);

  memset(cmd_instruction,0,sizeof(cmd_instruction));
  return CMD_OK;
}

//---------------------------------------------------------------------------
//
// mlcClear - Clear MLC NVM
//

/*!
  \brief Clear the NVM on the MicroLynx Controller

  \param i - index of mechanism
  \param dummy - confirm
  \return Returns CMD_OK on success, CMD_ERR on errors.

  Clear MicroLynx Controller NV Memory

*/

int 
mlcClear(int i,char dummy[])
{

  if(WriteTTYPort(&shm_addr->MODS.commport[i],"IP\r")<0)
    return CMD_ERR;
  if(ReadTTYPort(&shm_addr->MODS.commport[i],dummy,10L)<0) {
    sprintf(dummy,"%s=TIMEOUT mlcClear cannot read from %s",
	    shm_addr->MODS.who[i],
	    shm_addr->MODS.commport[i].Port);
    shm_addr->MODS.busy[i]=0;  // Clear the HOST busy bit.
    return CMD_ERR;
  }

  MilliSleep(100);
  if(WriteTTYPort(&shm_addr->MODS.commport[i],"DVF\r")<0)
    return CMD_ERR;

  if(ReadTTYPort(&shm_addr->MODS.commport[i],dummy,10L)<0) {
    sprintf(dummy,"%s=TIMEOUT mlcClear cannot read from %s",
	    shm_addr->MODS.who[i],
	    shm_addr->MODS.commport[i].Port);
    shm_addr->MODS.busy[i]=0;  // Clear the HOST busy bit.
    return CMD_ERR;
  }

  MilliSleep(100);
  if(WriteTTYPort(&shm_addr->MODS.commport[i],"CP 1,1\r")<0)
    return CMD_ERR;
  if(ReadTTYPort(&shm_addr->MODS.commport[i],dummy,30L)<0) {
    sprintf(dummy,"%s=TIMEOUT mlcClear cannot read from %s",
	    shm_addr->MODS.who[i],
	    shm_addr->MODS.commport[i].Port);
    shm_addr->MODS.busy[i]=0;  // Clear the HOST busy bit.
    return CMD_ERR;
  }

  MilliSleep(100);
  if(WriteTTYPort(&shm_addr->MODS.commport[i],"SAVE\r")<0)
    return CMD_ERR;
  if(ReadTTYPort(&shm_addr->MODS.commport[i],dummy,10L)<0) {
    sprintf(dummy,"%s=TIMEOUT mlcClear cannot read from %s",
	    shm_addr->MODS.who[i],
	    shm_addr->MODS.commport[i].Port);
    shm_addr->MODS.busy[i]=0;  // Clear the HOST busy bit.
    return CMD_ERR;
  }
  memset(dummy,0,sizeof(dummy));
  return CMD_OK;
}


//---------------------------------------------------------------------------
//
// mlcSendGet - Query or Send command to a mechanism
//

/*!
  \brief Query or Send a command to a mechanism

  \param who - mechanism name
  \param mlcCmdStr - command string
  \param dummy string reply

  \return Returns CMD_OK on success, CMD_ERR on errors.

  Sends a command to the named MicroLYNX controller.

*/
int 
mlcSendGet(int mlcUnit, char cmd[], char dummy[])
{
  unsigned short mlcPort;
  char mlcCmdStr[64];
  int mlcStrLen; 
  string mlcAddress;
  char charAddress[64];

  memset(mlcCmdStr,0,sizeof(mlcCmdStr));
  memset(dummy,0,sizeof(dummy));

  strcpy(mlcCmdStr,cmd);
  strcat(mlcCmdStr,"\r"); // Add a '\r'. <CR> to satisfy MicroLynx controller
  mlcStrLen = strlen(mlcCmdStr);       // input command length
  
  /* Search for the mechanism unit index */
  sscanf(shm_addr->MODS.commport[mlcUnit].Port,"%[^:]:%d",
	 charAddress, &mlcPort);
  mlcAddress=charAddress;

  shm_addr->MODS.busy[mlcUnit]=1;     // set HOST busy.

  try {

    TCPSocket sock(mlcAddress, mlcPort);  // establish the connection
    sock.send(mlcCmdStr, mlcStrLen);      // send the command
    char mlcBuffer[RCVBUFSIZE + 1];       // buffer for return string + \0
    int bytesReceived = 0;                // bytes read on each recv()
    int totalBytesReceived = 0;           // total bytes read

    /* Receive up to the buffer size bytes from the sender */
    if ((bytesReceived = (sock.recv(mlcBuffer, RCVBUFSIZE))) <= 0) {
      sprintf(dummy,"mlcSendGet Unable to read");
      shm_addr->MODS.busy[mlcUnit]=0;   // clear HOST busy.
      TCPSocket();                         // Close socket connection
      return CMD_ERR;
    }
    rmcrlf(mlcBuffer,mlcBuffer);
    sprintf(dummy,"%s",mlcBuffer);       // return mlcBuffer in dummy

    shm_addr->MODS.busy[mlcUnit]=0;     // Clear HOST busy.

    /*  Destructor closes the socket */
  } catch(SocketException &e) {

    sprintf(dummy,"%s",e.what());        // return error in dummy
    shm_addr->MODS.busy[mlcUnit]=0;     // clear HOST busy.
    TCPSocket();                         // Close socket connection
    return CMD_ERR;
  }

  TCPSocket();                           // Close socket connection
  shm_addr->MODS.busy[mlcUnit]=0;       // clear HOST busy.

  return CMD_OK;

}

//---------------------------------------------------------------------------
//
// digit - digitize it request
//

/*!
  \param c - char to convert

*/
int digit(char c)
{
  if (c>='a') c = toupper(c);
  return ((c>='0' && c<='9') ? c - '0':
          (c>='A' && c<='F') ? c - 'A' + 10:100);
}

//---------------------------------------------------------------------------
//
// atobase - from ascii to base
//

/*!
  \brief hex, octal, hex, octal, hex what the heck is this 
  a base convert, what kind of god would allow this!!!!

  \param s - string to convert
  \param radix - base to convert to 2,10,16..etc

  \return Returns convertion.

  Sends a command to the named MicroLYNX controller.
*/
int atobase(char *s, int radix)
{
  int sum, c; 
  int pos=0;
  
  if ((sum=digit(s[pos++]))>=radix) {
    return 2;
  }
  while (c=s[pos]) {
    if ((c=digit(c))>=radix) break;
    sum = sum * radix + c;
    ++pos;
  }
  return(sum);
}

//---------------------------------------------------------------------------
//
// mlcBitsBase10(i,cmd,dummy) - Convert bits to base 10 and return.
//

/*!
  \brief Check mechanism IO bits

  \param i index of the mechanism
  \param cmd IO bit string to convert
  \param dummy string to contain reply if error

  \return The value of the bits in base 10

  Queries the named mechanism for bit value

  <pre>
    Bits 1 - 6 IO 20, and IO 30
  </pre>

*/

int 
mlcBitsBase10(int i, char cmd [],char dummy[])
{
  int ierr;

  memset(dummy,0,sizeof(dummy)); // Clear dummy
  ierr=rawCommand(i,cmd,dummy);

  if(ierr!=0) return CMD_ERR;
  return(atobase(dummy,2));
}

//---------------------------------------------------------------------------
//
// mlcLimitBits(i,who,dummy) - Check limit bits
//

/*!
  \brief Check mechanism IO bits

  \param i index of the mechanism
  \param who What mechanism
  \param dummy string to contain reply if error

  \return 0=ok, -1=error

  Queries the named mechanism for bit value

  <pre>
    Bits IO 21(CW), and IO 22(CCW)
  </pre>

*/

int 
mlcLimitBits(int i, char dummy[])
{
  memset(dummy,0,sizeof(dummy));

  switch(mlcBitsBase10(i,"PRINT IO 21,IO 22",dummy)) {
  case 0: 
    sprintf(dummy,"%s=OFF CW and CCW are de-asserted",makeUpper(shm_addr->MODS.who[i]));
    break;
  case 1: 
    sprintf(dummy,"%s=CCW Limit is asserted CW de-asserted",makeUpper(shm_addr->MODS.who[i]));
    break;
  case 2: 
    sprintf(dummy,"%s=CW Limit asserted CCW de-asserted",makeUpper(shm_addr->MODS.who[i]));
    break;
  case 3: 
    sprintf(dummy,"%s=CWCCW CW and CCW are asserted",makeUpper(shm_addr->MODS.who[i]));

    break;
  default:
    sprintf(dummy,"%s RDBITS=%s",makeUpper(shm_addr->MODS.who[i]),dummy);
    return CMD_ERR;
  }
  return CMD_OK;
}

//---------------------------------------------------------------------------
//
// mlcReset(i,who,what,how,dummy) - Reset or seek the last know position
//

/*!
  \brief Seek the last known mechanism position

  \param i index of the mechanism
  \param who mechanism ID name
  \param what Bi-State=0, Linear=1, Indexed=2
  \param how command initializing or homing
  \param dummy string contains reply

  \return 0=ok, -1=error

  Instructs the mechanism to seek its last known safe(usually home)
  position, reset basic mechanism parameters, and clear fault
  condition flags

  <pre>
  Mechanisms:
  \param 1.) Bi-State - CW IO 21 or home position
  \param                (rare cases CCW IO 22 is home)
  \param 2.) Linear - Declared safe in configuration file 
  \param              loaded into Shared Memory
  \param 3.) Index - IO 20 or IO 30 bits for last position
  </pre>

*/

int 
mlcReset(int i, char who [], int what, char how[], char dummy[])
{
  int ierr;
  char mechanism_name[24];
  char temp[512];

  memset(dummy,0,sizeof(dummy));
  memset(temp,0,sizeof(temp));

  sprintf(mechanism_name,"%s",shm_addr->MODS.who[i]); // Get mechanisms name

  if(what==0) { // Bi-State
    ierr = sendCommand(i,how,temp);
    if(ierr!=0) {
      sprintf(dummy,"%s %s=%s", 
	      who,mechanism_name,temp);
      return CMD_ERR;
    }

    sprintf(dummy,"%s %s=%s Reset Successful", 
	    who,mechanism_name,temp);

    shm_addr->MODS.pos[i]=positionToShrMem(i,temp);
    
  } else if(what==1) { // Linear
    sprintf(dummy,"%s %s=Not Yet!",who,mechanism_name);    

  } else if(what==2) { // Indexed
    sprintf(dummy,"%s %s=Not Yet!",who,mechanism_name);
  }

  return CMD_OK;
}

//---------------------------------------------------------------------------
//
// mlcStep(i,who,what,valStr,dummy) - Reset or seek the last know position
//

/*!
  \brief Seek the last known mechanism position

  \param i index of the mechanism
  \param who mechanism ID name
  \param what Linear=1, Indexed=2
  \param val +/- relative move in revs
  \param dummy string that contains reply

  \return 0=ok, -1=error

  Instructs the mechanism to move +/-n position relative to 
  current position, Indexed mechanisms move full positions,
  linear move that many units (n=rev, 1rev=200steps)

  <pre>
  Mechanisms:
  \param 1.) Bi-State - +/- Relative move in units
  \param 2.) Linear - +/- Relative move in units
  \param 3.) Index - move in indexed units
  </pre>

*/

int 
mlcStep(int i, char who [], int what, float val, char dummy[])
{
  int ierr;
  char valStr[10];
  char mechanism_name[24];
  char temp[512];

  val = ((val/shm_addr->MODS.convf[i])); // convert physical units to revs
  sprintf(valStr,"%f",val); // convert the float step value to a string 

  memset(dummy,0,sizeof(dummy));
  memset(temp,0,sizeof(temp));

  sprintf(mechanism_name,"%s",shm_addr->MODS.who[i]); // Get mechanisms name

  if(what==0 || what==1) { // Bi-State, and Linear mechanisms
    sprintf(temp,"MOVR %s",valStr);

    rawCommand(i,temp,dummy);
    shm_addr->MODS.pos[i]=positionToShrMem(i,temp);
    sprintf(dummy,"%s %s=%f",
	    who,mechanism_name, shm_addr->MODS.pos[i]);

  } else if(what==2) { // Indexed
    ierr = sendTwoCommand(i,"TARGNUM=",valStr,temp);
    ierr = sendCommand(i,"BEGIN",temp);

    shm_addr->MODS.pos[i]=positionToShrMem(i,temp);

    if(ierr!=0) {
      sprintf(dummy,"%s %s=%f", 
	      who,mechanism_name,shm_addr->MODS.pos[i]);
      return CMD_ERR;
    }

    sprintf(dummy,"%s %s=%f", 
	    who,mechanism_name,shm_addr->MODS.pos[i]);
  }

  return CMD_OK;
}
