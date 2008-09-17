# $Id$
# Copyright (c) 2008 Oliver Lau <oliver@ersatzworld.net>
# Alle Rechte vorbehalten.

PROJ=libgps.a
INC=-I..
OPTIMIZE=-O2 -finline-functions
DEBUGFLAGS=-DDEBUG -D_DEBUG -ggdb -g3
CFLAGS=$(OPTIMIZE) $(INC) -Wall
CXXFLAGS=$(OPTIMIZE) $(INC) -w -Wall -fPIC
LDFLAGS=-lstdc++ -fexceptions
CC=gcc
CXX=g++
LD=ld
AR=ar
DOXYGEN=doxygen
OUTDIR=.
O=.o
X=
MKDIR=mkdir -p
MV=mv

GPS_O := $(foreach i,Ellipsoid GeoCoord GPSTrackFile GPXFile helper Line Point Polygon Stochastics SuuntoDatafile TCXFile Timestamp Track TrackLine Trackpoint Vector Waypoint WPL1000File,$i.o)
GPS_H := $(foreach i,Ellipsoid GeoCoord GPSTrackFile GPXFile helper Line MercatorCoord Point Polygon Stochastics SuuntoDatafile TCXFile Timestamp Track TrackLine Trackpoint UTMCoord Value Vector Waypoint WPL1000File,$i.h)


all: $(PROJ)

macosx-release-universal:
	$(MAKE) clean
	$(MAKE) macosx-release-x86
	$(MKDIR) x86
	$(RM) i386/*
	$(MV) $(PROJ) x86
	$(MAKE) clean
	$(MAKE) macosx-release-ppc
	$(MKDIR) ppc
	$(RM) ppc/*
	$(MV) $(PROJ) ppc
	lipo -create ppc/$(PROJ) x86/$(PROJ) -output libgps.a

macosx-debug-universal:
	$(MAKE) clean
	$(MAKE) macosx-debug-x86
	$(MKDIR) x86
	$(RM) i386/*
	$(MV) $(PROJ) x86
	$(MAKE) clean
	$(MAKE) macosx-debug-ppc
	$(MKDIR) ppc
	$(RM) ppc/*
	$(MV) $(PROJ) ppc
	lipo -create ppc/$(PROJ) x86/$(PROJ) -output libgps.a

macosx-release-x86:
	$(MAKE) release O="$(O)" \
	        CFLAGS="$(CFLAGS)" \
        	CXXFLAGS="$(CXXFLAGS) -arch i386" \
        	LDFLAGS="$(LDFLAGS)"

macosx-release-ppc:
	$(MAKE) release O="$(O)" \
		CFLAGS="$(CFLAGS)" \
		CXXFLAGS="$(CXXFLAGS) -arch ppc" \
		LDFLAGS="$(LDFLAGS)"

macosx-debug-x86:
	$(MAKE) debug O="$(O)" \
	        CFLAGS="$(CFLAGS)" \
        	CXXFLAGS="$(CXXFLAGS) -arch i386" \
        	LDFLAGS="$(LDFLAGS)"

macosx-debug-ppc:
	$(MAKE) debug O="$(O)" \
		CFLAGS="$(CFLAGS)" \
		CXXFLAGS="$(CXXFLAGS) -arch ppc" \
		LDFLAGS="$(LFLAGS)"

release:
	$(RM) $(PROJ)
	$(MAKE) all O="$(O)" \
		CFLAGS="$(CFLAGS)" \
		CXXFLAGS="$(CXXFLAGS)" \
		LDFLAGS="$(LDFLAGS)" \
	$(MAKE) doc

debug:
	$(RM) $(PROJ)
	$(MAKE) all O="$(O)" \
		CFLAGS="$(CFLAGS) $(DEBUGFLAGS)" \
		CXXFLAGS="$(CXXFLAGS) $(DEBUGFLAGS)" \
		LDFLAGS="$(LDFLAGS)"

clean:
	$(RM) $(GPS_O)
	$(RM) $(PROJ)

%.o: %.cpp $(GPS_H) Makefile
	$(CXX) -c $(CXXFLAGS) -o $@ $<

$(PROJ): $(GPS_O) Makefile
	$(AR) r $(PROJ) $(GPS_O)

doc: 
	$(DOXYGEN) Doxyfile
