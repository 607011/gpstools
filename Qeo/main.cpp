// $Id$
// Copyright (c) 2008 Oliver Lau <oliver@ersatzworld.net>
// Alle Rechte vorbehalten.


#include "MainWindow.h"
#include "QeoApplication.h"

int main(int argc, char* argv[])
{
    QeoApplication a(argc, argv);
    MainWindow w;
    a.setMainWindow(&w);
    w.show();
    return a.exec();
}
