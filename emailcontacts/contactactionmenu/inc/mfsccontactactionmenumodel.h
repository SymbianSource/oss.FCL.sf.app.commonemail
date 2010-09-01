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
* Description:  Definition of class MFscContactActionMenuModel.
*
*/


#ifndef M_FSCCONTACTACTIONMENUMODEL_H
#define M_FSCCONTACTACTIONMENUMODEL_H

#include <e32base.h>

//<cmail>
#include "fsccontactactionmenudefines.h"
#include "fsccontactactionservicedefines.h"
//</cmail>

// FORWARD DECLARATIONS
class CFscContactActionMenuItem;
class CGulIcon;

/**
 *  Contact Action Menu Model interface.
 *
 *  @since S60 3.1
 */
class MFscContactActionMenuModel
    {
    
public: // Public methods

    /**
     * Factory method for creating a new menu item. 
     *   Item is not added to the menu.
     *   Ownership of new item is transferred and it is not added to 
     *   the menu automatically
     *
     * @param aMenuText Menu text
     * @param aIcon Icon. Icon ownership is not transferred
     * @param aPriority Priority
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
    virtual CFscContactActionMenuItem* NewMenuItemL( 
        const TDesC& aMenuText,
        const CGulIcon* aIcon,
        TInt aPriority,
        TBool aStatic,
        TUid aUid ) = 0;

    /**
     * Add a single item
     *
     * @param aItem New item. Ownership of the item is transferred.
     * @return Index of added item
     */
    virtual TInt AddItemL( CFscContactActionMenuItem* aItem ) = 0;   
    
    /**
     * Add pre queried items to the list
     *   This method can be used when client application is using same 
     *   instance of cas and it is know that cas's queryresults are ok 
     *   for the menu. This is usefull when Cas items are used in options 
     *   menu and action menu
     *   Previously set CAS items are removed before adding new items.
     */
    virtual void AddPreQueriedCasItemsL() = 0;    
 
    /**
     * Remove a single item from the menu
     *
     * @param aIndex Index of the item
     */
    virtual void RemoveItemL( TInt aIndex ) = 0;  
 
    /**
     * Remove all items from the menu
     *
     * @param aStaticAlso If ETrue also static items will be removed
     */
    virtual void RemoveAll( TBool aStaticAlso = EFalse ) = 0;  
 
    /**
     * Item getter
     *   Method leaves with KErrNotFound if given index is invalid
     *
     * @param aIndex Index of requested item
     * @return A reference to requested menu item.
     */
    virtual CFscContactActionMenuItem& ItemL( TInt aIndex ) = 0;  
 
    /**
     * Return number of items in the menu
     *
     * @return number of items in the menu
     */
    virtual TInt ItemCount() = 0; 
 
    /**
     * Visible item getter. 
     *   Visible items are not hidden or dimmed and they have an icon
     *
     * @param aIndex Index of requested item
     * @return A pointer to requested menu item or NULL if item is not found.
     */
    virtual CFscContactActionMenuItem& VisibleItemL( TInt aIndex ) = 0;  
 
    /**
     * Return number of visible items in the menu
     *   Visible items are not hidden or dimmed and they have an icon
     *
     * @return number of items in the menu
     */
    virtual TInt VisibleItemCount() = 0;     

protected: // destructor
    
    /**
    * Destructor.
    */
    virtual ~MFscContactActionMenuModel() {};

    };

#endif // M_FSCCONTACTACTIONMENUMODEL_H
