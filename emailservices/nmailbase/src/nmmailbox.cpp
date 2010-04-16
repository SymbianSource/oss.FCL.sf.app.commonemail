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
#include <QString>

#include "nmmailbox.h"

NmMailboxPrivate::NmMailboxPrivate()
{
}

NmMailboxPrivate::~NmMailboxPrivate()
{
}

/*!
    \class NmMailbox
    \brief Data model for mailbox spesific data
 */

/*!
    Constructs NmMailbox object with empty data
 */
NmMailbox::NmMailbox()
{
    d = new NmMailboxPrivate();
}

/*!
    Copy constructor
 */
NmMailbox::NmMailbox(const NmMailbox &mailbox)
{
    d = mailbox.d;
}

/*!
    Constructs NmMailbox object from \a mailbox data
 */
NmMailbox::NmMailbox(QExplicitlySharedDataPointer<NmMailboxPrivate> mailboxprivate)
{
    d = mailboxprivate;
}

/*!
    Assign data from \a mailbox
 */
NmMailbox &NmMailbox::operator=(const NmMailbox &mailbox)
{
    if (this != &mailbox) {
        d = mailbox.d;
    }
    return *this;
}

/*!
    Destructor
 */
NmMailbox::~NmMailbox()
{
}

/*!
    Returns mailbox id
 */
NmId NmMailbox::id() const
{
    return d->mId;
}

/*!
    Sets mailbox id from \a id
 */
void NmMailbox::setId(const NmId& id)
{
    d->mId = id;
}

/*!
    Returns mailbox name string
 */
QString NmMailbox::name() const
{
    return d->mName;
}

/*!
    Sets mailbox name from \a name
 */
void NmMailbox::setName(const QString &name)
{
    d->mName = name;
}

/*!
    Equal operator returns true if both NmMailbox objects have same name and id
 */
bool NmMailbox::operator==(const NmMailbox &mailbox) const
{
    bool ret = true;

    if (this->name().compare(mailbox.name()) != 0) {
        ret = false;
    }
    if (this->id() != mailbox.id()) {
        ret = false;
    }
    return ret;
}


NmAddress NmMailbox::address()
{
    return d->mAddress;
}


void NmMailbox::setAddress(NmAddress& address)
{
    d->mAddress = address;
}

