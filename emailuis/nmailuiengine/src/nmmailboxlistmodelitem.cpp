/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "nmuiengineheaders.h"


/*!
    \class NmMailboxListModelItem
    \brief The NmMailboxListModelItem class represents data model for mailbox list.
*/

/*!
    Constructor
 */
NmMailboxListModelItem::NmMailboxListModelItem()
:QStandardItem(),
mMailbox(NULL)
{
}

/*!
    Destructor
 */
NmMailboxListModelItem::~NmMailboxListModelItem()
{
    delete mMailbox;
}

/*!
    setItemMetaData. Set function for item related meta data object
 */
void NmMailboxListModelItem::setItemMetaData(NmMailboxMetaData *mailbox)
{
    if (mMailbox) {
        delete mMailbox;
    }
    mMailbox = mailbox;
    emitDataChanged();
}

/*!
    getItemMetaData. Get function for item related meta data object
 */
NmMailboxMetaData *NmMailboxListModelItem::itemMetaData()
{
    return mMailbox;
}

/*!

 */
void NmMailboxListModelItem::callEmitDataChanged()
{
    emitDataChanged();
}

