// $Id: ElevationProfileRenderArea.cpp 572e8ed1f764 2008/08/27 19:07:35 Oliver Lau <oliver.lau@gmail.com> $
// Copyright (c) 2008 Oliver Lau <oliver@ersatzworld.net>
// Alle Rechte vorbehalten.

#include "ElevationProfileRenderArea.h"

#include <QtGlobal>
#include <QPainterPath>
#include <QPainter>
#include <QFontMetrics>

#include <cstdlib>
#include <cmath>

#ifdef _DEBUG
#include <QtDebug>
#endif // _DEBUG


ElevationProfileRenderArea::ElevationProfileRenderArea(QWidget* parent) : QFrame(parent)
{
    _Trk = NULL;
    tickSplitter << 1 << 2 << 5;
}


ElevationProfileRenderArea::~ElevationProfileRenderArea()
{
    // ...
}


QSize ElevationProfileRenderArea::minimumSizeHint(void) const 
{
    return QSize(200, 50);
}


QSize ElevationProfileRenderArea::sizeHint(void) const
{
    return QSize(400, 100);
}


void ElevationProfileRenderArea::setTrack(GPS::Track* const trk)
{
    _Trk = trk;
    update();
}


void ElevationProfileRenderArea::setMarker(double marker)
{
    if (marker < 0)
        _Marker.undef();
    else
        _Marker = marker;
    update();
}


double ElevationProfileRenderArea::getTickWidth(double maxVal, int _tickWidth, double scale) const
{
    double tickWidth0f = pow(10.0, floor(log10(maxVal)));
    int tickWidth0 = (int) tickWidth0f;
    int tickWidth = tickWidth0;
    bool found = false;
    double minTickWidth = 2.5 * (_tickWidth + 3);
    while (tickWidth0 > 1 && !found)
    {
        for (QVector<int>::const_iterator i = tickSplitter.begin(); i != tickSplitter.end(); ++i)
        {
            tickWidth = tickWidth0 / (*i);
            if (tickWidth * scale <= minTickWidth) { found = true; break; }
        }
        tickWidth0 /= 10;
    }
    return tickWidth;
}


inline int roundDown(int x, int s)
{
    int rem = x % s;
    return x - rem;
}


inline int roundUp(int x, int s)
{
    int res = roundDown(x, s);
    return res + s;
}


void ElevationProfileRenderArea::paintEvent(QPaintEvent* /* event */)
{
    if (_Trk != NULL && _Trk->maxElevation().defined() && _Trk->minElevation().defined())
    {
        QPainter painter(this);
        QFontMetrics metrics = painter.fontMetrics();
        painter.setBrush(Qt::NoBrush);
        painter.fillRect(rect(), QBrush(Qt::white));
        double eleMin = _Trk->minElevation().value();
        double eleDiff = _Trk->maxElevation().value() - eleMin;
        double strokeWidth = 2;
        double eleScale = (height() - 2 * strokeWidth) / eleDiff;
        double distScale = width() / _Trk->distance();
        painter.setRenderHint(QPainter::Antialiasing, false);
        QPen rasterPen(QBrush(QColor(0x99, 0x99, 0x99), Qt::SolidPattern), 1, Qt::DotLine);
        double xd = getTickWidth(_Trk->distance(), metrics.width("1000 km"), distScale);
        for (double x = 0; x < _Trk->distance(); x += xd)
        {
            QPoint p0((int) (x * distScale), 0);
            painter.setPen(rasterPen);
            painter.drawLine(p0, QPoint((int) (x * distScale), height()));
            painter.setPen(Qt::darkGray);
            painter.drawText(p0 + QPoint(3, 10), QString("%1 km").arg((int) (1e-3*x)));
        }
        double yd = getTickWidth(eleDiff, metrics.ascent(), eleScale);
        eleMin = roundDown((int) eleMin, (int) yd);
        double eleMax = roundUp((int) _Trk->maxElevation().value(), (int) yd);
        eleDiff = eleMax - eleMin;
        eleScale = (height() - 2 * strokeWidth) / eleDiff;
#ifdef _DEBUG
        qDebug() << "eleMin = " << eleMin << " / eleDiff = " << eleDiff << " / yd = " << yd;
#endif
        for (double y = 0; y < eleDiff; y += yd)
        {
            QPoint p0(0, (int) (y * eleScale));
            painter.setPen(rasterPen);
            painter.drawLine(p0, QPoint(width(), (int) (y * eleScale)));
            painter.setPen(Qt::darkGray);
            painter.drawText(QPoint(3, height()) - p0, QString("%1 m").arg((int)(y+eleMin)));
        }
        painter.setRenderHint(QPainter::Antialiasing, true);
        QPen profilePen(QBrush(QColor(0x00, 0x00, 0xff, 0x80), Qt::SolidPattern), strokeWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        painter.setPen(profilePen);
        QPainterPath path(QPointF(_Trk->points().front()->distance(), _Trk->points().front()->elevation().value()));
        for (GPS::TrackpointList::const_iterator i = _Trk->points().begin()+1; i != _Trk->points().end(); ++i)
            if ((*i)->kept())
                path.lineTo((*i)->distance() * distScale, height() - strokeWidth - eleScale * ((*i)->elevation().value() - eleMin));
        painter.drawPath(path);
        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor(0xff, 0x00, 0x00, 0x80));
        for (GPS::TrackpointList::const_iterator i = _Trk->points().begin(); i != _Trk->points().end(); ++i)
            if ((*i)->kept())
                painter.drawRect((*i)->distance() * distScale - 2, height() - strokeWidth - eleScale * ((*i)->elevation().value() - eleMin) - 2, 4, 4);
        if (_Marker.defined()) 
        {
            painter.setRenderHint(QPainter::Antialiasing, false);
            const QPen markerPen(QBrush(QColor(0xff, 0x00, 0x00, 0x80), Qt::SolidPattern), 1.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
            painter.setPen(markerPen);
            QLineF m(_Marker.value()*distScale, 0, _Marker.value()*distScale, height());
            painter.drawLine(m);
        }
    }
}
