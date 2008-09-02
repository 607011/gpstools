// $Id: TrackStatisticsWindow.cpp bd86a16444b7 2008/08/27 15:09:21 Oliver Lau <ola@ct.heise.de> $
// Copyright (c) 2008 Oliver Lau <oliver@ersatzworld.net>
// Alle Rechte vorbehalten.

#include <QFileDialog>
#include <QFileInfo>

#include "TrackStatisticsWindow.h"


TrackStatisticsWindow::TrackStatisticsWindow(QWidget* parent) : QWidget(parent)
{
    refFile = NULL;
    myFile = NULL;

    ui.setupUi(this);

    refTrackTable = new GPXFileTableModel(this);
    refTrackTableSelections = new QItemSelectionModel(refTrackTable);
    ui.refTrackFileTableView->setModel(refTrackTable);
    ui.refTrackFileTableView->hideColumn(4);
    ui.refTrackFileTableView->setSelectionModel(refTrackTableSelections);

    connect(refTrackTableSelections,
            SIGNAL(currentChanged(const QModelIndex&, const QModelIndex&)),
            this,
            SLOT(refTrackTableSelectionChanged(const QModelIndex&, const QModelIndex&)));

    myTrackTable = new GPXFileTableModel(this);
    myTrackTableSelections = new QItemSelectionModel(myTrackTable);
    ui.myTrackFileTableView->setModel(myTrackTable);
    ui.myTrackFileTableView->hideColumn(4);
    ui.myTrackFileTableView->setSelectionModel(myTrackTableSelections);
    connect(myTrackTableSelections,
            SIGNAL(currentChanged(const QModelIndex&, const QModelIndex&)),
            this,
            SLOT(myTrackTableSelectionChanged(const QModelIndex&, const QModelIndex&)));

#ifdef _DEBUG
    addTrackFileEntry(refTrackTable, "../test/compare-4.gpx");
    addTrackFileEntry(refTrackTable, "../test/compare-3.gpx");
    addTrackFileEntry(refTrackTable, "../test/compare-2.gpx");
    addTrackFileEntry(refTrackTable, "../test/compare-1.gpx");
    addTrackFileEntry(refTrackTable, "../test/compare-0.gpx");
    addTrackFileEntry(refTrackTable, "../test/20080621-STUNT100-Runde1.gpx");
    addTrackFileEntry(refTrackTable, "../test/20080106-Gipfelstutzer.gpx");
#endif // _DEBUG
}


TrackStatisticsWindow::~TrackStatisticsWindow()
{
    if (refTrackTableSelections != NULL) {
        delete refTrackTableSelections;
        refTrackTableSelections = NULL;
    }
    if (myTrackTableSelections != NULL) {
        delete myTrackTableSelections;
        myTrackTableSelections = NULL;
    }
    if (refTrackTable != NULL) {
        delete refTrackTable;
        refTrackTable = NULL;
    }
}


void TrackStatisticsWindow::addTrackFileEntry(GPXFileTableModel* table, const QString& filename)
{
    table->insertRow(0);
    TrackFileData* data = new TrackFileData(filename);
    QModelIndex index = table->index(0, 0);
    table->setData(index, data->name);
    index = table->index(0, 1);
    table->setData(index, data->filename);
    index = table->index(0, 2);
    table->setData(index, data->startTime);
    index = table->index(0, 3);
    table->setData(index, data->finishTime);
    index = table->index(0, 4);
    table->setData(index, data->gpxFile);
}


void TrackStatisticsWindow::refTrackTableSelectionChanged(const QModelIndex& curr, const QModelIndex&)
{
    TrackFileDataList list = refTrackTable->getList();
    int row = curr.row();
    if (row >= 0 && row < list.size())
    {
        TrackFileData* d = list.at(row);
        refFile = d->gpxFile;
    }
}


void TrackStatisticsWindow::myTrackTableSelectionChanged(const QModelIndex& curr, const QModelIndex& )
{
    TrackFileDataList list = myTrackTable->getList();
    int row = curr.row();
    if (row >= 0 && row < list.size())
    {
        TrackFileData* d = list.at(row);
        myFile = d->gpxFile;
    }
}


void TrackStatisticsWindow::on_addRefTrkToolButton_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this,
        tr("GPX-Dateien finden"), lastTrackFilePath, tr("GPS-Dateien (*.gpx)"));
    if (!filename.isEmpty()) {
        QFileInfo inf(filename);
        lastTrackFilePath = inf.path();
        addTrackFileEntry(refTrackTable, filename);
    }
}


void TrackStatisticsWindow::on_delRefTrkToolButton_clicked()
{
    // ...
}


