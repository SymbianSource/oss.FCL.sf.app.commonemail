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

#ifndef NMAPICOMMON_H_
#define NMAPICOMMON_H_

#include <QMetaType>

namespace EmailClientApi
{
    /*!
       Enum to describe mailbox events
     */
    enum NmApiMailboxEvent
    {
        MailboxCreated, MailboxDeleted
    };
    
    /*!
       Enum to describe message events.
     */
    enum NmApiMessageEvent 
    {
        MessageCreated,
        MessageDeleted,
        MessageChanged
    };
    
    /*!
       Enum to describe type of email folder 
     */
    enum NmApiEmailFolderType
    {
        Inbox, Drafts, Outbox, Sent, Deleted, EOther
    };
    
    /*!
       Enum to describe error types
     */
    enum NmApiError
    {
        GeneralError = -1, NotSupportedError
    };
    
}
Q_DECLARE_METATYPE (EmailClientApi::NmApiMailboxEvent)
Q_DECLARE_METATYPE(EmailClientApi::NmApiMessageEvent)
Q_DECLARE_METATYPE (EmailClientApi::NmApiEmailFolderType)
Q_DECLARE_METATYPE (EmailClientApi::NmApiError)

#endif /* NMAPICOMMON_H_ */
