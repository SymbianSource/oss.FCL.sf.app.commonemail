/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Base class for online operations
*
*/



#include "emailtrace.h"
#include "ipsplgheaders.h"


// ----------------------------------------------------------------------------
// CIpsPlgOnlineOperation::~CIpsPlgOnlineOperation()
// ----------------------------------------------------------------------------
//
CIpsPlgOnlineOperation::~CIpsPlgOnlineOperation()
    {
    FUNC_LOG;
    Cancel();   
    delete iOperation;
    delete iBaseMtm;
    delete iMtmReg;
    }

// ----------------------------------------------------------------------------
// CIpsPlgOnlineOperation::CIpsPlgOnlineOperation()
// ----------------------------------------------------------------------------
//
CIpsPlgOnlineOperation::CIpsPlgOnlineOperation(
    CMsvSession& aMsvSession,
    TInt aPriority,
    TRequestStatus& aObserverRequestStatus,
    CIpsPlgTimerOperation& aActivityTimer,
    TFSMailMsgId aFSMailBoxId,
    MFSMailRequestObserver& aFSOperationObserver,
    TInt aFSRequestId,
    TBool aSignallingAllowed )
    :
    CIpsPlgBaseOperation( aMsvSession, aPriority, aObserverRequestStatus,
        aFSRequestId, aFSMailBoxId ), 
        iActivityTimer( &aActivityTimer ),
        iBaseMtm( NULL ),
        iMtmReg( NULL ), 
        iOperation( NULL ),
        iError( KErrNone ),
        iSignallingAllowed( aSignallingAllowed ),
        iFSOperationObserver( aFSOperationObserver )
    {
    FUNC_LOG;
    }

// ----------------------------------------------------------------------------
// CIpsPlgOnlineOperation::BaseConstructL()
// ----------------------------------------------------------------------------
//
void CIpsPlgOnlineOperation::BaseConstructL(TUid aMtmType)
    {
    FUNC_LOG;
    // reset timer, if operation not completed after timer fires causes
    // disconnection
    if( iActivityTimer )
        {
        iActivityTimer->ResetTimerOperation();
        }

    iMtmReg = CClientMtmRegistry::NewL( iMsvSession );
    
    iBaseMtm = iMtmReg->NewMtmL( aMtmType );
    
    iObserverRequestStatus = KRequestPending;
    CActiveScheduler::Add(this);
    }

// ----------------------------------------------------------------------------
// CIpsPlgOnlineOperation::DoCancel()
// ----------------------------------------------------------------------------
//
void CIpsPlgOnlineOperation::DoCancel()
    {
    FUNC_LOG;
    if( iOperation )
        {
        iOperation->Cancel();
        }
    CompleteObserver( KErrCancel );
    }

// ----------------------------------------------------------------------------
// CIpsPlgOnlineOperation::RunL()
// ----------------------------------------------------------------------------
//
void CIpsPlgOnlineOperation::RunL()
    {
    FUNC_LOG;
    
    TInt err = KErrNone;
    TRAP( err, DoRunL() );
    
    // Just end the operation, if something has gone wrong
    if ( err != KErrNone )
        {
        CompleteObserver( err );
        }
    else if ( iError != KErrNone )
        {
        CompleteObserver( iError );
        }
    }

// ----------------------------------------------------------------------------
// CIpsPlgOnlineOperation::RunError()
// ----------------------------------------------------------------------------
//
TInt CIpsPlgOnlineOperation::RunError( TInt aError )
    {
    FUNC_LOG;
    CompleteObserver( aError );
    return KErrNone; // RunError must return KErrNone to active sheduler.
    }

// ----------------------------------------------------------------------------
// CIpsPlgOnlineOperation::CompleteObserver()
// ----------------------------------------------------------------------------
//
void CIpsPlgOnlineOperation::CompleteObserver( TInt aStatus )
    {
    FUNC_LOG;
    TRequestStatus* status = &iObserverRequestStatus;
    if (status && status->Int() == KRequestPending)
        {
    
        SignalFSObserver( aStatus );
        // removed checks to prevent unwanted disconnections
        //if we're connected, reset activitytimer. if not, there is no reason to.
        if( iActivityTimer )
            {
            iActivityTimer->ResetTimerOperation();
            }
        User::RequestComplete(status, aStatus);
        }
    }

// ----------------------------------------------------------------------------
// CIpsPlgOnlineOperation::CompleteThis()
// ----------------------------------------------------------------------------
//
void CIpsPlgOnlineOperation::CompleteThis()
    {
    FUNC_LOG;
    
    TRequestStatus* status = &iStatus;
    User::RequestComplete(status, KErrNone);
    }

// ----------------------------------------------------------------------------
// CIpsPlgOnlineOperation::InvokeClientMtmAsyncFunctionL()
// ----------------------------------------------------------------------------
//
void CIpsPlgOnlineOperation::InvokeClientMtmAsyncFunctionL(
    TInt aFunctionId,
    TMsvId aEntryId,
    TMsvId aContextId,
    const TDesC8& aParams)
    {
    FUNC_LOG;
    
    CMsvEntrySelection* sel = new(ELeave) CMsvEntrySelection;
    CleanupStack::PushL( sel );
    sel->AppendL( aEntryId );
    InvokeClientMtmAsyncFunctionL(aFunctionId, *sel, aContextId, aParams);
    CleanupStack::PopAndDestroy( sel ); 
    }

// ----------------------------------------------------------------------------
// CIpsPlgOnlineOperation::InvokeClientMtmAsyncFunctionL()
// ----------------------------------------------------------------------------
//
void CIpsPlgOnlineOperation::InvokeClientMtmAsyncFunctionL(
    TInt aFunctionId,
    const CMsvEntrySelection& aSel,
    TMsvId aContextId,
    const TDesC8& aParams)
    {
    FUNC_LOG;
    
    TMsvEntry tEntry;
    TMsvId service;
    if ( aSel.Count() )
        {
        iMsvSession.GetEntry( aSel.At(0), service, tEntry );
        }

    if( aContextId != tEntry.iServiceId )
        {
        // Client context must be service for FetchL().
        iBaseMtm->SwitchCurrentEntryL( tEntry.iServiceId );
        }
    else
        {                
        iBaseMtm->SwitchCurrentEntryL( aContextId );
        }
    HBufC8* params = aParams.AllocLC();
    TPtr8 ptr(params->Des());
    // Delete previous operation if it exist
    if ( iOperation )
        {
        delete iOperation;
        iOperation = NULL;
        }
    iOperation = iBaseMtm->InvokeAsyncFunctionL(aFunctionId, aSel, ptr, iStatus);
    CleanupStack::PopAndDestroy( params ); 
    }

// ----------------------------------------------------------------------------
// CIpsPlgOnlineOperation::SignalFSObserver()
// ----------------------------------------------------------------------------
//
void CIpsPlgOnlineOperation::SignalFSObserver( TInt aStatus )
    {
    FUNC_LOG;
    if( iSignallingAllowed )
        {        
        TFSProgress prog;
        prog.iError = aStatus;
        // Initialize the progress data
        // it would be better to get fs progress from inherited class
        // by calling FSProgressL method??
        if ( prog.iError == KErrCancel || prog.iError == KErrImapBadLogon )
            {
            prog.iProgressStatus = TFSProgress::EFSStatus_RequestCancelled;
            }
        else
            {
            prog.iProgressStatus = TFSProgress::EFSStatus_RequestComplete;
            }
        // At least in the attachment download, FS UI assumes that
        // the counter fields are greater than 0
        prog.iMaxCount = 1;  
        prog.iCounter = 1;
        
        
        //in case of autoconnect, we don't have valid observer
        if( &iFSOperationObserver )
            {            
            TRAP_IGNORE( iFSOperationObserver.RequestResponseL( prog, iFSRequestId ) );
            }
        }
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
TInt CIpsPlgOnlineOperation::IpsOpType() const
    {
    FUNC_LOG;
    return EIpsOpTypeOnlineOp;
    }

