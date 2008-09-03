// $Id$
// Copyright (c) 2008 Oliver Lau <oliver@ersatzworld.net>
// Alle Rechte vorbehalten.

#ifndef __ELEVATIONPROFILERENDERAREA_H_
#define __ELEVATIONPROFILERENDERAREA_H_

#include <QWidget>
#include <QFrame>
#include <QVector>

#include "gpslib/Value.h"
#include "gpslib/Track.h"

class ElevationProfileRenderArea : public QFrame {
    Q_OBJECT

public:
    ElevationProfileRenderArea(QWidget* parent = 0);
    ~ElevationProfileRenderArea();
    QSize minimumSizeHint(void) const;
    QSize sizeHint(void) const;
    void setTrack(GPS::Track* const trk);

    double getTickWidth(double max, int tickWidth, double scale) const;

public slots:
    void setMarker(double marker = -1);

private:
    GPS::Track* _Trk;
    GPS::DoubleValue _Marker;
    QVector<int> tickSplitter;

protected:
    void paintEvent(QPaintEvent* event);
};


#endif // __ELEVATIONPROFILERENDERAREA_H_
