#
# Makefile for lbttcs - the LBT MODS IIF interface and simulator
#
# lbttcs has two modes: connect to the ICE/IIF interface and
# talk to a live IIF, or a simulation mode that creates plausible 
# looking FITS header info for replies to a lab data-acquisition system
# like the IC.
#
# Use "build", not "make" to build the lbttcs executable.
#
# R. Pogge, OSU Astronomy Dept.
# pogge@astronomy.ohio-state.edu
# Original 2009 Apr 12
#
# Last Modification:
#   2011 Aug 12 - fixed HA calculation and M2CTEMP [rwp/osu]
#   2012 May 15 - added test for warning in offsets [rwp/osu]
#   2012 Sep 16 - updated exit() status codes.  See the tables
#                 in doc/lbttcs_ExitStatus.txt for details [rwp/osu]
#   2013 Sep 23 - recompile with changes to iifUtils library to
#                 fix a new requirement for no-value for the filter
#                 type and color type in SetStars() [rwp/osu]
#   2013 Nov 16 - recompile, change in OffsetPointing(): yet more
#                 undocumented changes in IIF behavior with recent
#                 TCS builds.  [rwp/osu]
#   2015 Aug 11 - version 3: binocular telescope control implemented
#                 starting with LBT IIF TCS Build 2015B [rwp/osu]
#-------------------------------------------------------------------------

# Version Number: <major>.<minor>.<build>

VERSION     = v3.0.1-bino

# Compiler and libary info as required
#
# NB: Binocular control needs IIFLIB = ../Utilities/iifUtils_v2
#
CC          = /usr/bin/g++
AR          = /usr/bin/ar
ISISLIB     = ../Utilities/isisClient
SKYLIB      = ../Utilities/skyUtils
IIFLIB	    = ../Utilities/iifUtils_v2
ICELIBS     = -lIce -lIceUtil
BINDIR      = /home/dts/bin

INCLUDES    = -I$(ISISLIB) -I$(SKYLIB) -I$(IIFLIB)
LIBRARIES   = -L$(ISISLIB) -L$(SKYLIB) -L$(IIFLIB)
CFLAGS      = -w -c $(INCLUDES) 
VFLAGS      = -DAPP_VERSION='"$(VERSION)"' -DAPP_COMPDATE='"$(COMPDATE)"' \
              -DAPP_COMPTIME='"$(COMPTIME)"'
LIBS        = $(INCLUDES) $(LIBRARIES) \
              -lisis -lsky -liif $(ICELIBS) -lreadline -lhistory -lncurses
LFLAGS      = -o lbttcs

OBJS        = clientutils.o loadconfig.o commands.o 

.c.o:       client.h commands.h
	    $(CC) $(CFLAGS) $(VFLAGS) $*.c

all:        lbttcs

lbttcs:     $(OBJS) main.c
	    $(CC) $(LFLAGS) $(VFLAGS) main.c $(OBJS) $(LIBS)

clean:
	    /bin/rm -f lbttcs *.o

install:
	    \cp lbttcs $(BINDIR)
	    /bin/rm -f *.o
	    doxygen >& /dev/null
