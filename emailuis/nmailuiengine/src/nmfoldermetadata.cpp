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
    \class NmFolderMetaData
    \brief The NmFolderMetaData class represents data for NmFolderListModel.
    @alpha

    The class NmFolderMetaData provides an item that is used by NmFolderListModel and its' data
    method to get all information needed for one list row for a widget by calling the method once.
*/

class NmFolderMetaDataPrivate
{
public:
    QString mName;
    NmId mId;
};


/*!
    Constructor.
*/
NmFolderMetaData::NmFolderMetaData()
{
    d = new NmFolderMetaDataPrivate;
}


/*!
    Destructor.
*/
NmFolderMetaData::~NmFolderMetaData()
{
    delete d;
}

/*!
    Mailbox name accessor.
*/
QString NmFolderMetaData::name() const
{
    return d->mName;
}

/*!
    Set Mailbox name as \a name.
*/
void NmFolderMetaData::setName(const QString &name)
{
    d->mName = name;
}

/*!
    Mailbox id.
*/
NmId NmFolderMetaData::id() const
{
    return d->mId;
}

/*!
    Set folder id as \a id.
*/
void NmFolderMetaData::setId(const NmId &id)
{
    d->mId = id;
}








