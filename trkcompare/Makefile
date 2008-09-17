# $Id$
# Copyright (c) 2008 Oliver Lau <ola@ct.heise.de>, Heise Zeitschriften Verlag.
# Alle Rechte vorbehalten.

PROJ = trkcompare
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

PROJ_O  := $(foreach i,trkcompare,$i.o)
LIBS := $(foreach i,../gpslib/libgps ../tinyxml/libtinyxml,$i.a)

all: $(PROJ)

clean:
	$(RM) $(PROJ_O)
	$(RM) $(PROJ)

%.o: %.cpp %.h Makefile
	$(CXX) -c $(CXXFLAGS) $(INC) -o $@ $<

gpslib:
	$(MAKE) -C ../gpslib

tinyxml:
	$(MAKE) -C ../tinyxml

$(PROJ): $(PROJ_O) $(LIBS) Makefile
	$(CXX) -o $(PROJ)$(X) $(LDFLAGS) $(PROJ_O) $(LIBS)
