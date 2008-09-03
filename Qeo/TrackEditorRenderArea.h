// $Id$
// Copyright (c) 2008 Oliver Lau <oliver@ersatzworld.net>
// Alle Rechte vorbehalten.

#ifndef __TRACKEDITORRENDERAREA_H_
#define __TRACKEDITORRENDERAREA_H_

#include <QtGlobal>
#include <QWidget>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QImage>
#include <QPixmap>
#include <QTransform>
#include <QRect>
#include <QPoint>

#include <cstdlib>

#include "gpslib/Trackpoint.h"
#include "gpslib/Track.h"
#include "gpslib/TrackLine.h"

#include "CalibrationIniFile.h"

class TrackEditorRenderArea : public QWidget
{
    Q_OBJECT

private:
    GPS::Track* _Trk;
    double lonMax;
    double lonMin;
    double latMax;
    double latMin;
    double lonDiff;
    double latDiff;
    double lat;
    double lon;
    double curScale;
    QPoint lastDragPos;
    QPointF plotOffset;
    bool drawTrackpoints;
    bool drawCenter;
    bool drawInfoBox;
    QCursor currentCursor;
    bool pointing;
    GPS::Trackpoint* grabbedTrackpoint;
    GPS::Trackpoint undoableTrackpoint;
    GPS::Trackpoint* newTrackpoint;
    bool newTrackpointPending;
    bool _TrkWasEdited;
    GPS::TrackLine matchedLine;
    QString mapDirectoryName;
    QPixmap* mapPixmap;
    QTransform _TransGeoToPixel;
    QTransform _TransPixelToGeo;
    CalibrationIniFile* calibration;

private:
    void calcBounds(void);
    void zoom(double zoomFactor, const QPoint& pos = QPoint());
    void pan(const QPointF& offset);
    bool findMatchingMap(void);

public:
    TrackEditorRenderArea(QWidget* parent = NULL);
    ~TrackEditorRenderArea();
    QSize minimumSizeHint(void) const;
    QSize sizeHint(void) const;
    void setTrack(GPS::Track*);
    void setMapDirectoryName(const QString&);
    inline bool trackWasEdited(void) const { return _TrkWasEdited; }
    void reset(void);
    const QTransform& transGeoToPixel(void) const { return _TransGeoToPixel; }
    const QTransform& transPixelToGeo(void) const { return _TransPixelToGeo; }

signals:
    void trackChanged(void);
    void distanceChanged(double);

protected:
    void paintEvent(QPaintEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void resizeEvent(QResizeEvent *event);
};

#endif // __TRACKEDITORRENDERAREA_H_
