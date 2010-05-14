/*
 * Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies). 
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
 * Description:  mailbox branding object
 *
 */

#include "emailmailboxinfo.h"
#include "emailmailboxinfo_p.h"

/*!
    private constructor of EmailMailboxInfo
 */
EmailMailboxInfo::EmailMailboxInfo()
{
    d = EmailMailboxInfoPrivate::getInstance();
}

/*!
    destructor of EmailMailboxInfo
 */
EmailMailboxInfo::~EmailMailboxInfo()
{
    EmailMailboxInfoPrivate::releaseInstance(d);
}

/*!
    Returns mailbox icon name 
    \param identifier can be domain name or mailbox id (NmId from nmailbase)
    \return mailbox icon name that can be used to construct HbIcon
 */
QString EmailMailboxInfo::mailboxIcon(const QVariant &identifier) const
{
    return d->icon(identifier);
}

/*!
    Returns branded mailbox name
    \param identifier can be domain name or mailbox id (NmId from nmailbase)
    \return mailbox name
 */
QString EmailMailboxInfo::mailboxName(const QVariant &identifier) const
{
    return d->name(identifier);
}

