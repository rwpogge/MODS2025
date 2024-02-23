#!/bin/csh
#
# build script for libisis - basically a wrapper for make that
# allows us to set date/time of creation tags as preprocessor
# flags.
#
setenv CDATE `date +'%Y-%b-%d'`
setenv CTIME `date +'%T'`
make clean
make "COMPDATE=$CDATE" "COMPTIME=$CTIME"
/bin/rm *.o

exit
