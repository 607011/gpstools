# $Id$
# Copyright (c) 2008 Oliver Lau <ola@ct.heise.de>, Heise Zeitschriften Verlag.
# Alle Rechte vorbehalten.

TEMPLATE = app
TARGET = Qeo
win32:LIBS += gpslib.lib getopt.lib tinyxmld.lib
unix:LIBS += -lgps -ltinyxml
win32:LIBPATH += ../Debug ../tinyxml/Debug
unix:LIBPATH += ../gpslib ../tinyxml
DEFINES += _DEBUG DEBUG
DESTDIR = ./Debug
CONFIG += debug
INCLUDEPATH += ./GeneratedFiles \
    ./GeneratedFiles/Debug \
    . \
    ..
DEPENDPATH += .
MOC_DIR += ./GeneratedFiles/debug
OBJECTS_DIR += debug
UI_DIR += ./GeneratedFiles
RCC_DIR += ./GeneratedFiles

include(Qeo.pri)
