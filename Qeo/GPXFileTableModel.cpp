// $Id: GPXFileTableModel.cpp bd86a16444b7 2008/08/27 15:09:21 Oliver Lau <ola@ct.heise.de> $
// Copyright (c) 2008 Oliver Lau <oliver@ersatzworld.net>
// Alle Rechte vorbehalten.


#include "GPXFileTableModel.h"
#ifdef _DEBUG
#include <QtDebug>
#endif


GPXFileTableModel::GPXFileTableModel(QObject* parent)
    : QAbstractTableModel(parent)
{
    init();
}


GPXFileTableModel::GPXFileTableModel(const TrackFileDataList& trackList, QObject* parent)
    : QAbstractTableModel(parent), _TrackFileList(trackList)
{
    init();
}


void GPXFileTableModel::init(void)
{
    _Headers << tr("Name") << tr("Datei") << tr("Startzeit") << tr("Endezeit") << tr("_file");
}


int GPXFileTableModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return _TrackFileList.count();
}


int GPXFileTableModel::columnCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return _Headers.count();
}


bool GPXFileTableModel::insertRows(int position, int rows, const QModelIndex &index)
{
    Q_UNUSED(index);
    beginInsertRows(QModelIndex(), position, position+rows-1);
    for (int row=0; row < rows; row++) {
        _TrackFileList.insert(position, new TrackFileData);
    }
    endInsertRows();
    return true;
}


bool GPXFileTableModel::removeRows(int position, int rows, const QModelIndex &index)
{
    Q_UNUSED(index);
    beginRemoveRows(QModelIndex(), position, position+rows-1);
    for (int row=0; row < rows; ++row) {
        TrackFileData* d = _TrackFileList.at(position);
        delete d;
        _TrackFileList.remove(position);
    }
    endRemoveRows();
    return true;
}



QVariant GPXFileTableModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
         return QVariant();

    if (index.row() >= rowCount())
        return QVariant();

    if (role == Qt::DisplayRole)
    {
        TrackFileData* d = _TrackFileList.at(index.row());
        switch (index.column())
        {
        case 0: // Name
            return d->name;
            break;
        case 1: // Datei
            return d->filename;
            break;
        case 2: // Startzeit
            return d->startTime;
            break;
        case 3: // Endezeit
            return d->finishTime;
            break;
        default:
            return QVariant();
            break;
        }
    }
    else {
        return QVariant();
    }
}


bool GPXFileTableModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (index.isValid() && role == Qt::EditRole)
    {
        int row = index.row();
        switch (index.column())
        {
        case 0: // Name
            _TrackFileList[row]->name = value.toString();
            break;
        case 1: // Datei
            _TrackFileList[row]->filename = value.toString();
            break;
        case 2: // Startzeit
            _TrackFileList[row]->startTime = value.toString();
            break;
        case 3: // Endezeit
            _TrackFileList[row]->finishTime = value.toString();
            break;
        default:
            return false;
            break;
        }
        emit dataChanged(index, index);
        return true;
    }
    return false;
}


bool GPXFileTableModel::setData(const QModelIndex& index, GPS::GPXFile* f, int role)
{
    if (index.isValid() && role == Qt::EditRole)
    {
        int row = index.row();
        _TrackFileList[row]->gpxFile = f;
        return true;
    }
    return false;
}


QVariant GPXFileTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal) {
        return (section < 0 || section > _Headers.count())
            ? QVariant()
            : _Headers.at(section);
    }

    return QVariant();
}


Qt::ItemFlags GPXFileTableModel::flags(const QModelIndex& index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;
    switch (index.column())
    {
    case 1:
        // fall-through
    case 2:
        return Qt::ItemIsSelectable | Qt::ItemIsEditable; // QAbstractTableModel::flags(index);
    default:
        break;
    }
    return QAbstractTableModel::flags(index);
}
