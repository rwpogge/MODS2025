#
# Makefile for modsHEB
#
# Use "build", not "make" to build the modsheb executable.
#
# This Makefile compiles all *.c files in SRCDIR into object files in BUILDDIR.
# Then it combines all the object files into an executable.
#
# X. Carroll
#
# R. Pogge, OSU Astronomy Dept.
# pogge@astronomy.ohio-state.edu
#
# Date: 2024 August 16
#-------------------------------------------------------------

# Version Number: <major>.<minor>.<build>

VERSION     = v0.0.1

# Compiler and libary info as required

CC          = /usr/bin/g++
HOMEDIR     = /home/dts
ISISLIB     = $(HOMEDIR)/ISIS/lib
TELEMDIR	= /usr/include/lbto/lib-telemetry

# Flags, includes, etc

TARGET 		= modsheb

SRCDIR 		= .
BUILDDIR	= ./bin

INCLUDES    = -I$(ISISLIB) -I/usr/include/modbus -I$(HOMEDIR)/include -I$(TELEMDIR)
LIBRARIES   = -L$(ISISLIB) 
CFLAGS      = -w -c $(INCLUDES) 
VFLAGS      = -DAPP_VERSION='"$(VERSION)"' -DAPP_COMPDATE='"$(COMPDATE)"' \
              -DAPP_COMPTIME='"$(COMPTIME)"'
LIBS        = $(INCLUDES) $(LIBRARIES) -L/usr/X11R6/lib -lX11 \
              -lisis -lmodbus -lreadline -lhistory -lncurses -ltelcollection

HEADERS		= client.h commands.h modbusutils.h

# A list of all the .c files in the SRCDIR directory, and an altered list of those files with a .o extension in the BUILDDIR directory.

C_FILES=$(wildcard $(SRCDIR)/*.c)
OBJECTS=$(patsubst $(SRCDIR)/%.c, $(BUILDDIR)/%.o, $(C_FILES) )

# The build instructions

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) -o $(TARGET) $(VFLAGS) $(OBJECTS) $(LIBS) -o $(TARGET)

$(BUILDDIR)/%.o: $(SRCDIR)/%.c $(HEADERS)
	mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) $(VFLAGS) -c $< -o $@

clean:
	/bin/rm -rf $(BUILDDIR) $(TARGET)
