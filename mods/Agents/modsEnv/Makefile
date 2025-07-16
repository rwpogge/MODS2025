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
#   2024 Feb 18 - Migrating to open source libmodbus [rwp/osu]
#   2024 May 21 - Moving libmodbus to a standalone interface [xc/osu]
#   2024 Jul 08 - Adding LBTO's libtelemetry library [xc/osu]
#-------------------------------------------------------------

# Version Number: <major>.<minor>.<build>

VERSION     = v3.1.0

# Compiler and libary info as required

CC          = /usr/bin/g++
AR          = /usr/bin/ar
HOMEDIR     = /home/dts
ISISLIB     = $(HOMEDIR)/ISIS/lib
TELEMDIR	= /usr/include/lbto/lib-telemetry

# keep binaries local during development

BINDIR      = ./

INCLUDES    = -I$(ISISLIB) -I/usr/include/modbus -I$(HOMEDIR)/include -I$(TELEMDIR)
LIBRARIES   = -L$(ISISLIB) 
CFLAGS      = -w -c $(INCLUDES) 
VFLAGS      = -DAPP_VERSION='"$(VERSION)"' -DAPP_COMPDATE='"$(COMPDATE)"' \
              -DAPP_COMPTIME='"$(COMPTIME)"'
LIBS        = $(INCLUDES) $(LIBRARIES) -L/usr/X11R6/lib -lX11 \
              -lisis -lmodbus -lreadline -lhistory -lncurses -ltelcollection

LFLAGS      = -o modsenv

OBJS        = clientutils.o loadconfig.o commands.o modbusutils.o logutils.o

%.o : %.c client.h commands.h modbusutils.h
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
