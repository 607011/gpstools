// $Id$
// Copyright (c) 2008 Oliver Lau <oliver@ersatzworld.net>
// Alle Rechte vorbehalten.

#ifndef __CALIBRATIONRENDERAREA_H_
#define __CALIBRATIONRENDERAREA_H_

#include <QtGlobal>
#include <QWidget>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QPixmap>
#include <QPolygon>
#include <QRect>
#include <QHash>
#include <QRadialGradient>

#include "gpslib/Trackpoint.h"
#include "gpslib/Track.h"

#include "CalibrationIniFile.h"


typedef QVector<QColor*> CalibrationPointColorList;
typedef QVector<QBrush*> CalibrationPointBrushList;


class CalibrationRenderArea : public QWidget
{
    Q_OBJECT

private:
    double curScale;
    double xCenter;
    double yCenter;
    QPoint lastDragPos;
    QPointF plotOffset;
    QCursor currentCursor;
    int calibrationPhase;
    QPixmap _MapPixmap;
    QPolygonF _XYPolygon;
    CalibrationPointColorList _Colors;
    CalibrationPointBrushList _Brushes;
    CalibrationIniFile* calibration;

private:
    void zoom(double zoomFactor, const QPoint& pos = QPoint());
    void pan(const QPointF& offset);

public:
    CalibrationRenderArea(QWidget* parent = NULL);
    ~CalibrationRenderArea();
    QSize minimumSizeHint(void) const;
    QSize sizeHint(void) const;
    void setTrack(const GPS::Track& trk);
    void reset(void);
    void loadMap(const QString& filename, CalibrationIniFile* cal);
    void clearCalibrationPoints(void);
    inline const CalibrationPointColorList& colors(void) const { return _Colors; }
    inline const QPolygonF& xyPolygon(void) const { return _XYPolygon; }
    inline const QPixmap& mapPixmap(void) const { return _MapPixmap; }

protected:
    void paintEvent(QPaintEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void resizeEvent(QResizeEvent *event);

signals:
    void newCalibrationPoint(double, double);
    void calibrationReady(void);
    // void xyChanged(double, double);

};

#endif // __CALIBRATIONRENDERAREA_H_
