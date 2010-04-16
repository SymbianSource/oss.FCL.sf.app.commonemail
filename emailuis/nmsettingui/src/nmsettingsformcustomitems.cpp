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

#include "nmsettingsformcustomitems.h"

#include <hbpushbutton.h>


/*!
    \class NmSettingsFormCustomItems
    \brief Contains definitions for custom setting data form items.
*/


/*!
    Class constructor.
*/
NmSettingsFormCustomItems::NmSettingsFormCustomItems(QGraphicsItem *parent)
: HbDataFormViewItem(parent)
{
}


/*!
    Class destructor.
*/
NmSettingsFormCustomItems::~NmSettingsFormCustomItems()
{
}


/*!
    From HbDataFormViewItem.
*/
HbAbstractViewItem *NmSettingsFormCustomItems::createItem()
{
    return new NmSettingsFormCustomItems(*this);
}

bool NmSettingsFormCustomItems::canSetModelIndex(const QModelIndex &index) const
{
    int type(index.data(HbDataFormModelItem::ItemTypeRole).toInt());
    return type==NmButtonItem;
}


/*!
    From HbDataFormViewItem.
*/
HbWidget *NmSettingsFormCustomItems::createCustomWidget()
{
    HbDataFormModelItem::DataItemType itemType =
        static_cast<HbDataFormModelItem::DataItemType>(
            modelIndex().data(HbDataFormModelItem::ItemTypeRole).toInt());

    HbWidget *widget = 0;

    switch (itemType) {
      case NmButtonItem: {
            // Push button.
            QString text = modelIndex().data(HbDataFormModelItem::KeyRole).toString();
            widget = new HbPushButton(text);
        }
        default: {
            break;
        }
    }

    return widget;
}


// End of file.
