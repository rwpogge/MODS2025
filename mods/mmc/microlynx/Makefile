# ISL MicroLYNX Client
# all purpose microLynx client that works with the mmcServer
#
# 2025 Oct 3 - deprecated [rwp/osu]
#
ROOTDIR = /home/dts/mods
ISISDIR = /home/dts/ISIS

#ISLLIB = /home/dts/mods/ulib

#ifeq ($(shell uname),SunOS)
#endif
#LIBS = -L$(ISLLIB) -lislutils -lsocket -lnsl	
#LIBS = -L$(ISLLIB) -lnsl	
LIBS = -L$(ROOTDIR)/ulib -L$(ISISDIR)/lib
INCS = -I$(ROOTDIR)/include -I$(ISISDIR)/lib

CXX = g++
CXXFLAGS = -Wall -D_REENTRANT -pthread -ansi -pedantic -g

#all: islmlynx islmlynxShm isisMlcShm install
all: islmlynx islmlynxShm install

islmlynx: islmlynx.cpp ISLSocket.cpp
	$(CXX) $(CXXFLAGS) -o islmlynx islmlynx.cpp ISLSocket.cpp $(LIBS) $(INCS)

islmlynxShm: islmlynxShm.cpp ISLSocket.cpp
	$(CXX) $(CXXFLAGS) -o islmlynxShm islmlynxShm.cpp ISLSocket.cpp $(LIBS) $(INCS)

#isisMlcShm: isisMlcShm.c ISLSocket.cpp ISLSocket.h
#	$(CXX) $(CXXFLAGS) -o isisMlcShm isisMlcShm.c ISLSocket.cpp $(LIBS) $(INCS)

clean:
	\rm islmlynx
	\rm islmlynxShm

install:
	\cp islmlynx $(ROOTDIR)/bin/
	\cp islmlynxShm $(ROOTDIR)/bin/

