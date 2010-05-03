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
* Description: This file implements class CIpsPlgCreateReplyMessageOperation.
*
*/

// <qmail>

// INCLUDE FILES

#include "emailtrace.h"
#include "ipsplgheaders.h"

// LOCAL CONSTANTS AND MACROS

// ================= MEMBER FUNCTIONS =======================

// ----------------------------------------------------------------------------
// CIpsPlgCreateReplyMessageOperation::CIpsPlgCreateReplyMessageOperation
// ----------------------------------------------------------------------------
//
CIpsPlgCreateReplyMessageOperation::CIpsPlgCreateReplyMessageOperation(
    CIpsPlgSmtpService* aSmtpService,
    CMsvSession& aMsvSession,
    TRequestStatus& aObserverRequestStatus,
    TMsvPartList aPartList,
    TFSMailMsgId aMailBoxId,
    TMsvId aOriginalMessageId, 
    MFSMailRequestObserver& aOperationObserver,
    TInt aRequestId ) 
    :
    CIpsPlgCreateMessageOperation(
        aSmtpService,
        aMsvSession,
        aObserverRequestStatus,
        KMsvNullIndexEntryId,
        aPartList,
        aMailBoxId,
        aOperationObserver,
        aRequestId),
    iOriginalMessageId(aOriginalMessageId)
    {
    FUNC_LOG;
    }


// ----------------------------------------------------------------------------
// CIpsPlgCreateReplyMessageOperation::NewL
// ----------------------------------------------------------------------------
//
CIpsPlgCreateReplyMessageOperation* CIpsPlgCreateReplyMessageOperation::NewL(
    CIpsPlgSmtpService* aSmtpService,
    CMsvSession& aMsvSession,
    TRequestStatus& aObserverRequestStatus,
    TMsvPartList aPartList,
    TFSMailMsgId aMailBoxId,
    TMsvId aOriginalMessageId, 
    MFSMailRequestObserver& aOperationObserver,
    TInt aRequestId )
    {
    FUNC_LOG;
    CIpsPlgCreateReplyMessageOperation* self =
        new (ELeave) CIpsPlgCreateReplyMessageOperation(
            aSmtpService, 
            aMsvSession, 
            aObserverRequestStatus,
            aPartList, 
            aMailBoxId, 
            aOriginalMessageId, 
            aOperationObserver, 
            aRequestId );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self ); 
    return self;
    }

// ----------------------------------------------------------------------------
// CIpsPlgCreateReplyMessageOperation::~CIpsPlgCreateReplyMessageOperation
// ----------------------------------------------------------------------------
//
CIpsPlgCreateReplyMessageOperation::~CIpsPlgCreateReplyMessageOperation()
    {
    }

// ----------------------------------------------------------------------------
// CIpsPlgCreateReplyMessageOperation::RunL
// ----------------------------------------------------------------------------
//
void CIpsPlgCreateReplyMessageOperation::RunL()
    {
    FUNC_LOG;

    if( iStatus.Int() == KErrNone )
        {
        CFSMailMessage* newMessage = NULL;
        
        // new message creation has finished and we have an id...
        TMsvId msgId = TMsvId();
        TRAPD( err, msgId = GetIdFromProgressL( iOperation->FinalProgress() ) );
            
        if( err == KErrNone )
            {
            //  ...so we can create an FS type message
            newMessage = iSmtpService->CreateFSMessageAndSetFlagsL( 
                    msgId, iOriginalMessageId, iFSMailboxId.Id() );
            CleanupStack::PushL( newMessage ); // ***
        
            // dig out new reply message's header
            CMsvEntry* cEntry = iMsvSession.GetEntryL( msgId );
            CleanupStack::PushL( cEntry ); // ***
            CMsvStore* store = cEntry->ReadStoreL();
            CleanupStack::PushL( store ); // ***
            if( store->IsPresentL( KUidMsgFileIMailHeader ) == EFalse )
                 {
                 User::Leave(KErrCorrupt);
                 }
            CImHeader* header = CImHeader::NewLC(); // ***
            header->RestoreL( *store );
            
            // Start handling recipients
            HBufC* emailAddr( NULL );
            CFSMailAddress* fsAddr( NULL );
            
            // copy to recipients            
            TInt toRecipientCount( header->ToRecipients().Count() );
            for( TInt i = 0; i < toRecipientCount; i++ )
                {
                emailAddr = header->ToRecipients()[i].AllocLC(); // ***
                fsAddr = CFSMailAddress::NewLC(); // ***
                fsAddr->SetEmailAddress( *emailAddr ); // Copy created
                newMessage->AppendToRecipient( fsAddr ); // No copy
                CleanupStack::Pop( fsAddr ); // fsAddr belong now to fsMsg
                CleanupStack::PopAndDestroy( emailAddr ); // emailAddr not used
                }

            // get additional recipients (reply all case)
            if( iPartList & KMsvMessagePartRecipient )
                {
                // check if CC recipient read from header was present in To field
                // of original message. If so, copy it into To recipients.
                // 
                CMsvEntry* tmpEntry = iMsvSession.GetEntryL( iOriginalMessageId );
                CleanupStack::PushL( tmpEntry ); // ***
                CMsvStore* tmpStore = tmpEntry->ReadStoreL();
                CleanupStack::PushL( tmpStore ); // ***
                if( tmpStore->IsPresentL( KUidMsgFileIMailHeader ) )
                    {
                    CImHeader* tmpHeader = CImHeader::NewLC(); // ***
                    tmpHeader->RestoreL( *tmpStore );
                    
                    TInt originalToRecipientsCount = tmpHeader->ToRecipients().Count(); 
                    TBool present = EFalse;
                    for( TInt i = 0; i < header->CcRecipients().Count(); i++ )
                        {
                        emailAddr = header->CcRecipients()[i].AllocLC(); // ***
                        fsAddr = CFSMailAddress::NewLC(); // ***
                        fsAddr->SetEmailAddress( *emailAddr ); // Copy created
                        
                        present = EFalse;
                        for( TInt j = 0; j < originalToRecipientsCount; j++ )
                            {
                            if( emailAddr->Find( tmpHeader->ToRecipients()[j]) != KErrNotFound )
                                {
                                present = ETrue;
                                break;
                                }
                            }
                        
                        if( present )
                            {
                            newMessage->AppendToRecipient( fsAddr ); // No copy
                            }
                        else
                            {
                            newMessage->AppendCCRecipient( fsAddr ); // No copy
                            }
                        
                        CleanupStack::Pop( fsAddr ); // fsAddr belong now to fsMsg
                        CleanupStack::PopAndDestroy( emailAddr ); // emailAddr not used
                        }
                    CleanupStack::PopAndDestroy( tmpHeader );
                    }
                CleanupStack::PopAndDestroy( 2, tmpEntry );
                
                // copy bcc recipients
                for( TInt i = 0; i < header->BccRecipients().Count(); i++ )
                    {
                    emailAddr = header->BccRecipients()[i].AllocLC(); // ***
                    fsAddr = CFSMailAddress::NewLC(); // ***
                    fsAddr->SetEmailAddress( *emailAddr ); // Copy created
                    newMessage->AppendBCCRecipient( fsAddr ); // No copy
                    CleanupStack::Pop( fsAddr ); // fsAddr belong now to fsMsg
                    CleanupStack::PopAndDestroy( emailAddr ); // emailAddr not used
                    }
                }
                        
            CleanupStack::PopAndDestroy( 3, cEntry ); // header, store, cEntry
            CleanupStack::Pop( newMessage ); // fsMsg is given to client
            
            // save the added recipients to the message
            newMessage->SaveMessageL();
            }
        
        // relay the created message (observer takes ownership)
        SignalFSObserver( iStatus.Int(), newMessage );        
        }
    
    // nothing left to process, so complete the observer
    TRequestStatus* status = &iObserverRequestStatus;
    User::RequestComplete( status, iStatus.Int() );
    }

// ----------------------------------------------------------------------------
// CIpsPlgCreateReplyMessageOperation::StartMessageCreationL
// ----------------------------------------------------------------------------
//
void CIpsPlgCreateReplyMessageOperation::StartMessageCreationL()
    {
    FUNC_LOG;
    delete iOperation;
    iOperation = NULL;
    
    // Start a new operation, execution continues in RunL 
    // once the operation has finished.
    iOperation = CImEmailOperation::CreateReplyL(
            iStatus, 
            iMsvSession,
            iOriginalMessageId,
            KMsvDraftEntryId, 
            iPartList, 
            KIpsPlgReplySubjectFormat,
            KMsvEmailTypeListMHTMLMessage,
            //0,
            KUidMsgTypeSMTP);
    }

//  End of File

// </qmail>
