# $Id$
# Copyright (c) 2008 Oliver Lau <ola@ct.heise.de>, Heise Zeitschriften Verlag.
# Alle Rechte vorbehalten.

PROJ=wpl1000reader
INC=-I..
OPTIMIZE=-O2 -finline-functions
DEBUGFLAGS = -DDEBUG -D_DEBUG -ggdb -g3
CFLAGS=$(OPTIMIZE) $(INC) -Wall
CXXFLAGS=$(OPTIMIZE) $(INC) -Wall
LDFLAGS=
CC=gcc
CXX=g++
LD=ld
AR=ar
X=
O=.o

PROJ_O  := $(foreach i,wpl1000reader,$i$(O))
LIBS := $(foreach i,../gpslib/libgps ../tinyxml/libtinyxml,$i.a)

release: all

all: $(PROJ)

clean:
	$(RM) $(PROJ_O)
	$(RM) $(PROJ)

%$(O): %.cpp %.h Makefile
	$(CXX) -c $(CXXFLAGS) $(INC) -o $@ $<

gpslib:
	$(MAKE) -C ../gpslib

tinyxml:
	$(MAKE) -C ../tinyxml

$(PROJ): $(PROJ_O) $(LIBS) Makefile
	$(CXX) -o $(PROJ)$(X) $(LDFLAGS) $(PROJ_O) $(LIBS)

test: $(PROJ)
	./$(PROJ) Testdaten/NVPIPE-zemp.DAT Testdaten/zemp.gpx
	./$(PROJ) --multi Testdaten/NVPIPE-zemp.DAT Testdaten/zemp.gpx
