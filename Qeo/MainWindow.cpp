// $Id$
// Copyright (c) 2008 Oliver Lau <oliver@ersatzworld.net>
// Alle Rechte vorbehalten.

#include "MainWindow.h"
#include "CalibrationPointDelegate.h"

#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QPolygonF>
#include <QClipboard>
#include <QStatusBar>

#ifdef _DEBUG
#include <QtDebug>
#endif



MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent)
{
    recentFileCount = 0;
    ui.setupUi(this);

    QToolButton* closeButton = new QToolButton;
    closeButton->setIcon(QIcon(":/Qeo/Resources/closetab.png"));
    closeButton->setAutoRaise(true);
    closeButton->setToolTip(tr("Close current editor"));
    ui.tabWidget->setCornerWidget(closeButton, Qt::TopRightCorner);
    connect(closeButton, SIGNAL(clicked()), this, SLOT(closeCurrentTab()));

    for (int i = 0; i < MaxRecentFiles; ++i)
    {
        recentFileActs[i] = new QAction(this);
        recentFileActs[i]->setVisible(false);
        connect(recentFileActs[i], SIGNAL(triggered()),
            this, SLOT(openRecentFile()));
    }
    recentFileListSeparator = ui.menu_File->addSeparator();
    recentFileListSeparator->setVisible(false);
    for (int i = 0; i < MaxRecentFiles; ++i)
        ui.menu_File->addAction(recentFileActs[i]);

    readSettings();

    status = new QStatusBar(this);
    setStatusBar(status);
}


MainWindow::~MainWindow()
{
    if (settings != NULL) {
        delete settings;
        settings = NULL;
    }
}


void MainWindow::readSettings(void)
{
    settings = new QSettings(QSettings::IniFormat, QSettings::UserScope, "von und für lau", "Qeo", this);
    settings->beginGroup("dir");
    calibrationWindow.setLastMapFilePath(settings->value("maps", QDir::currentPath()).toString());
    lastTrackFilePath = settings->value("tracks", QDir::currentPath()).toString();
    settings->endGroup();
    QStringList files = settings->value("recentFileList").toStringList();
    updateRecentFileActions();
    settings->beginGroup("window");
    bool doRestore = settings->value("restore").toBool();
    if (doRestore) {
        QSize size = settings->value("size").toSize();
        if (!size.isEmpty())
            resize(size);
        QPoint pos = settings->value("pos").toPoint();
        if (!pos.isNull())
            move(pos);
    }
    settings->endGroup();
}


void MainWindow::writeSettings(void)
{
    if (settings != NULL)
    {
        settings->beginGroup("dir");
        settings->setValue("maps", calibrationWindow.lastMapFilePath());
        settings->setValue("tracks", lastTrackFilePath);
        settings->endGroup();
        settings->beginGroup("window");
        settings->setValue("restore", true);
        settings->setValue("pos", pos());
        settings->setValue("size", size());
        settings->endGroup();
        settings->sync();
    }
}


void MainWindow::updateRecentFileActions(void)
{
    if (settings != NULL)
    {
        QStringList files = settings->value("recentFileList").toStringList();
        recentFileCount = qMin<int>(files.size(), MaxRecentFiles);
        recentFileListSeparator->setVisible(recentFileCount > 0);
        for (int i = 0; i < recentFileCount; ++i)
        {
            if (files.at(i).size() > 0)
            {
                QString text = tr("&%1 %2").arg(i + 1).arg(QFileInfo(files.at(i)).fileName());
                recentFileActs[i]->setText(text);
                recentFileActs[i]->setData(files.at(i));
                recentFileActs[i]->setVisible(true);
            }
        }
        for (int j = recentFileCount; j < MaxRecentFiles; ++j)
            recentFileActs[j]->setVisible(false);
    }
}


void MainWindow::closeTab(int tabIndex)
{
    Q_ASSERT( tabIndex < ui.tabWidget->count() );
    TrackEditor* editor = qobject_cast<TrackEditor*>(ui.tabWidget->currentWidget());
    ui.tabWidget->removeTab(tabIndex);
    delete editor;
}


void MainWindow::closeCurrentTab(void)
{
    closeTab(ui.tabWidget->currentIndex());
}


void MainWindow::saveAllTabs(void)
{
    for (int i = 0; i < ui.tabWidget->count(); ++i)
    {
        TrackEditor* editor = qobject_cast<TrackEditor*>(ui.tabWidget->widget(i));
        if (editor->trackWasEdited())
        {
#ifdef _DEBUG
            qDebug() << "MainWindow::saveTracks() " << editor->gpxFile()->filename().c_str();
#endif
        }
    }
}


void MainWindow::closeAllTabs(void)
{
    bool anyTrackChanged = false;
    for (int i = 0; i < ui.tabWidget->count(); ++i)
        anyTrackChanged |= qobject_cast<TrackEditor*>(ui.tabWidget->widget(i))->trackWasEdited();
    if (anyTrackChanged)
    {
        int rc = QMessageBox::question(this, tr("Really quit?"), tr("You have made changes to one or more tracks. Do you really want to close the application without saving?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
        if (rc == QMessageBox::Yes)
            close();
        else
            saveAllTabs();
    }
    for (int i = 0; i < ui.tabWidget->count(); ++i)
        closeTab(i);
}


void MainWindow::prepareExit(void)
{
    writeSettings();
    closeAllTabs();
}


void MainWindow::on_actionAboutQt_triggered()
{
    QApplication::aboutQt();
}


void MainWindow::on_actionAboutQeo_triggered()
{
    QMessageBox::information(this, tr("About Qeo"), tr("<b>Qeo</b> helps you visually edit tracks, calibrate maps und statistically compare tracks."
        "<br/>"
        "<br/>"
        "Copyright &copy; 2008 Oliver Lau <oliver@ersatzworld.net>. All rights reserved."));
}


void MainWindow::on_actionCalibrateMaps_triggered()
{
    calibrationWindow.show();
}


void MainWindow::on_actionCompareTracks_triggered()
{
    statisticsWindow.show();
}


void MainWindow::on_actionBrowseTheWeb_triggered()
{
#ifdef _DEBUG
    browserWindow.show();
#endif
}


GPS::GPXFile* MainWindow::currentGPXFile(void)
{
    return qobject_cast<TrackEditor*>(ui.tabWidget->currentWidget())->gpxFile();
}


void MainWindow::saveCurrentTab(void)
{
    GPS::GPXFile* file = currentGPXFile();
    if (file != NULL)
    {
        file->write();
        statusBar()->showMessage(QString("Track \"%1\" saved with %2 track points.")
            .arg(file->filename().c_str())
            .arg(file->track()->points().size()));
    }
}


void MainWindow::on_actionSave_triggered()
{
    GPS::GPXFile* file = currentGPXFile();
    if (file != NULL)
    {
        if (file->track() == NULL) {
            QMessageBox::information(this, tr("No track"), tr("The file you want to save does not contain a track."));
        }
        else {
            saveCurrentTab();
        }
    }
}


void MainWindow::on_actionSaveAll_triggered()
{
    saveAllTabs();
}


void MainWindow::on_actionClose_triggered()
{
    closeCurrentTab();
}


void MainWindow::on_actionCloseAll_triggered()
{
    closeAllTabs();
}


void MainWindow::on_actionExit_triggered()
{
    prepareExit();
    close();
}


void MainWindow::on_actionNew_triggered()
{
    createTab(tr("<new file>.gpx"), new GPS::GPXFile);
}


void MainWindow::createTab(const QString& filename, GPS::GPXFile* file)
{
    TrackEditor* editor = new TrackEditor;
    editor->setGPXFile(file);
    editor->editedTrackChanged();
    editTrackTabList.append(editor);
    ui.tabWidget->addTab(editor, filename);
    ui.tabWidget->setCurrentIndex(ui.tabWidget->count()-1);
    connect(editor, SIGNAL(trackChanged()), this, SLOT(trackChanged()));

}


void MainWindow::on_actionOpen_triggered()
{
    QString filename = QFileDialog::getOpenFileName(this,
        tr("Find GPS files"), lastTrackFilePath, tr("GPS files (*.gpx)"));
    if (!filename.isEmpty())
    {
        QFileInfo inf(filename);
        lastTrackFilePath = inf.path();
        openGPXFile(filename);
        QStringList files = settings->value("recentFileList").toStringList();
        if (!files.contains(filename))
        {
            files.insert(0, filename);
            if (files.size() > MaxRecentFiles)
                files.pop_back();
            settings->setValue("recentFileList", files);
            updateRecentFileActions();
        }
    }
}


void MainWindow::openGPXFile(const QString& filename)
{
    GPS::GPXFile* file = new GPS::GPXFile;
    file->load(filename.toStdString());
    if (file->track() == NULL) {
        QMessageBox::information(this, tr("GPX-File without a track"), tr("GPX file \"%1\" doesn't contain a track.").arg(filename));
    }
    else {
        file->track()->keepAll();
    }
    createTab(QFileInfo(filename).fileName(), file);
}


void MainWindow::openRecentFile(void)
{
    QAction* action = qobject_cast<QAction*>(sender());
    if (action != NULL)
        openGPXFile(action->data().toString());
}


void MainWindow::trackChanged(void)
{
    TrackEditor* editor = qobject_cast<TrackEditor*>(sender());
    int tabIdx = ui.tabWidget->indexOf(editor);
    Q_ASSERT(tabIdx >= 0);
    const QString& tabText = ui.tabWidget->tabText(tabIdx);
    if (!tabText.endsWith("*")) {
        ui.tabWidget->setTabText(tabIdx, tabText + "*");
    }
}


void MainWindow::keyPressEvent(QKeyEvent* event)
{
    QWidget::keyPressEvent(event);
}
