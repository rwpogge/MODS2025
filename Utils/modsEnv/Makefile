#
# Makefile for modsenv - MODS environmental monitor agent
#
# Use "build", not "make" to build the modsenv executable.
#
# R. Pogge, OSU Astronomy Dept.
# pogge@astronomy.ohio-state.edu
# Original 2010 June 21
#
# Modification History:
#   2013 Aug 15 - Start of v2.x development [rwp/osu]
#   2014 Jul 10 - Added shared memory utilization [rwp/osu]
#
#-------------------------------------------------------------

# Version Number: <major>.<minor>.<build>

VERSION     = v2.2.0

# Compiler and libary info as required

CC          = /usr/bin/g++
AR          = /usr/bin/ar
HOMEDIR     = /home2/mods
ISISLIB     = $(HOMEDIR)/modsUI/Utilities/isisClient
WAGOLIB	    = $(HOMEDIR)/modsUI/Utilities/wagoUtils

# keep binaries local during development

BINDIR      = ./

INCLUDES    = -I$(ISISLIB) -I$(WAGOLIB)/include -I$(HOMEDIR)/include
LIBRARIES   = -L$(ISISLIB) -L$(WAGOLIB)/lib -L$(HOMEDIR)/ulib
CFLAGS      = -w -c $(INCLUDES) 
VFLAGS      = -DAPP_VERSION='"$(VERSION)"' -DAPP_COMPDATE='"$(COMPDATE)"' \
              -DAPP_COMPTIME='"$(COMPTIME)"'
LIBS        = $(INCLUDES) $(LIBRARIES) -L/usr/X11R6/lib -lX11 \
              -lisis -lmbusmaster -lreadline -lhistory -lncurses \
              -lislutils

LFLAGS      = -o modsenv

OBJS        = clientutils.o loadconfig.o commands.o 

.c.o:       client.h commands.h
	    $(CC) $(CFLAGS) $(VFLAGS) $*.c

all:        modsenv

#
# Interactive modsenv agent
#
modsenv:    $(OBJS) main.c
	    $(CC) $(LFLAGS) $(VFLAGS) main.c $(OBJS) $(LIBS)


clean:
	    /bin/rm -f modsenv *.o

install:
	    \cp modsenv $(BINDIR)
	    /bin/rm -f *.o
	    doxygen >& /dev/null
