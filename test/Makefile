# $Id$
# Copyright (c) 2008 Oliver Lau <oliver@ersatzworld.net>
# Alle Rechte vorbehalten.

PROJ = test
INC = -I..
OPTIMIZE = -O2 -finline-functions
DEBUGFLAGS = -DDEBUG -D_DEBUG -ggdb -g3
CFLAGS = $(INC) -Wall
CXXFLAGS = $(INC) -Wall
LDFLAGS =
CC = gcc
CXX = g++
LD = ld
AR = ar
X =

PROJ_O  := $(foreach i,test,$i.o)
LIBS := $(foreach i,../gpslib/libgps ../tinyxml/libtinyxml,$i.a)

debug:
	$(MAKE) all "CXXFLAGS=$(CXXFLAGS) $(DEBUGFLAGS)"

release:
	$(MAKE) all "CXXFLAGS=$(CXXFLAGS) $(OPTIMIZE)"

all: $(PROJ)

clean:
	$(RM) $(PROJ_O)
	$(RM) $(PROJ)
	$(RM) hh-?d-*.gpx

%.o: %.cpp %.h Makefile
	$(CXX) -c $(CXXFLAGS) $(INC) -o $@ $<

gpslib:
	$(MAKE) -C ../gpslib

tinyxml:
	$(MAKE) -C ../tinyxml

$(PROJ): $(PROJ_O) $(LIBS) Makefile
	$(CXX) -o $(PROJ)$(X) $(LDFLAGS) $(PROJ_O) $(LIBS)
