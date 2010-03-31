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
 * Description:
 *
 */

#include "cmrcmailremoteattachmentplugindownloader.h"
#include "mcalremoteattachmentoperationobserver.h"
#include "ccalremoteattachment.h"
#include "esmrhelper.h"
#include "cfsmailclient.h"
#include "cfsmailmessage.h"

#include <coemain.h>

#include "emailtrace.h"

// Unnamed namespace for local definitions
namespace { // codescanner::namespace

// Preferred byte count definition
const TInt KPreferredByteCount(0);

// Percentage for progress calculation
const TInt KPercentage( 100 );

// Literal for temporary path
_LIT( KTempPath, "temp\\" );

}

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CMRCmailRemoteAttachmentDownloader::CMRCmailRemoteAttachmentDownloader
// ---------------------------------------------------------------------------
//
CMRCmailRemoteAttachmentDownloader::CMRCmailRemoteAttachmentDownloader(
        CFSMailClient& aMailClient,
        MCalRemoteAttachmentOperationObserver& aObserver )
:   CActive( EPriorityStandard ),
    iMailClient( aMailClient ),
    iObserver( aObserver )
    {
    CActiveScheduler::Add( this );
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CMRCmailRemoteAttachmentDownloader::CMRCmailRemoteAttachmentDownloader
// ---------------------------------------------------------------------------
//
CMRCmailRemoteAttachmentDownloader::~CMRCmailRemoteAttachmentDownloader()
    {
    FUNC_LOG;

    if( IsActive() )
    	{
		Cancel();
    	}
    else
    	{
		DoCancel();
    	}
    delete iMailMessage;
    delete iUri;
    iAttachmentFile.Close();
    delete iRemoteAttachment;
    }

// ---------------------------------------------------------------------------
// CMRCmailRemoteAttachmentDownloader::CMRCmailRemoteAttachmentDownloader
// ---------------------------------------------------------------------------
//
CMRCmailRemoteAttachmentDownloader*
        CMRCmailRemoteAttachmentDownloader::NewL(
                CFSMailClient& aMailClient,
                MCalRemoteAttachmentOperationObserver& aObserver,
                const TDesC& aUri )
    {
    FUNC_LOG;
    CMRCmailRemoteAttachmentDownloader* self =
            new (ELeave) CMRCmailRemoteAttachmentDownloader(
                    aMailClient,
                    aObserver );

    CleanupStack::PushL( self );
    self->ConstructL( aUri );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CMRCmailRemoteAttachmentDownloader::CMRCmailRemoteAttachmentDownloader
// ---------------------------------------------------------------------------
//
void CMRCmailRemoteAttachmentDownloader::ConstructL(
        const TDesC& aUri )
    {
    FUNC_LOG;
    iUri = aUri.AllocL();

    TInt error = iMailIdParser.Parse( *iUri );
    User::LeaveIfError( error );

    // Fetch message structure and construct attachment information
    iMailMessage = iMailClient.GetMessageByUidL(
            iMailIdParser.iMailboxUid,
            iMailIdParser.iFolderUid,
            iMailIdParser.iMessageUid,
            EFSMsgDataStructure );

    CFSMailMessagePart* attachment =
        iMailMessage->ChildPartL( iMailIdParser.iMessagepartUid );
    CleanupStack::PushL( attachment );

    TPtrC attachmentName( attachment->AttachmentNameL() );
    TInt attachmentSize( attachment->ContentSize() );

    MCalRemoteAttachment::TCalRemoteAttachmentState attachmentState(
            MCalRemoteAttachment::EAttachmentStateNotDownloaded );

    if ( EFSFull == attachment->FetchLoadState() )
        {
        attachmentState = MCalRemoteAttachment::EAttachmentStateDownloaded;
        }

    iRemoteAttachment = CCalRemoteAttachment::NewL(
                                aUri,
                                attachmentName,
                                attachmentSize,
                                attachmentState );

    CleanupStack::PopAndDestroy( attachment );

    // Trigger asynchronous request
    IssueRequest();
    }

// ---------------------------------------------------------------------------
// CMRCmailRemoteAttachmentDownloader::DoCancel
// ---------------------------------------------------------------------------
//
void CMRCmailRemoteAttachmentDownloader::DoCancel()
    {
    FUNC_LOG;

    TRAP_IGNORE( DoCancelL() );
    }

// ---------------------------------------------------------------------------
// CMRCmailRemoteAttachmentDownloader::RunL
// ---------------------------------------------------------------------------
//
void CMRCmailRemoteAttachmentDownloader::RunL()
    {
    FUNC_LOG;

    // We first check whether attachment is already downloaded to device or not
    CFSMailMessagePart* attachment =
        iMailMessage->ChildPartL( iMailIdParser.iMessagepartUid );
    CleanupStack::PushL( attachment );

    if ( EFSFull == attachment->FetchLoadState() )
        {
        // Attachment is fecthed to device --> Trigger completion
        NotifyCompletionL( *attachment );
        }
    else
        {
        // Attachment is not fetched --> Start fetching attachment
        iFSMailOperationId =
                attachment->FetchMessagePartL(
                            iMailIdParser.iMessagepartUid,
                            *this,
                            KPreferredByteCount );
        }

    CleanupStack::PopAndDestroy( attachment );
    }

// ---------------------------------------------------------------------------
// CMRCmailRemoteAttachmentDownloader::RunError
// ---------------------------------------------------------------------------
//
TInt CMRCmailRemoteAttachmentDownloader::RunError( TInt aError )
    {
    FUNC_LOG;

    NotifyError( aError );

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMRCmailRemoteAttachmentDownloader::CMRCmailRemoteAttachmentDownloader
// ---------------------------------------------------------------------------
//
TInt CMRCmailRemoteAttachmentDownloader::Progress() const
    {
    FUNC_LOG;

    TInt progress( 0 );
    TRAP_IGNORE( progress = ProgressL() );
    return progress;
    }

// ---------------------------------------------------------------------------
// CMRCmailRemoteAttachmentDownloader::CMRCmailRemoteAttachmentDownloader
// ---------------------------------------------------------------------------
//
const MCalRemoteAttachment&
        CMRCmailRemoteAttachmentDownloader::AttachmentInformation() const
    {
    FUNC_LOG;
    return *iRemoteAttachment;
    }

// ---------------------------------------------------------------------------
// CMRCmailRemoteAttachmentDownloader::CMRCmailRemoteAttachmentDownloader
// ---------------------------------------------------------------------------
//
void CMRCmailRemoteAttachmentDownloader::RequestResponseL(
        TFSProgress aEvent,
        TInt aRequestId )
    {
    FUNC_LOG;

    // Make sure that correct notification is received from mail framework
    ASSERT( aRequestId == iFSMailOperationId );

    if ( aRequestId == iFSMailOperationId )
        {
        // Fetching message structure
        switch ( aEvent.iProgressStatus )
            {
            case TFSProgress::EFSStatus_RequestComplete:
                {
                // Fetch attachment part and notify completion
                CFSMailMessagePart* attachment =
                    iMailMessage->ChildPartL( iMailIdParser.iMessagepartUid );
                CleanupStack::PushL( attachment );

                NotifyCompletionL( *attachment );

                CleanupStack::PopAndDestroy( attachment );
                }
                break;

            case TFSProgress::EFSStatus_Status:
                {
                // Notify progress about downloading
                TInt progress = ( KPercentage * aEvent.iCounter ) / aEvent.iMaxCount;
                iObserver.Progress( this, progress );
                }
                break;

            case TFSProgress::EFSStatus_RequestCancelled:
                {
                // Error occured during fetch operation
                NotifyError( KErrCancel );
                }
                break;
            default:
                break;
            }
        }
    }

// ---------------------------------------------------------------------------
// CMRCmailRemoteAttachmentDownloader::CMRCmailRemoteAttachmentDownloader
// ---------------------------------------------------------------------------
//
void CMRCmailRemoteAttachmentDownloader::IssueRequest()
    {
    FUNC_LOG;

    SetActive();
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrNone );
    }

// ---------------------------------------------------------------------------
// CMRCmailRemoteAttachmentDownloader::CMRCmailRemoteAttachmentDownloader
// ---------------------------------------------------------------------------
//
void CMRCmailRemoteAttachmentDownloader::NotifyCompletionL(
        CFSMailMessagePart& aAttachment )
    {
    FUNC_LOG;

    // Make sure that file server can share files between processes
    RFs& fs = CCoeEnv::Static()->FsSession();
    fs.ShareProtected();

    // Construct filename for attachment
    TFileName fileName( KTempPath );
    fileName.Append( iRemoteAttachment->AttachmentLabel() );
    User::LeaveIfError(
            ESMRHelper::CreateAndAppendOthersDirToFileName( fileName ) );

    // Copy attachment from mail storage to file system
    aAttachment.CopyContentFileL( fileName );

    // Open file handle to attachment
    User::LeaveIfError( iAttachmentFile.Open(
                        fs,
                        fileName,
                        EFileRead | EFileShareReadersOnly ) );

    // Notify observer about completion
    iRemoteAttachment->SetAttachmentState(
            MCalRemoteAttachment::EAttachmentStateDownloaded );

    iObserver.OperationCompleted( this, iAttachmentFile );
    }

// ---------------------------------------------------------------------------
// CMRCmailRemoteAttachmentDownloader::NotifyError
// ---------------------------------------------------------------------------
//
void CMRCmailRemoteAttachmentDownloader::NotifyError( TInt aError )
    {
    FUNC_LOG;
    iObserver.OperationError( this, aError );
    }

// ---------------------------------------------------------------------------
// CMRCmailRemoteAttachmentDownloader::ProgressL
// ---------------------------------------------------------------------------
//
TInt CMRCmailRemoteAttachmentDownloader::ProgressL() const
    {
    FUNC_LOG;

    TInt progress( 0 );

    CFSMailMessagePart* attachment =
        iMailMessage->ChildPartL( iMailIdParser.iMessagepartUid );
    CleanupStack::PushL( attachment );

    TInt fetchedContent( attachment->FetchedContentSize() );
    TInt contentSize( attachment->ContentSize() );
    progress = ( KPercentage * fetchedContent ) / contentSize;

    CleanupStack::PopAndDestroy( attachment );

    return progress;
    }

// ---------------------------------------------------------------------------
// CMRCmailRemoteAttachmentDownloader::DoCancelL
// ---------------------------------------------------------------------------
//
void CMRCmailRemoteAttachmentDownloader::DoCancelL()
    {
    FUNC_LOG;

    CFSMailMessagePart* attachment =
        iMailMessage->ChildPartL( iMailIdParser.iMessagepartUid );
    CleanupStack::PushL( attachment );

    if ( EFSFull != attachment->FetchLoadState() )
        {
        iMailClient.CancelL( iFSMailOperationId );
        }

    CleanupStack::PopAndDestroy( attachment );
    }

// EOF
