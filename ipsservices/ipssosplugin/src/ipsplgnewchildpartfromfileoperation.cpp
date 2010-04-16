/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: This file implements class CCIpsPlgNewChildPartFromFileOperation.
*
*/

// <qmail>
// INCLUDE FILES

#include "emailtrace.h"
#include "ipsplgheaders.h"

// LOCAL CONSTANTS AND MACROS

_LIT( KMimeTextCalRequest,  "text/calendar; method=REQUEST;" );
_LIT( KMimeTextCalResponse, "text/calendar; method=RESPONSE;" );
_LIT( KMimeTextCalCancel,   "text/calendar; method=CANCEL;" );

_LIT8( KMethod, "method" );
_LIT8( KRequest, "REQUEST" );
_LIT8( KResponse, "RESPONSE" );
_LIT8( KCancel, "CANCEL" );

// ================= MEMBER FUNCTIONS =======================

// ----------------------------------------------------------------------------
// CCIpsPlgNewChildPartFromFileOperation::CCIpsPlgNewChildPartFromFileOperation
// ----------------------------------------------------------------------------
//
CCIpsPlgNewChildPartFromFileOperation::CCIpsPlgNewChildPartFromFileOperation(
    CMsvSession& aMsvSession,
    TRequestStatus& aObserverRequestStatus,
    const TFSMailMsgId& aMailBoxId,
    const TFSMailMsgId& aMessageId,
    MFSMailRequestObserver& aOperationObserver,
    const TInt aRequestId)
    :
    CMsvOperation( 
    aMsvSession, 
    CActive::EPriorityStandard, 
    aObserverRequestStatus),
    iMailBoxId(aMailBoxId),
    iMessageId(aMessageId),
    iOperationObserver(aOperationObserver),
    iRequestId(aRequestId)
    {
    FUNC_LOG;
    CActiveScheduler::Add( this );
    }

// ----------------------------------------------------------------------------
// CCIpsPlgNewChildPartFromFileOperation::NewL
// ----------------------------------------------------------------------------
//
CCIpsPlgNewChildPartFromFileOperation* CCIpsPlgNewChildPartFromFileOperation::NewL(
    CMsvSession& aMsvSession,
    TRequestStatus& aObserverRequestStatus,
    const TFSMailMsgId& aMailBoxId,
    const TFSMailMsgId& aMessageId,
    const TDesC& aContentType,
    const TDesC& aFilePath,
    MFSMailRequestObserver& aOperationObserver,
    const TInt aRequestId,
    CIpsPlgMsgMapper *aMsgMapper)
    {
    FUNC_LOG;
    CCIpsPlgNewChildPartFromFileOperation* self =
        new (ELeave) CCIpsPlgNewChildPartFromFileOperation(
            aMsvSession,
            aObserverRequestStatus,
            aMailBoxId,
            aMessageId,
            aOperationObserver,
            aRequestId );
    CleanupStack::PushL( self );
    self->ConstructL(aMsgMapper,aContentType,aFilePath);
    CleanupStack::Pop( self ); 
    return self;
    }

// ----------------------------------------------------------------------------
// CCIpsPlgNewChildPartFromFileOperation::ConstructL
// ----------------------------------------------------------------------------
//
void CCIpsPlgNewChildPartFromFileOperation::ConstructL(CIpsPlgMsgMapper *aMsgMapper,
    const TDesC& aContentType,
    const TDesC& aFilePath)
    {
    FUNC_LOG;
    iMsgMapper = aMsgMapper;
    iStatus = KRequestPending;
    iContentType = aContentType.AllocL();
    iFilePath = aFilePath.AllocL();

    // Start async request in RunL
    iStep = EStep1; // First step
    SetActive();
    iStatus = KRequestPending;
    TRequestStatus* status = &iStatus;
    User::RequestComplete(status,KErrNone);
    }

// ----------------------------------------------------------------------------
// CCIpsPlgNewChildPartFromFileOperation::~CCIpsPlgNewChildPartFromFileOperation
// ----------------------------------------------------------------------------
//
CCIpsPlgNewChildPartFromFileOperation::~CCIpsPlgNewChildPartFromFileOperation()
    {
    FUNC_LOG;
    Cancel(); // Cancel any request, if outstanding
    delete iOperation;
    delete iContentType;
    delete iFilePath;
    }

// ----------------------------------------------------------------------------
// CCIpsPlgNewChildPartFromFileOperation::DoCancel
// ----------------------------------------------------------------------------
//
void CCIpsPlgNewChildPartFromFileOperation::DoCancel()
    {
    FUNC_LOG;
    if (iOperation)
        {
        iOperation->Cancel();
        }
    TRequestStatus* status = &iObserverRequestStatus;
    if ( status && status->Int() == KRequestPending )
        {
        SignalFSObserver( iStatus.Int(), NULL );
        User::RequestComplete( status, iStatus.Int() );
        }
    }

// ----------------------------------------------------------------------------
// CIpsPlgSosBasePlugin::GetMessageEntryL( )
// Checks whether the requested message is already cached. If not, the cached 
// objects are deleted and new objects are created.
// ----------------------------------------------------------------------------
void CCIpsPlgNewChildPartFromFileOperation::GetMessageEntryL( 
    TMsvId aId, 
    CMsvEntry*& aMessageEntry,
    CImEmailMessage*& aImEmailMessage )
    {
    FUNC_LOG;
    if ( !iCachedEntry || ( aId != iCachedEntry->Entry().Id() ) ||
            iCachedEmailMessage->IsActive() )
        {
        CleanCachedMessageEntries();
        
        iCachedEntry = iMsvSession.GetEntryL( aId );
        if ( iCachedEntry->Entry().iType == KUidMsvMessageEntry )
            {
            iCachedEmailMessage = CImEmailMessage::NewL( *iCachedEntry );
            }
        }
    aMessageEntry = iCachedEntry;
    aImEmailMessage = iCachedEmailMessage;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
void CCIpsPlgNewChildPartFromFileOperation::CleanCachedMessageEntries()
    {
    FUNC_LOG;
    delete iCachedEmailMessage;
    iCachedEmailMessage = NULL;
    delete iCachedEntry;
    iCachedEntry = NULL;
    }

// ----------------------------------------------------------------------------
// CCIpsPlgNewChildPartFromFileOperation::RunL
// ----------------------------------------------------------------------------
//
void CCIpsPlgNewChildPartFromFileOperation::RunL()
    {
    if( iStatus.Int() == KErrNone )
        // divided to 4 steps, run asunchronously from 1 to 4
        switch (iStep)
        {
            case EStep1:
                {
                iEntry = NULL;
                iMessage = NULL;
                RFile file;
                TInt fileSize( 0 );

                // Read attachment size
                TRAPD(err,file.Open( iMsvSession.FileSession(), iFilePath->Des(), EFileShareReadersOnly ));
                if (err != KErrNone)
                    {
                    RunError(err);
                    }
             
                //in rare case that file has disappeared while sending
                //we just won't get the size for it
                file.Size( fileSize );
                file.Close();    

                // Initialize CMsvAttachment instance for the attachment creation
                CMsvAttachment* info = CMsvAttachment::NewL( CMsvAttachment::EMsvFile );
                CleanupStack::PushL( info );

                info->SetAttachmentNameL( iFilePath->Des() );
                info->SetSize( fileSize );

                // Create/acquire Symbian message entry objects
                GetMessageEntryL( iMessageId.Id(), iEntry, iMessage );

                // Start attachment creation
                iStatus = KRequestPending;
                iMessage->AttachmentManager().AddAttachmentL( 
                    iFilePath->Des(), info, iStatus );
                CleanupStack::Pop( info ); // attachment manager takes ownership
                iStep = EStep2; // Next step
                SetActive();
                break;
                }
            case EStep2:
                {
                // Dig out the entry ID of the new attachment (unbelievable that
                // there seems to be no better way to do this)
                iMessage->GetAttachmentsListL( iEntry->Entry().Id( ), 
                    CImEmailMessage::EAllAttachments, CImEmailMessage::EThisMessageOnly );
                TKeyArrayFix key( 0, ECmpTInt32 );
                CMsvEntrySelection* attachmentIds = iMessage->Selection().CopyLC();
                attachmentIds->Sort( key );
                if ( !attachmentIds->Count() )
                    {
                    User::Leave( KErrGeneral );
                    }
                iNewAttachmentId = (*attachmentIds)[ attachmentIds->Count()-1 ];
                CleanupStack::PopAndDestroy( attachmentIds );
                
                CMsvEntry* cAtta = iMsvSession.GetEntryL( iNewAttachmentId );
                CleanupStack::PushL( cAtta );
                
                // Set filename to iDetails
                TMsvEntry tEntry = cAtta->Entry();
                tEntry.iDetails.Set(iFilePath->Des());

                // Do async
                iStatus = KRequestPending;
                cAtta->ChangeL( tEntry, iStatus );
                CleanupStack::PopAndDestroy( cAtta );
                iStep = EStep3; // Next step
                SetActive();
                break;
                }
            case EStep3:
                {
                CMsvEntry* cAtta = iMsvSession.GetEntryL( iNewAttachmentId );
                CleanupStack::PushL( cAtta );
                TBool parentToMultipartAlternative( EFalse );
                if( cAtta->HasStoreL() )
                    {
                    CMsvStore* store = cAtta->EditStoreL();
                    CleanupStack::PushL( store );
                    CImMimeHeader* mimeHeader = CImMimeHeader::NewLC();
                    
                    if( store->IsPresentL( KUidMsgFileMimeHeader ) )
                        {
                        mimeHeader->RestoreL( *store );
                        CDesC8Array& array = mimeHeader->ContentTypeParams();
                        array.AppendL( KMethod );
                        parentToMultipartAlternative = ETrue;
                        
                        if( iContentType->Des().Find( KMimeTextCalRequest ) != KErrNotFound )
                            {    
                            array.AppendL( KRequest );
                            }
                        else if( iContentType->Des().Find( KMimeTextCalResponse ) != KErrNotFound )
                            {
                            array.AppendL( KResponse );
                            }
                        else if( iContentType->Des().Find( KMimeTextCalCancel ) != KErrNotFound ) 
                            {
                            array.AppendL( KCancel );
                            }
                        else
                            {
                            parentToMultipartAlternative = EFalse;
                            }
                        mimeHeader->StoreWithoutCommitL( *store );
                        store->CommitL();
                        }
                    
                    CleanupStack::PopAndDestroy( 2, store );
                    }
            
                if( parentToMultipartAlternative &&
                    iFilePath->Find( _L(".ics")) != KErrNotFound )
                    {        
                    TMsvEntry tAttaEntry = cAtta->Entry();
                    TMsvId id = tAttaEntry.Parent();
                    CMsvEntry* cParent = iMsvSession.GetEntryL( id );
                    CleanupStack::PushL( cParent );
                    
                    TMsvEmailEntry tEntry = cParent->Entry();
                    tEntry.SetMessageFolderType( EFolderTypeAlternative );
                    
                    // Do async again if needed
                    iStatus = KRequestPending;
                    cParent->ChangeL( tEntry, iStatus );
                    CleanupStack::PopAndDestroy( cParent );
                    CleanupStack::PopAndDestroy( cAtta );
                    iStep = EStep4; // Next step
                    SetActive();
                    break;
                    }
                CleanupStack::PopAndDestroy( cAtta );
                iStep = EStep4; // Next step
                // Continue to next step wihout break;
                }
            case EStep4:
                {
                // Delete the message entries to get all the changes to disk and 
                // possible store locks released
                CleanCachedMessageEntries();
                
                CFSMailMessagePart* result ( NULL );
                // Create the FS message part object
                result = iMsgMapper->GetMessagePartL( iNewAttachmentId, iMailBoxId, 
                    iMessageId );
                
                // Set attachment name
                result->SetAttachmentNameL(iFilePath->Des());
            
                // store message part
                result->SaveL();
                
                // set flag
                result->SetFlag(EFSMsgFlag_Attachments);
                
                SignalFSObserver(iStatus.Int(),result);
                // nothing left to process, so complete the observer
                TRequestStatus* status = &iObserverRequestStatus;
                User::RequestComplete( status, iStatus.Int() );
                break;
                }
        }
    }

// ----------------------------------------------------------------------------
// CCIpsPlgNewChildPartFromFileOperation::RunError
// ----------------------------------------------------------------------------
//
TInt CCIpsPlgNewChildPartFromFileOperation::RunError(TInt aError)
    {
    FUNC_LOG;
    SignalFSObserver( aError, NULL );
    
    TRequestStatus* status = &iObserverRequestStatus;
    User::RequestComplete( status, aError );
    return KErrNone; // RunError must return KErrNone to active sheduler.
    }

// ----------------------------------------------------------------------------
// CCIpsPlgNewChildPartFromFileOperation::SignalFSObserver
// ----------------------------------------------------------------------------
//
void CCIpsPlgNewChildPartFromFileOperation::SignalFSObserver(
        TInt aStatus, CFSMailMessagePart* aMessagePart )
    {
    FUNC_LOG;
    TFSProgress result =
        { TFSProgress::EFSStatus_Waiting, 0, 0, KErrNone, aMessagePart };
    
    if ( aStatus == KErrCancel )
        {
        result.iProgressStatus = TFSProgress::EFSStatus_RequestCancelled;
        result.iError = KErrCancel;
        }
    else
        {
        result.iProgressStatus = TFSProgress::EFSStatus_RequestComplete;
        result.iError = aStatus;
        }

    TRAP_IGNORE( iOperationObserver.RequestResponseL( result, iRequestId ) );
    }

// ----------------------------------------------------------------------------
// CCIpsPlgNewChildPartFromFileOperation::ProgressL
// ----------------------------------------------------------------------------
//
const TDesC8& CCIpsPlgNewChildPartFromFileOperation::ProgressL()
    {
    FUNC_LOG;
    // Make sure that operation is active
    if ( IsActive() )
        {
        // Get progress
        if ( iOperation )
            {
            return iOperation->ProgressL();
            }
        }
    return iBlank;
    }

//  End of File

// </qmail>
