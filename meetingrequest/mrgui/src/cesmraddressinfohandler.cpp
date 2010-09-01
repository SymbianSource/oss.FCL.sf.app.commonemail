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
* Description:  ESMR Address info handler implementation
*
*/

// Includes
#include "emailtrace.h"
#include "cesmraddressinfohandler.h"

#include <eikenv.h>
#include <AiwServiceHandler.h>
#include <AiwContactSelectionDataTypes.h>
#include <StringLoader.h>
#include <aknnotewrappers.h>
#include <calentry.h>
#include <esmrgui.rsg>

//Virtual Phonebook
#include <CVPbkContactManager.h>
#include <MVPbkContactOperationBase.h>
#include <MVPbkStoreContact.h>
#include <MVPbkFieldType.h>
#include <VPbkEng.rsg>
#include <CVPbkContactFieldIterator.h>
#include <MVPbkContactFieldTextData.h>
#include <MVPbkContactLink.h>

#include "cesmrdynamicitemselectionlist.h"
#include "cesmrlocationpluginhandler.h"

namespace { //codescanner::namespace
/**
 * Cleanup helpers for RPointerArrays
 */
template <class T>
class CleanupResetAndDestroy
    {

public:
    
    inline static void PushL( T& aRef )
        {
        CleanupStack::PushL( TCleanupItem( &ResetAndDestroy, &aRef ) );
        }
    
private:
    inline static void ResetAndDestroy( TAny* aPtr )
        {
        static_cast< T*>( aPtr )->ResetAndDestroy();
        }
    };

template <class T>
inline void CleanupResetAndDestroyPushL( T& aRef )
    {
    CleanupResetAndDestroy<T>::PushL( aRef );
    }

// Tabulator for address selection list formatting 
_LIT( KTab, "\t" );

} //namespace

// ---------------------------------------------------------------------------
// CESMRAddressInfoHandler::NewL
// ---------------------------------------------------------------------------
//
CESMRAddressInfoHandler* CESMRAddressInfoHandler::NewL()
    {
    FUNC_LOG;
    CESMRAddressInfoHandler* self = 
        new (ELeave) CESMRAddressInfoHandler;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CESMRAddressInfoHandler::CESMRAddressInfoHandler
// ---------------------------------------------------------------------------
//
CESMRAddressInfoHandler::CESMRAddressInfoHandler()
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CESMRAddressInfoHandler::~CESMRAddressInfoHandler
// ---------------------------------------------------------------------------
//
CESMRAddressInfoHandler::~CESMRAddressInfoHandler()
    {
    FUNC_LOG;
    delete iWait;
    delete iServiceHandler;
    delete iLinksSet;
    delete iLinkOperationFetch;
    iAddressGeneralFields.Reset();
    iAddressHomeFields.Reset();
    iAddressWorkFields.Reset();
    
    iCompleteAddresses.ResetAndDestroy();
    
    iESMRStatic.Close();
    }

// ---------------------------------------------------------------------------
// CESMRAddressInfoHandler::ConstructL
// ---------------------------------------------------------------------------
//
void CESMRAddressInfoHandler::ConstructL()
    {
    FUNC_LOG;
    iWait = new( ELeave ) CActiveSchedulerWait;
    
    iESMRStatic.ConnectL();
    CESMRContactManagerHandler& contactManagerHandler = iESMRStatic.ContactManagerHandlerL();

    iContactManager = &contactManagerHandler.GetContactManager();

    //start service handler and add the interests of this class
    iServiceHandler = CAiwServiceHandler::NewL();
    iServiceHandler->AttachL( R_CONTACT_SELECTION_INTEREST );
    
    //setup contact field arrays
    iAddressGeneralFields.AppendL( R_VPBK_FIELD_TYPE_ADDRLABELGEN );
    iAddressGeneralFields.AppendL( R_VPBK_FIELD_TYPE_ADDRSTREETGEN );
    iAddressGeneralFields.AppendL( R_VPBK_FIELD_TYPE_ADDRLOCALGEN );
    iAddressGeneralFields.AppendL( R_VPBK_FIELD_TYPE_ADDRCOUNTRYGEN );
        
    iAddressWorkFields.AppendL( R_VPBK_FIELD_TYPE_ADDRLABELWORK );
    iAddressWorkFields.AppendL( R_VPBK_FIELD_TYPE_ADDRSTREETWORK );
    iAddressWorkFields.AppendL( R_VPBK_FIELD_TYPE_ADDRLOCALWORK );
    iAddressWorkFields.AppendL( R_VPBK_FIELD_TYPE_ADDRCOUNTRYWORK );
        
    iAddressHomeFields.AppendL( R_VPBK_FIELD_TYPE_ADDRLABELHOME );
    iAddressHomeFields.AppendL( R_VPBK_FIELD_TYPE_ADDRSTREETHOME );
    iAddressHomeFields.AppendL( R_VPBK_FIELD_TYPE_ADDRLOCALHOME );
    iAddressHomeFields.AppendL( R_VPBK_FIELD_TYPE_ADDRCOUNTRYHOME );
    }

// ---------------------------------------------------------------------------
// CESMRAddressInfoHandler::HandleNotifyL
// ---------------------------------------------------------------------------
//
TInt CESMRAddressInfoHandler::HandleNotifyL( TInt aCmdId, //codescanner::intleaves
                        TInt aEventId,
                        CAiwGenericParamList& aEventParamList,
                        const CAiwGenericParamList& /*aInParamList*/)
    {
    FUNC_LOG;
    if ( iLinksSet )
        {
        delete iLinksSet;
        iLinksSet = NULL;
        }
    
    if ( iLinkOperationFetch )
        {
        delete iLinkOperationFetch;
        iLinkOperationFetch = NULL;
        }
    TInt result = 0;
    
    if ((aCmdId == KAiwCmdSelect) && aEventId == KAiwEventCompleted )
        {
        TInt index = 0;
        const TAiwGenericParam* param = 
            aEventParamList.FindFirst(index, EGenericParamContactLinkArray);
        if (param)
            {
            TPtrC8 contactLinks = param->Value().AsData();

            iLinksSet = iContactManager->CreateLinksLC( contactLinks );
            CleanupStack::Pop(); //codescanner::cleanup
            if ( iLinksSet->Count() )
                {
                iLinkOperationFetch = iContactManager->
                    RetrieveContactL( iLinksSet->At(0), *this );
                }
            }
            
        }
    else if ( aEventId == KAiwEventError )
        {
        if ( iWait->IsStarted())
            {
            // Stop iWait so editor dialog continues 
            iWait->AsyncStop();
            }
        }
    else if ( (aEventId == KAiwEventQueryExit) )
        {
        if ( iWait->IsStarted())
            {
            // Stop iWait so editor dialog continues 
            iWait->AsyncStop();
            }
        // We don't need to check outParams, or do anything special during the exit
        result = ETrue;
        }
    else if ( aEventId == KAiwEventOutParamCheck )
        {
        // Parameters don't need to be checked
        result = 1;
        }
    else if ( aEventId == KAiwEventCanceled )
        {
        if ( iWait->IsStarted())
            {
            // Stop iWait so editor dialog continues 
            iWait->AsyncStop();
            }
        }

    return result;
    }
    
// ---------------------------------------------------------------------------
// CESMRAddressInfoHandler::SearchAddressFromContacts
// ---------------------------------------------------------------------------
//
TBool CESMRAddressInfoHandler::SearchAddressFromContactsL( MESMRCalEntry& aEntry )
    {
    FUNC_LOG;
    if( !(iWait->IsStarted()) )
        {
        iEntry = &aEntry;
        iEntryUpdated = EFalse;
    
        TUint fetchFlags = 0;

        CAiwGenericParamList& inParamList = iServiceHandler->InParamListL();
        inParamList.AppendL(
                TAiwGenericParam(
                        EGenericParamContactSelectionData,
                        TAiwVariant(TAiwSingleEntrySelectionDataV2Pckg(
                                TAiwSingleEntrySelectionDataV2().                          
                                SetFlags( fetchFlags )))));

        iServiceHandler->ExecuteServiceCmdL(
            KAiwCmdSelect,
            inParamList,
            iServiceHandler->OutParamListL(),
            0,
            this);
        iWait->Start(); // codescanner::callActiveObjectWithoutCheckingOrStopping
        }
    
    return iEntryUpdated;
    }

// ---------------------------------------------------------------------------
// CESMRAddressInfoHandler::VPbkSingleContactOperationComplete
// ---------------------------------------------------------------------------
//
void CESMRAddressInfoHandler::VPbkSingleContactOperationComplete(
                MVPbkContactOperationBase& aOperation,
                MVPbkStoreContact* aContact )
    {
    FUNC_LOG;
    TRAP_IGNORE( VPbkSingleContactOperationCompleteL( aOperation, aContact ) );
    if ( iWait->IsStarted() )
        {
        // Stop iWait so editor dialog continues 
        iWait->AsyncStop();
        }
    
    }

// ---------------------------------------------------------------------------
// CESMRAddressInfoHandler::VPbkSingleContactOperationFailed
// ---------------------------------------------------------------------------
//
void CESMRAddressInfoHandler::VPbkSingleContactOperationFailed(
                MVPbkContactOperationBase& /*aOperation*/, 
                TInt /*aError*/ )
    {
    FUNC_LOG;
    if ( iWait->IsStarted() )
        {
        iWait->AsyncStop();
        }
    }

// ---------------------------------------------------------------------------
// CESMRAddressInfoHandler::VPbkSingleContactOperationCompleteL
// ---------------------------------------------------------------------------
//
void CESMRAddressInfoHandler::VPbkSingleContactOperationCompleteL(
                MVPbkContactOperationBase& aOperation,
                MVPbkStoreContact* aContact )
    {
    FUNC_LOG;
    if ( iLinkOperationFetch == &aOperation )
        {
        CleanupDeletePushL( aContact ); 
        
        if( iCompleteAddresses.Count() )
            {
            iCompleteAddresses.ResetAndDestroy();
            }
        
        RPointerArray<HBufC> homeAddress;
        CleanupResetAndDestroyPushL( homeAddress );
        RPointerArray<HBufC> workAddress;
        CleanupResetAndDestroyPushL( workAddress );
        RPointerArray<HBufC> generalAddress;
        CleanupResetAndDestroyPushL( generalAddress );

        GetContactFieldsL( iAddressHomeFields, homeAddress, aContact );
        GetContactFieldsL( iAddressWorkFields, workAddress, aContact );
        GetContactFieldsL( iAddressGeneralFields, generalAddress, aContact );
        
        FormatAddressL( homeAddress, EHomeAddress );
        FormatAddressL( workAddress, EWorkAddress );
        FormatAddressL( generalAddress, EGeneralAddress );
        

        
        // Contact has more than one addresses, address selection 
        // dialog need to be shown.
        if ( iCompleteAddresses.Count() > 1 )
            {
            // Show address selection dialog
            CESMRDynamicItemSelectionList* query = 
                    CESMRDynamicItemSelectionList::NewL();
            CleanupStack::PushL( query );
            TInt index = 
                query->ExecuteL( 
                        iCompleteAddresses, 
                        CESMRDynamicItemSelectionList::EESMRAddressSelectionList );
            CleanupStack::PopAndDestroy( query );
            if ( index >= 0 && ( index < iCompleteAddresses.Count() ) ) 
                {
                UpdateEntryL( index );
                }
            }
        
        // Contact has only one address
        else if ( iCompleteAddresses.Count() == 1 )
            {
            UpdateEntryL( 0 );
            }
        
        // Contact doesn't have any address, information note is shown
        else if ( iCompleteAddresses.Count() == 0 )
            {
            HBufC* noAddressBuf = 
                StringLoader::LoadL( R_MEET_REQ_NO_ADDRESS_FOUND );
            CleanupStack::PushL( noAddressBuf );
            CAknInformationNote* infNote = 
                new (ELeave) CAknInformationNote();
            infNote->ExecuteLD( *noAddressBuf );
            // note will be deleted automatically 
            // and pointer will be set to NULL after execution
            CleanupStack::PopAndDestroy( noAddressBuf );            
            }

        CleanupStack::PopAndDestroy( &generalAddress );
        CleanupStack::PopAndDestroy( &workAddress );
        CleanupStack::PopAndDestroy( &homeAddress );

        CleanupStack::PopAndDestroy( aContact );
        }
    }

// ---------------------------------------------------------------------------
// CESMRAddressInfoHandler::GetContactFieldsL
// ---------------------------------------------------------------------------
//
void CESMRAddressInfoHandler::GetContactFieldsL( RArray<TInt>& aFieldIds,
                                             RPointerArray<HBufC>& aNumbers,
                                             MVPbkStoreContact* aContact)
    {
    FUNC_LOG;
    for (TInt i = 0; i < aFieldIds.Count(); i++ )
        {
        const MVPbkFieldType*  myContactDataField =
        iContactManager->FieldTypes().Find( aFieldIds[i] );

        CVPbkBaseContactFieldTypeIterator* itrNumber =
        CVPbkBaseContactFieldTypeIterator::NewLC( *myContactDataField,
                aContact->Fields() );

        // Iterate through each of the data fields
        while ( itrNumber->HasNext() )
            {
            const MVPbkBaseContactField* field = itrNumber->Next();

            if ( (field->FieldData()).DataType() == EVPbkFieldStorageTypeText )
                {
                HBufC* toAppend = MVPbkContactFieldTextData::Cast(
                        field->FieldData() ).Text().AllocL();

                aNumbers.Append( toAppend );
                }
            }
        CleanupStack::PopAndDestroy( itrNumber );
        }
    }

// ----------------------------------------------------------------------------
// CESMRAddressInfoHandler::FormatAddressL
// ----------------------------------------------------------------------------
//
void CESMRAddressInfoHandler::FormatAddressL( RPointerArray<HBufC>& aAddresses,
                    CESMRAddressInfoHandler::TAddressType aAddressType )
    {
    FUNC_LOG;
    if ( aAddresses.Count() > 0 )
        {      
        HBufC* addressTitle = NULL;
        switch ( aAddressType )
            {
            case EHomeAddress:
                {
                addressTitle = StringLoader::LoadL( R_MEET_REQ_HOME_ADDRESS );
                break;
                }
            case EWorkAddress:
                {
                addressTitle = StringLoader::LoadL( R_MEET_REQ_WORK_ADDRESS );
                break;
                }
            case EGeneralAddress:
                {
                addressTitle = 
                    StringLoader::LoadL( R_MEET_REQ_GENERAL_ADDRESS );
                break;
                }
            default:
                {
                User::Leave( KErrGeneral );
                }
            }
        CleanupStack::PushL( addressTitle);
        
        CPtrCArray* addressPtr = 
            new( ELeave ) CPtrCArray( aAddresses.Count());
        CleanupStack::PushL( addressPtr );
        
        for (TInt i = 0; i < aAddresses.Count(); i++ )
            {
            addressPtr->AppendL( aAddresses[i]->Des() );
            }
        HBufC* streetAddress = NULL;
        switch ( aAddresses.Count())
            {
            case EFormatOneItem:
                {
                streetAddress = aAddresses[0]->AllocL(); 
                break;
                }
            case EFormatTwoItems:
                {
                streetAddress = StringLoader::
                    LoadL( R_MEET_REQ_ADDRESS_FROM_CONTACTS_TWO_ITEMS, 
                            *addressPtr );
                break;
                }
            case EFormatThreeItems:
                {
                streetAddress = StringLoader::
                    LoadL( R_MEET_REQ_ADDRESS_FROM_CONTACTS_THREE_ITEMS, 
                            *addressPtr );
                break;
                }
            case EFormatFourItems:
                {
                streetAddress = StringLoader::
                    LoadL( R_MEET_REQ_ADDRESS_FROM_CONTACTS_FORMAT, 
                            *addressPtr );
                break;
                }
            default:
                {
                User::Leave( KErrGeneral );
                }
            }
        
        CleanupStack::PopAndDestroy( addressPtr );
        CleanupStack::PushL( streetAddress );
        streetAddress = streetAddress->ReAllocL( 
                streetAddress->Length() + addressTitle->Length() +1 );
        // streetAddress item before reallocation need to be 
        // removed from cleanupstack.
        CleanupStack::Pop(); //codescanner::cleanup
        CleanupStack::PushL( streetAddress );
        
        
        streetAddress->Des().Insert( 0, KTab );
        streetAddress->Des().Insert( 0, *addressTitle );
        iCompleteAddresses.Append( streetAddress );
        CleanupStack::Pop( streetAddress );
        CleanupStack::PopAndDestroy( addressTitle );
        }
    }

// ---------------------------------------------------------------------------
// CESMRAddressInfoHandler::UpdateEntryL
// ---------------------------------------------------------------------------
//
void CESMRAddressInfoHandler::UpdateEntryL( TInt aIndex )
    {
    FUNC_LOG;
    TInt offset = iCompleteAddresses[aIndex]->Find( KTab ); //codescanner::accessArrayElementWithoutCheck2
    TPtrC address( iCompleteAddresses[aIndex]->Mid( offset + 1) ); //codescanner::accessArrayElementWithoutCheck2
    TBool replaced;
    iEntryUpdated =
        CESMRLocationPluginHandler::UpdateEntryLocationL(
                *iEntry,
                address,
                EFalse,
                replaced );
    }

//EOF

