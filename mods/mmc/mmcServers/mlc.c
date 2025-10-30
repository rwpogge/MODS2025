#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <vector>
#include <cstdlib>            // For atoi()

#include "ISLSocket.h"        // For Socket and SocketException
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
// makeLower() - return all characters in a string in lowercase
//
 
/*!
  \brief Return lowercase string
 
  \param str2 string remains unchanged.
  \param str  returned as lowercase string, 
 
  Helper utility will return requested string in lowercase, and leave
  the requested string unchanged.
 
*/
 
char* makeLower(char str2[])
{
  int i;
  char str[strlen(str2)+1];

  strcpy(str,str2);

  for(i=0;i<strlen(str);i++)
    if(str[i] >= 'A' && str[i] <= 'Z')
      str[i] = tolower(str[i]);

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

  memset(dummy,0,sizeof(dummy));
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
  memset(dummy,0,sizeof(dummy));

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
    
  MilliSleep(10);
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
  int ierr=0;
  char send[64];
  char dummy2[PAGE_SIZE];
  char modserr[512];

  strcpy(send,cmd);
  strcat(send,"\r"); // Add a '\r'. <CR> to satisfy MicroLynx controller

  shm_addr->MODS.busy[i]=1;    // Set the HOST busy bit.

  memset(dummy,0,sizeof(dummy)); // Clear the dummy and start again.
  memset(dummy2,0,sizeof(dummy2)); // Clear the dummy and start again.

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

  if(strstr(dummy,"?")) {  // check for a MicroLynx Controller ERROR '?'
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

    if ( ierr == 0 ) { // Check to see if it was a MODS type error.
      GetArg(modserr,3,argbuf);
      ierr=atoi(argbuf);
    }

    checkForError(ierr, &dummy[0]); // message for this error number
    if ( strstr(dummy, "0000") ) {
      rmcrlf(dummy2,dummy2);
      memset(dummy,0,sizeof(dummy));
      sprintf(dummy,"%s",dummy2); 
      shm_addr->MODS.busy[i] = 0;   // Clear the HOST busy bit.
      return CMD_ERR;
    }   

    shm_addr->MODS.busy[i]=0;   // Clear the HOST busy bit.
    return CMD_ERR;
  }

  rmcrlf(dummy, dummy);

  sprintf(dummy2, "%s", &dummy[strlen(send)+1]);
  memset(dummy, 0, sizeof(dummy)); // Clear dummy return
  sprintf(dummy, "%s", dummy2);
  /* ********* */

  shm_addr->MODS.busy[i]=0;   // Clear the HOST busy bit.
  return CMD_OK;

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
  memset(dummy,0,sizeof(dummy));

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

  memset(dummy,0,sizeof(dummy));
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

  sprintf(dummy2,"%s",&dummy[strlen(send)+1]);
  memset(dummy,0,sizeof(dummy)); // Clear dummy return
  sprintf(dummy,"%s",dummy2);
  /* ********* */

  shm_addr->MODS.busy[i]=0;   // Clear the HOST busy bit.
  return CMD_OK;

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
  char dummy2[PAGE_SIZE];
  char modserr[512];

  strcpy(send,cmd);
  strcat(send,"\r"); // Add a '\r'. <CR> to satisfy MicroLynx controller

  shm_addr->MODS.busy[i]=1;    // Set the HOST busy bit.
  memset(dummy,0,sizeof(dummy)); // Clear the dummy and start again.
  memset(dummy2,0,sizeof(dummy2)); // Clear the dummy and start again.

  if(WriteTTYPort(&shm_addr->MODS.commport[i],send)<0) {
    sprintf(dummy,"%s=TIMEOUT rawCommand cannot write to %s",
	    makeUpper(shm_addr->MODS.who[i]),
	    shm_addr->MODS.commport[i].Port);
    shm_addr->MODS.busy[i]=0;    // Clear the HOST busy bit.
    return CMD_ERR;
  }

  MilliSleep(100);

  if(ierr=ReadTTYPort(&shm_addr->MODS.commport[i],dummy,
		      (long )shm_addr->MODS.timeout[i])<0) {
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
    if(ReadTTYPort(&shm_addr->MODS.commport[i],dummy,1L)<0) {
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
  shm_addr->MODS.busy[i]=1;    // Set the HOST busy bit.

  if(WriteTTYPort(&shm_addr->MODS.commport[i],send)<0) {
    sprintf(dummy,"%s=TIMEOUT rawCommandOnly cannot write %s",
	    makeUpper(shm_addr->MODS.who[i]),
	    shm_addr->MODS.commport[i].Port);
    return CMD_ERR;
  }

  MilliSleep(10);
  ReadTTYPort(&shm_addr->MODS.commport[i],dummy,3L);
  memset(dummy,0,sizeof(dummy)); // Clear the dummy and start again.
  shm_addr->MODS.busy[i]=0;    // reset busy bit.
  return CMD_OK;
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
hebCommand(int i, char cmd[], char dummy[])
{
  int ierr;
  char send[64];
  char dummy2[PAGE_SIZE];
  char modserr[512];

  strcpy(send,cmd);

  shm_addr->MODS.busy[i]=1;    // Set the HOST busy bit.
  memset(dummy,0,sizeof(dummy)); // Clear the dummy and start again.
  memset(dummy2,0,sizeof(dummy2)); // Clear the dummy and start again.

  if(WriteTTYPort(&shm_addr->MODS.commport[i],send)<0) {
    sprintf(dummy,"%s=TIMEOUT rawCommand cannot write to %s",
	    makeUpper(shm_addr->MODS.who[i]),
	    shm_addr->MODS.commport[i].Port);
    shm_addr->MODS.busy[i]=0;    // Clear the HOST busy bit.
    return CMD_ERR;
  }

  MilliSleep(100);

  if(ierr=ReadTTYPort(&shm_addr->MODS.commport[i],dummy,
		      (long )shm_addr->MODS.timeout[i])<0) {
    sprintf(dummy,"%s=TIMEOUT rawCommand cannot read from %s",
	    makeUpper(shm_addr->MODS.who[i]),
	    shm_addr->MODS.commport[i].Port);
    shm_addr->MODS.busy[i]=0;   // Clear the HOST busy bit.
    return CMD_ERR;
  }

  rmcrlf(dummy,dummy);

  shm_addr->MODS.busy[i]=0;   // Clear the HOST busy bit.
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
    sprintf(dummy,"%s=FAULT cable disconnected or sensor fault",
	    makeUpper(shm_addr->MODS.who[i]),ierr);
    shm_addr->MODS.busy[i]=0;    // Clear the HOST busy bit.
    return CMD_ERR;
  }

  shm_addr->MODS.busy[i]=0;    // Clear the HOST busy bit.
  return(ierr);
}

//---------------------------------------------------------------------------
//
// mlcQuery(i,timeout,dummy)
//

/*!
  \brief Query the device with a short time out

  \param i index of the mechanism
  \param timeout timeout is in sec's
  \param dummy return time out message if timed out
  \return The value of success for now (0)

  Query the named mechanism
  <pre>

  </pre>

*/

int 
mlcQuery(int i, long timeout, char dummy[])
{
  int ierr;

  shm_addr->MODS.busy[i]=1;    // Clear the HOST busy bit.
  
  if(WriteTTYPort(&shm_addr->MODS.commport[i],"PRINT WHO\r")<0) {
    sprintf(dummy,"%s=TIMEOUT cannot write to %s",
	    makeUpper(shm_addr->MODS.who[i]),
	    shm_addr->MODS.commport[i].Port);
    shm_addr->MODS.busy[i]=0;   // Clear the HOST busy bit.
    return CMD_ERR;
  }
  MilliSleep(10);
  if(ReadTTYPort(&shm_addr->MODS.commport[i],dummy,timeout)<0) {
    sprintf(dummy,"%s=TIMEOUT cannot read from %s",
	    makeUpper(shm_addr->MODS.who[i]),
	    shm_addr->MODS.commport[i].Port);
    shm_addr->MODS.busy[i]=0;   // Clear the HOST busy bit.
    return CMD_ERR;
  }

  shm_addr->MODS.busy[i]=0;   // Clear the HOST busy bit.
  return CMD_OK;
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

  ReadTTYPort(&shm_addr->MODS.commport[i],dummy,1L);

  memset(dummy,0,sizeof(dummy)); // Clear the dummy before you start
  sprintf(dummy,"%s=STOPPED mechanism and/or operation was halted",
	  makeUpper(shm_addr->MODS.who[i]));

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

  shm_addr->MODS.busy[i]=1;    // set busy bit.

  ierr=0;
  memset(dummy,0,sizeof(dummy)); // Clear the dummy before you start
  ierr=sendCommand(i,"MASK_ID",dummy); // Get mask current ID

  if(ierr!=0)  return CMD_ERR;

  masknumber=atoi(dummy);
  shm_addr->MODS.busy[i]=0;    // Clear the HOST busy bit.

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

  memset(dummy,0,sizeof(dummy)); // Clear the dummy before you start

  /* List active comm port(s) */
  sprintf(dummy,"PORTS '%s %s' ",shm_addr->MODS.who[0],shm_addr->MODS.commport[0].Port);
  for(i=1;i<MAX_ML;i++) {
    if(shm_addr->MODS.host[i]) {
      sprintf(dummy,"%s '%s %s' ",dummy,shm_addr->MODS.who[i],shm_addr->MODS.commport[i].Port);

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
mlcWhoAmI(int device,char mechanism_name[],char dummy[])
{
  char dummy2[PAGE_SIZE];
  char whoisit[24];

  shm_addr->MODS.busy[device]=1;    // set busy bit.

  memset(dummy,0,sizeof(dummy)); // Clear the dummy before you start
  memset(dummy2,0,sizeof(dummy2));
  WriteTTYPort(&shm_addr->MODS.commport[device],"WHO\r");
  ReadTTYPort(&shm_addr->MODS.commport[device],dummy2,10L);

  rmcrlf(dummy2,dummy2); // remove all CR and LF
  GetArg(dummy2,2,whoisit); // The one we want is the second argument.

  if(atoi(dummy2)>0) {
    sprintf(dummy,"MLCERR=FAULT Microlynx Controller ML%d not preloaded you must use an engineering software program",device);
    shm_addr->MODS.busy[device]=0;    // clear busy bit.
    return CMD_ERR;
  }

  if(!strstr(dummy2,shm_addr->MODS.who[device])) {
    sprintf(dummy,"MLCERR=FAULT requested name '%s' does not match microLynx Controller '%s'",mechanism_name, dummy2);
    shm_addr->MODS.busy[device]=0;    // clear busy bit.
    return CMD_ERR;
  }

  shm_addr->MODS.busy[device]=0;    // clear busy bit.
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
  //  shm_addr->MODS.busy[device]=0;    // clear busy bit.
  
  for(dev=0;
      !strstr(mechanism_name,shm_addr->MODS.who[dev]) && dev<=MAX_ML;
      dev++);

  if(dev<0 || dev>=MAX_ML-1) {
    sprintf(dummy,"Unknown mechanism '%s'",makeUpper(mechanism_name));
    return CMD_ERR;
  }

  return dev;
}

//---------------------------------------------------------------------------
//
// getWagoID(dummy,reply) - Get the mechanism ID from Shared Memory
//

/*!
  \brief Get the WAGO ID from Shared Memory

  \param dummy wago id
  \param reply reply message
  \return wagoID  or error

  Get WAGO ID

  <pre>
  </pre>

*/

int 
getWagoID(char wagoid_name[], char dummy[])
{
  int dev;
  int len;
  char dummy2[PAGE_SIZE];

  memset(dummy2,0,sizeof(dummy2));
  
  for(dev=0;
      !strstr(wagoid_name,shm_addr->MODS.WAGOWHO[dev]) && dev<=6;
      dev++);

  if(dev<0 || dev>=6) {
    sprintf(dummy,"Unknown WAGO device '%s'",makeUpper(wagoid_name));
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

  shm_addr->MODS.busy[device]=1;    // set busy bit.
  memset(dummy,0,sizeof(dummy));
  /* Check the Power Failure variable PWRFAIL */
  WriteTTYPort(&shm_addr->MODS.commport[device],"PRINT PWRFAIL\r");
  MilliSleep(100);
  ReadTTYPort(&shm_addr->MODS.commport[device],dummy,5L);
  rmcrlf(dummy,dummy); // remove all CR and LF
  ierr=atoi(&dummy[14]);
  if(ierr) {
    sprintf(dummy,"%s=PWRFLR %s has been power cycled and must be reset to initialize",makeUpper(shm_addr->MODS.who[device]),makeUpper(shm_addr->MODS.who[device]));
    shm_addr->MODS.busy[device]=0;    // clear busy bit.
      return CMD_ERR;
  }
  shm_addr->MODS.busy[device]=0;    // clear busy bit.

  return CMD_OK;

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

  memset(dummy,0,sizeof(dummy));
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
  char mlccmd[PAGE_SIZE]; // to remove any commas
  char dummy2[PAGE_SIZE];
  char dummy3[PAGE_SIZE];
  char tempo[200][24];
  char tempo2[24];
  char tempo3[24]; // save the IO parameter label

  shm_addr->MODS.busy[device]=1;    // set busy bit.

  memset(dummy,0,sizeof(dummy));
  memset(dummy2,0,sizeof(dummy2));
  memset(dummy3,0,sizeof(dummy3));
  for(j=0;j<100;j++) memset(tempo[j],0,sizeof(tempo[j]));

  memset(tempo2,0,sizeof(tempo2));
  memset(tempo3,0,sizeof(tempo3));

  sprintf(cmd2,"%s",&args[2]); // Get MicroLynx instruction

  if (strstr(args,"LIST") || strstr(args,"VARS")) {
    sprintf(dummy,"%s MLCERR=FAULt MicroLynx Command <%s> not allowed or busy",who,cmd_instruction); 
    shm_addr->MODS.busy[device]=0;    // clear busy bit.
    return CMD_ERR;
  }

  cmd2[strlen(cmd2)]='\0';
  GetArg(cmd2,1,mlccmd);

  if(cmd2[0]=='"') {
    memset(cmd_instruction,0,sizeof(cmd_instruction));
    for(i=1,j=0;cmd2[i]!='\0';i++) cmd_instruction[j++]=cmd2[i];

    if(strlen(cmd2)>=76) {
      sprintf(dummy,"%s MLCERR=FAULT Command string too long for Microlynx controller or busy",who,cmd_instruction); 
      shm_addr->MODS.busy[device]=0;    // clear busy bit.
      return CMD_ERR;
    }

    len=strlen(cmd_instruction);
    if(cmd_instruction[len-1]=='"') {
      cmd_instruction[len-1]='\0';
    }

    ierr=rawCommand(device,cmd_instruction,dummy2); // Send a Raw command

    if(strlen(dummy2)<=1 && !strstr(cmd_instruction,"=")) 
       sprintf(dummy,"%s MLCOUT=UNKNOWN mangled command (example: mechanism m#\"print pos accl io 21 \"or\" mechanism m#\"print pos,vm,io 21,io 25\"  ", who);
    else if(strstr(cmd_instruction,"=")); 
    //else sprintf(dummy,"%s MLCOUT=%s",who,&dummy2[strlen(who)+1]);
    else sprintf(dummy,"%s MLCOUT=%s",who,dummy2); // data fill

    shm_addr->MODS.busy[device]=0;    // clear busy bit.

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
	    shm_addr->MODS.busy[device]=0;    // clear busy bit.

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

	if(ierr<=-1) {
	  sprintf(dummy,"%s MLCERR=%s",who,dummy2); // fill the return
	  shm_addr->MODS.busy[device]=0;    // clear busy bit.
	  return CMD_ERR;
	}

	/* 
	 * Stack the returns into dummy3 until 
	 * all requests have been sent and received 
	 */
	if(!strcasecmp(tempo3,"IO")) {
	  if(tempoLen!=0)
	    sprintf(dummy3,"%s IO%s=%s",dummy3,tempo[i],dummy2); // data fill
	  memset(tempo3,0,sizeof(tempo3));
	} else 
	  if(tempoLen!=0)
	    sprintf(dummy3,"%s %s=%s",dummy3,tempo[i],dummy2); // data fill
	i++;
      }
    }
    sprintf(dummy,"%s%s",who,dummy3); // output data.
    memset(cmd_instruction,0,sizeof(cmd_instruction));
    shm_addr->MODS.busy[device]=0;    // clear busy bit.
    return CMD_OK;
  } else
    ierr=rawCommand(device,cmd2,dummy2); // Send a Raw command

  sprintf(dummy,"%s %s=%s",who,mlccmd,dummy2);

  memset(cmd_instruction,0,sizeof(cmd_instruction));
  shm_addr->MODS.busy[device]=0;    // clear busy bit.
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
  memset(dummy,0,sizeof(dummy));
  shm_addr->MODS.busy[i]=1;    // set busy bit.

  if(WriteTTYPort(&shm_addr->MODS.commport[i],"IP\r")<0) {
    sprintf(dummy,"%s=TIMEOUT mlcClear cannot write to %s",
	    shm_addr->MODS.who[i],
	    shm_addr->MODS.commport[i].Port);
    shm_addr->MODS.busy[i]=0;    // clear busy bit.
    return CMD_ERR;
  }
  if(ReadTTYPort(&shm_addr->MODS.commport[i],dummy,10L)<0) {
    sprintf(dummy,"%s=TIMEOUT mlcClear cannot read from %s",
	    shm_addr->MODS.who[i],
	    shm_addr->MODS.commport[i].Port);
	    shm_addr->MODS.busy[i]=0;    // clear busy bit.
    return CMD_ERR;
  }

  MilliSleep(100);
  if(WriteTTYPort(&shm_addr->MODS.commport[i],"DVF\r")<0) {
    sprintf(dummy,"%s=TIMEOUT mlcClear cannot write to %s",
	    shm_addr->MODS.who[i],
	    shm_addr->MODS.commport[i].Port);
    shm_addr->MODS.busy[i]=0;    // clear busy bit.
    return CMD_ERR;
  }

  if(ReadTTYPort(&shm_addr->MODS.commport[i],dummy,10L)<0) {
    sprintf(dummy,"%s=TIMEOUT mlcClear cannot read from %s",
	    shm_addr->MODS.who[i],
	    shm_addr->MODS.commport[i].Port);
    shm_addr->MODS.busy[i]=0;  // Clear the HOST busy bit.
    return CMD_ERR;
  }

  MilliSleep(100);
  if(WriteTTYPort(&shm_addr->MODS.commport[i],"CP 1,1\r")<0) {
    sprintf(dummy,"%s=TIMEOUT mlcClear cannot write to %s",
	    shm_addr->MODS.who[i],
	    shm_addr->MODS.commport[i].Port);
    shm_addr->MODS.busy[i]=0;    // clear busy bit.
    return CMD_ERR;
  }
  if(ReadTTYPort(&shm_addr->MODS.commport[i],dummy,30L)<0) {
    sprintf(dummy,"%s=TIMEOUT mlcClear cannot read from %s",
	    shm_addr->MODS.who[i],
	    shm_addr->MODS.commport[i].Port);
    shm_addr->MODS.busy[i]=0;  // Clear the HOST busy bit.
    return CMD_ERR;
  }

  MilliSleep(100);
  if(WriteTTYPort(&shm_addr->MODS.commport[i],"SAVE\r")<0) {
    sprintf(dummy,"%s=TIMEOUT mlcClear cannot write to %s",
	    shm_addr->MODS.who[i],
	    shm_addr->MODS.commport[i].Port);
    shm_addr->MODS.busy[i]=0;    // clear busy bit.
    return CMD_ERR;
  }
  if(ReadTTYPort(&shm_addr->MODS.commport[i],dummy,10L)<0) {
    sprintf(dummy,"%s=TIMEOUT mlcClear cannot read from %s",
	    shm_addr->MODS.who[i],
	    shm_addr->MODS.commport[i].Port);
    shm_addr->MODS.busy[i]=0;  // Clear the HOST busy bit.
    return CMD_ERR;
  }
  shm_addr->MODS.busy[i]=0;  // Clear the HOST busy bit.

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
// isisStatusMsg(dummy) - Send a status message to ISIS
//

/*!
  \brief Status message for request Agent

  \param dummy message string

  \return The value of -1 is error

  Sends status information

  <pre>
  </pre>

*/

int 
isisStatusMsg(char dummy[])
{
  int ierr;
  char msg[512];
  
  memset(msg,0,sizeof(msg));
  
  if(!strncasecmp(dummy,"WARNING:",8))
    strcpy(msg,ISISMessage(client.ID,who_srcID,WARNING,&dummy[8]));
  else if(!strncasecmp(dummy,"FATAL:",6))
    strcpy(msg,ISISMessage(client.ID,who_srcID,FATAL,&dummy[6]));
  else if(!strncasecmp(dummy,"ERROR:",6))
    strcpy(msg,ISISMessage(client.ID,who_srcID,ERROR,&dummy[6]));
  else if(!strncasecmp(dummy,"DONE:",5))
    strcpy(msg,ISISMessage(client.ID,who_srcID,DONE,&dummy[5]));
  else
    strcpy(msg,ISISMessage(client.ID,who_srcID,STATUS,dummy));
  msg[strlen(msg)-1]='\0';
  SendToISISServer(&client,msg); // and send it off

  return CMD_OK;
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
  shm_addr->MODS.busy[i]=1;  // set busy bit.

  memset(dummy,0,sizeof(dummy)); // Clear dummy
  ierr=rawCommand(i,cmd,dummy);

  if(ierr!=0) {
    shm_addr->MODS.busy[i]=0;  // Clear busy bit.
    return CMD_ERR;
  }

  shm_addr->MODS.busy[i]=0;  // Clear busy bit.

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
  int bits;
  memset(dummy,0,sizeof(dummy));

  shm_addr->MODS.busy[i]=1;  // Set busy bit.

  bits=mlcBitsBase10(i,"PRINT IO 21,IO 22",dummy);
  switch(bits) {
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
    shm_addr->MODS.busy[i]=0;  // Clear busy bit.
    return CMD_ERR;
  }
  shm_addr->MODS.busy[i]=0;  // Clear busy bit.

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

  shm_addr->MODS.busy[i]=1;  // Set busy bit.

  sprintf(mechanism_name,"%s",shm_addr->MODS.who[i]); // Get mechanisms name

  if(what==0) { // Bi-State
    ierr = sendCommand(i,how,temp);
    if(ierr!=0) {
      sprintf(dummy,"%s %s=%s", 
	      who, makeUpper(mechanism_name), temp);
      shm_addr->MODS.busy[i]=0;  // Clear busy bit.
      return CMD_ERR;
    }

    sprintf(dummy,"%s %s=%s Reset Successful", 
	    who, makeUpper(mechanism_name), temp);

    shm_addr->MODS.pos[i]=positionToShrMem(i,temp);
    
  } else if(what==1) { // Linear
    sprintf(dummy,"%s %s=Not Yet!",who,mechanism_name);    

  } else if(what==2) { // Indexed
    sprintf(dummy,"%s %s=Not Yet!",who,mechanism_name);
  }
  shm_addr->MODS.busy[i]=0;  // Clear busy bit.

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

  shm_addr->MODS.busy[i]=1;  // Set busy bit.

  val = ((val/shm_addr->MODS.convf[i])); // convert physical units to revs
  sprintf(valStr,"%f",val); // convert the float step value to a string 

  memset(dummy,0,sizeof(dummy));
  memset(temp,0,sizeof(temp));


  sprintf(mechanism_name,"%s",shm_addr->MODS.who[i]); // Get mechanisms name

  if(what==0 || what==1) { // Bi-State, and Linear mechanisms
    sprintf(temp,"MOVR %s",valStr);

    rawCommand(i,temp,dummy);
    shm_addr->MODS.pos[i]=fabs(positionToShrMem(i,temp));
    sprintf(dummy,"%s %s=%f",
	    who,mechanism_name, shm_addr->MODS.pos[i]);

  } else if(what==2) { // Indexed
    ierr = sendTwoCommand(i,"TARGNUM=",valStr,temp);
    ierr = sendCommand(i,"BEGIN",temp);

    shm_addr->MODS.pos[i]=positionToShrMem(i,temp);

    if(ierr!=0) {
      sprintf(dummy,"%s %s=%f", 
	      who,mechanism_name,shm_addr->MODS.pos[i]);
      shm_addr->MODS.busy[i]=0;  // Clear busy bit.
      return CMD_ERR;
    }

    sprintf(dummy,"%s %s=%f", 
	    who,mechanism_name,shm_addr->MODS.pos[i]);
  }

  shm_addr->MODS.busy[i]=0;  // Clear busy bit.

  return CMD_OK;
}

//---------------------------------------------------------------------------
//
// wagoRW(iebID, who, what, cmd, dummy) - Read and/or Write to Wago system
//

/*!
  \brief Last known mechanism position, lamps, and lasers

  \param iebID request IEB 1 or 2
  \param who request from who
  \param what Set or Query (1 or 0)
  \param value a command or an item
  \param dummy string that contains reply

  \return 0=ok, -1=error

  Mechanisms, Lamps, and Lasers status

  <pre>
  </pre>
  2025 Oct 29 - This is still a mess, put in diagnostic printouts
  
*/

int 
wagoRW(int iebID, char who[], int what, int value,char dummy[])
{
  int i;
  int ierr;
  int cmd;
  int ieb_id;
  short onoff[1];
  short regData[1];
  char temp[512];
  int blueIndex;
  float temperature[5];
  float vdrive;
  float idrive;
  float vcontrol;
  
  memset(dummy,0,sizeof(dummy));
  memset(temp,0,sizeof(temp));

  ieb_id=iebID-1;

  if (!strcasecmp(who,"IEBS")) {
    if (what) {
      onoff[0]=(short)value; // Turn off the 65V power supply
      onoff[1]=(short)value;
      ierr = wagoSetGet(1,shm_addr->MODS.WAGOIP[ieb_id],1,513,&onoff[0],1);
      if (!ierr)	sprintf(dummy,"IEB_%c=ON MPOWER_%c=ON",
			(iebID==1 ? 'R' : 'B'),
			(iebID==1 ? 'R' : 'B'));
      else if (ierr==-1)
	sprintf(dummy,"IEB_%c=OFF",(iebID==1 ? 'R' : 'B'));
      else 
	sprintf(dummy,"IEB_%c=UNKNOWN",(iebID==1 ? 'R' : 'B'));
      return ierr;

    }
    else {
      //printf("wagoRW IEBS what=%d argbuf=%s\n",what,argbuf);
      //ierr = wagoSetGet(what,shm_addr->MODS.WAGOIP[ieb_id],1,513,(short *)atoi(argbuf),1);
      ierr = wagoSetGet(what,shm_addr->MODS.WAGOIP[ieb_id],1,513,regData,1);
      printf("wagoRW IEBS ierr=%d\n",ierr);
      if (!ierr)
	sprintf(dummy,"IEB_%c=ON MPOWER_%c=ON",
		(iebID==1 ? 'R' : 'B'),
		(iebID==1 ? 'R' : 'B'));
      else if (ierr==-1)
	sprintf(dummy,"IEB_%c=OFF MPOWER_%c=OFF",
		(iebID==1 ? 'R' : 'B'),
		(iebID==1 ? 'R' : 'B'));
      else 
	sprintf(dummy,"IEB_%c=UNKNOWN",
		(iebID==1 ? 'R' : 'B'));
      return ierr;
    }
    return CMD_OK;
  }

  else if (!strcasecmp(who,"BYNAME")) {
    //ierr = wagoSetGet(what,shm_addr->MODS.WAGOIP[ieb_id],1,513,(short *)atoi(argbuf),1);
    ierr = wagoSetGet(what,shm_addr->MODS.WAGOIP[ieb_id],1,513,regData,1);
    MilliSleep(100);

    ierr = wagoSetGet(what,shm_addr->MODS.WAGOIP[ieb_id],1,512,onoff,1);
    if (iebID==1)
      blueIndex=-1;
    else
      blueIndex=17;

    if (value==0 || value>16) {
      if (ierr==-1) {
	for (i=1,ierr=1;ierr<=16;i+=i,ierr++) 
	  sprintf(dummy,"%s %s_%s=OFF",dummy,
		  makeUpper(shm_addr->MODS.who[ierr+blueIndex]),
		  (iebID==1 ? "R" : "B"));
      } else {
 	for (i=1,ierr=1;ierr<=16;i+=i,ierr++) {
	  sprintf(dummy,"%s %s_%s=%s",dummy,
		  makeUpper(shm_addr->MODS.who[ierr+blueIndex]),
		  (iebID==1 ? "R" : "B"),(!(i&onoff[0]) ? "ON" : "OFF"));
	}
      }
    } else {
      for (i=1,ierr=1;ierr<=value-1;i+=i,ierr++);
      sprintf(dummy,"%s_%s=%s",makeUpper(shm_addr->MODS.who[ierr+blueIndex]),
	      (iebID==1 ? "R" : "B"),
	      (!(i&onoff[0]) ? "ON" : "OFF"));
    }
  }

  else if (!strcasecmp(who,"MLCS")) {
    //printf("wagoRW MLCS what=%d argbuf=%s\n",what,argbuf);
    //ierr = wagoSetGet(what,shm_addr->MODS.WAGOIP[ieb_id],1,513,(short *)atoi(argbuf),1);
    ierr = wagoSetGet(what,shm_addr->MODS.WAGOIP[ieb_id],1,513,regData,1);
    printf("wagoRW MLCS ierr=%d\n",ierr);
    MilliSleep(100);
    ierr = wagoSetGet(what,shm_addr->MODS.WAGOIP[ieb_id],1,512,onoff,1);
    printf("wagoRW MLCS onoff ierr=%d\n",ierr);

    if (value==0 || value>16) {
      if (ierr==-1) {
	for(i=1,ierr=1;ierr<=16;i+=i,ierr++) 
	  sprintf(dummy,"%s MLC%d_%s=OFF",dummy,ierr,
		  (iebID==1 ? "R" : "B"));
      }
      else {
 	for(i=1,ierr=1;ierr<=16;i+=i,ierr++) {
	  sprintf(dummy,"%s MLC%d_%s=%s",dummy,ierr,
		  (iebID==1 ? "R" : "B"),(!(i&onoff[0]) ? "ON" : "OFF"));
	}
      }
    }
    else {
      if(value>18) value-=18; // This is for the BLUE IEB set 18-34

      for(i=1,ierr=1;ierr<=value-1;i+=i,ierr++);
      sprintf(dummy,"MLC%d_%s=%s",abs(value),(iebID==1 ? "R" : "B"),
	      (!(i&onoff[0]) ? "ON" : "OFF"));
    }

  }

  else if (!strcasecmp(who,"TEMPS")) {

    char tempRMonitor[4][9] = {"IEBTEMPR","IEBGRT_R","TAIRTOP","TAIRBOT" };
    char tempBMonitor[4][9] = {"IEBTEMPB","IEBGRT_B","TCOLLTOP","TCOLLBOT"};
    
    if (!value) {
      for (i=4,ierr=0;i<8;i++,ierr++) {
	ierr = wagoSetGet(what,shm_addr->MODS.WAGOIP[ieb_id],1,i,regData,1);
	temperature[ierr]=(float)(regData[0])/10.0;
	if (iebID==1) {
	  sprintf(dummy,"%s %s=%0.1f",dummy,tempRMonitor[ierr],temperature[ierr]);
	}
	else {
	  sprintf(dummy,"%s %s=%0.1f",dummy,tempBMonitor[ierr],temperature[ierr]);
	}
      }
    } else {
      ierr = wagoSetGet(what,shm_addr->MODS.WAGOIP[ieb_id],1,value+3,regData,1);
      temperature[value-1]=(float)(regData[0])/10.0;
      if (iebID==1)
	sprintf(dummy,"%s %s=%0.1f",dummy,tempRMonitor[value-1],temperature[value-1]);
      else
	sprintf(dummy,"%s %s=%0.1f",dummy,tempBMonitor[value-1],temperature[value-1]);
    }
  }

  else if(!strcasecmp(who,"MPOWER")) {
    //printf("wagoRW MPOWER what=%d argbuf=%s\n",what,argbuf);
    //ierr = wagoSetGet(what,shm_addr->MODS.WAGOIP[ieb_id],1,513,(short *)atoi(argbuf),1);
    ierr = wagoSetGet(what,shm_addr->MODS.WAGOIP[ieb_id],1,513,regData,1);
    printf("wagoRW MPOWER ierr=%d\n",ierr);
    
    sprintf(dummy,"%s MPOWER_%c=%s ",dummy,(iebID==1 ? 'R' : 'B'),( !ierr ? "ON" : "OFF"));

    ierr = wagoSetGet(what,shm_addr->MODS.WAGOIP[ieb_id],1,0,regData,1);
    vdrive = ((float)(regData[0])/pow(2,15)*10)*9.28;
    sprintf(dummy,"%s VDRIVE_%c=%0.3f",dummy,(iebID==1 ? 'R' : 'B'),vdrive);

    ierr = wagoSetGet(what,shm_addr->MODS.WAGOIP[ieb_id],1,2,regData,1);
    idrive = ((float)(regData[0])/pow(2,15)*10)*1.25;
    sprintf(dummy,"%s IDRIVE_%c=%0.3f",dummy,(iebID==1 ? 'R' : 'B'),idrive);

  }

  else if(!strcasecmp(who,"IVS")) {

    ierr = wagoSetGet(what,shm_addr->MODS.WAGOIP[ieb_id],1,0,regData,1);
    vdrive = ((float)(regData[0])/pow(2,15)*10)*9.28;
    sprintf(dummy,"%s VDRIVE_%c=%0.3f",dummy,(iebID==1 ? 'R' : 'B'),vdrive);

    ierr = wagoSetGet(what,shm_addr->MODS.WAGOIP[ieb_id],1,2,regData,1);
    idrive = ((float)(regData[0])/pow(2,15)*10)*1.25;
    sprintf(dummy,"%s IDRIVE_%c=%0.3f",dummy,(iebID==1 ? 'R' : 'B'),idrive);

    ierr = wagoSetGet(what,shm_addr->MODS.WAGOIP[ieb_id],1,1,regData,1);
    vcontrol = ((float)(regData[0])/pow(2,15)*10)*3.12;
    sprintf(dummy,"%s VCONTROL_%c=%0.3f",dummy,(iebID==1 ? 'R' : 'B'),vcontrol);
  }

  return CMD_OK;

}

//---------------------------------------------------------------------------
//
// mlcMechanismConfig(i,who,dummy) - get Configuration information
//

/*!
  \brief Get mechanism configuration information

  \param device address of the mechanism
  \param who Name of mechanism 
  \param dummy string contains reply

  Queries the named mechanisms configuration information

  <pre>
  </pre>

*/

int 
mlcMechanismConfig(int device, char who [], char dummy[])
{
  int ierr;
  char mlccomm[10];

  shm_addr->MODS.busy[device]=1;  // Set busy bit.
  memset(dummy,0,sizeof(dummy)); // Clear dummy

  sprintf(dummy,"%s %s=%s IEB=%s IP=%s MLC=%d MIN=%0.0f MAX=%0.0f TIMEOUT=%d CFACTOR=%0.0f",who,who,shm_addr->MODS.state_word[device],
	  (shm_addr->MODS.ieb_i[device]==1 ? "RED" : "BLUE"),
	  shm_addr->MODS.commport[device].Port,
	  (shm_addr->MODS.ieb_i[device]==1 ? device+1 : device-15),
	  shm_addr->MODS.min[device],
	  shm_addr->MODS.max[device],
	  shm_addr->MODS.timeout[device],
	  shm_addr->MODS.convf[device]);

  shm_addr->MODS.busy[device]=0;  // Clear busy bit.

  return CMD_OK;
}

//---------------------------------------------------------------------------
//
// mlcStow(who,dummy) - Stow MODS mechanism(s) 
//

/*!
  \brief stow mechanism(s)

  \param who Name of mechanism or all 
  \param dummy string contains reply

  Stows on or all MODS mechanisms

  <pre>
  </pre>

*/

int 
mlcStow(char who [], char dummy[])
{
  int i;
  int ierr;
  char temp[80];
  char temp2[80];

  i=getMechanismID(who, temp2);

  shm_addr->MODS.busy[i]=1;  // Clear busy bit.

  memset(dummy,0,sizeof(dummy)); // Clear dummy
  sprintf(temp,"%s reset",who);
  KeyCommand(temp, dummy);
  if(strstr(dummy,"ERROR:")) {
    sprintf(dummy,"RESET Invalid request %s",&dummy[6]);
    shm_addr->MODS.busy[i]=0;  // Clear busy bit.
    return CMD_ERR;
  }
  shm_addr->MODS.busy[i]=0;  // Clear busy bit.

  sprintf(dummy,"%s",&dummy[6]);
  return CMD_OK;
}

//---------------------------------------------------------------------------
//
// mlcPANIC(who, dummy) - PANIC very serious command
//

/*!
  \brief panic Turns off all Power to MODS1 and/or MODS2. If this command is 
  \brief used it means that something very serious has occurred.

  \param who Name of MODSx (x=1..2)
  \param dummy string contains reply

  PANIC!

  <pre>
  </pre>

*/

int 
mlcPANIC(int device, char who [], char dummy[])
{
  int ierr;
  char temp[80];
  short devOnOff[1],devEnable[1];

  if(!strcasecmp(who,"MODS1")) {
    devOnOff[0]=0;
    devOnOff[1]=0;
    ierr = wagoSetGet(1,shm_addr->MODS.WAGOIP[device],1,512,devOnOff,1);
    
  } else if(!strcasecmp(who,"MODS2")) {
    devOnOff[0]=0;
    devOnOff[1]=0;
    ierr = wagoSetGet(1,shm_addr->MODS.WAGOIP[device],1,512,devOnOff,1);

  } else {
    sprintf(dummy,"Invalid '%s' command",argbuf); 
    return CMD_ERR;
  }

  if(ierr!=0) {
    sprintf(dummy,"PANIC %s error occurred while testing WAGO system",who);
    return CMD_ERR;
  }

  sprintf(dummy,"UTIL=OFF %s a careful evaluation is needed, Please review documentation",who);

  return ierr;
}

//---------------------------------------------------------------------------
//
// agwRestore(int, char[], char[]) - Restore AGw from a CALMODE saved position
//

/*!
  \brief agwRestore AGw guide probe

  Usage: agwRestore(obscal, who_selected, dummy1)

  obscal = Not used here.
  dummy1 = Return message
  who_selected = who called me
  return   0=OK 0<Error Error means that an error was detected;

  Moves the AGw guide probe to the previously saved position, 

  <pre>
  </pre>

*/


int 
agwRestore(int obscal, char who_selected[], char dummy1[])
{
  char errorMsg[PAGE_SIZE]; 
  char temp[PAGE_SIZE];
  char dummy[PAGE_SIZE];
  char statusMsg[PAGE_SIZE];

  int agwYID         = getMechanismID("agwy", dummy);
  int agwXID         = getMechanismID("agwx", dummy);
  int agwFID         = getMechanismID("agwfoc", dummy);
  int len            = 0; // Length for building the reply
  int cmdLen         = 0; // Command length
  int errorLen       = 0; // Length for building error message
  int mechanismError = 0; // Length for building error message
  int ierr           = 0;
  int YPos           = 0;
  int XPos           = 0;
  int FPos           = 0;

  len = strlen(dummy1);
  memset(dummy, 0, sizeof(dummy));
  
  // Check position of AGw stage

  ierr = agwcu("localhost", 0, "agwy -c rdbits", dummy);
  GetArg(dummy,2,argbuf);
  if ( strcasecmp(dummy,"IO21=1") == 6 ) YPos = 1;
  ierr = agwcu("localhost",0,"agwx -c rdbits",dummy);
  memset(argbuf, 0, sizeof(argbuf));
  GetArg(dummy,2,argbuf);
  if ( strcasecmp(dummy,"IO21=1") == 0 ) XPos = 1;
  memset(argbuf, 0, sizeof(argbuf));
  GetArg(dummy,2,argbuf);
  ierr = agwcu("localhost",0,"agwfoc -c rdbits",dummy);
  if ( strcasecmp(dummy,"IO22=1") == 0 ) XPos = 1;

  YPos = YPos + XPos + FPos;

  memset(dummy, 0, sizeof(dummy));
  if ( YPos > 0 ) {
    KeyCommand("gprobe", dummy); // Stowed
    
    sprintf(statusMsg,"%s AGw guide probe is retracted", who_selected);
    sprintf(&dummy1[len], "%s", &dummy[cmdLen]);
    mechanismError = 0;

  } else if ( shm_addr->MODS.host[agwYID] == 0 
	      || shm_addr->MODS.host[agwXID] == 0 
	      || shm_addr->MODS.host[agwFID] == 0 ) { // No IP Address given for X, Y, or Focus
    
    cmdLen = strlen("ERROR: GPROBE "); // Add a space 
    
    sprintf(statusMsg, "%s could not restore AGw, no IP Address for X,Y and/or Focus, %s GPROBE=FAULT", who_selected, &dummy[cmdLen]);
    
    errorLen = strlen(errorMsg);
    sprintf(&errorMsg[errorLen], "could not restore AGw, no IP Address for X,Y, or Focus, ");

    len = strlen(dummy1);
    sprintf(&dummy1[len], "GPROBE=FAULT ");
    mechanismError = 1;
    
  } else if ( agwYID == -1 || agwXID == -1 || agwFID == -1 ) { //<! Mechanism not in the configuration list
    
    sprintf(statusMsg, "%s could not restore AGw, X Y and/or Focus not found in config file, GPROBE=FAULT", who_selected);

    errorLen = strlen(errorMsg);
    sprintf(&errorMsg[errorLen], "could not restore AGw, X Y or Focus not found in config file, ");

    len = strlen(dummy1);    
    sprintf(&dummy1[len], "GPROBE=FAULT ");
    mechanismError = 1;

  } else {
    len = strlen(dummy1);
    cmdLen = strlen("DONE: GPROBE "); // Add a space 

    sprintf(statusMsg, "%s Restoring AGw XGPLAST=%0.3f YGPLAST=%0.3f", who_selected, shm_addr->MODS.xgpLast, shm_addr->MODS.ygpLast);

    isisStatusMsg(statusMsg);
    
    sprintf(temp, "gprobe %0.3f %0.3f", shm_addr->MODS.xgpLast, shm_addr->MODS.ygpLast);
	
    KeyCommand(temp, dummy); // Restore AGw guide probe position

    if ( strncasecmp(dummy, "ERROR", 5) == 0 ) {
    
      if ( !strstr( dummy, "Invalid guide probe")) {
	cmdLen = strlen("ERROR: GPROBE "); // Add a space 
	sprintf(statusMsg, "%s could not retore AGw, %s GPROBE=FAULT", who_selected, &dummy[cmdLen]);

	errorLen = strlen(errorMsg);
	sprintf(&errorMsg[errorLen], "could not restore AGw, %s ", &dummy[cmdLen]);
	len = strlen(dummy1);
	sprintf(&dummy1[len], "GPROBE=FAULT ");
	mechanismError = 1;
	isisStatusMsg(statusMsg);
	return mechanismError;
      } else {
	cmdLen = strlen("DONE: GPROBE "); // Add a space 

	KeyCommand("gprobe", dummy); // Stowed
	sprintf(statusMsg,"%s AGw guide probe is unchanged", who_selected);
    
	len = strlen(dummy1);
	sprintf(&dummy1[len], "%s", &dummy[cmdLen]);
	mechanismError = 0;
      }
    } else {
      cmdLen = strlen("DONE: GPROBE "); // Add a space 

      KeyCommand("gprobe", dummy); // Stowed
            sprintf(statusMsg,"%s AGw guide probe position restored", who_selected);
      len = strlen(dummy1);
      sprintf(&dummy1[len], "%s", &dummy[cmdLen]);
      isisStatusMsg(statusMsg);
      mechanismError = 0;
    }
  }

  return mechanismError;
}

//---------------------------------------------------------------------------
//
// Hatch(int, char[], char [], char [])
//

/*!
  \brief Open the HATCH or Close HATCH

  Usage: Hatch(obscal, who_selected[], errorMsg[], dummy1[])

  obscal       = (1=CLOSE 0=OPEN)
  who_selected = who called me
  errorMsg     = For printing to isisStatus()
  dummy1       = Return message
  return       = 0=OK, >0=Error

  Open or close the hatch

  <pre>
  </pre>

*/

int 
Hatch(int obscal, char who_selected[], char errorMsg[], char dummy1[])
{
  char dummy[PAGE_SIZE];
  char statusMsg[PAGE_SIZE];

  int hatchID        = getMechanismID("hatch", dummy);
  int len            = 0; // Length for building the reply
  int cmdLen         = 0; // Command length
  int errorLen       = 0; // Length for building error message
  int mechanismError = 0; // Length for building error message
  int ierr           = 0;

  len = strlen(dummy1);
  errorLen = strlen(errorMsg);

  if ( shm_addr->MODS.host[hatchID] == 0 ) { // No IP Address given to hatch

    if ( obscal == 1 ) {
      sprintf(statusMsg, "%s could not close hatch, no IP Address, HATCH=FAULT", who_selected);
      errorLen = strlen(errorMsg);
      sprintf(&errorMsg[errorLen], "could not close hatch, no IP Address, ");

    } else {
      sprintf(statusMsg, "%s could not open hatch, no IP Address, HATCH=FAULT", who_selected);
      errorLen = strlen(errorMsg);
      sprintf(&errorMsg[errorLen], "could not open hatch, no IP Address, ");
    }
    sprintf(&dummy1[len], "HATCH=FAULT ");
    mechanismError = 1;
    
  } else if ( hatchID == -1 ) { //<! Mechanism no in the configuration list
    
    if ( obscal == 1 ) {
      sprintf(statusMsg, "%s could not close hatch. not found in config file, HATCH=FAULT ", who_selected);
      
      errorLen = strlen(errorMsg);
      sprintf(&errorMsg[errorLen], "could not close hatch, not found in config file, ");
    } else {
      sprintf(statusMsg, "%s could not open hatch. not found in config file, HATCH=FAULT ", who_selected);
      
      errorLen = strlen(errorMsg);
      sprintf(&errorMsg[errorLen], "could not open hatch, not found in config file, ");
    }
    sprintf(&dummy1[len], "HATCH=FAULT ");
    mechanismError = 1;
    
  } else {
    
    if ( obscal == 1 ) {
      sprintf(statusMsg,"%s Closing the instrument dark hatch", who_selected); //<! Closing dark hatch
    } else {
      sprintf(statusMsg,"%s Opening the instrument dark hatch", who_selected); //<! Closing dark hatch
    }
    isisStatusMsg(statusMsg); //<! Send a Guide probe status message
    
    KeyCommand("hatch m#pwrfail=0", dummy); // reset the PLC power bit
    if ( obscal == 1 ) {
      KeyCommand("hatch close",dummy);

    } else {
      KeyCommand("hatch open",dummy);

    }

    if ( strncasecmp(dummy, "ERROR", 5) == 0 ) {
      cmdLen = strlen("ERROR: HATCH HATCH="); // Add a space 
      
      if ( obscal == 1 ) {
	sprintf(statusMsg, "%s could not close hatch, %s HATCH=FAULT", who_selected, &dummy[cmdLen]);
	
	errorLen = strlen(errorMsg);
	sprintf(&errorMsg[errorLen], "could not close hatch, %s ", &dummy[cmdLen]);
      } else {
	sprintf(statusMsg, "%s could not open hatch, %s HATCH=FAULT", who_selected, &dummy[cmdLen]);
	
	errorLen = strlen(errorMsg);
	sprintf(&errorMsg[errorLen], "could not open hatch, %s ", &dummy[cmdLen]);
      }
      sprintf(&dummy1[len], "HATCH=FAULT ");
      mechanismError = 1;
      
    } else {
      cmdLen = strlen("DONE: HATCH "); // Add a space 

      if ( obscal == 1 ) {
	sprintf(statusMsg,"%s Hatch closed, %s", who_selected, &dummy[cmdLen]);
      } else 
	sprintf(statusMsg,"%s Hatch opened, %s", who_selected, &dummy[cmdLen]);

      sprintf(&dummy1[len],"%s ", &dummy[cmdLen]);
      mechanismError = 0;
    }

  }
  isisStatusMsg(statusMsg);
  return  mechanismError;
}

//---------------------------------------------------------------------------
//
// Calib(int, char[], char [], char [])
//

/*!
  \brief Retract or Insert the Calibration Tower 

  Usage: Calib(obscal, who_selected[], errorMsg[], dummy1[])

  obscal       = (1=CLOSE 0=OPEN)
  who_selected = who called me
  errorMsg     = For printing to isisStatus()
  dummy1       = Return message
  return       = 0=OK, >0=Error

  Retract or Insert the Calibration Tower

  <pre>
  </pre>

*/

int 
Calib(int obscal, char who_selected[], char errorMsg[], char dummy1[])
{
  char dummy[PAGE_SIZE];
  char statusMsg[PAGE_SIZE];

  int calibID        = getMechanismID("calib", dummy);
  int len            = 0; // Length for building the reply
  int cmdLen         = 0; // Command length
  int errorLen       = 0; // Length for building error message
  int mechanismError = 0; // Length for building error message
  int ierr           = 0;

  len = strlen(dummy1);
  errorLen = strlen(errorMsg);

  if ( shm_addr->MODS.host[calibID] == 0 ) { // No IP Address given to hatch
    if ( obscal == 1 ) {
      sprintf(statusMsg, "%s could not insert Calibration Tower, no IP Address, HATCH=FAULT", who_selected);
      errorLen = strlen(errorMsg);
      sprintf(&errorMsg[errorLen], "could not insert Calibration Tower, no IP Address, ");
    } else {
      sprintf(statusMsg, "%s could not retract Calibration Tower, no IP Address, HATCH=FAULT", who_selected);
      errorLen = strlen(errorMsg);
      sprintf(&errorMsg[errorLen], "could not retract Calibration Tower, no IP Address, ");
    }
    sprintf(&dummy1[len], "CALIB=FAULT ");
    mechanismError = 1;
    
  } else if ( calibID == -1 ) { //<! Mechanism no in the configuration list
    
    if ( obscal == 1 ) {
      sprintf(statusMsg, "%s could not insert Calibration Tower. not found in config file, HATCH=FAULT ", who_selected);
      
      errorLen = strlen(errorMsg);
      sprintf(&errorMsg[errorLen], "could not retract Calibration Tower, not found in config file, ");
    } else {
      sprintf(statusMsg, "%s could not insert Calibration Tower. not found in config file, HATCH=FAULT ", who_selected);
      
      errorLen = strlen(errorMsg);
      sprintf(&errorMsg[errorLen], "could not retract Calibration Tower, not found in config file, ");
    }
    sprintf(&dummy1[len], "CALIB=FAULT ");
    mechanismError = 1;
    
  } else {
    
    if ( obscal == 1 ) {
      sprintf(statusMsg,"%s Inserting Calibration Tower", who_selected); //<! Closing dark hatch
    } else {
      sprintf(statusMsg,"%s Retracting Calibration Tower", who_selected); //<! Closing dark hatch
    }
    isisStatusMsg(statusMsg); //<! Send a Guide probe status message
    
    KeyCommand("calib m#pwrfail=0", dummy); // reset the PLC power bit
    if ( obscal == 1 ) {
      KeyCommand("calib in",dummy);

    } else {
      KeyCommand("calib out",dummy);
    }

    if ( strncasecmp(dummy, "ERROR", 5) == 0 ) {
      cmdLen = strlen("ERROR: CALIB "); // Add a space 
      
      if ( obscal == 1 ) {
	sprintf(statusMsg, "%s could not insert Calibration Tower, %s HATCH=FAULT", who_selected, &dummy[cmdLen]);
	
	errorLen = strlen(errorMsg);
	sprintf(&errorMsg[errorLen], "could not insert Calibration Tower, %s ", &dummy[cmdLen]);
      } else {
	sprintf(statusMsg, "%s could not retract Calibration Tower, %s HATCH=FAULT", who_selected, &dummy[cmdLen]);
	
	errorLen = strlen(errorMsg);
	sprintf(&errorMsg[errorLen], "could not retract Calibration, %s ", &dummy[cmdLen]);
      }
      sprintf(&dummy1[len], "CALIB=FAULT ");
      mechanismError = 1;
      
    } else {
      
      cmdLen = strlen("DONE: CALIB "); // Add a space 

      if ( obscal == 1 ) {
	sprintf(statusMsg,"%s Calibration Tower inserted, %s", who_selected, &dummy[cmdLen]);

      } else 
	sprintf(statusMsg,"%s Calibration Tower retracted, %s", who_selected, &dummy[cmdLen]);

      sprintf(&dummy1[len],"%s ", &dummy[cmdLen]);
      mechanismError = 0;
    }
  }
  isisStatusMsg(statusMsg);

  return  mechanismError;
}

//---------------------------------------------------------------------------
//
// agwProbe(int, char[], char [], char [])
//

/*!
  \brief Retract or Move the AGw guide probe into position

  Usage: agwProbe(obscal, who_selected[], errorMsg[], dummy1[])

  obscal       = not used
  who_selected = who called me
  errorMsg     = For printing to isisStatus()
  dummy1       = Return message
  return       = 0=OK, >0=Error

  Retract or Insert the Calibration Tower

  <pre>
  </pre>

*/

int 
agwProbe(int obscal, char who_selected[], char errorMsg[], char dummy1[])
{
  char temp[PAGE_SIZE];
  char dummy[PAGE_SIZE];
  char statusMsg[PAGE_SIZE];

  int agwYID         = getMechanismID("agwy", dummy);
  int agwXID         = getMechanismID("agwx", dummy);
  int agwFID         = getMechanismID("agwfoc", dummy);
  int len            = 0; // Length for building the reply
  int cmdLen         = 0; // Command length
  int errorLen       = 0; // Length for building error message
  int mechanismError = 0; // Length for building error message
  int ierr           = 0;
  int YPos           = 0;
  int XPos           = 0;
  int FPos           = 0;

  len = strlen(dummy1);
  memset(dummy,0,sizeof(dummy));
  
  // Check position of AGw stage

  ierr = agwcu("localhost", 0, "agwy -c rdbits", dummy);
  GetArg(dummy,2,argbuf);
  if ( strcasecmp(dummy,"IO21=1") == 0 ) YPos = 1;
  ierr = agwcu("localhost", 0, "agwx -c rdbits", dummy);
  memset(argbuf, 0, sizeof(argbuf));
  GetArg(dummy,2,argbuf);
  if ( strcasecmp(dummy,"IO21=1") == 0 ) XPos = 1;
  memset(argbuf, 0, sizeof(argbuf));
  GetArg(dummy,2,argbuf);
  ierr = agwcu("localhost", 0, "agwfoc -c rdbits", dummy);
  if ( strcasecmp(dummy,"IO22=1") == 0 ) FPos = 1;

  YPos = YPos + XPos + FPos;

  if ( YPos < 0 ) {
    memset(dummy,0,sizeof(dummy));
    cmdLen = strlen("DONE: GPROBE "); // Add a space 
    KeyCommand("gprobe", dummy); // Stowed
    
    sprintf(statusMsg,"%s AGw guide probe retracted", who_selected, &dummy[cmdLen]);
    sprintf(&dummy1[len], "%s", &dummy[cmdLen]);
    mechanismError = 0;
    
  } else if ( shm_addr->MODS.host[agwYID] == 0 ) { // No IP Address given to hatch

    sprintf(statusMsg, "%s could not retract AGw, no IP Address for X, Y, or FOCUS, GPROBE=FAULT", who_selected);
    
    errorLen = strlen(errorMsg);
    sprintf(&errorMsg[errorLen], "could retract AGw no IP for AGw X,Y, or Focus, ");
    
    sprintf(&dummy1[len], "GPROBE=FAULT ");
    mechanismError = 1;
    
  } else if ( agwYID == -1 || agwXID == -1 || agwFID == -1 ) { // Mechanism not in the configuration list
    
    sprintf(statusMsg,  "%s could not retract AGw, not in config file, GPROBE=FAULT", who_selected);
    
    errorLen = strlen(errorMsg);
    sprintf(&errorMsg[errorLen], "could not retract AGw not found config file, ");
    sprintf(&dummy1[len], "GPROBE=FAULT ");
    mechanismError = 1;
    
  } else {
    memset(dummy,0,sizeof(dummy));

    cmdLen = strlen("DONE: GPROBE "); // Add a space 
    sprintf(statusMsg,"%s Retracting the AGw guide probe",who_selected);
    isisStatusMsg(statusMsg);
    
    KeyCommand("gprobe reset", dummy);
    
    if ( strncasecmp(dummy, "ERROR", 5) == 0) {
      
      cmdLen = strlen("ERROR: GPROBE "); // Add a space 
      
      sprintf(statusMsg,"%s could not retract AGw, %s", who_selected, &dummy[cmdLen]);
      errorLen = strlen(errorMsg);
      sprintf(&errorMsg[errorLen], "could not retract AGw, %s ",&dummy[cmdLen]);
      sprintf(&dummy1[len],"GPROBE=FAULT ");
      mechanismError = 1;
      
    } else {

      sprintf(statusMsg,"%s AGw guide probe retracted, %s", who_selected, &dummy[cmdLen]);
      
      mechanismError = 0;
      
    }
  }
  isisStatusMsg(statusMsg);
  return mechanismError;
  
}

