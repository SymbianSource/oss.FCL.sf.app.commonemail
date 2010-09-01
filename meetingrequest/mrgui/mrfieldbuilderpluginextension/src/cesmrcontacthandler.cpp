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
* Description:  ESMR Contact Handler for fetching contacts for attendee field
*
*/

#include "emailtrace.h"
#include "cesmrcontacthandler.h"

//INCLUDES
#include <eikenv.h>
#include <StringLoader.h>
#include <s32mem.h>

//Virtual Phonebook
#include <CVPbkContactManager.h>
#include <MVPbkContactOperationBase.h>
#include <MVPbkStoreContact.h>
#include <MVPbkFieldType.h>
#include <VPbkEng.rsg>
#include <CVPbkContactFieldIterator.h>
#include <MVPbkContactFieldTextData.h>
#include <MVPbkContactLink.h>

//Remote Contact Lookup
#include <cpbk2remotecontactlookupaccounts.h>
#include <pbk2remotecontactlookupfactory.h>
#include <CPbk2SortOrderManager.h>

// Aiw launcher
#include <AiwCommon.hrh>
#include <AiwContactSelectionDataTypes.h>
#include <AiwServiceHandler.h>
#include <AiwGenericParam.h>

//FS Email framework
#include "cfsmailbox.h"

//MRUI
#include <esmrgui.rsg>
#include "cesmrcontactmanagerhandler.h"
#include "cesmrclslistshandler.h"
#include "mesmrcontacthandlerobserver.h"
#include "cesmrclsitem.h"
#include "esmrfieldbuilderdef.h"

// Constants

// Unnamed namespace for local definitions
namespace{

template<typename T> class CleanupResetAndDestroyClose
    {
    public:
        inline static void PushL( T& aRef );
    private:
        static void Close( TAny *aPtr );
    };

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
// CESMRContactHandler::NewL
// -----------------------------------------------------------------------------
//
CESMRContactHandler* CESMRContactHandler::NewL()
    {
    FUNC_LOG;

    CESMRContactHandler* self = CESMRContactHandler::NewLC();
    CleanupStack::Pop(self);

    return self;
    }

// -----------------------------------------------------------------------------
// CESMRContactHandler::NewLC
// -----------------------------------------------------------------------------
//
CESMRContactHandler* CESMRContactHandler::NewLC()
    {
    FUNC_LOG;
    CESMRContactHandler* self = new (ELeave) CESMRContactHandler();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CESMRContactHandler::CESMRContactHandler
// -----------------------------------------------------------------------------
//
CESMRContactHandler::CESMRContactHandler():
    iState(EContactHandlerIdle)
    {
    FUNC_LOG;
    //do nothing
    }

// -----------------------------------------------------------------------------
// CESMRContactHandler::ConstructL
// -----------------------------------------------------------------------------
//
void CESMRContactHandler::ConstructL()
    {
    FUNC_LOG;
    iESMRStatic.ConnectL();
    CESMRContactManagerHandler& contactManagerHandler = iESMRStatic.ContactManagerHandlerL();

    if ( !contactManagerHandler.IsReady() )
        {
        contactManagerHandler.AddObserverL( this );
        }

    iContactManager = &contactManagerHandler.GetContactManager();
    }

// -----------------------------------------------------------------------------
// CESMRContactHandler::ContactManagerReady
// -----------------------------------------------------------------------------
//
void CESMRContactHandler::ContactManagerReady()
    {
    FUNC_LOG;
    CESMRContactManagerHandler* contactManagerHandler = NULL;
    TRAPD( error, contactManagerHandler = &iESMRStatic.ContactManagerHandlerL());
    if ( error != KErrNone )
        {
        CEikonEnv::Static()->HandleError(error);// codescanner::eikonenvstatic
        }
    else
        {
        contactManagerHandler->RemoveObserver( this );
        }
    }

// -----------------------------------------------------------------------------
// CESMRContactHandler::~CESMRContactHandler
// -----------------------------------------------------------------------------
//
CESMRContactHandler::~CESMRContactHandler()
    {
    FUNC_LOG;
    delete iClsListHandler;
    delete iServiceHandler;
    iLastnameFields.Reset();
    iFirstnameFields.Reset();
    iEmailFields.Reset();
    delete iSearchMatch;
    iContactObjects.ResetAndDestroy();
    delete iLinksSet;
    delete iLinkOperationFetch;
    delete iCurrentLink;
    iESMRStatic.Close();
    if(iESMRMailBoxStaticCalled)
        {
        iESMRMailBoxStatic.Close();
        }
    }

// -----------------------------------------------------------------------------
// CESMRContactHandler::SearchMatchesL
// -----------------------------------------------------------------------------
//
void CESMRContactHandler::SearchMatchesL( const TDesC& aText,
                                          MESMRContactHandlerObserver* aObserver )
    {
    FUNC_LOG;
    if ( (iState == EContactHandlerIdle) ||
         (iState == EContactHandlerSearchMatches)  )
        {
        iState = EContactHandlerSearchMatches;
        iHandlerObserver = aObserver;
        
        //delayed launch of listhandler
        if(!iClsListHandler)
            {
            iClsListHandler = CESMRClsListsHandler::NewL(
                    CEikonEnv::Static()->FsSession(), // codescanner::eikonenvstatic
                    iContactManager );
            iClsListHandler->SetObserver( this );
            }
        
        iClsListHandler->SetCurrentMailboxL( &DelayedMailBoxL() );
        iClsListHandler->SearchMatchesL( aText );
        }
    }

// -----------------------------------------------------------------------------
// CESMRContactHandler::RemoteLookupSupportedL
// -----------------------------------------------------------------------------
//
TBool CESMRContactHandler::RemoteLookupSupportedL()
    {
    FUNC_LOG;
    DelayedMailBoxL();
    return iRemoteLookupSupported;
    }

// -----------------------------------------------------------------------------
// CESMRContactHandler::DelayedMailBoxL
// -----------------------------------------------------------------------------
//
CFSMailBox& CESMRContactHandler::DelayedMailBoxL()
    {
    FUNC_LOG;
    if(!iESMRMailBoxStaticCalled)
        {
        iESMRMailBoxStatic.ConnectL();
        iMailBox = &iESMRMailBoxStatic.DefaultFSMailBoxL();
        iRemoteLookupSupported = iMailBox->HasCapability( EFSMBoxCapaSupportsRCL );
        iESMRMailBoxStaticCalled = ETrue;
        }
    
    return *iMailBox;
    }
// -----------------------------------------------------------------------------
// CESMRContactHandler::GetContactManager
// -----------------------------------------------------------------------------
//
CVPbkContactManager* CESMRContactHandler::GetContactManager()
    {
    FUNC_LOG;
    return iContactManager;
    }

// -----------------------------------------------------------------------------
// CESMRContactHandler::GetAddressesFromPhonebookL
// -----------------------------------------------------------------------------
//
void CESMRContactHandler::GetAddressesFromPhonebookL( MESMRContactHandlerObserver* aObserver )
    {
    FUNC_LOG;
    if ( iState == EContactHandlerIdle )
        {
        if ( !aObserver )
            {
            //Request can't be fulfilled without observer
            return;
            }
        iHandlerObserver = aObserver;
        iState = EContactHandlerGetAddressesFromPhonebook;
        TUint fetchFlags = 0;

        iContactObjects.ResetAndDestroy();

        //start service handler and add the interests of this class
        if(!iServiceHandler)
            {
            iServiceHandler = CAiwServiceHandler::NewL();
            iServiceHandler->AttachL( R_CONTACT_SELECTION_INTEREST );
            }
        
        CAiwGenericParamList& inParamList = iServiceHandler->InParamListL();
        inParamList.AppendL(
                TAiwGenericParam(
                        EGenericParamContactSelectionData,
                        TAiwVariant(TAiwMultipleItemSelectionDataV1Pckg(
                                TAiwMultipleItemSelectionDataV1().
                                SetAddressSelectType(EAiwEMailSelect).
                                SetFlags( fetchFlags )))));

        iServiceHandler->ExecuteServiceCmdL(
                KAiwCmdSelect,
                inParamList,
                iServiceHandler->OutParamListL(),
                0,
                this);
        }
    }

// -----------------------------------------------------------------------------
// CESMRContactHandler::ArrayUpdatedL
// -----------------------------------------------------------------------------
//
void CESMRContactHandler::ArrayUpdatedL( RPointerArray<CESMRClsItem>& aMatchingItems )
    {
    FUNC_LOG;
    switch ( iState )
            {
            case EContactHandlerSearchMatches:
                if ( aMatchingItems.Count() == 1 )
                    {
                    if ( !iSearchMatch )
                        {
                        iSearchMatch = CESMRClsItem::NewL();
                        }
                    iSearchMatch->SetDisplayNameL( aMatchingItems[0]->DisplayName() );
                    iSearchMatch->SetEmailAddressL( aMatchingItems[0]->EmailAddress() );
                    }
                else
                    {
                    delete iSearchMatch;
                    iSearchMatch = NULL;
                    }

                // The ownership of the array content is transferred to observer.
                iState = EContactHandlerIdle;
                if ( iHandlerObserver )
                    {
                    iHandlerObserver->OperationCompleteL( ESearchContacts, &aMatchingItems );
                    }
                break;
                
            default:
                break;
            }
    }

// -----------------------------------------------------------------------------
// CESMRContactHandler::OperationErrorL
// -----------------------------------------------------------------------------
//
void CESMRContactHandler::OperationErrorL( TInt aErrorCode )
    {
    FUNC_LOG;
    if ( aErrorCode != KErrNone )
        {
        iState = EContactHandlerIdle;
        if ( iHandlerObserver )
            {
            iHandlerObserver->OperationErrorL( ESearchContacts, aErrorCode );
            }
        }
    }

// -----------------------------------------------------------------------------
// CESMRContactHandler::VPbkSingleContactOperationComplete
// -----------------------------------------------------------------------------
//
void CESMRContactHandler::VPbkSingleContactOperationComplete(
        MVPbkContactOperationBase& aOperation,
        MVPbkStoreContact* aContact )
    {
    FUNC_LOG;
    TRAP_IGNORE( VPbkSingleContactOperationCompleteL( aOperation, aContact) );
    }

// -----------------------------------------------------------------------------
// CESMRContactHandler::VPbkSingleContactOperationFailed
// -----------------------------------------------------------------------------
//
void CESMRContactHandler::VPbkSingleContactOperationFailed(
        MVPbkContactOperationBase& aOperation, TInt aError )
    {
    FUNC_LOG;
    TRAP_IGNORE( VPbkSingleContactOperationFailedL( aOperation, aError ) );
    }

// -----------------------------------------------------------------------------
// CESMRContactHandler::VPbkSingleContactOperationFailedL
// -----------------------------------------------------------------------------
//
void CESMRContactHandler::VPbkSingleContactOperationFailedL(
        MVPbkContactOperationBase& /*aOperation*/, TInt aError )
    {
    FUNC_LOG;
    iState = EContactHandlerIdle;
    if ( iHandlerObserver )
        {
        iHandlerObserver->OperationErrorL( EGetAddressesFromPhonebook, aError );
        iHandlerObserver = NULL;
        }
    }

// -----------------------------------------------------------------------------
// CESMRContactHandler::HandleNotifyL
// -----------------------------------------------------------------------------
//
TInt CESMRContactHandler::HandleNotifyL( TInt aCmdId, 
                                         TInt aEventId,
                                         CAiwGenericParamList& aEventParamList,
                                         const CAiwGenericParamList& /*aInParamList*/)
    {
    FUNC_LOG;
    TInt result = 0;
    if ( (aCmdId == KAiwCmdSelect) && 
         (iState == EContactHandlerGetAddressesFromPhonebook) &&
         (aEventId == KAiwEventCompleted))
        {
        TInt index = 0;
        const TAiwGenericParam* param = 
            aEventParamList.FindFirst(index, EGenericParamContactLinkArray);
        if (param)
            {
            TPtrC8 contactLinks = param->Value().AsData();

            iLinksSet = iContactManager->CreateLinksLC(contactLinks);
            CleanupStack::Pop();
            if ( iLinksSet->Count() )
                {
                iCurrentLink = &iLinksSet->At(0);
                iLinkOperationFetch = iContactManager->RetrieveContactL( 
                        iLinksSet->At(0), *this );
                }
            }
        }
    else if ( aEventId == KAiwEventError )
        {
        iState = EContactHandlerIdle;
        if ( iHandlerObserver )
            {
            //Notify caller of the error
            iHandlerObserver->OperationErrorL( ESearchContacts, KErrGeneral );
            iHandlerObserver = NULL;
            }
        }
    else if ( (aEventId == KAiwEventQueryExit) )
        {
        // We don't need to check outParams, 
        // or do anything special during the exit
        result = 1;
        }
    else if ( aEventId == KAiwEventOutParamCheck && aCmdId == KAiwCmdSelect )
        {
        // We want select this contact
        result = 1;
        }
    else if ( aEventId == KAiwEventCanceled )
        {
        iState = EContactHandlerIdle;
        if ( iHandlerObserver )
            {
            RPointerArray<CESMRClsItem> emptyItems;
            iHandlerObserver->OperationCompleteL( ESearchContacts, &emptyItems );
            emptyItems.Close();
            iHandlerObserver = NULL;
            }
        }

    return result;
    }

// -----------------------------------------------------------------------------
// CESMRContactHandler::GetContactFieldsL
// -----------------------------------------------------------------------------
//
void CESMRContactHandler::GetContactFieldsL( RArray<TInt>& aFieldIds, RPointerArray<HBufC>& aNumbers,
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

// -----------------------------------------------------------------------------
// CESMRContactHandler::LaunchRemoteLookupL
// -----------------------------------------------------------------------------
//
TBool CESMRContactHandler::LaunchRemoteLookupL( const TDesC& aQueryString,
                                                TDes& aDisplayname, 
                                                TDes& aEmailAddress )
    {
    FUNC_LOG;
    CPbkxRemoteContactLookupServiceUiContext::TResult result;
    DoRemoteLookupL( aQueryString, 
                     result,
                     CPbkxRemoteContactLookupServiceUiContext::EModeContactSelector);

    if ( result.iExitReason == CPbkxRemoteContactLookupServiceUiContext::TResult::EExitContactSelected )
        {
        GetNameAndEmail( aDisplayname, aEmailAddress, *(result.iSelectedContactItem) );
        return ETrue;
        }

    return EFalse;
    }

// -----------------------------------------------------------------------------
// CESMRContactHandler::GetLastSearchNameLC
// -----------------------------------------------------------------------------
//
HBufC* CESMRContactHandler::GetLastSearchNameLC( const TDesC& aEmailAddress )
    {
    FUNC_LOG;
    HBufC* name(NULL);
    if ( iSearchMatch )
        {
        if ( !iSearchMatch->EmailAddress().CompareF( aEmailAddress ) )
            {
            if ( iSearchMatch->DisplayName() != KNullDesC )
                {
                name = HBufC::NewLC( iSearchMatch->DisplayName().Length() );
                name->Des().Copy( iSearchMatch->DisplayName() );
                }
            }
        }

    return name;
    }

// -----------------------------------------------------------------------------
// CESMRContactHandler::GetNameAndEmail
// -----------------------------------------------------------------------------
//
void CESMRContactHandler::GetNameAndEmail( TDes& aName, TDes& aEmail, CContactItem& aItem )
    {
    FUNC_LOG;
    CContactItemFieldSet& fieldSet = aItem.CardFields();
    aName.Zero();

    RBuf familyName;
    RBuf givenName;
    TInt pos;

    pos = fieldSet.Find(KUidContactFieldFamilyName);
    if (pos >= 0)
        {
        CContactItemField& itemField=fieldSet[pos];
        if (!(itemField.IsHidden()) && !(itemField.IsDisabled()))
            {
            CContactTextField* textField = itemField.TextStorage();
            familyName.Create( textField->Text() );
            }
        }
    pos = fieldSet.Find(KUidContactFieldGivenName);
    if (pos >= 0)
        {
        CContactItemField& itemField=fieldSet[pos];
        if (!(itemField.IsHidden()) && !(itemField.IsDisabled()))
            {
            CContactTextField* textField = itemField.TextStorage();
            givenName.Create( textField->Text() );
            }
        }
    RBuf email;
    pos = fieldSet.Find(KUidContactFieldEMail);
    if (pos >= 0)
        {
        CContactItemField& itemField=fieldSet[pos];
        if (!(itemField.IsHidden()) && !(itemField.IsDisabled()))
            {
            CContactTextField* textField = itemField.TextStorage();
            email.Create( textField->Text() );
            aEmail.Copy( email );
            }
        }
    email.Close();


    if ( familyName.Length() )
        {
        aName.Append(givenName);
        aName.Trim();
        }

    if ( givenName.Length() )
        {
        aName.Append(' ');
        aName.Append(familyName);
        aName.Trim();
        }

    givenName.Close();
    familyName.Close();
    }

// -----------------------------------------------------------------------------
// CESMRContactHandler::DoRemoteLookupL
// -----------------------------------------------------------------------------
//
void CESMRContactHandler::DoRemoteLookupL( const TDesC& aQueryString,
    CPbkxRemoteContactLookupServiceUiContext::TResult& aResult,
    CPbkxRemoteContactLookupServiceUiContext::TMode aLookupMode )
    {
    FUNC_LOG;
    TUid protocolUid = TUid::Null();
    TUint accountUid = 0;
    DelayedMailBoxL().GetRCLInfo( protocolUid, accountUid );
    const TPbkxRemoteContactLookupProtocolAccountId accountId =
        TPbkxRemoteContactLookupProtocolAccountId( protocolUid, accountUid );

    CPbkxRemoteContactLookupServiceUiContext::TContextParams params = 
        { accountId, aLookupMode }; 

    CPbkxRemoteContactLookupServiceUiContext* context =
        Pbk2RemoteContactLookupFactory::NewContextL( params );
    CleanupStack::PushL( context );

    context->ExecuteL( aQueryString, aResult );
    CleanupStack::PopAndDestroy( context );
    }

// -----------------------------------------------------------------------------
// CESMRContactHandler::VPbkSingleContactOperationCompleteL
// -----------------------------------------------------------------------------
//
void CESMRContactHandler::VPbkSingleContactOperationCompleteL(
        MVPbkContactOperationBase& aOperation,
        MVPbkStoreContact* aContact )
    {
    if ( (iLinkOperationFetch == &aOperation) && iState == EContactHandlerGetAddressesFromPhonebook )
        {
        CleanupDeletePushL( aContact );

        //setup contact field arrays, only must be done on first result
        if( iEmailFields.Count() < 1 )
            {
            iEmailFields.Append( R_VPBK_FIELD_TYPE_EMAILGEN );
            iEmailFields.Append( R_VPBK_FIELD_TYPE_EMAILWORK );
            iEmailFields.Append( R_VPBK_FIELD_TYPE_EMAILHOME );
            iFirstnameFields.Append( R_VPBK_FIELD_TYPE_FIRSTNAME );
            iLastnameFields.Append( R_VPBK_FIELD_TYPE_LASTNAME );
            }
        
        RPointerArray<HBufC> firstname;
        CleanupResetAndDestroyClosePushL( firstname );
        RPointerArray<HBufC> lastname;
        CleanupResetAndDestroyClosePushL( lastname );
        RPointerArray<HBufC> emailAddresses;
        CleanupResetAndDestroyClosePushL( emailAddresses );
        GetContactFieldsL( iFirstnameFields, firstname, aContact );
        GetContactFieldsL( iLastnameFields, lastname, aContact );

        const TInt KMaxNameLength = 255;
        // Create display name, this will be used in UI.
        HBufC* displayname = HBufC::NewLC( KMaxNameLength );
        if ( firstname.Count() )
            {
            displayname->Des().Copy( *firstname[0] );
            }
        if ( lastname.Count() )
            {
            if ( firstname.Count() )
                {
                displayname->Des().Append( KEmptySpace );
                }
            displayname->Des().Append( *lastname[0] );
            }

        // retrieve selected email address
        MVPbkStoreContactFieldCollection& fields = aContact->Fields();
        MVPbkBaseContactField* selectedField = fields.RetrieveField( *iCurrentLink );
        HBufC* selectedEmailAddress = MVPbkContactFieldTextData::Cast(
                        selectedField->FieldData() ).Text().AllocL();
        CleanupStack::PushL( selectedEmailAddress );
        
        // create contact object
        CESMRClsItem* item = CESMRClsItem::NewLC();
        item->SetDisplayNameL( displayname->Des() );
        item->SetEmailAddressL( *selectedEmailAddress );
        //check number of email addreses
        GetContactFieldsL( iEmailFields, emailAddresses, aContact );
        if ( emailAddresses.Count() > 1 )
            {
            item->SetMultipleEmails();
            }
        CleanupStack::Pop( item );
        iContactObjects.Append( item );

        CleanupStack::PopAndDestroy( selectedEmailAddress );
        selectedEmailAddress = NULL;

        CleanupStack::PopAndDestroy( displayname );
        CleanupStack::PopAndDestroy( &emailAddresses );
        CleanupStack::PopAndDestroy( &lastname );
        CleanupStack::PopAndDestroy( &firstname );
        CleanupStack::PopAndDestroy( aContact );

        TInt index = (iLinksSet && iCurrentLink ? iLinksSet->Find(*iCurrentLink) + 1 : 0);
        if (iLinksSet && index < iLinksSet->Count())
            {
            iCurrentLink = &iLinksSet->At(index);
            delete iLinkOperationFetch;
            iLinkOperationFetch = NULL;
            iLinkOperationFetch = iContactManager->RetrieveContactL(
                *iCurrentLink, *this);
            }
        else
            {
            delete iLinkOperationFetch;
            iLinkOperationFetch = NULL;
            iCurrentLink = NULL;
            iState = EContactHandlerIdle;
            delete iLinksSet;
            iLinksSet = NULL;

            if ( iHandlerObserver )
                {
                iHandlerObserver->OperationCompleteL( EGetAddressesFromPhonebook, &iContactObjects );
                iHandlerObserver = NULL;
                }
            }
        }
    }

//EOF

