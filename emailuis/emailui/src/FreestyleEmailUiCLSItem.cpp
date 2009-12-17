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
* Description:  CLS list item class implementation
*
*/



#include "emailtrace.h"
#include <MVPbkContactLink.h>

#include "FreestyleEmailUiCLSItem.h"		// CFSEmailUiClsItem
#include "ncsconstants.h"

// ================= MEMBER FUNCTIONS ==========================================
// -----------------------------------------------------------------------------
// CFSEmailUiClsItem::NewL
// -----------------------------------------------------------------------------
CFSEmailUiClsItem* CFSEmailUiClsItem::NewL()
	{
    FUNC_LOG;
	CFSEmailUiClsItem* object =  CFSEmailUiClsItem::NewLC();
	CleanupStack::Pop( object );
	return object;
	}

// -----------------------------------------------------------------------------
// CFSEmailUiClsItem::NewLC
// -----------------------------------------------------------------------------
CFSEmailUiClsItem* CFSEmailUiClsItem::NewLC()
	{
    FUNC_LOG;
	CFSEmailUiClsItem* object = new (ELeave) CFSEmailUiClsItem();
	CleanupStack::PushL( object );
	object->ConstructL();
	return object;
	} 

// -----------------------------------------------------------------------------
// CFSEmailUiClsItem::~CFSEmailUiClsItem
// -----------------------------------------------------------------------------
CFSEmailUiClsItem::~CFSEmailUiClsItem()
	{
    FUNC_LOG;
	delete iDisplayName;
	delete iEMailAddress;
	delete iFullText;
	delete iLink;

	iHighlights.Close();
	}

// -----------------------------------------------------------------------------
// Creates a copy of the CLS item.
// -----------------------------------------------------------------------------
CFSEmailUiClsItem* CFSEmailUiClsItem::CloneLC() const
    {
    FUNC_LOG;
    CFSEmailUiClsItem* clone = CFSEmailUiClsItem::NewLC();
    clone->SetDisplayNameL( DisplayName() );
    clone->SetEmailAddressL( EmailAddress() );
    if ( ContactLink() )
        {
    	clone->SetContactLinkL( *ContactLink() );
        }
    clone->SetHighlights( Highlights() );
    clone->SetIsMruItem( IsMruItem() );
    clone->SetMultipleEmails( MultipleEmails() );
    return clone;
    }

// -----------------------------------------------------------------------------
// CFSEmailUiClsItem::SetDisplayNameL
// -----------------------------------------------------------------------------
void CFSEmailUiClsItem::SetDisplayNameL( const TDesC& aDisplayName )
	{
    FUNC_LOG;
	delete iDisplayName;
	iDisplayName = NULL;
	iDisplayName = aDisplayName.AllocL();
	}

// -----------------------------------------------------------------------------
// CFSEmailUiClsItem::SetEmailAddressL
// -----------------------------------------------------------------------------
void CFSEmailUiClsItem::SetEmailAddressL( const TDesC& aEmailAddress )
	{
    FUNC_LOG;
	delete iEMailAddress;
	iEMailAddress = NULL;
	iEMailAddress = aEmailAddress.AllocL();
	}

// -----------------------------------------------------------------------------
// CFSEmailUiClsItem::SetContactId
// -----------------------------------------------------------------------------
void CFSEmailUiClsItem::SetContactLinkL( const MVPbkContactLink& aLink )
    {
    FUNC_LOG;
    MVPbkContactLink* copyLink = aLink.CloneLC();
    CleanupStack::Pop();
    iLink = copyLink;
    }

void CFSEmailUiClsItem::SetIsMruItem( TBool aIsMruItem )
    {
    FUNC_LOG;
    iIsMruItem = aIsMruItem;
    }

void CFSEmailUiClsItem::SetMultipleEmails( TBool ETrue )
	{
    FUNC_LOG;
	iMultipleEmails = ETrue;
	}

// -----------------------------------------------------------------------------
// CFSEmailUiClsItem::Highlights
// -----------------------------------------------------------------------------
const RArray<TPsMatchLocation>& CFSEmailUiClsItem::Highlights() const
	{
	return iHighlights;
	}

// -----------------------------------------------------------------------------
// CFSEmailUiClsItem::DisplayName
// -----------------------------------------------------------------------------
const TDesC& CFSEmailUiClsItem::DisplayName() const
	{
    FUNC_LOG;
	return *iDisplayName;
	}

// -----------------------------------------------------------------------------
// CFSEmailUiClsItem::EmailAddress
// -----------------------------------------------------------------------------
const TDesC& CFSEmailUiClsItem::EmailAddress() const
	{
    FUNC_LOG;
	return *iEMailAddress;
	}

// -----------------------------------------------------------------------------
// CFSEmailUiClsItem::FullText
// -----------------------------------------------------------------------------
const TDesC& CFSEmailUiClsItem::FullTextL()
	{
    FUNC_LOG;
	delete iFullText;
	iFullText = NULL;
	iFullText = HBufC::NewL( iDisplayName->Length() + 
							 KSpace().Length() +
							 iEMailAddress->Length() );
   
	// don't add space if there is no display name
	if( iDisplayName->CompareC( KNullDesC ) != 0 )
		{
        iFullText->Des().Append( *iDisplayName );
		iFullText->Des().Append( KSpace );
		}
	iFullText->Des().Append( *iEMailAddress );
	return *iFullText;
	}

// -----------------------------------------------------------------------------
// CFSEmailUiClsItem::ContactItemId
// -----------------------------------------------------------------------------
MVPbkContactLink* CFSEmailUiClsItem::ContactLink() const
    {
    FUNC_LOG;
    return iLink;
    }

TBool CFSEmailUiClsItem::IsMruItem() const
    {
    FUNC_LOG;
    return iIsMruItem;
    }

TBool CFSEmailUiClsItem::MultipleEmails() const
	{
    FUNC_LOG;
	return iMultipleEmails;
	}

// -----------------------------------------------------------------------------
// CFSEmailUiClsItem::CFSEmailUiClsItem
// -----------------------------------------------------------------------------
CFSEmailUiClsItem::CFSEmailUiClsItem()
    : iMultipleEmails( EFalse ), iLink( NULL )
	{
    FUNC_LOG;
	}

// -----------------------------------------------------------------------------
// CFSEmailUiClsItem::ConstructL
// -----------------------------------------------------------------------------
void CFSEmailUiClsItem::ConstructL()
	{
    FUNC_LOG;
	iDisplayName = KNullDesC().AllocL();
	iEMailAddress = KNullDesC().AllocL();
	}

TInt CompareHighlightElements( TPsMatchLocation const& aElement, TPsMatchLocation const& aElement2)
{
	return aElement.index - aElement2.index;
}

void CFSEmailUiClsItem::SetHighlights( const RArray<TPsMatchLocation>& aHighlights )
	{
    FUNC_LOG;
	iHighlights.Reset();
	
	// Convert right->left to left->right, this eases the visualisation
	for ( TInt i=0; i<aHighlights.Count(); i++ )
		{
		iHighlights.Append(aHighlights[i]);
		if(iHighlights[i].direction == TBidiText::ERightToLeft)
			{
			iHighlights[i].index -= aHighlights[i].length - 1;
			iHighlights[i].direction = TBidiText::ELeftToRight;
			}
		}
	
	// Sort match locations according to index, this eases the visualisation
	TLinearOrder<TPsMatchLocation> highlightSortOrder( &CompareHighlightElements );
	iHighlights.Sort(highlightSortOrder);
	}	

