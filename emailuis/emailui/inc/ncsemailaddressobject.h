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
* Description:  Implementation of the MNcsEmailAddressObject
*
*/


#ifndef C_NCSEMAILADDRESSOBJECT_H
#define C_NCSEMAILADDRESSOBJECT_H


#include <e32base.h>
#include <e32def.h>

/**
*  The CNcsEmailAddressObject class
*/
NONSHARABLE_CLASS( CNcsEmailAddressObject ) : public CBase
	{
    public: // Constructors and destructor
    
    static CNcsEmailAddressObject* NewL( TBool aUserAdded );
    static CNcsEmailAddressObject* NewL( const CNcsEmailAddressObject& aAddress );
    static CNcsEmailAddressObject* NewL( 
        const TDesC& aDisplayName, 
        const TDesC& aEmailAddress );
    
    ~CNcsEmailAddressObject();
    
    private: // Constructors
    
    CNcsEmailAddressObject( TBool aUserAdded );
    CNcsEmailAddressObject( const CNcsEmailAddressObject& aAddress );
    void ConstructL();
    void ConstructL( const CNcsEmailAddressObject& aAddress );
    void ConstructL(
        const TDesC& aDisplayName, 
        const TDesC& aEmailAddress );

public: // new functions

    // inlines
    inline TBool IsUserAdded() const;
    inline const TDesC& EmailAddress() const;
    inline const TDesC& DisplayName() const;
    
    /**
    * Display full means that both name and email address must be shown.
    *
    * This is the case for example when contact has multiple email 
    * addresses.
    */
    inline void SetDisplayFull( TBool aDisplayFull );
    inline TBool DisplayFull() const;
		
    void SetEmailAddressL( const TDesC& aEmailAddress );
    void SetDisplayNameL ( const TDesC& aGivenName, const TDesC& aFamilyName);
    void SetDisplayNameL( const TDesC& aName );
    const TDesC& FormattedAddressL();

	private: // new functions
	
    void GenerateFormattedAddressL();

    private: // Data
    
    //Own:
    HBufC*               iEmailAddress;
    HBufC*               iDisplayName;
    HBufC*               iFormattedAddress;
    TBool                iDirtyFlag;
    TBool                iUserAdded;
    TBool                iDisplayFull;
	};

#include "ncsemailaddressobject.inl"

#endif      //  C_NCSEMAILADDRESSOBJECT_H

// End of file
