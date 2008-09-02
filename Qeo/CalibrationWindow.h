// $Id$
// Copyright (c) 2008 Oliver Lau <oliver@ersatzworld.net>
// Alle Rechte vorbehalten.

#ifndef __CALIBRATIONWINDOW_H_
#define __CALIBRATIONWINDOW_H_

#include <QHeaderView>

#include "ui_CalibrationWindow.h"

#include "gpslib/GPXFile.h"
#include "GPXFileTableModel.h"
#include "LonLatTableModel.h"


class CalibrationWindow : public QWidget
{
    Q_OBJECT

public:
    CalibrationWindow(QWidget* parent = 0);
    ~CalibrationWindow();
    void addCalibrationPoint(LonLatTableModel* table, int x, int y);
    void loadMap(const QString& filename);
    inline const QString& lastMapFilePath(void) const { return _LastMapFilePath; }
    inline void setLastMapFilePath(QString path) { _LastMapFilePath = path; }
    inline const QTransform& mapTrans(void) const { return _MapTrans; }
    inline CalibrationIniFile* calibration(void) { return _Calibration; }

public:
    QString _LastMapFilePath;

private:
    Ui::CalibrationWindowClass ui;
    bool calibrationDataChanged;
    QString mapFilename;
    CalibrationIniFile* _Calibration;
    LonLatTableModel* lonLatTable;
    QItemSelectionModel* lonLatTableSelections;
    QTransform _MapTrans;

private:
    void prepareExit(void);

private slots:
    void lonLatTableDataChanged(const QModelIndex&, const QModelIndex&);
    void calibrationPointAdded(double, double);
    void calibrationReady(void);
    void calibrationChanged(const QPointF& topLeft, const QPointF& bottomRight);
    void on_mapFileOpenPushButton_clicked(void);
    void on_calibRestartPushButton_clicked(void);
    void on_calibCancelPushButton_clicked(void);
    void on_calibOkPushButton_clicked(void);

};



#endif // __CALIBRATIONWINDOW_H_
