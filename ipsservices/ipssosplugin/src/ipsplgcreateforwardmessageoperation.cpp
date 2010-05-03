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
* Description: This file implements class CIpsPlgCreateForwardMessageOperation.
*
*/

// <qmail>

// INCLUDE FILES

#include "emailtrace.h"
#include "ipsplgheaders.h"

// LOCAL CONSTANTS AND MACROS

// ================= MEMBER FUNCTIONS =======================

// ----------------------------------------------------------------------------
// CIpsPlgCreateForwardMessageOperation::CIpsPlgCreateForwardMessageOperation
// ----------------------------------------------------------------------------
//
CIpsPlgCreateForwardMessageOperation::CIpsPlgCreateForwardMessageOperation(
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
// CIpsPlgCreateForwardMessageOperation::NewL
// ----------------------------------------------------------------------------
//
CIpsPlgCreateForwardMessageOperation* CIpsPlgCreateForwardMessageOperation::NewL(
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
    CIpsPlgCreateForwardMessageOperation* self =
        new (ELeave) CIpsPlgCreateForwardMessageOperation(
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
// CIpsPlgCreateForwardMessageOperation::~CIpsPlgCreateForwardMessageOperation
// ----------------------------------------------------------------------------
//
CIpsPlgCreateForwardMessageOperation::~CIpsPlgCreateForwardMessageOperation()
    {
    }

// ----------------------------------------------------------------------------
// CIpsPlgCreateForwardMessageOperation::RunL
// ----------------------------------------------------------------------------
//
void CIpsPlgCreateForwardMessageOperation::RunL()
    {
    FUNC_LOG;

    if( iStatus.Int() == KErrNone )
        {
        // new message creation has finished so make an FS type message
        CFSMailMessage* newMessage = NULL;
        
        TMsvId msgId = TMsvId();
        TRAPD( err, msgId = GetIdFromProgressL( iOperation->FinalProgress() ) );
            
        if( err == KErrNone )
            {
            newMessage = iSmtpService->CreateFSMessageAndSetFlagsL( 
                    msgId, iOriginalMessageId, iFSMailboxId.Id(), ETrue );
            }
        
        // relay the created message (observer takes ownership)
        SignalFSObserver( iStatus.Int(), newMessage );        
        }
    
    // nothing left to process, so complete the observer
    TRequestStatus* status = &iObserverRequestStatus;
    User::RequestComplete( status, iStatus.Int() );
    }

// ----------------------------------------------------------------------------
// CIpsPlgCreateForwardMessageOperation::StartMessageCreationL
// ----------------------------------------------------------------------------
//
void CIpsPlgCreateForwardMessageOperation::StartMessageCreationL()
    {
    FUNC_LOG;
    delete iOperation;
    iOperation = NULL;
    
    // Start a new operation, execution continues in RunL 
    // once the operation has finished.
    iOperation = CImEmailOperation::CreateForwardL(
            iStatus, 
            iMsvSession,
            iOriginalMessageId,
            KMsvDraftEntryId,
            iPartList, 
            KIpsPlgForwardSubjectFormat,
            KMsvEmailTypeListMHTMLMessage,
            //0,
            KUidMsgTypeSMTP);
    }

//  End of File

// </qmail>
