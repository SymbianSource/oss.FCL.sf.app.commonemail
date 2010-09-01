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
* Description:  CLS list item class definition
*
*/


#ifndef FREESTYLEEMAILUICLSITEM_H
#define FREESTYLEEMAILUICLSITEM_H

//  INCLUDES
#include <e32base.h>
#include <cntdef.h>
#include "FSEmailBuildFlags.h"

#include <CPcsDefs.h> // TPsMatchLocation

// FORWARD DECLARATIONS

class MVPbkContactLink;

// CLASS DECLARATION
/**
 * Freestyle email application cls list item
 *
 *
 */
 
class CFSEmailUiClsItem : public CBase
	{

 public:
  	  
    /**
     * Two-phased constructor.
     * Creates a new instance of class
     *
     * @returns CFSEmailUiClsItem instance
     */
  	 static CFSEmailUiClsItem* NewL();
  	
    /**
     * Two-phased constructor.
     * Creates a new instance of class
     *
     * @returns CFSEmailUiClsItem instance
     */
  	 static CFSEmailUiClsItem* NewLC();

    /**
     * Destructor.
     */  
  	 virtual ~CFSEmailUiClsItem();

     /**
      * Creates a copy of the CLS item and leaves it in cleanup stack.
      */
     CFSEmailUiClsItem* CloneLC() const;

    /**
     * Finds actual match place from the cls item and sets the highlight
     * information to cls item.
     *
	 * @param aHighlights Match list for the item text
     *
     */	
	void SetHighlights( const RArray<TPsMatchLocation>& aHighlights );
	void SetDisplayNameL( const TDesC& aDisplayName );
	void SetEmailAddressL( const TDesC& aEmailAddress );
	void SetIsMruItem( TBool aIsMruItem );
    void SetContactLinkL( const MVPbkContactLink& aLink );
    void SetMultipleEmails( TBool ETrue );

	// getters
 
	const RArray<TPsMatchLocation>& Highlights() const;

	const TDesC& DisplayName() const;
	const TDesC& EmailAddress() const;
	const TDesC& FullTextL();
    TBool IsMruItem() const;
    TBool MultipleEmails() const;

	MVPbkContactLink* ContactLink() const;
	
protected:

    /**
     * C++ default constructor.
     *
     */
  	 CFSEmailUiClsItem();

private:

    /**
     * ConstructL
     *
     */
	void ConstructL();
	

private: // data

	// Highlight positions
	RArray<TPsMatchLocation> iHighlights;

	// Display name
	HBufC*	iDisplayName;
	// Email address 
	HBufC*	iEMailAddress;
	
	HBufC*	iFullText;

	TBool 	iIsMruItem;
	
	TBool iMultipleEmails;
	
    // Contact item id in which this item refers to. MRU list items
    // do not have this set.
	MVPbkContactLink* iLink;
    };

#endif  // FREESTYLEEMAILUICLSITEM_H

// End of File
