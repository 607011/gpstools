# $Id$
# Copyright (c) 2008 Oliver Lau <oliver@ersatzworld.net>
# Alle Rechte vorbehalten.

PROJ=libtinyxml.a
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
DOXYGEN=doxygen
O=.o
X=
MKDIR=mkdir -p
MV=mv

TINYXML_O  := $(foreach i,tinyxml tinyxmlparser tinyxmlerror tinystr,$i.o)

all: $(PROJ)

macosx-release-universal:
	$(RM) -R i386
	$(RM) -R ppc
	$(MKDIR) x86
	$(MKDIR) ppc
	$(MAKE) clean
	$(MAKE) macosx-release-x86
	$(MV) $(PROJ) x86
	$(MAKE) clean
	$(MAKE) macosx-release-ppc
	$(MV) $(PROJ) ppc
	lipo -create ppc/$(PROJ) x86/$(PROJ) -output $(PROJ)

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

release:
	$(RM) $(PROJ)
	$(MAKE) all O="$(O)" \
		CFLAGS="$(CFLAGS)" \
		CXXFLAGS="$(CXXFLAGS)" \
		LDFLAGS="$(LDFLAGS)"

debug:
	$(RM) $(PROJ)
	$(MAKE) all O="$(O)" \
		CFLAGS="$(CFLAGS) $(DEBUGFLAGS)" \
		CXXFLAGS="$(CXXFLAGS) $(DEBUGFLAGS)" \
		LDFLAGS="$(LDFLAGS)"

clean:
	$(RM) $(TINYXML_O)
	$(RM) $(PROJ)

%.o: %.cpp %.h Makefile
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(PROJ): $(TINYXML_O)
	$(AR) r $(PROJ) $(TINYXML_O)
