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
* Description:  MR observer for scroll bar position updating
*
*/

#ifndef MMRSCROLLBAROBSERVER_H
#define MMRSCROLLBAROBSERVER_H

//<cmail>
#include "esmrdef.h"
//</cmail>

/**
 *  Observer interface for the scroll bar position handling
 *
 *  @see cesmrview.h
 *  @lib esmrgui.lib
 */
 class MMRScrollBarObserver
    {
public:
    /**
     * Method to be called when list pane wants to notify list component 
     * that scroll bar thumb position needs to be updated.
     */
    virtual void ScrollBarPositionChanged( TInt aThumbPosition ) = 0;
    };

#endif // MMRPHYSICSOBSERVER_H
