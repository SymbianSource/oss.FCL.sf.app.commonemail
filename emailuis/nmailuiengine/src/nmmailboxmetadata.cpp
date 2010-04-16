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
    \class NmMailboxMetaData
    \brief The NmMailboxMetaData class represents data for NmMailboxListModel.
    @alpha

    The NmMailboxMetaData class provides an item that is used by NmMailboxListModel and its' data
    method to get all information needed for one list row for a widget by calling the method once.
*/

class NmMailboxMetaDataPrivate
{
public:
    QString mName;
    NmId mId;
    NmId mIconId;
};


/*!
    Constructor.
*/
NmMailboxMetaData::NmMailboxMetaData()
{
    d = new NmMailboxMetaDataPrivate;
}


/*!
    Destructor.
*/
NmMailboxMetaData::~NmMailboxMetaData()
{
    delete d;
}

/*!
    Mailbox name accessor.
*/
QString NmMailboxMetaData::name() const
{
    return d->mName;
}

/*!
    Set Mailbox name as \a name.
*/
void NmMailboxMetaData::setName(const QString &name)
{
    d->mName = name;
}

/*!
    Mailbox id.
*/
NmId NmMailboxMetaData::id() const
{
    return d->mId;
}

/*!
    Set Mailbox id as \a id.
*/
void NmMailboxMetaData::setId(const NmId& id)
{
    d->mId = id;
}

/*!
    Mailbox icon accessor. \note Temporary solution.
*/
NmId NmMailboxMetaData::IconId() const
{
    return d->mIconId;
}

/*!
    Set Mailbox icon as \a id. \note Temporary solution.
*/
void NmMailboxMetaData::setIconId(const NmId &id)
{
    d->mIconId = id;
}





