// $Id$
// Copyright (c) 2008 Oliver Lau <oliver@ersatzworld.net>.
// Alle Rechte vorbehalten.

#include <QMessageBox>

#include "QeoApplication.h"


QeoApplication::QeoApplication(int& argc, char* argv[]) : QApplication(argc, argv)
{
    _MainWindow = NULL;
}


QeoApplication::~QeoApplication()
{
    // ...
}


void QeoApplication::commitData(QSessionManager& manager)
{
    if (manager.allowsInteraction())
    {
        int rc = QMessageBox::warning(
            _MainWindow,
            tr("My Application"),
            tr("Save changes to document?"),
            QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        switch (rc)
        {
        case QMessageBox::Save:
            manager.release();
            //if (!saveDocument())
            //    manager.cancel();
            break;
        case QMessageBox::Discard:
            break;
        case QMessageBox::Cancel:
            /* fall-through */
        default:
            manager.cancel();
        }
    }
    else
    {
        // we did not get permission to interact, then
        // do something reasonable instead
    }
}
