/*********************************************************************/
/* Copyright (c) 2013-2018, EPFL/Blue Brain Project                  */
/*                          Raphael Dumusc <raphael.dumusc@epfl.ch>  */
/* All rights reserved.                                              */
/*                                                                   */
/* Redistribution and use in source and binary forms, with or        */
/* without modification, are permitted provided that the following   */
/* conditions are met:                                               */
/*                                                                   */
/*   1. Redistributions of source code must retain the above         */
/*      copyright notice, this list of conditions and the following  */
/*      disclaimer.                                                  */
/*                                                                   */
/*   2. Redistributions in binary form must reproduce the above      */
/*      copyright notice, this list of conditions and the following  */
/*      disclaimer in the documentation and/or other materials       */
/*      provided with the distribution.                              */
/*                                                                   */
/*    THIS  SOFTWARE IS PROVIDED  BY THE  UNIVERSITY OF  TEXAS AT    */
/*    AUSTIN  ``AS IS''  AND ANY  EXPRESS OR  IMPLIED WARRANTIES,    */
/*    INCLUDING, BUT  NOT LIMITED  TO, THE IMPLIED  WARRANTIES OF    */
/*    MERCHANTABILITY  AND FITNESS FOR  A PARTICULAR  PURPOSE ARE    */
/*    DISCLAIMED.  IN  NO EVENT SHALL THE UNIVERSITY  OF TEXAS AT    */
/*    AUSTIN OR CONTRIBUTORS BE  LIABLE FOR ANY DIRECT, INDIRECT,    */
/*    INCIDENTAL,  SPECIAL, EXEMPLARY,  OR  CONSEQUENTIAL DAMAGES    */
/*    (INCLUDING, BUT  NOT LIMITED TO,  PROCUREMENT OF SUBSTITUTE    */
/*    GOODS  OR  SERVICES; LOSS  OF  USE,  DATA,  OR PROFITS;  OR    */
/*    BUSINESS INTERRUPTION) HOWEVER CAUSED  AND ON ANY THEORY OF    */
/*    LIABILITY, WHETHER  IN CONTRACT, STRICT  LIABILITY, OR TORT    */
/*    (INCLUDING NEGLIGENCE OR OTHERWISE)  ARISING IN ANY WAY OUT    */
/*    OF  THE  USE OF  THIS  SOFTWARE,  EVEN  IF ADVISED  OF  THE    */
/*    POSSIBILITY OF SUCH DAMAGE.                                    */
/*                                                                   */
/* The views and conclusions contained in the software and           */
/* documentation are those of the authors and should not be          */
/* interpreted as representing official policies, either expressed   */
/* or implied, of Ecole polytechnique federale de Lausanne.          */
/*********************************************************************/

#include "BackgroundWidget.h"

#include "configuration/Configuration.h"
#include "scene/Background.h"
#include "scene/ContentFactory.h"

#include <QtWidgets>

BackgroundWidget::BackgroundWidget(Configuration& configuration,
                                   QWidget* parent_)
    : QDialog{parent_}
    , _configuration{configuration}
    , _backgroundFolder{configuration.folders.contents}
{
    setWindowTitle(tr("Background settings"));

    const auto frameStyle = QFrame::Sunken | QFrame::Panel;

    // Get current variables

    _previousColor = _background().getColor();
    _previousBackgroundUri = _background().getUri();

    // Color chooser

    _colorLabel = new QLabel(_previousColor.name());
    _colorLabel->setFrameStyle(frameStyle);
    _colorLabel->setPalette(QPalette(_previousColor));
    _colorLabel->setAutoFillBackground(true);

    auto colorButton = new QPushButton(tr("Choose background color..."));
    connect(colorButton, SIGNAL(clicked()), this, SLOT(_chooseColor()));

    // Background chooser

    _backgroundLabel = new QLabel(_previousBackgroundUri);
    _backgroundLabel->setFrameStyle(frameStyle);
    auto backgroundButton = new QPushButton(tr("Choose background content..."));
    connect(backgroundButton, SIGNAL(clicked()), this,
            SLOT(_openBackgroundContent()));

    auto backgroundClearButton = new QPushButton(tr("Remove background"));
    connect(backgroundClearButton, SIGNAL(clicked()), this,
            SLOT(_removeBackground()));

    // Standard buttons

    using button = QDialogButtonBox::StandardButton;
    auto buttonBox =
        new QDialogButtonBox(button::Ok | button::Cancel, Qt::Horizontal, this);

    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    // Layout

    auto gridLayout = new QGridLayout();
    gridLayout->setColumnStretch(1, 1);
    gridLayout->setColumnMinimumWidth(1, 250);
    setLayout(gridLayout);

    gridLayout->addWidget(colorButton, 0, 0);
    gridLayout->addWidget(_colorLabel, 0, 1);
    gridLayout->addWidget(backgroundButton, 1, 0);
    gridLayout->addWidget(_backgroundLabel, 1, 1);
    gridLayout->addWidget(backgroundClearButton, 2, 0);
    gridLayout->addWidget(buttonBox, 2, 1);
}

void BackgroundWidget::accept()
{
    if (_configuration.saveBackgroundChanges())
    {
        _previousColor = _background().getColor();
        _previousBackgroundUri = _background().getUri();

        QDialog::accept();
    }
    else
    {
        QMessageBox messageBox;
        messageBox.setText(
            "An error occured while saving the configuration file."
            "Changes could not be saved.");
        messageBox.exec();
    }
}

void BackgroundWidget::reject()
{
    // Revert to saved settings
    _colorLabel->setText(_previousColor.name());
    _colorLabel->setPalette(QPalette(_previousColor));
    _backgroundLabel->setText(_previousBackgroundUri);

    _background().setColor(_previousColor);
    _background().setUri(_previousBackgroundUri);

    QDialog::reject();
}

void BackgroundWidget::setActiveSurface(const uint surfaceIndex)
{
    if (surfaceIndex >= _configuration.surfaces.size())
        return;

    _surfaceIndex = surfaceIndex;

    const auto color = _background().getColor();
    _colorLabel->setText(color.name());
    _colorLabel->setPalette(QPalette(color));
    _previousColor = color;

    const auto uri = _background().getUri();
    _backgroundLabel->setText(uri);
    _previousBackgroundUri = uri;
}

void BackgroundWidget::_chooseColor()
{
    const auto color = QColorDialog::getColor(Qt::green, this);
    if (!color.isValid())
        return;

    _colorLabel->setText(color.name());
    _colorLabel->setPalette(QPalette(color));

    _background().setColor(color);
}

void BackgroundWidget::_openBackgroundContent()
{
    const auto filter = ContentFactory::getSupportedFilesFilterAsString();
    const auto filename =
        QFileDialog::getOpenFileName(this, tr("Choose content"),
                                     _backgroundFolder, filter);
    if (filename.isEmpty())
        return;

    _backgroundFolder = QFileInfo(filename).absoluteDir().path();

    if (ContentFactory::getContent(filename))
    {
        _backgroundLabel->setText(filename);
        _background().setUri(filename);
    }
    else
    {
        QMessageBox messageBox;
        messageBox.setText(tr("Error: Unsupported file."));
        messageBox.exec();
    }
}

void BackgroundWidget::_removeBackground()
{
    _backgroundLabel->setText("");
    _background().setUri("");
}

Background& BackgroundWidget::_background()
{
    return *_configuration.surfaces[_surfaceIndex].background;
}
