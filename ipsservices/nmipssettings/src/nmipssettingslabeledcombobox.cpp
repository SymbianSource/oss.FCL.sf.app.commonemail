/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/

#include <qgraphicslinearlayout.h>
#include <hbcombobox.h>
#include <hblabel.h>

#include "nmipssettingslabeledcombobox.h"

/*!
    \class NmIpsSettingsLabelItem
    \brief The class implements a custom HbDataFormViewItem that combines a HbComboBox and HbLabel
    widgets that are linked. I.e. When the user selects changes the combobox's value, the label
    text is updated accordingly.

*/

// ======== MEMBER FUNCTIONS ========

/*!
    Constructor of NmIpsSettingsLabeledComboBox.
*/
NmIpsSettingsLabeledComboBox::NmIpsSettingsLabeledComboBox(QGraphicsItem *parent, Qt::WindowFlags wFlags)
: HbWidget(parent, wFlags)
{
    // Create widget layout.
    QGraphicsLinearLayout* layout = new QGraphicsLinearLayout(Qt::Vertical);
    this->setLayout(layout); // Takes ownership

    // Create combobox.
    mComboBox = new HbComboBox();
    layout->addItem(mComboBox); // Takes ownership

    // Create label.
    mLabel = new HbLabel();
    mLabel->setElideMode(Qt::ElideNone);
    mLabel->setTextWrapping(Hb::TextWordWrap);
    HbFontSpec spec(HbFontSpec::PrimarySmall);
    mLabel->setFontSpec(spec);
    layout->addItem(mLabel); // Takes ownership

    // Connect signals and slots.
    connect(mComboBox, SIGNAL(currentIndexChanged(int)), 
        this, SLOT(comboBoxIndexChanged(int)), Qt::UniqueConnection);
    connect(mComboBox, SIGNAL(currentIndexChanged(int)), 
        this, SIGNAL(currentIndexChanged(int)), Qt::UniqueConnection);
}

/*!
    Destructor of NmIpsSettingsLabeledComboBox.
*/
NmIpsSettingsLabeledComboBox::~NmIpsSettingsLabeledComboBox()
{
}

/*!
    Sets the current combobox index.
    \param index Combobox index to set.
*/
void NmIpsSettingsLabeledComboBox::setCurrentIndex(int index)
{
    // Change is not signaled outside via currentIndexChanged.
    disconnect(mComboBox, SIGNAL(currentIndexChanged(int)), 
        this, SIGNAL(currentIndexChanged(int)));
    mComboBox->setCurrentIndex(index);

    // Force refreshing the label text when the new index is same as current index.
    if (mComboBox->currentIndex() == index) {
        comboBoxIndexChanged(index);
        }

    // Reconnect the slot to signal changes outside.
    connect(mComboBox, SIGNAL(currentIndexChanged(int)),
            this, SIGNAL(currentIndexChanged(int)), Qt::UniqueConnection);
}

/*!
    Sets the combobox items and corresponding texts for the label.
    \param texts Item texts.
*/
void NmIpsSettingsLabeledComboBox::setItems(QStringList comboBoxItems,
                                            QStringList texts)
{
    // Set the combobox items without signaling index change.
    disconnect(mComboBox, SIGNAL(currentIndexChanged(int)), 
        this, SIGNAL(currentIndexChanged(int)));
    mComboBox->setItems(comboBoxItems);
    connect(mComboBox, SIGNAL(currentIndexChanged(int)),
        this, SIGNAL(currentIndexChanged(int)), Qt::UniqueConnection);

    // Set the corresponding texts for the label.
    mLabelTexts.clear();
    for (int i=0; i<comboBoxItems.count(); ++i) {
        mLabelTexts.insert(i, texts.at(i));
    }
}

/*!
    Updates the label text based on the combobox's value.
    \param index Current combobox index.
*/
void NmIpsSettingsLabeledComboBox::comboBoxIndexChanged(int index)
{
    mLabel->setPlainText(mLabelTexts.value(index));
}
