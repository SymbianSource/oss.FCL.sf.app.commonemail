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
* Description:  Data Store Adapter for MRU list
 *
*/




#include <CPsData.h>
#include <CPcsDefs.h>
#include <coemain.h>
#include <data_caging_path_literals.hrh>
#include <VPbkEng.rsg>
//<cmail>
#include "cfsmailclient.h"
#include "CPsMruAdapter.h"
//</cmail>
#include <mmf/common/mmfcontrollerpluginresolver.h> // CleanupResetAndDestroy


#define PRINT(x)

#include "emailtrace.h"

// ============================== MEMBER FUNCTIONS ============================

// Identifier for mailbox Uri
_LIT(KDefaultMailBoxURI, "mailbox://");
// Separator for plugin/mailbox identifiers inside the Uri string
const TText8 KDefaultMailBoxURISeparator = '/';
// Maximum length of uri
const TInt KMaximumMailboxUriLength = 64;

// ----------------------------------------------------------------------------
// CPsMruAdapter::NewL
// Two Phase Construction
// ----------------------------------------------------------------------------
CPsMruAdapter* CPsMruAdapter::NewL( TAny* aPsDataPluginParameters )
    {
    FUNC_LOG;
    PRINT ( _L( "Enter CPsMruAdapter::NewL" ) );
    
    // Get the PsData plugin parametrs
    TPsDataPluginParams* params =
            reinterpret_cast<TPsDataPluginParams*>(aPsDataPluginParameters );
    CPsMruAdapter* self = new ( ELeave ) CPsMruAdapter();
    CleanupStack::PushL(self);
    self->ConstructL( params->iDataStoreObserver, params->iStoreListObserver );
    CleanupStack::Pop( self );
    
    PRINT ( _L( "End CPsMruAdapter::NewL" ) );
    
    return self;
    }

// ----------------------------------------------------------------------------
// CPsMruAdapter::CPsMruAdapter
// Two Phase Construction
// ----------------------------------------------------------------------------
CPsMruAdapter::CPsMruAdapter()
    {
    FUNC_LOG;
    PRINT ( _L( "Enter CPsMruAdapter::CPsMruAdapter" ) );
    iDelayMailboxCreationPtr = NULL;
    PRINT ( _L( "End CPsMruAdapter::CPsMruAdapter" ) );
    }

// ----------------------------------------------------------------------------
// CPsMruAdapter::ConstructL
// Two Phase Construction
// ----------------------------------------------------------------------------
void CPsMruAdapter::ConstructL( MDataStoreObserver* aObserverForDataStore,
        MStoreListObserver* aStoreListObserver )
    {
    FUNC_LOG;   
    PRINT ( _L( "Enter CPsMruAdapter::ConstructL" ) );

    // Mail client for FS Email framework
    iMailClient = CFSMailClient::NewL();

    // Register as observer for mail client, we'll need this to observe MRU list changes
    // When i.e mail is sent
    iMailClient->AddObserverL( *this );

    // This updates the list of all the supported datastores
    UpdateSupportedDataStoresList();

    // Store these for later use
    iDataStoreObserver = aObserverForDataStore;
    iStoreListObserver = aStoreListObserver;

    PRINT ( _L( "End CPsMruAdapter::ConstructL" ) );
    }

// ----------------------------------------------------------------------------
// CPsMruAdapter::~CPsMruAdapter
// Destructor
// ----------------------------------------------------------------------------
CPsMruAdapter::~CPsMruAdapter()
    {
    FUNC_LOG;   
    PRINT ( _L( "Enter CPsMruAdapter::~CPsMruAdapter" ) );

    iSupportedUris.ResetAndDestroy();

    // Remove observer, we don't need it anymore because we're shutting down
    RemoveAllMailboxObservers();
    iObservedMailboxes.Close();

    if ( iMailClient )
        {
        iMailClient->RemoveObserver( *this );
        iMailClient->Close();
        }
    if(iDelayMailboxCreationPtr)
        {
        delete iDelayMailboxCreationPtr;
        iDelayMailboxCreationPtr = NULL;
        }
    
    iDelayedCreatedMailboxes.Reset();
    iDelayedCreatedMailboxes.Close();
    
    PRINT ( _L( "End CPsMruAdapter::~CPsMruAdapter" ) );
    }

// ----------------------------------------------------------------------------
// CPsMruAdapter::RequestForDataL
// 
// ----------------------------------------------------------------------------
void CPsMruAdapter::RequestForDataL( TDesC& aDataStoreURI )
    {
    FUNC_LOG;   
    PRINT ( _L( "Enter CPsMruAdapter::RequestForDataL" ) );

    // Add data to datastore, fill only requested one
    FillDataStoreL( aDataStoreURI );

    PRINT ( _L( "Enter CPsMruAdapter::RequestForDataL" ) );
    }

// ----------------------------------------------------------------------------
// CPsMruAdapter::GetSupportedDataStoresL
// 
// ----------------------------------------------------------------------------

void CPsMruAdapter::GetSupportedDataStoresL(
        RPointerArray<TDesC> &aDataStoresURIs )
    {
    FUNC_LOG;   
    for ( TInt i = 0; i < iSupportedUris.Count(); i++ )
        {
        aDataStoresURIs.Append( iSupportedUris[i] );
        }
    }

// ----------------------------------------------------------------------------
// CPsMruAdapter::RequestForDataExtensionL
// 
// ----------------------------------------------------------------------------
TAny* CPsMruAdapter::RequestForDataExtensionL(TInt /*aItemId*/)
    {
    FUNC_LOG;   
    // No extention required for this since we have and interger as itemId
    // Simply return NULL
    return NULL;
    }

// ----------------------------------------------------------------------------
// CPsMruAdapter::UpdateSupportedDataStoresList
// 
// ----------------------------------------------------------------------------
void CPsMruAdapter::UpdateSupportedDataStoresList()
    {
    FUNC_LOG;   
    // Mailboxes will be fetched to this array
    RPointerArray<CFSMailBox> mailBoxes;

    // List all mailboxes
    TFSMailMsgId plugin;
    iMailClient->ListMailBoxes( plugin, mailBoxes );

    iSupportedUris.ResetAndDestroy();

    // Add all mailboxes as data stores
    for ( TInt i = 0; i < mailBoxes.Count(); i++ )
        {
        // Get id of mailbox
        TFSMailMsgId id = mailBoxes[i]->GetId();

        // Convert it to string, this will be the unique identifier for this mailbox
        HBufC* identifier = HBufC::New( KMaximumMailboxUriLength );
        if ( identifier && GetUriFromMailboxIdentifier( id, *identifier ) )
            {
            // Add to supported Uris list
            if ( iSupportedUris.Append( identifier ) == KErrNone )
                {
                // Ownership of the string is successfully moved to the array
                identifier = NULL;
                }
            }
        delete identifier;
        }

    // Release allocated memory
    mailBoxes.ResetAndDestroy();
    }

// ----------------------------------------------------------------------------
// CPsMruAdapter::FillDataStoreL
// ----------------------------------------------------------------------------
TBool CPsMruAdapter::FillDataStoreL( TDesC& aDataStoreURI )
    {
    FUNC_LOG;   
    TBool result = EFalse;

    TFSMailMsgId dataStoreId;

    if ( GetMailboxIdentifierFromUri( aDataStoreURI, dataStoreId ) )
        {
        result = FillDataStoreL( dataStoreId, aDataStoreURI );
        }

    return result;
    }

TBool CPsMruAdapter::FillDataStoreL( TFSMailMsgId& aId )
    {
    FUNC_LOG;   
    TBool result = EFalse;

    // Create Uri for this mailbox
    HBufC* identifier = HBufC::NewLC( KMaximumMailboxUriLength );
    if ( GetUriFromMailboxIdentifier( aId, *identifier ) )
        {
        result = FillDataStoreL( aId, *identifier );
        }
    CleanupStack::PopAndDestroy( identifier );
    return result;
    }

void CPsMruAdapter::AddMruEmailsL( MDesCArray* aMruList, TDesC& aDataStoreURI )
    {
    FUNC_LOG;   
    TInt entryIndex = 0;
    // Add all data to data store, the format is:
    // index0: displayname
    // index1: email
    // index2: next displayname
    // index3: next email
    // etc..
    for ( int mruIndex = 0; mruIndex < aMruList->MdcaCount(); mruIndex += 2 )
        {
        TPtrC displayName = aMruList->MdcaPoint( mruIndex );
        TPtrC emailAddress = aMruList->MdcaPoint( mruIndex + 1 );

        CPsData* mruData = CPsData::NewL();
        CleanupStack::PushL(mruData);
        mruData->SetId( entryIndex++ );

        // Set the data
        mruData->SetDataL( 0, displayName );
        mruData->SetDataL( 1, KNullDesC );
        mruData->SetDataL( 2, emailAddress );

        iDataStoreObserver->AddData( aDataStoreURI, mruData );
        CleanupStack::Pop(mruData); // transferred ownership
        }
    }

TBool CPsMruAdapter::FillDataStoreL( TFSMailMsgId& aId, TDesC& aDataStoreURI )
    {
    FUNC_LOG;   
    TBool result = EFalse;
// code was simplified not to trace all mailboxes
// function has trap in  Event() -case> TFSEventNewMailbox and in DeleayedMailboxCreationEventL()
// should not leave when new mailbox only when new mail address
// TODO SK how to avoid extra calls?    
    CFSMailBox *mailBox = iMailClient->GetMailBoxByUidLC(aId);
    if( mailBox )
        {
        AddMailboxObserverL( aId );
        
        // Get MRU list for this mailbox
        MDesCArray* mruList = mailBox->ListMrusL(); // TODO SK this value can be cached?
        //TODO add to cleanup stack?
        
        // update the caching status as InProgress
        iDataStoreObserver->UpdateCachingStatus( aDataStoreURI,
                ECachingInProgress );

        // Update datastore contents, first reset
        iDataStoreObserver->RemoveAll( aDataStoreURI );

        TInt trap_err=KErrNone; // for trap macro
        if ( mruList )
            {
            // trap the error to enable returning status back
            TRAP(trap_err, AddMruEmailsL( mruList, aDataStoreURI ) );  
            delete mruList;
            }
        // update the caching status as Complete
        iDataStoreObserver->UpdateCachingStatus( aDataStoreURI,
                ECachingComplete );
        if ( trap_err != KErrNone ) // check for error leave code 
            {  
            User::Leave(trap_err);       
            }
        result = ETrue;
        }   // if (mailBox)
    CleanupStack::PopAndDestroy( mailBox );
    return result;
    }

// ----------------------------------------------------------------------------
// CPsContactDataAdapter::IsDataStoresSupportedL
// 
// ----------------------------------------------------------------------------
TBool CPsMruAdapter::IsDataStoresSupportedL( TDesC& aDataStoreURI )
    {
    FUNC_LOG;   
    for ( TInt i = 0; i < iSupportedUris.Count(); i++ )
        {
        if ( iSupportedUris[i]->Compare( aDataStoreURI ) == 0 )
            return ETrue;
        }
    return EFalse;
    }

// ----------------------------------------------------------------------------
// CPsContactDataAdapter::GetSupportedDataFieldsL
// 
// ----------------------------------------------------------------------------
void CPsMruAdapter::GetSupportedDataFieldsL( RArray<TInt>& aDataFields )
    {
    FUNC_LOG;   
    aDataFields.Append( R_VPBK_FIELD_TYPE_FIRSTNAME );
    aDataFields.Append( R_VPBK_FIELD_TYPE_LASTNAME );
    aDataFields.Append( R_VPBK_FIELD_TYPE_EMAILGEN );
    }

TBool CPsMruAdapter::GetMailboxIdentifierFromUri( TDesC& aUri, TFSMailMsgId& aId )
    {
    FUNC_LOG;   
    // Find first separator in reverse order
    TInt lastSeparator = aUri.LocateReverseF( KDefaultMailBoxURISeparator );
    if ( lastSeparator == KErrNotFound )
        return EFalse;

    // This is where plugin id string starts
    TInt pluginIdStartPosition = KDefaultMailBoxURI().Length();
    TInt mailboxIdStartPosition = lastSeparator + 1;

    // It cannot be further than mailbox id
    if ( pluginIdStartPosition >= mailboxIdStartPosition )
        return EFalse;

    // Use TLex to convert string to integer
    TLex pluginIdConverter( aUri.Mid( pluginIdStartPosition,
            mailboxIdStartPosition - pluginIdStartPosition - 1 ) );

    // Use TLex to convert string to integer
    TLex mailboxIdConverter( aUri.Mid( mailboxIdStartPosition, aUri.Length()
            - mailboxIdStartPosition ) );

    TInt pluginId;
    TInt mailboxId;

    // Get plugin ID
    if ( pluginIdConverter.Val( pluginId ) != KErrNone)
        return EFalse;

    // Get mailbox ID
    if ( mailboxIdConverter.Val( mailboxId ) != KErrNone)
        return EFalse;

    // Store and we're ready
    aId.SetPluginId( TUid::Uid( pluginId ) );
    aId.SetId( mailboxId );

    return ETrue;
    }

TBool CPsMruAdapter::GetUriFromMailboxIdentifier( TFSMailMsgId& aId, HBufC& aUri )
    {
    FUNC_LOG;   
    // Add the uri identifier
    aUri.Des().Copy( KDefaultMailBoxURI );
    // Add plugin ID
    aUri.Des().AppendNum( aId.PluginId().iUid );
    // Add separator
    aUri.Des().Append( KDefaultMailBoxURISeparator );
    // Add mailbox id
    aUri.Des().AppendNum( aId.Id() );
    
    return true;
    }

void CPsMruAdapter::EventL( TFSMailEvent aEvent, TFSMailMsgId aMailbox,
        TAny* /*aParam1*/, TAny* aParam2, TAny* /*aParam3*/ )
    {
    FUNC_LOG;   
    switch ( aEvent )
        {
        case TFSEventMailMoved:
        case TFSEventMailCopied:        
        case TFSEventNewMail:
            {
            // Check the new parent folder id for this message
            // For all these events, param2 indicates the new parent folder
            TFSMailMsgId* parentFolderId =
                    static_cast< TFSMailMsgId* >( aParam2 );
            if ( parentFolderId )
                {
                TFSFolderType folderType( EFSInbox ); 
                if ( (*parentFolderId) == iPreviousParentFolderId && aMailbox == iPreviousMailboxId )
                    {
                    // we assume that folder with some id does not change 
                    // its type during mail synchronization
                    folderType = iPreviousParentFolderType;
                    }
                else
                    {
                    // Get the parent folder object
                    CFSMailFolder* parentFolder = iMailClient->GetFolderByUidL(
                            aMailbox, *parentFolderId );
                    if ( parentFolder )
                        {
                        iPreviousParentFolderId = (*parentFolderId);
                        iPreviousMailboxId = aMailbox;
                        folderType = parentFolder->GetFolderType();
                        iPreviousParentFolderType = folderType;
                        delete parentFolder;
                        parentFolder = NULL;
                        }
                    }
                // If it's sent/outbox folder,
                // we'll consider that as a new message being sent
                // and therefore we'll update the MRU list here
                if ( ( folderType == EFSSentFolder ) || 
                     ( folderType == EFSOutbox ) )
                    {
                    FillDataStoreL( aMailbox );
                    }
                }
            }
            break;
            
        case TFSEventNewMailbox:
            {
            CFSMailBox *mailboxPtr(NULL);
            // if mailbox is not ready it may leave here
            TRAPD(trap_err, mailboxPtr = iMailClient->GetMailBoxByUidL( aMailbox ));
            if ( trap_err != KErrNone )
                {
                mailboxPtr = NULL;
                }
            if( mailboxPtr ) 
                {
                delete mailboxPtr;
                mailboxPtr = NULL;
                }
            else
                {
                // mailbox still does not exist
                DeleayMailboxCreationEventL( aMailbox ); // start timer to postpone creation    
                break;
                }
            
            HBufC* identifier = HBufC::NewLC( KMaximumMailboxUriLength ); // new string ident
            if ( GetUriFromMailboxIdentifier( aMailbox, *identifier ) )
                {
                // Add to supported Uri list
                iSupportedUris.AppendL( identifier );

                // Add new data store to cache
                iStoreListObserver->AddDataStore( *identifier );

                // Add all data to data store
                // FillDataStoreL( *identifier );
                // FillDataStoreL removed - called by AddDataStore through RequestForDataL callback

                // In case there is a problem with transferring strings to  - should be obsolete 
                AddMailboxObserverL( aMailbox ); // will be added by FillDataStoreL

                CleanupStack::Pop( identifier );
                }
            else
                {
                CleanupStack::PopAndDestroy( identifier );
                }
            }
            break;

        case TFSEventMailboxDeleted:
            {
            HBufC* identifier = HBufC::NewL( KMaximumMailboxUriLength );
            if ( GetUriFromMailboxIdentifier( aMailbox, *identifier ) )
                {
                RemoveMailboxObserver( aMailbox );

                // Remove data store from cache
                iStoreListObserver->RemoveDataStore( *identifier );

                // Remove from supported Uri list
                for ( TInt i = 0; i < iSupportedUris.Count(); i++ )
                    {
                    if ( iSupportedUris[i]->Compare( *identifier ) == 0 )
                        {
                        delete iSupportedUris[i];
                        iSupportedUris.Remove( i );
                        break;
                        }
                    }
                }
            delete identifier;
            
            iPreviousParentFolderId = TFSMailMsgId();
            iPreviousMailboxId = TFSMailMsgId();  
            break;
            }
        case TFSEventMailboxSettingsChanged: // TODO SK check
            {
            iPreviousParentFolderId = TFSMailMsgId();
            iPreviousMailboxId = TFSMailMsgId();  
            break;
            }
        }
    }
TBool CPsMruAdapter::AddMailboxObserverL( TFSMailMsgId& aId )
    {
    FUNC_LOG;   
    for( TInt index = 0; index < iObservedMailboxes.Count(); index++ )
        {
        if( iObservedMailboxes[index] == aId )
            {
            // Already observing
            return EFalse;
            }
        }
    
    iMailClient->SubscribeMailboxEventsL( aId, *this );
    iObservedMailboxes.Append( aId );
    return ETrue;
    }

TBool CPsMruAdapter::RemoveMailboxObserver( TFSMailMsgId& aId )
    {
    FUNC_LOG;   
    for( TInt index = 0; index < iObservedMailboxes.Count(); index++ )
        {
        if( iObservedMailboxes[index] == aId )
            {
            iMailClient->UnsubscribeMailboxEvents( aId, *this );
            iObservedMailboxes.Remove( index );
            return ETrue;
            }
        }
    
    return EFalse;
    }

void CPsMruAdapter::RemoveAllMailboxObservers()
    {
    FUNC_LOG;   
    for( TInt index = 0; index < iObservedMailboxes.Count(); index++ )
        {
        iMailClient->UnsubscribeMailboxEvents( iObservedMailboxes[index], *this );
        }

    iObservedMailboxes.Reset(); 
    }

/**
 * If problem with NewMailbox this function will be called 
 * by CDelayMailboxCreationHelper timer to try it after some delay
 */
TBool CPsMruAdapter::DeleayedMailboxCreationEventL()
    {
    FUNC_LOG;
    for ( int i = iDelayedCreatedMailboxes.Count()-1; i>=0; i-- )
        {
        CFSMailBox *mailboxPtr(NULL);
        // if mailbox is not ready it may leave there
        TRAPD(trap_err, mailboxPtr = iMailClient->GetMailBoxByUidL( iDelayedCreatedMailboxes[i] ));
        if ( trap_err != KErrNone )
            {
            mailboxPtr = NULL;
            }
        if ( mailboxPtr ) 
            {
            delete mailboxPtr;
            mailboxPtr = NULL;
            
            HBufC* identifier = HBufC::NewLC( KMaximumMailboxUriLength ); // new string ident
            if ( GetUriFromMailboxIdentifier( iDelayedCreatedMailboxes[i], *identifier ) )
                {

                // Add to supported Uri list
                iSupportedUris.AppendL( identifier );

                // Add new data store to cache
                iStoreListObserver->AddDataStore( *identifier );

                // Add all data to data store 
                // removed because this is called by AddDataStore through RequestForDataL callback
                // FillDataStoreL( *identifier );

                // In case there is a problem with transferring strings to  - should be obsolete 
                AddMailboxObserverL( iDelayedCreatedMailboxes[i] ); // will be added by FillDataStoreL

                CleanupStack::Pop( identifier );
                iDelayedCreatedMailboxes.Remove( i );       
                }
            else
                {
                CleanupStack::PopAndDestroy( identifier );
                }
            } 
        }
    return ( 0 == iDelayedCreatedMailboxes.Count() );
    }

/**
 * If problem with NewMailbox this function will use 
 * CDelayMailboxCreationHelper to try it after some delay
 */
void CPsMruAdapter::DeleayMailboxCreationEventL( TFSMailMsgId &aMailbox )
    {
    FUNC_LOG;   
    if ( NULL == iDelayMailboxCreationPtr )
        {
        iDelayMailboxCreationPtr = CDelayMailboxCreationHelper::NewL( this );
        }
    iDelayedCreatedMailboxes.Append( aMailbox );
    iDelayMailboxCreationPtr->StartDelayedCall();
    }

// ---------------------------------------------------------------------------------------------------
// class CDelayMailboxCreationHelper : public CTimer
// ---------------------------------------------------------------------------------------------------

// static construction leaving on the stack
CDelayMailboxCreationHelper* CDelayMailboxCreationHelper::NewLC( CPsMruAdapter *aPsMruAdapterPtr )
    {
    FUNC_LOG;
    CDelayMailboxCreationHelper* self = new ( ELeave ) CDelayMailboxCreationHelper( aPsMruAdapterPtr );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// static construction leaving
CDelayMailboxCreationHelper* CDelayMailboxCreationHelper::NewL( CPsMruAdapter *aPsMruAdapterPtr )
    {
    FUNC_LOG;
    CDelayMailboxCreationHelper* self = CDelayMailboxCreationHelper::NewLC( aPsMruAdapterPtr );
    CleanupStack::Pop( self );
    return self;
    }

// used by MruAdapter when delayed datasource adding is needed
void CDelayMailboxCreationHelper::StartDelayedCall()    
    {
    FUNC_LOG;
    if( IsActive() )  // don't call again in case the timer rq is pending
        {
        Cancel();
        }
    iNumberOfDelayedTrials = KNumberOfDelayedTrials;
    After( KDelayToRunAddMailbox );  // CTimer::After contains SetActive() 
    }

// Limited unsuccesful call repeating
void CDelayMailboxCreationHelper::RunL()
    {
    FUNC_LOG;
    iNumberOfDelayedTrials --; 
    User::LeaveIfError( iStatus.Int() );
    TBool Handled = iPsMruAdapterPtr->DeleayedMailboxCreationEventL();
    if (( Handled ) || ( 0 >= iNumberOfDelayedTrials ))
        { // no need to call again
        Cancel();
        }
    else
        { // wait once more
//      SetActive is called by After
        After( KDelayToRunAddMailbox ); 
        }
    }

// when leave from RunL, if err handled return KErrNone
TInt CDelayMailboxCreationHelper::RunError( TInt aError )
    {
    FUNC_LOG;
    if ( KErrNone != aError )
        {
        Cancel(); // stop pending requiest
        }
    return KErrNone; //not desired to panic the thread in case of error returned
    }

// 2nd phase of construction
void CDelayMailboxCreationHelper::ConstructL()
    {
    FUNC_LOG;
    CTimer::ConstructL();
    CActiveScheduler::Add( this );
    }

// c-tor   
CDelayMailboxCreationHelper::CDelayMailboxCreationHelper(
    CPsMruAdapter *aPsMruAdapterPtr )
    : CTimer( EPriorityStandard )  //possible also EPriorityLow, EPriorityIdle
    , iPsMruAdapterPtr ( aPsMruAdapterPtr )  // not ownink ptr
    , iNumberOfDelayedTrials ( KNumberOfDelayedTrials ) //limited repeating num.
    {
    FUNC_LOG;
    }

// d-tor
CDelayMailboxCreationHelper::~CDelayMailboxCreationHelper()
    {
    FUNC_LOG;
    iPsMruAdapterPtr = NULL; // not owning interface ptr
    Cancel();  // stop pending request
    Deque();   // remove from CActiveScheduler
    }

// End of file
