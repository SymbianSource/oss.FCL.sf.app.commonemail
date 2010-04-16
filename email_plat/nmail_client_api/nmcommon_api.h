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

#ifndef NMAPICOMMON_API_H_
#define NMAPICOMMON_API_H_

#include <QtCore>

/*!
 * Enum to describe mailbox events
 */
enum MailboxEvent
{
    MailboxCreated, MailboxDeleted
};
Q_DECLARE_METATYPE ( MailboxEvent)
/*!
 * Enum to describe message events.
 */
enum MessageEvent {
    MessageCreated,
    MessageDeleted,
    MessageChanged    
};

Q_DECLARE_METATYPE(MessageEvent)
#endif /* NMCOMMON_API_H_ */
