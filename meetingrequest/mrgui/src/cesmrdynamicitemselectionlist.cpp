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
* Description:  Implementation for mrui dynamic item selection list
*
*/


// INCLUDE FILES
#include "emailtrace.h"
#include "cesmrdynamicitemselectionlist.h"

#include <aknlists.h>
#include <aknpopup.h>
#include <bautils.h>
#include <esmrgui.rsg>
#include <data_caging_path_literals.hrh>

#include "esmrhelper.h"//locateresourcefile

// CONSTANTS
// Unnamed namespace for local definitions
namespace { // codescanner::namespace

#ifdef _DEBUG
    _LIT( KESMRDynamicItemSelectionListPanicTxt, "ESMRDYNAMICITEMSELECTIONLIST" );
    /** Enumeration for panic codes */
    enum TESMRDynamicItemSelectionListPanicCode
        {
        // Errors
        EESMRDynamicItemSelectioListInvalidListType = 0
        };

    void Panic( TESMRDynamicItemSelectionListPanicCode aPanic )
        {
        User::Panic ( KESMRDynamicItemSelectionListPanicTxt, aPanic );
        }
#endif
    
// resource file:
_LIT( KResourceFileName, "esmrgui.rsc" );

} // namespace

// ======== MEMBER FUNCTIONS ========

// -----------------------------------------------------------------------------
// CESMRAddressSelectionQuery::CESMRAddressSelectionQuery
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CESMRDynamicItemSelectionList::CESMRDynamicItemSelectionList()
    {
    FUNC_LOG;
    // Do nothing
    }

// -----------------------------------------------------------------------------
// CESMRDynamicItemSelectionList::~CESMRDynamicItemSelectionList
// -----------------------------------------------------------------------------
//
CESMRDynamicItemSelectionList::~CESMRDynamicItemSelectionList()
    {
    FUNC_LOG;
    iEnv->DeleteResourceFile( iResourceOffset );
    }

// -----------------------------------------------------------------------------
// CESMRDynamicItemSelectionList::NewL
// -----------------------------------------------------------------------------
//
CESMRDynamicItemSelectionList* CESMRDynamicItemSelectionList::NewL()
    {
    FUNC_LOG;
    CESMRDynamicItemSelectionList* self = 
                            new (ELeave) CESMRDynamicItemSelectionList();
    CleanupStack::PushL( self );
    self->ConstructL( );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CESMRDynamicItemSelectionList::ConstructL
// -----------------------------------------------------------------------------
//
void CESMRDynamicItemSelectionList::ConstructL()
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
// CESMRDynamicItemSelectionList::ExecuteL
// -----------------------------------------------------------------------------
//
TInt CESMRDynamicItemSelectionList::ExecuteL( // codescanner::intleaves
        const RPointerArray<HBufC>& aItemArray,
        const TESMRSelectionListType aType )
    {
    FUNC_LOG;
    CPtrCArray* array = new (ELeave) CPtrCArray( 1 );
    CleanupStack::PushL( array );
    array->SetReserveL( aItemArray.Count() );
    
    for ( TInt i = 0; i < aItemArray.Count(); ++i )
        {
        array->AppendL( *aItemArray[i] );
        }
    
    TInt retVal = ExecuteL( *array, aType );
    CleanupStack::PopAndDestroy( array );
    return retVal;
    }

// -----------------------------------------------------------------------------
// CESMRDynamicItemSelectionList::ExecuteL
// -----------------------------------------------------------------------------
//
TInt CESMRDynamicItemSelectionList::ExecuteL( // codescanner::intleaves
		MDesCArray& aItemArray,
		const TESMRSelectionListType aType )
    {
    FUNC_LOG;
    TInt response( KErrCancel );
    
    CEikFormattedCellListBox* listBox = NULL;
    CAknPopupList* popupList = NULL;
    
    switch( aType )
        {
        case EESMRAddressSelectionList:
            {
            listBox = new( ELeave )CAknDoublePopupMenuStyleListBox;
            CleanupStack::PushL( listBox );
            
            popupList = CAknPopupList::NewL( listBox,
                    R_AVKON_SOFTKEYS_SELECT_CANCEL__SELECT,
                    AknPopupLayouts::EMenuDoubleWindow );
            CleanupStack::PushL( popupList );
            
            popupList->SetTitleL( 
                    *iEnv->AllocReadResourceAsDes16LC(
                            R_QTN_MEET_REQ_LOC_ADDRESS_SELECTION_TITLE ) );
            CleanupStack::PopAndDestroy(); // Resource string //codescanner::cleanup
            break;
            }
        case EESMRRecentLocationList:
            {
            listBox = new( ELeave )CAknSinglePopupMenuStyleListBox;
            CleanupStack::PushL( listBox );
            
            popupList = CAknPopupList::NewL( listBox,
                    R_AVKON_SOFTKEYS_SELECT_CANCEL__SELECT,
                    AknPopupLayouts::EMenuWindow );
            CleanupStack::PushL( popupList );
            
            popupList->SetTitleL( 
                    *iEnv->AllocReadResourceAsDes16LC(
                            R_QTN_MEET_REQ_RECENT_LOCATIONS_SELECTION_TITLE ) ); 
            CleanupStack::PopAndDestroy(); // Resource string //codescanner::cleanup
            break;
            }
        default:
            {
            __ASSERT_DEBUG( 
                    EFalse, Panic( EESMRDynamicItemSelectioListInvalidListType ));
            break;
            }
        }
    
    listBox->ConstructL( popupList, EAknListBoxMenuList );
    listBox->CreateScrollBarFrameL( ETrue );
    listBox->ScrollBarFrame()->SetScrollBarVisibilityL( 
            CEikScrollBarFrame::EOff, 
            CEikScrollBarFrame::EOn );
    
    listBox->Model()->SetItemTextArray( &aItemArray );
    listBox->Model()->SetOwnershipType( ELbmDoesNotOwnItemArray );
    
    // Show popup list
    TBool popupOk = popupList->ExecuteLD();
    if ( popupOk )
        {
        response = listBox->CurrentItemIndex();
        }

    CleanupStack::Pop( popupList );
    CleanupStack::PopAndDestroy( listBox );
    
    return response;
    }

//  End of File

