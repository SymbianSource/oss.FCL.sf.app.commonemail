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
* Description:  ESMR observer for the custom list component
*
*/

#ifndef MESMRLISTOBSERVER_H
#define MESMRLISTOBSERVER_H

//<cmail>
#include "esmrdef.h"
//</cmail>

class CESMRField;

/**
 *  Observer interface for the custom list component.
 *
 *  @see cesmrlistcomponent.h
 *  @lib esmrgui.lib
 */
 class MESMRListObserver
    {
public:
    /**
     * Method to be called when field wants to notify that its size has changed.
     * By calling this, all the positions and sizes in the list are recalculated
     *
     * @param aCtrl - caller field.
     */
    virtual void ControlSizeChanged( CESMRField* aCtrl ) = 0;

    /**
     * Sets field to be visible in the list.
     *
     * @param aField - id of the field.
     */
    virtual void InsertControl( TESMREntryFieldId aField ) = 0;

    /**
     * Disables field in the list. This method does not delete field.
     *
     * @param aField - id of the field.
     */
    virtual void RemoveControl( TESMREntryFieldId aField ) = 0;

    /**
     * Is field visible or not.
     *
     * @return Boolean ETrue/EFalse
     */
    virtual TBool IsControlVisible( TESMREntryFieldId aField ) = 0;

    /**
     * Sets field to be focused.
     *
     * @param aField - id of the field.
     */
    virtual void SetControlFocusedL( TESMREntryFieldId aField ) = 0;

    /**
     * Scrolls the specific field to be fully visible
     *
     * @param aInd Index of the field that needs to be fully visible
     * @param aSizeChanged ETrue if the field size has been changed
     */
    virtual void ScrollItemVisible( TInt aInd ) = 0;

// SCROLLING_MOD: Two methods for CESMRRichTextViewer usage
    virtual void MoveListAreaDownL( TInt aAmount) = 0;
    virtual void MoveListAreaUpL(TInt aAmount) = 0;

    virtual TInt ListHeight() = 0;
    virtual TBool IsFieldBottomVisible() = 0;
    };

#endif // MESMRLISTOBSERVER_H
