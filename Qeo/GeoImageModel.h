// $Id$
// Copyright (c) 2008 Oliver Lau <oliver@ersatzworld.net>
// Alle Rechte vorbehalten.

#ifndef __GEOIMAGEMODEL_H_
#define __GEOIMAGEMODEL_H_

#include <QAbstractListModel>
#include <QList>

class GeoImage;

class GeoImageModel : public QAbstractListModel
{
    Q_OBJECT

public:
    GeoImageModel(QObject *parent);
    ~GeoImageModel();

    void insert(const GeoImage* item);
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    int rowCount(const QModelIndex& parent = QModelIndex()) const;
    const GeoImage* data(int index) const;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

private:
    QList<const GeoImage*> images;
};

#endif // __GEOIMAGEMODEL_H_
