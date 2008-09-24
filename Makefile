# $Id$
# Copyright (c) 2008 Oliver Lau <oliver@ersatzworld.net>
# Alle Rechte vorbehalten.

PROJ=gpstools
VERSION=1.0
INC=-I.. -I.
OPTIMIZE=-O2 -finline-functions
DEBUGFLAGS=-DDEBUG -D_DEBUG -ggdb -g3
LDFLAGS=
CC=gcc
CXX=g++
LD=ld
TAR=tar
MV=mv
CP=cp
MKDIR=mkdir -p
STRIP=strip
X=
O=.o
CFLAGS=$(OPTIMIZE) $(INC) -Wall
CXXFLAGS=$(OPTIMIZE) $(INC) -Wall -fPIC

PROJECTS:=tinyxml gpslib gpsplot splittrack trkcompare wpl1000reader areameter gpxtimeshift
FILES := $(foreach i,gpsplot splittrack trkcompare wpl1000reader areameter gpxtimeshift,$i/$i)

all:
	for i in $(PROJECTS); do \
	  $(MAKE) -C "$${i}" all \
	    O="$(O)" \
	    CFLAGS="$(CFLAGS)" \
            CXXFLAGS="$(CXXFLAGS)" \
            LDFLAGS="$(LDFLAGS)" ; \
	done

macosx-release-universal:
	for i in $(PROJECTS); do \
	  $(MAKE) -C "$${i}" macosx-release-universal \
	    O="$(O)" \
	    CFLAGS="$(CFLAGS)" \
            CXXFLAGS="$(CXXFLAGS)" \
            LDFLAGS="$(LDFLAGS)" ; \
	done

macosx-release-x86:
	for i in $(PROJECTS); do \
	  $(MAKE) -C "$${i}" macosx-release-x86 \
	    O="$(O)" \
	    CFLAGS="$(CFLAGS)" \
            CXXFLAGS="$(CXXFLAGS)" \
            LDFLAGS="$(LDFLAGS)" ; \
	done

macosx-release-ppc:
	for i in $(PROJECTS); do \
	  $(MAKE) -C "$${i}" macosx-release-ppc \
	    O="$(O)" \
	    CFLAGS="$(CFLAGS)" \
            CXXFLAGS="$(CXXFLAGS)" \
            LDFLAGS="$(LDFLAGS)" ; \
	done

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
	for i in $(PROJECTS); do \
	    $(MAKE) -C "$${i}" clean \
	; \
	done

dist: 

dist-bz2:
	if [[ -d "$(PROJ)-$(VERSION)" ]] ; then $(RM) -R $(PROJ)-$(VERSION); fi
	$(MKDIR) $(PROJ)-$(VERSION)
	for i in $(FILES); do \
	    $(CP) "$${i}" $(PROJ)-$(VERSION) \
	; \
	done
	$(STRIP) $(PROJ)-$(VERSION)/*
	$(CP) LICENSE.txt "$(PROJ)-$(VERSION)"
	$(TAR) -cjvf $(PROJ)-$(VERSION).tar.bz2 $(PROJ)-$(VERSION)/*

distclean: clean
	$(RM) -R $(PROJ)-$(VERSION)
