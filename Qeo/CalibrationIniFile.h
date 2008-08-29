// $Id: CalibrationIniFile.h bd86a16444b7 2008/08/27 15:09:21 Oliver Lau <ola@ct.heise.de> $
// Copyright (c) 2008 Oliver Lau <oliver@ersatzworld.net>
// Alle Rechte vorbehalten.

#ifndef __CALIBRATIONINIFILE_H_
#define __CALIBRATIONINIFILE_H_

#include <QString>
#include <QPointF>
#include <QPolygonF>
#include <QTransform>

class CalibrationIniFile
{
public:
    CalibrationIniFile(void);
    CalibrationIniFile(QString filename);
    ~CalibrationIniFile(void);

    inline double top(void) const { return _Top; }
    inline double left(void) const { return _Left; }
    inline double bottom(void) const { return _Bottom; }
    inline double right(void) const { return _Right; }
    inline QPointF topLeft(void) const { return QPointF(_Left, _Top); }
    inline QPointF bottomRight(void) const { return QPointF(_Right, _Bottom); }
    inline const QTransform& transformGeoToPixel(void) const { return _TransformGeoToPixel; }
    inline const QTransform& transformPixelToGeo(void) const { return _TransformPixelToGeo; }
    inline const QPolygonF& xyPolygon(void) const { return _XYPolygon; }
    inline const QPolygonF& lonLatPolygon(void) const { return _LonLatPolygon; }
    inline void setTop(double top) { _Top = top; }
    inline void setLeft(double left) { _Left = left; }
    inline void setBottom(double bottom) { _Bottom = bottom; }
    inline void setRight(double right) { _Right = right; }
    inline void setXYPolygon(QPolygonF polygon) { _XYPolygon = polygon; }
    inline void setLonLatPolygon(QPolygonF polygon) { _LonLatPolygon = polygon; }
    void save(void);

private:
    QString _Filename;
    double _Top;
    double _Left;
    double _Bottom;
    double _Right;
    QPolygonF _XYPolygon;
    QPolygonF _LonLatPolygon;
    QTransform _TransformGeoToPixel;
    QTransform _TransformPixelToGeo;

private:
    void load(void);

};


#endif // __CALIBRATIONINIFILE_H_
