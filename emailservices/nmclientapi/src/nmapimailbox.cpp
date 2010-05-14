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

#include <nmapimailbox.h>
#include "nmapimailbox_p.h"

#include <QString>

namespace EmailClientApi
{

/*!
    constructor for NmApiMailbox 
 */
NmApiMailbox::NmApiMailbox()
{
    d = new NmApiMailboxPrivate();
}

/*!
    Copy constructor for NmApiMailbox 
 */
NmApiMailbox::NmApiMailbox(const NmApiMailbox &nmApiMailbox)
: d(nmApiMailbox.d)
{
    
}

/*!
   destructor for NmApiMailbox
 */
NmApiMailbox::~NmApiMailbox()
{

}

/*!
   Assign data from \a mailbox
 */
NmApiMailbox &NmApiMailbox::operator=(const NmApiMailbox &mailbox)
{
    if (this != &mailbox) {
        d = mailbox.d;
    }
    return *this;
}

/*!
   Compare data from \a mailbox
 */
bool NmApiMailbox::operator==(const NmApiMailbox &mailbox)
{
    bool returnValue = false;
    if (d == mailbox.d) {
        returnValue = true;
    }
    return returnValue;
}

/*!
   getter for id
 */
quint64 NmApiMailbox::id() const
{
    return d->id;
}

/*!
   getter for address
 */
QString NmApiMailbox::address() const
{
    return d->address;
}

/*!
   getter for name
 */
QString NmApiMailbox::name() const
{
    return d->name;
}

/*!
   setter for id
 */
void NmApiMailbox::setId(quint64 id)
{
    d->id = id;
}

/*!
   setter for address
 */
void NmApiMailbox::setAddress(const QString &address)
{
    d->address = address;
}

/*!
   setter for name
 */
void NmApiMailbox::setName(const QString &name)
{
    d->name = name;
}

}

