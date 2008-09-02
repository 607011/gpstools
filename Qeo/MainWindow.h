// $Id: MainWindow.h bd86a16444b7 2008/08/27 15:09:21 Oliver Lau <ola@ct.heise.de> $
// Copyright (c) 2008 Oliver Lau <oliver@ersatzworld.net>
// Alle Rechte vorbehalten.

#ifndef __MAINWINDOW_H_
#define __MAINWINDOW_H_

#include <QMainWindow>
#include <QString>
#include <QVector>
#include <QColor>
#include <QPolygon>
#include <QTransform>
#include <QSettings>
#include <QStringList>

#include "gpslib/GPXFile.h"

#include "ui_MainWindow.h"

#include "TrackEditor.h"
#include "CalibrationWindow.h"
#include "TrackStatisticsWindow.h"
#include "BrowserWindow.h"
#include "GPXFileTableModel.h"
#include "TrackTableModel.h"
#include "LonLatTableModel.h"
#include "CalibrationIniFile.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = 0);
    ~MainWindow();

protected:
    void keyPressEvent(QKeyEvent*);

private:
    static const int MaxRecentFiles = 10;
    QAction* recentFileActs[MaxRecentFiles];
    QAction* recentFileListSeparator;
    int recentFileCount;

    Ui::MainWindowClass ui;

    QStringList lastLoaded;
    QString lastTrackFilePath;
    QStatusBar* status;
    QSettings* settings;

    QVector<TrackEditor*> editTrackTabList;

#ifdef _DEBUG
    BrowserWindow browserWindow;
#endif
    TrackStatisticsWindow statisticsWindow;
    CalibrationWindow calibrationWindow;

private:
    void readSettings(void);
    void writeSettings(void);
    void closeTab(int tabIndex);
    void closeAllTabs(void);
    void saveAllTabs(void);
    void saveCurrentTab(void);
    void updateRecentFileActions(void);
    void openGPXFile(const QString& filename);
    GPS::GPXFile* currentGPXFile(void);
    void createTab(const QString& filename, GPS::GPXFile* file);

private slots:
    void on_actionAboutQt_triggered();
    void on_actionAboutQeo_triggered();
    void on_actionSave_triggered();
    void on_actionSaveAll_triggered();
    void on_actionClose_triggered();
    void on_actionCloseAll_triggered();
    void on_actionOpen_triggered();
    void on_actionExit_triggered();
    void on_actionNew_triggered();
    void on_actionCalibrateMaps_triggered();
    void on_actionCompareTracks_triggered();
    void on_actionBrowseTheWeb_triggered();
    void openRecentFile(void);
    void prepareExit(void);
    void trackChanged(void);
    void closeCurrentTab(void);
};

#endif // __MAINWINDOW_H_
