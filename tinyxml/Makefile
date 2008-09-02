# $Id$

PROJ = libtinyxml.a
INC = 
OPTIMIZE = -O2 -finline-functions
# DEBUGFLAGS = -DDEBUG -D_DEBUG -ggdb -g3
CFLAGS = $(OPTIMIZE) $(INC) $(DEBUGFLAGS) -Wall
CXXFLAGS = $(OPTIMIZE) $(INC) $(DEBUGFLAGS) -Wall
LDFLAGS =
CC = gcc
CXX = g++
LD = ld
X =

TINYXML_O  := $(foreach i,tinyxml tinyxmlparser tinyxmlerror tinystr,$i.o)

all: $(PROJ)

clean:
	$(RM) $(TINYXML_O)
	$(RM) $(PROJ)

%.o: %.cpp %.h Makefile
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(PROJ): $(TINYXML_O)
	$(AR) r $(PROJ) $(TINYXML_O)
