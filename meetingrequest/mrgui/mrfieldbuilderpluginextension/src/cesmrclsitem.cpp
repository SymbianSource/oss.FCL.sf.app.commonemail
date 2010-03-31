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
* Description:  CLS list item class implementation
 *
*/


#include "emailtrace.h"
#include "cesmrclsitem.h"// CESMRClsItem

#include "esmrfieldbuilderdef.h"
#include <mvpbkcontactlink.h>

// ======== MEMBER FUNCTIONS ========

// -----------------------------------------------------------------------------
// CESMRClsItem::NewL
// -----------------------------------------------------------------------------
//
CESMRClsItem* CESMRClsItem::NewL( )
    {
    FUNC_LOG;
    CESMRClsItem* object = CESMRClsItem::NewLC ( );
    CleanupStack::Pop ( object );
    return object;
    }

// -----------------------------------------------------------------------------
// CESMRClsItem::NewLC
// -----------------------------------------------------------------------------
//
CESMRClsItem* CESMRClsItem::NewLC( )
    {
    FUNC_LOG;
    CESMRClsItem* object = new (ELeave) CESMRClsItem();
    CleanupStack::PushL ( object );
    object->ConstructL ( );
    return object;
    }

// -----------------------------------------------------------------------------
// CESMRClsItem::~CESMRClsItem
// -----------------------------------------------------------------------------
//
CESMRClsItem::~CESMRClsItem( )
    {
    FUNC_LOG;
    delete iDisplayName;
    delete iEMailAddress;
    delete iFullText;
    delete iLink;
    iHighlights.Close();
    }

// -----------------------------------------------------------------------------
// CESMRClsItem::CloneLC
// -----------------------------------------------------------------------------
//
CESMRClsItem* CESMRClsItem::CloneLC() const
    {
    FUNC_LOG;
    CESMRClsItem* clone = CESMRClsItem::NewLC();
    clone->SetDisplayNameL( DisplayName() );
    clone->SetEmailAddressL( EmailAddress() );
    if ( iLink )
        {
        clone->SetContactLinkL( *iLink );
        }
    clone->SetHighlightsL( Highlights() );
    clone->SetIsMruItem( IsMruItem() );
    if ( MultipleEmails() )
        {
        clone->SetMultipleEmails( );
        }
    return clone;
    }

// -----------------------------------------------------------------------------
// CESMRClsItem::::SetDisplayNameL
// -----------------------------------------------------------------------------
//
void CESMRClsItem::SetDisplayNameL( const TDesC& aDisplayName )
    {
    FUNC_LOG;
    HBufC* temp = aDisplayName.AllocL();
    delete iDisplayName;
    iDisplayName = temp;
    }

// -----------------------------------------------------------------------------
// CESMRClsItem::SetEmailAddressL
// -----------------------------------------------------------------------------
//
void CESMRClsItem::SetEmailAddressL( const TDesC& aEmailAddress )
    {
    FUNC_LOG;
    HBufC* temp = aEmailAddress.AllocL();
    delete iEMailAddress;
    iEMailAddress = temp;
    }

// -----------------------------------------------------------------------------
// CESMRClsItem::SetContactId
// -----------------------------------------------------------------------------
//
void CESMRClsItem::SetContactLinkL( MVPbkContactLink& aLink )
    {
    FUNC_LOG;
    MVPbkContactLink* copyLink = aLink.CloneLC();
    CleanupStack::Pop(); //co
    delete iLink;
    iLink = copyLink;
    }

// -----------------------------------------------------------------------------
// CESMRClsItem::SetIsMruItem
// -----------------------------------------------------------------------------
//
void CESMRClsItem::SetIsMruItem( TBool aIsMruItem )
    {
    FUNC_LOG;
    iIsMruItem = aIsMruItem;
    }
// -----------------------------------------------------------------------------
// CESMRClsItem::SetMultipleEmails
// -----------------------------------------------------------------------------
//
void CESMRClsItem::SetMultipleEmails( )
    {
    FUNC_LOG;
    iMultipleEmails = ETrue;
    }

// -----------------------------------------------------------------------------
// CESMRClsItem::Highlights
// -----------------------------------------------------------------------------
//

const RArray<TPsMatchLocation>& CESMRClsItem::Highlights() const
    {
    return iHighlights;
    }

// -----------------------------------------------------------------------------
// CESMRClsItem::DisplayName
// -----------------------------------------------------------------------------
//
const TDesC& CESMRClsItem::DisplayName() const
    {
    FUNC_LOG;
    return *iDisplayName;
    }
// -----------------------------------------------------------------------------
// CESMRClsItem::EmailAddress
// -----------------------------------------------------------------------------
//
const TDesC& CESMRClsItem::EmailAddress() const
    {
    FUNC_LOG;
    return *iEMailAddress;
    }

// -----------------------------------------------------------------------------
// CESMRClsItem::FullText
// -----------------------------------------------------------------------------
//
const TDesC& CESMRClsItem::FullTextL()
    {
    FUNC_LOG;
    delete iFullText;
    iFullText = NULL;
    iFullText = HBufC::NewL( iDisplayName->Length() +
                             KEmptySpace().Length() +
                             iEMailAddress->Length() );

    // don't add space if there is no display name
    if( iDisplayName->CompareC( KNullDesC ) )
        {
        iFullText->Des().Append( *iDisplayName );
        iFullText->Des().Append( KEmptySpace );
        }
    iFullText->Des().Append( *iEMailAddress );
    return *iFullText;
    }

// -----------------------------------------------------------------------------
// CESMRClsItem::ContactItemId
// -----------------------------------------------------------------------------
//
MVPbkContactLink* CESMRClsItem::ContactLink() const
    {
    FUNC_LOG;
    return iLink;
    }

// -----------------------------------------------------------------------------
// CESMRClsItem::IsMruItem
// -----------------------------------------------------------------------------
//
TBool CESMRClsItem::IsMruItem() const
    {
    FUNC_LOG;
    return iIsMruItem;
    }

// -----------------------------------------------------------------------------
// CESMRClsItem::MultipleEmails
// -----------------------------------------------------------------------------
//
TBool CESMRClsItem::MultipleEmails() const
    {
    FUNC_LOG;
    return iMultipleEmails;
    }
// -----------------------------------------------------------------------------
// CESMRClsItem::CFSEmailUiClsItem
// -----------------------------------------------------------------------------
//
CESMRClsItem::CESMRClsItem() 
    : iMultipleEmails( EFalse ) 
    {
    FUNC_LOG;
    //do nothing
    }

// -----------------------------------------------------------------------------
// CESMRClsItem::ConstructL
// -----------------------------------------------------------------------------
//
void CESMRClsItem::ConstructL()
    {
    FUNC_LOG;
    iDisplayName = KNullDesC().AllocL();
    iEMailAddress = KNullDesC().AllocL();
    }

// -----------------------------------------------------------------------------
// CompareHighlightElements
// -----------------------------------------------------------------------------
//
TInt CompareHighlightElements( TPsMatchLocation const& aElement, TPsMatchLocation const& aElement2)
    {
    return aElement.index - aElement2.index;
    }

// -----------------------------------------------------------------------------
// CompareHighlightElements
// -----------------------------------------------------------------------------
//
void CESMRClsItem::SetHighlightsL( const RArray<TPsMatchLocation>& aHighlights )
    {
    FUNC_LOG;
    iHighlights.Reset();
    iHighlights.ReserveL( aHighlights.Count() );

    // Convert right->left to left->right, this eases the visualisation
    for ( TInt i = 0; i < aHighlights.Count(); ++i )
        {
        iHighlights.AppendL( aHighlights[i] );
        if ( iHighlights[i].direction == TBidiText::ERightToLeft )
            {
            iHighlights[i].index -= aHighlights[i].length - 1;
            iHighlights[i].direction = TBidiText::ELeftToRight;
            }
        }

    // Sort match locations according to index, this eases the visualisation
    TLinearOrder<TPsMatchLocation> highlightSortOrder( &CompareHighlightElements );
    iHighlights.Sort(highlightSortOrder);
    }

//EOF

