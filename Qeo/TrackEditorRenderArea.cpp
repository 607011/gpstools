// $Id: TrackEditorRenderArea.cpp bd86a16444b7 2008/08/27 15:09:21 Oliver Lau <ola@ct.heise.de> $
// Copyright (c) 2008 Oliver Lau <oliver@ersatzworld.net>
// Alle Rechte vorbehalten.

#include <QtGlobal>
#include <QPainter>
#include <QBrush>
#include <QPen>
#include <QDir>
#include <QStringList>
#include <QFileInfo>
#include <QPainterPath>
#include <QToolTip>
#include <QFont>

#ifdef _DEBUG
#include <QtDebug>
#endif

#include <cmath>

#include "TrackEditorRenderArea.h"
#include "CalibrationIniFile.h"

#include "gpslib/Track.h"

using namespace GPS;

static const double ZoomInFactor = 1.16;
static const double ZoomOutFactor = 1.0 / ZoomInFactor;
static const double PanSkip = 16;

TrackEditorRenderArea::TrackEditorRenderArea(QWidget* parent) : QWidget(parent)
{
    qRegisterMetaType<QPixmap>("QPixmap");
    _Trk = NULL;
    mapPixmap = NULL;
    drawTrackpoints = true;
    drawCenter = true;
    drawInfoBox = true;
    pointing = false;
    newTrackpoint = NULL;
    newTrackpointPending = false;
    grabbedTrackpoint = NULL;
    _TrkWasEdited = false;
    lonDiff = 0.0;
    latDiff = 0.0;
    curScale = 1.0;
    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);
    setMouseTracking(true);
    setFocusPolicy(Qt::ClickFocus);
    setFocus(Qt::MouseFocusReason);
    currentCursor = Qt::OpenHandCursor;
    setCursor(currentCursor);
    mapDirectoryName = "maps";
}


TrackEditorRenderArea::~TrackEditorRenderArea()
{
    // ...
}


QSize TrackEditorRenderArea::sizeHint(void) const
{
    return QSize(400, 300);
}


QSize TrackEditorRenderArea::minimumSizeHint(void) const
{
    return QSize(240, 160);
}


void TrackEditorRenderArea::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    if (lonDiff > 0.0 && latDiff > 0.0)
    {
        painter.save();
        painter.setRenderHint(QPainter::Antialiasing, true);
        painter.translate(plotOffset);
        if (mapPixmap != NULL)
        {
            painter.scale(curScale, curScale);
            QRect imgRect(0, 0, mapPixmap->width(), mapPixmap->height());
            painter.drawPixmap(imgRect, *mapPixmap);
        }
        else
        {
            // painter.scale(curScale * sin(deg2rad(latMax)), curScale);
            painter.scale(curScale, curScale);
        }
        const QPen refPen(QBrush(QColor(0x00, 0x00, 0xff, 0x80), Qt::SolidPattern), 5.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        const QPen ref2Pen(QBrush(QColor(0x00, 0x00, 0x00, 0x80), Qt::SolidPattern), 5.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        if (_Trk != NULL && _Trk->points().size() > 2)
        {
            QPainterPath path(QPointF(_Trk->points().front()->x(), _Trk->points().front()->y()));
            for (TrackpointList::const_iterator i = _Trk->points().begin() + 1; i != _Trk->points().end(); ++i)
                if ((*i)->kept())
                    path.lineTo((*i)->x(), (*i)->y());

            painter.setPen(refPen);
            painter.drawPath(path);
            if (matchedLine.p1() != NULL && matchedLine.p2() != NULL)
            {
                painter.setPen(ref2Pen);
                QPointF p1 = QPointF(matchedLine.p1()->x(), matchedLine.p1()->y());
                QPointF p2 = QPointF(matchedLine.p2()->x(), matchedLine.p2()->y());
                painter.drawLine(p1, p2);
            }

            if (drawTrackpoints)
            {
                painter.setPen(Qt::transparent);
                painter.setBrush(Qt::red);
                const QSizeF tptSize(4.0, 4.0);
                const QPointF tptOffset(0.5 * tptSize.width(), 0.5 * tptSize.height());
                for (TrackpointList::const_iterator i = _Trk->points().begin(); i != _Trk->points().end(); ++i)
                {
                    if ((*i)->kept())
                    {
                        const QPointF pt0((*i)->x(), (*i)->y());
                        const QRectF trkptRect(pt0 - tptOffset, tptSize);
                        painter.drawEllipse(trkptRect);
                    }
                }
            }
        }

        if (newTrackpoint != NULL)
        {
            painter.setPen(Qt::transparent);
            painter.setBrush(Qt::green);
            const QSizeF tptSize(4.0, 4.0);
            const QPointF tptOffset(0.5 * tptSize.width(), 0.5 * tptSize.height());
            const QPointF pt0(newTrackpoint->x(), newTrackpoint->y());
            const QRectF trkptRect(pt0 - tptOffset, tptSize);
            painter.drawEllipse(trkptRect);
        }

        painter.restore();
        if (drawInfoBox)
        {
            int line = 0;
            QFont boxFont;
            boxFont.setPointSize(8);
            boxFont.setStyleHint(QFont::TypeWriter, QFont::PreferBitmap);
            QFont boxHeaderFont(boxFont);
            boxHeaderFont.setWeight(QFont::Bold);
            painter.setRenderHint(QPainter::Antialiasing, true);
            painter.setFont(boxFont);
            QFontMetrics metrics = painter.fontMetrics();
            const int lineHeight = metrics.ascent()+1;
            const int baselineOffset = metrics.ascent()/2;
            const int boxPadding = metrics.averageCharWidth();
            const int boxMargin = metrics.averageCharWidth();
            const QString tString = tr("time: %1").arg(QString(Timestamp().toString().c_str()));
            QSize boxSize(metrics.width(tString)+2*boxPadding, 4*lineHeight);
            QPoint boxOrigin(width() - boxSize.width() - boxMargin, 2*baselineOffset);
            QRect boxRect(boxOrigin, boxSize);
            painter.setPen(Qt::transparent);
            painter.setBrush(QColor(0, 0, 0, 0x7f));
            painter.drawRoundedRect(boxRect, boxPadding, boxPadding);
            painter.setPen(Qt::white);
            painter.setFont(boxHeaderFont);
            painter.drawText(boxOrigin + QPoint(boxPadding, ++line*lineHeight), tr("current point"));
            painter.setFont(boxFont);
            painter.drawText(boxOrigin + QPoint(boxPadding, ++line*lineHeight), tr("lon: %1").arg(lon, 0, 'f', 9));
            painter.drawText(boxOrigin + QPoint(boxPadding, ++line*lineHeight), tr("lat: %1").arg(lat, 0, 'f', 9));
            if (grabbedTrackpoint != NULL)
            {
                line = 0;
                boxOrigin.setY(boxOrigin.y() + boxRect.height() + boxMargin);
                boxRect.setTopLeft(boxOrigin);
                boxRect.setHeight(7*lineHeight);
                painter.setPen(Qt::transparent);
                painter.drawRoundedRect(boxRect, boxPadding, boxPadding);
                painter.setPen(Qt::white);
                painter.setFont(boxHeaderFont);
                painter.drawText(boxOrigin + QPoint(boxPadding, ++line*lineHeight), tr("trackpoint"));
                painter.setFont(boxFont);
                painter.drawText(boxOrigin + QPoint(boxPadding, ++line*lineHeight), tr("lon: %1").arg(grabbedTrackpoint->longitude(), 0, 'f', 9));
                painter.drawText(boxOrigin + QPoint(boxPadding, ++line*lineHeight), tr("lat: %1").arg(grabbedTrackpoint->latitude(), 0, 'f', 9));
                painter.drawText(boxOrigin + QPoint(boxPadding, ++line*lineHeight), tr("km: %1").arg(1e-3*grabbedTrackpoint->distance(), 0, 'f'));
                if (grabbedTrackpoint->elevation().defined())
                    painter.drawText(boxOrigin + QPoint(boxPadding, ++line*lineHeight), tr("ele: %1").arg(grabbedTrackpoint->elevation().value()));
                if (!grabbedTrackpoint->timestamp().isNull())
                    painter.drawText(boxOrigin + QPoint(boxPadding, ++line*lineHeight), tr("time: %1").arg(QString(grabbedTrackpoint->timestamp().toString().c_str())));
            }
            if (newTrackpoint != NULL && !pointing) {
                line = 0;
                boxOrigin.setY(boxOrigin.y() + boxRect.height() + boxMargin);
                boxRect.setTopLeft(boxOrigin);
                boxRect.setHeight(7*lineHeight);
                painter.setPen(Qt::transparent);
                painter.drawRoundedRect(boxRect, boxPadding, boxPadding);
                painter.setPen(Qt::white);
                painter.setFont(boxHeaderFont);
                painter.drawText(boxOrigin + QPoint(boxPadding, ++line*lineHeight), tr("new point"));
                painter.setFont(boxFont);
                painter.drawText(boxOrigin + QPoint(boxPadding, ++line*lineHeight), tr("lon: %1").arg(newTrackpoint->longitude(), 0, 'f', 9));
                painter.drawText(boxOrigin + QPoint(boxPadding, ++line*lineHeight), tr("lat: %1").arg(newTrackpoint->latitude(), 0, 'f', 9));
                painter.drawText(boxOrigin + QPoint(boxPadding, ++line*lineHeight), tr("km: %1").arg(1e-3*newTrackpoint->distance(), 0, 'f'));
                if (newTrackpoint->elevation().defined())
                    painter.drawText(boxOrigin + QPoint(boxPadding, ++line*lineHeight), tr("ele: %1").arg(newTrackpoint->elevation().value()));
                if (!newTrackpoint->timestamp().isNull())
                    painter.drawText(boxOrigin + QPoint(boxPadding, ++line*lineHeight), tr("time: %1").arg(QString(newTrackpoint->timestamp().toString().c_str())));

                painter.drawText(boxOrigin + QPoint(4, ++line*metrics.ascent()+baselineOffset), tr("time: %1").arg(QString(newTrackpoint->timestamp().toString().c_str())));
            }
        }
    }
    if (drawCenter)
    {
        painter.setRenderHint(QPainter::Antialiasing, false);
        const QPen centerPen(Qt::darkGray, 1.0, Qt::DashLine);
        const int x0 = width()  / 2;
        const int y0 = height() / 2;
        painter.setPen(centerPen);
        painter.drawLine(x0 - 20, y0, x0 + 20, y0);
        painter.drawLine(x0, y0 - 20, x0, y0 + 20);
    }
}


void TrackEditorRenderArea::setTrack(GPS::Track* trk)
{
    if (trk != NULL && !trk->isEmpty()) {
        _Trk = trk;
        calcBounds();
    }
    else if (_Trk != NULL) {
        _Trk->clear();
    }

    findMatchingMap();
    // transform all trackpoints' longitude and latitude to pixel coordinates
    if (_Trk != NULL) {
        for (GPS::TrackpointList::iterator i = _Trk->points().begin(); i != _Trk->points().end(); ++i)
        {
            QPointF pixelCoord(_TransGeoToPixel.map(QPointF((*i)->longitude(), (*i)->latitude())));
            (*i)->setX(pixelCoord.x());
            (*i)->setY(pixelCoord.y());
        }
    }
    reset();
}


void TrackEditorRenderArea::calcBounds(void)
{
    lonMin = DBL_MAX;
    latMin = DBL_MAX;
    lonMax = DBL_MIN;
    latMax = DBL_MIN;
    if (_Trk != NULL)
    {
        for (GPS::TrackpointList::const_iterator i = _Trk->points().begin(); i != _Trk->points().end(); ++i)
        {
            if ((*i)->latitude()  < latMin) latMin = (*i)->latitude();
            if ((*i)->latitude()  > latMax) latMax = (*i)->latitude();
            if ((*i)->longitude() < lonMin) lonMin = (*i)->longitude();
            if ((*i)->longitude() > lonMax) lonMax = (*i)->longitude();
        }
    }
    lonDiff = lonMax - lonMin;
    latDiff = latMax - latMin;
}


void TrackEditorRenderArea::wheelEvent(QWheelEvent* event)
{
    double decelerator = (event->modifiers() & Qt::AltModifier)? 640 : 160;
    if (event->modifiers() & Qt::ControlModifier)
        decelerator *= 2;
    double numSteps = (double) event->delta() / decelerator;
    zoom(pow(ZoomInFactor, numSteps), event->pos());
}


void TrackEditorRenderArea::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton && !pointing && newTrackpoint == NULL)
    {
        lastDragPos = event->pos();
        currentCursor = Qt::ClosedHandCursor;
        setCursor(currentCursor);
    }
}


void TrackEditorRenderArea::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        if (newTrackpoint == NULL && !pointing)
        {
            plotOffset += event->pos() - lastDragPos;
            lastDragPos = QPoint();
            currentCursor = Qt::OpenHandCursor;
            setCursor(currentCursor);
        }
        else if (newTrackpoint != NULL && !pointing)
        {
            TrackpointList::iterator i = _Trk->getMatchingTrackpointIter(matchedLine);
            newTrackpoint->keep();
            _Trk->insert(i, newTrackpoint);
            _TrkWasEdited = true;
            newTrackpoint = NULL;
            emit trackChanged();
        }
        else
        {
            pointing = false;
            calcBounds();
            emit trackChanged();
        }
        update();
    }
}


void TrackEditorRenderArea::mouseMoveEvent(QMouseEvent* event)
{
    if (lonDiff > 0.0 && latDiff > 0.0)
    {
        QPointF pixelCoord((event->x() - plotOffset.x()) / curScale, (event->y() - plotOffset.y()) / curScale);
        if (_Trk != NULL && !_Trk->isEmpty())
        {
            QPointF geo = _TransPixelToGeo.map(pixelCoord);
            lon = geo.x();
            lat = geo.y();
        }

        if ((event->buttons() & Qt::LeftButton) && pointing)
        {
            if (grabbedTrackpoint != NULL)
            {
                grabbedTrackpoint->setLongitude(lon);
                grabbedTrackpoint->setLatitude(lat);
                grabbedTrackpoint->setX(pixelCoord.x());
                grabbedTrackpoint->setY(pixelCoord.y());
                _TrkWasEdited = true;
            }
        }
        else if (event->buttons() & Qt::LeftButton)
        {
            QPoint panOffset = event->pos() - lastDragPos;
            lastDragPos = event->pos();
            pan(panOffset);
        }
        else
        {
            grabbedTrackpoint = _Trk->getMatchingTrackpoint(lon, lat, 4.0*curScale);
            if (grabbedTrackpoint != NULL)
                undoableTrackpoint = *grabbedTrackpoint;
            matchedLine = _Trk->getMatchingLineXY(pixelCoord.x(), pixelCoord.y(), 2.0*curScale);
            if (newTrackpoint != NULL)
                delete newTrackpoint;
            newTrackpoint = matchedLine.interpolated();
            if (newTrackpoint != NULL) {
                QPointF newTrackpointXY(_TransGeoToPixel.map(QPointF(newTrackpoint->longitude(), newTrackpoint->latitude())));
                newTrackpoint->setX(newTrackpointXY.x());
                newTrackpoint->setY(newTrackpointXY.y());
                lastDragPos = event->pos();
                emit distanceChanged(newTrackpoint->distance());
            }
            else {
                emit distanceChanged(-1);
            }
        }

        Trackpoint current(lon, lat);
        if (((newTrackpoint != NULL) && (current.rangeTo(newTrackpoint) < 7.5)) || grabbedTrackpoint != NULL)
        {
            setCursor(Qt::PointingHandCursor);
            pointing = grabbedTrackpoint != NULL;
        }
        else
        {
            setCursor(currentCursor);
            pointing = false;
        }

        if ((_Trk != NULL && !_Trk->isEmpty()) || (newTrackpoint != NULL) || (grabbedTrackpoint != NULL))
            update();
    }
}


void TrackEditorRenderArea::pan(const QPointF& offset)
{
    if (lonDiff > 0.0 && latDiff > 0.0)
        plotOffset += offset;
}


void TrackEditorRenderArea::zoom(double zoomFactor, const QPoint& pos)
{
    if (lonDiff > 0.0 && latDiff > 0.0)
    {
        curScale *= zoomFactor;
        QPointF panOffset = (!pos.isNull())? QPointF(pos) : QPointF(width()/2, height()/2);
        pan((1 - zoomFactor) * (panOffset - plotOffset));
        update();
    }
}


void TrackEditorRenderArea::reset(void)
{
    curScale = 1.0;
    plotOffset = QPoint();
    grabbedTrackpoint = NULL;
    newTrackpoint = NULL;
    matchedLine.reset();
    update();
}


void TrackEditorRenderArea::keyPressEvent(QKeyEvent* event)
{
    switch (event->key())
    {
    case Qt::Key_Right:
        pan(QPointF(-PanSkip, 0.0));
        update();
        break;
    case Qt::Key_Left:
        pan(QPointF(+PanSkip, 0.0));
        update();
        break;
    case Qt::Key_Up:
        pan(QPointF(0.0, +PanSkip));
        update();
        break;
    case Qt::Key_Down:
        pan(QPointF(0.0, -PanSkip));
        update();
        break;
    case Qt::Key_Plus:
        zoom(ZoomInFactor);
        update();
        break;
    case Qt::Key_Minus:
        zoom(ZoomOutFactor);
        update();
        break;
    case Qt::Key_Escape: // undo
        if (grabbedTrackpoint != NULL)
        {
            grabbedTrackpoint->setLatitude(undoableTrackpoint.latitude());
            grabbedTrackpoint->setLongitude(undoableTrackpoint.longitude());
        }
        update();
        break;
    case Qt::Key_Backspace:
        reset();
        break;
    case Qt::Key_Delete:
        if (grabbedTrackpoint != NULL && _Trk != NULL)
        {
            TrackpointList::iterator trkpt = _Trk->getMatchingTrackpointIter(lon, lat, 7.5);
            if (trkpt != _Trk->points().end())
            {
                _Trk->erase(trkpt);
                delete grabbedTrackpoint;
                grabbedTrackpoint = NULL;
                matchedLine.reset();
                _TrkWasEdited = true;
                emit trackChanged();
            }
        }
        update();
        break;
    default:
        QWidget::keyPressEvent(event);
        break;
    }
}


void TrackEditorRenderArea::resizeEvent(QResizeEvent* /* event */)
{
    update();
}


void TrackEditorRenderArea::setMapDirectoryName(const QString& name)
{
    mapDirectoryName = name;
}


bool TrackEditorRenderArea::findMatchingMap(void)
{
    QDir mapDir(mapDirectoryName);
    QStringList nameFilters;
    nameFilters << "*.ini";
    QStringList calibrationFileNames = mapDir.entryList(nameFilters, QDir::Files | QDir::NoDotAndDotDot | QDir::Readable | QDir::CaseSensitive);
    QString calFile;
    foreach (calFile, calibrationFileNames)
    {
        QFileInfo fileInfo(mapDirectoryName + "/" + calFile);
        QString mapFile = fileInfo.completeBaseName();
        CalibrationIniFile cal(mapDirectoryName + "/" + calFile);
        if ((cal.topLeft().x() > lonMin && cal.topLeft().x() < lonMax) ||
            (cal.bottomRight().x() > lonMin && cal.bottomRight().x() < lonMax) ||
            (cal.topLeft().y() > latMin && cal.topLeft().y() < latMax) ||
            (cal.bottomRight().y() > latMin && cal.bottomRight().y() < latMax))
        {
            mapPixmap = new QPixmap(mapDirectoryName + "/" + mapFile);
            _TransGeoToPixel = cal.transformGeoToPixel();
            _TransPixelToGeo = cal.transformPixelToGeo();
            return true;
        }
    }

    if (mapPixmap != NULL) {
        delete mapPixmap;
        mapPixmap = NULL;
    }
    QPolygonF lonLatPolygon;
    lonLatPolygon << QPointF(lonMin, latMax) << QPointF(lonMax, latMax) << QPointF(lonMax, latMin) << QPointF(lonMin, latMin);
    QPolygonF xyPolygon;
    xyPolygon << QPointF(0, 0) << QPointF(width(), 0) << QPointF(width(), height()) << QPointF(0, height());
    QTransform::quadToQuad(lonLatPolygon, xyPolygon, _TransGeoToPixel);
    QTransform::quadToQuad(xyPolygon, lonLatPolygon, _TransPixelToGeo);
    return false;
}
