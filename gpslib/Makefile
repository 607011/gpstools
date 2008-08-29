# $Id$
# Copyright (c) 2008 Oliver Lau <ola@ct.heise.de>, Heise Zeitschriften Verlag.
# Alle Rechte vorbehalten.

PROJ = libgps.a
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
DOXYGEN = doxygen
X =

GPS_O  := $(foreach i,Ellipsoid GeoCoord GPSTrackFile GPXFile helper Line Point Stochastics SuuntoDatafile TCXFile Timestamp Track TrackLine Trackpoint Vector Waypoint WPL1000File,$i.o)
GPS_H  := $(foreach i,Ellipsoid GeoCoord GPSTrackFile GPXFile helper Line MercatorCoord Point Stochastics SuuntoDatafile TCXFile Timestamp Track TrackLine Trackpoint UTMCoord Value Vector Waypoint WPL1000File,$i.h)

all: $(PROJ)

clean:
	$(RM) $(GPS_O)
	$(RM) $(PROJ)

%.o: %.cpp $(GPS_H) Makefile
	$(CXX) -c $(CXXFLAGS) -o $@ $<

$(PROJ): $(GPS_O)
	$(AR) r $(PROJ) $(GPS_O)

doc: 
	$(DOXYGEN) Doxyfile
