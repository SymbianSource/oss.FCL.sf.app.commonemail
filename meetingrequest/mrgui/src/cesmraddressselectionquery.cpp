/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implementation for mrui address selection query
*
*/


// INCLUDE FILES
#include "emailtrace.h"
#include "cesmraddressselectionquery.h"

#include <aknlists.h>
#include <aknPopup.h>
#include <bautils.h>
#include <esmrgui.rsg>
#include <data_caging_path_literals.hrh>

#include "esmrhelper.h"//locateresourcefile

// CONSTANTS
// Unnamed namespace for local definitions
namespace { // codescanner::namespace

// resource file:
_LIT( KResourceFileName, "esmrgui.rsc" );

// List box format
_LIT( KListItemFormat, "%S\t%S" );

// List box data separator
_LIT( KSeparator, "\t" );

// Address can be 255 max, plus separators
const TInt KMaxLength( 300 );

// Additional length for buffer needed due to separator 
const TInt KSeparatorLength( 1 );


} // namespace

// ======== MEMBER FUNCTIONS ========

// -----------------------------------------------------------------------------
// CESMRAddressSelectionQuery::CESMRAddressSelectionQuery
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CESMRAddressSelectionQuery::CESMRAddressSelectionQuery()
    {
    FUNC_LOG;
    // Do nothing
    }

// -----------------------------------------------------------------------------
// CESMRAddressSelectionQuery::~CESMRAddressSelectionQuery
// -----------------------------------------------------------------------------
//
CESMRAddressSelectionQuery::~CESMRAddressSelectionQuery()
    {
    FUNC_LOG;
    iEnv->DeleteResourceFile( iResourceOffset );
    }

// -----------------------------------------------------------------------------
// CESMRAddressSelectionQuery::NewL
// -----------------------------------------------------------------------------
//
CESMRAddressSelectionQuery* CESMRAddressSelectionQuery::NewL()
    {
    FUNC_LOG;
    CESMRAddressSelectionQuery* self = new (ELeave) CESMRAddressSelectionQuery();
    CleanupStack::PushL( self );
    self->ConstructL( );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CESMRAddressSelectionQuery::ConstructL
// -----------------------------------------------------------------------------
//
void CESMRAddressSelectionQuery::ConstructL()
    {
    FUNC_LOG;
    TFileName filename;

    iEnv = CEikonEnv::Static();  // codescanner::eikonenvstatic

    ESMRHelper::LocateResourceFile(
            KResourceFileName,
            KDC_RESOURCE_FILES_DIR,
            filename,
            &iEnv->FsSession() );

    //for localization
    BaflUtils::NearestLanguageFile(iEnv->FsSession(),filename); 
    iResourceOffset = iEnv->AddResourceFileL(filename);
    }

// -----------------------------------------------------------------------------
// CESMRAddressSelectionQuery::ExecuteAddressSelectionListL
// -----------------------------------------------------------------------------
//
TInt CESMRAddressSelectionQuery::ExecuteAddressSelectionListL( 
        const RPointerArray<HBufC>& aAddressArray )
    {
    FUNC_LOG;
    TInt ret = ExecuteL( aAddressArray );
    return ret;
    }

// -----------------------------------------------------------------------------
// CESMRAddressSelectionQuery::ExecuteL
// -----------------------------------------------------------------------------
//
TInt CESMRAddressSelectionQuery::ExecuteL( 
        const RPointerArray<HBufC>& aAddressArray )
    {
    FUNC_LOG;
    TInt response( KErrCancel );

    CAknDoublePopupMenuStyleListBox* list = 
        new( ELeave )CAknDoublePopupMenuStyleListBox;
    CleanupStack::PushL( list );
    
    CAknPopupList* popupList = CAknPopupList::NewL( list,
            R_AVKON_SOFTKEYS_SELECT_CANCEL,
            AknPopupLayouts::EMenuDoubleWindow );
    CleanupStack::PushL( popupList );

    list->ConstructL( popupList, EAknListBoxMenuList );
    list->CreateScrollBarFrameL( ETrue );
    list->ScrollBarFrame()->SetScrollBarVisibilityL( CEikScrollBarFrame::EOff,
                                                    CEikScrollBarFrame::EOn );
    
    CDesCArrayFlat* itemArray = new(ELeave)CDesCArrayFlat( aAddressArray.Count() );
    CleanupStack::PushL( itemArray );

    SetListQueryTextsL( itemArray, popupList, aAddressArray );

    CleanupStack::Pop( itemArray );
    list->Model()->SetItemTextArray( itemArray );
    list->Model()->SetOwnershipType( ELbmOwnsItemArray );
    
    // Show popup list
    TInt popupOk = popupList->ExecuteLD();

    if ( popupOk )
        {
        response = list->CurrentItemIndex();
        }

    CleanupStack::Pop( popupList );
    CleanupStack::PopAndDestroy( list );

    return response;
    }

// -----------------------------------------------------------------------------
// CESMRAddressSelectionQuery::SetListQueryTextsL
// -----------------------------------------------------------------------------
//

void CESMRAddressSelectionQuery::SetListQueryTextsL( 
        CDesCArrayFlat* aItemArray,
        CAknPopupList* aPopupList,
        const RPointerArray<HBufC>& aAddressArray )
    {
    FUNC_LOG;
    // Set title for popup
    aPopupList->SetTitleL( *iEnv->AllocReadResourceAsDes16LC( 
            R_QTN_MEET_REQ_LOC_ADDRESS_SELECTION_TITLE ) );
    CleanupStack::PopAndDestroy(); // Resource string

    // Amount of addresses the contact has
    TInt itemCount = aAddressArray.Count();
  
    RBuf item;
    item.Create( KMaxLength );
    item.CleanupClosePushL();
    
    for ( TInt i(0); i < itemCount; ++i )
        {
        // Descriptor has separator between label and the actual address
        TPtr16 temp = aAddressArray[i]->Des();

        // Let's truncate too long adresses
        if( temp.Length() > KMaxLength )
            {
            temp.SetLength( KMaxLength );
            }
        
        // Finding the first instance of a separator
        TInt offset = temp.Find( KSeparator );
        
        if( 0 <= offset && offset < KMaxLength )
            {
            // Forming label from the beginning to the first separator
            TPtrC16 tempLabel = temp.Mid( 0, offset );
            
            // Forming address from the first separator to the end
            TPtrC16 tempAddress = temp.Mid( 
                    offset + KSeparatorLength, 
                    temp.Length() - offset - KSeparatorLength );
                
            // appending address item to array
            item.Format( KListItemFormat, &tempLabel, &tempAddress );
            aItemArray->AppendL( item );
            }
        
        }
    CleanupStack::PopAndDestroy( &item );
    }

//  End of File

