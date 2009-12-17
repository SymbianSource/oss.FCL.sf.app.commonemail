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
* Description:  ESMR mrinfo calendar entry retriever implementation
*
*/


#include "emailtrace.h"
#include "cesmrimapicalretriever.h"
#include "cesmrimapmailfetcher.h"
#include "cesmrattachmentinfo.h"
#include "tesmrinputparams.h"
#include "esmricalviewerutils.h"
#include "esmrconfig.hrh"

//<cmail>
#include "CFSMailClient.h"
#include "CFSMailBox.h"
#include "CFSMailMessage.h"
#include "CFSMailMessagePart.h"
#include "CFSMailCommon.h"
#include "cesmragnexternalinterface.h"
#include "esmricalvieweropcodes.hrh"
//</cmail>

//<cmail> hardcoded paths removal
#include "esmrhelper.h"
//</cmail>
#include <calentry.h>
#include <caluser.h>
#include <calalarm.h>
#include <CalenInterimUtils2.h>
#include <s32file.h>
#include <eikenv.h>

// Unnamed namespace for local definitions
namespace {

// Mime type literal for text/calendar
_LIT8(  KTextCalendar8,  "text/calendar" );

// Literal for received ics filename
//<cmail> hard coded path removal, store goes to process's own private directory
//_LIT( KTempReceivedIcsFile, 
//	  "c:\\temp\\tempreceived.ics" ); //codescanner::driveletters
_LIT( KTempReceivedIcsFileName, "temp\\tempreceived.ics" );
//</cmail>

// Definition for temporary folder
//<cmail> hard coded path removal, store goes to process's own private directory
//_LIT( KTempFolder, "c:\\temp\\" ); //codescanner::driveletters
//</cmail>

// Definition for first position
const TInt KFirstPos(0);

/**
 * Cleanup operations for RPointerArray.
 *
 * @param aArray Pointer to RPointerArray.
 */
void MessagePartPointerArrayCleanup( TAny* aArray )
    {
    RPointerArray<CFSMailMessagePart>* messagePartArray =
        static_cast<RPointerArray<CFSMailMessagePart>*>( aArray );

    messagePartArray->ResetAndDestroy();
    messagePartArray->Close();
    }

/**
 * Cleanup operations for RPointerArray.
 *
 * @param aArray Pointer to RPointerArray.
 */
void CalEntryPointerArrayCleanup( TAny* aArray )
    {
    RPointerArray<CCalEntry>* entryArray =
        static_cast<RPointerArray<CCalEntry>*>( aArray );

    entryArray->ResetAndDestroy();
    entryArray->Close();
    }

}//namespace

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRImapIcalRetriever::CESMRImapIcalRetrieverCmd
// ---------------------------------------------------------------------------
//
CESMRImapIcalRetrieverCmd::CESMRImapIcalRetrieverCmd(
        CCalSession& aCalSession,
        CFSMailClient& aMailClient,
        TESMRInputParams& aInputParameters )
:   CESMRIcalViewerAsyncCommand( EESMRLoadMRIcalData, aCalSession ),
    iMailClient( aMailClient ),
    iInputParameters( aInputParameters )
    {
    FUNC_LOG;
    iResult.iResultCode = KErrNone;
    iResult.iOpType = OperationType();
    }

// ---------------------------------------------------------------------------
// CESMRImapIcalRetriever::~CESMRImapIcalRetrieverCmd
// ---------------------------------------------------------------------------
//
CESMRImapIcalRetrieverCmd::~CESMRImapIcalRetrieverCmd()
    {
    FUNC_LOG;
    CancelCommand();
    delete iCalImporter;
    delete iMailFetcher;
    delete iConvertedEntry;
    delete iIcsFilename;
    delete iAttachmentInfo;
    }

// ---------------------------------------------------------------------------
// CESMRImapIcalRetriever::NewL
// ---------------------------------------------------------------------------
//
CESMRImapIcalRetrieverCmd* CESMRImapIcalRetrieverCmd::NewL(
        CCalSession& aCalSession,
        CFSMailClient& aMailClient,
        TESMRInputParams& aInputParameters )
    {
    FUNC_LOG;
    CESMRImapIcalRetrieverCmd* self =
            new (ELeave) CESMRImapIcalRetrieverCmd(
                    aCalSession,
                    aMailClient,
                    aInputParameters );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CESMRImapIcalRetriever::ConstructL
// ---------------------------------------------------------------------------
//
void CESMRImapIcalRetrieverCmd::ConstructL()
    {
    FUNC_LOG;
    BaseConstructL();

    TFileName fileName(KTempReceivedIcsFileName);
    User::LeaveIfError(ESMRHelper::CreateAndAppendPrivateDirToFileName(fileName));
    iIcsFilename = fileName.AllocL();
    }

// ---------------------------------------------------------------------------
// CESMRImapIcalRetriever::ExecuteAsyncCommandL
// ---------------------------------------------------------------------------
//
void CESMRImapIcalRetrieverCmd::ExecuteAsyncCommandL()
    {
    FUNC_LOG;
    delete iCalImporter; iCalImporter = NULL;
    delete iMailFetcher; iMailFetcher = NULL;

    iCalImporter = CESMRAgnExternalInterface::NewL( KTextCalendar8 );
    iMailFetcher = CESMRImapMailFetcherCmd::NewL(
            iMailClient ,
            CalSession() );

    // Ensure mesasge is fetched to device
    // Completion is noticed via observer
    iMailFetcher->ExecuteCommandL(
            *Message(),
            *this );
    }

// ---------------------------------------------------------------------------
// CESMRImapIcalRetriever::CancelAsyncCommand
// ---------------------------------------------------------------------------
//
void CESMRImapIcalRetrieverCmd::CancelAsyncCommand()
    {
    FUNC_LOG;
    if ( iMailFetcher )
        {
        iMailFetcher->CancelCommand();
        }
    }

// ---------------------------------------------------------------------------
// CESMRImapIcalRetriever::AgnImportErrorL
// ---------------------------------------------------------------------------
//
MESMRAgnImportObserver::TImpResponse CESMRImapIcalRetrieverCmd::AgnImportErrorL(
        MESMRAgnImportObserver::TImpError /*aType*/,
        const TDesC8& /*aUid*/,
        const TDesC& /*aContext*/)
    {
    FUNC_LOG;
    // Ignore error and continue processing
    return MESMRAgnImportObserver::EImpResponseContinue;
    }

// ---------------------------------------------------------------------------
// CESMRImapIcalRetrieverCmd::OperationCompleted
// ---------------------------------------------------------------------------
//
void CESMRImapIcalRetrieverCmd::OperationCompleted(
        MESMRIcalViewerObserver::TIcalViewerOperationResult aResult )
    {
    FUNC_LOG;
    if ( aResult.iOpType == EESMRFetchMailContent )
        {
        TRAPD( error, HandleMailContentL() );

        if ( error != KErrNone ||
             !iInputParameters.iCalEntry )
            {
            aResult.iOpType = OperationType();
            aResult.iResultCode = KErrNotSupported;
            Observer()->OperationError( aResult );
            }
        else
            {
            aResult.iOpType = OperationType();
            Observer()->OperationCompleted( aResult );
            }
        }
    else
        {
        aResult.iOpType = OperationType();
        aResult.iResultCode = KErrNotSupported;

        Observer()->OperationError( aResult );
        }
    }

// ---------------------------------------------------------------------------
// CESMRImapIcalRetrieverCmd::HandleMailContentL
// ---------------------------------------------------------------------------
//
void CESMRImapIcalRetrieverCmd::HandleMailContentL()
    {
    FUNC_LOG;
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

    CFSMailMessagePart* calendarPart = ESMRIcalViewerUtils::LocateCalendarPartL( *message );
    if ( !calendarPart )
        {
        // message does not contain calendar part at all
        iResult.iResultCode = KErrNotFound;
        User::Leave( KErrNotFound );
        }
    CleanupStack::PushL( calendarPart );
    
    CreateEntryL( *calendarPart, *msg );
    iInputParameters.iCalEntry = iConvertedEntry;
    iCalendarPartId = calendarPart->GetPartId();

    CheckAttachmentDataL( *message );

    CleanupStack::PopAndDestroy( calendarPart );
    CleanupStack::PopAndDestroy( message );
    }

// ---------------------------------------------------------------------------
// CESMRImapIcalRetrieverCmd::OperationError
// ---------------------------------------------------------------------------
//
void CESMRImapIcalRetrieverCmd::OperationError(
        MESMRIcalViewerObserver::TIcalViewerOperationResult aResult )
    {
    FUNC_LOG;
    iResult.iResultCode = aResult.iResultCode;
    Observer()->OperationError( iResult );
    }

// ---------------------------------------------------------------------------
// CESMRImapIcalRetriever::CreateEntryL
// ---------------------------------------------------------------------------
//
void CESMRImapIcalRetrieverCmd::CreateEntryL(
        CFSMailMessagePart& aMessagePart,
        CFSMailMessage& aMsg)
    {
    FUNC_LOG;
    delete iConvertedEntry; iConvertedEntry = NULL;

    aMessagePart.CopyContentFileL( *iIcsFilename );

    //set up calendar session; just creating and deleting these leads to memory
    //and handle leaks.
    RFs& rfs( CEikonEnv::Static()->FsSession() );// codescanner::eikonenvstatic

    RFileReadStream fileStream;
    fileStream.PushL();
    User::LeaveIfError( fileStream.Open( rfs, *iIcsFilename, EFileRead)); //<cmail>

    RPointerArray<CCalEntry> entries;
    CleanupStack::PushL(
            TCleanupItem(
                CalEntryPointerArrayCleanup,
                &entries    ) );

    // Replaced with internal ical module
    TRAPD( err, iCalImporter->ImportL(
                        entries,
                        fileStream,
                        0,
                        *this ) );

    User::LeaveIfError( err );

    TInt entryCount( entries.Count() );
    if ( !entryCount )
        {
        // There are no entries to be processed
        User::Leave( KErrNotSupported );
        }

    // Only the first entry is processed --> Others are discarded.
    iConvertedEntry = entries[ KFirstPos ];
    entries.Remove( KFirstPos );

    CleanupStack::PopAndDestroy( &entries );
    CleanupStack::PopAndDestroy( &fileStream );

    //If there is no organizer (special case for some Exchange servers)
    //we need to add mail sender as organizer
    if(!iConvertedEntry->OrganizerL())
        {
        CFSMailAddress* sender = aMsg.GetSender();

        if(sender)
            {
            CCalUser* organizer = CCalUser::NewL( sender->GetEmailAddress() );
            CleanupStack::PushL(organizer);

            organizer->SetCommonNameL( sender->GetDisplayName() );
            iConvertedEntry->SetOrganizerL(organizer);

            CleanupStack::Pop( organizer ); // Ownership trasferred
            }
        }
    }

// ---------------------------------------------------------------------------
// CESMRImapIcalRetriever::CheckAttachmentDataL
// ---------------------------------------------------------------------------
//
void CESMRImapIcalRetrieverCmd::CheckAttachmentDataL(
        CFSMailMessage& aMessage )
    {
    FUNC_LOG;
    if ( aMessage.IsFlagSet( EFSMsgFlag_Attachments ) )
        {
        RPointerArray<CFSMailMessagePart> attachmentParts;
        CleanupStack::PushL(
                TCleanupItem(
                    MessagePartPointerArrayCleanup,
                    &attachmentParts    ) );

        aMessage.AttachmentListL( attachmentParts );

        TInt attachmentCount( attachmentParts.Count() );
        if ( attachmentCount > 0 )
            {
            delete iAttachmentInfo;
            iAttachmentInfo = NULL;
            
            iInputParameters.iAttachmentInfo = NULL;

            CESMRAttachmentInfo* attachmentInfo = CESMRAttachmentInfo::NewL();
            CleanupStack::PushL( attachmentInfo );

            for( TInt i(0); i < attachmentCount; ++i )
                {
                CESMRAttachment::TESMRAttachmentState state(
                        CESMRAttachment::EAttachmentStateDownloaded );

                if ( EFSFull != attachmentParts[i]->FetchLoadState() )
                    {
                    state = CESMRAttachment::EAttachmentStateNotDownloaded;
                    }

                TInt contentSize( attachmentParts[i]->ContentSize() );
                TPtrC attachmentName( attachmentParts[i]->AttachmentNameL() );

                if ( contentSize >= 0 && attachmentName.Length() &&
                        iCalendarPartId !=  attachmentParts[i]->GetPartId() )
                    {
                    attachmentInfo->AddAttachmentInfoL(
                            attachmentName,
                            contentSize,
                            state );
                    }
                }

            if ( attachmentInfo->AttachmentCount() )
                {
                iAttachmentInfo = attachmentInfo;
                CleanupStack::Pop( attachmentInfo );
                // ownership does not change
                iInputParameters.iAttachmentInfo = iAttachmentInfo;
                }
            else
                {
                CleanupStack::PopAndDestroy( attachmentInfo );
                }

            attachmentInfo = NULL;

            }
        CleanupStack::PopAndDestroy(); // attachmentparts
        }
    }

// EOF

