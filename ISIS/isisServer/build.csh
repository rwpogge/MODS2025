#!/bin/csh
#

setenv CDATE `date +'%Y-%b-%d'`
setenv CTIME `date +'%T'`
setenv ISCFG /home/dts/Config/isis.ini
setenv ISDCFG /home/dts/Config/isisd.ini
setenv ISLOG /home/data/Logs/ISIS/isis

make -f Makefile.build clean
make -f Makefile.build "COMPDATE=$CDATE" "COMPTIME=$CTIME" "CONFIG=$ISCFG" "LOGS=$ISLOG" "DCONFIG=$ISDCFG"
/bin/rm *.o
exit
