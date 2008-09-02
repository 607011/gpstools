// $Id$
// Copyright (c) 2008 Oliver Lau <oliver@ersatzworld.net>
// Alle Rechte vorbehalten.

#include <QSettings>

#include "CalibrationIniFile.h"

CalibrationIniFile::CalibrationIniFile(void)
{
    // ...
}


CalibrationIniFile::CalibrationIniFile(QString filename)
{
    _Filename = filename;
    load();
}


CalibrationIniFile::~CalibrationIniFile(void)
{
    // ...
}


void CalibrationIniFile::load(void)
{
    _XYPolygon.clear();
    QSettings settings(_Filename, QSettings::IniFormat);
    int size = settings.beginReadArray("xy");
    for (int i = 0; i < size; ++i)
    {
        settings.setArrayIndex(i);
        double x = settings.value("x").toDouble();
        double y = settings.value("y").toDouble();
        QPointF xy(x, y);
        _XYPolygon << xy;
    }
    settings.endArray();

    _LonLatPolygon.clear();
    size = settings.beginReadArray("geo");
    for (int i = 0; i < size; ++i)
    {
        settings.setArrayIndex(i);
        double lon = settings.value("lon").toDouble();
        double lat = settings.value("lat").toDouble();
        QPointF ll(lon, lat);
        _LonLatPolygon << ll;
    }
    settings.endArray();

    QTransform::quadToQuad(_XYPolygon, _LonLatPolygon, _TransformPixelToGeo);
    QTransform::quadToQuad(_LonLatPolygon, _XYPolygon, _TransformGeoToPixel);

    settings.beginGroup("boundaries");
    _Top = settings.value("top").toDouble();
    _Left = settings.value("left").toDouble();
    _Bottom = settings.value("bottom").toDouble();
    _Right = settings.value("right").toDouble();
    settings.endGroup();
}


void CalibrationIniFile::save(void)
{
    QSettings settings(_Filename, QSettings::IniFormat);
    settings.beginGroup("boundaries");
    settings.setValue("left", _Left);
    settings.setValue("top", _Top);
    settings.setValue("right", _Right);
    settings.setValue("bottom", _Bottom);
    settings.endGroup();
    settings.beginWriteArray("xy");
    int idx = 0;
    for (QPolygonF::const_iterator i = _XYPolygon.begin(); i != _XYPolygon.end(); ++i)
    {
        settings.setArrayIndex(idx++);
        settings.setValue("x", (*i).x());
        settings.setValue("y", (*i).y());
    }
    settings.endArray();
    settings.beginWriteArray("geo");
    idx = 0;
    for (QPolygonF::const_iterator i = _LonLatPolygon.begin(); i != _LonLatPolygon.end(); ++i)
    {
        settings.setArrayIndex(idx++);
        settings.setValue("lon", (*i).x());
        settings.setValue("lat", (*i).y());
    }
    settings.endArray();
    settings.sync();
}
