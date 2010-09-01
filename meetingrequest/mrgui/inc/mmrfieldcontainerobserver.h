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
* Description:  MR observer for the custom list pane
*
*/

#ifndef MMRFIELDCONTAINEROBSERVER_H
#define MMRFIELDCONTAINEROBSERVER_H

//<cmail>
#include "esmrdef.h"
//</cmail>

class CESMRField;

/**
 *  Observer interface for scrolling fields up and down.
 *
 *  @see cmrlistpane.h
 *  @lib esmrgui.lib
 */
 class MMRFieldContainerObserver
    {
public:
    /**
     * Scrolls list items up
     * @param aPx Amount of scroll in pixels
     */
    virtual void ScrollFieldsUp( TInt aPx ) = 0;
    
    /**
     * Scrolls list items down
     * @param aPx Amount of scroll in pixels
     */
    virtual void ScrollFieldsDown( TInt aPx ) = 0;
    
    /**
     * Tells to the list pane that scroll bar 
     * and physics require updating.
     *
     * @param aPx Amount of scroll in pixels
     */
    virtual void UpdateScrollBarAndPhysics() = 0;
    
    };

#endif // MMRFIELDCONTAINERPOSITIONOBSERVER_H
