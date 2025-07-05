/* vueinfo.c -- view, use, and execute information

  \mainpage vueinfo - View Use Execute INFOrmation

  \author Staff, OSU Dept. of Astronomy rgonzale@astronomy.ohio-state.edu
 
  \date 2005 May 01
 
  \section Usage
  
  Usage: information and commands for ISL system and GUI(s)

  NOTE: This is a mess. It will have to be cleaned up some day.

*/
/************************************************************************
examples:
display_it(0, 4, "Y5", string);
display_it(1, 4, "R5", string);
display_it(2, 4, "C", STRING);
display_it(2, 4, "Y", STING);
display_it(2, 40, "", string);
display_it(2, 105, "Y", "BAD ");
display_it(2,105,"", "GOOD");
display_it(2, 4, "B", "VAC  TEMP     STOP  ????????");
************************************************************************/
/* Function declarations:
 *   Row - Screen row position
 *   Col - Screen column position
 *   Attrib - Modifiying attributes (blink, underline, red, blue...)
 *   InLine - The text to print
 */
#include <iostream>
using namespace std;

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#define MAX_LEN 256+1
void
display_it(int Row, int Col, char *Attrib, char *InLine)
{
  int i;

  /* Check for attribute modifiers */
  if (strstr(Attrib, "R")){
    (void)printf("%c[1;31m", 27);
  }
  else if (strstr(Attrib, "G"))
    (void)printf("%c[1;32m", 27);
  else if (strstr(Attrib, "Y"))
    (void)printf("%c[1;33m", 27);
  else if (strstr(Attrib, "B")) {
    (void)printf("%c[1;34m", 27);
  }
  else if (strstr(Attrib, "M"))
    (void)printf("%c[1;35m", 27);
  else if (strstr(Attrib, "C"))
    (void)printf("%c[1;36m", 27);
  else if (strstr(Attrib, "W"))
    (void)printf("%c[1;37m", 27);
  else if (strstr(Attrib, "X"))
    (void)printf("%c[1;30m", 27);
  
  if (strstr(Attrib, "7"))
    (void)printf("%c[7m", 27);
  if (strstr(Attrib, "5"))
    (void)printf("%c[5m", 27);
  if (strstr(Attrib, "4"))
    (void)printf("%c[4m", 27);
  if (strstr(Attrib, "1"))
    (void)printf("%c[1m", 27);
  if (strstr(Attrib, "0"))
    (void)printf("%c[0m", 27);
  
/* Position the cursor and print the text. If Row and Col are 0, just print */
  if (Row== 0 && Col== 0)
    (void)printf("%s", InLine);
  else
    (void)printf("%c[%d;%dH%s", 27, Row, Col, InLine);
  
/* Reset the video attributes. Don't send anything if we don't have to */
  if (*Attrib!= '\0')
    (void)printf("%c[0m", 27);
  return;  
}
