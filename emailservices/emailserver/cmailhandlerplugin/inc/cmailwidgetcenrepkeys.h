/*
* Copyright (c) 2008 - 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Constants for cmailhandlerplugin Central Repository keys.
*
*/


#ifndef CMAILWIDGETCENREPKEYS_H
#define CMAILWIDGETCENREPKEYS_H

#include <e32std.h>

// Repository Uid
const TUid KCRUidCmailWidget                      = {0x2001E272};

const TUint32 KCMailMailboxIdBase                 = 0x00000100;
const TUint32 KCMailPluginIdBase                  = 0x00000200;
const TUint32 KCMailWidgetContentIdBase           = 0x00000300;

// Constants for observing the whole repository
const TUint32 KCmailPartialKey                    = 0x00000000;
const TUint32 KCmailMask                          = 0x00000000;

// Constants that define internal mailbox entries
const TUint32 KCmailPartialKeyRange               = 0x00000100;
const TUint32 KCmailRangeMask                     = 0x0000ff00;

// Constants that define external mailbox entries
const TUint32 KCmailExtMailboxKeyRange            = 0x00001100;
const TUint32 KCmailExtMailboxRangeMask           = 0x0000ff00;

// External mailboxes' base indexes
const TUint32 KCMailExtMailboxBase                = 0x00001100;
const TUint32 KCMailExtPluginIdOffset             = 0x00000100;
const TUint32 KCMailExtWidgetCidOffset            = 0x00000200;

// Key to store mailboxes where is new messages. Format: <mailbox1><mailbox2><mailbox3>...
const TUint32 KCMailMailboxesWithNewMail          = 0x10000003;

// Key to variate widget setup brand icons
const TUint32 KCMailWidgetSetupVariant            = 0x10000004;
const TInt KWidgetSetupBrandNone                  = 0;
const TInt KWidgetSetupBrandDefault               = 1;
const TInt KWidgetSetupBrandChina                 = 2;

#endif // CMAILWIDGETCENREPKEYS_H
