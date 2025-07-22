/* 
 * Function finds the correct help file to list with the help command.
 * The first extension character matches the mods(x). An underscore 
 * for an extension character will be for any type of equipment if any.
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include "params.h"

#define MAX_STRING  256

void what_help(char *cnam,
	       int clength,
	       char *runstr,
	       int ierr, 
	       int clen, 
	       int rlen)
{
  char string[MAX_STRING+1],*s1;
  char *decloc, *declocstr;
  int inum;
  FILE *idum;
  int freq;
  //,system();
  char outbuf[80];
  char equip1, equip2, ch1, ch2, ch3;

  ierr=0;
  if (clength > MAX_STRING) {
    ierr=-2;
    return;
  }

  s1=strncpy(string,cnam,clength);
  string[clength]='\0';

  declocstr = strchr(string,'.');
  if(declocstr==NULL)
    strcat(string,".*");

  strcpy(outbuf,"ls ");
  strcat(outbuf,ISL_ROOT);
  strcat(outbuf,"/mods1/help/");
  strcat(outbuf,string);

  strcat(outbuf," >> /tmp/LS.NUM 2> /dev/null");
  freq = system(outbuf);

  idum=fopen("/tmp/LS.NUM","r");
  unlink("/tmp/LS.NUM");
  ierr = -3;
  while(-1!=fscanf(idum,"%s",outbuf)){
    decloc = strrchr(outbuf,'.');
    if(declocstr !=NULL) {
      strcpy(runstr,outbuf);
      ierr = 0;
      break;
    } else if(decloc != NULL) {
      ch1=*(decloc+1);
      ch2=*(decloc+2);
      ch3=*(decloc+3);
      if(ch1== '_' ||
         ch2 == '_' ||
	 ch3 == '_') //|| ch3 == '+') ||
	  //	  (ch2 == '+' || ch3 == '+') ||
	  //	  (ch2 =='_' && ch3 == '_'))) ||
	  //	 (&& (ch2 != '+' && ch3 != '+')))
	{
	  strcpy(runstr,outbuf);
	  ierr = 0;
	  break;
	}  else if(ch1== 'g' ||
         ch2 == 'u' ||
	 ch3 == 'i') //|| ch3 == '+') ||
	  //	  (ch2 == '+' || ch3 == '+') ||
	  //	  (ch2 =='_' && ch3 == '_'))) ||
	  //	 (&& (ch2 != '+' && ch3 != '+')))
	{
	  strcpy(runstr,outbuf);
	  ierr = 0;
	  break;
	}
    }
  }
  fclose(idum);
}
