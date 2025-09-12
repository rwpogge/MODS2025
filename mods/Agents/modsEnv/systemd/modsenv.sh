#!/bin/sh
#
# SELinux enforcing mode means the executable needs to be in /usr/local/bin 
# copied as sudo
#
file=/usr/local/bin/modsenv
if [ -f ${file} ]
then
        ${file}
fi
