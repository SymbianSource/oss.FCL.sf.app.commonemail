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
* Description:  Defines for Action Menu.
 *
*/


#ifndef T_FSACTIONMENUDEFINES_H
#define T_FSACTIONMENUDEFINES_H

//CONSTANTS
const TInt KFsGroupActionMenu = 1024;

const TInt KFsActionMenuScrollTime = 300;

const TReal KFsActionMenuAccelerationRate = 0.4;
//ENUMS

/**
 * Action menu result codes.
 */
enum TFsActionMenuResult
    {
    /**
     * Action menu was dismissed by e.g. cancel key 
     */
    EFsMenuDismissed,
    
    /**
     * Item was selected.
     */
    EFsMenuItemSelected
    };

/**
 * Action menu's vertical position.
 */
enum TFsVActionMenuPosition
    {
    EFsVPosTop,
    EFsVPosCenter,
    EFsVPosBottom
    };

#endif //T_FSACTIONMENUDEFINES_H
