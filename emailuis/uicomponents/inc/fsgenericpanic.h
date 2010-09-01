/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Panic codes used in fs_generic.dll
*
*/


#ifndef FS_GENERIC_PANIC_H
#define FS_GENERIC_PANIC_H


enum TFsGenericPanicReason
    {
    // panic codes related to list
    EFsListPanicInvalidItemId,
    EFsListPanicInvalidItemType,
    EFsListPanicIndexOutOfRange,
    EFsListPanicBorderNotSet,
    EFsListPanicInvalidComponentState,
    EFsListPanicMovingNodeToItsChild,
    // panic codes related to text viewer
    // panic codes related to fast addressing list
    EFsFastAddressingListBadIconId,
    EFsFastAddressingListTextureNotSet,
    // panic codes related to scroll bar
    // panic codes related to text parser
    // panic codes related to texture loader
    // panic codes related to control bar
    EFsControlBarNoButtonWithSpecifiedId,
    EFsControlBarButtonWithSpecifiedIdAlreadyExists,
    EFsControlBarUnknownEvent,
    // panic codes related to control button
    EFsControlButtonCannotSetFocusToDisabledControl,
    EFsControlButtonCannotDisableFocusedControl,
    EFsControlButtonIncorrectButtonType,
    EFsControlButtonIncorrectButtonElement,
    EFsControlButtonNoSuchEnumValue,
    EFsControlButtonEnumValueNotSupported,
    EFsControlButtonIncorrectButtonIndex,
    EFsControlButtonIncorrectButtonId,
    EFsControlButtonIdRestricted
    };

/**
 * The generic panic function used by all components in fs_generic.dll
 */
void FsGenericPanic( const TFsGenericPanicReason aReason );

#endif // FS_GENERIC_PANIC_H
