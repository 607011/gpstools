// $Id: CalibrationPointView.cpp bd86a16444b7 2008/08/27 15:09:21 Oliver Lau <ola@ct.heise.de> $
// Copyright (c) 2008 Oliver Lau <oliver@ersatzworld.net>
// Alle Rechte vorbehalten.


#include "CalibrationPointView.h"

#ifdef _DEBUG
#include <QtDebug>
#endif // _DEBUG


CalibrationPointView::CalibrationPointView(QWidget* parent)
    : QTableView(parent)
{

}

CalibrationPointView::~CalibrationPointView()
{
    // ...
}


bool CalibrationPointView::edit(const QModelIndex& index, EditTrigger trigger, QEvent* event)
{
    switch (index.column())
    {
    case 1:
        // fall-through
    case 2:
        return QAbstractItemView::edit(index, trigger, event);
    default:
        break;
    }
    return false;
}


void CalibrationPointView::dataChanged(const QModelIndex& /* topLeft */, const QModelIndex& /* bottomRight */)
{
    // ...
}
