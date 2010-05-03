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
* Description:  common mailbox object
*
*/

// <qmail>
#include <nmcommonheaders.h>
// <//qmail>

#include "emailtrace.h"
#include <bamdesca.h>
#include "CFSMailPlugin.h"
#include "cmrcalendarinfoimpl.h"
#include "CFSMailBox.h"
#include "CFSMailRequestObserver.h"
#include "cmailboxstateext.h"
// <qmail>
#include "CFSMailFolder.h"
#include "CFSMailAddress.h"
// </qmail>

const TInt KMaxMruEntries( 150 );

// ================= MEMBER FUNCTIONS ==========================================
// -----------------------------------------------------------------------------
// CFSMailBox::NewLC
// -----------------------------------------------------------------------------
EXPORT_C CFSMailBox* CFSMailBox::NewLC(TFSMailMsgId aMailBoxId)
{
    FUNC_LOG;
  CFSMailBox* api = new (ELeave) CFSMailBox();
  CleanupStack:: PushL(api);
  api->ConstructL(aMailBoxId);
  return api;
} 

// -----------------------------------------------------------------------------
// CFSMailBox::NewL
// -----------------------------------------------------------------------------
EXPORT_C CFSMailBox* CFSMailBox::NewL(TFSMailMsgId aMailBoxId)
{
    FUNC_LOG;
  CFSMailBox* api =  CFSMailBox::NewLC(aMailBoxId);
  CleanupStack:: Pop(api);
  return api;
}

// -----------------------------------------------------------------------------
// CFSMailBox::CFSMailBox
// -----------------------------------------------------------------------------
CFSMailBox::CFSMailBox()
{
    FUNC_LOG;
    // get requesthandler pointer
    iRequestHandler = static_cast<CFSMailRequestHandler*>(Dll::Tls());
}

// -----------------------------------------------------------------------------
// CFSMailBox::~CFSMailBox
// -----------------------------------------------------------------------------
EXPORT_C CFSMailBox::~CFSMailBox()
{
    FUNC_LOG;
    iFolders.ResetAndDestroy();
}

// -----------------------------------------------------------------------------
// CFSMailBox::ConstructL
// -----------------------------------------------------------------------------
void CFSMailBox::ConstructL(TFSMailMsgId aMailBoxId)
{
    FUNC_LOG;
// <qmail>
    CFSMailBoxBase::ConstructL(aMailBoxId);
// </qmail>
}

// -----------------------------------------------------------------------------
// CFSMailBox::GoOnlineL
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailBox::GoOnlineL()
    {
    FUNC_LOG;
    
    if(CFSMailPlugin* plugin = iRequestHandler->GetPluginByUid(GetId()))
        {
        plugin->GoOnlineL( GetId() );
        }
    }

// -----------------------------------------------------------------------------
// CFSMailBox::GoOfflineL
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailBox::GoOfflineL()
    {
    FUNC_LOG;
    
    if(CFSMailPlugin* plugin = iRequestHandler->GetPluginByUid(GetId()))
        {
        plugin->GoOfflineL( GetId() );
        }
        
    }

// -----------------------------------------------------------------------------
// CFSMailBox::CancelSyncL
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailBox::CancelSyncL()
    {
    FUNC_LOG;
    
    if(CFSMailPlugin* plugin = iRequestHandler->GetPluginByUid(GetId()))
        {
        plugin->CancelSyncL( GetId() );
        }
    }

// -----------------------------------------------------------------------------
// CFSMailBox::GetLastSyncStatusL
// -----------------------------------------------------------------------------
EXPORT_C TFSProgress CFSMailBox::GetLastSyncStatusL()
    {
    FUNC_LOG;
    TFSProgress progress;
    progress.iError = EFalse;
    if(CFSMailPlugin* plugin = iRequestHandler->GetPluginByUid(GetId()))
        {
        progress = plugin->GetLastSyncStatusL( GetId() );       
        }
    return progress;
    }

// -----------------------------------------------------------------------------
// CFSMailBox::RefreshNowL
// -----------------------------------------------------------------------------
EXPORT_C TInt CFSMailBox::RefreshNowL(
    MFSMailRequestObserver& aOperationObserver )
    {
    FUNC_LOG;

    TFSPendingRequest request;
    request.iRequestId = 0;
    if(CFSMailPlugin* plugin = iRequestHandler->GetPluginByUid(GetId()))
        {
        // init asynchronous request
        request = iRequestHandler->InitAsyncRequestL( GetId().PluginId(),
                                                        aOperationObserver );
        MFSMailRequestObserver* observer = request.iObserver;
        TRAPD(err,plugin->RefreshNowL( GetId(), *observer, request.iRequestId));
        if(err != KErrNone)
            {
            iRequestHandler->CompleteRequest(request.iRequestId);
            User::Leave(err);
            }
        }
    return request.iRequestId;

   }

// -----------------------------------------------------------------------------
// CFSMailBox::RefreshNowL
// -----------------------------------------------------------------------------
EXPORT_C TInt CFSMailBox::RefreshNowL( )
    {
    FUNC_LOG;

    TFSPendingRequest request;
    request.iRequestId = 0;
    MFSMailRequestObserver* observer = NULL;
    if(CFSMailPlugin* plugin = iRequestHandler->GetPluginByUid(GetId()))
        {
        // init asynchronous request
        request = iRequestHandler->InitAsyncRequestL( GetId().PluginId(), *observer );
        
        observer = request.iObserver;
        TRAPD(err,plugin->RefreshNowL( GetId(), *observer, request.iRequestId));
        if(err != KErrNone)
            {
            iRequestHandler->CompleteRequest(request.iRequestId);
            User::Leave(err);
            }
        }
    return request.iRequestId;

   }

// -----------------------------------------------------------------------------
// CFSMailBox::CreateMessageToSend
// -----------------------------------------------------------------------------
EXPORT_C CFSMailMessage* CFSMailBox::CreateMessageToSend( )
{
    FUNC_LOG;

    CFSMailMessage* message = NULL;
    if(CFSMailPlugin* plugin = iRequestHandler->GetPluginByUid(GetId()))
        {
        TRAPD(err,message = plugin->CreateMessageToSendL( GetId() ));
        if(err != KErrNone)
            {
            message = NULL;
            }
        }
    return message;
}

// <qmail>
// -----------------------------------------------------------------------------
// CFSMailBox::CreateMessageToSendL
// -----------------------------------------------------------------------------
EXPORT_C TInt CFSMailBox::CreateMessageToSendL(
        MFSMailRequestObserver& aOperationObserver )
    {
    TFSPendingRequest request;
    
    CFSMailPlugin* plugin = iRequestHandler->GetPluginByUid( GetId() );
    
    if ( plugin )
        {
        // init asynchronous request
        request = iRequestHandler->InitAsyncRequestL( GetId().PluginId(),
                                                      aOperationObserver );

        MFSMailRequestObserver* observer = request.iObserver;

        TRAPD( err, plugin->CreateMessageToSendL( 
            GetId(), *observer, request.iRequestId ) );

        if( err != KErrNone )
            {
            iRequestHandler->CompleteRequest( request.iRequestId );
            User::Leave( err );
            }
        }    
    else
        {
        User::Leave( KErrNotFound );
        }
    
    return request.iRequestId;
    }
// </qmail>

//<qmail>
EXPORT_C int CFSMailBox::UpdateMessageFlagsL( 
    const TFSMailMsgId aMailboxId,
    RPointerArray<CFSMailMessage> &messages,
    MFSMailRequestObserver& aOperationObserver)
{
    Q_UNUSED(aMailboxId);
    TFSPendingRequest request;
        
    CFSMailPlugin* plugin = iRequestHandler->GetPluginByUid( GetId() );
    
    if ( plugin )
        {
        // init asynchronous request
        request = iRequestHandler->InitAsyncRequestL( GetId().PluginId(),
                                                      aOperationObserver );

        MFSMailRequestObserver* observer = request.iObserver;

        TRAPD( err, plugin->StoreMessagesL( 
                GetId(), 
                messages,
                *observer, request.iRequestId ) );

        if( err != KErrNone )
            {
            iRequestHandler->CompleteRequest( request.iRequestId );
            User::Leave( err );
            }
        }
    else
        {
        User::Leave( KErrNotFound );
        }
    
    return request.iRequestId;
    
}
//</qmail>

// -----------------------------------------------------------------------------
// CFSMailBox::CreateForwardMessage
// -----------------------------------------------------------------------------
EXPORT_C CFSMailMessage* CFSMailBox::CreateForwardMessage(  TFSMailMsgId aOriginalMessageId,
                                                            const TDesC& aHeaderDescriptor )
{
    FUNC_LOG;
    CFSMailMessage* message = NULL;
    if(CFSMailPlugin* plugin = iRequestHandler->GetPluginByUid(GetId()))
        {
        TRAPD(err,message = plugin->CreateForwardMessageL( GetId(), 
                                                                aOriginalMessageId,
                                                                aHeaderDescriptor ));
        if(err != KErrNone)
            {
            message = NULL;
            }
        }
    return message;
}

// <qmail>
// -----------------------------------------------------------------------------
// CFSMailBox::CreateForwardMessageL
// -----------------------------------------------------------------------------
EXPORT_C TInt CFSMailBox::CreateForwardMessageL( const TFSMailMsgId aOriginalMessageId,
                        MFSMailRequestObserver& aOperationObserver,
                        const TDesC& aHeaderDescriptor )
    {
    TFSPendingRequest request;
    
    CFSMailPlugin* plugin = iRequestHandler->GetPluginByUid( GetId() );
    
    if ( plugin )
        {
        // init asynchronous request
        request = iRequestHandler->InitAsyncRequestL( GetId().PluginId(),
                                                      aOperationObserver );

        MFSMailRequestObserver* observer = request.iObserver;

        TRAPD( err, plugin->CreateForwardMessageL( 
            GetId(), aOriginalMessageId, *observer, request.iRequestId, aHeaderDescriptor ) );

        if( err != KErrNone )
            {
            iRequestHandler->CompleteRequest( request.iRequestId );
            User::Leave( err );
            }
        }    
    else
        {
        User::Leave( KErrNotFound );
        }
    
    return request.iRequestId;
    }
// </qmail>

// -----------------------------------------------------------------------------
// CFSMailBox::CreateReplyMessage
// -----------------------------------------------------------------------------
EXPORT_C CFSMailMessage* CFSMailBox::CreateReplyMessage( TFSMailMsgId aOriginalMessageId,
                                                         TBool aReplyToAll,
                                                         const TDesC& aHeaderDescriptor )
{
    FUNC_LOG;
    CFSMailMessage* message = NULL;
    if(CFSMailPlugin* plugin = iRequestHandler->GetPluginByUid(GetId()))
        {
        TRAPD(err,message = plugin->CreateReplyMessageL( GetId(),
                                                            aOriginalMessageId,
                                                            aReplyToAll,
                                                            aHeaderDescriptor ));
        if(err != KErrNone)
            {
            message = NULL;
            }
        }
    return message;
}

// <qmail>
// -----------------------------------------------------------------------------
// CFSMailBox::CreateReplyMessageL
// -----------------------------------------------------------------------------
EXPORT_C TInt CFSMailBox::CreateReplyMessageL( const TFSMailMsgId aOriginalMessageId,
                                      const TBool aReplyToAll,
                                      MFSMailRequestObserver& aOperationObserver,
                                      const TDesC& aHeaderDescriptor )
{
    TFSPendingRequest request;
    
    CFSMailPlugin* plugin = iRequestHandler->GetPluginByUid( GetId() );
    
    if ( plugin )
        {
        // init asynchronous request
        request = iRequestHandler->InitAsyncRequestL( GetId().PluginId(),
                                                      aOperationObserver );

        MFSMailRequestObserver* observer = request.iObserver;

        TRAPD( err, plugin->CreateReplyMessageL( 
            GetId(), aOriginalMessageId, aReplyToAll, *observer, request.iRequestId, aHeaderDescriptor ) );

        if( err != KErrNone )
            {
            iRequestHandler->CompleteRequest( request.iRequestId );
            User::Leave( err );
            }
        }    
    else
        {
        User::Leave( KErrNotFound );
        }
    
    return request.iRequestId;
}
// </qmail>

// -----------------------------------------------------------------------------
// CFSMailBox::GetStandardFolderId
// -----------------------------------------------------------------------------
EXPORT_C TFSMailMsgId CFSMailBox::GetStandardFolderId(TFSFolderType aFolderType)
{
    FUNC_LOG;

    TFSMailMsgId folderId;
    if(CFSMailPlugin* plugin = iRequestHandler->GetPluginByUid(GetId()))
        {
        TRAPD(err, folderId = plugin->GetStandardFolderIdL(GetId(), aFolderType ));
        if(err != KErrNone)
            {
            folderId.SetNullId();
            }
        }
    return folderId;
}

// -----------------------------------------------------------------------------
// CFSMailBox::SendMessageL
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailBox::SendMessageL( CFSMailMessage& aMessage )
    {
    FUNC_LOG;
    if(CFSMailPlugin* plugin = iRequestHandler->GetPluginByUid(GetId()))
        {
        // <qmail> Not activated yet.
        //UpdateMrusL( aMessage.GetToRecipients(),
        //         aMessage.GetCCRecipients(),
        //       aMessage.GetBCCRecipients() );
        // </qmail>
        plugin->SendMessageL( aMessage );
        }
	}

// <qmail>
// -----------------------------------------------------------------------------
// CFSMailBox::SendMessageL
// -----------------------------------------------------------------------------
EXPORT_C TInt CFSMailBox::SendMessageL(
        CFSMailMessage& aMessage,
        MFSMailRequestObserver& aOperationObserver )
    {
    FUNC_LOG;

    TFSPendingRequest request;

    CFSMailPlugin* plugin = iRequestHandler->GetPluginByUid( GetId() );

    if ( plugin )
        {
        // <qmail> Not activated yet.
        //UpdateMrusL( aMessage.GetToRecipients(),
        //         aMessage.GetCCRecipients(),
        //       aMessage.GetBCCRecipients() );
        // </qmail>

        // init asynchronous request
        request = iRequestHandler->InitAsyncRequestL( GetId().PluginId(),
                                                      aOperationObserver );

        MFSMailRequestObserver* observer = request.iObserver;

        TRAPD( err, plugin->SendMessageL(
            aMessage, *observer, request.iRequestId ) );

        if( err != KErrNone )
            {
            iRequestHandler->CompleteRequest( request.iRequestId );
            User::Leave( err );
            }
        }
    else
        {
        User::Leave( KErrNotFound );
        }

    return request.iRequestId;
    }
// </qmail>

// -----------------------------------------------------------------------------
// CFSMailBox::ListFolders
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailBox::ListFolders(  TFSMailMsgId aFolder,
                                        RPointerArray<CFSMailFolder>& aFolderList)
{
    FUNC_LOG;

    if(CFSMailPlugin* plugin = iRequestHandler->GetPluginByUid(GetId()))
    {
        TRAPD(err, plugin->ListFoldersL(GetId(),aFolder,aFolderList));
        if(err != KErrNone)
            {
            aFolderList.ResetAndDestroy();
            }
    }
}

// -----------------------------------------------------------------------------
// CFSMailBox::ListFolders
// -----------------------------------------------------------------------------
EXPORT_C RPointerArray<CFSMailFolder>& CFSMailBox::ListFolders( )
{   
    iFolders.ResetAndDestroy();
    if(CFSMailPlugin* plugin = iRequestHandler->GetPluginByUid(GetId()))
    {
        TRAPD(err,plugin->ListFoldersL(GetId(),iFolders));
        if(err != KErrNone)
            {
            iFolders.ResetAndDestroy();
            }
    }

    return iFolders;
}

// -----------------------------------------------------------------------------
// CFSMailBox::GetBrandingIdL
// -----------------------------------------------------------------------------
EXPORT_C TDesC& CFSMailBox::GetBrandingIdL( )
{
    FUNC_LOG;
    return BrandingId();
}

// -----------------------------------------------------------------------------
// CFSMailBox::MoveMessagesL
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailBox::MoveMessagesL( const RArray<TFSMailMsgId>& aMessageIds, 
                                         const TFSMailMsgId aSourceFolderId, 
                                         const TFSMailMsgId aDestinationFolderId )
{
    FUNC_LOG;
    if(CFSMailPlugin* plugin = iRequestHandler->GetPluginByUid(GetId()))
        {
        plugin->MoveMessagesL(GetId(), aMessageIds, aSourceFolderId, aDestinationFolderId);
        }
}

// -----------------------------------------------------------------------------
// CFSMailBox::MoveMessagesL
// -----------------------------------------------------------------------------
EXPORT_C TInt CFSMailBox::MoveMessagesL( MFSMailRequestObserver& aOperationObserver,
                                         const RArray<TFSMailMsgId>& aMessageIds, 
                                         const TFSMailMsgId aSourceFolderId, 
                                         const TFSMailMsgId aDestinationFolderId )
{
    FUNC_LOG;
    TFSPendingRequest request;
    if( CFSMailPlugin* plugin = iRequestHandler->GetPluginByUid( GetId() ) )
        {
        // init asynchronous request
        request = iRequestHandler->InitAsyncRequestL( GetId().PluginId(),
                                                        aOperationObserver );
        plugin->MoveMessagesL(
            GetId(), 
            aMessageIds, 
            aSourceFolderId, 
            aDestinationFolderId, 
            aOperationObserver, 
            request.iRequestId );
        }
    return request.iRequestId;
}

// -----------------------------------------------------------------------------
// CFSMailBox::CopyMessagesL
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailBox::CopyMessagesL( const RArray<TFSMailMsgId>& aMessageIds, 
                                         RArray<TFSMailMsgId>& aNewMessages, 
                                         const TFSMailMsgId aSourceFolderId, 
                                         const TFSMailMsgId aDestinationFolderId )
{
    FUNC_LOG;
    if(CFSMailPlugin* plugin = iRequestHandler->GetPluginByUid(GetId()))
        {
        plugin->CopyMessagesL(GetId(), aMessageIds, aNewMessages,
                                aSourceFolderId, aDestinationFolderId );    
        }
}

// -----------------------------------------------------------------------------
// CFSMailBox::SearchL
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailBox::SearchL( const RPointerArray<TDesC>& /*aSearchStrings*/,
                                   const TFSMailSortCriteria&  /*aSortCriteria*/,
                                   MFSMailBoxSearchObserver&   /*aSearchObserver*/,
                                   const RArray<TFSMailMsgId>  /*aFolderIds */ )
    {
    FUNC_LOG;

    }
    
// -----------------------------------------------------------------------------
// CFSMailBox::SearchL
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailBox::SearchL( const RPointerArray<TDesC>& aSearchStrings,
                                   const TFSMailSortCriteria& aSortCriteria,
                                   MFSMailBoxSearchObserver& aSearchObserver )
    {
    FUNC_LOG;
    if(CFSMailPlugin* plugin = iRequestHandler->GetPluginByUid(GetId()))
        {

        // get mailbox folder list
        iFolders.ResetAndDestroy();
        plugin->ListFoldersL(GetId(),iFolders);

        TFSMailMsgId draftsFolderId = GetStandardFolderId( EFSDraftsFolder );
        TFSMailMsgId outboxId = GetStandardFolderId( EFSOutbox );
        
        // remove outbox, drafts folder from folder list
        RArray<TFSMailMsgId> folderIds;
        folderIds.Reset();
        for(TInt i=0;i<iFolders.Count();i++)
        {
            TFSMailMsgId id = iFolders[i]->GetFolderId();
            if( id != draftsFolderId && id != outboxId )
            {
                folderIds.Append(id);
            }
        }
        
        // start search
        plugin->SearchL( GetId(), folderIds, aSearchStrings, aSortCriteria, aSearchObserver );
        folderIds.Reset();
        }
    }

// -----------------------------------------------------------------------------
// CFSMailBox::CancelSearch
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailBox::CancelSearch()
    {
    FUNC_LOG;
    if(CFSMailPlugin* plugin = iRequestHandler->GetPluginByUid(GetId()))
        {
        plugin->CancelSearch( GetId() );
        }
    }

// -----------------------------------------------------------------------------
// CFSMailBox::ClearSearchResultCache
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailBox::ClearSearchResultCache()
    {
    FUNC_LOG;
    if(CFSMailPlugin* plugin = iRequestHandler->GetPluginByUid(GetId()))
        {
        plugin->ClearSearchResultCache( GetId() );
        }
    }

// -----------------------------------------------------------------------------
// CFSMailBox::ListMrusL
// -----------------------------------------------------------------------------
EXPORT_C MDesCArray* CFSMailBox::ListMrusL() const
    {
    FUNC_LOG;
    MDesCArray* mruList(0);
    if ( CFSMailPlugin* plugin = iRequestHandler->GetPluginByUid(GetId() ) )
        {
        mruList = plugin->GetMrusL( GetId() );
        }
    return mruList;
    }


// -----------------------------------------------------------------------------
// CFSMailBox::CurrentSyncState
// -----------------------------------------------------------------------------
EXPORT_C TSSMailSyncState CFSMailBox::CurrentSyncState() const
    {
    FUNC_LOG;
    TSSMailSyncState syncState(Idle);
    if ( CFSMailPlugin* plugin = iRequestHandler->GetPluginByUid( GetId() ) )
        {
        syncState = plugin->CurrentSyncState( GetId() );
        }
    return syncState;
    }

// -----------------------------------------------------------------------------
// CFSMailBox::HasCapability
// -----------------------------------------------------------------------------
EXPORT_C TBool CFSMailBox::HasCapability( const TFSMailBoxCapabilities aCapability ) const
{
    FUNC_LOG;
    TBool capability = EFalse;
    if ( CFSMailPlugin* plugin = iRequestHandler->GetPluginByUid( GetId() ) )
        {
        TRAPD( err,capability = plugin->MailboxHasCapabilityL( aCapability,GetId() )) ;
        if ( err != KErrNone )
            {
            capability = EFalse;
            }
        }
    return capability;
}

// -----------------------------------------------------------------------------
// CFSMailBox::GetMailBoxStatus
// -----------------------------------------------------------------------------
EXPORT_C TFSMailBoxStatus CFSMailBox::GetMailBoxStatus()
{
    FUNC_LOG;
    TFSMailBoxStatus status(EFSMailBoxOffline);
    if ( CFSMailPlugin* plugin = iRequestHandler->GetPluginByUid(GetId() ) )
    {
        status = plugin->GetMailBoxStatus( GetId() );
    }
    return status;
}

// -----------------------------------------------------------------------------
// CFSMailBox::SetCredentialsL
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailBox::SetCredentialsL( const TDesC& aUsername, const TDesC& aPassword )
    {
    FUNC_LOG;
    if ( CFSMailPlugin* plugin = iRequestHandler->GetPluginByUid(GetId() ) )
        {
        plugin->SetCredentialsL( GetId(), aUsername, aPassword );
        }
    }

// -----------------------------------------------------------------------------
// CFSMailBox::RemoveDownLoadedAttachmentsL
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailBox::RemoveDownLoadedAttachmentsL()
    {
    FUNC_LOG;
    
    CFSMailPlugin* plugin = iRequestHandler->GetPluginByUid( GetId() );
    if ( plugin )
        {
        // get inbox folder from plugin
        TFSMailMsgId folderId = GetStandardFolderId( EFSInbox );
        CFSMailFolder* folder = plugin->GetFolderByUidL( GetId(), folderId );
        if ( folder )
            {
            folder->RemoveDownLoadedAttachmentsL();
            delete folder;
            }
        }
    }

// -----------------------------------------------------------------------------
// CFSMailBox::GetConnectionId
// -----------------------------------------------------------------------------
EXPORT_C TInt CFSMailBox::GetConnectionId( TUint32& aConnectionId )
    {
    FUNC_LOG;
    TInt rcode = KErrNotSupported;
    if ( CFSMailPlugin* plugin = iRequestHandler->GetPluginByUid( GetId() ) )
        {
        rcode = plugin->GetConnectionId( GetId(), aConnectionId );
        }
    return rcode;
    }

// -----------------------------------------------------------------------------
// CFSMailBox::IsConnectionAllowedWhenRoaming
// -----------------------------------------------------------------------------
EXPORT_C TInt CFSMailBox::IsConnectionAllowedWhenRoaming( TBool& aConnectionAllowed )
    {
    FUNC_LOG;
    TInt rcode = KErrNotSupported;
    if ( CFSMailPlugin* plugin = iRequestHandler->GetPluginByUid( GetId() ) )
        {
        rcode = plugin->IsConnectionAllowedWhenRoaming( GetId(), aConnectionAllowed );
        }
    return rcode;
    }

// -----------------------------------------------------------------------------
// CFSMailBox::CreateMessageFromFileL
// -----------------------------------------------------------------------------
EXPORT_C CFSMailMessage* CFSMailBox::CreateMessageFromFileL( const RFile& aFile )
{
    FUNC_LOG;
    CFSMailMessage* message = NULL;
    if(CFSMailPlugin* plugin = iRequestHandler->GetPluginByUid(GetId()))
    {
		message = plugin->CreateMessageFromFileL( GetId(), aFile );
    }
    return message;
}

// -----------------------------------------------------------------------------
// CFSMailBox::UpdateMrusL
// -----------------------------------------------------------------------------
void CFSMailBox::UpdateMrusL(
    const RPointerArray<CFSMailAddress>& aRecipients,
    const RPointerArray<CFSMailAddress>& aCCRecipients,
    const RPointerArray<CFSMailAddress>& aBCCRecipients ) const
    {
    FUNC_LOG;
    
    // First lets make a copy of the current mru list
    // whose content we can later alter as we wish
    MDesCArray* currentMruList( NULL );

    CFSMailPlugin* plugin = iRequestHandler->GetPluginByUid(GetId());
    if ( !plugin )
        {
        User::Leave( KErrGeneral );
        }
    currentMruList = plugin->GetMrusL( GetId() );
    if ( !currentMruList )
        {
        // This should not happen because previous function
        // should leave in error cases and if there are no
        // entries then the pointer should still be pointing
        // to valid array.
        User::Leave( KErrGeneral );
        }
    
    CDesCArraySeg* newMruList( NULL );
    TRAPD( error, newMruList = CopyArrayL( *currentMruList ) );
        
    delete currentMruList;
       
    if ( error != KErrNone )
        {
        User::Leave( error );
        }
        
    CleanupStack::PushL( newMruList );
        
    // Now check that all given recipients are found from the
    // mru list.

    // Notice that the order here has a meaning. For example
    // if the latest used address is appended to the end, then
    // the aRecipients' addresses are found from the end because
    // they are updated after cc and bcc recipients.
    UpdateMruListL( *newMruList, aBCCRecipients );
    UpdateMruListL( *newMruList, aCCRecipients );
    UpdateMruListL( *newMruList, aRecipients );
    
    // Finally update the new mru list to the plugin
    plugin->SetMrusL( GetId(), newMruList );
    
    CleanupStack::PopAndDestroy( newMruList );
    }

// -----------------------------------------------------------------------------
// CFSMailBox::CopyArrayL
// -----------------------------------------------------------------------------    
CDesCArraySeg* CFSMailBox::CopyArrayL( MDesCArray& aArrayToBeCopied ) const
    {
    FUNC_LOG;
    CDesCArraySeg* newArray = new (ELeave) CDesCArraySeg( 10 );
    CleanupStack::PushL( newArray );
    
    TInt itemCount( aArrayToBeCopied.MdcaCount() );
    TInt index( 0 );
    while ( index < itemCount )
        {        
        newArray->AppendL( aArrayToBeCopied.MdcaPoint( index ) );
        ++index;
        }
    
    CleanupStack::Pop( newArray );
    return newArray;
    }

// -----------------------------------------------------------------------------
// CFSMailBox::UpdateMruListL
// -----------------------------------------------------------------------------
void CFSMailBox::UpdateMruListL(
    CDesCArraySeg& aMruList,
    const RPointerArray<CFSMailAddress>& aNewRecentlyUsedOnes ) const
    {
    FUNC_LOG;
    TUint newCount( aNewRecentlyUsedOnes.Count() );
    TUint newIndexer( 0 );
    
    while ( newIndexer < newCount )
        {
        if ( aNewRecentlyUsedOnes[newIndexer] )
            {
            // The address is used as a search string because every
            // address does not have a display name
            TDesC& searchedAddress(
                aNewRecentlyUsedOnes[newIndexer]->GetEmailAddress() );
            TInt position( -1 );
            
            TInt found(
                FindAddressFromMruList( aMruList, searchedAddress, position ) );
            
            if ( found != 0 )
                {
                AddAndRemoveExcessMruL( aMruList,
                                        *aNewRecentlyUsedOnes[newIndexer] );
                }
            else
                {
                SetAsMostRecentMruL( aMruList,
                                     position,
                                     *aNewRecentlyUsedOnes[newIndexer] );
                }
            }        
        
        ++newIndexer;
        }
    }

// -----------------------------------------------------------------------------
// CFSMailBox::FindAddressFromMruList
// -----------------------------------------------------------------------------
TInt CFSMailBox::FindAddressFromMruList( CDesCArraySeg& aMruList,
                                         TDesC& searchedAddress,
                                         TInt& aPos ) const
    {
    FUNC_LOG;
    // CDesCArray::Find() is not used here because there is
    // possibility that we have to go through the whole array
    // and return the index for one specific match. Find() returns
    // only the index of the first match and searching for the rest
    // using Find() would cause undesired complexity.
    
    
    const TInt KMruListCount( aMruList.Count() );
    // Start indexing from 1 because the first
    // address is on that index if it exists.
    TInt mruListIndexer( 1 );
    while( mruListIndexer < KMruListCount )
        {
        TPtrC address( aMruList[mruListIndexer] );
        if ( address == searchedAddress )
            {
            aPos = mruListIndexer;
            return 0;
            }
        
        // We are only interested of the addresses so let's
        // check only every other descriptor.
        // (the addresses)
        mruListIndexer = mruListIndexer + 2;
        }
    
    aPos = aMruList.Count();
    return 1;
    }




// -----------------------------------------------------------------------------
// CFSMailBox::AddAndRemoveExcessMruL
// -----------------------------------------------------------------------------
void CFSMailBox::AddAndRemoveExcessMruL( CDesCArraySeg& aMruList,
                                         CFSMailAddress& aToBeAdded ) const
    {
    FUNC_LOG;
    if ( aMruList.Count() == KMaxMruEntries )
        {
        // Remove the oldest entry pair from the beginning
        aMruList.Delete( 0, 2 );
        }
    // Latest address is always found from the end.
    AppendMruItemL( aMruList, aToBeAdded );
    }

// -----------------------------------------------------------------------------
// CFSMailBox::SetAsMostRecentMruL
// -----------------------------------------------------------------------------
void CFSMailBox::SetAsMostRecentMruL( CDesCArraySeg& aMruList,
                                      TInt aPosition,
                                      CFSMailAddress& aMostRecent ) const
    {
    FUNC_LOG;
    // Position of the address is given so the possible display name is
    // in the previous slot. Delete both.
    aMruList.Delete( aPosition - 1, 2 );
    // Latest address is always found from the end.
    AppendMruItemL( aMruList, aMostRecent );
    }

// -----------------------------------------------------------------------------
// CFSMailBox::AppendMruItemL
// -----------------------------------------------------------------------------
void CFSMailBox::AppendMruItemL( CDesCArraySeg& aMruList,
                                 CFSMailAddress& aToBeAppended ) const
    {
    FUNC_LOG;
    // In the array, display name is always the first and then comes
    // the actual address.
    
    // <cmail> avoid setting email address as display name so it won't 
    // be displayed twice in the list
    TDesC* displayName  = &aToBeAppended.GetDisplayName();    
    TDesC* emailAddress = &aToBeAppended.GetEmailAddress();

    if( displayName->Length() > 0 && displayName->Compare(*emailAddress) == 0 )
    {
        aMruList.AppendL( KNullDesC );
    }
    else
    {
        aMruList.AppendL( *displayName );                
    }
    
    aMruList.AppendL( *emailAddress );    
    }

// -----------------------------------------------------------------------------
// CFSMailBox::ReleaseExtension
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailBox::ReleaseExtension( CEmailExtension* aExtension )
    {
    FUNC_LOG;
    // no specialized behaviour, call base class
    CExtendableEmail::ReleaseExtension( aExtension );
    }
    
// -----------------------------------------------------------------------------
// CFSMailBox::ExtensionL
// -----------------------------------------------------------------------------
EXPORT_C CEmailExtension* CFSMailBox::ExtensionL( const TUid& aInterfaceUid )
    {
    FUNC_LOG;
    CEmailExtension* extension = NULL;
    if ( aInterfaceUid == KMailboxExtMrCalInfo )
        {
        extension = CExtendableEmail::ExtensionL( aInterfaceUid );
        if ( extension == NULL )
            {
            extension = new ( ELeave ) CMRCalendarInfoImpl();
            CleanupStack::PushL( extension );
            iExtensions.AddL( extension );
            CleanupStack::Pop(); // calInfo
            }
        }    
    else if ( aInterfaceUid == KEmailMailboxStateExtensionUid )
        {
        if ( !extension )
            {
            // check that plugin supports requested extension.
            if ( CFSMailPlugin* plugin = iRequestHandler->GetPluginByUid( GetId() ) )
                {
                // request extension from plugin, leaves if not supported
                extension = plugin->ExtensionL( aInterfaceUid );
                }
    
            }
        }
    else
        {
        User::Leave( KErrNotSupported );
        }
    return extension;
    }

