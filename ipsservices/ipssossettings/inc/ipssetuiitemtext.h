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
* Description:  Decleares class with textual user data.
*
*/


#ifndef IPSSETUIITEMTEXT_H
#define IPSSETUIITEMTEXT_H

#include <e32base.h>

#include "ipssetuiitembase.h"

/**
 *  Container class for user data and branched items.
 *
 *  @lib IpsSosSettings.lib
 *  @since FS v1.0
 */
class CIpsSetUiItemsEditText : public CIpsSetUiItem
    {
public:

// Constructors and Destructor

    /**
     * Destructor
     */
    virtual ~CIpsSetUiItemsEditText();

    /**
     * @return New text item.
     */
    static CIpsSetUiItemsEditText* NewL();

    /**
     * @return New text item.
     */
    static CIpsSetUiItemsEditText* NewLC();

// Operators

    /**
     * Copy operator
     *
     * @param aBaseItem Item, which values are copied.
     */
    CIpsSetUiItemsEditText& operator=(
        const CIpsSetUiItemsEditText& aBaseItem );

// New virtual functions

    /**
     * @return Read-only reference to user data.
     */
    virtual inline const TDesC& Text() const;

    /**
     * @param aText Sets user data.
     */
    virtual inline void SetText( const TDesC& aText );

protected:

// Constructors

    /**
     * Constructor
     */
    CIpsSetUiItemsEditText();

    /**
     * 2nd-phase constructor
     */
    void ConstructL();

// Data

    /**
     * Container for user text
     */
    RBuf                iUserText;

public:

// Data

    /**
     * Maximum value for the item.
     */
    TInt                iItemMaxLength;
    };

#include "ipssetuiitemtext.inl"

#endif /* IPSSETUIITEMTEXT_H */

// End of file
