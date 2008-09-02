// $Id$
// Copyright (c) 2008 Oliver Lau <ola@ct.heise.de>, Heise Zeitschriften Verlag.
// Alle Rechte vorbehalten.

#include <QPainter>
#include <QBrush>
#include <QPen>

#ifdef _DEBUG
#include <QtDebug>
#endif

#include <cmath>

#include "RenderArea.h"

using namespace GPS;

static const double ZoomInFactor = 1.16f;
static const double ZoomOutFactor = 1.0f / ZoomInFactor;
static const int PanSkip = 16;

RenderArea::RenderArea(QWidget* parent) : QWidget(parent)
{
    drawTrackpoints = true;
    drawCenter = true;
    drawRaster = true;
    pointing = false;
    inserting = false;
    grabbedTrackpoint = NULL;
    trackHasChanged = false;
    calibrationPhase = 0;
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
    calcScale();
    mapImage.load("maps/Brocken-300dpi.tif");
    lonlatDialog = new LonLatDialog(this);
    lonlatDialog->setModal(false);
}


RenderArea::~RenderArea()
{ 
}


QSize RenderArea::sizeHint(void) const
{
    return QSize(500, 400);
}


QSize RenderArea::minimumSizeHint(void) const
{
    return QSize(240, 160);
}


void RenderArea::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.drawImage(QPoint(0, 0), mapImage);
    if (lonDiff > 0 && latDiff > 0)
    {
        painter.setRenderHint(QPainter::Antialiasing, false);
        const QPen rasterPen(Qt::black, 1.0f, Qt::DotLine);
        const QPen refPen(Qt::blue, 3);
        const QPen stdPen(Qt::green, 2);
        const QPen pointPen(Qt::transparent);
        const int x0 = plotOffset.x();
        const int y0 = plotOffset.y() + height();
        if (drawRaster)
        {
            painter.setPen(rasterPen);
            double xfac =  xs * (lonCenter - lonMin);
            double yfac = -ys * (latCenter - latMin);
            for (int x = 0; x < width(); x += width() / 10)
            {
                double xc = lonMin - ( plotOffset.x() - (double) x ) / xs;
                painter.drawLine(x, 0, x, height());
                painter.drawText(x + 4, height() - 2, QString("%1").arg(xc));
            }
            for (int y = 0; y < height(); y += height() / 10)
            {
                double yc = latMin + ( plotOffset.y() + (double) y ) / ys;
                painter.drawLine(0, y, width(), y);
                painter.drawText(4, y - 2, QString("%1").arg(yc));
            }
        }
        if (_RefTrk.points().size() > 2)
        {
            // Referenztrack zeichnen
            painter.setRenderHint(QPainter::Antialiasing, true);
            painter.setPen(refPen);
            int x1 = (int) ((_RefTrk.points().front()->longitude() - lonMin) * xs);
            int y1 = (int) ((_RefTrk.points().front()->latitude()  - latMin) * ys);
            for (TrackpointList::const_iterator i = _RefTrk.points().begin() + 1; i != _RefTrk.points().end(); ++i)
            {
                int x2 = (int) (((*i)->longitude() - lonMin) * xs);
                int y2 = (int) (((*i)->latitude()  - latMin) * ys);
                if ((*i)->latitude() == matchedLine.p1().x() && (*i)->longitude() == matchedLine.p1().y())
                    painter.setPen(Qt::black);
                else
                    painter.setPen(refPen);
                painter.drawLine(x0 + x1, y0 - y1, x0 + x2, y0 - y2);
                x1 = x2;
                y1 = y2;
            }
            const int tptSize = (int) (curScale / 2.56f);
            if (drawTrackpoints && tptSize >= 2)
            {
                painter.setRenderHint(QPainter::Antialiasing, false);
                painter.setPen(pointPen);
                painter.setBrush(Qt::darkGreen);
                const int tptOffset = tptSize / 2;
                for (TrackpointList::const_iterator i = _RefTrk.points().begin(); i != _RefTrk.points().end(); ++i)
                {
                    x1 = (int) (((*i)->longitude() - lonMin) * xs);
                    y1 = (int) (((*i)->latitude()  - latMin) * ys);
                    painter.drawRect(x0 + x1 - tptOffset, y0 - y1 - tptOffset, tptSize, tptSize);
                }
            }
        }
        if (_Trk.points().size() > 2)
        {
            // Vergleichstrack zeichnen
            painter.setPen(stdPen);
            painter.setRenderHint(QPainter::Antialiasing, true);
            int x1 = (int) ((_Trk.points().front()->longitude() - lonMin) * xs);
            int y1 = (int) ((_Trk.points().front()->latitude()  - latMin) * ys);
            for (TrackpointList::const_iterator i = _Trk.points().begin() + 1; i != _Trk.points().end(); ++i)
            {
                int x2 = (int) (((*i)->longitude() - lonMin) * xs);
                int y2 = (int) (((*i)->latitude()  - latMin) * ys);
                painter.drawLine(x0 + x1, y0 - y1, x0 + x2, y0 - y2);
                x1 = x2;
                y1 = y2;
            }
        }
        if (!newTrackpoint.isNull())
        {
            painter.setPen(pointPen);
            painter.setBrush(Qt::darkRed);
            painter.setRenderHint(QPainter::Antialiasing, false);
            painter.drawRect(newTrackpoint.x() - 3, newTrackpoint.y() - 3, 6, 6);
        }
    }
    if (drawCenter)
    {
        painter.setRenderHint(QPainter::Antialiasing, false);
        const QPen centerPen(Qt::darkGray, 1.0f, Qt::DashLine);
        const int x0 = width() / 2;
        const int y0 = height() / 2;
        painter.setPen(centerPen);
        painter.drawLine(x0 - 20, y0, x0 + 20, y0);
        painter.drawLine(x0, y0 - 20, x0, y0 + 20);
    }
}


void RenderArea::setTrack(const GPS::Track& trk)
{
    _Trk = trk;
    calcBounds();
    update();
}


void RenderArea::setReferenceTrack(const GPS::Track& trk)
{
    if (!trk.isEmpty())
    {
        _RefTrk = trk;
        calcBounds();
        pan(QPointF());
    }
    else
    {
        _RefTrk.clear();
        reset();
    }
    update();
}


void RenderArea::calcScale(void)
{
    if (lonDiff > 0 && latDiff > 0)
    {
        xs = (double) width()  / lonDiff * curScale;
        ys = (double) height() / latDiff * curScale;
    }
}


void RenderArea::calcBounds(void)
{
    lonMin = +1e30;
    latMin = +1e30;
    lonMax = -1e30;
    latMax = -1e30;
    for (GPS::TrackpointList::const_iterator i = _Trk.points().begin(); i != _Trk.points().end(); ++i)
    {
        if ((*i)->latitude()  < latMin) latMin = (*i)->latitude();
        if ((*i)->latitude()  > latMax) latMax = (*i)->latitude();
        if ((*i)->longitude() < lonMin) lonMin = (*i)->longitude();
        if ((*i)->longitude() > lonMax) lonMax = (*i)->longitude();
    }
    for (GPS::TrackpointList::const_iterator i = _RefTrk.points().begin(); i != _RefTrk.points().end(); ++i)
    {
        if ((*i)->latitude()  < latMin) latMin = (*i)->latitude();
        if ((*i)->latitude()  > latMax) latMax = (*i)->latitude();
        if ((*i)->longitude() < lonMin) lonMin = (*i)->longitude();
        if ((*i)->longitude() > lonMax) lonMax = (*i)->longitude();
    }
    lonDiff = lonMax - lonMin;
    latDiff = latMax - latMin;
    calcScale();
}


void RenderArea::wheelEvent(QWheelEvent* event)
{
    double decelerator = (event->modifiers() & Qt::AltModifier)? 640 : 160;
    if (event->modifiers() & Qt::ControlModifier) 
        decelerator *= 2;
    double numSteps = (double) event->delta() / decelerator;
    zoom(pow(ZoomOutFactor, numSteps));
}


void RenderArea::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton && !pointing && calibrationPhase == 0)
    {
        lastDragPos = event->pos();
        currentCursor = Qt::ClosedHandCursor;
        setCursor(currentCursor);
    }
}


void RenderArea::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        if (calibrationPhase > 0)
        {
            if (calibrationPhase < 5) 
            {
                lonlatPolygon << QPointF(lonlatDialog->longitude(), lonlatDialog->latitude());
                xyPolygon << QPointF(event->x(), event->y());
                qDebug() << "lonlatPolygon = " << lonlatPolygon;
                qDebug() << "xyPolygon = " << xyPolygon;
                if (calibrationPhase < 4)
                {
                    ++calibrationPhase;
                    lonlatDialog->setCalibrationPhase(calibrationPhase);
                }
                else
                {
                    QTransform::quadToQuad(lonlatPolygon, xyPolygon, mapTrans);
                    qDebug() << mapTrans;
                    calibrationPhase = 0;
                }
            }
        }
        else if (!pointing)
        {
            plotOffset += event->pos() - lastDragPos;
            lastDragPos = QPoint();
            update();
            currentCursor = Qt::OpenHandCursor;
            setCursor(currentCursor);
        }
        else
        {
            pointing = false;
            calcBounds();
            calcScale();
            update();
        }
    }
}


void RenderArea::mouseMoveEvent(QMouseEvent* event)
{
    if (lonDiff > 0.0 && latDiff > 0.0)
    {
        if (!_RefTrk.isEmpty())
        {
            int x = plotOffset.x() - event->x();
            int y = plotOffset.y() - event->y() + height();
            lon = lonMin - (double) x / xs;
            lat = latMin + (double) y / ys;
        }

        if ((event->buttons() & Qt::LeftButton) && inserting)
        {
            // TODO: Einfügen eines Punktes auf einer Track-Linie
        }
        else if ((event->buttons() & Qt::LeftButton) && pointing)
        {
            if (grabbedTrackpoint != NULL)
            {
                grabbedTrackpoint->setLatitude(lat);
                grabbedTrackpoint->setLongitude(lon);
                trackHasChanged = true;
                update();
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
            grabbedTrackpoint = _RefTrk.getMatchingTrackpoint(lat, lon, 7.5);
            if (grabbedTrackpoint != NULL) 
                undoableTrackpoint = *grabbedTrackpoint;
            matchedLine = _RefTrk.getMatchingLine(lat, lon);
        }

        if (calibrationPhase == 0)
        {
            if (grabbedTrackpoint == NULL)
            {
                setCursor(currentCursor);
                pointing = false;
            }
            else
            {
                setCursor(Qt::PointingHandCursor);
                pointing = true;
            }
        }

        emit latlonChanged(lat, lon);
    }
}


void RenderArea::pan(const QPointF& offset)
{
    if (lonDiff > 0.0 && latDiff > 0.0)
    {
        plotOffset += offset;
        lonCenter = lonMin - (plotOffset.x() - width()  / 2 ) / xs;
        latCenter = latMin + (plotOffset.y() + height() / 2 ) / ys;
        update();
        emit latlonMChanged(latCenter, lonCenter);
    }
}


void RenderArea::zoom(double zoomFactor)
{
    if (lonDiff > 0 && latDiff > 0)
    {
        curScale *= zoomFactor;
        double xfac =  xs * (lonCenter - lonMin);
        double yfac = -ys * (latCenter - latMin);
        QPointF panOffset = (1 - zoomFactor) * QPointF(xfac, yfac);
        calcScale();
        pan(panOffset);
    }
}


void RenderArea::reset(void)
{
    curScale = 1.0;
    plotOffset = QPoint();
    calcScale();
    update();
}


void RenderArea::keyPressEvent(QKeyEvent* event)
{
    switch (event->key())
    {
    case Qt::Key_Right:
        pan(QPointF(-PanSkip, 0));
        break;
    case Qt::Key_Left:
        pan(QPointF(+PanSkip, 0));
        break;
    case Qt::Key_Up:
        pan(QPointF(0, +PanSkip));
        break;
    case Qt::Key_Down:
        pan(QPointF(0, -PanSkip));
        break;
    case Qt::Key_Plus:
        zoom(ZoomInFactor);
        break;
    case Qt::Key_Minus:
        zoom(ZoomOutFactor);
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
        if (grabbedTrackpoint != NULL) {
            TrackpointList::iterator trkpt = _RefTrk.getMatchingTrackpointIter(lat, lon, 7.5);
            if (trkpt != _RefTrk.points().end()) {
                _RefTrk.erase(trkpt);
                delete grabbedTrackpoint;
                grabbedTrackpoint = NULL;
            }
        }
        update();
        break;
    default:
        QWidget::keyPressEvent(event);
        break;
    }
}


void RenderArea::resizeEvent(QResizeEvent* /* event */)
{
    calcScale();
    update();
}
