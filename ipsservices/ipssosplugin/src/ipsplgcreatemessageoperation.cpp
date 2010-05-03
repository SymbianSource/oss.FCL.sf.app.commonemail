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
* Description: This file implements class CIpsPlgCreateMessageOperation.
*
*/

// <qmail>

// INCLUDE FILES

#include "emailtrace.h"
#include "ipsplgheaders.h"

// LOCAL CONSTANTS AND MACROS

// ================= MEMBER FUNCTIONS =======================

// ----------------------------------------------------------------------------
// CIpsPlgCreateMessageOperation::CIpsPlgCreateMessageOperation
// ----------------------------------------------------------------------------
//
// <qmail> priority parameter has been removed
CIpsPlgCreateMessageOperation::CIpsPlgCreateMessageOperation(
    CIpsPlgSmtpService* aSmtpService,
    CMsvSession& aMsvSession,
    TRequestStatus& aObserverRequestStatus,
    TMsvId aSmtpServiceId, 
    TMsvPartList aPartList,
    TFSMailMsgId aMailBoxId,
    MFSMailRequestObserver& aOperationObserver,
    TInt aRequestId ) 
    :
    CIpsPlgBaseOperation( 
        aMsvSession, 
        aObserverRequestStatus,
        aRequestId,
        aMailBoxId),
    iSmtpService(aSmtpService),
    iSmtpServiceId(aSmtpServiceId),
    iPartList(aPartList),
    iOperationObserver(aOperationObserver)
    {
    FUNC_LOG;
    CActiveScheduler::Add( this );
    }

// ----------------------------------------------------------------------------
// CIpsPlgCreateMessageOperation::ConstructL
// ----------------------------------------------------------------------------
//
void CIpsPlgCreateMessageOperation::ConstructL()
    {
    FUNC_LOG;
    iStatus = KRequestPending;
    StartMessageCreation();
    }

// ----------------------------------------------------------------------------
// CIpsPlgCreateMessageOperation::NewL
// ----------------------------------------------------------------------------
//
CIpsPlgCreateMessageOperation* CIpsPlgCreateMessageOperation::NewL(
    CIpsPlgSmtpService* aSmtpService,
    CMsvSession& aMsvSession,
    TRequestStatus& aObserverRequestStatus,
    TMsvId aSmtpServiceId, 
    TMsvPartList aPartList,
    TFSMailMsgId aMailBoxId,
    MFSMailRequestObserver& aOperationObserver,
    TInt aRequestId )
    {
    FUNC_LOG;
    CIpsPlgCreateMessageOperation* self =
        new (ELeave) CIpsPlgCreateMessageOperation(
            aSmtpService, aMsvSession, aObserverRequestStatus, aSmtpServiceId,
            aPartList, aMailBoxId, aOperationObserver, aRequestId );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self ); 
    return self;
    }

// ----------------------------------------------------------------------------
// CIpsPlgCreateMessageOperation::~CIpsPlgCreateMessageOperation
// ----------------------------------------------------------------------------
//
CIpsPlgCreateMessageOperation::~CIpsPlgCreateMessageOperation()
    {
    FUNC_LOG;
    Cancel();

    delete iOperation;
    }

// ----------------------------------------------------------------------------
// CIpsPlgCreateMessageOperation::DoCancel
// ----------------------------------------------------------------------------
//
void CIpsPlgCreateMessageOperation::DoCancel()
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
// CIpsPlgCreateMessageOperation::RunL
// ----------------------------------------------------------------------------
//
void CIpsPlgCreateMessageOperation::RunL()
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
                    msgId, KErrNotFound, iFSMailboxId.Id() );
            }
        
        // relay the created message (observer takes ownership)
        SignalFSObserver( iStatus.Int(), newMessage );        
        }
    
    // nothing left to process, so complete the observer
    TRequestStatus* status = &iObserverRequestStatus;
    User::RequestComplete( status, iStatus.Int() );
    }

// ----------------------------------------------------------------------------
// CIpsPlgCreateMessageOperation::RunError()
// ----------------------------------------------------------------------------
//
TInt CIpsPlgCreateMessageOperation::RunError( TInt aError )
    {
    FUNC_LOG;
    SignalFSObserver( aError, NULL );
    
    TRequestStatus* status = &iObserverRequestStatus;
    User::RequestComplete( status, aError );
    return KErrNone; // RunError must return KErrNone to active sheduler.
    }

// ----------------------------------------------------------------------------
// CIpsPlgCreateMessageOperation::GetIdFromProgressL
// ----------------------------------------------------------------------------
//
TMsvId CIpsPlgCreateMessageOperation::GetIdFromProgressL( const TDesC8& aProg )
    {
    FUNC_LOG;
    // Taken from symbian os help example code
    // Create and initialise a temporary TPckg object that 
    // can hold a message Id.
    TMsvId msgId;
    TPckg<TMsvId> param(msgId);
    
    // Copy the message Id returned from the ProgressL() call into the
    // temporary TPckg object.
    param.Copy( aProg ); 

    // Check the value of the returned message Id in the TPckg object.
    if ( msgId == KMsvNullIndexEntryId )
        {
        // Email is not correctly created, so leave
        User::Leave( KErrNotFound );
        }
    return msgId;
    }

// ----------------------------------------------------------------------------
// CIpsPlgCreateMessageOperation::ProgressL
// ----------------------------------------------------------------------------
//
void CIpsPlgCreateMessageOperation::SignalFSObserver(
        TInt aStatus, CFSMailMessage* aMessage )
    {
    if ( aStatus == KErrCancel )
        {
        iFSProgress.iProgressStatus = TFSProgress::EFSStatus_RequestCancelled;
        iFSProgress.iError = KErrCancel;
        iFSProgress.iParam = NULL;
        }
    else
        {
        iFSProgress.iProgressStatus = TFSProgress::EFSStatus_RequestComplete;
        iFSProgress.iError = aStatus;
        iFSProgress.iParam = aMessage;
        }

    TRAP_IGNORE( iOperationObserver.RequestResponseL( iFSProgress, iFSRequestId ) );
    }

// ----------------------------------------------------------------------------
// CIpsPlgCreateMessageOperation::ProgressL
// ----------------------------------------------------------------------------
//
const TDesC8& CIpsPlgCreateMessageOperation::ProgressL()
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

    return KNullDesC8;
    }

// ---------------------------------------------------------------------------
// CIpsPlgCreateMessageOperation::GetErrorProgressL
// ---------------------------------------------------------------------------
//   
const TDesC8& CIpsPlgCreateMessageOperation::GetErrorProgressL( TInt /*aError*/ )
    {
    FUNC_LOG;
    
    return KNullDesC8; // error progress info not supported
    }

// ---------------------------------------------------------------------------
// CIpsPlgCreateMessageOperation::GetFSProgressL
// ---------------------------------------------------------------------------
//   
TFSProgress CIpsPlgCreateMessageOperation::GetFSProgressL() const
    {
    FUNC_LOG;
    
    return iFSProgress;
    }

// ----------------------------------------------------------------------------
// CIpsPlgCreateMessageOperation::StartMessageCreation
// ----------------------------------------------------------------------------
//
void CIpsPlgCreateMessageOperation::StartMessageCreation()
    {
    FUNC_LOG;
    iStatus = KRequestPending;
    
    TRAPD( err, StartMessageCreationL() );
    SetActive();
    // if error then complete this pass with the error code
    if ( err )
        {
        TRequestStatus* status = &iStatus;
        User::RequestComplete(status, err);
        }
    }

// ----------------------------------------------------------------------------
// CIpsPlgCreateMessageOperation::StartMessageCreationL
// ----------------------------------------------------------------------------
//
void CIpsPlgCreateMessageOperation::StartMessageCreationL()
    {
    FUNC_LOG;
    delete iOperation;
    iOperation = NULL;
    
    // Start a new operation, execution continues in RunL 
    // once the operation has finished.
    iOperation = CImEmailOperation::CreateNewL(
        iStatus, 
        iMsvSession,
        KMsvDraftEntryId,
        iSmtpServiceId, 
        iPartList, 
        KMsvEmailTypeListMHTMLMessage,
        //0,
        KUidMsgTypeSMTP);
    }


// ----------------------------------------------------------------------------
// CIpsPlgCreateMessageOperation::IpsOpType
// ----------------------------------------------------------------------------    
TIpsOpType CIpsPlgCreateMessageOperation::IpsOpType() const
    {
    FUNC_LOG;
    return EIpsOpTypeCreateMessageOp;
    }

//  End of File

// </qmail>
