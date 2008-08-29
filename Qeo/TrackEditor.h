// $Id: TrackEditor.h bd86a16444b7 2008/08/27 15:09:21 Oliver Lau <ola@ct.heise.de> $
// Copyright (c) 2008 Oliver Lau <oliver@ersatzworld.net>
// Alle Rechte vorbehalten.

#ifndef __TRACKEDITOR_H_
#define __TRACKEDITOR_H_

#include <QtGlobal>
#include <QWidget>

#include "ui_TrackEditor.h"

#include "gpslib/GPXFile.h"


class TrackEditor : public QWidget
{
    Q_OBJECT

private:
    Ui::TrackEditorClass ui;
    GPS::GPXFile* _GPX;

public:
    TrackEditor(QWidget* parent = NULL);
    ~TrackEditor();
    QSize minimumSizeHint(void) const;
    QSize sizeHint(void) const;

    void setGPXFile(GPS::GPXFile* gpx);
    inline GPS::GPXFile* gpxFile(void) const { return _GPX; }
    bool trackWasEdited(void) const;

private slots:
    void on_weedingTypeComboBox_currentIndexChanged(int);
    void on_thresholdSpinBox_valueChanged(double);

public slots:
    void editedTrackChanged(void); // TODO: <-- change method name to somewhat more meaningful

signals:
    void trackChanged(void);
};

#endif // __TRACKEDITOR_H_
