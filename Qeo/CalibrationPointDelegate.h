// $Id$
// Copyright (c) 2008 Oliver Lau <oliver@ersatzworld.net>
// Alle Rechte vorbehalten.

#include <QAbstractItemDelegate>
#include <QPainter>
#include <QSize>
#include <QModelIndex>
#include <QStyleOptionViewItem>


class CalibrationPointDelegate : public QAbstractItemDelegate {
    Q_OBJECT

public:
    CalibrationPointDelegate(QObject* parent = 0);
    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const;
    QSize sizeHint(const QStyleOptionViewItem &option,
                   const QModelIndex &index ) const;

private:
    int pixelSize;
};
