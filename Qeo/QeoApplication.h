// $Id: QeoApplication.h 572e8ed1f764 2008/08/27 19:07:35 Oliver Lau <oliver.lau@gmail.com> $
// Copyright (c) 2008 Oliver Lau <oliver@ersatzworld.net>
// Alle Rechte vorbehalten.

#ifndef __QEOAPPLICATION_H_
#define __QEOAPPLICATION_H_

#include <QApplication>
#include <QSessionManager>


class QeoApplication : public QApplication {
    Q_OBJECT

private:
    QWidget* _MainWindow;

public:
    QeoApplication(int& argc, char* argv[]);
    ~QeoApplication();
    inline void setMainWindow(QWidget* mainWindow) { _MainWindow = mainWindow; }
    void commitData(QSessionManager& manager);
};

#endif //  __QEOAPPLICATION_H_
