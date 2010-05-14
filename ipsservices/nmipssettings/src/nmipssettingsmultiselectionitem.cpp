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

#include <QGraphicsLinearLayout>
#include <QScopedPointer>
#include <HbPushButton>
#include <HbSelectionDialog>
#include <HbLabel>
#include <HbExtendedLocale>

#include "nmipssettingsmultiselectionitem.h"

/*!
    \class NmIpsSettingsMultiSelectionItem
    \brief The class implements a custom HbDataFormViewItem for showing multi selection dialog.

*/

// ======== MEMBER FUNCTIONS ========

/*!
    Constructor of NmIpsSettingsMultiSelectionItem.
*/


NmIpsSettingsMultiSelectionItem::NmIpsSettingsMultiSelectionItem(
    QGraphicsItem *parent, Qt::WindowFlags wFlags)
 : HbWidget(parent, wFlags),
   mButton(0),
   mSelectionDialog(0)
{
    // Create widget layout.
    QScopedPointer<QGraphicsLinearLayout> layout(new QGraphicsLinearLayout(Qt::Vertical, this));
    this->setLayout(layout.data());

    // Create button.
    mButton = new HbPushButton();
    layout->addItem(mButton);

    connect(mButton, SIGNAL(clicked()), this, SLOT(launchSelectionDialog()));
    (void)layout.take();
}

/*!
    Destructor of NmIpsSettingsMultiSelectionItem.
*/
NmIpsSettingsMultiSelectionItem::~NmIpsSettingsMultiSelectionItem()
{
    delete mSelectionDialog;
}

/*!
    Returns shown text.

    \return Shown text.
*/
QString NmIpsSettingsMultiSelectionItem::text() const
{
    return mButton->text();
}

/*!
    Sets shown text.

    \param text Shown text.
*/
void NmIpsSettingsMultiSelectionItem::setText(QString text)
{
    mButton->setText(text);
}

/*!
    Returns items which are shown in dialog.

    \return items which are shown in dialog.
*/
QStringList NmIpsSettingsMultiSelectionItem::stringItems() const
{
    return mItems;
}

/*!
    Sets items which are shown in dialog.

    \param items Items which are shown in dialog.
*/
void NmIpsSettingsMultiSelectionItem::setStringItems(QStringList items)
{
    mItems = items;
    generateButtonText();
}

/*!
    Returns selected items.

    \return selected items.
*/
QList<QVariant> NmIpsSettingsMultiSelectionItem::selectedItems()const
{
    return mSelectedItems;
}

/*!
    Sets selected items.

    \param selectedItems Indexes of items which will be seleceted when dialog is shown.
*/
void NmIpsSettingsMultiSelectionItem::setSelectedItems(QList<QVariant> selectedItems)
{
    // Store selected items and generate button text.
    mSelectedItems = selectedItems;
    generateButtonText();
}

/*!
    Returns heading text.

    \return heading text.
*/
QString NmIpsSettingsMultiSelectionItem::heading() const
{
    return mHeading;
}

/*!
    Sets heading text.

    \param heading Shown heading text in dialog.
*/
void NmIpsSettingsMultiSelectionItem::setHeading(QString heading)
{
    mHeading = heading;
}

/*!
    Launches the Selection dialog.
*/
void NmIpsSettingsMultiSelectionItem::launchSelectionDialog()
{
    if (mSelectionDialog) {
        delete mSelectionDialog;
        mSelectionDialog = 0;
    }

    // Create the dialog.
    mSelectionDialog = new HbSelectionDialog();
    mSelectionDialog->setSelectionMode(HbAbstractItemView::MultiSelection);
    mSelectionDialog->setStringItems(mItems, mItems.count() + 1);
    mSelectionDialog->setSelectedItems(mSelectedItems);

    // Set the heading for the dialog.
    HbLabel *headingLabel = new HbLabel(mHeading, mSelectionDialog);
    mSelectionDialog->setHeadingWidget(headingLabel);
    mSelectionDialog->open(this, SLOT(selectionDialogClosed(HbAction *)));
}

/*!
    Handles the action from selection dialog.

    \param action Dialog closing action.
*/
void NmIpsSettingsMultiSelectionItem::selectionDialogClosed(HbAction *action)
{
    if (action == mSelectionDialog->primaryAction()) {
        // Get selected items.
        mSelectedItems = mSelectionDialog->selectedItems();

        // Generate button text from selected items.
        generateButtonText();

        QMap<QString, QVariant> properties;
        properties.insert("selectedItems", mSelectedItems);

        emit propertyChanged(properties);

        // Emit the signal.
        emit editingFinished();
    }
}

/*!
    Generates shown text from selected items.
*/
void NmIpsSettingsMultiSelectionItem::generateButtonText()
{
    // Generate button text only if both properties are valid.
    const int itemCount(mItems.count());
    if (mItems.count() >= mSelectedItems.count()) {
        // Construct separator for button text.
        QChar groupSeparator = HbExtendedLocale::system().groupSeparator();
        QString separator(" ");
        separator.insert(0, groupSeparator);

        QString buttonText;
        QListIterator<QVariant> itemIterator(mSelectedItems);
        while (itemIterator.hasNext()) {
            QVariant item(itemIterator.next());
            bool conversionOk(false);
            const int selectedIndex(item.toInt(&conversionOk));
            if (conversionOk && (selectedIndex >= 0) && (selectedIndex < itemCount)) {
                // Generate button text.
                buttonText.append(mItems.at(selectedIndex));
                // Add separator if not last item.
                if (itemIterator.hasNext()) {
                    buttonText.append(separator);
                }
            }
        }
        mButton->setText(buttonText);
    }
}