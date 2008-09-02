// $Id$
// Copyright (c) 2008 Oliver Lau <ola@ct.heise.de>, Heise Zeitschriften Verlag.
// Alle Rechte vorbehalten.

#ifndef __QTALTI_H_
#define __QTALTI_H_

#include <QMainWindow>
#include <QWidget>
#include <QTabWidget>
#include <QFileDialog>
#include <QPushButton>
#include <QLabel>
#include <QComboBox>
#include <QLayout>
#include <QLineEdit>

#include "gpslib/Track.h"
#include "gpslib/GPXFile.h"

#include "RenderArea.h"

using namespace GPS;

class QtAlti : public QMainWindow
{
    Q_OBJECT

private slots:
    void browseRef(void);
    void browseMy(void);
    void refTrackChanged(int);
    void myTrackChanged(int);
    void latlonChanged(double,double);
    void latlonMChanged(double,double);

private:
    QString lastPath;
    GPXFile refFile;
    GPXFile myFile;
    QComboBox* fileComboBoxRef;
    QPushButton* browseButtonRef;
    QComboBox* fileComboBoxMy;
    QPushButton* browseButtonMy;
    QPushButton* startButton;
    RenderArea* plotWidget;
    QLineEdit* latLineEdit;
    QLineEdit* lonLineEdit;
    QLineEdit* latMLineEdit;
    QLineEdit* lonMLineEdit;
    QTabWidget* tabWidget;

public:
    QtAlti(QWidget* parent = 0, Qt::WFlags flags = 0);
    ~QtAlti();
};

#endif // __QTALTI_H_
