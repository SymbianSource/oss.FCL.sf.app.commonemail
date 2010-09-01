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
* Description:  Observer for FsControlBar class
*
*/


#ifndef M_FSCONTROLBAROBSERVER_H
#define M_FSCONTROLBAROBSERVER_H

/**
 *  Control Bar observer class.
 *
 *  @code
 *
 *  @endcode
 *
 *  @lib fs_generic.lib
 */
NONSHARABLE_CLASS( MFsControlBarObserver )
    {
public:
    /** events sent to observers */
    enum TFsControlBarEvent
        {
        EEventSelectionChanged = 0,
        EEventFocusGained,
        EEventFocusLost,
        EEventFocusLostAtBottom,
        EEventFocusLostAtSide,
        // <cmail> Touch
        EEventTouchFocused,
        // </cmail>
        EEventButtonPressed,
        EEventButton,
        // <cmail> Touch
        EEventButtonReleased,
        EEventButtonTouched,
        // </cmail>
        EEventFocusVisibilityChanged
        };

public:
    /**
     * Invoked when event occurs.
     *
     * @param aEvent event type.
     * @param aData additional data. For EButton - id of pressed button.
     */
    virtual void HandleControlBarEvent(
        TFsControlBarEvent aEvent,
        TInt aData ) = 0;
    };

#endif // M_FSCONTROLBAROBSERVER_H
