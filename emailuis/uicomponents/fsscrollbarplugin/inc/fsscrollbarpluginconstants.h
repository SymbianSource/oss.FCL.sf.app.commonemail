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
* Description: This file defines constants for FsScrollBarPlugin
*
*/



#ifndef FSSCROLLBARPLUGINCONSTANTS_H
#define FSSCROLLBARPLUGINCONSTANTS_H

//<cmail> SF
#include <alf/alftimedvalue.h>
#include <alf/alfconstants.h>
//</cmail>
// <cmail> S60 UID update
const TInt KFsScrollbarImplementationId = 0x2001E282;
// </cmail> S60 UID update

// all the components in this factory
enum
    {
    EFsScrollbarCreateScreenLayout
    };

// Scrollbar layout commands
enum TFsScrollbarCommand
    {
    EFsScrollbarLayoutSetSizeCommand = EAlfLastReservedOpCmd + 1,
    //EFsScrollbarLayoutSetScrollbarVisibleCommand,
    EFsScrollbarLayoutSetScrollbarRangeCommand,
    EFsScrollbarLayoutSetScrollbarPosCommand,
    EFsScrollbarLayoutSetScrollbarVisibilityCommand,
    //EFsScrollbarLayoutGetScrollbarVisibilityCommand,
    EFsScrollbarLayoutGetScrollbarSettingsCommand,
    EFsScrollbarLayoutSetVerticalScrollbarWidthCommand,
    EFsScrollbarLayoutSetHorizontalScrollbarHeightCommand,
    EFsScrollbarLayoutSetScrollbarThumbSpanCommand,
    EFsScrollbarLayoutSetScrollbarCustomImagesCommand,
    EFsScrollbarLayoutSetScrollbarClearCustomImagesCommand,
    EFsScrollbarLayoutSetScrollbarUpdateCommand,
    EFsScrollbarLayoutGetScrollbarLayoutSizeCommand
    };

#endif // FSSCROLLBARPLUGINCONSTANTS_H
