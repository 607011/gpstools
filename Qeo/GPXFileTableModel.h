// $Id$
// Copyright (c) 2008 Oliver Lau <oliver@ersatzworld.net>
// Alle Rechte vorbehalten.

#ifndef __GPXFILETABLEMODEL_H_
#define __GPXFILETABLEMODEL_H_


#include <QAbstractTableModel>
#include <QVector>
#include <QStringList>

#include "gpslib/GPXFile.h"
#include "gpslib/Track.h"

#ifdef _DEBUG
#include <QtDebug>
#endif

struct TrackFileData {
    TrackFileData(void) : gpxFile(NULL)
    { 
        // ...
    }
    TrackFileData(const QString& _filename)
    {
        gpxFile = new GPS::GPXFile;
        errno_t rc = gpxFile->load(_filename.toStdString());
        if (rc == 0)
        {
            name = gpxFile->track()->name().c_str();
            filename = gpxFile->filename().c_str();
            startTime = gpxFile->track()->startTimestamp().toString().c_str();
            finishTime = gpxFile->track()->finishTimestamp().toString().c_str();
        }
        
    }
    ~TrackFileData()
    {
        if (gpxFile != NULL)
            delete gpxFile;
        gpxFile = NULL;
    }
    
    QString name;
    QString filename;
    QString startTime;
    QString finishTime;
    GPS::GPXFile* gpxFile;
};


typedef QVector<TrackFileData*> TrackFileDataList;


class GPXFileTableModel : public QAbstractTableModel
{
    Q_OBJECT
        
public:
    GPXFileTableModel(QObject* parent = 0);
    GPXFileTableModel(const TrackFileDataList& trackList, QObject* parent = 0);
    int rowCount(const QModelIndex& parent = QModelIndex()) const;
    int columnCount(const QModelIndex& parent = QModelIndex()) const;
    QVariant data(const QModelIndex& index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    Qt::ItemFlags flags(const QModelIndex& index) const;
    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);
    bool setData(const QModelIndex& index, GPS::GPXFile* f, int role = Qt::EditRole);
    bool insertRows(int position, int rows, const QModelIndex& index = QModelIndex());
    bool removeRows(int position, int rows, const QModelIndex& index = QModelIndex());
    TrackFileDataList getList(void) const { return _TrackFileList; }

private:
    TrackFileDataList _TrackFileList;
    QStringList _Headers;

private:
    void init(void);
};


#endif // __GPXFILETABLEMODEL_H_
