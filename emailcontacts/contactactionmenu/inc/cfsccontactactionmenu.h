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
* Description:  Declaration of class CFscContactActionMenu.
*
*/


#ifndef C_FSCCONTACTACTIONMENU_H
#define C_FSCCONTACTACTIONMENU_H

#include <e32base.h>

//<cmail>
#include "fsccontactactionmenudefines.h"
//</cmail>

// FORWARD DECLARATIONS
class CFscContactActionService;
class MFscContactActionMenuModel;
class CFscContactActionMenuItem;

/**
 *  Contact Action Menu API.
 *  Defines an ECom interface for the Contact Action Menu.
 *
 *  @since S60 3.1
 */
class CFscContactActionMenu : public CBase
    {

public: // Public constructor and destructor

    /**
     * Two-phased constructor.
     *
     * @param aService Reference to Contact Action Service.
     * @param aMode Menu mode
     *  In EFscContactActionMenuModeNormal mode Action Menu works 
     *  like a normal modal dialog. In EFscContactActionMenuModeAI mode 
     *  number key events close the menu without consuming the events. 
     *  AI Mode is intended to be used in Active Idle when it must be 
     *  possible to start an emergency call while Action Menu is open
     * @return New isntance of the component  
     */
    static CFscContactActionMenu* NewL( 
        CFscContactActionService& aService,
        TFscContactActionMenuMode aMode = EFscContactActionMenuModeNormal,
        TBool aOpenedFromMR = EFalse
        );

    /**
     * Destructor.
     */
    virtual ~CFscContactActionMenu();
    
public: // Public methods

    /**
     * Execute Action Menu
     *   ExecuteL opens Action Menu with current item set. Method is
     *   synchronous and it will return when Action Menu is closed.
     *
     * @param aPosition Vertical position where menu is to be openned
     * @param aIndex Index of focused/highlighted item when menu is opened
     * @return Execution result
     */
    virtual TFscActionMenuResult ExecuteL( 
        TFscContactActionMenuPosition aPosition = EFscCenter, TInt aIndex = 0, MFsActionMenuPositionGiver* aPositionGiver = 0) = 0;

    /**
     * Dismiss menu
     *   Menu gets closed if it was visible when the method was called. 
     *   Otherwise method call does not do anything
     *
     * @param aSlide If ETrue menu slides away from the screen. 
     *   Otherwise it disappears instantly
     */
    virtual void Dismiss( TBool aSlide ) = 0;  
  
    /**
     * Return a reference to Action Menu's model
     *  
     * @return Reference to menu model
     */
    virtual MFscContactActionMenuModel& Model() = 0;
  
    /**
     * Return index of highlighted item
     * 
     * @return index of highlighted item
     */
    virtual TInt FocusedItemIndex() = 0;
    
    /**
     * Return selected menu item object.
     * 
     * @return selected menu item object. 
     */
    virtual CFscContactActionMenuItem& FocusedItem() = 0;

    /**
     * Set whether or not the menu is opened from meeting request.
     * 
     * @param aOpenedFromMR ETrue, if the menu is opened from Meeting request.
     */
    virtual void SetOpenedFromMR( TBool aOpenedFromMR ) = 0;
    
private: // data

    /** iDtor_ID_Key Instance identifier key. When instance of an
     *               implementation is created by ECOM framework, the
     *               framework will assign UID for it. The UID is used in
     *               destructor to notify framework that this instance is
     *               being destroyed and resources can be released.
     */
    TUid iDtor_ID_Key;

    };

#include "cfsccontactactionmenu.inl"

#endif // C_FSCCONTACTACTIONMENU_H
