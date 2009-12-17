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
* Description:  CLS lists class implementation
 *
*/

#include "emailtrace.h"
#include "cesmrclslistshandler.h"

#include <eikenv.h>                                 // CEikonEnv
#include <centralrepository.h>                      // CRepository
#include <AknFepInternalCRKeys.h>                   // KCRUidAknFep
#include <CPbkContactEngine.h>                      // CPbkContactEngine
#include <CPbkContactItem.h>                        // CPbkContactItem, TPbkContactItemField
#include <aknnotewrappers.h>                        // CAknErrorNote
//<cmail>
#include "CFSMailBox.h"                             // cfsmailbox
//</cmail>
#include <CPsRequestHandler.h>
#include <CPsSettings.h>
#include <CPcsDefs.h>
#include <VPbkEng.rsg>
#include <e32const.h>                               // TLanguage

#include <CVPbkContactManager.h>

#include "mesmrclslistsobserver.h"
#include "cesmrclsmatchobserver.h"
#include "cesmrinputmodeobserver.h"
#include "esmrfieldbuilderdef.h"

// CONSTANTS

// Unnamed namespace for local definitionsm functions and templates
namespace{ // codescanner::namespace

const TInt KMaxMRUMatches = 50;
const TInt KNumDisplayFields = 6;
const TInt KMaxUriLen = 50;

template<typename T> class CleanupResetAndDestroyClose
    {
    public:
        inline static void PushL( T& aRef );
    private:
        static void Close( TAny *aPtr );
    };

template<typename T> inline void CleanupResetAndDestroyClosePushL( T& aRef );

template<typename T> inline void CleanupResetAndDestroyClose<T>::PushL( T& aRef )
    {
    CleanupStack::PushL( TCleanupItem( &Close, &aRef ) );
    }

template<typename T> void CleanupResetAndDestroyClose<T>::Close( TAny *aPtr )
    {
    static_cast<T*>(aPtr)->ResetAndDestroy();
    static_cast<T*>(aPtr)->Close();
    }

template<typename T> inline void CleanupResetAndDestroyClosePushL( T& aRef )
    {
    CleanupResetAndDestroyClose<T>::PushL( aRef );
    }

}//namespace

// ======== MEMBER FUNCTIONS ========

// -----------------------------------------------------------------------------
// CESMRClsListsHandler::NewL
// -----------------------------------------------------------------------------
CESMRClsListsHandler* CESMRClsListsHandler::NewL( RFs& aFs, CVPbkContactManager* aContactManager )

    {
    FUNC_LOG;
    CESMRClsListsHandler* object =  CESMRClsListsHandler::NewLC( aFs, aContactManager );
    CleanupStack::Pop( object );
    return object;
    }

// -----------------------------------------------------------------------------
// CESMRClsListsHandler::NewLC
// -----------------------------------------------------------------------------
CESMRClsListsHandler* CESMRClsListsHandler::NewLC( RFs& aFs, CVPbkContactManager* aContactManager )
    {
    FUNC_LOG;
    CESMRClsListsHandler* object = new (ELeave) CESMRClsListsHandler( aFs, aContactManager );
    CleanupStack::PushL( object );
    object->ConstructL();
    return object;
    }

// -----------------------------------------------------------------------------
// CESMRClsListsHandler::~CESMRClsListsHandler
// -----------------------------------------------------------------------------
CESMRClsListsHandler::~CESMRClsListsHandler( )
    {
    FUNC_LOG;
    delete iSearchedText;
    delete iPcsMatchObserver;

    iMatchingCompleteContacts.Reset();
    iMatchingMRUContacts.Reset();
    iMatchingMissingEmailContacts.Reset();

    iMatchingCompleteContacts.Close();
    iMatchingMRUContacts.Close();
    iMatchingMissingEmailContacts.Close();

    delete iAllMRUContacts;
    delete iInputObserver;
    delete iAknFepCenRep;
    delete iRequestHandler;
    }

// -----------------------------------------------------------------------------
// CESMRClsListsHandler::UpdateContactMatchListsL
// -----------------------------------------------------------------------------
void CESMRClsListsHandler::UpdateContactMatchListsL( const RPointerArray<CESMRClsItem>& aMatches )
    {
    FUNC_LOG;
    for( TInt i = 0 ; i < aMatches.Count() ; ++i )
        {
        if( aMatches[i]->IsMruItem() )
            {
            // If this is MRU item, we need to check that it doesn't
            // already exist in iMatchingCompleteContacts
            TInt itemIndex = FindDuplicate( *aMatches[i], iMatchingCompleteContacts );
            if( itemIndex != KErrNotFound )
                {
                // Found, no need to add this, free memory
                }
            else
                {
                // Not found, add new to MRU list
                iMatchingMRUContacts.AppendL( aMatches[i] );
                }
            }
        else if( aMatches[i]->EmailAddress().Length() > 0 )
            {
            // If this is Phonebook item, we need to check that it doesn't
            // already exist in iMatchingMRUContacts
            TInt itemIndex = FindDuplicate( *aMatches[i], iMatchingMRUContacts );
            if( itemIndex != KErrNotFound )
                {
                // Found, this needs to be removed from the MRU list
                iMatchingMRUContacts.Remove( itemIndex );
                }

            // Phonebook items are always added to the top of the list
            iMatchingCompleteContacts.AppendL( aMatches[i] );
            }
        else
            {
            // No email, nothing to compare, so add this always to the end of the list
            iMatchingMissingEmailContacts.AppendL( aMatches[i] );
            }
        }

    RPointerArray<CESMRClsItem> allMatches = ConstructOneListL( iMatchingCompleteContacts,
                                                                      iMatchingMRUContacts,
                                                                      iMatchingMissingEmailContacts );
    if(iClsListObserver)
        {
        iClsListObserver->ArrayUpdatedL( allMatches );
        }
    
    allMatches.ResetAndDestroy();
    }

// -----------------------------------------------------------------------------
// CESMRClsListsHandler::InputModeChangedL
// -----------------------------------------------------------------------------
void CESMRClsListsHandler::InputModeChangedL( TKeyboardModes aNewInputMode )
    {
    FUNC_LOG;
    iPcsMatchObserver->SetInputMode( aNewInputMode );
    }

// -----------------------------------------------------------------------------
// CESMRClsListsHandler::CurrentSearchText
// -----------------------------------------------------------------------------
const TDesC& CESMRClsListsHandler::CurrentSearchText( )
    {
    FUNC_LOG;
    return *iSearchedText;
    }

// -----------------------------------------------------------------------------
// CESMRClsListsHandler::SearchMatchesL
// -----------------------------------------------------------------------------
void CESMRClsListsHandler::SearchMatchesL( const TDesC& aText )
    {
    FUNC_LOG;
    iMatchingCompleteContacts.Reset();
    iMatchingMRUContacts.Reset();
    iMatchingMissingEmailContacts.Reset();

    delete iSearchedText;
    iSearchedText = NULL;
    iSearchedText = aText.AllocL();

    iPcsMatchObserver->SearchMatchesL( aText );
    }

// -----------------------------------------------------------------------------
// CESMRClsListsHandler::CESMRClsListsHandler
// -----------------------------------------------------------------------------
CESMRClsListsHandler::CESMRClsListsHandler( RFs& aFs, CVPbkContactManager* aContactManager ) :
    iContactManager( aContactManager ),
    iFs( aFs )
    {
    FUNC_LOG;
    // do nothing
    }

// -----------------------------------------------------------------------------
// CESMRClsListsHandler::ConstructL
// -----------------------------------------------------------------------------
void CESMRClsListsHandler::ConstructL( )
    {
    FUNC_LOG;
    iRequestHandler = CPSRequestHandler::NewL();

    iAknFepCenRep = CRepository::NewL( KCRUidAknFep );

    iPcsMatchObserver = CESMRClsMatchObserver::NewL( *iAknFepCenRep, *this, *iRequestHandler, iContactManager );
    SetSearchSettingsForPcsMatchObserverL();

    // Monitors inputMode changes (predictive vs. non-predictive
    iInputObserver = CESMRInputModeObserver::NewL( *iAknFepCenRep, *this );

    if( !iSearchedText )
        iSearchedText = KNullDesC().AllocL();
    }

// -----------------------------------------------------------------------------
// CESMRClsListsHandler::ReadCLSInfoFromMRUListIndexL
// -----------------------------------------------------------------------------
TBool CESMRClsListsHandler::ReadCLSInfoFromMRUListIndexL(
        MDesCArray& aTextArray, CESMRClsItem& aClsItem,
        const TInt aCurrentMatchIndex, const TInt aPreviousMatchIndex )
    {
    FUNC_LOG;
    TBool retVal = EFalse;
    // This should be even number
    TInt currentRealItemIndex = aCurrentMatchIndex - ( aCurrentMatchIndex % 2 );
    // Here we need to check if match is found both from the display name and email address
    if( currentRealItemIndex != aPreviousMatchIndex )
        {
        aClsItem.SetDisplayNameL( aTextArray.MdcaPoint( currentRealItemIndex )  );
        aClsItem.SetEmailAddressL( aTextArray.MdcaPoint( currentRealItemIndex + 1 )  );
        retVal = ETrue;
        }
    return retVal;
    }

// -----------------------------------------------------------------------------
// CESMRClsListsHandler::FindDuplicate
// -----------------------------------------------------------------------------
TInt CESMRClsListsHandler::FindDuplicate( const CESMRClsItem& aClsItem,
          RPointerArray<CESMRClsItem>& aContacts )
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
// CESMRClsListsHandler::ConstructOneListL
// -----------------------------------------------------------------------------
RPointerArray<CESMRClsItem> CESMRClsListsHandler::ConstructOneListL(
        const RPointerArray<CESMRClsItem>& aContactMatchesWithEmail,
        const RPointerArray<CESMRClsItem>& aMRUMatches,
        const RPointerArray<CESMRClsItem>& aContactMatchesWithoutEmail )
    {
    RPointerArray<CESMRClsItem> allMatches;
    CleanupResetAndDestroyClosePushL( allMatches );

    // Copy all the objects from three other lists to the all matches list
    TInt matchingComleteItemsCount = aContactMatchesWithEmail.Count();
    for( TInt i = 0 ; i <  matchingComleteItemsCount; i++ )
        {
        CESMRClsItem* newClsItem = CopyClsItemLC( *aContactMatchesWithEmail[i] );
        allMatches.AppendL( newClsItem );
        CleanupStack::Pop( newClsItem );
        }

    // max MRU matches count limited to 50
    TInt matchingMRUItemsCount = aMRUMatches.Count();
    for( TInt i = 0; i < matchingMRUItemsCount && i < KMaxMRUMatches; i++ )
        {
        CESMRClsItem* newClsItem = CopyClsItemLC( *aMRUMatches[i] );
        allMatches.AppendL( newClsItem );
        CleanupStack::Pop( newClsItem );
        }

    TInt mathingContactItemsWithoutEmailCount = aContactMatchesWithoutEmail.Count();
    for( TInt i = 0 ; i < mathingContactItemsWithoutEmailCount ; i++ )
        {
        CESMRClsItem* newClsItem = CopyClsItemLC( *aContactMatchesWithoutEmail[i] );
        allMatches.AppendL( newClsItem );
        CleanupStack::Pop( newClsItem );
        }

    CleanupStack::Pop( &allMatches );
    return allMatches;
    }

// -----------------------------------------------------------------------------
// CESMRClsListsHandler::CopyClsItemL
// -----------------------------------------------------------------------------
CESMRClsItem* CESMRClsListsHandler::CopyClsItemLC( const CESMRClsItem& aClsItem )
    {
    FUNC_LOG;
    return aClsItem.CloneLC();
    }

// -----------------------------------------------------------------------------
// CESMRClsListsHandler::SetSearchSettingsForPcsMatchObserverL()
// -----------------------------------------------------------------------------
void CESMRClsListsHandler::SetSearchSettingsForPcsMatchObserverL()
    {
    FUNC_LOG;
    // Create predictive search settings
    CPsSettings* searchSettings = CPsSettings::NewL();
    CleanupStack::PushL(searchSettings);

    RPointerArray<TDesC> databases;
    CleanupClosePushL(databases);

    HBufC* store1 = HBufC::NewLC(KMaxUriLen);
    store1->Des().Copy( KVPbkDefaultCntDbURI ); // To specify phone contacts store
    databases.AppendL(store1);
    HBufC* store2 = NULL;
    if ( iMailBox ) 
        {
        store2 = HBufC::NewLC(KMaxUriLen);
        GetMruDatastoreUriFromMailbox( *iMailBox, *store2 );
        databases.AppendL( store2 );
        }
    searchSettings->SetSearchUrisL(databases);

    // Set displayfields (first name, last name, email addresses)
    RArray<TInt> displayFields(KNumDisplayFields);
    CleanupClosePushL(displayFields);
    displayFields.AppendL(R_VPBK_FIELD_TYPE_FIRSTNAME);
    displayFields.AppendL(R_VPBK_FIELD_TYPE_LASTNAME);
    displayFields.AppendL(R_VPBK_FIELD_TYPE_EMAILGEN);
    displayFields.AppendL(R_VPBK_FIELD_TYPE_EMAILHOME);
    displayFields.AppendL(R_VPBK_FIELD_TYPE_EMAILWORK);
    searchSettings->SetDisplayFieldsL(displayFields);
    CleanupStack::Pop(&displayFields);
    displayFields.Close();

    // Set the new sort order of data fields
    RArray<TInt> sortOrder;
    CleanupClosePushL(sortOrder);
    sortOrder.AppendL(R_VPBK_FIELD_TYPE_FIRSTNAME);
    sortOrder.AppendL(R_VPBK_FIELD_TYPE_LASTNAME);
    sortOrder.AppendL(R_VPBK_FIELD_TYPE_EMAILGEN);
    sortOrder.AppendL(R_VPBK_FIELD_TYPE_EMAILHOME);
    sortOrder.AppendL(R_VPBK_FIELD_TYPE_EMAILWORK);
    iRequestHandler->ChangeSortOrderL(*store1, sortOrder);
    if ( store2 )
        {
        iRequestHandler->ChangeSortOrderL(*store2, sortOrder);
        }

    CleanupStack::Pop(&sortOrder);
    sortOrder.Close();
    
    // Set the new search settings
    iRequestHandler->SetSearchSettingsL(*searchSettings);

    if ( store2 )
        {
        CleanupStack::PopAndDestroy(store2);
        }
    CleanupStack::PopAndDestroy(store1);
    CleanupStack::Pop(&databases);
    databases.Close();
    CleanupStack::PopAndDestroy(searchSettings);
    }

// -----------------------------------------------------------------------------
// CESMRClsListsHandler::isLanguageSupportedL()
// -----------------------------------------------------------------------------
TBool CESMRClsListsHandler::IsLanguageSupportedL()
    {
    FUNC_LOG;
    // Get the current language
    TLanguage lang = User::Language();

    // Check for language support
    return iRequestHandler->IsLanguageSupportedL(lang);
    }
// -----------------------------------------------------------------------------
// CESMRClsListsHandler::GetMruDatastoreUriFromMailbox()
// -----------------------------------------------------------------------------
void CESMRClsListsHandler::GetMruDatastoreUriFromMailbox( CFSMailBox& aMailbox, HBufC& aUri )
    {
    FUNC_LOG;
    aUri.Des().Copy( KDefaultMailBoxURI );
    aUri.Des().AppendNum( aMailbox.GetId().PluginId().iUid );
    aUri.Des().Append( KDefaultMailBoxURISeparator );
    aUri.Des().AppendNum( aMailbox.GetId().Id() );
    }
// -----------------------------------------------------------------------------
// CESMRClsListsHandler::SetObserver()
// -----------------------------------------------------------------------------
void CESMRClsListsHandler::SetObserver( MESMRClsListsObserver* aClsListObserver )
    {
    FUNC_LOG;
    iClsListObserver = aClsListObserver;
    }
// -----------------------------------------------------------------------------
// CESMRClsListsHandler::SetCurrentMailboxL()
// -----------------------------------------------------------------------------
void CESMRClsListsHandler::SetCurrentMailboxL( CFSMailBox* aMailBox )
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
// CESMRClsListsHandler::OperationErrorL()
// -----------------------------------------------------------------------------
void CESMRClsListsHandler::OperationErrorL( TInt aErrorCode )
    {
    FUNC_LOG;
    if ( iClsListObserver  )
        {
        iClsListObserver->OperationErrorL( aErrorCode );
        }
    }

