# $Id$
# Copyright (c) 2008 Oliver Lau <oliver@ersatzworld.net>
# Alle Rechte vorbehalten.

PROJ=libgps.a
INC=-I..
OPTIMIZE=-O2 -finline-functions
DEBUGFLAGS=-DDEBUG -D_DEBUG -ggdb -g3
CFLAGS=$(OPTIMIZE) $(INC) -Wall
CXXFLAGS=$(OPTIMIZE) $(INC) -w -Wall -fPIC
LDFLAGS=$(CXXFLAGS) -lstdc++ -fexceptions
CC=gcc
CXX=g++
LD=ld
AR=ar
MKDIR=mkdir -p
MV=mv
LIPO=lipo
DOXYGEN=doxygen
O=.o
X=

GPS_O := $(foreach i,Ellipsoid GeoCoord GPSTrackFile GPXFile helper Line Point Polygon portable Stochastics SuuntoDatafile TCXFile Timestamp Track TrackLine Trackpoint Vector Waypoint WPL1000File,$i$(O))
GPS_H := $(foreach i,Ellipsoid GeoCoord GPSTrackFile GPXFile helper Line MercatorCoord Point Polygon portable Stochastics SuuntoDatafile TCXFile Timestamp Track TrackLine Trackpoint UTMCoord Value Vector Waypoint WPL1000File,$i.h)


all: $(PROJ)

macosx-release-universal:
	$(RM) -R x86
	$(RM) -R ppc
	$(MKDIR) x86
	$(MKDIR) ppc
	@echo "**************************************"
	@echo "************ X86 RELEASE *************"
	@echo "**************************************"
	$(MAKE) clean
	$(MAKE) macosx-release-x86
	$(MV) $(PROJ) x86
	@echo "**************************************"
	@echo "************ PPC RELEASE *************"
	@echo "**************************************"
	$(MAKE) clean
	$(MAKE) macosx-release-ppc
	$(MV) $(PROJ) ppc
	@echo "**************************************"
	@echo "********** UNIVERSAL BINARY **********"
	@echo "**************************************"
	$(LIPO) -create ppc/$(PROJ) x86/$(PROJ) -output $(PROJ)

macosx-release-x86:
	$(MAKE) release \
	    O="$(O)" \
            CFLAGS="$(CFLAGS) -arch i386" \
	    CXXFLAGS="$(CXXFLAGS) -arch i386" \
	    LDFLAGS="$(LDFLAGS) -arch i386"

macosx-release-ppc:
	$(MAKE) release \
	    O="$(O)" \
	    CFLAGS="$(CFLAGS) -arch ppc -faltivec" \
	    CXXFLAGS="$(CXXFLAGS) -arch ppc -faltivec" \
	    LDFLAGS="$(LDFLAGS) -arch ppc -faltivec"

release:
	$(RM) $(PROJ)
	$(MAKE) all \
	  O="$(O)" \
	  CFLAGS="$(CFLAGS)" \
	  CXXFLAGS="$(CXXFLAGS)" \
	  LDFLAGS="$(LDFLAGS)"

debug:
	$(RM) $(PROJ)
	$(MAKE) all \
	  O="$(O)" \
	  CFLAGS="$(CFLAGS) $(DEBUGFLAGS)" \
	  CXXFLAGS="$(CXXFLAGS) $(DEBUGFLAGS)" \
	  LDFLAGS="$(LDFLAGS) $(DEBUGFLAGS)"

clean:
	$(RM) $(GPS_O)
	$(RM) $(PROJ)

distclean: clean
	$(RM) -R ppc
	$(RM) -R x86

%.o: %.cpp $(GPS_H) Makefile
	$(CXX) -c $(CXXFLAGS) -o $@ $<

$(PROJ): $(GPS_O) Makefile
	$(AR) r $(PROJ) $(GPS_O)

doc: 
	$(DOXYGEN) Doxyfile
