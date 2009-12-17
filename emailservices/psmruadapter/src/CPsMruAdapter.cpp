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
#include "CFSMailClient.h"
#include "CPsMruAdapter.h"
//</cmail>
#include <mmf/common/mmfcontrollerpluginresolver.h> // CleanupResetAndDestroy


#define PRINT(x)

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
	PRINT ( _L( "Enter CPsMruAdapter::CPsMruAdapter" ) );
	PRINT ( _L( "End CPsMruAdapter::CPsMruAdapter" ) );
	}

// ----------------------------------------------------------------------------
// CPsMruAdapter::ConstructL
// Two Phase Construction
// ----------------------------------------------------------------------------
void CPsMruAdapter::ConstructL( MDataStoreObserver* aObserverForDataStore,
		MStoreListObserver* aStoreListObserver )
	{
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

    PRINT ( _L( "End CPsMruAdapter::~CPsMruAdapter" ) );
    }

// ----------------------------------------------------------------------------
// CPsMruAdapter::RequestForDataL
// 
// ----------------------------------------------------------------------------
void CPsMruAdapter::RequestForDataL( TDesC& aDataStoreURI )
	{
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

TBool CPsMruAdapter::FillDataStoreL( TFSMailMsgId& aId, TDesC& aDataStoreURI )
	{
	TBool result = EFalse;

	// Mailboxes will be fetched to this array
	RPointerArray<CFSMailBox> mailBoxes;
	CleanupResetAndDestroyPushL( mailBoxes );

	// List all mailboxes
	TFSMailMsgId plugin;
	iMailClient->ListMailBoxes( plugin, mailBoxes );

	// Find the requested mailbox
	for ( TInt i = 0; i < mailBoxes.Count(); i++ )
		{
		// Get id of mailbox
		TFSMailMsgId id = mailBoxes[i]->GetId();

		// Check that this is the requested mailbox
		if ( id == aId )
			{
			AddMailboxObserverL( aId );
			
			// update the caching status as InProgress
			iDataStoreObserver->UpdateCachingStatus( aDataStoreURI,
					ECachingInProgress );

			// Update datastore contents, first reset
			iDataStoreObserver->RemoveAll( aDataStoreURI );

			// Get MRU list for this mailbox
			MDesCArray* mruList = mailBoxes[i]->ListMrusL();

			if ( mruList )
				{
				TInt entryIndex = 0;
	
				// Add all data to data store, the format is:
				// index0: displayname
				// index1: email
				// index2: next displayname
				// index3: next email
				// etc..
				for ( int mruIndex = 0; mruIndex < mruList->MdcaCount(); mruIndex += 2 )
					{
					TPtrC displayName = mruList->MdcaPoint( mruIndex );
					TPtrC emailAddress = mruList->MdcaPoint( mruIndex + 1 );
	
					CPsData* mruData = CPsData::NewL();
					mruData->SetId( entryIndex++ );
	
					// Set the data
					mruData->SetDataL( 0, displayName );
					mruData->SetDataL( 1, KNullDesC );
					mruData->SetDataL( 2, emailAddress );
	
					iDataStoreObserver->AddData( aDataStoreURI, mruData );
					}
	
				delete mruList;
				}
			// update the caching status as Complete
			iDataStoreObserver->UpdateCachingStatus( aDataStoreURI,
					ECachingComplete );

			result = ETrue;
			break;
			}
		}

	CleanupStack::PopAndDestroy( &mailBoxes );
	return result;
	}

// ----------------------------------------------------------------------------
// CPsContactDataAdapter::IsDataStoresSupportedL
// 
// ----------------------------------------------------------------------------
TBool CPsMruAdapter::IsDataStoresSupportedL( TDesC& aDataStoreURI )
	{
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
	aDataFields.Append( R_VPBK_FIELD_TYPE_FIRSTNAME );
	aDataFields.Append( R_VPBK_FIELD_TYPE_LASTNAME );
	aDataFields.Append( R_VPBK_FIELD_TYPE_EMAILGEN );
	}

TBool CPsMruAdapter::GetMailboxIdentifierFromUri( TDesC& aUri, TFSMailMsgId& aId )
	{
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
				// Get the parent folder object
				CFSMailFolder* parentFolder = iMailClient->GetFolderByUidL(
						aMailbox, *parentFolderId );
				if ( parentFolder )
					{
					CleanupStack::PushL( parentFolder );

					// If it's sent/outbox folder,
					// we'll consider that as a new message being sent
					// and therefore we'll update the MRU list here
					if ( ( parentFolder->GetFolderType() == EFSSentFolder ) || 
						 ( parentFolder->GetFolderType() == EFSOutbox ) )
						{
						FillDataStoreL( aMailbox );
						}
					CleanupStack::PopAndDestroy( parentFolder );
					}
				}
			}
			break;
			
		case TFSEventNewMailbox:
			{
			HBufC* identifier = HBufC::NewLC( KMaximumMailboxUriLength );
			if ( GetUriFromMailboxIdentifier( aMailbox, *identifier ) )
				{
				// Add to supported Uri list
				iSupportedUris.AppendL( identifier );

				// Add new data store to cache
				iStoreListObserver->AddDataStore( *identifier );

				// Add all data to data store
				FillDataStoreL( *identifier );

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
			}
			break;
		}
	}

TBool CPsMruAdapter::AddMailboxObserverL( TFSMailMsgId& aId )
	{
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
	for( TInt index = 0; index < iObservedMailboxes.Count(); index++ )
		{
		iMailClient->UnsubscribeMailboxEvents( iObservedMailboxes[index], *this );
		}

	iObservedMailboxes.Reset();	
	}


// End of file


