/*
 * StrUpper() - convert all characters in a string to uppercase
 */
  
/*!
  \brief Convert all characters in a string to uppercase
  
  \param str String to be converted to uppercase. Changed by this function.
  
  Helper utility to convert all characters in a string to uppercase.
  All commands are required to be in uppercase.
  
*/
#include <stdio.h>
#include <ctype.h>
#include <string.h>

void
StrToUpper(char *str)
{
  int i;
  for(i=0;i<strlen(str);i++) {
    if(str[i] >= 'a' && str[i] <= 'z')
      str[i] = toupper(str[i]);
  }
}
