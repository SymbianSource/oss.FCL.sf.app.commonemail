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

#ifndef NMIPSSETTINGSCUSTOMITEM_H
#define NMIPSSETTINGSCUSTOMITEM_H

#include <hbdataformviewitem.h>
#include <hbdataformmodelitem.h>

class QGraphicsItem;
class HbAbstractViewItem;
class QModelIndex;
class HbWidget;

class NmIpsSettingsCustomItem : public HbDataFormViewItem
{
    Q_OBJECT

public:

    enum { LabeledComboBox = HbDataFormModelItem::CustomItemBase + 10,
           TimeEditor };

    explicit NmIpsSettingsCustomItem(QGraphicsItem *parent=0);
    virtual ~NmIpsSettingsCustomItem();
    virtual HbAbstractViewItem* createItem();
    virtual bool canSetModelIndex(const QModelIndex &index) const;

public slots:

    void restore();
	
protected:

    virtual HbWidget* createCustomWidget();

private:

    void setWidgetProperty(const QString &property, const QHash<QString, QVariant> &properties);

};

#endif // NMIPSSETTINGSCUSTOMITEM_H
