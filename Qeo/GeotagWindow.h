#ifndef GEOTAGWINDOW_H
#define GEOTAGWINDOW_H

#include <QWidget>
#include "ui_GeotagWindow.h"

class GeotagWindow : public QWidget
{
    Q_OBJECT

public:
    GeotagWindow(QWidget *parent = 0);
    ~GeotagWindow();

private:
    Ui::GeotagWindowClass ui;

private slots:
    void on_comboBox_textChanged(const QString &);
};

#endif // GEOTAGWINDOW_H
