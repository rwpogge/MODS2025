#
# Makefile for modsenv - MODS environmental monitor agent
#
# Use "build", not "make" to build the modsenv executable.
#
# R. Pogge, OSU Astronomy Dept.
# pogge.1@osu.edu
# Original 2010 June 21
#
# Modification History:
#   2013 Aug 15 - Start of v2.x development [rwp/osu]
#   2014 Jul 10 - Added shared memory utilization [rwp/osu]
#   2024 Feb 18 - Migrating to open source libmodbus [rwp/osu]
#   2024 May 21 - Moving libmodbus to a standalone interface [xc/osu]
#   2024 Jul 08 - Adding LBTO's libtelemetry library [xc/osu]
#   2025 Aug 17 - Added Archon HEB data, shmem handling [rwp/osu]
#   2025 Aug 25 - Added ionization gauge interface [xc/osu]
#   2025 Oct 04 - Logic for powerState=(switch&&breaker) for IUB [rwp/osu]
#   2025 Oct 16 - bug in HEB-B power switch state logic fixed [rwp/osu]
#
#---------------------------------------------------------------------------

ROOTDIR     = /home/dts/mods
ISISDIR     = /home/dts/ISIS
TELEMDIR    = /usr/include/lbto/lib-telemetry
VERSION     = v3.3.5
CC          = /usr/bin/g++
AR          = /usr/bin/ar

# keep binaries local during development

BINDIR      = $(HOMEDIR)/mods/bin/

INCS        = -I$(ISISLIB)/lib -I$(ROOTDIR)/include -I/usr/include/modbus -I$(TELEMDIR)

CFLAGS      = -w -c $(INCS) 

VFLAGS      = -DAPP_VERSION='"$(VERSION)"' -DAPP_COMPDATE='"$(COMPDATE)"' \
              -DAPP_COMPTIME='"$(COMPTIME)"'

LIBS        = -L$(ISISDIR)/lib -L$(ROOTDIR)/ulib -lislutils -linstrutils \
              -lisis -lmodbus -lreadline -lhistory -lncurses -ltelcollection

LFLAGS      = -o modsenv

OBJS        = clientutils.o loadconfig.o commands.o modbusutils.o logutils.o ionutils.o

%.o : %.c client.h commands.h modbusutils.h ionutils.h
	    $(CC) $(CFLAGS) $(VFLAGS) $*.c

all:        modsenv

#
# Interactive modsenv agent
#
modsenv:    $(OBJS) main.c
	    $(CC) $(LFLAGS) $(VFLAGS) main.c $(OBJS) $(LIBS) $(INCS)

clean:
	    /bin/rm -f modsenv *.o

install:
	    \cp modsenv $(BINDIR)
	    /bin/rm -f *.o
	    doxygen >& /dev/null
