#include <stdio.h>
#include <string.h>
#include <sys/types.h>

#include "params.h"

void flt2str(char *,float ,int ,int );

void uns2str(char *output,unsigned uvalue,int width)
{
      int i;
      char local[11];     // big enough to hold largest possible
                          // formatted unsigned int (32 bits) plus '\0'

      output=output+strlen(output);

      sprintf(local,"%u",uvalue);
      if(strlen(local) > width) {
        for (i=0; i< width; i++) 
          output[i]='$';
        output[width]='\0';
      } else
        strcpy(output,local);
     
      return;
}

// floating point to string 
void flt2str(char *output,float fvalue,int width,int deci)
// output = output string to append to
// fvalue = value to convert
// width =  maximum field width, >0 left justify, <0 right justify
// fewer than width characters may be used for left just.
// deci = // digits after decimal point, >=0 blank fill for right
          // justify, <0 zero fill, 0 will print decimal point
// if output won't fit in specified width, that many characters are filled
// with dollar signs
// this function is intended to be a replacement for FORTRAN ir2as routine
{
  char form[30];   // must be big enough to hold largest possible format
  char string[256];// must be big enough to hold largest possible float

      char *ptr, zero[2], sign[2];
      int i,decpt, ndigit, wide, decd ,isnum;

      output=output+strlen(output);     // locate where to start filling

      wide=width; strcpy(sign,"-");     // handle justification
      if(wide<0) {
        wide=-width;
        strcpy(sign,"");
      }

      decd=deci; strcpy(zero,"");       // handle padding
      if(decd<0) {
        decd=-deci;
        strcpy(zero,"0");
      }

      if(deci==0) decd=1;               // make sure we get a decimal point

      sprintf(form,"%%%s%s%d.%df",sign,zero,wide,decd);  // make format
      sprintf(string,form,fvalue);                       // do it

      if(width>0) {                   // trim trailing spaces for left-just.
         ptr=strchr(string,' ');
         if(ptr!=NULL) *ptr='\0';
      }
      // remove post-decimal digit for deci==0

      if(deci==0 && strlen(string)>0) string[strlen(string)-1]='\0';

      isnum=TRUE;
      for(i=0;i<strlen(string);i++) {
	if(NULL==strchr(" +-0123456789.",string[i])) {
	  isnum=FALSE;
	  break;
	}
      }

      if(strlen(string)>wide || !isnum) {     // too wide or nan/inf, $ fill
        for (i=0; i< wide; i++) 
          output[i]='$';
        output[wide]='\0';
        return;
      } else
        strcat(output,string);      // okay, append result

       return;
}
// floating point to string
void dble2str(char *output,double fvalue,int width,int deci)
// output;                              /* output string to append to */
//fvalue;                              /* value to convert */
//width;        // maximum field width, >0 left justify, <0 right justify
//deci;         // digits after decimal point, >=0 blank fill for right
// justify, <0 zero fill, 0 will print decimal point
// if output won't fit in specified width, that many characters are filled
// with dollar signs
// this function is intended to be a replacement for FORTRAN ir2as routine
{
  char form[30];   // must be big enough to hold largest possible format
  char string[256];// must be big enough to hold largest possible float

      char *ptr, zero[2], sign[2];
      int i,decpt, ndigit, wide, decd, isnum;

      output=output+strlen(output);     // locate where to start filling

      wide=width; strcpy(sign,"-");     // handle justification
      if(wide<0) {
        wide=-width;
        strcpy(sign,"");
      }

      decd=deci; strcpy(zero,"");       // handle padding
      if(decd<0) {
        decd=-deci;
        strcpy(zero,"0");
      }

      if(deci==0) decd=1;               // make sure we get a decimal point

      sprintf(form,"%l%%s%s%d.%df",sign,zero,wide,decd);  // make format
      sprintf(string,form,fvalue);                       // do it

      if(width>0) {                   // trim trailing spaces for left-just.
         ptr=strchr(string,' ');
         if(ptr!=NULL) *ptr='\0';
      }
      // remove post-decimal digit for deci==0

      if(deci==0 && strlen(string)>0) string[strlen(string)-1]='\0';

      isnum=TRUE;
      for(i=0;i<strlen(string);i++) {
	if(NULL==strchr(" +-0123456789.",string[i])) {
	  isnum=FALSE;
	  break;
	}
      }

      if(strlen(string)>wide || !isnum) {     // too wide or nan/inf, $ fill
        for (i=0; i< wide; i++) 
          output[i]='$';
        output[wide]='\0';
        return;
      } else
        strcat(output,string);      // okay, append result

       return;
}

// integer point to string
void int2str(char *output,int ivalue,int width,int zorb)
// output;        output string to append to
// ivalue;        value to convert
// width;         maximum field width, >0 left justify, <0 right justify
//                fewer than width characters may be used for left just.
// zorb;          zeros or blanks, = 0 blank fill for non digit
//                  positions, !=0 zero fill
// if output won't fit in specified width, that many characters are filled
// with dollar signs */
// this function is intended to be a replacement for FORTRAN ib2as routine
{
  char form[30];   // must be big enough to hold largest possible format
  char string[256]; // must be big enough to hold largest possible integer

      char *ptr, zero[2], sign[2];
      int i,decpt, ndigit, wide, decd;

      output=output+strlen(output);     // locate where to start filling

      wide=width; strcpy(sign,"-");     // handle justification
      if(wide<0) {
        wide=-width;
        strcpy(sign,"");
      }

      decd=zorb; strcpy(zero,"");       // handle padding
      if(decd!=0) {
        decd=-zorb;
        strcpy(zero,"0");
      }

      if(zorb==0) decd=1;               // make sure we get a decimal point

      sprintf(form,"%%%s%s%dd",sign,zero,wide);  // make format
      sprintf(string,form,ivalue);                       // do it

      if(width>0) {                   // trim trailing spaces for left-just.
         ptr=strchr(string,' ');
         if(ptr!=NULL) *ptr='\0';
      }

      if(strlen(string)>wide) {     // too wide, $ fill
        for (i=0; i< wide; i++) 
          output[i]='$';
        output[wide]='\0';
        return;
      } else
        strcat(output,string);      // okay, append result

       return;
}
