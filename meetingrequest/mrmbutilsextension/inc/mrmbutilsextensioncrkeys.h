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
#ifndef MRMBUTILSCRKEYS_H
#define MRMBUTILSCRKEYS_H

#include "esmrinternaluid.h"

/**
 * MRUI Feature local variation repository uid.
 * The repository contains read-only keys for supported features.
 * Values are on/off settings stored as integer data.
 */
const TUid KCRUidMRDefaultCMailMRMailbox = { KMRDefaultCMailMRMailbox };

/**
 * Link to S60 MR Framework Default mailbox setting.
 * 
 * Default value: 0 (invalid)
 */
const TUint32 KMRUIS60DefMailbox = 0x1;

/**
 * Defines the selected CMAIL mailboxes plug-in UID.
 * 
 * Default value: 0 (invalid)
 */
const TUint32 KMRUICMailMailboxUID = 0x2;

/**
 * Defines the selected CMAIL's mailboxes ID.
 * 
 * Default value: 0 (invalid)
 */
const TUint32 KMRUICMailMailboxID = 0x3;

const TInt KMRUICMailInvalidIntValue( 0 );

#endif // MRMBUTILSCRKEYS_H
