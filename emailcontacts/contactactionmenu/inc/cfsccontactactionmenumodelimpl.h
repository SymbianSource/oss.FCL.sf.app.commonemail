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
* Description:  Definition of class CFscContactActionMenuModelImpl.
*
*/


#ifndef C_FSCCONTACTACTIONMENUMODELIMPL_H
#define C_FSCCONTACTACTIONMENUMODELIMPL_H

#include <e32base.h>
//<cmail>
#include "mfsccontactactionmenumodel.h"
#include "cfsccontactactionmenuitem.h"
//</cmail>

// FORWARD DECLARATIONS
class CFscContactActionService;

/**
 *  Contact Action Menu Model implementation.
 *
 *  @since S60 3.1
 */
class CFscContactActionMenuModelImpl : 
    public CBase,
    public MFscContactActionMenuModel
    {

public: // Public constructor and destructor

    /**
     * Two-phased constructor.
     *
     * @param aService Pointer to Contact Action Service
     * @return New instance of the component  
     */
    static CFscContactActionMenuModelImpl* NewL( 
            CFscContactActionService& aService );

    /**
    * Destructor.
    */
    virtual ~CFscContactActionMenuModelImpl();
    
public: // Public methods from base class

    /**
     * Factory method for creating a new menu item. 
     *   Item is not added to the menu.
     *   Ownership of new item is transferred and it is not added to 
     *   the menu automatically
     *
     * @param aMenuText Menu text
     * @param aIcon Icon. Icon ownership is not transferred
     * @param aPriority Priority of menu item
     * @param aStatic Is static
     *   Static items are ment to be kept in the menu longer than non-static. 
     *   Normal use case for static items are custom items set by Action Menu's 
     *   client. These special items are always in the menu so client does not 
     *   want to set them again after clearing other items from the menu. All CAS 
     *   based items are non-static and therefore they can easily be removed from 
     *   the menu without removing static items also. 
     * @param aUid Item Uid
     * @return New menu item
     */
    CFscContactActionMenuItem* NewMenuItemL( 
        const TDesC& aMenuText,
        const CGulIcon* aIcon,
        TInt aPriority,
        TBool aStatic,
        TUid aUid );
 
    /**
     * Add a single item
     *
     * @param aItem New item. Ownership of the item is transferred.
     * @return Index of added item
     */
    TInt AddItemL( CFscContactActionMenuItem* aItem );   
    
    /**
     * Add pre queried items to the list
     *   Previously set CAS items are removed before adding new items.
     */
    void AddPreQueriedCasItemsL();
 
    /**
     * Remove a single item from the menu
     *
     * @param aIndex Index of the item
     */
    void RemoveItemL( TInt aIndex );  
 
    /**
     * Remove all items from the menu
     *
     * @param aStaticAlso If ETrue also static items will be removed
     */
    void RemoveAll( TBool aStaticAlso = EFalse );  
 
    /**
     * Item getter
     *
     * @param aIndex Index of requested item
     * @return A pointer to requested menu item or NULL if item is not found.
     */
    CFscContactActionMenuItem& ItemL( TInt aIndex );  
 
    /**
     * Return number of items in the menu
     *
     * @return number of items in the menu
     */
    TInt ItemCount();
 
    /**
     * Visible item getter. 
     *   Visible items are not hidden or dimmed and they have an icon
     *
     * @param aIndex Index of requested item
     * @return A pointer to requested menu item or NULL if item is not found.
     */
    CFscContactActionMenuItem& VisibleItemL( TInt aIndex );  
 
    /**
     * Return number of visible items in the menu
     *   Visible items are not hidden or dimmed and they have an icon
     *
     * @return number of items in the menu
     */
    TInt VisibleItemCount();
    
public: // public own methods

    /**
     * Get Contact Action Service pointer
     *
     * @return pointer to Contact Action Service
     */
    CFscContactActionService* Service();
    
private: // private contrtuctors

    /**
     * Constructor.
     *
     * @param aService Pointer to Contact Action Service
     */
    CFscContactActionMenuModelImpl( CFscContactActionService& aService );

    /**
     * Second phase constructor.
     */
    void ConstructL();
    
private: // private methods

    /**
     * Method sorts menu items into priority order
     */
    void SortMenuItems();
    
    /**
     * Method to check item's visibility
     *
     * @param aItem Item to be checked
     * @return ETrue if item is visible
     */
    TBool IsVisible( const CFscContactActionMenuItem& aItem );
    
    /**
     * Sorting methods used by sorting algorithm
     *
     * @param aItem1
     * @param aItem2
     * @return comparison result
     */
    static TInt PriorityCompare( 
        const CFscContactActionMenuItem& aItem1,
        const CFscContactActionMenuItem& aItem2 );
                                   
private: // data

    /**
     * Instance of Contact Action Service
     */
    CFscContactActionService& iCas;
    
    /**
     * Menu item array
     */
    RPointerArray<CFscContactActionMenuItem> iMenuItems;
    
    };

#endif // C_FSCCONTACTACTIONMENUMODELIMPL_H

