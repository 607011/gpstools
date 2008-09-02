// $Id: CalibrationPointView.h bd86a16444b7 2008/08/27 15:09:21 Oliver Lau <ola@ct.heise.de> $
// Copyright (c) 2008 Oliver Lau <oliver@ersatzworld.net>
// Alle Rechte vorbehalten.

#ifndef __CALIBRATIONPOINTVIEW_H_
#define __CALIBRATIONPOINTVIEW_H_

#include <QTableView>

class CalibrationPointView : public QTableView
{
    Q_OBJECT

public:
    CalibrationPointView(QWidget* parent = 0);
    ~CalibrationPointView();

protected:
    bool edit(const QModelIndex& index, EditTrigger trigger, QEvent* event);

protected slots:
    void dataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight);

private:
};

#endif // __CALIBRATIONPOINTVIEW_H_
