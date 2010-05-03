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

#include <QModelIndex>
#include <HbPushButton>
#include <HbAbstractViewItem>
#include <HbDataFormModel>
#include <HbDataFormModelItem>
#include <HbAbstractItemView>

#include "nmsettingsformcustomitems.h"


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
    Sets the custom widget's properties from the model item.
*/
void NmSettingsFormCustomItems::restore()
{
    HbDataFormModelItem::DataItemType itemType = static_cast<HbDataFormModelItem::DataItemType>(
        modelIndex().data(HbDataFormModelItem::ItemTypeRole).toInt());
     if (itemType==NmButtonItem) {

         HbDataFormModel* model = static_cast<HbDataFormModel*>
             (static_cast<HbAbstractViewItem*>(this)->itemView()->model());
         HbDataFormModelItem* modelItem = model->itemFromIndex(modelIndex());
         QHash<QString ,QVariant> properties =
             modelItem->data(HbDataFormModelItem::PropertyRole).toHash();
         QList <QString> propertyNames = properties.keys();

         for (int index=0; index < propertyNames.count(); index++) {
             QString propName = propertyNames.at(index);
             dataItemContentWidget()->setProperty(propName.toAscii().data(),
                                                  properties.value(propName));
         }
     }
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
            widget = new HbPushButton();
        }
        default: {
            break;
        }
    }

    return widget;
}


// End of file.
