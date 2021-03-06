/*
    Copyright 2020 VUKOZ

    This file is part of 3D Forest.

    3D Forest is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    3D Forest is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with 3D Forest.  If not, see <https://www.gnu.org/licenses/>.
*/

/** @file WindowFileImport.cpp */

#include <QCheckBox>
#include <QDebug>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <WindowFileImport.hpp>

WindowFileImport::WindowFileImport(QWidget *parent) : QDialog(parent)
{
    // Widgets
    QLabel *description = new QLabel(tr("This operation may modify"
                                        " the input file."));

    description->setToolTip(tr("The file size may get bigger.\n"
                               "Unknown User data will be lost.\n"
                               "Some values will be normalized."));

    QGroupBox *options = new QGroupBox(tr("Options"));

    centerCheckBox_ = new QCheckBox;
    centerCheckBox_->setChecked(true);

    convertCheckBox_ = new QCheckBox;
    convertCheckBox_->setChecked(true);
    convertCheckBox_->setEnabled(false);

    acceptButton_ = new QPushButton(tr("Import"));
    connect(acceptButton_, SIGNAL(clicked()), this, SLOT(setResultAccept()));

    rejectButton_ = new QPushButton(tr("Cancel"));
    connect(rejectButton_, SIGNAL(clicked()), this, SLOT(setResultReject()));

    // Layout
    QGridLayout *optionsLayout = new QGridLayout;
    int row = 0;
    optionsLayout->addWidget(new QLabel(tr("Center offset")), row, 0);
    optionsLayout->addWidget(centerCheckBox_, row, 1);
    row++;
    optionsLayout->addWidget(new QLabel(tr("Convert to v1.4+")), row, 0);
    optionsLayout->addWidget(convertCheckBox_, row, 1);
    row++;
    options->setLayout(optionsLayout);

    QHBoxLayout *dialogButtons = new QHBoxLayout;
    dialogButtons->addStretch();
    dialogButtons->addWidget(acceptButton_);
    dialogButtons->addWidget(rejectButton_);

    QVBoxLayout *dialogLayout = new QVBoxLayout;
    dialogLayout->addWidget(options);
    dialogLayout->addSpacing(10);
    dialogLayout->addWidget(description);
    dialogLayout->addSpacing(10);
    dialogLayout->addLayout(dialogButtons);
    dialogLayout->addStretch();

    setLayout(dialogLayout);

    // Window
    setWindowTitle("Import File");
    setMaximumWidth(width());
    setMaximumHeight(height());
}

bool WindowFileImport::center() const
{
    return centerCheckBox_->isChecked();
}

void WindowFileImport::setResultAccept()
{
    close();
    setResult(QDialog::Accepted);
}

void WindowFileImport::setResultReject()
{
    close();
    setResult(QDialog::Rejected);
}
