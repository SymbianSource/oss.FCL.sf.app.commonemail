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
* Description:  MR observer for physics handling
*
*/

#ifndef MMRPHYSICSOBSERVER_H
#define MMRPHYSICSOBSERVER_H

//<cmail>
#include "esmrdef.h"
//</cmail>

/**
 *  Observer interface for physics event's end handling
 *
 *  @see cesmrlistpane.h
 *  @lib esmrgui.lib
 */
 class MMRPhysicsObserver
    {
public:
    /**
     * Method to be called when physics wants to notify that physics operation 
     * has ended. By calling this, list pane updates for example 
     * scroll bar and focus.
     */
    virtual void PhysicsEmulationEnded() = 0;
    
    /**
     * Method to be called when physics wants to notify scroll bar that 
     * view position has changed, so that scroll bar can update itself
     */
    virtual void UpdateScrollBarDuringOngoingPhysics() = 0;
    };

#endif // MMRPHYSICSOBSERVER_H
