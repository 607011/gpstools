// $Id$
// Copyright (c) 2008 Oliver Lau <oliver@ersatzworld.net>
// Alle Rechte vorbehalten.


#include "TrackTableModel.h"
#ifdef _DEBUG
#include <QtDebug>
#endif


TrackTableModel::TrackTableModel(QObject* parent)
    : QAbstractTableModel(parent)
{
    init();
}


TrackTableModel::TrackTableModel(const QTrackList& trackList, QObject* parent)
    : QAbstractTableModel(parent), _TrackList(trackList)
{
    init();
}


void TrackTableModel::init(void)
{
    _Headers << tr("Name") << tr("Startzeit") << tr("Endezeit");
}


int TrackTableModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return _TrackList.size();
}


int TrackTableModel::columnCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return _Headers.count();
}


bool TrackTableModel::insertRows(int position, int rows, const QModelIndex &index)
{
    Q_UNUSED(index);
    beginInsertRows(QModelIndex(), position, position+rows-1);
    for (int row=0; row < rows; row++) {
        _TrackList.insert(position, new GPS::Track);
    }
    endInsertRows();
    return true;
}


bool TrackTableModel::removeRows(int position, int rows, const QModelIndex &index)
{
    Q_UNUSED(index);
    beginRemoveRows(QModelIndex(), position, position+rows-1);
    for (int row=0; row < rows; ++row) {
        GPS::Track* t = _TrackList.at(position);
        delete t; // ???
        _TrackList.remove(position);
    }
    endRemoveRows();
    return true;
}


QVariant TrackTableModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
         return QVariant();

    if (index.row() >= rowCount())
        return QVariant();

    if (role == Qt::DisplayRole)
    {
        GPS::Track* t = _TrackList.at(index.row());
        switch (index.column())
        {
        case 0: // Name
            return t->name().c_str();
            break;
        case 1: // Startzeit
            return t->startTimestamp().toString().c_str();
            break;
        case 2: // Endezeit
            return t->finishTimestamp().toString().c_str();
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


bool TrackTableModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (index.isValid() && role == Qt::EditRole)
    {
        int row = index.row();
        switch (index.column())
        {
        case 0: // Name
            _TrackList.at(row)->setName(value.toString().toStdString());
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


bool TrackTableModel::setData(const QModelIndex& index, GPS::Track* t, int role)
{
    if (index.isValid() && role == Qt::EditRole)
    {
        int row = index.row();
        _TrackList[row] = t;
        return true;
    }
    return false;
}


QVariant TrackTableModel::headerData(int section, Qt::Orientation orientation, int role) const
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


Qt::ItemFlags TrackTableModel::flags(const QModelIndex& index) const
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
