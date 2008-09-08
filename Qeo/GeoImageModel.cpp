// $Id$
// Copyright (c) 2008 Oliver Lau <oliver@ersatzworld.net>
// Alle Rechte vorbehalten.

#include "GeoImageModel.h"
#include <QStringList>


GeoImageModel::GeoImageModel(QObject *parent)
    : QAbstractListModel(parent)
{
    // ...
}


GeoImageModel::~GeoImageModel()
{
    // ...
}


void GeoImageModel::insert ( const GeoImage* item )
{
    beginInsertRows(QModelIndex(), images.count(), images.count() + 1 );
    images.append(item);
    endInsertRows();
}


QVariant GeoImageModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
    {
        return QVariant();
    }
    if (orientation == Qt::Horizontal)
    {
        return QString("Column %1").arg(section);
    }
    else{
        return QString("Image %1").arg(section);
    }
}


int GeoImageModel::rowCount(const QModelIndex&) const
{
    return images.count();
}


QVariant GeoImageModel::data(const QModelIndex & index, int role) const
{
    if (!index.isValid())
    {
        return QVariant();
    }
    if (index.row() < 0 || index.row() >= images.size())
    {
        return QVariant();
    }
    if (role == Qt::DisplayRole)
    {
        return images.at(index.row())->name();
    }
    else
    {
        return QVariant();
    }
}


const GeoImageModel* GeoImageModel::data(int index) const
{
    Q_ASSERT(index >=0 && index < images.size());
    return images.at(index);
}
