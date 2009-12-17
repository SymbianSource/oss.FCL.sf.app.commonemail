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
* Description:  Declaration of class CFscContactActionMenuItem.
*
*/


#ifndef C_FSCCONTACTACTIONMENUITEM_H
#define C_FSCCONTACTACTIONMENUITEM_H

#include <e32base.h>

//<cmail>
#include "fsccontactactionmenudefines.h"
//</cmail>

// FORWARD DECLARATIONS
class CGulIcon;

/**
 *  Contact Action Menu Item interface.
 *    Contact Action Menu Items can be created using a factory method 
 *    provided by Action Menu's model
 *
 *  @since S60 3.1
 */
class CFscContactActionMenuItem : public CBase
    {

public: // Public destructor

    /**
    * Destructor.
    */
    virtual ~CFscContactActionMenuItem() {};
    
public: // Public methods

    /**
     * Set menu text
     *
     * @param aMenuText New menu text
     */
    virtual void SetMenuTextL( const TDesC& aMenuText ) = 0;

    /**
     * Get menu text
     *
     * @return Menu text
     */
    virtual const TDesC& MenuText() const = 0;

    /**
     * Set icon
     *  Icon ownership is not transferred
     *
     * @param aIcon New icon
     */
    virtual void SetIcon( const CGulIcon* aIcon ) = 0;

    /**
     * Get icon
     *  Icon ownership is not transferred
     *
     * @return Item's icon
     */
    virtual const CGulIcon* Icon() const = 0;

    /**
     * Set priority
     *
     * @param aPriority Item priority
     */
    virtual void SetPriority( TInt aPriority ) = 0;

    /**
     * Get priority
     *
     * @return Item's priority
     */
    virtual TInt Priority() const = 0;

    /**
     * Set implementation uid
     *
     * @param aUid Implementation Uid of the item
     */
    virtual void SetImplementationUid( TUid aUid ) = 0;

    /**
     * Get implementation Uid
     *
     * @return Item's implementation Uid
     */
    virtual TUid ImplementationUid() const = 0;

    /**
     * Set item dimmed
     *
     * @param aDimmed ETrue if item is dimmed
     */
    virtual void SetDimmed( TBool aDimmed ) = 0;

    /**
     * Is item dimmed
     *
     * @return ETrue if item is dimmed, otherwise EFalse
     */
    virtual TBool Dimmed() const = 0;

    /**
     * Set dimm reason
     *
     * @param aDimmReason dimm reason
     */
    virtual void SetDimmReasonL( const TDesC& aDimmReason ) = 0;

    /**
     * Get dimm reason
     *
     * @return dimm reason
     */
    virtual const TDesC& DimmReason() const = 0;

    /**
     * Set item hidden
     *
     * @param aHidden ETrue if item is hidden
     */
    virtual void SetHidden( TBool aHidden ) = 0;

    /**
     * Is item hidden
     *
     * @return ETrue if item is hidden, otherwise EFalse
     */
    virtual TBool Hidden() const = 0;

    /**
     * Set item static
     *  Static items are ment to be kept in the menu longer than non-static. 
     *  Normal use case for static items are custom items set by Action Menu's 
     *  client. These special items are always in the menu so client does not 
     *  want to set them again after clearing other items from the menu. All CAS 
     *  based items are non-static and therefore they can easily be removed from 
     *  the menu without removing static items also. 
     *
     * @param aStatic ETrue if item is hidden
     */
    virtual void SetStatic( TBool aStatic ) = 0;

    /**
     * Is item static
     *
     * @return ETrue if item is static, otherwise EFalse
     */
    virtual TBool Static() const = 0;

    };

#endif // C_FSCCONTACTACTIONMENUITEM_H
