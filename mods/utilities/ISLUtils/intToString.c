/*!
  \file intToString.c
  \brief intToString - ISL logging routine
 
  \author Staff, OSU Dept. of Astronomy (rgonzale@astronomy.ohio-state.edu)
  \date 2005 March 20
 
  \par Usage:
  intToString(int);
 
  intToString returns the string after conversion from integer.

  <pre>
  2005 March 20 - new application [rdg]
  </pre>
*/
#include <stdio.h>
#include <string.h>

void
intToString(int n, char s[])
{
  int i, j, c;
  int sign;
  /* this has to be done in reverse order */
  if(((sign=n) < 0)) n = -n;
  i = 0;
  do 
    {
    s[i++] = n % 10 + '0';
  }
  while ((n /= 10) > 0);
  if(sign<0)  s[i++]='-';
  s[i] ='\0';
  
  /* correct for reverse order */
  for(i=0, j=strlen(s)-1; i<j; i++, j--)
    { 
      c=s[i];
      s[i]=s[j];
      s[j]=c;
    }
}
