# $Id$
# Copyright (c) 2008 Oliver Lau <oliver@ersatzworld.net>
# Alle Rechte vorbehalten.

PROJ=gpxtimeshift
INC=-I..
OPTIMIZE=-O2 -finline-functions
DEBUGFLAGS=-DDEBUG -D_DEBUG -ggdb -g3
CFLAGS=$(OPTIMIZE) $(INC) -Wall
CXXFLAGS=$(OPTIMIZE) $(INC) -Wall -fPIC
LDFLAGS=$(CXXFLAGS) -lstdc++ -fexceptions
CC=gcc
CXX=g++
LD=ld
AR=ar
X=
O=.o
MKDIR=mkdir -p
MV=mv
OUTDIR=.
STRIP=strip
LIPO=lipo

PROJ_O := $(foreach i,gpxtimeshift,$i$(O))
LIBS := $(foreach i,../gpslib/$(OUTDIR)/libgps ../tinyxml/$(OUTDIR)/libtinyxml,$i.a)

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
	@echo "**************************************"
	@echo "************ PPC RELEASE *************"
	@echo "**************************************"
	$(MAKE) clean
	$(MAKE) macosx-release-ppc
	@echo "**************************************"
	@echo "********** UNIVERSAL BINARY **********"
	@echo "**************************************"
	$(LIPO) -create ppc/$(PROJ) x86/$(PROJ) -output $(PROJ)
	$(STRIP) $(PROJ)

macosx-release-x86:
	$(MAKE) release \
	    O="$(O)" \
            CFLAGS="$(CFLAGS)" \
	    CXXFLAGS="$(CXXFLAGS) -arch i386" \
	    LDFLAGS="$(LDFLAGS) -arch i386" \
	    OUTDIR="x86"

macosx-release-ppc:
	$(MAKE) release \
	    O="$(O)" \
	    CFLAGS="$(CFLAGS)" \
	    CXXFLAGS="$(CXXFLAGS) -arch ppc" \
	    LDFLAGS="$(LDFLAGS) -arch ppc" \
	    OUTDIR="ppc"

release:
	$(MAKE) all \
	    O="$(O)" \
	    CFLAGS="$(CFLAGS)" \
            CXXFLAGS="$(CXXFLAGS)" \
            LDFLAGS="$(LDFLAGS)" \
	    OUTDIR="$(OUTDIR)" 

debug:
	$(MAKE) all \
	    O="$(O)" \
	    CFLAGS="$(CFLAGS) $(DEBUGFLAGS)" \
            CXXFLAGS="$(CXXFLAGS) $(DEBUGFLAGS)" \
            LDFLAGS="$(LDFLAGS) $(DEBUGFLAGS)"
	    OUTDIR="$(OUTDIR)" 

clean:
	$(RM) $(PROJ_O)
	$(RM) $(PROJ)

%.o: %.cpp %.h Makefile
	$(CXX) -c $(CXXFLAGS) $(INC) -o $@ $<

../gpslib/libgps.a:
	$(MAKE) -C ../gpslib

../tinyxml/libtinyxml.a:
	$(MAKE) -C ../tinyxml

$(PROJ): $(PROJ_O) $(LIBS) Makefile
	$(CXX) $(LDFLAGS) -o $(OUTDIR)/$(PROJ)$(X) $(PROJ_O) $(LIBS)


test:
	@echo $(PROJ) --to 1995-12-24T18:35:51Z
	@./$(PROJ) t/20081004-Altenau-und-umzu.gpx t/to.gpx --to 1995-12-24T18:35:51Z -q
	@echo -n "2xOK? -->"
	@if [ "1" == `grep -c 1995-12-24T18:35:51Z t/to.gpx` ]; then echo -n " OK"; fi
	@if [ "1" == `grep -c 1995-12-25T00:42:50Z t/to.gpx` ]; then echo " OK"; fi

	@echo $(PROJ) --minus 07:47:24
	@echo -n "2xOK? -->"
	@./$(PROJ) t/20081004-Altenau-und-umzu.gpx t/minus.gpx --minus 07:47:24 -q
	@if [ "1" == `grep -c 2008-10-03T23:59:59Z t/minus.gpx` ]; then echo -n " OK"; fi
	@if [ "1" == `grep -c 2008-10-04T06:06:58Z t/minus.gpx` ]; then echo " OK"; fi

	@echo $(PROJ) --plus 03:47:24
	@echo -n "2xOK? -->"
	@./$(PROJ) t/20081004-Altenau-und-umzu.gpx t/plus.gpx --plus 03:47:24 -q
	@if [ "1" == `grep -c 2008-10-04T11:34:47Z t/plus.gpx` ]; then echo -n " OK"; fi
	@if [ "1" == `grep -c 2008-10-04T17:41:46Z t/plus.gpx` ]; then echo " OK"; fi

