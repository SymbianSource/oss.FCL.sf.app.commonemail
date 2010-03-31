/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CLS list item class definition
 *
*/


#ifndef CESMRCLSITEM_H
#define CESMRCLSITEM_H

//  INCLUDES
#include <e32base.h>
#include <cntdef.h>

#include <cpcsdefs.h> // TPsMatchLocation

// FORWARD DECLARATIONS
class MVPbkContactLink;

// CLASS DECLARATION
/**
 * list item wrapper for attendee field and its popuplistboxes
 */
NONSHARABLE_CLASS( CESMRClsItem ) : public CBase
    {
public:
    /**
     * Two-phased constructor.
     * Creates a new instance of class
     * @returns CESMRClsItem instance
     */
    static CESMRClsItem* NewL();

    /**
     * Two-phased constructor.
     * Creates a new instance of class
     * and leaves it on the cleanupstack
     * @returns CESMRClsItem instance
     */
    static CESMRClsItem* NewLC();

    /**
     * C++ Destructor.
     */
    virtual ~CESMRClsItem();

public: // new methods
    /**
     * Clones a cls item
     * @return pointer to new cls item
     */
    CESMRClsItem* CloneLC() const;

    /**
     * Finds actual match place from the cls item and sets the highlight information to cls item.
     * @param aHighlights Match list for the item text
     */
    void SetHighlightsL( const RArray<TPsMatchLocation>& aHighlights );
    
    /**
     * Sets aDisplayName as new name 
     * @param aDisplayName New name to be set 
     */
    void SetDisplayNameL( const TDesC& aDisplayName );
    
    /**
     * Sets aEmailAddress as new email address 
     * @param aEmailAddress New email to be set 
     */
    void SetEmailAddressL( const TDesC& aEmailAddress );
    
    /**
     * Sets item to be mrui item or not.
     * @param aIsMruItem ETrue maeining items is mrui item  
     */
    void SetIsMruItem( TBool aIsMruItem );
    
    /**
     * Sets aLink as new ContactLink.
     * Copies information from aLink to member variable
     * @param aLink contact link to be set 
     */
    void SetContactLinkL( MVPbkContactLink& aLink );
    
    /**
     * Sets multiple emails flag true
     */
    void SetMultipleEmails( );

    /**
     * Returns  highlight locations 
     */
    const RArray<TPsMatchLocation>& Highlights() const;

    /**
     * Returns  display name 
     */
    const TDesC& DisplayName() const;
    
    /**
     * Returns  email address 
     */
    const TDesC& EmailAddress() const;
    
    /**
     * Returns  display name and email address
     */
    const TDesC& FullTextL();
    
    /**
     * Tells wether this is a mrui item 
     */
    TBool IsMruItem() const;
    
    /**
     * Tells wether this item has multiple emails 
     */
    TBool MultipleEmails() const;

    /**
     * Returns  contact link
     */
    MVPbkContactLink* ContactLink() const;

protected:
    /**
     * C++ default constructor.
     */
    CESMRClsItem();

private:
    /**
     * ConstructL
     */
    void ConstructL();

private: // data
    // Highlight positions
    RArray<TPsMatchLocation> iHighlights;
    // Display name. Own
    HBufC*  iDisplayName;
    // Email address. Own
    HBufC*  iEMailAddress;
    // Name/address text. Own
    HBufC*  iFullText;
    // Contact item id in which this item refers to. MRU list items
    // do not have this set.
    MVPbkContactLink* iLink;
   
    TBool iIsMruItem;
    TBool iMultipleEmails;
    };

#endif  // CESMRCLSITEM_H

// End of File
