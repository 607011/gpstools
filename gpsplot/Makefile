# $Id$
# Copyright (c) 2008 Oliver Lau <ola@ct.heise.de>, Heise Zeitschriften Verlag.
# Alle Rechte vorbehalten.

PROJ = gpsplot
INC = -I..
OPTIMIZE = -O2 -finline-functions
DEBUGFLAGS = -DDEBUG -D_DEBUG -ggdb -g3
CFLAGS = $(OPTIMIZE) $(INC) $(DEBUGFLAGS) -Wall
CXXFLAGS = $(OPTIMIZE) $(INC) $(DEBUGFLAGS) -Wall
LDFLAGS =
CC = gcc
CXX = g++
LD = ld
AR = ar
X =

PROJ_O  := $(foreach i,gpsplot globals,$i.o)
PROJ_H  := $(foreach i,SmoothingOptions globals,$i.h)
LIBS := $(foreach i,../gpslib/libgps ../tinyxml/libtinyxml,$i.a)

all: $(PROJ)

release: all

clean:
	$(RM) $(PROJ_O)
	$(RM) $(PROJ)

distclean: clean
	$(RM) *.dat
	$(RM) *.plt
	$(RM) *.js
	$(RM) *.kml

%.o: %.cpp %.h Makefile $(PROJ_H)
	$(CXX) -c $(CXXFLAGS) $(INC) -o $@ $<

../gpslib/libgps.a:
	$(MAKE) -C ../gpslib

../tinyxml/libtinyxml.a:
	$(MAKE) -C ../tinyxml


$(PROJ): $(PROJ_O) $(LIBS) Makefile ../gpslib ../tinyxml
	$(CXX) -o $(PROJ)$(X) $(LDFLAGS) $(PROJ_O) $(LIBS)
