// $Id: CalibrationRenderArea.cpp b1c70e73719f 2008/08/28 15:26:05 Oliver Lau <ola@ct.heise.de> $
// Copyright (c) 2008 Oliver Lau <oliver@ersatzworld.net>
// Alle Rechte vorbehalten.

#include <QtGlobal>
#include <QPainter>
#include <QBrush>
#include <QPen>
#include <QMessageBox>
#include <QSettings>


#ifdef _DEBUG
#include <QtDebug>
#endif

#include <cmath>

#include "CalibrationRenderArea.h"

using namespace GPS;

static const double ZoomInFactor = 1.16;
static const double ZoomOutFactor = 1.0 / ZoomInFactor;
static const int PanSkip = 16;

CalibrationRenderArea::CalibrationRenderArea(QWidget* parent) : QWidget(parent)
{
    _Colors << new QColor(77, 212, 65)
        << new QColor(250, 70, 70)
        << new QColor(70, 130, 220)
        << new QColor(255, 165, 0)
        << new QColor(148, 0, 211)
        << new QColor(255, 182, 193)
        << new QColor(255, 215, 0)
        << new QColor(192, 192, 192)
        << new QColor(216, 191, 216)
        << new QColor(64, 224, 208);
    QColor* color;
    QRadialGradient gradient(QPointF(0.5, 0.5), 0.5, QPointF(0.25, 0.25));
    gradient.setCoordinateMode(QGradient::ObjectBoundingMode);
    gradient.setColorAt(0.0, Qt::white);
    foreach(color, _Colors) {
        gradient.setColorAt(0.8, *color);
        _Brushes << new QBrush(gradient);
    }
    calibration = NULL;
    calibrationPhase = 0;
    curScale = 1.0;
    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);
    setMouseTracking(true);
    setFocusPolicy(Qt::ClickFocus);
    setFocus(Qt::MouseFocusReason);
    currentCursor = Qt::CrossCursor;
    setCursor(currentCursor);
    pan(QPointF());
}


CalibrationRenderArea::~CalibrationRenderArea()
{
    // ...
}


QSize CalibrationRenderArea::sizeHint(void) const
{
    return QSize(400, 300);
}


QSize CalibrationRenderArea::minimumSizeHint(void) const
{
    return QSize(240, 160);
}


void CalibrationRenderArea::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
#ifdef _DEBUG
    painter.save();
#endif // _DEBUG
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.translate(plotOffset);
    painter.scale(curScale, curScale);
    QRect imgRect(0, 0, _MapPixmap.width(), _MapPixmap.height());
    painter.drawPixmap(imgRect, _MapPixmap);
    CalibrationPointBrushList::const_iterator c = _Brushes.begin();
    painter.setPen(Qt::black);
    const double r = 10.0;
    for (QPolygonF::const_iterator i = _XYPolygon.begin(); i != _XYPolygon.end(); ++i)
    {
        painter.setBrush(*(*c));
#if QT_VERSION >= 0x040400
        QPointF center(i->x(), i->y());
        painter.drawEllipse(center, r / curScale, r / curScale);
#else
        painter.drawEllipse(i->x() - r / curScale, i->y() - r  / curScale, r * 2 / curScale, r * 2 / curScale);
#endif
        ++c;
    }
#ifdef _DEBUG
    painter.restore();
    const QPen centerPen(Qt::darkGray, 1.0, Qt::DashLine);
    const int x0 = width() / 2;
    const int y0 = height() / 2;
    painter.setPen(centerPen);
    painter.drawLine(x0 - 20, y0, x0 + 20, y0);
    painter.drawLine(x0, y0 - 20, x0, y0 + 20);
#endif // _DEBUG
}


void CalibrationRenderArea::wheelEvent(QWheelEvent* event)
{
    double decelerator = (event->modifiers() & Qt::AltModifier)? 640 : 160;
    if (event->modifiers() & Qt::ControlModifier)
        decelerator *= 2;
    double numSteps = (double) event->delta() / decelerator;
    zoom(pow(ZoomOutFactor, numSteps), event->pos());
}


void CalibrationRenderArea::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
        lastDragPos = event->pos();
}


void CalibrationRenderArea::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::RightButton)
    {
        if (calibrationPhase < 4)
        {
            ++calibrationPhase;
            QPointF newp((event->x() - plotOffset.x()) / curScale, (event->y() - plotOffset.y()) / curScale);
            emit newCalibrationPoint(newp.x(), newp.y());
            _XYPolygon << newp;
            update();
            if (calibrationPhase == 4)
            {
                if (calibration != NULL)
                    calibration->setXYPolygon(_XYPolygon);
                emit calibrationReady();
            }
        }
    }
    else if (event->button() == Qt::LeftButton)
    {
        plotOffset += event->pos() - lastDragPos;
        lastDragPos = QPoint();
        update();
    }
}


void CalibrationRenderArea::mouseMoveEvent(QMouseEvent* event)
{
    if (event->buttons() & Qt::LeftButton)
    {
        QPoint panOffset = event->pos() - lastDragPos;
        lastDragPos = event->pos();
        pan(panOffset);
    }
}


void CalibrationRenderArea::pan(const QPointF& offset)
{
    plotOffset += offset;
    xCenter = - (plotOffset.x() - width()  / 2) / curScale;
    yCenter = - (plotOffset.y() - height() / 2) / curScale;
    update();
}


void CalibrationRenderArea::zoom(double zoomFactor, const QPoint& pos)
{
    curScale *= zoomFactor;
    QPointF panOffset = (!pos.isNull())? QPointF(pos) : QPointF(width()/2, height()/2);
    pan((1 - zoomFactor) * (panOffset - plotOffset));
}


void CalibrationRenderArea::reset(void)
{
    curScale = 1.0;
    plotOffset = QPoint();
    update();
}


void CalibrationRenderArea::keyPressEvent(QKeyEvent* event)
{
    switch (event->key())
    {
    case Qt::Key_Right:
        pan(QPoint(-PanSkip, 0));
        update();
        break;
    case Qt::Key_Left:
        pan(QPoint(+PanSkip, 0));
        update();
        break;
    case Qt::Key_Up:
        pan(QPoint(0, +PanSkip));
        update();
        break;
    case Qt::Key_Down:
        pan(QPoint(0, -PanSkip));
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
    case Qt::Key_Escape:
        break;
    case Qt::Key_Backspace:
        reset();
        update();
        break;
    case Qt::Key_Delete:
        break;
    default:
        QWidget::keyPressEvent(event);
        break;
    }
}


void CalibrationRenderArea::resizeEvent(QResizeEvent* /* event */)
{
    update();
}


void CalibrationRenderArea::loadMap(const QString& filename, CalibrationIniFile* cal)
{
    bool loadSucceeded = _MapPixmap.load(filename);
    if (loadSucceeded)
    {
        plotOffset = QPoint();
        curScale = qMin<double>((double) width() / (double) _MapPixmap.width(), (double) height() / (double) _MapPixmap.height());
        if (cal != NULL)
        {
            _XYPolygon = cal->xyPolygon();
            calibrationPhase = _XYPolygon.size();
        }
        update();
    }
    else {
        QMessageBox::warning(this, tr("Error loading the map."), tr("The map in '%1' could not be loaded.").arg(filename));
    }
}


void CalibrationRenderArea::clearCalibrationPoints(void)
{
    _XYPolygon.clear();
    calibrationPhase = 0;
    update();
}
