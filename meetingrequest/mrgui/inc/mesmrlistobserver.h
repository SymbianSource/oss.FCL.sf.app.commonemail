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
 *  Observer interface for the custom list pane.
 *
 *  @see cesmrlistpane.h
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
    virtual void ShowControl( TESMREntryFieldId aField ) = 0;

    /**
     * Disables field in the list. This method does not delete field.
     *
     * @param aField - id of the field.
     */
    virtual void HideControl( TESMREntryFieldId aField ) = 0;

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
     */
    virtual void ScrollControlVisible( TInt aInd ) = 0;

    /**
     * Returns the height of the whole field container area containing all visible fields
     *
     * @return World height.
     */
    virtual TInt ListHeight() = 0;
    
    /**
     * Returns ETrue if focused fields bottom is visible, otherwise EFalse
     *
     * @return Boolean ETrue/EFalse
     */
    virtual TBool IsFocusedControlsBottomVisible() = 0;
    
    /**
     * Moves fields the amount of pixels given. 
     *
     * @param aAmount, amount of the movement in pixels. Positive down, negative up.
     */
    virtual void RePositionFields( TInt aAmount ) = 0;
    
    /**
     * Return viewable areas rect, which in other words is
     * listpanes rect.
     *
     * @return TRect, rect of the viewable area.
     */
    virtual TRect ViewableAreaRect() = 0;
    };

#endif // MESMRLISTOBSERVER_H
