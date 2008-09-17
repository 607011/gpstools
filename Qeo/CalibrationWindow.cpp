// $Id$
// Copyright (c) 2008 Oliver Lau <oliver@ersatzworld.net>
// Alle Rechte vorbehalten.

#include <QMessageBox>
#include <QFileDialog>
#include <QFileInfo>

#include "CalibrationIniFile.h"
#include "CalibrationWindow.h"
#include "CalibrationPointDelegate.h"
#include "CalibrationPointView.h"


CalibrationWindow::CalibrationWindow(QWidget* parent) : QWidget(parent)
{
    _Calibration = NULL;
    calibrationDataChanged = false;

    ui.setupUi(this);

    connect(ui.calibRenderArea,
            SIGNAL(newCalibrationPoint(double, double)),
            this,
            SLOT(calibrationPointAdded(double, double)));
    connect(ui.calibRenderArea,
            SIGNAL(calibrationReady()),
            this,
            SLOT(calibrationReady()));

    lonLatTable = new LonLatTableModel(this);

    connect(lonLatTable,
            SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)),
            this,
            SLOT(lonLatTableDataChanged(const QModelIndex&, const QModelIndex&)));

    ui.lonLatTableView->setModel(lonLatTable);
    ui.lonLatTableView->hideColumn(3);
    ui.lonLatTableView->hideColumn(4);
    CalibrationPointDelegate* delegate = new CalibrationPointDelegate(this);
    ui.lonLatTableView->setItemDelegateForColumn(0, delegate);
    ui.lonLatTableView->verticalHeader()->resizeSections(QHeaderView::ResizeToContents);
    ui.lonLatTableView->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);
    lonLatTableSelections = new QItemSelectionModel(lonLatTable);
    ui.lonLatTableView->setSelectionModel(lonLatTableSelections);

#ifdef _DEBUG
    loadMap("maps/Brocken-300dpi.jpg");
#endif // _DEBUG
}

CalibrationWindow::~CalibrationWindow()
{
}

void CalibrationWindow::lonLatTableDataChanged(const QModelIndex&, const QModelIndex&)
{
    calibrationDataChanged = true;
}


void CalibrationWindow::calibrationChanged(const QPointF& topLeft, const QPointF& bottomRight)
{
    ui.leftLineEdit->setText(QString("%1").arg(topLeft.x(), 0, 'f', 10));
    ui.topLineEdit->setText(QString("%1").arg(topLeft.y(), 0, 'f', 10));
    ui.rightLineEdit->setText(QString("%1").arg(bottomRight.x(), 0, 'f', 10));
    ui.bottomLineEdit->setText(QString("%1").arg(bottomRight.y(), 0, 'f', 10));
}


void CalibrationWindow::calibrationPointAdded(double x, double y)
{
    if (lonLatTable->rowCount() < 4)
    {
        int row = lonLatTable->rowCount();
        lonLatTable->insertRow(lonLatTable->rowCount());
        QModelIndex index = lonLatTable->index(row, 0);
        QColor* color = ui.calibRenderArea->colors().at(row);
        lonLatTable->setData(index, *color);
        index = lonLatTable->index(row, 3);
        lonLatTable->setData(index, x);
        index = lonLatTable->index(row, 4);
        lonLatTable->setData(index, y);
        ui.lonLatTableView->verticalHeader()->resizeSections(QHeaderView::ResizeToContents);
        ui.lonLatTableView->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);
    }
}


void CalibrationWindow::calibrationReady(void)
{
    QPolygonF lonlatPolygon;
    for (int i = 0; i < lonLatTable->rowCount(); ++i)
    {
        QModelIndex index = lonLatTable->index(i, 1);
        double lon = lonLatTable->data(index, Qt::DisplayRole).toDouble();
        index = lonLatTable->index(i, 2);
        double lat = lonLatTable->data(index, Qt::DisplayRole).toDouble();
        lonlatPolygon << QPointF(lon, lat);
    }
}


void CalibrationWindow::prepareExit(void)
{
#ifndef _DEBUG
    if (calibrationDataChanged)
    {
        int rc = QMessageBox::question(this, tr("Beenden?"), tr("Die Kalibrierungsdaten haben sich verändert. Wollen Sie die Anwendung ohne speichern beenden?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
        if (rc == QMessageBox::Yes)
            close();
        else if (_Calibration != NULL)
            _Calibration->save();
    }
#endif // !_DEBUG
}



void CalibrationWindow::on_mapFileOpenPushButton_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this,
        tr("Find graphics files"), _LastMapFilePath, tr("Graphics files (*.bmp *.gif *.jpeg *.jpg *.pbm *.ppm *.png *.tif *.xbm *.xpm)"));
    if (!filename.isEmpty())
    {
        QFileInfo inf(filename);
        _LastMapFilePath = inf.path();
        ui.mapFileLineEdit->setText(inf.fileName());
        loadMap(filename);
    }
}


void CalibrationWindow::loadMap(const QString& filename)
{
    mapFilename = filename;
    ui.mapFileLineEdit->setText(filename);
    if (_Calibration != NULL)
        delete _Calibration;
    _Calibration = new CalibrationIniFile(filename + ".ini");
    ui.calibRenderArea->loadMap(filename, _Calibration);
    for (QPolygonF::const_iterator i = _Calibration->xyPolygon().begin(); i != _Calibration->xyPolygon().end(); ++i)
        calibrationPointAdded((*i).x(), (*i).y());
    int idx = 0;
    for (QPolygonF::const_iterator i = _Calibration->lonLatPolygon().begin(); i != _Calibration->lonLatPolygon().end(); ++i)
    {
        QModelIndex index = lonLatTable->index(idx, 1);
        lonLatTable->setData(index, (*i).x());
        index = lonLatTable->index(idx, 2);
        lonLatTable->setData(index, (*i).y());
        ++idx;
    }
    QPointF topLeft(_Calibration->left(), _Calibration->top());
    QPointF bottomRight(_Calibration->right(), _Calibration->bottom());
    calibrationChanged(topLeft, bottomRight);
}


void CalibrationWindow::on_calibOkPushButton_clicked()
{
    QPolygonF lonlatPolygon;
    for (int i = 0; i < lonLatTable->rowCount(); ++i)
    {
        QModelIndex index = lonLatTable->index(i, 1);
        double lon = lonLatTable->data(index, Qt::DisplayRole).toDouble();
        index = lonLatTable->index(i, 2);
        double lat = lonLatTable->data(index, Qt::DisplayRole).toDouble();
        lonlatPolygon << QPointF(lon, lat);
    }
    QPolygonF xyPolygon = ui.calibRenderArea->xyPolygon();
    QTransform::quadToQuad(xyPolygon, lonlatPolygon, _MapTrans);
    double w = ui.calibRenderArea->mapPixmap().width();
    double h = ui.calibRenderArea->mapPixmap().height();
    QPointF topLeft = _MapTrans.map(QPointF(0, 0));
    QPointF bottomRight = _MapTrans.map(QPointF(w, h));
    if (_Calibration != NULL)
        delete _Calibration;
    _Calibration = new CalibrationIniFile(mapFilename + ".ini");
    _Calibration->setLeft(topLeft.x());
    _Calibration->setTop(topLeft.y());
    _Calibration->setRight(bottomRight.x());
    _Calibration->setBottom(bottomRight.y());
    _Calibration->setLonLatPolygon(lonlatPolygon);
    _Calibration->setXYPolygon(xyPolygon);
    _Calibration->save();
    calibrationChanged(topLeft, bottomRight);
    QMessageBox::information(this, tr("Calibration data saved"), tr("Saved calibration data for map \"%1\".").arg(mapFilename));
}


void CalibrationWindow::on_calibCancelPushButton_clicked()
{
    // ...
}


void CalibrationWindow::on_calibRestartPushButton_clicked()
{
    if (lonLatTable->rowCount() > 0) 
    {
        int rc = QMessageBox::question(this, tr("Cancel calibration?"), tr("Do you really want to dismiss the calibration data?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
        if (rc == QMessageBox::Yes)
        {
            ui.calibRenderArea->clearCalibrationPoints();
            lonLatTable->removeRows(0, lonLatTable->rowCount());
            _MapTrans.reset();
        }
    }
}
