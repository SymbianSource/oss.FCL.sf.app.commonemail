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

#ifndef EMAILMAILBOXINFO_H_
#define EMAILMAILBOXINFO_H_

#include <QString>
#include <QVariant>

#include "emailutilitiesdef.h"

class EmailMailboxInfoPrivate;

class EMAIL_UTILITIES_EXPORT EmailMailboxInfo
{
public:

    /*!
        constructs object
     */
    EmailMailboxInfo();

    /*!
        destroys object 
     */
    ~EmailMailboxInfo();

    /*!
        Returns mailbox icon name 
        \param identifier can be domain name or mailbox id (NmId from nmailbase)
        \return mailbox icon name that can be used to construct HbIcon
     */
    QString mailboxIcon(const QVariant &identifier) const;

    /*!
        Returns branded mailbox name
        \param identifier can be domain name or mailbox id (NmId from nmailbase)
        \return mailbox name
     */
    QString mailboxName(const QVariant &identifier) const;

private:
    EmailMailboxInfoPrivate *d;

};

#endif /* EMAILMAILBOXINFO_H_ */