/*!
  \file arg_util.c
  \brief  Argument parsing utilites

*/

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <sys/types.h>
#include "cmd_ds.h"

// parse command into data structure
int cmd_parse(char *buf, struct cmd_ds *command)
// buf - input command STRING
// command - cmd_ds struct
// parse string buffer into command ds
// return number of arguments if no error, -1 if argv overrun
{
  int argc;
  char *equal_ptr, *old_ptr, *ptr;

  //command may be followed by an '=' and parameters
  
  command->name=buf;

  equal_ptr=strchr(buf,'=');
  if(equal_ptr == NULL) { 
    command->equal='\0';
    command->argv[ 0]=NULL;
    return 0; // only the name so return
  }

  *equal_ptr='\0';
  command->equal= '=';

  // get the remaining arguments delimited by commas

  argc=0;
  old_ptr=equal_ptr+1;
  ptr=old_ptr;
  while(*ptr!=0) {
    if(*ptr == '\\') {
      char *lptr;
      for (lptr=ptr;*lptr!=0;lptr++)
	*lptr=*(lptr+1);
    } else if (*ptr==',') {
      if(argc<MAX_ARGS)
	command->argv[argc++]=old_ptr;
      *ptr=0;
      old_ptr=ptr+1;
    }
    ptr++;
  }
  if(strlen(old_ptr) > 0 && argc<MAX_ARGS) // last argument
    command->argv[argc++]=old_ptr;

  if(argc >= MAX_ARGS) {   // ungracefully handle too many argument
    command->argv[argc-1]=NULL;
    return -1;
  } else {
    command->argv[argc]=NULL;       // terminate list
    //    return 0;
    return argc;
  }
}

// traverse argv array
char *arg_next(struct cmd_ds *command,int *ilast)
// input: next argv to use
// output: incremented by 1 unless argv[*ilast]==NULL

/* basically this rouitine returns the next argv element until it
 * encounters a NULL, then it only returns NULLs                 
 * after setting-up cmd_ds intialize argc to 0, first call returns first
 * argv, second returns second, ..., until end of arguments (NULL)
 * *ilast is used for book-keeping by this routine
 */
{
  if(command->argv[*ilast]==NULL) return NULL;
  return command->argv[(*ilast)++];
}
/*!
  \brief arg_int - parse arg string for integer
 
  \parm ptr: pointer to string
  \parm iptr: pointer to store result
  \parm dflt: default result value
  \parm flag: TRUE if default is okay

  This routine handles QCMDS argument interpretation for int arguments
  "*" use current value (already stored in *iptr) on entry
  ""  (empty string) use default if flag is TRUE, if FALSE, error
  other strings are decoded as int
  return value: 0 no errror, -100 no default allowed and arg was ""
                             -200 wouldn't decode
 */

int arg_int(char *ptr, int *iptr, int dflt, int flag)
{
  int ierr,i;

  ierr=0;
  
  if(ptr == NULL || *ptr == '\0') {
    if (flag)
      *iptr=dflt;
    else
      ierr=-100;
    return ierr;
  }
  if(0==strcmp(ptr,"*")) return ierr;
  
  if(1 != sscanf(ptr,"%d",iptr)) ierr=-200;
  for(i=strlen(ptr)-1;i>-1;i--)
    if(NULL==strchr("+-0123456789 \t\n",ptr[i])) {
      ierr=-200;
      goto end;
    }
  
 end:
  return ierr;
}

/*!
  \brief arg_int - parse arg string for float
 
  \parm ptr: pointer to string
  \parm fptr: pointer to store result
  \parm dflt: default result value
  \parm flag: TRUE if default is okay

  This routine handles QCMDS argument interpretation for float args
  "*" use current value (already stored in *fptr) on entry
  ""  (empty string) use default if flag is TRUE, if FALSE, error
  other strings are decoded as float
  return value: 0 no errror, -100 no default allowed and arg was ""
                             -200 wouldn't decode
 */

int arg_float(char *ptr, float *fptr, float dflt, int flag)
{
  int ierr,i;
  
  ierr=0;
  
  if(ptr == NULL || *ptr == '\0') {
    if (flag)
      *fptr=dflt;
    else
      ierr=-100;
    return ierr;
  }
  if(0==strcmp(ptr,"*")) return ierr;
  
  if(1 != sscanf(ptr,"%f",fptr)) ierr=-200;
  for(i=strlen(ptr)-1;i>-1;i--)
    if(NULL==strchr("+-0123456789.ed \t\n",ptr[i])) {
      ierr=-200;
      goto end;
    }
  
 end:
  
  return ierr;
}

/*!
  \brief arg_dble - parse arg string for double
 
  \parm ptr: pointer to string
  \parm dptr: pointer to store result
  \parm dflt: default result value
  \parm flag: TRUE if default is okay

  This routine handles QCMDS argument interpretation for double args
  "*" use current value (already stored in *dptr) on entry
  ""  (empty string) use default if flag is TRUE, if FALSE, error
  other strings are decoded as float
  return value: 0 no errror, -100 no default allowed and arg was ""
                             -200 wouldn't decode
 */

int arg_dble(char *ptr, double *dptr,double dflt,int flag)
{
  int ierr,i;
  
  ierr=0;
  
  if(ptr == NULL || *ptr == '\0') {
    if (flag)
      *dptr=dflt;
    else
      ierr=-100;
    return ierr;
  }
  if(0==strcmp(ptr,"*")) return ierr;
  
  if(1 != sscanf(ptr,"%lf",dptr)) ierr=-200;
  for(i=strlen(ptr)-1;i>-1;i--)
    if(NULL==strchr("+-0123456789.ed \t\n",ptr[i])) {
      ierr=-200;
      goto end;
    }
  
 end:
  
  return ierr;
}

/*!
  \brief arg_key - parse arg string for keyword
 
  \parm ptr pointer to string
  \parm key array of pointers to keyword STRINGS
  \parm nkey number of keyword strings
  \parm iptr pointer to store result
  \parm dflt default result value
  \parm flag TRUE if default is okay

  This routine handles QCMDS argument interpretation for integer args
  "*" use current value (already stored in *iptr) on entry
  ""  (empty string) use default if flag is TRUE, if FALSE, error
  other strings are decoded as keywords
  return value: 0 no errror, -100 no default allowed and arg was ""
                             -200 wouldn't decode
  a "*" can be matched in a key list because the key list is checked
  before the "*" use current value check is made
 
*/

int arg_key(char *ptr,char **key, int nkey,int *iptr,int dflt,int flag)
{
  int ierr, icount;
  
  ierr=0;
  
  if(ptr == NULL || *ptr == '\0') {
    if (flag)
      *iptr=dflt;
    else
      ierr=-100;
    return ierr;
  }
  
  icount=0;
  while (icount < nkey)
    if(0==strcmp(ptr,key[icount++])) {
      *iptr=icount-1;
      return 0;
    }
  
  if(0==strcmp(ptr,"*")) return ierr;
  
  
  return -200;
}

/*! 
  \brief arg_key - parse arg string for key flt

  \parm ptr: pointer to string
  \parm key: array of pointers to keyword STRINGS
  \parm nkey: number of keyword strings
  \parm iptr: ptr to store result
  \parm dflt: default result value
  \parm flag: TRUE if default is okay  \parm ptr pointer to string

  This routine handles QCMDS argument interpretation for keyword args
  that are expressed and floats and we want all legitamite floats
  "*" use current value (already stored in *iptr) on entry
  ""  (empty string) use default if flag is TRUE, if FALSE, error
  other strings are compared as floats to keywords
  return value: 0 no errror, -100 no default allowed and arg was ""
                             -200 wouldn't decode                   
 
*/

int arg_key_flt(char *ptr, char **key, int nkey, int *iptr, int dflt, int flag)
{
  int ierr, icount,i;
  float fltarg, fltkey;

  ierr=0;
  
  if(ptr == NULL || *ptr == '\0') {
    if (flag)
      *iptr=dflt;
    else
      ierr=-100;
    return ierr;
  }
  
  if(0==strcmp(ptr,"*")) return ierr;
  
  sscanf(ptr,"%f",&fltarg);
  for(i=strlen(ptr)-1;i>-1;i--)
    if(NULL==strchr("+-0123456789.ed \t\n",ptr[i])) {
      goto end;
    }
  
  
  icount=0;
  while (icount < nkey) {
    sscanf(key[icount++],"%f",&fltkey);
    if(fabs( (double)(fltkey-fltarg))<=1e-5*fabs((double)fltarg)) {
      *iptr=icount-1;
      return 0;
    }
  }
 end:
  return -200;
}
