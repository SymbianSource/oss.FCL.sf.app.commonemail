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
* Description:  Message extension interface
*
*/

#ifndef CCONNECTIONSTATUSQUERY_H
#define CCONNECTIONSTATUSQUERY_H

#include "cemailextensionbase.h"
#include "cfsmailcommon.h"
#include "FreestyleEmailUiConstants.h"

// interface UID value passed to CFSMailMessageBase::ExtensionL
const TUid KEmailConnectionStatusQueryExtensionUid = { 0x2002EA82 };

/**
 * Extension interface for querying connection status directly from plugin
 */
class CConnectionStatusQueryExtension : public CEmailExtension
    {

public:
    enum TConnectionStatus
        {
        // Reserve 0 for undefined status
        EUndefined,
        // Mailbox is disconnected
        EDisconnected,
        // Mailbox is connected
        EConnected,
        // Mailbox is synchronizing
        ESynchronizing
        };

public:

    virtual TConnectionStatus ConnectionStatus(
            const TFSMailMsgId& aMailboxId,
            TForcedStatus aForcedStatus) const = 0;

protected:

    inline CConnectionStatusQueryExtension();

    };

inline CConnectionStatusQueryExtension::CConnectionStatusQueryExtension() :
    CEmailExtension( KEmailConnectionStatusQueryExtensionUid )
    {
    }

#endif // CCONNECTIONSTATUSQUERY_H
