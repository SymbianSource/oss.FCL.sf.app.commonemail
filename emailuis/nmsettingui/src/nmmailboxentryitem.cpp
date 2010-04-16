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

#include <cpitemdatahelper.h>
#include <hbicon.h>
#include <hbdataformmodelitem.h>

#include "nmmailboxentryitem.h"

/*!
    \class NmMailboxEntryItem
    \brief Class for implementing mailbox item data.

    Stores mailbox specific data, i.e. name, icon and id of the mailbox.

*/

// ======== MEMBER FUNCTIONS ========

/*!
    Constructor of NmMailboxEntryItem.
*/
NmMailboxEntryItem::NmMailboxEntryItem(
        CpItemDataHelper &itemDataHelper,
        const NmId &mailboxId,
        const QString &mailboxName,
        const HbIcon &mailboxIcon,
        const HbDataFormModelItem *parent,
        const QString &mailboxDescription)
: CpSettingFormEntryItemData(itemDataHelper, mailboxName, mailboxDescription, mailboxIcon, parent),
  mMailboxId(mailboxId.id())
{
}

/*!
    Destructor of NmMailboxEntryItem.
*/
NmMailboxEntryItem::~NmMailboxEntryItem()
{
}

/*!
    From base class. No implementation.
*/
CpBaseSettingView *NmMailboxEntryItem::createSettingView() const
{
	NMLOG("NmMailboxEntryItem::createSettingView");
    return 0;
}

/*!
    Return id for the mailbox.
*/
const NmId &NmMailboxEntryItem::id() const
{
    return mMailboxId;
}
