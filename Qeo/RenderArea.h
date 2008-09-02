// $Id$
// Copyright (c) 2008 Oliver Lau <ola@ct.heise.de>, Heise Zeitschriften Verlag.
// Alle Rechte vorbehalten.

#ifndef __RENDERAREA_H_
#define __RENDERAREA_H_

#include <QtGlobal>
#include <QWidget>
#include <QDialog>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QImage>
#include <QPixmap>
#include <QTransform>
#include <QPolygon>
#include <QRect>

#include <cstdlib>

#include "gpslib/Trackpoint.h"
#include "gpslib/Track.h"

class RenderArea : public QWidget
{
    Q_OBJECT
private:
    GPS::Track _Trk;
    GPS::Track _RefTrk;
    double lonMax;
    double lonMin;
    double latMax;
    double latMin;
    double lonDiff;
    double latDiff;
    double lonCenter;
    double latCenter;
    double lat;
    double lon;
    double xs;
    double ys;
    double curScale;
    QPoint lastDragPos;
    QPointF plotOffset;
    bool drawTrackpoints;
    bool drawCenter;
    bool drawRaster;
    QCursor currentCursor;
    bool pointing;
    bool inserting;
    int calibrationPhase;
    GPS::Trackpoint* grabbedTrackpoint;
    GPS::Trackpoint undoableTrackpoint;
    bool trackHasChanged;
    QPoint newTrackpoint;
    GPS::Line matchedLine;
    QPixmap mapPixmap;
    QImage mapImage;
    QPolygonF lonlatPolygon;
    QPolygonF xyPolygon;
    QTransform mapTrans;

private:
    void calcBounds(void);
    void calcScale(void);
    void zoom(double);
    void pan(const QPointF& offset);

public:
    RenderArea(QWidget* parent = NULL);
    ~RenderArea();

    QSize minimumSizeHint(void) const;
    QSize sizeHint(void) const;

    void setTrack(const GPS::Track& trk);
    void setReferenceTrack(const GPS::Track& trk);
    void reset(void);

signals:
    void latlonChanged(double, double);
    void latlonMChanged(double, double);

protected:
     void paintEvent(QPaintEvent *event);
     void mouseReleaseEvent(QMouseEvent *event);
     void mouseMoveEvent(QMouseEvent *event);
     void mousePressEvent(QMouseEvent *event);
     void wheelEvent(QWheelEvent *event);
     void keyPressEvent(QKeyEvent *event);
     void resizeEvent(QResizeEvent *event);
};

#endif // __RENDERAREA_H_
