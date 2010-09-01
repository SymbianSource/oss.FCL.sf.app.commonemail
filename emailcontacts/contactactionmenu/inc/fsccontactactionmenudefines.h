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
* Description:  Common defines for Contact Action Menu
*
*/


#ifndef FSCCONTACTACTIONMENUDEFINES_H
#define FSCCONTACTACTIONMENUDEFINES_H

#include <e32base.h>

// DEFINES

// FORWARD DECLARATIONS

// TYPEDEFS

// CONSTS
const TUid KFscDefaultItemUid = { 0 };

// ENUMS

/**
 * Action Menu positions
 *  Position is used by client application to specify 
 *  Action Menu's vertical position
 */
// <cmail> Touch
enum TFscContactActionMenuPosition
    {
    EFscTop,
    EFscCenter,
    EFscBottom,
    EFscCustom
    };
// </cmail>

/**
 * Action Menu modes
 *  In EFscContactActionMenuModeNormal mode Action Menu works 
 *  like a normal modal dialog. In EFscContactActionMenuModeAI mode 
 *  number key events close the menu without consuming the events. 
 *  AI Mode is intended to be used in Active Idle when it must be 
 *  possible to start an emergency call while Action Menu is open
 */
enum TFscContactActionMenuMode
    {
    EFscContactActionMenuModeNormal,
    EFscContactActionMenuModeAI
    };

/**
 * Action Menu execution result codes
 * 
 *   EFscMenuDismissed is used when Action Menu is closed with cancel key etc.
 *   EFscMenuDismissedAIMode is used when menu is in AI mode and for example 
 *     numberic key is pressed 
 *   EFscCasItemSelectedAndExecuted is used when CAS item was selected from the menu
 *   EFscCustomItemSelected is used when custom (non-CAS) item was selected from the menu. 
 *     Client of AM needs to take care of custom action execution.
 */
enum TFscActionMenuResult
    {
    EFscMenuDismissed,
    EFscMenuDismissedAIMode,
    EFscCasItemSelectedAndExecuting,
    EFscCasItemSelectedAndExecuted,
    EFscCustomItemSelected
    };

// <cmail>
/**
 * Interface for asking the menu launcher for menu position
 */
class MFsActionMenuPositionGiver
    {
public:
    /**
     * The action menu asks through this function where it should place itself
     * 
     * @return The action menu position
     */
    virtual TPoint ActionMenuPosition() = 0;
    };

// </cmail>

#endif // FSCCONTACTACTIONMENUDEFINES_H
