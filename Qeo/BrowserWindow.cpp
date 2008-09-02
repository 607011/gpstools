// $Id: BrowserWindow.cpp bd86a16444b7 2008/08/27 15:09:21 Oliver Lau <ola@ct.heise.de> $
// Copyright (c) 2008 Oliver Lau <oliver@ersatzworld.net>
// Alle Rechte vorbehalten.

#include <QFileDialog>
#include <QFileInfo>

#include "BrowserWindow.h"


BrowserWindow::BrowserWindow(QWidget* parent) : QWidget(parent)
{
    ui.setupUi(this);

    connect(ui.urlLineEdit, SIGNAL(textChanged(const QString&)),
        this, SLOT(urlChanged(const QString&)));

#ifdef _DEBUG
    ui.urlLineEdit->setText("http://preview.von-und-fuer-lau.de/");
#endif // _DEBUG
}


BrowserWindow::~BrowserWindow()
{
    // ...
}


void BrowserWindow::urlChanged(const QString& url)
{
    ui.webView->load(QUrl(url));
}
