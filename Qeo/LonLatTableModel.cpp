// $Id: LonLatTableModel.cpp bd86a16444b7 2008/08/27 15:09:21 Oliver Lau <ola@ct.heise.de> $
// Copyright (c) 2008 Oliver Lau <oliver@ersatzworld.net>
// Alle Rechte vorbehalten.


#include "LonLatTableModel.h"
#ifdef _DEBUG
#include <QtDebug>
#endif


LonLatTableModel::LonLatTableModel(QObject* parent)
    : QAbstractTableModel(parent)
{
    init();
}


LonLatTableModel::LonLatTableModel(const LonLatDataList& lonLatList, QObject* parent)
    : QAbstractTableModel(parent), _LonLatList(lonLatList)
{
    init();
}


void LonLatTableModel::init(void)
{
    _Headers << tr("") << tr("Längengrad") << tr("Breitengrad") << tr("x") << tr("y");
}


int LonLatTableModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return _LonLatList.count();
}


int LonLatTableModel::columnCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return _Headers.count();
}


bool LonLatTableModel::insertRows(int position, int rows, const QModelIndex& index)
{
    Q_UNUSED(index);
    beginInsertRows(QModelIndex(), position, position+rows-1);
    for (int row=0; row < rows; row++) {
        _LonLatList.insert(position, new LonLatData);
    }
    endInsertRows();
    return true;
}


bool LonLatTableModel::removeRows(int position, int rows, const QModelIndex& index)
{
    Q_UNUSED(index);
    beginRemoveRows(QModelIndex(), position, position+rows-1);
    for (int row=0; row < rows; ++row) {
        LonLatData* d = _LonLatList.at(position);
        delete d;
        _LonLatList.remove(position);
    }
    endRemoveRows();
    return true;
}



QVariant LonLatTableModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
         return QVariant();

    if (index.row() >= rowCount())
        return QVariant();

    if (role == Qt::DisplayRole)
    {
        LonLatData* d = _LonLatList.at(index.row());
        switch (index.column())
        {
        case 0:
            return d->color;
            break;
        case 1:
            return d->lat;
            break;
        case 2:
            return d->lon;
            break;
        case 3:
            return d->x;
            break;
        case 4:
            return d->y;
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


bool LonLatTableModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (index.isValid() && role == Qt::EditRole)
    {
        int row = index.row();
        switch (index.column())
        {
        case 0:
            _LonLatList[row]->color = value.value<QColor>();
            break;
        case 1:
            _LonLatList[row]->lat = value.toDouble();
            break;
        case 2:
            _LonLatList[row]->lon = value.toDouble();
            break;
        case 3:
            _LonLatList[row]->x = value.toDouble();
            break;
        case 4:
            _LonLatList[row]->y = value.toDouble();
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


QVariant LonLatTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal)
    {
        return (section < 0 || section > _Headers.count())
            ? QVariant()
            : _Headers.at(section);
    }

    return QVariant();
}


Qt::ItemFlags LonLatTableModel::flags(const QModelIndex& index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;
    switch (index.column()) 
    {
    case 0:
        return 0;
    case 1:
        // fall-through
    case 2:
        return QAbstractTableModel::flags(index) | Qt::ItemIsSelectable | Qt::ItemIsEditable;
    default:
        break;
    }
    return 0;
}
