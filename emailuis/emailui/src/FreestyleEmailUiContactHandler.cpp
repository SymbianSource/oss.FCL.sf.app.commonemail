/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Contanct handler implementation
*
*/


#include "emailtrace.h"
#include <AknQueryDialog.h>
#include <aknlists.h>
#include <StringLoader.h>


//Virtual Phonebook 
#include <CVPbkContactManager.h>
#include <CVPbkContactStoreUriArray.h>
#include <VPbkContactStoreUris.h>       // VPbkContactStoreUris
#include <TVPbkContactStoreUriPtr.h>    // TVPbkContactStoreUriPtr
#include <MVPbkContactOperationBase.h>
#include <MVPbkStoreContact.h>
#include <MVPbkFieldType.h>
#include <VPbkEng.rsg>
#include <CVPbkContactFieldIterator.h>
#include <MVPbkContactFieldTextData.h>
#include <MVPbkContactStoreList.h>

// CCA Launcher view
#include <mccaparameter.h>
#include <ccafactory.h>
#include <ccapbkcontextlauncher.h>


// Phonebook 2
#include <CPbk2SortOrderManager.h>
#include <MPbk2ContactNameFormatter.h>
#include <Pbk2ContactNameFormatterFactory.h>

// Send UI
#include <sendui.h>
#include <SendUiConsts.h>
#include <CMessageData.h>

#include <aiwdialdataext.h>
#include <MVPbkContactLink.h>

// Remote Contact Lookup
#include <cpbk2remotecontactlookupaccounts.h>
#include <pbk2remotecontactlookupfactory.h>


#include <MPbkGlobalSetting.h> // Global setting data 
#include <PbkGlobalSettingFactory.h>

// Aiw launcher
#include <AiwDialDataTypes.h>
#include <AiwContactAssignDataTypes.h>
#include <AiwContactSelectionDataTypes.h>
#include <AiwServiceHandler.h>

// FS Email framework
#include "cfsmailbox.h"

#include <FreestyleEmailUi.rsg>

#include "FreestyleEmailUiContactHandler.h"
#include "FreestyleEmailUiCLSListsHandler.h"
#include "FreestyleEmailUiUtilities.h"
#include "FreestyleEmailUiLiterals.h"
#include "FreestyleEmailUiContactHandlerObserver.h"
#include "FreestyleEmailUiCLSItem.h"
#include "FreestyleEmailUiAppui.h"

//Constants
const TInt KInternetCallPreferred = 1;


CFSEmailUiContactHandler* CFSEmailUiContactHandler::NewL( RFs& aSession )
    {
    FUNC_LOG;
    CFSEmailUiContactHandler* self = CFSEmailUiContactHandler::NewLC( aSession );
    CleanupStack::Pop(self);
    return self;
    }

CFSEmailUiContactHandler* CFSEmailUiContactHandler::NewLC( RFs& aSession )
    {
    FUNC_LOG;
    CFSEmailUiContactHandler* self = new (ELeave) CFSEmailUiContactHandler( aSession );
    CleanupStack::PushL(self);
    self->ConstructL( aSession );
    return self;
    }

CFSEmailUiContactHandler::~CFSEmailUiContactHandler()
    {
    FUNC_LOG;
    delete iContactForMsgCreation;
    if (iPersistentSetting)
        {
        iPersistentSetting->Close();
        delete iPersistentSetting;
        }          
    delete iClsListHandler;
    delete iContactManager;
    delete iServiceHandler;
    
    ResetFieldIds();
     
    delete iPreviousEmailAddress;
    delete iSearchMatch;
   
    iContactObjects.ResetAndDestroy();

    delete iLinksSet;
    delete iLinkOperation;
    delete iLinkOperationFetch;
    delete iCurrentLink;
    delete iLastClsItem;
    
    if ( iConnection )
        {
        iConnection->Close();
        }
    }

// -----------------------------------------------------------------------------
// CFSEmailUiContactHandler::AddToContactL
// -----------------------------------------------------------------------------
void CFSEmailUiContactHandler::AddToContactL( const TDesC& aDes, 
                                                    TContactUpdateType aUpdateType,
                                                    TAddToContactsType aAddToContactsType,
                                                    MFSEmailUiContactHandlerObserver* aObserver )
    {
    FUNC_LOG;
    if ( iState == EContactHandlerIdle ) //Check that there are no other request on going
        {
        iState = EContactHandlerAddToContacts;
        iHandlerObserver = aObserver; //Can be null
        TUint assignFlags = 0;
        if ( aAddToContactsType == EContactHandlerCreateNewContact )
            {
            // In case of EUpdateExisting, we don't have to do anything as default is updateexisting
            assignFlags = AiwContactAssign::ECreateNewContact;
            }

        CAiwGenericParamList& inParamList =
        iServiceHandler->InParamListL();

        inParamList.AppendL(
                TAiwGenericParam(
                        EGenericParamContactAssignData,
                        TAiwVariant(AiwContactAssign::TAiwSingleContactAssignDataV1Pckg(
                                AiwContactAssign::TAiwSingleContactAssignDataV1().SetFlags(assignFlags)))));
        
        TGenericParamIdValue updateType;
        if ( aUpdateType == EContactUpdateEmail )
            {
            updateType = EGenericParamEmailAddress;
            }
        else // EContactUpdateNumber
            {
            updateType = EGenericParamPhoneNumber;
            }
        
        inParamList.AppendL(
                TAiwGenericParam(
                        updateType,
                        TAiwVariant(aDes)
                ));

        //Async call, CallBack is HandleNotify
        iServiceHandler->ExecuteServiceCmdL(
                KAiwCmdAssign,
                inParamList,
                iServiceHandler->OutParamListL(),
                0,
                this);
        }
    }

// -----------------------------------------------------------------------------
// CFSEmailUiContactHandler::AddToContactsQueryL
// -----------------------------------------------------------------------------
TBool CFSEmailUiContactHandler::AddtoContactsQueryL( TAddToContactsType& aType )
    {
    FUNC_LOG;
    // create popup list dialog asking new/update
    // Costruct listbox and popup list
    CEikTextListBox* listbox = new ( ELeave ) CAknSinglePopupMenuStyleListBox();
    CleanupStack::PushL( listbox );

    CAknPopupList* popupList = CAknPopupList::NewL( listbox, 
            R_AVKON_SOFTKEYS_SELECT_BACK );

    CleanupStack::PushL( popupList );
    listbox->ConstructL( popupList, CEikListBox::ELeftDownInViewRect );

    listbox->CreateScrollBarFrameL(ETrue);
    listbox->ScrollBarFrame()->SetScrollBarVisibilityL(
            CEikScrollBarFrame::EOff,
            CEikScrollBarFrame::EAuto );

    // Get list item array and put all removable theme names in it.
    MDesCArray* itemList = listbox->Model()->ItemTextArray();
    CDesCArray* listitems = static_cast< CDesCArray* >( itemList );

    // Load title for popup list.
    HBufC* resource = StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_VIEWER_ADD_TO_CONTACTS );
    popupList->SetTitleL( *resource );
    CleanupStack::PopAndDestroy( resource );            

    // Load and append actual selection lines
    resource = StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_VIEWER_CREATE_NEW_CONTACT );
    listitems->AppendL( *resource );
    CleanupStack::PopAndDestroy( resource );            

    resource = StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_VIEWER_UPDATE_EXISTING );
    listitems->AppendL( *resource );
    CleanupStack::PopAndDestroy( resource );            
    
    TBool returnValue = EFalse;
    // Execute popup list.
    if ( popupList->ExecuteLD() )
        {
        if ( !listbox->CurrentItemIndex() )
            {
            aType = EContactHandlerCreateNewContact;
            }
        else
            {
            //UpdateExisting is "default" so no flags are needed
            aType = EContactHandlerUpdateExisting;
            }
        returnValue = ETrue; //User pressed OK
        }
    CleanupStack::Pop( popupList ); 
    CleanupStack::PopAndDestroy( listbox ); 

    return returnValue;
    }

// ---------------------------------------------------------------------------
// CFSEmailUiContactHandler::FindAndCallToContactByEmailL
// ---------------------------------------------------------------------------
//
void CFSEmailUiContactHandler::FindAndCallToContactByEmailL(
    const TDesC& aEmailAddress, CFSMailBox* aMailBox,
    MFSEmailUiContactHandlerObserver* aObserver, TBool aUseSenderText )
    {
    FUNC_LOG;
    if ( iOpenComplete && (iState == EContactHandlerIdle) ) //If contact stores all ready
        {
        
        iState = EContactHandlerCallToContactByEmail;
        iHandlerObserver = aObserver;
        iUseSenderText = aUseSenderText;
        iMailBox = aMailBox;
       
        FindContactLinkL( aEmailAddress );
          
        }
    }


// -----------------------------------------------------------------------------
// CFSEmailUiContactHandler::FindAndCreateMsgToContactByEmailL
// -----------------------------------------------------------------------------
void CFSEmailUiContactHandler::FindAndCreateMsgToContactByEmailL( const TDesC& aEmailAddress,
                                                                  CFSMailBox* aMailBox )
    {
    FUNC_LOG;
    if ( iOpenComplete && (iState == EContactHandlerIdle) ) //If contact stores all ready
        {
        
        iState = EContactHandlerCreateMessage;
        iMailBox = aMailBox;        

        FindContactLinkL( aEmailAddress );
        
        }
    }


    
// ---------------------------------------------------------------------------
// Searches for the contact if necessary and shows the contact details.
// ---------------------------------------------------------------------------
//
void CFSEmailUiContactHandler::ShowContactDetailsL( const TDesC& aAddress, 
    TContactUpdateType /*aType*/, MFSEmailUiContactHandlerObserver* aObserver )
    {
    FUNC_LOG;
    iHandlerObserver = aObserver;
    iState = EContactHandlerShowContactDetails;

    FindContactLinkL( aAddress );
    
    }


void CFSEmailUiContactHandler::MakeAiwCallL(MVPbkContactLink* aContactLink,
                                            const TDesC& aContactNumber,
                                            TBool aVoipOverride )
    {
    FUNC_LOG;
    CAiwDialDataExt* dialData = CAiwDialDataExt::NewLC();
    dialData->SetWindowGroup( CCoeEnv::Static()->RootWin().Identifier() );
   
    HBufC8* linkBuffer(NULL);
    
    if( aContactLink )
        {
        linkBuffer = aContactLink->PackLC();
        dialData->SetContactLinkL( *linkBuffer );
        // Contact link exists, check if voip is preferred
        // Create iPersistentSetting here only when needed for the first time
        if ( !iPersistentSetting )
            {
            iPersistentSetting = 
                PbkGlobalSettingFactory::CreatePersistentSettingL();
            iPersistentSetting->
                ConnectL(MPbkGlobalSetting::ERichCallSettingsCategory);     
            } 
        if ( aVoipOverride || SelectBetweenCsAndVoip() == EAiwVoIPCall )
            {
            dialData->SetCallType( CAiwDialData::EAIWVoiP );            
            }
        // <cmail> video call
        else if ( iVideoCall )
            {
            iVideoCall = EFalse;
            dialData->SetCallType( CAiwDialData::EAIWForcedVideo );
            }
        // </cmail>
        else
            {
            dialData->SetCallType( CAiwDialData::EAIWVoice );           
            }                           
        }
    else
        {
        if ( aVoipOverride )
            {
            dialData->SetCallType( CAiwDialData::EAIWVoiP );            
            }
        // <cmail> video call
        else if ( iVideoCall ) 
            {
            iVideoCall = EFalse;
            dialData->SetCallType( CAiwDialData::EAIWForcedVideo );
            }
        // </cmail>
        else
            {
            dialData->SetCallType( CAiwDialData::EAIWVoice );           
            }   
        dialData->SetPhoneNumberL( aContactNumber );
        }
    
    CAiwGenericParamList& inParamList = iServiceHandler->InParamListL();
    dialData->FillInParamListL( inParamList );    
    if( aContactLink )
        {
        CleanupStack::PopAndDestroy( linkBuffer );
        }
    
    CleanupStack::PopAndDestroy( dialData );
    
    iServiceHandler->ExecuteServiceCmdL(
        KAiwCmdCall,
        inParamList,
        iServiceHandler->OutParamListL(),
        0,
        NULL );
    if ( iHandlerObserver )
        {
        RPointerArray<CFSEmailUiClsItem> emptyArray;
        iHandlerObserver->OperationCompleteL( EFindAndCallToContactByEmailL, emptyArray );
        emptyArray.Close();
        iHandlerObserver = NULL;
        }
    iState = EContactHandlerIdle;
    } 


TInt CFSEmailUiContactHandler::SelectBetweenCsAndVoip() const
    {
    FUNC_LOG;
    // Default to voice call
    TInt ret = EAiwVoice;
    
    TInt preferred = KErrNotFound;
    if ( iPersistentSetting )
        {
        iPersistentSetting->Get
            (MPbkGlobalSetting::EPreferredTelephony, preferred);    
        }
   
    // VoIP type is preferred only if the setting is,
    // KInternetCallPreferred. In other cases, like when the setting
    // is not found, voice call is preferred.
    switch (preferred)
        {
        case KInternetCallPreferred:
            {
            ret = EAiwVoIPCall;
            break;
            }
        default:
            {
            break;
            }
        }
    return ret;
    }

// ---------------------------------------------------------------------------
// Searches contacts matching the given address
// ---------------------------------------------------------------------------
//
void CFSEmailUiContactHandler::SearchMatchesL( const TDesC& aText, 
    MFSEmailUiContactHandlerObserver* aObserver, CFSMailBox* aMailBox,
    TInt aMode )
    {
    FUNC_LOG;
    if ( (iState == EContactHandlerIdle) ||
         (iState == EContactHandlerSearchMatches)  )
        {
        iState = EContactHandlerSearchMatches;
        iHandlerObserver = aObserver;
        
        iClsListHandler->SetCurrentMailboxL( aMailBox );
        
        if( aMode != EModeUndefined )
        	{
        	iClsListHandler->InputModeChangedL( (TKeyboardModes)aMode );
        	}
        //Async call, CallBack is ArrayUpdatedL (when error: OperationErrorL)
        iClsListHandler->SearchMatchesL( aText );
        }
    }

TBool CFSEmailUiContactHandler::IsLanguageSupportedL()
    {
    FUNC_LOG;
    return iClsListHandler->IsLanguageSupportedL();
    }

CVPbkContactManager* CFSEmailUiContactHandler::GetContactManager()
    {
    FUNC_LOG;
    return iContactManager;
    }


void CFSEmailUiContactHandler::GetAddressesFromPhonebookL(
    MFSEmailUiContactHandlerObserver* aObserver )
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

        CAiwGenericParamList& inParamList = iServiceHandler->InParamListL();
        inParamList.AppendL(
                TAiwGenericParam(
                        EGenericParamContactSelectionData,
                        TAiwVariant(TAiwMultipleItemSelectionDataV1Pckg(
                                TAiwMultipleItemSelectionDataV1().
                                SetAddressSelectType(EAiwEMailSelect).
                                SetFlags( fetchFlags )))));


        //Async call, CallBack is HandleNotify
        iServiceHandler->ExecuteServiceCmdL(
                KAiwCmdSelect,
                inParamList,
                iServiceHandler->OutParamListL(),
                0,
                this);  
        }
    }

// <cmail> video call
// ---------------------------------------------------------------------------
// Enables/disables video call.
// ---------------------------------------------------------------------------
//
void CFSEmailUiContactHandler::SetVideoCall( TBool aState )
    {
    iVideoCall = aState;
    }
// </cmail>

/////////////////////////////////////////////////////////////////////////////
// CFSEmailUiContactHandler::GetSmsAddressFromPhonebookAndSendL
//
/////////////////////////////////////////////////////////////////////////////
void CFSEmailUiContactHandler::GetSmsAddressFromPhonebookAndSendL( MVPbkContactLink* aContactLink )
    {
    FUNC_LOG;
    if ( iState == EContactHandlerIdle )
        {
        iState = EContactHandlerGetSmsAddressFromPhonebook;

        CAiwGenericParamList& inParamList = iServiceHandler->InParamListL();
        // Set data type select
        TAiwSingleItemSelectionDataV3 dataSelect;
        dataSelect.SetAddressSelectType( EAiwPhoneNumberSelect );
        TAiwSingleItemSelectionDataV3Pckg dataPckg( dataSelect );
        inParamList.AppendL( TAiwGenericParam( EGenericParamContactSelectionData, 
                           TAiwVariant( dataPckg )));
            
        // Append contact links         
        CVPbkContactLinkArray* links = CVPbkContactLinkArray::NewLC();
        links->AppendL( aContactLink ); // Clone ownership is transferred    
        HBufC8* packedLinks = links->PackLC();          
        inParamList.AppendL( TAiwGenericParam( EGenericParamContactLinkArray,
                          TAiwVariant(*packedLinks)));
        CleanupStack::PopAndDestroy( packedLinks ); 
        CleanupStack::PopAndDestroy( links ); 

        //Async call, CallBack is HandleNotify
        iServiceHandler->ExecuteServiceCmdL(
                KAiwCmdSelect,
                inParamList,
                iServiceHandler->OutParamListL(),
                0,
                this);  
        }
    }

/////////////////////////////////////////////////////////////////////////////
// CFSEmailUiContactHandler::GetMmsAddressFromPhonebookAndSendL
//
/////////////////////////////////////////////////////////////////////////////
void CFSEmailUiContactHandler::GetMmsAddressFromPhonebookAndSendL( MVPbkContactLink* aContactLink,
                                                            TBool aIsVoiceMessage )
    {
    FUNC_LOG;
    if ( iState == EContactHandlerIdle )
        {
        if ( aIsVoiceMessage )
            {
            iState = EContactHandlerGetVoiceMsgAddressFromPhonebook;        
            }
        else
            {
            iState = EContactHandlerGetMmsAddressFromPhonebook;         
            }

        CAiwGenericParamList& inParamList = iServiceHandler->InParamListL();
        // Set data type select
        TAiwSingleItemSelectionDataV3 dataSelect;
        dataSelect.SetAddressSelectType( EAiwMMSSelect );
        TAiwSingleItemSelectionDataV3Pckg dataPckg( dataSelect );
        inParamList.AppendL( TAiwGenericParam( EGenericParamContactSelectionData, 
                           TAiwVariant( dataPckg )));
            
        // Append contact links                 
        CVPbkContactLinkArray* links = CVPbkContactLinkArray::NewLC();
        links->AppendL( aContactLink ); // Clone ownership is transferred
        HBufC8* packedLinks = links->PackLC();          
        inParamList.AppendL( TAiwGenericParam( EGenericParamContactLinkArray,
                          TAiwVariant(*packedLinks)));
        CleanupStack::PopAndDestroy( packedLinks ); 
        CleanupStack::PopAndDestroy( links );

        //Async call, CallBack is HandleNotify
        iServiceHandler->ExecuteServiceCmdL(
                KAiwCmdSelect,
                inParamList,
                iServiceHandler->OutParamListL(),
                0,
                this);  
        }
    }

/////////////////////////////////////////////////////////////////////////////
// CFSEmailUiContactHandler::ArrayUpdatedL
//
/////////////////////////////////////////////////////////////////////////////
void CFSEmailUiContactHandler::ArrayUpdatedL(
    const RPointerArray<CFSEmailUiClsItem>& aMatchingItems )
    {
    FUNC_LOG;

    switch ( iState )
        {
        case EContactHandlerCreateMessage:
            CreateMessageL( aMatchingItems );   
            break;
        case EContactHandlerCallToContactByEmail:
            if ( aMatchingItems.Count() )
                {
                delete iLastClsItem;
                iLastClsItem = NULL;
                iLastClsItem = aMatchingItems[0]->CloneLC();
                CleanupStack::Pop();
                }
            HandleCallL( aMatchingItems );
            break;
        case EContactHandlerShowContactDetails:
            ShowDetailsL( aMatchingItems );
            break;
        case EContactHandlerSearchMatches: 
            if ( aMatchingItems.Count() == 1 )
                {
                if ( !iSearchMatch )
                    {
                    iSearchMatch = CFSEmailUiClsItem::NewL();           
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
                iHandlerObserver->OperationCompleteL( ESearchContacts, aMatchingItems );
                }
            break;
        default:
            break;
        }

    }

void CFSEmailUiContactHandler::OperationErrorL( TInt aErrorCode )
    {
    FUNC_LOG;
    if ( iHandlerObserver )
        {
        // KerrNotReady --> caching in progress, KErrNotFound --> caching not started yet
        if ( (aErrorCode == KErrNotReady) || (aErrorCode == KErrNotFound) )
            {
            iCachingInProgressError = ETrue;
            iState = EContactHandlerIdle;
            iHandlerObserver->OperationErrorL( ESearchContacts, aErrorCode );
            }
        else if ( aErrorCode == KErrNone )
            {
            // handle only if caching was in progress
            if ( iCachingInProgressError )
                {
                iCachingInProgressError = EFalse;
                iState = EContactHandlerIdle;
        iHandlerObserver->OperationErrorL( ESearchContacts, aErrorCode );
        }
    }
        else
            {
            iState = EContactHandlerIdle;
            iHandlerObserver->OperationErrorL( ESearchContacts, aErrorCode );
            }
        }
    else
        {
        iState = EContactHandlerIdle;
        }
    }


void CFSEmailUiContactHandler::VPbkSingleContactOperationComplete(
        MVPbkContactOperationBase& aOperation,
        MVPbkStoreContact* aContact )
    {
    FUNC_LOG;
//inform also client in case of error to enable client's actions (i.e. return search priority)
//  TRAP_IGNORE( VPbkSingleContactOperationCompleteL( aOperation, aContact) );
    TRAPD(error, VPbkSingleContactOperationCompleteL( aOperation, aContact) );
    if ( error != KErrNone )
        {
        TRAP_IGNORE(ObserverOperationErrorL( CurrentCommand(), error ));
        }
//
    }

void CFSEmailUiContactHandler::VPbkSingleContactOperationCompleteL(
            MVPbkContactOperationBase& aOperation,
            MVPbkStoreContact* aContact )
    {
    FUNC_LOG;
    
    if ( (iLinkOperationFetch == &aOperation) && iState == EContactHandlerGetAddressesFromPhonebook )
        {
        CleanupDeletePushL( aContact );
        
        RPointerArray<HBufC> firstname;
        CleanupResetAndDestroyClosePushL( firstname );
        RPointerArray<HBufC> lastname;
        CleanupResetAndDestroyClosePushL( lastname );
        RPointerArray<HBufC> emailAddresses;            
        CleanupResetAndDestroyClosePushL( emailAddresses );
        GetContactFieldsL( iFirstnameFields, firstname, aContact );
        GetContactFieldsL( iLastnameFields, lastname, aContact );
    
        // Create display name, this will be used in UI.
        TInt dispNameLength = 1;
        if ( firstname.Count() ) dispNameLength += firstname[0]->Length();
        if ( lastname.Count() ) dispNameLength += lastname[0]->Length();
        HBufC* displayname = HBufC::NewLC( dispNameLength );
        
        if ( firstname.Count() )
            {
            displayname->Des().Copy( *firstname[0] );
            displayname->Des().Append( KSpace );
            }
        if ( lastname.Count() )
            {
            displayname->Des().Append( *lastname[0] );
            }
        
        // retrieve selected email address
        MVPbkStoreContactFieldCollection& fields = aContact->Fields();
        MVPbkBaseContactField* selectedField = fields.RetrieveField( *iCurrentLink );
        HBufC* selectedEmailAddress = MVPbkContactFieldTextData::Cast( 
                        selectedField->FieldData() ).Text().AllocLC();

        //check number of email addreses
        GetContactFieldsL( iEmailFields, emailAddresses, aContact );
        // create contact object
        CFSEmailUiClsItem* item = CFSEmailUiClsItem::NewLC();
        
        if ( emailAddresses.Count() > 1 )
            {
            item->SetMultipleEmails( ETrue );
            }

        item->SetDisplayNameL( displayname->Des() );
        item->SetEmailAddressL( *selectedEmailAddress );
        CleanupStack::Pop( item );
        iContactObjects.Append( item );

        CleanupStack::PopAndDestroy( selectedEmailAddress );
        selectedEmailAddress = NULL;

        CleanupStack::PopAndDestroy( displayname );
        CleanupStack::PopAndDestroy( &emailAddresses );
        CleanupStack::PopAndDestroy( &lastname );
        CleanupStack::PopAndDestroy( &firstname );
        CleanupStack::PopAndDestroy( aContact );
        
        // Get index of Next ContactLink if there's no LinkSet
        // or iCurrenLink index is set to 0
        TInt index = ( iLinksSet && iCurrentLink ? 
            iLinksSet->Find( *iCurrentLink ) + 1 : 0 );

        if ( iLinksSet && index < iLinksSet->Count() )
            {
            delete iCurrentLink;
            iCurrentLink = NULL;

            iCurrentLink = iLinksSet->At(index).CloneLC();
            CleanupStack::Pop();

            delete iLinkOperationFetch;
            iLinkOperationFetch = NULL;

            //Async operation, callback VPbkSingleContactOperationCompleteL
            //Error situations: VPbkSingleContactOperationFailed
            iLinkOperationFetch = iContactManager->RetrieveContactL(
                *iCurrentLink, *this);
            }
        else 
            {
            delete iLinkOperationFetch; 
            iLinkOperationFetch = NULL;

            delete iCurrentLink;
            iCurrentLink = NULL;

            iState = EContactHandlerIdle;
            delete iLinksSet;
            iLinksSet = NULL;

            if ( iHandlerObserver )
                {
                iHandlerObserver->OperationCompleteL( EGetAddressesFromPhonebook, iContactObjects );
                iContactObjects.ResetAndDestroy();
                iHandlerObserver = NULL;
                }
            }
        }
    
    else if ( (iLinkOperationFetch == &aOperation) && (iState == EContactHandlerCallToContactByEmail ) )
    {   
        CleanupDeletePushL( aContact );
    
        RPointerArray<HBufC> phonenumbers;
        CleanupResetAndDestroyClosePushL( phonenumbers );
        GetContactFieldsL( iPhoneNumberFields, phonenumbers, aContact );
        TInt phoneNumbersAvailable = phonenumbers.Count();
        CleanupStack::PopAndDestroy( &phonenumbers );
        
        if ( phoneNumbersAvailable )
            {
            if ( CallQueryL( iLastClsItem->DisplayName() ) )
                {
                MakeAiwCallL( iLastClsItem->ContactLink(), KNullDesC );
                }
            else
                {
// user cancelled operation - inform client to enable its actions (i.e. return search priority)
                ObserverOperationErrorL( EFindAndCallToContactByEmailL, KErrCancel );
                iState = EContactHandlerIdle;
                }
            }
        else 
            {
            if ( IsRemoteLookupSupported() )
                {
                RemoteContactQueryL();
                }
            else
                {
                TFsEmailUiUtility::ShowErrorNoteL( R_FREESTYLE_EMAIL_UI_VIEWER_NO_PHONE_NUMBER );                   
//no phone number found - inform client to enable its actions (i.e. return search priority)
                ObserverOperationErrorL( EFindAndCallToContactByEmailL, KErrNotFound );
                }
            iState = EContactHandlerIdle;
            }
        
        CleanupStack::PopAndDestroy( aContact );
    }
    // Addition to get contact for message creation.
    else if ( (iLinkOperationFetch == &aOperation) && ( iMsgCreationHelperState == EContactHandlerGetSmsAddressFromPhonebook || 
            iMsgCreationHelperState == EContactHandlerGetMmsAddressFromPhonebook || iMsgCreationHelperState == EContactHandlerGetVoiceMsgAddressFromPhonebook ) )
        {
        if ( iContactForMsgCreation )
            {
            delete iContactForMsgCreation;
            iContactForMsgCreation = NULL;
            }
        // Check that we have a contact for message creatiun
        if ( aContact )
            {
            // State is now idle.
            iState = EContactHandlerIdle;
            // Store contact
            iContactForMsgCreation = aContact;
            // Create clonelink for address selection
            MVPbkContactLink* cloneLink = iCurrentLink->CloneLC();
            CleanupStack::Pop();
            switch ( iMsgCreationHelperState )
                {
                case EContactHandlerGetSmsAddressFromPhonebook:
                    // Select SMS address, sending is done when callback completes
                    GetSmsAddressFromPhonebookAndSendL( cloneLink );
                    break;
                case EContactHandlerGetMmsAddressFromPhonebook:
                    // Select MMS address, sending is done when callback completes
                    GetMmsAddressFromPhonebookAndSendL( cloneLink, EFalse );    
                    break;
                case EContactHandlerGetVoiceMsgAddressFromPhonebook:
                    // Select Voice MSG address, sending is done when callback completes
                    GetMmsAddressFromPhonebookAndSendL( cloneLink, ETrue ); 
                    break;
                default:
                    iState = EContactHandlerIdle;
                    iMsgCreationHelperState = EContactHandlerIdle;
                    break;
                }       
            }   
        else
            {
            iMsgCreationHelperState = EContactHandlerIdle;
            iState = EContactHandlerIdle;
            }
        }
    
    }

void CFSEmailUiContactHandler::VPbkSingleContactOperationFailed(
        MVPbkContactOperationBase& aOperation, TInt aError )
    {
    FUNC_LOG;
    TRAP_IGNORE( VPbkSingleContactOperationFailedL( aOperation, aError ) );
    }

void CFSEmailUiContactHandler::VPbkSingleContactOperationFailedL(
        MVPbkContactOperationBase& /*aOperation*/, TInt aError )
    {
    FUNC_LOG;
    iState = EContactHandlerIdle;
    if ( iHandlerObserver )
        {
        iHandlerObserver->OperationErrorL( EFindAndCallToContactByEmailL, aError );
        iHandlerObserver = NULL;
        }
    }


void CFSEmailUiContactHandler::OpenComplete()
    {
    FUNC_LOG;
    if ( iStoreReady )
        {
        iOpenComplete = ETrue;
        }
    }

void CFSEmailUiContactHandler::StoreReady(MVPbkContactStore& /*aContactStore*/)
    {
    FUNC_LOG;
    iStoreReady = ETrue;
    }


void CFSEmailUiContactHandler::StoreUnavailable(MVPbkContactStore& /*aContactStore*/, TInt /*aReason*/)
    {
    FUNC_LOG;
    iStoreReady = EFalse;
    }


void CFSEmailUiContactHandler::HandleStoreEventL( MVPbkContactStore& /*aContactStore*/, 
                                                  TVPbkContactStoreEvent /*aStoreEvent*/)
    {
    FUNC_LOG;
    
    }

// -----------------------------------------------------------------------------
// CFSEmailUiContactHandler::HandleNotifyL
// -----------------------------------------------------------------------------
TInt CFSEmailUiContactHandler::HandleNotifyL( TInt aCmdId, TInt aEventId,
                    CAiwGenericParamList& aEventParamList,
                    const CAiwGenericParamList& /*aInParamList*/)
    {
    FUNC_LOG;
    TInt result = 0;
    if ( (aCmdId == KAiwCmdAssign) && (iState == EContactHandlerAddToContacts) &&
         (aEventId == KAiwEventCompleted) )
        {
        // Add to contacts function is completed
        iState = EContactHandlerIdle;
        if ( iHandlerObserver )
            {
            RPointerArray<CFSEmailUiClsItem> emptyItems;
            iHandlerObserver->OperationCompleteL( EAddToContactL, emptyItems );
            emptyItems.Close();
            iHandlerObserver = NULL;
            }
        
        }
    else if ( (aCmdId == KAiwCmdSelect) && (iState == EContactHandlerGetAddressesFromPhonebook) &&
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
                delete iCurrentLink;
                iCurrentLink = NULL;

                iCurrentLink = iLinksSet->At(0).CloneLC();
                CleanupStack::Pop();
                //Async operation, callback VPbkSingleContactOperationCompleteL
                //Error situations: VPbkSingleContactOperationFailed
                iLinkOperationFetch = iContactManager->RetrieveContactL( 
                    iLinksSet->At(0), *this );
                }

            iContactManager->ContactStoresL().OpenAllL( *this );
            }

        }
    else if ( (aCmdId == KAiwCmdSelect) && (iState == EContactHandlerGetSmsAddressFromPhonebook) &&
              (aEventId == KAiwEventCompleted))
        {
        iState = EContactHandlerIdle;
        SendMessageL( aEventParamList,  EContactHandlerGetSmsAddressFromPhonebook );
        }
    else if ( (aCmdId == KAiwCmdSelect) && (iState == EContactHandlerGetMmsAddressFromPhonebook) &&
              (aEventId == KAiwEventCompleted))
        {
        iState = EContactHandlerIdle;
        // Send SMS to contact address
        SendMessageL( aEventParamList,  EContactHandlerGetMmsAddressFromPhonebook );
        }
    else if ( (aCmdId == KAiwCmdSelect) && (iState == EContactHandlerGetVoiceMsgAddressFromPhonebook) &&
              (aEventId == KAiwEventCompleted))
        {
        iState = EContactHandlerIdle;
        // Send Voice MSG to contact address
        SendMessageL( aEventParamList,  EContactHandlerGetVoiceMsgAddressFromPhonebook );
        }
    
    //Notify caller of the error
    else if ( aEventId == KAiwEventError )
        {
        iState = EContactHandlerIdle;
        if ( iHandlerObserver )
            {
            iHandlerObserver->OperationErrorL( CurrentCommand(), KErrGeneral );
            iHandlerObserver = NULL;
            }
        }

    // We don't need to check outParams, or do anything special during the exit
    else if ( (aEventId == KAiwEventQueryExit) )
        {
        result = ETrue;
        }
    
    else if ( aEventId == KAiwEventOutParamCheck )
        {
        result = 1;
        }
    else if ( aEventId == KAiwEventCanceled )
        {
        iState = EContactHandlerIdle;
        if ( iHandlerObserver )
            {
            RPointerArray<CFSEmailUiClsItem> emptyItems;
            iHandlerObserver->OperationCompleteL( CurrentCommand(), emptyItems );
            emptyItems.Close();
            iHandlerObserver = NULL;
            }
        }

    return result;
    }


CFSEmailUiContactHandler::CFSEmailUiContactHandler( RFs& aSession ): 
    iState(EContactHandlerIdle),
    iHandlerObserver(NULL),
    iOpenComplete(EFalse),
    iStoreReady(EFalse),
    iUseSenderText(EFalse),
    iFs( aSession ),
    iCachingInProgressError(EFalse),
    // <cmail> video call
    iVideoCall( EFalse )
    // </cmail>
    {
    FUNC_LOG;
    }

void CFSEmailUiContactHandler::ConstructL( RFs& aSession )
    {
    FUNC_LOG;
    CVPbkContactStoreUriArray* uriArray = CVPbkContactStoreUriArray::NewL();    
    CleanupStack::PushL( uriArray );
    TVPbkContactStoreUriPtr phoneMemoryUri( VPbkContactStoreUris::DefaultCntDbUri() );  
    uriArray->AppendL( phoneMemoryUri );       //default contact database store uri
    iContactManager = CVPbkContactManager::NewL( *uriArray );

    CleanupStack::PopAndDestroy( uriArray );

    // Open all the contact stores
    iContactManager->ContactStoresL().OpenAllL( *this );
    
    iClsListHandler = CFSEmailUiClsListsHandler::NewL( aSession, iContactManager );
    iClsListHandler->SetObserver( this );
    iServiceHandler = CAiwServiceHandler::NewL();
    
    iServiceHandler->AttachL( R_FSE_CONTACT_SELECTION_INTEREST );
    iServiceHandler->AttachL( R_FSE_CONTACT_ASSIGNMENT_INTEREST );
    iServiceHandler->AttachL( R_FSE_CALL_INTEREST );

    FormatFieldIds();
    }


void CFSEmailUiContactHandler::HandleCallL(
    const RPointerArray<CFSEmailUiClsItem>& aMatchingItems )
    {
    FUNC_LOG;
   
    if ( aMatchingItems.Count() == 0 )
        {
        if ( IsRemoteLookupSupported() )
            {
            RemoteContactQueryL();
            }
        else
            {
            TFsEmailUiUtility::ShowErrorNoteL( R_FREESTYLE_EMAIL_UI_VIEWER_NO_PHONE_NUMBER );                   
            }
        // <cmail> video call flag needs to be cleared
        iVideoCall = EFalse;
        // </cmail>
        iState = EContactHandlerIdle;
        }               
    else if ( aMatchingItems.Count() > 0 ) // Call to one directly
        {
        // Create contact item in which to copy number or address, async operation.
        delete iCurrentLink;
        iCurrentLink = NULL;

        iCurrentLink = aMatchingItems[0]->ContactLink()->CloneLC();
        CleanupStack::Pop();
        if ( iLinkOperationFetch )
            {
            delete iLinkOperationFetch;
            iLinkOperationFetch = NULL;
            }

        //Async operation, callback VPbkSingleContactOperationCompleteL
        //Error situations: VPbkSingleContactOperationFailed
        iLinkOperationFetch = iContactManager->RetrieveContactL(
            *iCurrentLink, *this);
        }
    }

void CFSEmailUiContactHandler::CreateMessageL( const RPointerArray<CFSEmailUiClsItem>& aMatchingItems )
    {
    FUNC_LOG;
    iState = EContactHandlerIdle;
    
    if ( aMatchingItems.Count() == 0 )
        {
        // <cmail>
        /*if ( IsRemoteLookupSupported() )
            {
            TInt answer = TFsEmailUiUtility::ShowConfirmationQueryL( R_FREESTYLE_EMAIL_FETCH_FROM_REMOTE_QUERY );
            if ( answer )
                {
                LaunchRemoteLookupWithQueryL( *iMailBox, *iPreviousEmailAddress );
                }
            }
        else
            {
            // No email address found and no RCL available, send MMS to email addres
            // using utilities class. MMS Service is validated in utilities class.
            TFsEmailUiUtility::ShowCreateMessageQueryL( *iPreviousEmailAddress, ETrue );
            }*/
        // phone number not found for email address in contacts - 
        // no Text message available - subset of Multimedia and Voice message is displayed
        TFsEmailUiUtility::ShowCreateMessageQueryL( *iPreviousEmailAddress, ETrue );
        // </cmail>
        iState = EContactHandlerIdle;
        }
    else if ( aMatchingItems.Count() > 0 ) 
        {
        CSendUi* sendUi = CSendUi::NewLC();

        CArrayFix<TUid>* showedServicesUidArray = new( ELeave ) CArrayFixFlat<TUid>(4);
        CleanupStack::PushL( showedServicesUidArray );

        CDesCArrayFlat* array = new ( ELeave ) CDesCArrayFlat( 2 );
        CleanupStack::PushL( array );

        TSendingCapabilities noCapabilities(0,0,0);
        
        if( sendUi->ValidateServiceL( KSenduiMtmSmsUid, noCapabilities ) )
            {
            HBufC* textMessage = StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_VIEWER_TEXT_MESSAGE );
            array->AppendL( *textMessage );
            CleanupStack::PopAndDestroy( textMessage );             
            showedServicesUidArray->AppendL( KSenduiMtmSmsUid );
            }
        if( sendUi->ValidateServiceL( KSenduiMtmMmsUid, noCapabilities ) )
            {
            HBufC* multimediaMessage = StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_VIEWER_MULTIMEDIA_MESSAGE );
            array->AppendL( *multimediaMessage );
            CleanupStack::PopAndDestroy( multimediaMessage );               
            showedServicesUidArray->AppendL( KSenduiMtmMmsUid );
            }
        if( sendUi->ValidateServiceL( KSenduiMtmAudioMessageUid, noCapabilities ) )
            {
            HBufC* voiceMessage = StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_VIEWER_VOICE_MESSAGE );
            array->AppendL( *voiceMessage );
            CleanupStack::PopAndDestroy( voiceMessage );                
            showedServicesUidArray->AppendL( KSenduiMtmAudioMessageUid );
            }

        TInt selectedOption;
        CAknListQueryDialog* dlg = new ( ELeave ) CAknListQueryDialog( &selectedOption );
        dlg->PrepareLC( R_MAIL_VIEWER_CREATE_MESSAGE_DIALOG  );
                    
        dlg->SetItemTextArray( array );
        dlg->SetOwnershipType( ELbmDoesNotOwnItemArray );   
        TInt ret = dlg->RunLD();
        CleanupStack::PopAndDestroy( array );
        CleanupStack::PopAndDestroy( showedServicesUidArray );
        CleanupStack::PopAndDestroy( sendUi );      
    
        // Continue sending process if query is not cancelled
        if( ret )
            {
            switch ( selectedOption )
                {
                case 0:
                    // Select SMS address, sending is done when callback completes
                    iMsgCreationHelperState = EContactHandlerGetSmsAddressFromPhonebook;    
                    break;
                case 1:
                    // Select MMS address, sending is done when callback completes
                    iMsgCreationHelperState = EContactHandlerGetMmsAddressFromPhonebook;    
                    break;
                case 2:
                    // Select Voice MSG address, sending is done when callback completes
                    iMsgCreationHelperState = EContactHandlerGetVoiceMsgAddressFromPhonebook;   
                    break;
                default:
                    iMsgCreationHelperState = EContactHandlerIdle;
                    break;
                }
            // Create contact item in which to copy number or address, async operation.
            delete iCurrentLink;
            iCurrentLink = NULL;
            
            iCurrentLink = aMatchingItems[0]->ContactLink()->CloneLC();
            CleanupStack::Pop();
            if ( iLinkOperationFetch )
                {
                delete iLinkOperationFetch;
                iLinkOperationFetch = NULL;
                }
            
            //Async operation, callback VPbkSingleContactOperationCompleteL
            //Error situations: VPbkSingleContactOperationFailed
            iLinkOperationFetch = iContactManager->RetrieveContactL(
                *iCurrentLink, *this);
            }
        }
    }

// -----------------------------------------------------------------------------
// CFSEmailUiContactHandler::SendMessageL
// -----------------------------------------------------------------------------
void CFSEmailUiContactHandler::SendMessageL( CAiwGenericParamList& aEventParamList, TInt aServiceType )
    {
    FUNC_LOG;
    TInt paramIndex(0);
    iState = EContactHandlerIdle;

    const TAiwGenericParam* param = 
            aEventParamList.FindFirst( paramIndex, EGenericParamContactLinkArray );
    if ( param )
        {
        TPtrC8 packedLinks = param->Value().AsData();
        if ( packedLinks.Length() > 0 )
            {      
            CSendUi* sendUi = CSendUi::NewLC();
            CMessageData* messageData = CMessageData::NewLC();       
            MVPbkContactLinkArray* linkArray = GetContactManager()->CreateLinksLC(packedLinks);
            MVPbkStoreContactField* field = iContactForMsgCreation->Fields().RetrieveField(linkArray->At(0));
            MVPbkContactFieldData& data = field->FieldData();
            if (data.DataType() == EVPbkFieldStorageTypeText)
                {
                const TDesC& dataText = MVPbkContactFieldTextData::Cast(data).Text();           
                // Get contact alias to message data if there is name data
                //RFs& rFs = CEikonEnv::Static()->FsSession();
                //User::LeaveIfError(rFs.Connect());    
                CPbk2SortOrderManager* sortOrderManager = 
                        CPbk2SortOrderManager::NewL(iContactManager->FieldTypes(), &iFs);
                MPbk2ContactNameFormatter* nameFormatter = 
                        Pbk2ContactNameFormatterFactory::CreateL(iContactManager->FieldTypes(), *sortOrderManager, &iFs);            
                HBufC* nameBuffer = nameFormatter->GetContactTitleL( iContactForMsgCreation->Fields(), 0);
                
                delete sortOrderManager;
                sortOrderManager = NULL;
                delete nameFormatter;     
                nameFormatter = NULL;
                
                //rFs.Close();
                if ( nameBuffer )
                    {
                    CleanupStack::PushL( nameBuffer );
                    messageData->AppendToAddressL( dataText, *nameBuffer );
                    CleanupStack::PopAndDestroy( nameBuffer );
                    }
                else
                    {
                    messageData->AppendToAddressL( dataText );  
                    }   
                // Send selected message                            
                TSendingCapabilities noCapabilities(0,0,0);
                switch ( aServiceType )
                    {               
                    case EContactHandlerGetSmsAddressFromPhonebook:
                        {
                        if( sendUi->ValidateServiceL( KSenduiMtmSmsUid, noCapabilities ) )
                            {
                            sendUi->CreateAndSendMessageL( KSenduiMtmSmsUid, messageData, KNullUid, EFalse );                           
                            }
                        }                   
                        break;
                    case EContactHandlerGetMmsAddressFromPhonebook:
                        {
                        if( sendUi->ValidateServiceL( KSenduiMtmMmsUid, noCapabilities ) )
                            {
                            sendUi->CreateAndSendMessageL( KSenduiMtmMmsUid, messageData, KNullUid, EFalse );                           
                            }
                        }
                        break;
                    case EContactHandlerGetVoiceMsgAddressFromPhonebook:
                        {
                        if( sendUi->ValidateServiceL( KSenduiMtmAudioMessageUid, noCapabilities ) )
                            {
                            sendUi->CreateAndSendMessageL( KSenduiMtmAudioMessageUid, messageData, KNullUid, EFalse );                          
                            }
                        }
                        break;
                    default:
                        break;          
                    }
                }   
            CleanupStack::PopAndDestroy( 3 ); // Sendui, messageData and linkArray.
            }
        }
    }
// ---------------------------------------------------------------------------
// CFSEmailUiContactHandler::IsRemoteLookupSupported
// ---------------------------------------------------------------------------
//
TBool CFSEmailUiContactHandler::IsRemoteLookupSupported()
    {
    FUNC_LOG;
    TBool supported = iMailBox &&
        iMailBox->HasCapability( EFSMBoxCapaSupportsRCL );
    return supported;
    }


void CFSEmailUiContactHandler::GetContactFieldsL( RArray<TInt>& aFieldIds,
    RPointerArray<HBufC>& aNumbers, MVPbkStoreContact* aContact)
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

// ---------------------------------------------------------------------------
// Activates phonebook view to show details of give contact
// ---------------------------------------------------------------------------
//
void CFSEmailUiContactHandler::ShowDetailsL(
    const RPointerArray<CFSEmailUiClsItem>& aMatchingItems )
    {
    FUNC_LOG;
    if ( aMatchingItems.Count() &&
        aMatchingItems[0]->ContactLink() )
        {
        MVPbkContactLink* contact = aMatchingItems[0]->ContactLink();
        HBufC8* packed = contact->PackLC();

        HBufC16* link16 = HBufC16::NewLC(packed->Length());
        link16->Des().Copy(packed->Des());

        MCCAParameter*  param = TCCAFactory::NewParameterL();
        CleanupClosePushL( *param );
        
        delete iConnection;
        iConnection = NULL;
        iConnection = TCCAFactory::NewConnectionL();

        // Setting up the contact link parameter, ownership transfers
        param->SetConnectionFlag(MCCAParameter::ENormal);
        param->SetContactDataFlag(MCCAParameter::EContactLink);
        param->SetContactDataL( link16->Des() );

        // switch to details view
        const TUid uid = TUid::Uid( 0x200159E7 ); 
        param->SetLaunchedViewUid( uid);

        // Launching the  CCA application
        iConnection->LaunchAppL( *param, this );

        CleanupStack::Pop( param );  // parameter is taken care by MCCAConnection
        CleanupStack::PopAndDestroy( link16 );
        CleanupStack::PopAndDestroy( packed );
        }
    else
        {
        TFsEmailUiUtility::ShowErrorNoteL(
            R_FREESTYLE_EMAIL_UI_VIEWER_NO_EMAIL_ADDRESS, EFalse );
        }

    iState = EContactHandlerIdle;
    }

// ---------------------------------------------------------------------------
// CFSEmailUiContactHandler::FindContactLinkL
// Searches contacts matching with the given email address
// ---------------------------------------------------------------------------
//
void CFSEmailUiContactHandler::FindContactLinkL( const TDesC& aDes )
    {
    FUNC_LOG;

    // Replace email address to match with the new search
    SetPreviousEmailAddressL( aDes );

    // Get contact links matching the given address
    iClsListHandler->SetCurrentMailboxL( NULL );
    iClsListHandler->SearchMatchesL( aDes );
    }

void CFSEmailUiContactHandler::FormatFieldIds()
    {
    FUNC_LOG;
    iEmailFields.Append( R_VPBK_FIELD_TYPE_EMAILGEN );
    iEmailFields.Append( R_VPBK_FIELD_TYPE_EMAILWORK );
    iEmailFields.Append( R_VPBK_FIELD_TYPE_EMAILHOME );
    
    iFirstnameFields.Append( R_VPBK_FIELD_TYPE_FIRSTNAME );
    iLastnameFields.Append( R_VPBK_FIELD_TYPE_LASTNAME );
    
    iPhoneNumberFields.Append( R_VPBK_FIELD_TYPE_LANDPHONEHOME );
    iPhoneNumberFields.Append( R_VPBK_FIELD_TYPE_MOBILEPHONEWORK );
    iPhoneNumberFields.Append( R_VPBK_FIELD_TYPE_MOBILEPHONEHOME );
    iPhoneNumberFields.Append( R_VPBK_FIELD_TYPE_LANDPHONEWORK );
    iPhoneNumberFields.Append( R_VPBK_FIELD_TYPE_LANDPHONEGEN );
    iPhoneNumberFields.Append( R_VPBK_FIELD_TYPE_MOBILEPHONEGEN );
    
    }

void CFSEmailUiContactHandler::ResetFieldIds()
    {
    FUNC_LOG;
    iLastnameFields.Reset();
    iFirstnameFields.Reset();
    iEmailFields.Reset();
    iPhoneNumberFields.Reset();
    }


// -----------------------------------------------------------------------------
// CFSEmailUiContactHandler::LaunchRemoteLookupL
// -----------------------------------------------------------------------------
TBool CFSEmailUiContactHandler::GetNameAndEmailFromRemoteLookupL( CFSMailBox& aMailBox,
    const TDesC& aQueryString, TDes& aDisplayname, TDes& aEmailAddress )
    {
    FUNC_LOG;

    CPbkxRemoteContactLookupServiceUiContext::TResult result;
    DoRemoteLookupL( aMailBox, aQueryString, result, 
                     CPbkxRemoteContactLookupServiceUiContext::EModeContactSelector);

    if ( result.iExitReason == 
         CPbkxRemoteContactLookupServiceUiContext::TResult::EExitContactSelected )
        {
        GetNameAndEmail( aDisplayname, aEmailAddress, *(result.iSelectedContactItem) );
        return ETrue;
        } 

    return EFalse;
    }

// -----------------------------------------------------------------------------
// CFSEmailUiContactHandler::LaunchRemoteLookupL
// -----------------------------------------------------------------------------
void CFSEmailUiContactHandler::LaunchRemoteLookupL( CFSMailBox& aMailBox )
    {
    FUNC_LOG;
    
    CPbkxRemoteContactLookupServiceUiContext::TResult result;
    DoRemoteLookupL( aMailBox, KNullDesC , result, 
                     CPbkxRemoteContactLookupServiceUiContext::EModeNormal );

    }

// -----------------------------------------------------------------------------
// CFSEmailUiContactHandler::LaunchRemoteLookupL
// -----------------------------------------------------------------------------
HBufC* CFSEmailUiContactHandler::GetNameAndNumberFromRemoteLookupL(
    CFSMailBox& aMailBox, const TDesC& aQuery, RBuf& aPhoneNumber )
    {
    FUNC_LOG;

    CPbkxRemoteContactLookupServiceUiContext::TResult result;
    DoRemoteLookupL( aMailBox, aQuery , result, 
        CPbkxRemoteContactLookupServiceUiContext::EModeContactSelector );

    HBufC* displayName = NULL;
    if ( result.iExitReason == 
         CPbkxRemoteContactLookupServiceUiContext::TResult::EExitContactSelected )
        {
        displayName = GetPhoneNumberAndNameL( aPhoneNumber, *(result.iSelectedContactItem) );
        } 
    return displayName;
    }

void CFSEmailUiContactHandler::LaunchRemoteLookupWithQueryL(
    CFSMailBox& aMailBox, const TDesC& aQuery )
    {
    FUNC_LOG;
    CPbkxRemoteContactLookupServiceUiContext::TResult result;
    DoRemoteLookupL( aMailBox, aQuery , result, 
        CPbkxRemoteContactLookupServiceUiContext::EModeExistingCriteria );
    }

HBufC* CFSEmailUiContactHandler::GetLastSearchNameL( const TDesC& aEmailAddress )
    {
    FUNC_LOG;
    HBufC* name(NULL);
    if ( iSearchMatch )
        {
        if ( !iSearchMatch->EmailAddress().CompareF( aEmailAddress ) )
            {
            if ( iSearchMatch->DisplayName() != KNullDesC )
                {
                name = iSearchMatch->DisplayName().AllocL();
                }
            }
        }
    return name;
    }

void CFSEmailUiContactHandler::Reset()
    {
    FUNC_LOG;
    iContactObjects.ResetAndDestroy();
    iState = EContactHandlerIdle;
    delete iLinksSet;
    iLinksSet = NULL;
    delete iLinkOperation;
    iLinkOperation = NULL;
    delete iLinkOperationFetch;
    iLinkOperationFetch = NULL;
    iHandlerObserver = NULL;
    }


// ---------------------------------------------------------------------------
// Gets the name and email address from given contact item
// ---------------------------------------------------------------------------
//
void CFSEmailUiContactHandler::GetNameAndEmail( TDes& aName, TDes& aEmail,
    CContactItem& aItem ) const
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

// ---------------------------------------------------------------------------
// Gets the phone number from given contact item
// ---------------------------------------------------------------------------
//
HBufC* CFSEmailUiContactHandler::GetPhoneNumberAndNameL( RBuf& aPhoneNumber, CContactItem& aItem ) const
    {
    FUNC_LOG;
    TInt pos;
    CContactItemFieldSet& fieldSet = aItem.CardFields();

    pos = fieldSet.Find(KUidContactFieldPhoneNumber);
    if (pos >= 0)
        {
        CContactItemField& itemField=fieldSet[pos];
        if (!(itemField.IsHidden()) && !(itemField.IsDisabled()))
            {
            CContactTextField* textField = itemField.TextStorage();
            //aPhoneNumber.Create( textField->Text()->Length() );
            //aPhoneNumber.Copy( textField->Text() );
            aPhoneNumber.Create( textField->Text() );
            }               
        }
    
    HBufC* firstname = NULL;
    
    pos = fieldSet.Find( KUidContactFieldGivenName );
    if ( pos >= 0 )
        {
        CContactItemField& itemField=fieldSet[pos];
        if ( !itemField.IsHidden() && !itemField.IsDisabled() )
            {
            CContactTextField* textField = itemField.TextStorage();
            firstname = textField->Text().AllocLC();
            }               
        }
    if ( !firstname )
        {
        firstname = KNullDesC().AllocLC();
        }

    HBufC* lastname = NULL;
    
    pos = fieldSet.Find( KUidContactFieldFamilyName );
    if ( pos >= 0 )
        {
        CContactItemField& itemField=fieldSet[pos];
        if ( !itemField.IsHidden() && !itemField.IsDisabled() )
            {
            CContactTextField* textField = itemField.TextStorage();
            lastname = textField->Text().AllocLC();
            }               
        }
    if ( !lastname )
        {
        lastname = KNullDesC().AllocLC();
        }
    
    HBufC* name = TFsEmailUiUtility::CreateDisplayNameLC( *firstname, *lastname, KNullDesC );
    CleanupStack::Pop( name );
    CleanupStack::PopAndDestroy( lastname );
    CleanupStack::PopAndDestroy( firstname );
    
    return name;
    }

// ---------------------------------------------------------------------------
// CFSEmailUiContactHandler::DoRemoteLookupL
// ---------------------------------------------------------------------------
//
void CFSEmailUiContactHandler::DoRemoteLookupL( CFSMailBox& aMailBox,
    const TDesC& aQueryString,
    CPbkxRemoteContactLookupServiceUiContext::TResult& aResult,
    CPbkxRemoteContactLookupServiceUiContext::TMode aLookupMode )
    {
    FUNC_LOG;
    TUid protocolUid = TUid::Null();
    TUint accountUid = 0;
    aMailBox.GetRCLInfo( protocolUid, accountUid );
    const TPbkxRemoteContactLookupProtocolAccountId accountId =
        TPbkxRemoteContactLookupProtocolAccountId( protocolUid, accountUid );

    CPbkxRemoteContactLookupServiceUiContext::TContextParams params = 
        { accountId, aLookupMode }; 

    CPbkxRemoteContactLookupServiceUiContext* context =
        Pbk2RemoteContactLookupFactory::NewContextL( params );
    CleanupStack::PushL( context );

    if ( !iEnv )
        {
        iEnv = CEikonEnv::Static();
        }
    
    CFreestyleEmailUiAppUi* fsAppUi =
            static_cast<CFreestyleEmailUiAppUi*>( iEnv->AppUi() );
    
    TBool indicatorsWereOn = EFalse;
    CCustomStatuspaneIndicators* indicators = fsAppUi->GetStatusPaneIndicatorContainer();

    if ( indicators->IsVisible() )
        {
        indicatorsWereOn = ETrue;
        // hide message indicators
        indicators->HideStatusPaneIndicators();
        }

    context->ExecuteL( aQueryString, aResult );
    if ( indicatorsWereOn )
        {
        // show message indicators
        indicators->ShowStatusPaneIndicators();
        }

    CleanupStack::PopAndDestroy( context );
    }


// ---------------------------------------------------------------------------
// Displays call query
// ---------------------------------------------------------------------------
//
TBool CFSEmailUiContactHandler::CallQueryL(
    const TDesC& aDisplayName )
    {
    FUNC_LOG;
    TInt answer = 0;
    
    if ( aDisplayName.Length() )
        {
        if ( iUseSenderText )
            {
            answer = TFsEmailUiUtility::ShowConfirmationQueryL( R_FREESTYLE_EMAIL_CALL_SENDER_QUERY, 
                                                            aDisplayName );
            }
        else
            {
            answer = TFsEmailUiUtility::ShowConfirmationQueryL( R_FREESTYLE_EMAIL_UI_VIEWER_CALL_HOTSPOT, 
                                                                aDisplayName );
            }
        }
    else // Show basic note if display name is not available.
        {
        _LIT(KEmpty, "");
        // Create dialog
        CFSEmailUiCallDialog* queryNote = new ( ELeave ) CFSEmailUiCallDialog();
        CleanupStack::PushL( queryNote ); 
        // Createa and set text
        HBufC* querytext = StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_VIEWER_CALL_HOTSPOT, KEmpty );       
        TPtr pointer = querytext->Des();
        _LIT(KLineBreak, "\n");
        AknTextUtils::StripCharacters( pointer, KLineBreak );
        queryNote->SetPromptL( *querytext );       
        CleanupStack::PopAndDestroy( querytext );
        CleanupStack::Pop( queryNote );
        // Show note.
        answer = queryNote->ExecuteLD( R_FSEMAIL_QUERY_DIALOG );
        }
    
    return answer;
    }



// ---------------------------------------------------------------------------
// Creates copy of given CLS items and appends them to given array.
// ---------------------------------------------------------------------------
//
void CFSEmailUiContactHandler::CopyItemsL(
    const RPointerArray<CFSEmailUiClsItem>& aItems,
    RPointerArray<CFSEmailUiClsItem>& aArray )
    {
    FUNC_LOG;
    for ( TInt ii = 0; ii < aItems.Count(); ++ii )
        {
        CFSEmailUiClsItem* clone = aItems[ii]->CloneLC();
        aArray.AppendL( clone );
        CleanupStack::Pop( clone );
        }
    }

// ---------------------------------------------------------------------------
// Sets the address used for previous contact search
// ---------------------------------------------------------------------------
//
void CFSEmailUiContactHandler::SetPreviousEmailAddressL( const TDesC& aAddress )
    {
    FUNC_LOG;
    delete iPreviousEmailAddress;
    iPreviousEmailAddress = NULL;
    iPreviousEmailAddress = aAddress.AllocL();
    }

void CFSEmailUiContactHandler::RemoteContactQueryL()
    {
    FUNC_LOG;
    TInt answer = TFsEmailUiUtility::ShowConfirmationQueryL( R_FREESTYLE_EMAIL_FETCH_FROM_REMOTE_QUERY );
    if ( answer )   
        {
        LaunchRemoteLookupWithQueryL( *iMailBox, *iPreviousEmailAddress );
        }
    else // user cancelled operation inform client to enable its actions (i.e. return search priority)
        {
        ObserverOperationErrorL( CurrentCommand(), KErrCancel );
        }

    }

TContactHandlerCmd CFSEmailUiContactHandler::CurrentCommand()
    {
    FUNC_LOG;
    switch (iState)
        {
        case EContactHandlerAddToContacts:
            return EAddToContactL;
        case EContactHandlerSearchMatches:
            return ESearchContacts;
        case EContactHandlerShowContactDetails:
            return EShowContactDetailsL;
        case EContactHandlerGetAddressesFromPhonebook:
            return EGetAddressesFromPhonebook;
        case EContactHandlerCallToContactByEmail:
            return ECallToContact;
        case EContactHandlerCreateMessage:
            return ECreateMessage;
        default:
            break;
        }
    return EIdle;
    }

void CFSEmailUiContactHandler::ClearObservers()
    {
    iHandlerObserver = NULL;
    }

// <cmail> call observer's MFSEmailUiContactHandlerObserver::OperationErrorL( TContactHandlerCmd aCmd, TInt aError ) 
void CFSEmailUiContactHandler::ObserverOperationErrorL( TContactHandlerCmd aCmd, TInt aErrorCode )
    {
    if( ( iHandlerObserver ) && ( KErrNone != aErrorCode ) )
        {
        iHandlerObserver->OperationErrorL( aCmd, aErrorCode ); 
        iHandlerObserver = NULL;
        }
    }

void CFSEmailUiContactHandler::CCASimpleNotifyL( TNotifyType aType, TInt /*aReason*/ )
    {
    if ( MCCAObserver::EExitEvent == aType )
        {
        // Calling Close() for iConnection will close the running 
        // CCApplication, so be careful when using it
        if ( iConnection )
            {
            iConnection->Close();
            iConnection = NULL;
            }
        }
    }
