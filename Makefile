# $Id$
# Copyright (c) 2008 Oliver Lau <oliver@ersatzworld.net>
# Alle Rechte vorbehalten.

INC=-I.. -I.
OPTIMIZE=-O2 -finline-functions
DEBUGFLAGS=-DDEBUG -D_DEBUG -ggdb -g3
LDFLAGS=
CC=gcc
CXX=g++
LD=ld
X=
O=.o
CFLAGS=$(OPTIMIZE) $(INC) -Wall
CXXFLAGS=$(OPTIMIZE) $(INC) -Wall -fPIC

PROJECTS:=tinyxml gpslib gpsplot splittrack trkcompare wpl1000reader areameter gpxtimeshift

all:
	for i in $(PROJECTS); do \
	  $(MAKE) -C "$${i}" all \
	    O="$(O)" \
	    CFLAGS="$(CFLAGS)" \
            CXXFLAGS="$(CXXFLAGS)" \
            LDFLAGS="$(LDFLAGS)" ; \
	done

macosx-release-universal:
	$(MAKE) macosx-release-universal \
	    O="$(O)" \
	    CFLAGS="$(CFLAGS)" \
            CXXFLAGS="$(CXXFLAGS)" \
            LDFLAGS="$(LDFLAGS)"

macosx-release-x86:
	$(MAKE) release \
	    O="$(O)" \
	    CFLAGS="$(CFLAGS) -arch i386" \
            CXXFLAGS="$(CXXFLAGS) -arch i386" \
            LDFLAGS="$(LDFLAGS) -arch i386"

macosx-debug-x86:
	$(MAKE) debug \
	    O="$(O)" \
	    CFLAGS="$(CFLAGS) -arch i386" \
            CXXFLAGS="$(CXXFLAGS) -arch i386" \
            LDFLAGS="$(LDFLAGS) -arch i386"

macosx-release-ppc:
	$(MAKE) release \
	    O="$(O)" \
	    CFLAGS="$(CFLAGS) -arch ppc" \
            CXXFLAGS="$(CXXFLAGS) -arch ppc" \
            LDFLAGS="$(LDFLAGS) -arch ppc"

macosx-debug-ppc:
	$(MAKE) debug \
	    O="$(O)" \
	    CFLAGS="$(CFLAGS) -arch ppc" \
            CXXFLAGS="$(CXXFLAGS) -arch ppc" \
            LDFLAGS="$(LDFLAGS) -arch ppc"

debug:
	$(MAKE) all \
	    O="$(O)" \
	    CFLAGS="$(CFLAGS) $(DEBUGFLAGS)" \
            CXXFLAGS="$(CXXFLAGS) $(DEBUGFLAGS)" \
            LDFLAGS="$(LDFLAGS)"

release:
	$(MAKE) all \
	    O="$(O)" \
	    CFLAGS="$(CFLAGS)" \
            CXXFLAGS="$(CXXFLAGS)" \
            LDFLAGS="$(LDFLAGS)"

clean:
	for i in $(PROJECTS); do $(MAKE) -C "$${i}" clean; done
