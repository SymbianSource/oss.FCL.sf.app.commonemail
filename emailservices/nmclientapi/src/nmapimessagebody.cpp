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

#include "nmapimessagebody.h"

namespace EmailClientApi
{
NmMessageBodyPrivate::NmMessageBodyPrivate()
{

}

NmMessageBodyPrivate::~NmMessageBodyPrivate()
{

}

/*
 * constructor for nmmessagebody
 */
NmMessageBody::NmMessageBody()
{
    d = new NmMessageBodyPrivate();
}

NmMessageBody::~NmMessageBody()
{

}
/*!
 * getter for total size of message body in bytes
 */
quint64 NmMessageBody::totalSize() const
{
    return d->totalSize;
}

/*!
 * getter for bytes available in local mailbox store
 */
quint64 NmMessageBody::fetchedSize() const
{
    return d->fetchedSize;
}

/*!
 * getter for fetched content
 */
QString NmMessageBody::content() const
{
    return d->content;
}

/*
 * setter for total size of message body in bytes
 */
void NmMessageBody::setTotalSize(quint64 size)
{
    d->totalSize = size;
}

/*
 * setter for bytes available on local mailbox store
 */
void NmMessageBody::setFetchedSize(quint64 size)
{
    d->fetchedSize = size;
}

/*
 * setter for content
 */
void NmMessageBody::setContent(const QString& content)
{
    d->content = content;
}
}

