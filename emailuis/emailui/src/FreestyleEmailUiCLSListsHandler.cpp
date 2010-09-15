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
* Description:  CLS lists class implementation
*
*/

#include "emailtrace.h"
#include <eikenv.h>
#include <centralrepository.h>
#include <AknFepInternalCRKeys.h>
#include "cfsmailbox.h"
#include <CPsRequestHandler.h>
#include <CPsSettings.h>
#include <VPbkEng.rsg>

#include "FreestyleEmailUiCLSListsHandler.h"
#include "FreestyleEMailUiCLSMatchObserverInterface.h"
#include "FreestyleEmailUiInputModeObserver.h"
#include "FreestyleEmailUiUtilities.h"
#include "FreestyleEmailUiCLSItem.h"

// ================= MEMBER FUNCTIONS ==========================================
// -----------------------------------------------------------------------------
// CFSEmailUiClsListsHandler::NewL
// -----------------------------------------------------------------------------
CFSEmailUiClsListsHandler* CFSEmailUiClsListsHandler::NewL( RFs& aFs,
        CVPbkContactManager* aContactManager )
	{
    FUNC_LOG;
	CFSEmailUiClsListsHandler* object = 
	    CFSEmailUiClsListsHandler::NewLC( aFs, aContactManager );
	CleanupStack::Pop( object );
	return object;
	}

// -----------------------------------------------------------------------------
// CFSEmailUiClsListsHandler::NewLC
// -----------------------------------------------------------------------------
CFSEmailUiClsListsHandler* CFSEmailUiClsListsHandler::NewLC( 
        RFs& aFs, CVPbkContactManager* aContactManager )
	{
    FUNC_LOG;
	CFSEmailUiClsListsHandler* object = 
	        new (ELeave) CFSEmailUiClsListsHandler( aFs, aContactManager );
	CleanupStack::PushL( object );
	object->ConstructL();
	return object;
	} 

// -----------------------------------------------------------------------------
// CFSEmailUiClsListsHandler::~CFSEmailUiClsListsHandler
// -----------------------------------------------------------------------------
CFSEmailUiClsListsHandler::~CFSEmailUiClsListsHandler()
	{
    FUNC_LOG;
	delete iSearchedText;
	delete iPcsMatchObserver;

	iMatchingCompleteContacts.Reset();
	iMatchingMRUContacts.Reset();
    iMatchingMissingEmailContacts.Reset();

    delete iAllMRUContacts;
    delete iInputObserver;
	delete iAknFepCenRep;
	delete iRequestHandler;
    }


// -----------------------------------------------------------------------------
// CFSEmailUiClsListsHandler::UpdateContactMatchListsL
// -----------------------------------------------------------------------------
void CFSEmailUiClsListsHandler::UpdateContactMatchListsL(
    const RPointerArray<CFSEmailUiClsItem>& aMatches )
	{
    FUNC_LOG;

    TInt matchCount = aMatches.Count();
	for( TInt i = 0 ; i < matchCount ; ++i )
		{
        if( aMatches[i]->IsMruItem() )
            {
            // MRU items are added to iMatchingMRUContacts and any already
            // added duplicate is removed from iMatchingCompleteContacts
            iMatchingMRUContacts.AppendL( aMatches[i] );
            TInt itemIndex = FindDuplicate( *aMatches[i], 
                    iMatchingCompleteContacts );
            if ( KErrNotFound != itemIndex )
                {
                iMatchingCompleteContacts.Remove( itemIndex );
                }
            }
        else if( aMatches[i]->EmailAddress().Length() > 0 )
            {
            // For Phonebook items, it is checked that it there doesn't 
            // already exist a duplicate in iMatchingMRUContacts
            if( KErrNotFound == 
                FindDuplicate( *aMatches[i], iMatchingMRUContacts ) )
                {
                // No duplicate in iMatchingMRUContacts, 
                // this can be appended to iMatchingCompleteContacts
                iMatchingCompleteContacts.AppendL( aMatches[i] );
                }
            }
        else
            {
            // No email, add always to the end of the list
            iMatchingMissingEmailContacts.AppendL( aMatches[i] );
            }
        }
	
    RPointerArray<CFSEmailUiClsItem> allMatches = 
            ConstructOneListL( iMatchingCompleteContacts,
                               iMatchingMRUContacts,
                               iMatchingMissingEmailContacts );
    CleanupResetAndDestroyClosePushL( allMatches ); // Ownership is taken
    iClsListObserver->ArrayUpdatedL( allMatches ); 
    CleanupStack::PopAndDestroy( &allMatches ); 
    }

// -----------------------------------------------------------------------------
// CFSEmailUiClsListsHandler::InputModeChangedL
// -----------------------------------------------------------------------------
void CFSEmailUiClsListsHandler::InputModeChangedL( 
        TKeyboardModes aNewInputMode )
	{
    FUNC_LOG;
	iPcsMatchObserver->SetInputMode( aNewInputMode );
	}


// -----------------------------------------------------------------------------
// CFSEmailUiClsListsHandler::CurrentSearchText
// -----------------------------------------------------------------------------
const TDesC& CFSEmailUiClsListsHandler::CurrentSearchText()
	{
    FUNC_LOG;
	return *iSearchedText;
	}

// -----------------------------------------------------------------------------
// CFSEmailUiClsListsHandler::SearchMatchesL
// -----------------------------------------------------------------------------
void CFSEmailUiClsListsHandler::SearchMatchesL( const TDesC& aText )
	{
    FUNC_LOG;
	// The ownership of the object is in caller of UpdateContactMatchListsL
	iMatchingCompleteContacts.Reset(); 
	iMatchingMRUContacts.Reset();
	iMatchingMissingEmailContacts.Reset();
    
	delete iSearchedText;
	iSearchedText = NULL;
	iSearchedText = aText.AllocL();

	iPcsMatchObserver->SearchMatchesL( aText );
	}


// -----------------------------------------------------------------------------
// CFSEmailUiClsListsHandler::CFSEmailUiClsListsHandler
// -----------------------------------------------------------------------------
CFSEmailUiClsListsHandler::CFSEmailUiClsListsHandler( 
        RFs& aFs, CVPbkContactManager* aContactManager ) :
	iContactManager( aContactManager ),
	iClsListObserver( NULL ), 
	iFs( aFs ),
	iMailBox ( NULL ),
	iUseLastNameFirstOrder( EFalse )
	{
    FUNC_LOG;
	// Nothing
	}

// -----------------------------------------------------------------------------
// CFSEmailUiClsListsHandler::ConstructL
// -----------------------------------------------------------------------------
void CFSEmailUiClsListsHandler::ConstructL()
	{
    FUNC_LOG;
	iRequestHandler = CPSRequestHandler::NewL();  
	iAknFepCenRep = CRepository::NewL( KCRUidAknFep );
	iPcsMatchObserver = CFSEmailUiClsMatchObserver::NewL( 
	        *iAknFepCenRep, *this, *iRequestHandler, iContactManager );
	SetSearchSettingsForPcsMatchObserverL();
	
    // Monitors inputMode changes (predictive vs. non-predictive
	iInputObserver = CFSEmailUiInputModeObserver::NewL( *iAknFepCenRep, *this );

	if( !iSearchedText )
	    {
		iSearchedText = KNullDesC().AllocL();
	    }
    }

// -----------------------------------------------------------------------------
// CFSEmailUiClsListsHandler::CopyClsItemL
// -----------------------------------------------------------------------------
CFSEmailUiClsItem* CFSEmailUiClsListsHandler::CopyClsItemLC(
    const CFSEmailUiClsItem& aClsItem )
    {
    FUNC_LOG;
    CFSEmailUiClsItem* newClsItem = aClsItem.CloneLC();
    return newClsItem;
    }

// -----------------------------------------------------------------------------
// CFSEmailUiClsListsHandler::ReadCLSInfoFromMRUListIndexL
// -----------------------------------------------------------------------------
TBool CFSEmailUiClsListsHandler::ReadCLSInfoFromMRUListIndexL( 
        MDesCArray& aTextArray,
        CFSEmailUiClsItem& aClsItem,
        TInt aCurrentMatchIndex, 
        TInt aPreviousMatchIndex )
	{
    FUNC_LOG;
	TBool retVal = EFalse;	
	// This should be even number
	TInt currentRealItemIndex = 
	        aCurrentMatchIndex - ( aCurrentMatchIndex % 2 );
	// Here we need to check if match is found both from the display name and email address 
	if( currentRealItemIndex != aPreviousMatchIndex )
		{
		aClsItem.SetDisplayNameL( 
		        aTextArray.MdcaPoint( currentRealItemIndex ) );
		aClsItem.SetEmailAddressL( 
		        aTextArray.MdcaPoint( currentRealItemIndex + 1 ) );
		retVal = ETrue;
		}
	return retVal;
	}


// -----------------------------------------------------------------------------
// CFSEmailUiClsListsHandler::FindAndDestroyDuplicate
// -----------------------------------------------------------------------------
TInt CFSEmailUiClsListsHandler::FindDuplicate( 
        const CFSEmailUiClsItem& aClsItem,
        RPointerArray<CFSEmailUiClsItem>& aContacts )
	{
    FUNC_LOG;
	// find duplicate email addresses from aContacts
	const TDesC& emailAddress = aClsItem.EmailAddress();

	for( TInt i = 0; i < aContacts.Count(); ++i )
		{
		const TDesC& currentItemEmailAddress = aContacts[i]->EmailAddress();
		
		if( emailAddress.CompareF( currentItemEmailAddress ) == 0 )
			{
			return i;
			}
		}
	
	return KErrNotFound;
	}

// -----------------------------------------------------------------------------
// CFSEmailUiClsListsHandler::ConstructOneListL
// -----------------------------------------------------------------------------
RPointerArray<CFSEmailUiClsItem> CFSEmailUiClsListsHandler::ConstructOneListL( 
        const RPointerArray<CFSEmailUiClsItem>& aContactMatchesWithEmail,
        const RPointerArray<CFSEmailUiClsItem>& aMRUMatches,
        const RPointerArray<CFSEmailUiClsItem>& aContactMatchesWithoutEmail )
	{
    FUNC_LOG;
	RPointerArray<CFSEmailUiClsItem> allMatches;
	CleanupResetAndDestroyClosePushL( allMatches );
	
	// Copy all the objects from three other lists to the all matches list
   TInt matchingMRUItemsCount = aMRUMatches.Count();
    for ( TInt i = 0 ; i < matchingMRUItemsCount ; i++ )
        {
        CFSEmailUiClsItem* newClsItem = CopyClsItemLC( *aMRUMatches[i] );
        allMatches.AppendL( newClsItem );
        CleanupStack::Pop( newClsItem );
        }

	TInt matchingComleteItemsCount = aContactMatchesWithEmail.Count();
	for( TInt i = 0 ; i <  matchingComleteItemsCount; i++ )
		{
		CFSEmailUiClsItem* newClsItem = 
		        CopyClsItemLC( *aContactMatchesWithEmail[i] );
		allMatches.AppendL( newClsItem );
		CleanupStack::Pop( newClsItem );
		}

	TInt mathingContactItemsWithoutEmailCount = 
	        aContactMatchesWithoutEmail.Count();
	for( TInt i = 0 ; i < mathingContactItemsWithoutEmailCount ; i++ )
		{
		CFSEmailUiClsItem* newClsItem = 
		        CopyClsItemLC( *aContactMatchesWithoutEmail[i] );
		allMatches.AppendL( newClsItem );
		CleanupStack::Pop( newClsItem );
		}

	CleanupStack::Pop( &allMatches );	
	return allMatches;
	}

// -----------------------------------------------------------------------------
// CFSEmailUiClsListsHandler::SetSearchSettingsForPcsMatchObserverL()
// -----------------------------------------------------------------------------
void CFSEmailUiClsListsHandler::SetSearchSettingsForPcsMatchObserverL()
    {
    FUNC_LOG;
    // Create predictive search settings
    CPsSettings* searchSettings = CPsSettings::NewL();
    CleanupStack::PushL( searchSettings );

    RPointerArray<TDesC> databases;
    CleanupClosePushL( databases );

    HBufC* store1 = HBufC::NewLC( 50 );
    store1->Des().Copy( KVPbkDefaultCntDbURI ); // Phone contacts store
    
    databases.AppendL( store1 );
    HBufC* store2 = NULL;
    if ( iMailBox )
        {
        store2 = HBufC::NewLC(50);
        GetMruDatastoreUriFromMailbox( *iMailBox, *store2 );
        databases.AppendL( store2 );
        }
    searchSettings->SetSearchUrisL( databases );

    // Set displayfields according to sort order
    RArray<TInt> sortOrder;
    CleanupClosePushL( sortOrder );
    iRequestHandler->GetSortOrderL( *store1, sortOrder );
    if ( sortOrder.Count() )
        {
        iUseLastNameFirstOrder = 
                ( sortOrder[0] == R_VPBK_FIELD_TYPE_LASTNAME );
        }
    else
        {
        iUseLastNameFirstOrder = EFalse; // default is not to change order
        }
    RArray<TInt> displayFields( 6 );
    CleanupClosePushL( displayFields );
    if ( sortOrder.Count() ) 
        {
        displayFields.AppendL( sortOrder[0] );
        }
    if ( sortOrder.Count() >= 1 ) 
        {
        displayFields.AppendL( sortOrder[1] );
        }
    displayFields.AppendL( R_VPBK_FIELD_TYPE_EMAILGEN );
    displayFields.AppendL( R_VPBK_FIELD_TYPE_EMAILHOME );
    displayFields.AppendL( R_VPBK_FIELD_TYPE_EMAILWORK );
    searchSettings->SetDisplayFieldsL( displayFields );
    CleanupStack::Pop( &displayFields );
    CleanupStack::Pop( &sortOrder );
    displayFields.Close();
    sortOrder.Close();

    // Set the new search settings
    iRequestHandler->SetSearchSettingsL( *searchSettings );	

    if ( store2 )
        {
        CleanupStack::PopAndDestroy( store2 );
        }
    CleanupStack::PopAndDestroy( store1 );
    CleanupStack::Pop( &databases );
    databases.Close();
    CleanupStack::PopAndDestroy( searchSettings );

	}

// -----------------------------------------------------------------------------
// CFSEmailUiClsListsHandler::IsLanguageSupported()
// -----------------------------------------------------------------------------
TBool CFSEmailUiClsListsHandler::IsLanguageSupportedL()
	{
    FUNC_LOG;
	// Get the current language
	TLanguage lang = User::Language();

	// Check for language support
	return iRequestHandler->IsLanguageSupportedL( lang );
	}

// -----------------------------------------------------------------------------
// CFSEmailUiClsListsHandler::GetMruDatastoreUriFromMailbox()
// -----------------------------------------------------------------------------
void CFSEmailUiClsListsHandler::GetMruDatastoreUriFromMailbox( 
        CFSMailBox& aMailbox, HBufC& aUri )
	{
    FUNC_LOG;
	aUri.Des().Copy( KDefaultMailBoxURI );
	aUri.Des().AppendNum( aMailbox.GetId().PluginId().iUid );
	aUri.Des().Append( KDefaultMailBoxURISeparator );
	aUri.Des().AppendNum( aMailbox.GetId().Id() );
	}

// -----------------------------------------------------------------------------
// CFSEmailUiClsListsHandler::SetObserver()
// -----------------------------------------------------------------------------
void CFSEmailUiClsListsHandler::SetObserver( 
        MFSEmailUiClsListsObserver* aClsListObserver )
    {
    FUNC_LOG;
    iClsListObserver = aClsListObserver;
    }

// -----------------------------------------------------------------------------
// CFSEmailUiClsListsHandler::SetCurrentMailboxL()
// -----------------------------------------------------------------------------
void CFSEmailUiClsListsHandler::SetCurrentMailboxL( CFSMailBox* aMailBox )
    {
    FUNC_LOG;
    TFSMailMsgId currentMailboxId;
    TFSMailMsgId newMailboxId;
  
    if( iMailBox )
    	{
    	currentMailboxId = iMailBox->GetId();
    	}
    if( aMailBox )
    	{
    	newMailboxId = aMailBox->GetId();
    	}
    
    if( currentMailboxId != newMailboxId )
    	{
        iMailBox = aMailBox;
    	SetSearchSettingsForPcsMatchObserverL();
    	}
    }

// -----------------------------------------------------------------------------
// CFSEmailUiClsListsHandler::OperationErrorL()
// -----------------------------------------------------------------------------
void CFSEmailUiClsListsHandler::OperationErrorL( TInt aErrorCode )
	{
    FUNC_LOG;
	iClsListObserver->OperationErrorL( aErrorCode );
	}

// -----------------------------------------------------------------------------
// CFSEmailUiClsListsHandler::UseLastNameFirstOrder()
// -----------------------------------------------------------------------------
TBool CFSEmailUiClsListsHandler::UseLastNameFirstOrder()
    {
    FUNC_LOG;
    return iUseLastNameFirstOrder;
    }
