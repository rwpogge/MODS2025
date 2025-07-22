/*
 * rmcrlf() - remove CRLF
 */
  
/*!
  \brief Remove CRLFs and replace them with blanks
  
  \param s  String to be returned
         s1 String to be operated on
  
  Helper utility to change CRLF to blanks in the return string
  
*/

/*
 * Remove CR's LF's 
 */
#include <stdio.h>
int rmcrlf(char s[], char s1[]) {
  int i,j;

  //  printf("s=%s\n",s);

  for(i=0,j=0;s[i]!='\0';i++) {
    if (s[i]=='\r' || s[i]=='\n' || s[i]=='>')
      s1[j++]=' ';
    else 
      s1[j++]=s[i];
  }
  /*
   * Blanks were produced by \r \n and > at the end of the return string
   * from the MicroLynx. This will remove the tail-end blanks.
   */
  j--;
  i--;
  for(;s1[j]==' ';--i,--j);
  s1[j+1]='\0';
  s[i+1]='\0';
  /* */
  //printf("[%d]s='%s'\n[%d]s1='%s'\n",i,s,j,s1);
}
