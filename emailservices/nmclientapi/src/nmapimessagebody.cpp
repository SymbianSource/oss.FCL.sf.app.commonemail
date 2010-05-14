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

#include <nmapimessagebody.h>
#include "nmapimessagebody_p.h"

namespace EmailClientApi
{

/*!
   constructor for NmApiMessageBody
 */
NmApiMessageBody::NmApiMessageBody()
{
    d = new NmApiMessageBodyPrivate();
}

/*!
   copying constructor for NmApiMessageBody
 */
NmApiMessageBody::NmApiMessageBody(const NmApiMessageBody &apiMessageBody) : d(apiMessageBody.d)
{

}

/*!
   assignment operator for nmapimessagebody
 */
NmApiMessageBody& NmApiMessageBody::operator=(const NmApiMessageBody &apiMessageBody)
{
	if (this != &apiMessageBody) 
	{
		d = apiMessageBody.d;
	}
	return *this;
}

/*!
   Compare data from \a apiMessageBody
 */    
bool NmApiMessageBody::operator==(const NmApiMessageBody &apiMessageBody)
{
    bool retVal = false;
    if (this->d == apiMessageBody.d)
    {
        retVal = true;
    }
    return retVal;
}

/*!
   Destructor for NmApiMessageBody
 */
NmApiMessageBody::~NmApiMessageBody()
{

}
/*!
   getter for total size of message body in bytes
 */
quint64 NmApiMessageBody::totalSize() const
{
    return d->totalSize;
}

/*!
   getter for bytes available in local mailbox store
 */
quint64 NmApiMessageBody::fetchedSize() const
{
    return d->fetchedSize;
}

/*!
   getter for fetched content
 */
QString NmApiMessageBody::content() const
{
    return d->content;
}

/*!
   setter for total size of message body in bytes
 */
void NmApiMessageBody::setTotalSize(quint64 size)
{
    d->totalSize = size;
}

/*!
   setter for bytes available on local mailbox store
 */
void NmApiMessageBody::setFetchedSize(quint64 size)
{
    d->fetchedSize = size;
}

/*!
   setter for content
 */
void NmApiMessageBody::setContent(const QString& content)
{
    d->content = content;
}
}

