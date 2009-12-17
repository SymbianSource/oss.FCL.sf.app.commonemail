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
* Description:  Definition of class CFscContactActionMenuItemImpl.
*
*/


#ifndef C_FSCCONTACTACTIONMENUITEMIMPL_H
#define C_FSCCONTACTACTIONMENUITEMIMPL_H

#include <e32base.h>
//<cmail>
#include "cfsccontactactionmenuitem.h"
//</cmail>

// FORWARD DECLARATIONS
class CGulIcon;

/**
 *  Contact Action Menu Item implementation.
 *
 *  @since S60 3.1
 */
class CFscContactActionMenuItemImpl : public CFscContactActionMenuItem
    {

public: // Public constructor and destructor

    /**
     * Two-phased constructor.
     *
     * @return New instance of the component  
     */
    static CFscContactActionMenuItemImpl* NewL();

    /**
     * Two-phased constructor.
     *
     * @return New instance of the component  
     */
    static CFscContactActionMenuItemImpl* NewLC();
    
    /**
     * Two-phased constructor.
     *
     * @param aMenuText Menu text
     * @param aIcon Icon
     * @param aPriority Priority
     * @param aStatic Is static
     * @param aUid Item Uid
     * @return New instance of the component  
     */
    static CFscContactActionMenuItemImpl* NewL( 
        const TDesC& aMenuText,
        const CGulIcon* aIcon,
        TInt aPriority,
        TBool aStatic,
        TUid aUid = KFscDefaultItemUid );
            
    /**
     * Two-phased constructor.
     *
     * @param aMenuText Menu text
     * @param aIcon Icon
     * @param aPriority Priority
     * @param aStatic Is static
     * @param aUid Item Uid
     * @return New instance of the component  
     */
    static CFscContactActionMenuItemImpl* NewLC( 
        const TDesC& aMenuText,
        const CGulIcon* aIcon,
        TInt aPriority,
        TBool aStatic,
        TUid aUid = KFscDefaultItemUid );

    /**
    * Destructor.
    */
    virtual ~CFscContactActionMenuItemImpl();
    
public: // Public methods from base class

    /**
     * Set menu text
     *
     * @param aMenuText New menu text
     */
    void SetMenuTextL( const TDesC& aMenuText );

    /**
     * Get menu text
     *
     * @return Menu text
     */
    const TDesC& MenuText() const;

    /**
     * Set icon
     *
     * @param aIcon New icon
     */
    void SetIcon( const CGulIcon* aIcon );

    /**
     * Get icon
     *
     * @return Item's icon
     */
    const CGulIcon* Icon() const;

    /**
     * Set priority
     *
     * @param aPriority Item priority
     */
    void SetPriority( TInt aPriority );

    /**
     * Get priority
     *
     * @return Item's priority
     */
    TInt Priority() const;

    /**
     * Set implementation uid
     *
     * @param aUid Implementation Uid of the item
     */
    void SetImplementationUid( TUid aUid );

    /**
     * Get implementation Uid
     *
     * @return Item's implementation Uid
     */
    TUid ImplementationUid() const;

    /**
     * Set item dimmed
     *
     * @param aDimmed ETrue if item is dimmed
     */
    void SetDimmed( TBool aDimmed );

    /**
     * Is item dimmed
     *
     * @return ETrue if item is dimmed, otherwise EFalse
     */
    TBool Dimmed() const;

    /**
     * Set dimm reason
     *
     * @param aDimmReason dimm reason
     */
    void SetDimmReasonL( const TDesC& aDimmReason );

    /**
     * Get dimm reason
     *
     * @return dimm reason
     */
    const TDesC& DimmReason() const;

    /**
     * Set item hidden
     *
     * @param aHidden ETrue if item is hidden
     */
    void SetHidden( TBool aHidden );

    /**
     * Is item hidden
     *
     * @return ETrue if item is hidden, otherwise EFalse
     */
    TBool Hidden() const;

    /**
     * Set item static
     *  Static items are ment to be kept in the menu longer than non-static. 
     *  Normal use case for static items are special items set by Action Menu's 
     *  client. These special items are always in the menu so client does not 
     *  want to set them again after clearing other items from the menu. All CAS 
     *  based items are non-static and therefore they can easily be removed from 
     *  the menu without removing static items also. 
     *
     * @param aStatic ETrue if item is hidden
     */
    void SetStatic( TBool aStatic );

    /**
     * Is item static
     *
     * @return ETrue if item is static, otherwise EFalse
     */
    TBool Static() const;

protected:

    /**
     * Constructor.
     *
     * @param aIcon Icon
     * @param aPriority Priority
     * @param aStatic Is static
     * @param aUid Item Uid
     */
    CFscContactActionMenuItemImpl(
        const CGulIcon* aIcon, 
        TInt aPriority, 
        TBool aStatic, 
        TUid aUid );

    /**
     * Second phase constructor.
     *
     * @param aMenuText Menu text
     */
    void ConstructL( const TDesC& aMenuText );
                 
protected: // data

    /**
     * Menu item text
     *  Owned
     */
    HBufC*      iMenuText;
    
    /**
     * Menu item icon
     *  Not owned
     */
    const CGulIcon* iIcon;
    
    /**
     * Priority of the menu item
     */
    TInt        iPriority;
    
    /**
     * Action implementation uid
     */
    TUid        iImplementationUid;
    
    /**
     * Dimmed flag
     */
    TBool       iDimmed;
    
    /**
     * Dimming reason.
     *  Owned
     */
    HBufC*      iDimmReason;
    
    /**
     * Hidden flag
     */
    TBool       iHidden; 
    
    /**
     * Static flag
     */
    TBool       iStatic;

    };

#endif // C_FSCCONTACTACTIONMENUITEMIMPL_H

