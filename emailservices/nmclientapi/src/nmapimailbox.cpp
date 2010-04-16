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

#include "nmapimailbox.h"

namespace EmailClientApi
{
NmMailboxPrivate::NmMailboxPrivate()
{

}

NmMailboxPrivate::~NmMailboxPrivate()
{

}

/*
 *  constructor for nmmailbox 
 */
NmMailbox::NmMailbox()
{
    d = new NmMailboxPrivate();
}

/*
 * destructor for nmmailbox
 */
NmMailbox::~NmMailbox()
{

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

/*
 * getter for id
 */
quint64 NmMailbox::id() const
{
    return d->id;
}

/*
 * getter for address
 */
QString NmMailbox::address() const
{
    return d->address;
}

/*
 * getter for name
 */
QString NmMailbox::name() const
{
    return d->name;
}

/*
 * setter for id
 */
void NmMailbox::setId(quint64 id)
{
    d->id = id;
}

/*
 * setter for address
 */
void NmMailbox::setAddress(const QString &address)
{
    d->address = address;
}

/* 
 * setter for name
 */
void NmMailbox::setName(const QString &name)
{
    d->name = name;
}

}

