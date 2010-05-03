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
* Description:  POP3 fetch message operation class
*
*/

#include "emailtrace.h"
#include "ipsplgheaders.h"

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
CIpsPlgPop3FetchOperation* CIpsPlgPop3FetchOperation::NewL(
    CMsvSession& aMsvSession,
    TRequestStatus& aObserverRequestStatus,
    TInt aFunctionId,
    TMsvId aService,
    CIpsPlgTimerOperation& aActivityTimer,
    const TImPop3GetMailInfo& aGetMailInfo,
    const CMsvEntrySelection& aSel,
    TFSMailMsgId aFSMailBoxId,
    MFSMailRequestObserver* aFSOperationObserver,
    TInt aFSRequestId,
    CIpsPlgEventHandler* aEventHandler )
    {
    FUNC_LOG;
    CIpsPlgPop3FetchOperation* op = new(ELeave) CIpsPlgPop3FetchOperation(
        aMsvSession, aObserverRequestStatus, aFunctionId, aService,
        aActivityTimer, aGetMailInfo, aFSMailBoxId, aFSOperationObserver,
        aFSRequestId, aEventHandler );
        
    CleanupStack::PushL(op);
    op->ConstructL(aSel);
    CleanupStack::Pop( op );
    return op;
    }

// ----------------------------------------------------------------------------
// ~CIpsPlgPop3FetchOperation
// ----------------------------------------------------------------------------
CIpsPlgPop3FetchOperation::~CIpsPlgPop3FetchOperation()
    {
    FUNC_LOG;
    delete iFetchErrorProgress;
    delete iSelection;
    }

// ----------------------------------------------------------------------------
// ProgressL
// ----------------------------------------------------------------------------
const TDesC8& CIpsPlgPop3FetchOperation::ProgressL()
    {
    FUNC_LOG;
    
    if(iFetchErrorProgress && (iState == EStateIdle))
        {
        // Completed, but with an error during fetch.
        return *iFetchErrorProgress;
        }
    else
        {
        TPop3Progress progg;
        progg.iErrorCode = KErrNone;
        TPckgBuf<TPop3Progress> param(progg);
        iProgressBuf.Copy(param);
        return iProgressBuf;   
        }
    }

// ----------------------------------------------------------------------------
// ConstructL
// ----------------------------------------------------------------------------
void CIpsPlgPop3FetchOperation::ConstructL(const CMsvEntrySelection& aSel)
    {
    FUNC_LOG;
    BaseConstructL( KUidMsgTypePOP3 );
    iSelection = aSel.CopyL();
    iSelection->InsertL( 0, iService );
    // For Get Mail API, first selection element must be service.
    DoConnectL();
    }

// ----------------------------------------------------------------------------
// DoConnectL
// ----------------------------------------------------------------------------
void CIpsPlgPop3FetchOperation::DoConnectL()
    {
    FUNC_LOG;
    

    iState = EStateConnecting;
    iStatus = KRequestPending;
    
    // when connecting for the fetch operation, don't let connect operation to do fetch,
    // because we do it by ourself. That's why give 0 to connect operation.    
    CIpsPlgPop3ConnectOp* connOp = CIpsPlgPop3ConnectOp::NewL(
        iMsvSession,
        iStatus, 
        iService, 
        EFalse, 
        iActivityTimer,
        iFSMailboxId, 
        iFSOperationObserver, 
        iFSRequestId, 
        NULL );
        
    delete iSubOperation;
    iSubOperation = connOp;

    SetActive();
    }

// ----------------------------------------------------------------------------
// DoFetchL
// ----------------------------------------------------------------------------
void CIpsPlgPop3FetchOperation::DoFetchL()
    {
    FUNC_LOG;

    iState = EStateFetching;

    // Switch operations.
    delete iSubOperation;
    iSubOperation = NULL;

    // Filters are not used when performing 'fetch' operation, use normal 
    // getmail info instead
    TPckg<TImPop3GetMailInfo> param( iGetMailInfo );
    InvokeClientMtmAsyncFunctionL( iFunctionId, *iSelection, param );
    SetActive();
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// <qmail> priority parameter has been removed
CIpsPlgPop3FetchOperation::CIpsPlgPop3FetchOperation(
    CMsvSession& aMsvSession,
    TRequestStatus& aObserverRequestStatus,
    TInt aFunctionId,
    TMsvId aService,
    CIpsPlgTimerOperation& aActivityTimer,
    const TImPop3GetMailInfo& aGetMailInfo,
    TFSMailMsgId aFSMailBoxId,
    MFSMailRequestObserver* aFSOperationObserver,
    TInt aFSRequestId,
    CIpsPlgEventHandler* aEventHandler )
    : 
    CIpsPlgOnlineOperation(
        aMsvSession,
        aObserverRequestStatus, 
        aActivityTimer, 
        aFSMailBoxId,
        aFSOperationObserver, 
        aFSRequestId ),
    iFunctionId( aFunctionId ),
    iGetMailInfo( aGetMailInfo ), 
    iEventHandler( aEventHandler )
    {
    FUNC_LOG;
    iService = aService;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsPlgPop3FetchOperation::RunL()
    {
    FUNC_LOG;

    TRAP( iError, DoRunL() );
    
    if( iError )
        {        
        // Notify observer we have finished.
        CompleteObserver();
        }    
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsPlgPop3FetchOperation::DoCancel()
    {
    FUNC_LOG;

    CIpsPlgOnlineOperation::DoCancel();
    if(iState == EStateFetching)
        {
        // Cancelled while fetching. Need to disconnect.
        iMsvSession.StopService( iService );
        // Ignore return value, nothing we can do.
        }
    if ( iEventHandler )
        {
        iEventHandler->SetNewPropertyEvent( 
                iService, KIpsSosEmailSyncCompleted, KErrCancel );
        }
    }

// ----------------------------------------------------------------------------
// DoRunL
// ----------------------------------------------------------------------------
void CIpsPlgPop3FetchOperation::DoRunL()
    {
    FUNC_LOG;

    switch(iState)
        {
        case EStateConnecting:
            {
            // Connect complete.
            // <qmail> Connected() usage
            if ( !Connected() )
                {
                CompleteObserver( KErrCouldNotConnect );
                return;
                }
            DoFetchL();
            }
            break;
        case EStateFetching:         
            {
            TInt err = iStatus.Int();

            if( KErrNone != err )
                {
                TPckgBuf<TPop3Progress> paramPack;
                if ( iSubOperation )
                    {
                paramPack.Copy( iSubOperation->ProgressL() );
                    }
                TPop3Progress& progress = paramPack();
                progress.iErrorCode = err;
                iFetchErrorProgress = paramPack.AllocL();
                }
            
            iState = EStateIdle;
            CompleteObserver( err );
            if ( iEventHandler )
                {
                iEventHandler->SetNewPropertyEvent( 
                        iService, KIpsSosEmailSyncCompleted, err );
                }
            }
            break;
            
        default:
            break;
        }        
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
const TDesC8& CIpsPlgPop3FetchOperation::GetErrorProgressL( TInt /* aError */ )
    {
    FUNC_LOG;

    return *iFetchErrorProgress;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
TFSProgress CIpsPlgPop3FetchOperation::GetFSProgressL() const
    {
    FUNC_LOG;
    // might not never called, but gives something reasonable if called
    TFSProgress result = { TFSProgress::EFSStatus_Waiting, 0, 0, KErrNone };
    result.iError = KErrNone;
    switch( iState )
        {
        case EStateConnecting:
            result.iProgressStatus = TFSProgress::EFSStatus_Connecting;
            break;
        case EStateFetching:
            result.iProgressStatus = TFSProgress::EFSStatus_Connected;
            break;
        default:
            result.iProgressStatus = TFSProgress::EFSStatus_RequestComplete;
            break;
        }
    if ( iStatus.Int() == KErrCancel )
        {
        result.iProgressStatus = TFSProgress::EFSStatus_RequestCancelled;
        result.iError = KErrCancel;
        }
    
    return result;
    }

// <qmail> new func to this op
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------    
TIpsOpType CIpsPlgPop3FetchOperation::IpsOpType() const
    {
    FUNC_LOG;
    return EIpsOpTypePop3FetchOp;
    }
