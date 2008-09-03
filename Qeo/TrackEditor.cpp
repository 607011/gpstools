// $Id$
// Copyright (c) 2008 Oliver Lau <oliver@ersatzworld.net>
// Alle Rechte vorbehalten.

#ifdef _DEBUG
#include <QtDebug>
#endif

#include "TrackEditor.h"


TrackEditor::TrackEditor(QWidget* parent) : QWidget(parent)
{
    ui.setupUi(this);

    connect(ui.trackEditorRenderArea,
            SIGNAL(distanceChanged(double)),
            ui.elevationProfileRenderArea,
            SLOT(setMarker(double)));

    // relay trackChanged() signal
    connect(ui.trackEditorRenderArea,
            SIGNAL(trackChanged()),
            this,
            SIGNAL(trackChanged()));
}


TrackEditor::~TrackEditor()
{
    // ...
}


QSize TrackEditor::sizeHint(void) const
{
    return QSize(400, 300);
}


QSize TrackEditor::minimumSizeHint(void) const
{
    return QSize(240, 160);
}



void TrackEditor::setGPXFile(GPS::GPXFile* gpx)
{
    _GPX = gpx;
    ui.trackEditorRenderArea->setTrack(_GPX->track());
    ui.elevationProfileRenderArea->setTrack(_GPX->track());
}


bool TrackEditor::trackWasEdited(void) const
{
    return ui.trackEditorRenderArea->trackWasEdited();
}


void TrackEditor::editedTrackChanged(void)
{
    GPS::Track* trk = _GPX->track();
    if (trk != NULL)
    {
        trk->postProcess();
        ui.trackDistanceLabel->setText(QString("%1").arg(1e-3 * trk->distance()));
        ui.trackPointCountLabel->setText(QString("%1").arg(trk->keptCount()));
        ui.ascentLabel->setText((trk->ascent().defined())? QString("%1").arg(trk->ascent().value()) : tr("unknown"));
        ui.descentLabel->setText((trk->descent().defined())? QString("%1").arg(trk->descent().value()) : tr("unknown"));
        ui.minEleLabel->setText((trk->minElevation().defined())? QString("%1").arg(trk->minElevation().value()) : tr("unknown"));
        ui.maxEleLabel->setText((trk->maxElevation().defined())? QString("%1").arg(trk->maxElevation().value()) : tr("unknown"));
    }
    else
    {
        ui.trackDistanceLabel->setText(QString());
        ui.trackPointCountLabel->setText(QString());
        ui.ascentLabel->setText(QString());
        ui.descentLabel->setText(QString());
        ui.minEleLabel->setText(QString());
        ui.maxEleLabel->setText(QString());
    }

    ui.elevationProfileRenderArea->update();
    ui.trackEditorRenderArea->update();
}


void TrackEditor::on_thresholdSpinBox_valueChanged(double threshold)
{
    GPS::Track* trk = _GPX->track();
    if (trk != NULL)
    {
        if (threshold < 1e-5) /* somewhat near 0 */ {
            trk->keepAll();
        }
        else {
            trk->keepNone();
            switch (ui.weedingTypeComboBox->currentIndex())
            {
            case 0: // 3D (Douglas-Peucker)
                trk->weed3D(threshold);
                break;
            case 1: // 2D (Douglas-Peucker)
                trk->weed2D(threshold);
                break;
            case 2: // Höhe (Douglas-Peucker)
                trk->smoothDouglasPeucker(threshold);
                break;
            case 3: // Höhe (Schwellwert)
                trk->smoothThreshold(threshold);
                break;
            default:
                break;
            }
        }
        editedTrackChanged();
    }
}


void TrackEditor::on_weedingTypeComboBox_currentIndexChanged(int)
{
    on_thresholdSpinBox_valueChanged(ui.thresholdSpinBox->value());
}
