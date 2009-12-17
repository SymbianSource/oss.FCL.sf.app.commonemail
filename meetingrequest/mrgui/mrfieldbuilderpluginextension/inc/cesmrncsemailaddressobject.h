/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies). 
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

#ifndef CESMRNCSEMAILADDRESSOBJECT_H
#define CESMRNCSEMAILADDRESSOBJECT_H

#include <e32base.h>
#include <e32def.h>

/**
*  The CNcsEmailAddressObject class. A single instance of this wrapper 
*  class stores the email address and the optional name to display 
*  inplace of the address
*/
NONSHARABLE_CLASS( CESMRNcsEmailAddressObject ) : public CBase
    {
public: // Constructors and destructor
    /**
     * Creates new CESMRNcsEmailAddressObject object. Ownership
     * is transferred to caller.
     * @param aUserAdded set that user is already added
     * @return Pointer to created object,
     */
    static CESMRNcsEmailAddressObject* NewL( TBool aUserAdded );
    
    /**
     * Creates new CESMRNcsEmailAddressObject object. Ownership
     * is transferred to caller.
     * @param aAddress set email address manually
     * @return Pointer to created object,
     */
    static CESMRNcsEmailAddressObject* NewL( const CESMRNcsEmailAddressObject& aAddress );
    
    /**
     * Creates new CESMRNcsEmailAddressObject object. Ownership
     * is transferred to caller.
     * @param aDisplayName set name of the address manually
     * @param aAddress set email address manually
     * @return Pointer to created object,
     */
    static CESMRNcsEmailAddressObject* NewL( const TDesC& aDisplayName, const TDesC& aEmailAddress );
    
    /**
     * C++ Destructor.
     */
    ~CESMRNcsEmailAddressObject();

public: // From MNCSEmailAddressObject
    
    // inlines

    /**
    * Test if object is filled/added comletely already
    * @return ETrue if user was already added
    */
    inline TBool IsUserAdded();
    
    /**
    * Get the email address of this contact address object
    * @return aEmailAddress descriptor of the email address of this contact address object
    */
    inline const TDesC& EmailAddress() const;
    
    /**
    * Get the display name of this contact address object
    * @return descriptor of the display name of this contact address object
    */
    inline const TDesC& DisplayName() const;
    
    /**
    * Display full means that both name and email address must be shown.
    * This is the case for example when contact has multiple email
    * addresses.
    */
    inline void SetDisplayFull( TBool aDisplayFull );
    
    /**
    * Check if the object needs to display both name and email address
    * @return ETrue if both name and email address need to be displayed
    */
    inline TBool DisplayFull() const;
    
    /**
    * Set the email address of this contact address object
    * @param aEmailAddress descriptor of the email address for this contact address object
    */
    void SetEmailAddressL( const TDesC& aEmailAddress );
    
    /**
    * Set the display names of this contact address object
    * @param aGivenName descriptor of the display name for this contact address object
    * @param aFamilyName descriptor of the display name for this contact address object
    */
    void SetDisplayNameL ( const TDesC& aGivenName, const TDesC& aFamilyName);
    
    /**
    * Set the display name of this contact address object
    * @param aName descriptor of the display name for this contact address object
    */
    void SetDisplayNameL( const TDesC& aName );
    
    /**
    * Get the formatted address of this contact address object
    * @return descriptor of this contact address object
    */
    const TDesC& FormattedAddressL();
    
    /**
    * Test of email address is idna compliant
    * @return ETrue if idna comliant
    */
    TBool IsIdnaCompliantAddress();
    
private: // Constructors
    CESMRNcsEmailAddressObject( TBool aUserAdded );
    void ConstructL();
    void ConstructL( const CESMRNcsEmailAddressObject& aAddress );
    void ConstructL( const TDesC& aDisplayName, const TDesC& aEmailAddress );
    
private: // Implementation
    void GenerateFormattedAddressL();

private: // Data
    HBufC*               iEmailAddress;//Own:
    HBufC*               iDisplayName;//Own:
    HBufC*               iFormattedAddress;//Own:
    TBool                iDirtyFlag;
    TBool                iIsIdnaCompliant;
    TBool                iUserAdded;
    TBool                iDisplayFull;
    };

#include "cesmrncsemailaddressobject.inl"

#endif      //  CESMRNCSEMAILADDRESSOBJECT_H

// End of file
