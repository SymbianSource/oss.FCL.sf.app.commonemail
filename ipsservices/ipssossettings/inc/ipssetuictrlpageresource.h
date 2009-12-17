/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Decleares class to control page resources.
*
*/


#ifndef IPSSETUICTRLPAGERESOURCE_H
#define IPSSETUICTRLPAGERESOURCE_H


/**
 * Page resources in array
 */
enum TIpsSetUiPageResource
    {
    EIpsSetUiRadioButton = 0,
    EIpsSetUiCheckbox,
    EIpsSetUiText,
    EIpsSetUiNumber,
    EIpsSetUiScNumber,
    EIpsSetUiSecret,
    EIpsSetUiMultiLine,
    EIpsSetUiLastPage
    };

typedef CArrayFixFlat<TInt> CResourceStack;
typedef CArrayPtrFlat<CResourceStack> CResourceStackArray;

/**
 * Class to control page resource array.
 *
 * @since FS v1.0
 * @lib IpsSosSettings.lib
 */
NONSHARABLE_CLASS( CIpsSetUiCtrlPageResource ): public CBase
    {
public:  // Constructors and destructor

    /**
     * Two-phase constructor with stack push.
     *
     * @return Constructed object
     */
    static CIpsSetUiCtrlPageResource* NewLC();

    /**
     * Create object from CIpsSetUiCtrlPageResource
     *
     * @return Constructed object
     */
    static CIpsSetUiCtrlPageResource* NewL();

    /**
     * Destructor
     */
    virtual ~CIpsSetUiCtrlPageResource();

// New functions

    /**
     * Pushes resource to stack.
     *
     * @param aType Type of the resource.
     * @param aPageResource ID of the resource.
     */
    void PushResourceL(
        const TInt aType,
        const TInt aPageResource );

    /**
     * Retrieves the resource ID from the stack.
     *
     * @param aType Type of the resource to be retrieved.
     * @param aRemove Resource ID should be removed or not.
     */
    TInt Resource(
        const TInt aType,
        const TBool aRemove = ETrue );

    /**
     * @param aType Type of resource to be counted
     * @return Number of items for resource.
     */
    TInt Count( const TInt aType );

    /**
     * Creates resource stack.
     *
     * @param aPageResource Stack type for creation.
     * @return Stack object, which client owns.
     */
    CResourceStack* CreateStackL( const TInt aPageResource );

    /**
     * Accessor for page resource.
     *
     * @param aType Resource type.
     * @return Id of the resource.
     */
    TInt operator[]( const TInt aType );

protected:  // Constructors

    /**
     * Default constructor for class CIpsSetUiCtrlPageResource
     *
     * @return, Constructed object
     */
    CIpsSetUiCtrlPageResource();

    /**
     * Symbian 2-phase constructor
     */
    void ConstructL();

private:  // New functions

    // TOOLS

    /**
     * @param aType Type of resource.
     * @return Resource stack of the type.
     */
    CResourceStack& Stack( const TInt aType );

private:    // Data

    /**
     * Page resource array.
     * Owned.
     */
    CResourceStackArray*    iPageResources;
    };

#endif /* IPSSETUICTRLPAGERESOURCE_H */

// End of File


