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
* Description:  Decleares class with user data and container for setting
*                items.
*
*/


#ifndef IPSSETUIITEMEXTLINK_H
#define IPSSETUIITEMEXTLINK_H


#include "ipssetuiitemlink.h"

/**
 *  Container class for user data and branched items.
 *
 *  @lib IpsSosSettings.lib
 *  @since FS v1.0
 */
class CIpsSetUiItemLinkExt : public CIpsSetUiItemLink
    {
public:

// Constructors and Destructor

    /**
     * Destructor
     */
    virtual ~CIpsSetUiItemLinkExt();

    /**
     * @return New link item.
     */
    static CIpsSetUiItemLinkExt* NewL();

    /**
     * @return New link item.
     */
    static CIpsSetUiItemLinkExt* NewLC();

// Operators

    /**
     * Copy operator
     *
     * @param aBaseItem Item, which values are copied.
     */
    CIpsSetUiItemLinkExt& operator=(
        const CIpsSetUiItemLinkExt& aBaseItem );

// New Functions

    /**
     * @return User data.
     */
    virtual inline TInt64 Value() const;

    /**
     * @param aValue Sets user data.
     */
    virtual inline void SetValue( const TInt64 aValue );

protected:

// Constructors

    /**
     * Constructor
     */
    CIpsSetUiItemLinkExt();

    /**
     * 2nd-phase constructor
     */
    void ConstructL();

// Data

    /**
     * Container for user data.
     */
    TInt64                  iUserValue;
    };

#include "ipssetuiitemextlink.inl"

#endif //IPSSETUIITEMEXTLINK_H

// End of file
