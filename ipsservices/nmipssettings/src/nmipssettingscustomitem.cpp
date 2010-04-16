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

#include <QVariant>

#include "nmipssettingscustomitem.h"
#include "nmipssettingslabeledcombobox.h"

/*!
    \class NmIpsSettingsCustomItem
    \brief The class implements a custom HbDataFormViewItem.

*/

// ======== MEMBER FUNCTIONS ========

/*!
    Constructor of NmIpsSettingsCustomItem.
*/
NmIpsSettingsCustomItem::NmIpsSettingsCustomItem(QGraphicsItem *parent)
: HbDataFormViewItem(parent)
{
}

/*!
    Destructor of NmIpsSettingsCustomItem.
*/
NmIpsSettingsCustomItem::~NmIpsSettingsCustomItem()
{
}

/*!
    Creates the custom data form view item.
*/
HbAbstractViewItem* NmIpsSettingsCustomItem::createItem()
{
    return new NmIpsSettingsCustomItem(*this);
}

/*!
    \param index Model index of the item.
    \return true if item type for the index is supported, otherwise false.
*/
bool NmIpsSettingsCustomItem::canSetModelIndex(const QModelIndex &index) const
{
    int type(index.data(HbDataFormModelItem::ItemTypeRole).toInt());
    return type==LabeledComboBox;
}

/*!
    Creates the custom widget.
*/
HbWidget *NmIpsSettingsCustomItem::createCustomWidget()
{
    QVariant data(modelIndex().data(HbDataFormModelItem::ItemTypeRole));
    int type(data.toInt());

    HbWidget *widget = 0;

    if (type == LabeledComboBox) {
        widget = new NmIpsSettingsLabeledComboBox();
    }

    return widget;
}
