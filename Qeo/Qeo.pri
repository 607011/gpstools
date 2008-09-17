# $Id: Qeo.pri 0aafbc2f4c11 2008/08/12 15:32:38 Oliver Lau <oliver@ersatzworld.net> $

HEADERS += ./CalibrationRenderArea.h \
    ./MainWindow.h \
    ./TrackEditorRenderArea.h \
    ./TrackTableModel.h \
    ./LonLatTableModel.h \
    ./CalibrationPointDelegate.h \
    ./CalibrationPointView.h \
    ./CalibrationIniFile.h

SOURCES += ./CalibrationRenderArea.cpp \
    ./main.cpp \
    ./MainWindow.cpp \
    ./TrackEditorRenderArea.cpp \
    ./TrackTableModel.cpp \
    ./LonLatTableModel.cpp \
    ./CalibrationPointDelegate.cpp \
    ./CalibrationPointView.cpp \
    ./CalibrationIniFile.cpp


FORMS += ./MainWindow.ui

RESOURCES += ./Qeo.qrc
