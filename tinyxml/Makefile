# $Id$

PROJ=libtinyxml.a
INC=
OPTIMIZE=-O2 -finline-functions
CFLAGS=$(OPTIMIZE) $(INC) -Wall
CXXFLAGS=$(OPTIMIZE) $(INC) -Wall
LFLAGS=
CC=gcc
CXX=g++
LD=ld
X=

TINYXML_O  := $(foreach i,tinyxml tinyxmlparser tinyxmlerror tinystr,$i.o)

release: all

all: $(PROJ)

clean:
	$(RM) $(TINYXML_O)
	$(RM) $(PROJ)

%.o: %.cpp %.h Makefile
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(PROJ): $(TINYXML_O)
	$(AR) r $(PROJ) $(TINYXML_O)
