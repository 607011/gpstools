// $Id: BrowserWindow.h bd86a16444b7 2008/08/27 15:09:21 Oliver Lau <ola@ct.heise.de> $
// Copyright (c) 2008 Oliver Lau <oliver@ersatzworld.net>
// Alle Rechte vorbehalten.

#ifndef __BROWSERWINDOW_H_
#define __BROWSERWINDOW_H_

#include <QHeaderView>

#include "ui_BrowserWindow.h"


class BrowserWindow : public QWidget
{
    Q_OBJECT

public:
    BrowserWindow(QWidget* parent = 0);
    ~BrowserWindow();

private:
    Ui::BrowserWindowClass ui;

private slots:
    void urlChanged(const QString&);
};



#endif // __TRACKSTATISTICSWINDOW_H_
