// $Id$
// Copyright (c) 2008 Oliver Lau <oliver@ersatzworld.net>
// Alle Rechte vorbehalten.


#include "CalibrationPointDelegate.h"


CalibrationPointDelegate::CalibrationPointDelegate(QObject* parent)
    : QAbstractItemDelegate(parent)
{
    pixelSize = 10;
}


void CalibrationPointDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    if (index.column() == 0)
    {
        if (option.state & QStyle::State_Selected)
            painter->fillRect(option.rect, option.palette.highlight());
        QColor color = index.model()->data(index, Qt::DisplayRole).value<QColor>();
        double radius = 0.5 * (double) qMin<int>(option.rect.width(), option.rect.height());
        painter->save();
        painter->setRenderHint(QPainter::Antialiasing, true);
        painter->setPen(Qt::NoPen);
        painter->setBrush(QBrush(color));
        painter->drawEllipse(QRectF(option.rect.x() + (option.rect.width()  - radius) / 2,
                                    option.rect.y() + (option.rect.height() - radius) / 2,
                                    radius, radius));
        painter->restore();
    }
}


QSize CalibrationPointDelegate::sizeHint(const QStyleOptionViewItem& /* option */, const QModelIndex& /* index */) const
{
    return QSize(pixelSize, pixelSize);
}
