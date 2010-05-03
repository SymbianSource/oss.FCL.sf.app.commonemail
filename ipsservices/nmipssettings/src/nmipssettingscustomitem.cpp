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
#include <hbdatetimeedit.h>
#include <hbabstractitemview.h>
#include <hbdataformmodel.h>
#include <hbdataformmodelitem.h>

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
    return type==LabeledComboBox || type==TimeEditor;
}

/*!
    Sets the custom widget's properties from the model item.
*/
void NmIpsSettingsCustomItem::restore()
{
    HbDataFormModelItem::DataItemType itemType = static_cast<HbDataFormModelItem::DataItemType>(
        modelIndex().data(HbDataFormModelItem::ItemTypeRole).toInt());

    if (itemType==LabeledComboBox || itemType==TimeEditor) {

        HbDataFormModel* model = static_cast<HbDataFormModel*>
            (static_cast<HbAbstractViewItem*>(this)->itemView()->model());
        HbDataFormModelItem* modelItem = model->itemFromIndex(modelIndex());
        QHash<QString ,QVariant> properties =
            modelItem->data(HbDataFormModelItem::PropertyRole).toHash();

         if (itemType==TimeEditor) {
             // Set time editor properties. Simply copy all set properties to the widget.
             QStringList propertyNames = properties.keys();

             for (int index=0 ; index < propertyNames.count() ; index++) {
                 QString propName = propertyNames.at(index);
                 dataItemContentWidget()->setProperty(propName.toAscii().data(),
                                                      properties.value(propName));
             }
         } else {
             // Set combobox properties in specific order. currentIndex must be set last so that
             // both the labelTexts and comboItems have been set before. Also, labelTexts must be
             // set before comboItems.
             setWidgetProperty("labelTexts", properties);
             setWidgetProperty("comboItems", properties);
             setWidgetProperty("currentIndex", properties);
         }
    }
}

/*!
    Creates the custom widget.
*/
HbWidget *NmIpsSettingsCustomItem::createCustomWidget()
{
    QVariant data(modelIndex().data(HbDataFormModelItem::ItemTypeRole));
    int type(data.toInt());

    HbWidget *widget = 0;

    switch (type) {
            case LabeledComboBox: {
                widget = new NmIpsSettingsLabeledComboBox();
                break;
                }
            case TimeEditor: {
                HbDateTimeEdit *edit = new HbDateTimeEdit();
                widget = edit;
                break;
                }
            default: {
                break;
                }
    }

    return widget;
}

/*!
    Sets \a property to the content widget if found from \a properties.
    \param property Name of the property to set.
    \param properties Available properties.
*/
void NmIpsSettingsCustomItem::setWidgetProperty(const QString &property,
    const QHash<QString, QVariant> &properties)
{
    if (properties.contains(property)) {
        dataItemContentWidget()->setProperty(property.toAscii().data(), properties.value(property));
    }
}

