// $Id$
// Copyright (c) 2008 Oliver Lau <oliver@ersatzworld.net>
// Alle Rechte vorbehalten.

#ifndef __TRACKTABLEMODEL_H_
#define __TRACKTABLEMODEL_H_


#include <QAbstractTableModel>
#include <QVector>
#include <QStringList>

#include "gpslib/Track.h"

#ifdef _DEBUG
#include <QtDebug>
#endif


typedef QVector<GPS::Track*> QTrackList;


class TrackTableModel : public QAbstractTableModel
{
    Q_OBJECT
        
public:
    TrackTableModel(QObject* parent = 0);
    TrackTableModel(const QTrackList& trackList, QObject* parent = 0);
    int rowCount(const QModelIndex& parent = QModelIndex()) const;
    int columnCount(const QModelIndex& parent = QModelIndex()) const;
    QVariant data(const QModelIndex& index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    Qt::ItemFlags flags(const QModelIndex& index) const;
    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);
    bool setData(const QModelIndex& index, GPS::Track* t, int role = Qt::EditRole);
    bool insertRows(int position, int rows, const QModelIndex& index = QModelIndex());
    bool removeRows(int position, int rows, const QModelIndex& index = QModelIndex());
    QTrackList getList(void) const { return _TrackList; }

private:
    QTrackList _TrackList;
    QStringList _Headers;

private:
    void init(void);
};


#endif // __TRACKTABLEMODEL_H_
