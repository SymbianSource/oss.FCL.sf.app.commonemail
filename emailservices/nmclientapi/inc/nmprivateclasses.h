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

#ifndef PRIVATECLASSES_H
#define PRIVATECLASSES_H

#include "nmenginedef.h"
#include <QtCore>
#include <QMetaType>

/*!
 * Enum for internal info about changes in emial store
 */
enum EApiAction
{
    ENew, EChange, EDeleted
};

/*!
 * Enum for internal info about object type when change in email store is done.
 */
enum EApiObjectType
{
    EMailbox, EFolder, EMessage
};

/*!
 * It keeps data with information about change in email store.
 * 
 * It is grouped in list of object by event when mailbox change and by
 * event, mailboxId anf folderId when messages change.
 */
struct NmApiMessage
{
    EApiAction action;
    EApiObjectType objectType;

    QList<quint64> objectIds;

    quint64 mailboxId;
    quint64 folderId;

};
Q_DECLARE_METATYPE(NmApiMessage)

#endif /* PRIVATECLASSES_H_ */
