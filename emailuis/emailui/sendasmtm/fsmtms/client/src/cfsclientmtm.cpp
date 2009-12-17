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
* Description:  Implementation of the class CFsMtmClient.
*
*/


// INCLUDE FILES
#include "emailtrace.h"
#include <txtrich.h>
#include <msvids.h>

#ifndef FF_CMAIL_INTEGRATION
#include <mtmuidsdef.hrh>
#endif // FF_CMAIL_INTEGRATION

#include <mtmdef.h>
#include <msvuids.h>
#include <bautils.h>
#include <featmgr.h>


#include <centralrepository.h>
#include "FreestyleEmailCenRepKeys.h"
#include <MeetingRequestIds.hrh>
#include "cfssendashelper.h"
#include "fsmtmsconstants.h"
#include "cfsclientmtm.h"


// ---------------------------------------------------------------------------
// CFsMtmClient::NewL
// Factory function. This will be called by messaging frame work while
// creating client side MTM. The ordinal of this function is assigned to
// entry_point in fsmtms.rss file. The ordinal can be found in
// FsClientMtmU.DEF
// ---------------------------------------------------------------------------
//
EXPORT_C CFsMtmClient* CFsMtmClient::NewL(
    CRegisteredMtmDll& aRegisteredMtmDll, CMsvSession& aMsvSession)
    {
    FUNC_LOG;

    CFsMtmClient* self = new (ELeave) CFsMtmClient( aRegisteredMtmDll,
        aMsvSession );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self ); //SCANNER

    return self;
    }

// ---------------------------------------------------------------------------
// CFsMtmClient::CFsMtmClient
// ---------------------------------------------------------------------------
//
CFsMtmClient::CFsMtmClient( CRegisteredMtmDll& aRegisteredMtmDll,
    CMsvSession& aMsvSession ):CBaseMtm( aRegisteredMtmDll, aMsvSession )
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CFsMtmClient::ConstructL
// ---------------------------------------------------------------------------
//
void CFsMtmClient::ConstructL()
    {
    FUNC_LOG;

    SwitchCurrentEntryL( KMsvRootIndexEntryId );
    iSendAsHelper = CFsSendAsHelper::NewL();

    // Check configuration status of "email attached in email" from cenrep
    // and set flag according to that.
    CRepository* repository = NULL;
    TRAPD( ret, repository = CRepository::NewL( KFreestyleEmailCenRep ) );
    if ( ret == KErrNone )
    	{
        CleanupStack::PushL( repository );
    	TInt rval = KErrNone;
    	TInt hidefeature = 0;
        rval = repository->Get( KEmailHideFS205UIFeature, hidefeature );
        if( rval == KErrNone && hidefeature == EFalse )
        	{
        	iEmailAttachedInEmail = ETrue;
        	iActiveSchedulerWait = CMsvOperationActiveSchedulerWait::NewLC();
			CleanupStack::Pop(); // iActiveSchedulerWait
        	}
        CleanupStack::PopAndDestroy( repository );
    	}
    }

// ---------------------------------------------------------------------------
// CFsMtmClient::~CFsMtmClient
// ---------------------------------------------------------------------------
//
CFsMtmClient::~CFsMtmClient()
    {
    FUNC_LOG;

    delete iAccountEntry;
    delete iSendAsHelper;

    if ( iEmailAttachedInEmail )
    	{
	    delete iActiveSchedulerWait;
    	}
    }

// ---------------------------------------------------------------------------
// CFsMtmClient::AddAddresseeL
// ---------------------------------------------------------------------------
//
void CFsMtmClient::AddAddresseeL( const TDesC& aRealAddress )
    {
    FUNC_LOG;

    TRAPD( err, iSendAsHelper->AddRecipientL( aRealAddress, KNullDesC,
        EMsvRecipientTo ) );
    if( err != KErrNone )
        {
        }

    }

// ---------------------------------------------------------------------------
// CFsMtmClient::AddAddresseeL
// ---------------------------------------------------------------------------
//
void CFsMtmClient::AddAddresseeL( const TDesC& aRealAddress,
    const TDesC& aAlias )
    {
    FUNC_LOG;

    TRAPD( err, iSendAsHelper->AddRecipientL( aRealAddress, aAlias,
        EMsvRecipientTo ) );
    if( err != KErrNone )
        {
        }

    }

// ---------------------------------------------------------------------------
// CFsMtmClient::AddAddresseeL
// ---------------------------------------------------------------------------
//
void CFsMtmClient::AddAddresseeL( TMsvRecipientType aType,
    const TDesC& aRealAddress )
    {
    FUNC_LOG;

    TRAPD( err, iSendAsHelper->AddRecipientL( aRealAddress, KNullDesC, aType ) );
    if( err != KErrNone )
        {
        }

    }

// ---------------------------------------------------------------------------
// CFsMtmClient::AddAddresseeL
// ---------------------------------------------------------------------------
//
void CFsMtmClient::AddAddresseeL( TMsvRecipientType aType,
    const TDesC& aRealAddress, const TDesC& aAlias )
    {
    FUNC_LOG;

    TRAPD( err, iSendAsHelper->AddRecipientL( aRealAddress, aAlias, aType ) );
    if( err != KErrNone )
        {
        }

    }

// ---------------------------------------------------------------------------
// CFsMtmClient::RemoveAddressee
// ---------------------------------------------------------------------------
//
void CFsMtmClient::RemoveAddressee(TInt /*aIndex*/)
    {
    FUNC_LOG;

    }

// ---------------------------------------------------------------------------
// CFsMtmClient::CFsMtmClient
// Store entry data. If it is a service entry, store the current settings.
// Otherwise it has to be a message.
// ---------------------------------------------------------------------------
//
void CFsMtmClient::SaveMessageL()
    {
    FUNC_LOG;
    
    HBufC* buf = HBufC::NewL( Body().DocumentLength() );
    CleanupStack::PushL( buf );
    TPtr ptr = buf->Des(); 
    Body().Extract( ptr, 0 );
    // INFO : By default the body will have x2029, so checking for > 1
    if ( ptr.Length() > 0 )
        {
        iSendAsHelper->SetBodyL( ptr );
        }
    CleanupStack::PopAndDestroy(buf);    

    // Save the message
    iAccountId = iMsvEntry->Entry().iServiceId;
    delete iAccountEntry;
    iAccountEntry = NULL;
    iAccountEntry = CMsvEntry::NewL( Session(), iAccountId,
        TMsvSelectionOrdering( KMsvNoGrouping, EMsvSortByNone,
        ETrue ));

    TUint pluginId = iAccountEntry->Entry().MtmData1();
    TUint id = iAccountEntry->Entry().MtmData2();
    iSendAsHelper->StoreDataL(pluginId, id);
    }

// ---------------------------------------------------------------------------
// CFsMtmClient::LoadMessageL
// Restore entry data Operation inverse to StoreL
// ---------------------------------------------------------------------------
//
void CFsMtmClient::LoadMessageL()
    {
    FUNC_LOG;

    }

// ---------------------------------------------------------------------------
// CFsMtmClient::ReplyL
// Not supported
// ---------------------------------------------------------------------------
//
CMsvOperation* CFsMtmClient::ReplyL (TMsvId /*aReplyEntryId*/,
    TMsvPartList /*aPartlist*/, TRequestStatus& /*aCompletionStatus*/ )
    {
    FUNC_LOG;

    User::Leave( KErrNotSupported );

    return NULL; //To statisfy the compiler
    }

// ---------------------------------------------------------------------------
// CFsMtmClient::ReplyL
// Not supported
// ---------------------------------------------------------------------------
//
CMsvOperation* CFsMtmClient::ForwardL( TMsvId /*aReplyEntryId*/,
    TMsvPartList /*aPartlist*/, TRequestStatus& /*aCompletionStatus*/ )
    {
    FUNC_LOG;

    User::Leave( KErrNotSupported );

    return NULL;
    }

// ---------------------------------------------------------------------------
// CFsMtmClient::ValidateMessage
// Not supported
// Message validation
// The only thing about the message that can be invalid
// is the iDescription, which should be an acceptable file name. So, length isn't
// allowed to be zero, and backslashes are not allowed in iDescription.
// ---------------------------------------------------------------------------
//
TMsvPartList CFsMtmClient::ValidateMessage( TUint /*aPartlist*/ )
    {
    FUNC_LOG;

    return 0; //SCANNER
    }

// ---------------------------------------------------------------------------
// CFsMtmClient::Find
// Not supported
// ---------------------------------------------------------------------------
//
TMsvPartList CFsMtmClient::Find( const TDesC& /*aTextToFind*/,
    TMsvPartList /*aPartlist*/ )
    {
    FUNC_LOG;

    return 0;
    }

// ---------------------------------------------------------------------------
// CFsMtmClient::QueryCapability
// ---------------------------------------------------------------------------
//
TInt CFsMtmClient::QueryCapability( TUid aCapability, TInt& aResponse )
    {
    FUNC_LOG;

    TInt error = KErrNone;
    switch ( aCapability.iUid )
        {
        case KUidMtmQueryMaxBodySizeValue:
        case KUidMtmQueryMaxTotalMsgSizeValue:
            aResponse = KMaxTInt;
            break;

        case KUidMtmQuerySupportedBodyValue:
            aResponse = KMtm7BitBody + KMtm8BitBody + KMtm16BitBody +
                KMtmBinaryBody;
            break;

        case KUidMtmQuerySupportAttachmentsValue:
        case KUidMtmQueryCanSendMsgValue:
            aResponse = ETrue;
            break;
        // Required to make MTM support the MR
        case KMtmUiSupportMeetingRequest:
            aResponse = ETrue;
            break;
        default:
            error = KErrNotSupported;
        }
    return error;
    }

// ---------------------------------------------------------------------------
// CFsMtmClient::SetSubjectL
// Sets the message context's subject text
// ---------------------------------------------------------------------------
//
void CFsMtmClient::SetSubjectL(const TDesC& aSubject)
    {
    FUNC_LOG;
    TRAPD( err, iSendAsHelper->SetSubjectL( aSubject) );
    if ( err != KErrNone )
        {
        }
}

// ---------------------------------------------------------------------------
// CFsMtmClient::AddAttachmentL
// ---------------------------------------------------------------------------
//
void  CFsMtmClient::AddAttachmentL(const TDesC& aFilePath,
    const TDesC8& aMimeType, TUint aCharset, TRequestStatus& aStatus)
    {
    FUNC_LOG;

    TRAPD( err,
    {
        iSendAsHelper->AddAttachmentL(aFilePath, aMimeType, aCharset );
    });

    if ( err != KErrNone )
        {
        }
    //Complete ourselves immediately.
    TRequestStatus* status = &aStatus;
    User::RequestComplete( status, KErrNone );

    }

// ---------------------------------------------------------------------------
// CFsMtmClient::AddAttachmentL
// ---------------------------------------------------------------------------
//
void CFsMtmClient::AddAttachmentL(RFile& aFile, const TDesC8& aMimeType,
    TUint aCharset, TRequestStatus& aStatus)
    {
    FUNC_LOG;
    iSendAsHelper->AddAttachmentL(aFile, aMimeType, aCharset );

    //Complete ourselves immediately.
    TRequestStatus* status = &aStatus;
    User::RequestComplete( status, KErrNone );

    }

// ---------------------------------------------------------------------------
// CFsMtmClient::AddLinkedAttachmentL
// ---------------------------------------------------------------------------
//
void CFsMtmClient::AddLinkedAttachmentL(const TDesC& aFilePath,
    const TDesC8& aMimeType, TUint aCharset, TRequestStatus& aStatus )
    {
    FUNC_LOG;
    iSendAsHelper->AddAttachmentL(aFilePath, aMimeType, aCharset );
    //Complete ourselves immediately.
    TRequestStatus* status = &aStatus;
    User::RequestComplete( status, KErrNone );
    }

// ---------------------------------------------------------------------------
// CFsMtmClient::AddEntryAsAttachmentL
// ---------------------------------------------------------------------------
//
void CFsMtmClient::AddEntryAsAttachmentL(TMsvId /*aAttachmentId*/,
    TRequestStatus& /*aStatus*/)
    {
    FUNC_LOG;

    User::Leave( KErrNotSupported );

    }

// ---------------------------------------------------------------------------
// CFsMtmClient::CreateAttachmentL
// ---------------------------------------------------------------------------
//
void CFsMtmClient::CreateAttachmentL( const TDesC& /*aFileName*/,
        RFile& /*aAttachmentFile*/, const TDesC8& /*aMimeType*/,
        TUint /*aCharset*/, TRequestStatus& /*aStatus*/)
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CFsMtmClient::InvokeSyncFunctionL
// Call MTM-specific operation synchronously
// ---------------------------------------------------------------------------
//
void CFsMtmClient::InvokeSyncFunctionL( TInt aFunctionId,
    const CMsvEntrySelection& aSelection, TDes8& aParameter )
    {
    FUNC_LOG;
    if ( aFunctionId == KMTMMailAddressRequestFunctionId &&
        ( &aSelection )->Count() > 0 ) //SCANNER
        {
        CMsvEntry* root = CMsvEntry::NewL( Session(), KMsvRootIndexEntryId,
            TMsvSelectionOrdering( KMsvNoGrouping, EMsvSortByNone, ETrue ) );
        CleanupStack::PushL( root );
        root->SetEntryL( ( &aSelection )->At( 0 ) );
        // aParameter =
        aParameter.Copy( root->Entry().iDescription );
        CleanupStack::PopAndDestroy( root );
        }

    else if ( iEmailAttachedInEmail &&
    		( aFunctionId == KEasEmailMTMAddEmailAttachment ||
    		  aFunctionId == KEasEmailMTMDeleteEntry ) )
        {
   		TInt error = KErrNone;
		CMsvOperation* operation = InvokeAsyncFunctionL( aFunctionId, aSelection, aParameter, iActiveSchedulerWait->iStatus );
		if ( operation )
    		{
			iActiveSchedulerWait->Start();
			// for now, the iStatus.Int contains the new entry ID in mtm
			iMTMAttachmentId = iActiveSchedulerWait->iStatus.Int();
			if ( iMTMAttachmentId <= 0 )
                {
                error = iActiveSchedulerWait->iStatus.Int();
                }
			else
                {
                error = KErrNone;
                }

			delete operation;
		    }
		else
		    {
			error = KErrNotSupported;
		    }

		User::LeaveIfError( error );
    	}
    else
        {
        User::Leave( KErrNotSupported );
        }
    }

// ---------------------------------------------------------------------------
// CFsMtmClient::InvokeAsyncFunctionL
// Call MTM-specific operation asynchronously
// ---------------------------------------------------------------------------
//
CMsvOperation* CFsMtmClient::InvokeAsyncFunctionL(
    TInt aFunctionId,
    const CMsvEntrySelection &aSelection,
    TDes8 &aParameter,
    TRequestStatus &aCompletionStatus)
    {
    FUNC_LOG;

	if ( iEmailAttachedInEmail )
		{
	    CMsvOperation* operation = 0;
	    if ( aFunctionId == KMTMStandardFunctionsSendMessage )
		    {
		    operation = CMsvCompletedOperation::NewL( Session(), Type(), KNullDesC8, iAccountId, aCompletionStatus );
	        }
	    else if ( ( aFunctionId >= 0 ) && ( aFunctionId < KEasEmailMTMLastCommand ) )
	        {
			operation = Session().TransferCommandL( aSelection, aFunctionId, aParameter, aCompletionStatus );
			}
		return operation;
		}
	else
		{
	    User::Leave( KErrNotSupported );
		}

	return NULL;
    }

// ---------------------------------------------------------------------------
// CFsMtmClient::ContextEntrySwitched
// Called by the base class functions SwitchCurrentEntryL() and
// SetCurrentEntryL() when the context is changed to another entry.
// Implementation should clear iAddresseeList & any MTM specific cache.
// ---------------------------------------------------------------------------
//
void CFsMtmClient::ContextEntrySwitched()
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CFsMtmClient::RemoveRedundantAccountsL
// This function is called by FS Email server every time an instance of
// mailserver is started. This function removes redundant mail boxes.
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CFsMtmClient::RemoveRedundantAccountsL(RArray<TFSMailMsgId>& aMailBoxIds)
	{
    FUNC_LOG;

	// Get S60 mailboxes
	CMsvEntry* root = CMsvEntry::NewL( Session(), KMsvRootIndexEntryId,
	       TMsvSelectionOrdering( KMsvNoGrouping, EMsvSortByNone, ETrue ) );
	CleanupStack::PushL( root );

	// Go through MsvEntries and delete account if corresponding
	// FS account doesn't exist.

	CMsvEntrySelection* childSelection = root->ChildrenWithMtmL(
	       KUidMsgValTypeFsMtmVal );
	CleanupStack::PushL( childSelection );
	for ( TInt i = 0; i < childSelection->Count(); i++ )
	    {
	    TBool accountExistsInFS = EFalse;
	    root->SetEntryL( childSelection->At( i ) );
	    TUint pluginId = root->Entry().MtmData1();
	    TUint mailboxId = root->Entry().MtmData2();

	    // Go through FS mailboxes and check if box is in there
	    for(TInt j = 0; j < aMailBoxIds.Count(); j++ )
	    	{
	    	TUint FSid = aMailBoxIds[j].Id();
	    	if( FSid == mailboxId )
	    		{
	    		accountExistsInFS = ETrue;
	    	    break;
	    	    }
	    }

	    // Remove if not found
	    if( !accountExistsInFS )
	    	{
	    	TFSMailMsgId accountId(pluginId, mailboxId);
	    	User::LeaveIfError( DeleteAccountL( KNullDesC, accountId ) );
	        }
	    }

	CleanupStack::PopAndDestroy( childSelection );
	CleanupStack::PopAndDestroy( root );

	return KErrNone;

	}
// ---------------------------------------------------------------------------
// CFsMtmClient::CreateAccountL
// This functions will be called by FS Email server every time a FS account is
// is created. For every FS account one MTM account will be created.
// ---------------------------------------------------------------------------
//
TInt CFsMtmClient::CreateAccountL(const TDesC& aAccountName,
	const TDesC& aOwnAddress, TFSMailMsgId aMsgId)
    {
    FUNC_LOG;

    HBufC* accountName = aAccountName.AllocL();
    // +accountName
    CleanupStack::PushL( accountName );
    HBufC* ownAddress = aOwnAddress.AllocL();
    // +ownAddress
    CleanupStack::PushL( ownAddress );

    // First phase - create Freestyle account
    CMsvEntry* root = CMsvEntry::NewL( Session(), KMsvRootIndexEntryId,
           TMsvSelectionOrdering( KMsvNoGrouping, EMsvSortByNone, ETrue ) );
    // +root
    CleanupStack::PushL( root );

    // Check if account Already exists with this name
    TBool accountExitsAlready = EFalse;
    TMsvEntry newAccount;
    CMsvEntrySelection* childSelection = root->ChildrenWithMtmL(
           KUidMsgValTypeFsMtmVal );
    // childSelection
    CleanupStack::PushL( childSelection );
    for ( TInt i = 0; i < childSelection->Count(); i++ )
        {
        root->SetEntryL( childSelection->At( i ) );
        TUint pluginId = root->Entry().MtmData1();
        TUint mailboxId = root->Entry().MtmData2();
        // Plugin id and Mailbox id are used instead of account name check
        // because names does not match in case of branded mailbox
        if ( pluginId == aMsgId.PluginId().iUid && mailboxId == aMsgId.Id() )
            {
            // newAccount = root->Entry();
            iAccountId = root->Entry().Id();
            accountExitsAlready = ETrue;
            break;
            }
        }
    if ( !accountExitsAlready )
        {
        root->SetEntryL( KMsvRootIndexEntryId );
        newAccount.iMtm = KUidMsgValTypeFsMtmVal;
        newAccount.iType = KUidMsvServiceEntry;
        newAccount.iServiceId = KMsvNullIndexEntryId;
        newAccount.iDate.HomeTime();
        newAccount.iDetails.Set( *accountName );
        newAccount.iDescription.Set( *ownAddress );

        TUid pluginId = aMsgId.PluginId();
        TUint id = aMsgId.Id();
        // Store the plugin id and mailbox id.
        newAccount.iMtmData1 = pluginId.iUid;
        newAccount.iMtmData2 = id;

        FeatureManager::InitializeLibL();
        if (FeatureManager::FeatureSupported(KFeatureIdEmailMceIntegration))
        	{
            newAccount.SetVisible( ETrue );
        	}
        else
        	{
        	newAccount.SetVisible( EFalse );
        	}
        FeatureManager::UnInitializeLib();

        root->CreateL( newAccount );
        iAccountId = newAccount.Id();
        root->SetEntryL( iAccountId );

        this->ChangeDefaultServiceL( iAccountId );
        }
    //  accountName, ownAddress, root, childSelection
    CleanupStack::PopAndDestroy( childSelection ); //SCANNER
    CleanupStack::PopAndDestroy( root );
    CleanupStack::PopAndDestroy( ownAddress );
    CleanupStack::PopAndDestroy( accountName );

    delete iAccountEntry;
    iAccountEntry = NULL;
    iAccountEntry = CMsvEntry::NewL( Session(), iAccountId,
        TMsvSelectionOrdering( KMsvNoGrouping, EMsvSortByNone, ETrue ) );
    SwitchCurrentEntryL( iAccountId );
    return KErrNone;
}

// ---------------------------------------------------------------------------
// CFsMtmClient::DeleteAccountL
// This functions will be called by FS Email server every time a FS account is
// deleted. For every FS account one MTM account will be existing.
// Remove unused parameter aAccountName
// ---------------------------------------------------------------------------
//

TInt CFsMtmClient::DeleteAccountL(const TDesC& aAccountName,
    TFSMailMsgId aMsgId)
    {
    FUNC_LOG;
    iAccountId = NULL;
    // Set iAccountId and iAccountEntry with proper values
    SetTMsvIdsL( aAccountName, aMsgId );
    // Its done in this way because, else there will be panic MSGS:294 while doing
    // iMsvEntry->DeleteL operation.
    iMsvEntry->SetEntryL( KMsvRootIndexEntryId );
    if( iAccountId )
        {
        delete iAccountEntry;
        iAccountEntry = NULL;
        iMsvEntry->DeleteL( iAccountId );
        }

    // Reset the AccountId to Null so that future calls to the
    // SetTMsvIdsL will create the account.
    iAccountId = NULL;

    return KErrNone;
}


// ---------------------------------------------------------------------------
// CFsMtmClient::RenameAccountL
// This functions will be called by FS Email server every time a FS account is
// renamed. For every FS account one MTM account will be existing. The
// existing is deleted and another account is created with different name.
// ---------------------------------------------------------------------------
//
TInt CFsMtmClient::RenameAccountL( const TDesC& aAccountNewName,
		const TDesC& aOwnAddress, TFSMailMsgId aMsgId)
    {
    FUNC_LOG;
    CMsvEntry* root = CMsvEntry::NewL( Session(), KMsvRootIndexEntryId,
        TMsvSelectionOrdering( KMsvNoGrouping, EMsvSortByNone, ETrue ) );

    CleanupStack::PushL( root );

    // Get Freestyle services.
    CMsvEntrySelection* childSelection = root->ChildrenWithMtmL(
            KUidMsgValTypeFsMtmVal );
    CleanupStack::PushL( childSelection );

    // Find Freestyle Service.
    for( TInt i = 0; i < childSelection->Count(); i++ )
        {
        root->SetEntryL( childSelection->At( i ) );
        if ( root->Entry().iMtmData1 == aMsgId.PluginId().iUid &&
            root->Entry().iMtmData2 == aMsgId.Id())
            {
            TMsvEntry& tEntry = CONST_CAST( TMsvEntry&, root->Entry() );
            if ( aAccountNewName != KNullDesC && tEntry.iDetails != aAccountNewName )
           		{
           	    // Operation is mailbox name change.
           		// We end up here when TFSEventMailboxRenamed  event is received by server.
                tEntry.iDetails.Set( *aAccountNewName.AllocL() );
            	}
            if ( aOwnAddress != KNullDesC && tEntry.iDescription != aOwnAddress )
            	{
            	// Operation is owner email address change.
            	// We end up here when TFSEventMailboxSettingsChanged event is received by server.
            	tEntry.iDescription.Set( *aOwnAddress.AllocL() );
            	}
            root->ChangeL( tEntry );
            break;
            }
        }
    CleanupStack::PopAndDestroy( childSelection );//SCANNER
    CleanupStack::PopAndDestroy( root );
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CFsMtmClient::SetTMsvIdsL
// Set the iAccountId and iAccountEntry to the entry that need to be deleted.
// ---------------------------------------------------------------------------
//
void CFsMtmClient::SetTMsvIdsL(const TDesC& /*aAccountName*/,
    TFSMailMsgId aMsgId)
    {
    FUNC_LOG;

    CMsvEntry* root = CMsvEntry::NewL( Session(), KMsvRootIndexEntryId,
        TMsvSelectionOrdering( KMsvNoGrouping, EMsvSortByNone, ETrue ) );

    CleanupStack::PushL( root );

    // Check if account Already exist
    TMsvEntry newAccount;

    // Get Freestyle services.
    CMsvEntrySelection* childSelection = root->ChildrenWithMtmL(
            KUidMsgValTypeFsMtmVal );
    CleanupStack::PushL( childSelection );

    // Find Freestyle Service.
    for( TInt i = 0; i < childSelection->Count(); i++ )
        {
        root->SetEntryL( childSelection->At( i ) );

        if ( root->Entry().iMtmData1 == aMsgId.PluginId().iUid &&
                root->Entry().iMtmData2 == aMsgId.Id() )
            {
            // newAccount = root->Entry();
            iAccountId = root->Entry().Id();
            delete iAccountEntry;//SCANNER
            iAccountEntry = NULL;
            iAccountEntry = CMsvEntry::NewL( Session(), iAccountId,
                TMsvSelectionOrdering( KMsvNoGrouping, EMsvSortByNone,
                ETrue ));
            break;
            }
        }

    CleanupStack::PopAndDestroy( childSelection ); //SCANNER
    CleanupStack::PopAndDestroy( root );
}

// ---------------------------------------------------------------------------
// CFsMtmClient::AddEmailAsAttachmentL
//
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CFsMtmClient::AddEmailAsAttachmentL( const TUint& aPluginId, const TDesC& aAttachmentPath,TMsvId& aNewAttachEntryId )
    {
    FUNC_LOG;

	if ( iEmailAttachedInEmail )
		{
		TMsvId accountId;

		// Get S60 mailboxes
		CMsvEntry* root = CMsvEntry::NewL( Session(), KMsvRootIndexEntryId,
		       TMsvSelectionOrdering( KMsvNoGrouping, EMsvSortByNone, ETrue ) );
		CleanupStack::PushL( root );

		// Go through MsvEntries
		// FS account doesn't exist.
		CMsvEntrySelection* childSelection = root->ChildrenWithMtmL(
		       KUidMsgValTypeFsMtmVal );
		CleanupStack::PushL( childSelection );
		for ( TInt i = 0; i < childSelection->Count(); i++ )
		    {

		    root->SetEntryL( childSelection->At( i ) );
		    TUint pluginId = root->Entry().MtmData1();

		     //find mailbox id matching the pluginid
		    if(pluginId==aPluginId)
		        {
		        //mailboxId = root->Entry().MtmData2();
	 	        accountId =root->Entry().Id();
	 	        break;
		        }
		    }

		CleanupStack::PopAndDestroy( childSelection );
		CleanupStack::PopAndDestroy( root );

		//if KUidMsgValTypeFsMtmVal not found we leave
	    if( accountId == 0 )
	        {
	        User::Leave( KErrNotFound );
	        }

		CMsvEntrySelection* selection = new ( ELeave ) CMsvEntrySelection();
		CleanupStack::PushL( selection );
		selection->AppendL( accountId );
		selection->AppendL( accountId );

		//Call command
		TBuf8<256> tmp;
		tmp.Copy( aAttachmentPath );

		InvokeSyncFunctionL( KEasEmailMTMAddEmailAttachment, *selection, tmp ) ;

		aNewAttachEntryId = iMTMAttachmentId;

		CleanupStack::PopAndDestroy( selection ); // selection
		}
	else
		{
		User::Leave( KErrNotSupported );
		}

	return KErrNone;
    }

// ---------------------------------------------------------------------------
// CFsMtmClient::DeleteEmailAsAttachmentL
//
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CFsMtmClient::DeleteEmailAsAttachmentL( const TMsvId aNewAttachEntryId   )
    {
    FUNC_LOG;

	if ( iEmailAttachedInEmail )
		{
		TMsvId accountId;

		// Get S60 mailboxes
		CMsvEntry* root = CMsvEntry::NewL( Session(), KMsvRootIndexEntryId,
		       TMsvSelectionOrdering( KMsvNoGrouping, EMsvSortByNone, ETrue ) );
		CleanupStack::PushL( root );

		// Go through MsvEntries
		// FS account doesn't exist.
		CMsvEntrySelection* childSelection = root->ChildrenWithMtmL(
		       KUidMsgValTypeFsMtmVal );
		CleanupStack::PushL( childSelection );
		for ( TInt i = 0; i < childSelection->Count(); i++ )
		    {
		    root->SetEntryL( childSelection->At( i ) );
		    TUint pluginId = root->Entry().MtmData1();

		    // find mailbox id matching the pluginid
		    if ( pluginId == 0x2000B020 )
		     	{
		    	//mailboxId = root->Entry().MtmData2();
	 	    	accountId = root->Entry().Id();
	 	      	break;
		    	}
		    }

		CleanupStack::PopAndDestroy( childSelection );
		CleanupStack::PopAndDestroy( root );

		//if KUidMsgValTypeFsMtmVal not found we leave
	    if ( accountId == 0 )
	    	{
	    	User::Leave( KErrNotFound );
	    	}

		CMsvEntrySelection* selection = new ( ELeave ) CMsvEntrySelection();
		CleanupStack::PushL( selection );
		selection->AppendL( accountId );
		selection->AppendL( aNewAttachEntryId );

		TPckgBuf<TBool> buf( ETrue );

		// call command
		InvokeSyncFunctionL( KEasEmailMTMDeleteEntry, *selection, buf );

		CleanupStack::PopAndDestroy( selection );
		}
	else
		{
		User::Leave( KErrNotSupported );
		}

    return KErrNone;
    }

// End of file
