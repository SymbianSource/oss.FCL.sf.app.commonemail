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


#ifndef CMAILCENREPKEYS_H
#define CMAILCENREPKEYS_H

#include <e32std.h>

const TUint32 KCMailMailboxIdBase                 = 0x00000100;
const TUint32 KCMailPluginIdBase                  = 0x00000200;
const TUint32 KCMailWidgetContentIdBase           = 0x00000300;

// Repository Uid
const TUid KCRUidCmailWidget                      = {0x2001E272};

// Keys

// Constants for observing the whole repository
const TUint32 KCmailPartialKey                    = 0x00000000;
const TUint32 KCmailMask                          = 0x00000000;

// Constants that define/specify mailbox entries
const TUint32 KCmailPartialKeyRange               = 0x00000100;
const TUint32 KCmailRangeMask                     = 0x00000f00;

const TUint32 KCMailMailbox1                      = 0x100;
const TUint32 KCMailMailbox2                      = 0x101;   
const TUint32 KCMailMailbox3                      = 0x102;
const TUint32 KCMailMailbox4                      = 0x103;
const TUint32 KCMailMailbox5                      = 0x104;   
const TUint32 KCMailMailbox6                      = 0x105;
const TUint32 KCMailMailbox7                      = 0x106;
const TUint32 KCMailMailbox8                      = 0x107;   
const TUint32 KCMailMailbox9                      = 0x108;
const TUint32 KCMailMailbox10                     = 0x109;
const TUint32 KCMailMailbox11                     = 0x10a;   
const TUint32 KCMailMailbox12                     = 0x10b;
const TUint32 KCMailMailbox13                     = 0x10c;
const TUint32 KCMailMailbox14                     = 0x10d;   
const TUint32 KCMailMailbox15                     = 0x10e;
const TUint32 KCMailMailbox16                     = 0x10f;
const TUint32 KCMailMailbox17                     = 0x110;   
const TUint32 KCMailMailbox18                     = 0x111;

const TUint32 KCMailPlugin1                       = 0x200;
const TUint32 KCMailPlugin2                       = 0x201;   
const TUint32 KCMailPlugin3                       = 0x202;
const TUint32 KCMailPlugin4                       = 0x203;
const TUint32 KCMailPlugin5                       = 0x204;   
const TUint32 KCMailPlugin6                       = 0x205;
const TUint32 KCMailPlugin7                       = 0x206;
const TUint32 KCMailPlugin8                       = 0x207;   
const TUint32 KCMailPlugin9                       = 0x208;
const TUint32 KCMailPlugin10                      = 0x209;
const TUint32 KCMailPlugin11                      = 0x20a;   
const TUint32 KCMailPlugin12                      = 0x20b;
const TUint32 KCMailPlugin13                      = 0x20c;
const TUint32 KCMailPlugin14                      = 0x20d;   
const TUint32 KCMailPlugin15                      = 0x20e;
const TUint32 KCMailPlugin16                      = 0x20f;
const TUint32 KCMailPlugin17                      = 0x210;   
const TUint32 KCMailPlugin18                      = 0x211;

// Constants that assosiates Home Screen widget instances to mailbox settings
const TUint32 KCMailWidgetContentId1              = 0x300;
const TUint32 KCMailWidgetContentId2              = 0x301;   
const TUint32 KCMailWidgetContentId3              = 0x302;
const TUint32 KCMailWidgetContentId4              = 0x303;
const TUint32 KCMailWidgetContentId5              = 0x304;   
const TUint32 KCMailWidgetContentId6              = 0x305;
const TUint32 KCMailWidgetContentId7              = 0x306;
const TUint32 KCMailWidgetContentId8              = 0x307;   
const TUint32 KCMailWidgetContentId9              = 0x308;
const TUint32 KCMailWidgetContentId10             = 0x309;
const TUint32 KCMailWidgetContentId11             = 0x30a;   
const TUint32 KCMailWidgetContentId12             = 0x30b;
const TUint32 KCMailWidgetContentId13             = 0x30c;
const TUint32 KCMailWidgetContentId14             = 0x30d;   
const TUint32 KCMailWidgetContentId15             = 0x30e;
const TUint32 KCMailWidgetContentId16             = 0x30f;
const TUint32 KCMailWidgetContentId17             = 0x310;   
const TUint32 KCMailWidgetContentId18             = 0x311;
#endif // CMAILCENREPKEYS_H
