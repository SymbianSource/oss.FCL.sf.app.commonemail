/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef CMAILCENREPKEYSINTERNAL_H
#define CMAILCENREPKEYSINTERNAL_H

#include <e32std.h>

#include "cmailwidgetcenrepkeys.h"

// Key to a bit mask value containing various configuration items
// (see also constants below)
const TUint32 KCmailCPConfiguration = 0x10000001;

// 1st bit: whether email content publisher listens to commands/events sent by Homescreen framework
//     value == 0: default behaviour, events are handled as normal
//     value != 0: publisher ignores HS events
const TUint32 KCMailIgnoreHsEvents = 1;

#endif // CMAILCENREPKEYSINTERNAL_H
