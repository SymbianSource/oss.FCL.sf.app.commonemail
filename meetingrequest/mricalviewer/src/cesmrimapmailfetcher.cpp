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
* Description:  Implementation for IMAP mail fetcher
*
*/


#include "emailtrace.h"
#include "cesmrimapmailfetcher.h"
#include "esmricalviewerutils.h"
//<cmail>
#include "cfsmailclient.h"
#include "cfsmailfolder.h"
#include "cfsmailmessagepart.h"
#include "cfsmailcommon.h"
#include "esmricalvieweropcodes.hrh"
//</cmail>


// Unnamed namespace for local definitions
namespace {

// Preferred byte count definition
const TInt KPreferredByteCount(0);
}

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRImapMailFetcherCmd::CESMRImapMailFetcherCmd
// ---------------------------------------------------------------------------
//
CESMRImapMailFetcherCmd::CESMRImapMailFetcherCmd(
        CFSMailClient& aMailClient,
        CCalSession& aCalSession )
:   CESMRIcalViewerAsyncCommand(
        EESMRFetchMailContent,
        aCalSession ),
    iMailClient( aMailClient )
    {
    FUNC_LOG;
    iResult.iOpType = OperationType();
    iResult.iResultCode = KErrNone;
    }

// ---------------------------------------------------------------------------
// CESMRImapMailFetcherCmd::~CESMRImapMailFetcherCmd
// ---------------------------------------------------------------------------
//
CESMRImapMailFetcherCmd::~CESMRImapMailFetcherCmd()
    {
    FUNC_LOG;
    CancelCommand();
    }

// ---------------------------------------------------------------------------
// CESMRImapMailFetcherCmd::NewL
// ---------------------------------------------------------------------------
//
CESMRImapMailFetcherCmd* CESMRImapMailFetcherCmd::NewL(
        CFSMailClient& aMailClient,
        CCalSession& aCalSession )
    {
    FUNC_LOG;

    CESMRImapMailFetcherCmd* self =
            new (ELeave) CESMRImapMailFetcherCmd( aMailClient, aCalSession );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);


    return self;
    }

// ---------------------------------------------------------------------------
// CESMRImapMailFetcherCmd::ConstructL
// ---------------------------------------------------------------------------
//
void CESMRImapMailFetcherCmd::ConstructL()
    {
    FUNC_LOG;
    //do nothing
    }

// ---------------------------------------------------------------------------
// CESMRImapMailFetcherCmd::FetchMailL
// ---------------------------------------------------------------------------
//
void CESMRImapMailFetcherCmd::ExecuteAsyncCommandL()
    {
    FUNC_LOG;

    iResult.iMessage = Message();
    iResult.iOpType = OperationType();
    iResult.iResultCode = KErrNone;

    iMailMessage = Message();
    iResult.iMessage = iMailMessage;
    TBool operationStarted( ETrue );

    TFSPartFetchState messageLoadState( iMailMessage->FetchLoadState() );
    if ( EFSEmailStructureUnknown == messageLoadState )
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
        operationStarted = FetchMessageContentsL();
        }

    if ( !operationStarted )
        {
        NotifyCompletion();
        }

    }

// ---------------------------------------------------------------------------
// CESMRImapMailFetcherCmd::CancelAsyncCommand
// ---------------------------------------------------------------------------
//
void CESMRImapMailFetcherCmd::CancelAsyncCommand()
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
// CESMRImapMailFetcherCmd::RequestResponseL
// ---------------------------------------------------------------------------
//
void CESMRImapMailFetcherCmd::RequestResponseL(
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
// CESMRImapMailFetcherCmd::FetchMessageStructureL
// -----------------------------------------------------------------------------
//
void CESMRImapMailFetcherCmd::FetchMessageStructureL()
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
// CESMRImapMailFetcherCmd::FetchMessageContentsL
// -----------------------------------------------------------------------------
//
TBool CESMRImapMailFetcherCmd::FetchMessageContentsL()
    {
    FUNC_LOG;
    TBool operationStarted( EFalse );

    CFSMailMessagePart* calendarPart = ESMRIcalViewerUtils::LocateCalendarPartL( *iMailMessage );
    if ( !calendarPart )
        {
        // message does not contain calendar part at all
        iResult.iResultCode = KErrNotFound;
        return operationStarted;
        }
    CleanupStack::PushL( calendarPart );
    
    TFSPartFetchState partLoadState( calendarPart->FetchLoadState() );
    if ( EFSFull != partLoadState )
        {
        // part is not fetched to device
        iMessagePartRequestId =
            calendarPart->FetchMessagePartL(
                        calendarPart->GetPartId(),
                        *this,
                        KPreferredByteCount );
    
        iState = EFetchingContents;
        operationStarted = ETrue;
        }
    
    CleanupStack::PopAndDestroy( calendarPart );
    calendarPart = NULL;
    return operationStarted;
    }

// -----------------------------------------------------------------------------
// CESMRImapMailFetcherCmd::FetchMessageContentsL
// -----------------------------------------------------------------------------
//
void CESMRImapMailFetcherCmd::HandleRequestResponseL(
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
                operationStarted = FetchMessageContentsL();
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
// CESMRImapMailFetcherCmd::NotifyCompletion
// -----------------------------------------------------------------------------
//
void CESMRImapMailFetcherCmd::NotifyCompletion()
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

