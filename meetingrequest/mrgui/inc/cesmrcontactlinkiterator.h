/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Definition of class CESMRContactLinkIterator.
*
*/


#ifndef CESMRCONTACTLINKITERATOR_H
#define CESMRCONTACTLINKITERATOR_H

#include <e32base.h>
//<cmail>
#include "mfsccontactlinkiterator.h"
//</cmail>

class MVPbkContactLinkArray;

/**
 * Vitual phonebooks's MVPbkContactLink implementation.
 */
NONSHARABLE_CLASS( CESMRContactLinkIterator ) : public CBase, 
												public MFscContactLinkIterator
    {
public:

    /**
     * Two-phase constructor.
     *
     * @return new instance of th class
     */
    static CESMRContactLinkIterator* NewL( );

    /**
     * Destructor.
     */
    ~CESMRContactLinkIterator( );

    /**
     * Sets link array for iterating. Ownership is transferred.
     *
     * @param aContactLinkArray contact link array
     */
    void SetContactLinkArray( MVPbkContactLinkArray* aContactLinkArray );

public: // from MFscContactLinkIteratator
    TInt ContactCount( ) const;
    TBool HasNextContact( ) const;
    MVPbkContactLink* NextContactL( );
    void SetToFirstContact( );
    TInt GroupCount( ) const;
    TBool HasNextGroup( ) const;
    MVPbkContactLink* NextGroupL( );
    void SetToFirstGroup( );

private:
    /**
     * Private constructor.
     */
    CESMRContactLinkIterator( );
    
private: //Data    
    /// Own: Current link index.
    TInt iContactLinkArrayIndex;
    /// Own: Contact link array.
    MVPbkContactLinkArray* iContactLinkArray;
    };

#endif
