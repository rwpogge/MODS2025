#
# Makefile for libisis
#
# Build the package using the "build" script not with make.
#
# Note that we are using g++ below for development
#
# R. Pogge, OSU Astronomy Dept.
# pogge.1@osu.edu
# Last Modified 2024 Feb 2
#
# Versions:
#   v1.0 - 2003 Sep 15 - first deployed version [rwp/osu]
#   v1.1 - 2004 Mar 18 - minor revisions & bug fixes [rwp/osu]
#   v1.2 - 2004 Mar 23 - big changes after field testing [rwp/osu]
#   v2.0 - 2004 Jul 22 - overhauled isisclient code [rwp/osu]
#   v3.0 - 2024 Feb 2 - update for modern compilers/Alma 9.3 [rwp/osu]
#

VERSION     = libisis v3.0.1
CC          = /usr/bin/g++
AR          = /usr/bin/ar

CFLAGS      = -c 
VFLAGS      = -DISIS_VERSION='"$(VERSION)"' -DISIS_COMPDATE='"$(COMPDATE)"' \
              -DISIS_COMPTIME='"$(COMPTIME)"'

OBJS      = isismessage.o isisserial.o isissocket.o isisutils.o

.c.o:       isisclient.h
	    $(CC) $(CFLAGS) $(VFLAGS) $*.c

all:        libisis.a 

libisis.a:  $(OBJS) 
	    /bin/rm -f  libisis.a
	    $(AR) cr libisis.a $(OBJS)
	    ranlib libisis.a

clean:
	    /bin/rm -f libisis.a *.o
