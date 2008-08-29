// $Id: LonLatTableModel.h bd86a16444b7 2008/08/27 15:09:21 Oliver Lau <ola@ct.heise.de> $
// Copyright (c) 2008 Oliver Lau <oliver@ersatzworld.net>
// Alle Rechte vorbehalten.

#ifndef __LATLONTABLEMODEL_H_
#define __LATLONTABLEMODEL_H_


#include <QAbstractTableModel>
#include <QVector>
#include <QStringList>
#include <QColor>

#ifdef _DEBUG
#include <QtDebug>
#endif

struct LonLatData {
    LonLatData() : lat(0.0), lon(0.0), x(0.0), y(0.0) { /* ... */ }
    QColor color;
    double lat;
    double lon;
    double x;
    double y;
};


typedef QVector<LonLatData*> LonLatDataList;


class LonLatTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    LonLatTableModel(QObject* parent = 0);
    LonLatTableModel(const LonLatDataList& lonLatList, QObject* parent = 0);
    int rowCount(const QModelIndex& parent = QModelIndex()) const;
    int columnCount(const QModelIndex& parent = QModelIndex()) const;
    QVariant data(const QModelIndex& index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    Qt::ItemFlags flags(const QModelIndex& index) const;
    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);
    bool insertRows(int position, int rows, const QModelIndex& index = QModelIndex());
    bool removeRows(int position, int rows, const QModelIndex& index = QModelIndex());

private:
    LonLatDataList _LonLatList;
    QStringList _Headers;

private:
    void init(void);
};


#endif // __LATLONTABLEMODEL_H_
