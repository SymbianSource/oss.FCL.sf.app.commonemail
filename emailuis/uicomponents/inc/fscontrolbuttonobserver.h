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
* Description:  Observer for FsControlButton class
*
*/


#ifndef M_FSCONTROLBUTTONOBSERVER_H
#define M_FSCONTROLBUTTONOBSERVER_H

/**
 *  Control Button observer class.
 *
 *  @code
 *
 *  @endcode
 *
 *  @lib fs_generic.lib
 */
NONSHARABLE_CLASS( MFsControlButtonObserver )
    {
public:
    /** events sent to observers */
    enum TFsControlButtonEvent
        {
        EEventButtonPressed,
        EEventButton,
        EEventButtonReleased,
        EEventButtonVisibilityChanged,
        // <cmail> Touch
        EEventButtonDimmStateChanged,
        EEventButtonTouchPressed,
        EEventButtonTouchReleased
        // </cmail>
        };

public:
    /**
     * Invoked when button is pressed.
     *
     * @param aEvent event type.
     * @param aId id of the pressed button.
     */
    virtual TBool HandleButtonEvent(
        TFsControlButtonEvent aEvent,
        TInt aId ) = 0;
    };

#endif // M_FSCONTROLBUTTONOBSERVER_H
