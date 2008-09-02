// $Id: Qeo.cpp 0aafbc2f4c11 2008/08/12 15:32:38 Oliver Lau <oliver@ersatzworld.net> $
// Copyright (c) 2008 Oliver Lau <ola@ct.heise.de>, Heise Zeitschriften Verlag.
// Alle Rechte vorbehalten.

#include <QtGlobal>
#include <QString>
#include <QLabel>
#include "qtalti.h"

#include "TrackEditorTab.h"


QtAlti::QtAlti(QWidget* parent, Qt::WFlags flags) : QMainWindow(parent, flags)
{
    tabWidget = new QTabWidget(this);
    TrackEditorTab* trackEditorTab = new TrackEditorTab;
    tabWidget->addTab(trackEditorTab, tr("Kalibrieren"));


    lastPath = "..\\test"; // QDir::currentPath();
    QLabel* browseLabelRef = new QLabel(tr("Referenztrack"));
    browseLabelRef->setMaximumSize(120, 24);
    fileComboBoxRef = new QComboBox;
    fileComboBoxRef->setMinimumSize(220, 21);
    browseButtonRef = new QPushButton(tr("Finden ..."));
    browseButtonRef->setMinimumSize(86, 21);
    browseButtonRef->setMaximumSize(112, 21);

    QLabel* browseLabelMy = new QLabel(tr("Mein Track"));
    browseLabelMy->setMaximumSize(120, 24);
    fileComboBoxMy = new QComboBox;
    fileComboBoxMy->setMinimumSize(220, 21);
    browseButtonMy = new QPushButton(tr("Finden ..."));
    browseButtonMy->setMinimumSize(86, 21);
    browseButtonMy->setMaximumSize(112, 21);

    QGridLayout *browserLayout = new QGridLayout;
    browserLayout->addWidget(browseLabelRef, 0, 0);
    browserLayout->addWidget(fileComboBoxRef, 0, 1);
    browserLayout->addWidget(browseButtonRef, 0, 2);
    browserLayout->addWidget(browseLabelMy, 1, 0);
    browserLayout->addWidget(fileComboBoxMy, 1, 1);
    browserLayout->addWidget(browseButtonMy, 1, 2);

    plotWidget = new RenderArea;
    plotWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QLabel* latLabel = new QLabel(tr("Breitengrad"));
    latLineEdit = new QLineEdit;
    latLineEdit->setMaximumSize(200, 21);
    QLabel* lonLabel = new QLabel(tr("Längengrad"));
    lonLineEdit = new QLineEdit;
    lonLineEdit->setMaximumSize(200, 21);

    QGridLayout *latlonLayout = new QGridLayout;
    latlonLayout->addWidget(latLabel, 0, 0);
    latlonLayout->addWidget(latLineEdit, 1, 0);
    latlonLayout->addWidget(lonLabel, 0, 1);
    latlonLayout->addWidget(lonLineEdit, 1, 1);

    QLabel* latMLabel = new QLabel(tr("Breitengrad Mitte"));
    latMLineEdit = new QLineEdit;
    latMLineEdit->setMaximumSize(200, 21);
    QLabel* lonMLabel = new QLabel(tr("Längengrad Mitte"));
    lonMLineEdit = new QLineEdit;
    lonMLineEdit->setMaximumSize(200, 21);

    QGridLayout *latlonMLayout = new QGridLayout;
    latlonMLayout->addWidget(latMLabel, 0, 0);
    latlonMLayout->addWidget(latMLineEdit, 1, 0);
    latlonMLayout->addWidget(lonMLabel, 0, 1);
    latlonMLayout->addWidget(lonMLineEdit, 1, 1);

    QHBoxLayout *hLayout = new QHBoxLayout;
    hLayout->addLayout(latlonLayout);
    hLayout->addLayout(latlonMLayout);
    hLayout->addStretch();

    QVBoxLayout *vLayout = new QVBoxLayout;
    vLayout->addLayout(browserLayout);
    vLayout->addWidget(plotWidget);
    vLayout->addLayout(hLayout);

    setLayout(vLayout);

    connect(browseButtonRef, SIGNAL(clicked()), this, SLOT(browseRef()));
    connect(browseButtonMy, SIGNAL(clicked()), this, SLOT(browseMy()));
    connect(fileComboBoxRef, SIGNAL(currentIndexChanged(int)), this, SLOT(refTrackChanged(int)));
    connect(fileComboBoxMy, SIGNAL(currentIndexChanged(int)), this, SLOT(myTrackChanged(int)));

    connect(plotWidget, SIGNAL(latlonChanged(double,double)), this, SLOT(latlonChanged(double,double)));
    connect(plotWidget, SIGNAL(latlonMChanged(double,double)), this, SLOT(latlonMChanged(double,double)));

    setWindowTitle(tr("QtAlti [DEMO]"));

#ifdef _DEBUG
    fileComboBoxRef->addItem("");
    fileComboBoxRef->addItem("../test/compare-ref.gpx");
    fileComboBoxRef->addItem("../test/20080621-STUNT100-Runde1.gpx");
    fileComboBoxRef->addItem("../test/20080106-Gipfelstutzer.gpx");
    fileComboBoxRef->setCurrentIndex(fileComboBoxRef->count()-1);
    fileComboBoxMy->addItem("");
    fileComboBoxMy->addItem("../test/compare-0.gpx");
    fileComboBoxMy->addItem("../test/compare-1.gpx");
    fileComboBoxMy->addItem("../test/compare-2.gpx");
    fileComboBoxMy->addItem("../test/compare-3.gpx");
    fileComboBoxMy->addItem("../test/compare-4.gpx");
    fileComboBoxMy->setCurrentIndex(0);
#endif
}


QtAlti::~QtAlti()
{
    // ...
}


void QtAlti::browseRef(void)
{
    QString filename = QFileDialog::getOpenFileName(this,
        tr("GPX-Dateien finden"), lastPath, tr("GPS-Dateien (*.gpx)"));
    if (!filename.isEmpty())
    {
        fileComboBoxRef->addItem(filename);
        fileComboBoxRef->setCurrentIndex(fileComboBoxRef->count()-1);
    }
}


void QtAlti::browseMy(void)
{
    QString filename = QFileDialog::getOpenFileName(this,
        tr("GPX-Dateien finden"), lastPath, tr("GPS-Dateien (*.gpx)"));
    if (!filename.isEmpty())
    {
        fileComboBoxMy->addItem(filename);
        fileComboBoxMy->setCurrentIndex(fileComboBoxMy->count()-1);
    }
}


void QtAlti::refTrackChanged(int idx)
{
    QString filename = fileComboBoxRef->itemText(idx);
    if (filename != "") {
        int rc = refFile.load(filename.toStdString());
        if (rc == 0)
        {
            qDebug("loaded %d trackpoints", refFile.track().points().size());
            plotWidget->setReferenceTrack(refFile.track());
        }
    }
    else {
        refFile.track().clear();
    }
}


void QtAlti::myTrackChanged(int idx)
{
    QString filename = fileComboBoxMy->itemText(idx);
    int rc = myFile.load(filename.toStdString());
    if (rc == 0) 
    {
        qDebug("loaded %d trackpoints", myFile.track().points().size());
        plotWidget->setTrack(myFile.track());
    }
}


void QtAlti::latlonChanged(double lat, double lon)
{
    latLineEdit->setText(QString("%1").arg(lat, 13, 'f'));
    lonLineEdit->setText(QString("%1").arg(lon, 13, 'f'));
}


void QtAlti::latlonMChanged(double lat, double lon)
{
    latMLineEdit->setText(QString("%1").arg(lat, 13, 'f'));
    lonMLineEdit->setText(QString("%1").arg(lon, 13, 'f'));
}
