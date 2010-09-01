/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Central repository key definitions for project MRUI
*
*/

#ifndef MRUIPRIVATECRKEYS_H
#define MRUIPRIVATECRKEYS_H

#include "mruidomaincrkeys.h"
#include "esmrinternaluid.h"

/**
 * MRUI Previous locations history repository uid.
 * Repository contains editor location field history settings.
 * 
 * Usage:
 * 
 * Product-configurable maximum amount of history items can be read from key
 * <c>KESMRUILocationHistoryItemMaxCount<c>.
 * 
 * Number of current history items can be read from key
 * <c>KESMRUILocationHistoryItemCount<c>
 * 
 * History items can be read from key range.
 * Number of initialized keyrange keys can be read from key
 * <c>KESMRUILocationHistoryItemKeyRangeCount<c>
 * 
 * Keys for reading history items must be formed by bitwise-or of
 * <c>KESMRUILocationHistoryItemFirstInt<c> and
 * <c>KESMRUILocationHistoryItemKeyMask<c>.
 * This creates a bitmask for accessing the key. The index of the actual key
 * must be added to this bitmask. Note that the first key index is defined by
 * <c>KESMRUILocationHistoryItemFirstIndex<c>.
 */
const TUid KCRUidESMRUIPreviousLocations = { KESMRUIPreviousLocationsCRUid };

/**
 * Read-only key for history list items maximum count.
 * 
 * Default value: 10
 * 
 * Integer data.
 */
const TUint32 KESMRUILocationHistoryItemMaxCount = 0x1;

/**
 * Key for current number of history items.
 * 
 * Default value: 0
 * 
 * Integer data.
 */
const TUint32 KESMRUILocationHistoryItemCount = 0x2;

/**
 * Key for current item list sort order.
 * 
 * Default value: "" (KNullDesC8)
 * 
 * 8-bit binary data (string8).
 */
const TUint32 KESMRUILocationHistoryItemOrder = 0x3;

// History item key range

/**
 * Key for current history items key range key count.
 * Number of initialized keys in the key range.
 * Key range uses 8-bit index mask providing maximum amount of 254 separate keys.
 * 
 * Default value: 0
 * 
 * Integer data.
 */
const TUint32 KESMRUILocationHistoryItemKeyRangeCount = 0x1000;

/**
 * First key in range keys.
 */
const TUint32 KESMRUILocationHistoryItemFirstInt = 0x1001;

/**
 * Last key in range keys.
 */
const TUint32 KESMRUILocationHistoryItemLastInt = 0x1fff;

/**
 * Bitmask for key index.
 */
const TUint32 KESMRUILocationHistoryItemIndexBits = 0x0ff0;

/**
 * Bitmask for creating actual keys. Adding this bitmask to
 * First int creates first actual key.
 * 
 */
const TUint32 KESMRUILocationHistoryItemIndexMask = 0x0010;

/**
 * Staring index stored in index bits
 */
const TUint32 KESMRUILocationHistoryItemFirstIndex = 1;

/**
 * Sub-key for History item data.
 * 
 * Default value: "" (KNullDesC8)
 * 
 * 8-bit binary data (string8).
 */
const TUint32 KESMRUILocationHistoryItemKeyMask = 0x0001;

#endif // MRUIPRIVATECRKEYS_H
