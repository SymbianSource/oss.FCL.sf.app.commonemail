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
* Description: Implementation of NmContactHistoryModelItem
*
*/

#include <QList>

#include "nmcontacthistorymodel.h"
#include "nmcontacthistorymodel_p.h"

// --------------------------------------------------------------------------
// Start of implementation (NmContactHistoryModelItem)
// --------------------------------------------------------------------------
/*!
     Not meant to be used alone.
*/
NmContactHistoryModelItem::NmContactHistoryModelItem()
{
    mItemData = new NmContactHistoryModelItemData();
}

/*!
    Not meant to be used alone.
*/
NmContactHistoryModelItem::NmContactHistoryModelItem(
    const NmContactHistoryModelItem &other)
{
    mItemData = other.mItemData;
}

/*!
    Not meant to be used alone.
*/
NmContactHistoryModelItem::~NmContactHistoryModelItem()
{
}

/*!
    Not meant to be used alone.
*/
unsigned int NmContactHistoryModelItem::contactId() const
{
    return mItemData->mContactId;
}

/*!
    Not meant to be used alone.
*/
void NmContactHistoryModelItem::setContactId(unsigned int contactId)
{
    mItemData->mContactId = contactId;
}

/*!
    Not meant to be used alone.
*/
int NmContactHistoryModelItem::subItemCount() const
{
    return mItemData->mItems.count();
}

/*!
    Not meant to be used alone.
*/
NmContactHistoryModelSubItem NmContactHistoryModelItem::subItemAt(
    int index) const
{
    if ( index <= mItemData->mItems.count() )
    {
        return mItemData->mItems[index];
    }
    else
    {
        return NmContactHistoryModelSubItem();
    }
}

/*!
    Returns SubEntries from private list
    First entry is contact name
    Second entry is email address / phone number

    \return List of items.
*/
QList<NmContactHistoryModelSubItem> NmContactHistoryModelItem::subEntries(
    ) const
{
    QList<NmContactHistoryModelSubItem> subItemList;

    if (!mItemData->mItems.isEmpty())
    {
        for ( int subIndex = 0; subIndex < mItemData->mItems.count(); subIndex++)
        {
            subItemList.append(mItemData->mItems[subIndex]);
        }
    }

    return subItemList;
}

/*!
    Public - Appends subItem to private list

    \param entry subItem to be appended
*/
void NmContactHistoryModelItem::appendSubItem(
    NmContactHistoryModelSubItem entry)
{
    mItemData->mItems.append(entry);
}
// --------------------------------------------------------------------------
// End of implementation (NmContactHistoryModelItem)
// --------------------------------------------------------------------------/*!
