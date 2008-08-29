# $Id$
# Copyright (c) 2008 Oliver Lau <ola@ct.heise.de>, Heise Zeitschriften Verlag.
# Alle Rechte vorbehalten.


INC = -I.. -I.
OPTIMIZE = -O2 -finline-functions
DEBUGFLAGS = -DDEBUG -D_DEBUG -ggdb -g3
CFLAGS = $(OPTIMIZE) $(INC) $(DEBUGFLAGS) -Wall
CXXFLAGS = $(OPTIMIZE) $(INC) $(DEBUGFLAGS) -Wall
LDFLAGS =
CC = gcc
CXX = g++
LD = ld
X =

all:
	make -C tinyxml
	make -C gpslib
	make -C gpsplot
	make -C test
	make -C wpl1000reader
	make -C trkcompare
	make -C splittrack
	make -C gpxtimeshift
	make -C Qeo

clean:
	make -C tinyxml clean
	make -C gpslib clean
	make -C gpsplot clean
	make -C test clean
	make -C wpl1000reader clean
	make -C trkcompare clean
	make -C splittrack clean
	make -C gpxtimeshift clean
	make -C Qeo clean

doc:
	make -C gpslib doc
