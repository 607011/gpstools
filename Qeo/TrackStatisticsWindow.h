// $Id$
// Copyright (c) 2008 Oliver Lau <oliver@ersatzworld.net>
// Alle Rechte vorbehalten.

#ifndef __TRACKSTATISTICSWINDOW_H_
#define __TRACKSTATISTICSWINDOW_H_

#include <QHeaderView>

#include "ui_TrackStatisticsWindow.h"

#include "gpslib/GPXFile.h"

#include "GPXFileTableModel.h"


class TrackStatisticsWindow : public QWidget
{
    Q_OBJECT
public:
    TrackStatisticsWindow(QWidget* parent = 0);
    ~TrackStatisticsWindow();

private:
    Ui::TrackStatisticsWindowClass ui;
    GPS::GPXFile* refFile;
    GPS::GPXFile* myFile;
    GPXFileTableModel* refTrackTable;
    GPXFileTableModel* myTrackTable;
    QItemSelectionModel* refTrackTableSelections;
    QItemSelectionModel* myTrackTableSelections;

    QString lastTrackFilePath;

private:
    void addTrackFileEntry(GPXFileTableModel* table, const QString& filename);

private slots:
    void on_addRefTrkToolButton_clicked(void);
    void on_delRefTrkToolButton_clicked(void);
    void refTrackTableSelectionChanged(const QModelIndex&, const QModelIndex&);
    void myTrackTableSelectionChanged(const QModelIndex&, const QModelIndex&);

};



#endif // __TRACKSTATISTICSWINDOW_H_
