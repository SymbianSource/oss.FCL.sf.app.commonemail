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
* Description:  Implementation for MR attachment fetcher
*
*/

#include "emailtrace.h"
#include "cmrattachmentinfofetcher.h"
#include "esmricalviewerutils.h"

#include "cfsmailclient.h"
#include "cfsmailfolder.h"
#include "cfsmailmessagepart.h"
#include "cfsmailcommon.h"
#include "esmricalvieweropcodes.hrh"

#include <ct/rcpointerarray.h>
#include <calentry.h>
#include <calattachment.h>

// Unnamed namespace for local definitions
namespace { // codescanner::namespace

// Maximum URI length 
const TInt KMaxUriLength( 256 );

// CMail attachment URI
_LIT8( KCMailUri, "cmail://");

// Literal for URI UID format
_LIT8( KUriUIDFormat, "%d.%d/");

/**
 * Appends attachment information to URI.
 * @param aUri Reference to URI
 * @param aId Id to be added
 */
void AppendMailIdToUri( 
        TDes8& aUri,
        TFSMailMsgId aId )
    {    
    aUri.AppendFormat( KUriUIDFormat,
                       aId.PluginId().iUid,
                       aId.Id() );
    }
}

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CMRAttachmentInfoFetcher::CMRAttachmentInfoFetcher
// ---------------------------------------------------------------------------
//
CMRAttachmentInfoFetcher::CMRAttachmentInfoFetcher(
        CFSMailClient& aMailClient,
        CCalSession& aCalSession,
        CCalEntry& aCalEntry )
:   CESMRIcalViewerAsyncCommand(
        EESMRFetchAttachmentInfo,
        aCalSession ),
    iMailClient( aMailClient ),
    iCalEntry( aCalEntry )
    {
    FUNC_LOG;
    iResult.iOpType = OperationType();
    iResult.iResultCode = KErrNone;
    }

// ---------------------------------------------------------------------------
// CMRAttachmentInfoFetcher::~CMRAttachmentInfoFetcher
// ---------------------------------------------------------------------------
//
CMRAttachmentInfoFetcher::~CMRAttachmentInfoFetcher()
    {
    FUNC_LOG;
    CancelCommand();
    }

// ---------------------------------------------------------------------------
// CMRAttachmentInfoFetcher::NewL
// ---------------------------------------------------------------------------
//
CMRAttachmentInfoFetcher* CMRAttachmentInfoFetcher::NewL(
        CFSMailClient& aMailClient,
        CCalSession& aCalSession,
        CCalEntry& aCalEntry )
    {
    FUNC_LOG;

    CMRAttachmentInfoFetcher* self =
            new (ELeave) CMRAttachmentInfoFetcher( 
                    aMailClient, aCalSession, aCalEntry );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);

    return self;
    }

// ---------------------------------------------------------------------------
// CMRAttachmentInfoFetcher::ConstructL
// ---------------------------------------------------------------------------
//
void CMRAttachmentInfoFetcher::ConstructL()
    {
    FUNC_LOG;
    //do nothing
    }

// ---------------------------------------------------------------------------
// CMRAttachmentInfoFetcher::FetchMailL
// ---------------------------------------------------------------------------
//
void CMRAttachmentInfoFetcher::ExecuteAsyncCommandL()
    {
    FUNC_LOG;

    iResult.iMessage = Message();
    iResult.iOpType = OperationType();
    iResult.iResultCode = KErrNone;

    iMailMessage = Message();
    iResult.iMessage = iMailMessage;
    TBool operationStarted( ETrue );

    TFSPartFetchState messageLoadState( iMailMessage->FetchLoadState() );
    if( !iMailMessage->IsFlagSet( EFSMsgFlag_Attachments ) )
        {
        operationStarted = EFalse;
        }
    else if ( EFSEmailStructureUnknown == messageLoadState )
        {
        // Message structure needs to be fetched first
        // After structure is fetched, then message parts can be fetched
        // When structure fetching is completed, part fetching is triggered
        // automatically
        FetchMessageStructureL();
        }
    else
        {
        // Message structure is already known --> Part can be fetched
        // Wait operation is started if part needs to fetched.
        operationStarted = ConstructAttachmentInformationL();
        }

    if ( !operationStarted )
        {
        NotifyCompletion();
        }

    }

// ---------------------------------------------------------------------------
// CMRAttachmentInfoFetcher::CancelAsyncCommand
// ---------------------------------------------------------------------------
//
void CMRAttachmentInfoFetcher::CancelAsyncCommand()
    {
    FUNC_LOG;

    if ( EIdle != iState )
        {
        TRAP_IGNORE( iMailClient.CancelAllL() );

        iResult.iResultCode = KErrCancel;
        Observer()->OperationError( iResult );
        }

    }

// ---------------------------------------------------------------------------
// CMRAttachmentInfoFetcher::RequestResponseL
// ---------------------------------------------------------------------------
//
void CMRAttachmentInfoFetcher::RequestResponseL(
        TFSProgress aEvent,
        TInt aRequestId )
    {
    FUNC_LOG;
    TRAPD( err, HandleRequestResponseL(aEvent, aRequestId) );

    if ( KErrNone != err )
        {
        // Error occured --> Cancel operations
        iResult.iResultCode = err;
        NotifyCompletion();
        }
    }

// -----------------------------------------------------------------------------
// CMRAttachmentInfoFetcher::FetchMessageStructureL
// -----------------------------------------------------------------------------
//
void CMRAttachmentInfoFetcher::FetchMessageStructureL()
    {
    FUNC_LOG;

    TFSMailMsgId currentMailboxId =
            iMailMessage->GetMailBoxId();

    TFSMailMsgId currentMessageFolderId =
            iMailMessage->GetFolderId();

    CFSMailFolder* currentFolder  =
            iMailClient.GetFolderByUidL(
                    currentMailboxId,
                    currentMessageFolderId );

    CleanupStack::PushL( currentFolder );

    iState = EFetchingStructure;
    RArray<TFSMailMsgId> messageIds;
    CleanupClosePushL( messageIds );
    messageIds.Append( iMailMessage->GetMessageId() );
    iStructureRequestId =
            currentFolder->FetchMessagesL(
                    messageIds,
                    EFSMsgDataStructure,
                    *this );
    
    CleanupStack::PopAndDestroy();//messageIds
    CleanupStack::PopAndDestroy( currentFolder );
    }

// -----------------------------------------------------------------------------
// CMRAttachmentInfoFetcher::ConstructAttachmentInformationL
// -----------------------------------------------------------------------------
//
TBool CMRAttachmentInfoFetcher::ConstructAttachmentInformationL()
    {
    FUNC_LOG;
    
    TBool operationStarted( EFalse );
    
    TFSMailMsgId calpartId;
    calpartId.SetNullId();
    
    CFSMailMessage* msg = Message();

    // Fetch own pointer email message, because message structure is
    // not necessarily up to date
    CFSMailMessage* message =
            iMailClient.GetMessageByUidL(
                    msg->GetMailBoxId(),
                    msg->GetFolderId(),
                    msg->GetMessageId(),
                    EFSMsgDataStructure );
    CleanupStack::PushL( message );    
    
    CFSMailMessagePart* calendarPart = 
            ESMRIcalViewerUtils::LocateCalendarPartL( *message );
    
    if ( !calendarPart && !message->IsMRInfoSet() )
        {
        // message does not contain calendar part at all
        iResult.iResultCode = KErrNotFound;
        User::Leave( KErrNotFound );
        }    
    
    CleanupStack::PushL( calendarPart );
    
    if ( calendarPart )
        {
        calpartId = calendarPart->GetPartId();
        }
    
    RCPointerArray<CFSMailMessagePart> attachments;
    CleanupClosePushL( attachments );

    message->AttachmentListL( attachments );
    
    TInt attachmentCount( attachments.Count() );
    for( TInt i(0); i < attachmentCount; i++ )
        {        
        TInt contentSize( attachments[i]->ContentSize() );
        TPtrC attachmentName( attachments[i]->AttachmentNameL() );

        if ( contentSize >= 0 && attachmentName.Length() &&
             calpartId !=  attachments[i]->GetPartId() )
            {
            HBufC8* uriBuf = HBufC8::NewLC( KMaxUriLength );
            TPtr8 uri( uriBuf->Des() );
            
            uri.Append( KCMailUri );
            
            // Setting attachment URI
            AppendMailIdToUri( uri, message->GetMailBoxId() );
            AppendMailIdToUri( uri, message->GetFolderId() );
            AppendMailIdToUri( uri, message->GetMessageId() );
            AppendMailIdToUri( uri, attachments[i]->GetPartId() );
      
            CCalAttachment* attachment = CCalAttachment::NewUriL( uri );
            CleanupStack::PushL( attachment );
                        
            // Setting attachment label
            attachment->SetLabelL( 
                    attachments[i]->AttachmentNameL() );
            
            iCalEntry.AddAttachmentL( *attachment );
            CleanupStack::Pop( attachment );
            CleanupStack::PopAndDestroy( uriBuf );
            }
        }
    
    CleanupStack::PopAndDestroy( &attachments );
    CleanupStack::PopAndDestroy( calendarPart );
    CleanupStack::PopAndDestroy( message );
    
    return operationStarted;
    }

// -----------------------------------------------------------------------------
// CMRAttachmentInfoFetcher::FetchMessageContentsL
// -----------------------------------------------------------------------------
//
void CMRAttachmentInfoFetcher::HandleRequestResponseL(
        TFSProgress aEvent,
        TInt /*aRequestId*/ )
    {
    FUNC_LOG;

    TBool operationStarted( EFalse );

    if ( /*aRequestId == iStructureRequestId && */
              EFetchingStructure == iState )
        {
        // Fetching message structure
        switch ( aEvent.iProgressStatus )
            {
            case TFSProgress::EFSStatus_RequestComplete:
                {
                // Starting part fetching
                operationStarted = ConstructAttachmentInformationL();
                iResult.iResultCode = aEvent.iError;
                }
            break;
            case TFSProgress::EFSStatus_RequestCancelled:
                {
                // Error occured during fetch operation
                iResult.iResultCode = KErrCancel;
                }
            break;
            default:
                break;
            }
        }

    if ( !operationStarted )
        {
        NotifyCompletion();
        }

    }

// -----------------------------------------------------------------------------
// CMRAttachmentInfoFetcher::NotifyCompletion
// -----------------------------------------------------------------------------
//
void CMRAttachmentInfoFetcher::NotifyCompletion()
    {
    FUNC_LOG;
    if ( KErrNone == iResult.iResultCode )
        {
        Observer()->OperationCompleted( iResult );
        }
    else
        {
        Observer()->OperationError( iResult );
        }
    }

// EOF
