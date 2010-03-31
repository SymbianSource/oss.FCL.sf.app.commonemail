/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
*       IpsPlgImap4FetchAttachmentOp implementation file
*
*/


#include "emailtrace.h"
#include "ipsplgheaders.h"

// Constants and defines
const TInt KFetchOpPriority = CActive::EPriorityStandard;
const TInt KIpsAttaFetchProgressReportInterval = 1000000; // 1 sec
const TInt KIpsAttaFetchRetryInterval = 1000000; // 1 sec
const TInt KIpsAttaFetchRetryCount = 30;

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
CIpsFetchProgReport* CIpsFetchProgReport::NewL( 
    CIpsPlgImap4FetchAttachmentOp& aAttaOp )
    {
    FUNC_LOG;
    CIpsFetchProgReport* self = new (ELeave) CIpsFetchProgReport( aAttaOp );
    CleanupStack::PushL( self );
    self->ConstructL( );
    CleanupStack::Pop( self );
    return self;
    }
    
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------    
CIpsFetchProgReport::CIpsFetchProgReport( 
    CIpsPlgImap4FetchAttachmentOp& aAttaOp ) : 
    CActive( CActive::EPriorityStandard ), 
    iAttaOp(aAttaOp)
    {
    FUNC_LOG;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------    
CIpsFetchProgReport::~CIpsFetchProgReport()
    {
    FUNC_LOG;
    Cancel();
    iTimer.Close();
    }
    
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------	    
void CIpsFetchProgReport::ConstructL()
    {
    FUNC_LOG;
    CActiveScheduler::Add(this);
    User::LeaveIfError( iTimer.CreateLocal() );
    AdjustTimer();
    }
    
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsFetchProgReport::AdjustTimer()   
    {
    FUNC_LOG;
    TTimeIntervalMicroSeconds32 sec( KIpsAttaFetchProgressReportInterval ); 
    TTime time;
    time.HomeTime();
    time = time + sec;
    iTimer.At(iStatus, time);
    SetActive();    
    }
    
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------    
void CIpsFetchProgReport::DoCancel()
    {
    FUNC_LOG;
    iTimer.Cancel();
    }
    
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------	
void CIpsFetchProgReport::RunL()
    {
    FUNC_LOG;
    iAttaOp.ReportProgressL();
    AdjustTimer();
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
CIpsPlgImap4FetchAttachmentOp* CIpsPlgImap4FetchAttachmentOp::NewL(
    CMsvSession& aMsvSession,
    TRequestStatus& aObserverRequestStatus,
    TInt aFunctionId,
    TMsvId aService,
    CIpsPlgTimerOperation& aActivityTimer,
    const TImImap4GetMailInfo& aGetMailInfo,
    const CMsvEntrySelection& aSel,
    TFSMailMsgId aFSMailBoxId,
    MFSMailRequestObserver& aFSOperationObserver,
    TInt aFSRequestId )
    {
    FUNC_LOG;
    CIpsPlgImap4FetchAttachmentOp* op = new (
        ELeave) CIpsPlgImap4FetchAttachmentOp(
        aMsvSession,
        aObserverRequestStatus,
        aFunctionId,
        aService,
        aActivityTimer,
        aGetMailInfo,
        aFSMailBoxId,
        aFSOperationObserver,
        aFSRequestId );
        
    CleanupStack::PushL( op );
    op->ConstructL( aSel );
    CleanupStack::Pop( op );
    return op;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
CIpsPlgImap4FetchAttachmentOp::CIpsPlgImap4FetchAttachmentOp(
    CMsvSession& aMsvSession,
    TRequestStatus& aObserverRequestStatus,
    TInt aFunctionId,
    TMsvId aService,
    CIpsPlgTimerOperation& aActivityTimer,
    const TImImap4GetMailInfo& aGetMailInfo,
    TFSMailMsgId aFSMailBoxId,
    MFSMailRequestObserver& aFSOperationObserver,
    TInt aFSRequestId )
    :
    CIpsPlgOnlineOperation(
    aMsvSession,
    KFetchOpPriority,
    aObserverRequestStatus,
    aActivityTimer,
    aFSMailBoxId,
    aFSOperationObserver,
    aFSRequestId),
    iSelection( NULL ),
    iGetMailInfo(aGetMailInfo),
    iFunctionId(aFunctionId),
    iRetryCount( 0 )
    {
    FUNC_LOG;
    iService = aService;
    iRetryTimer.CreateLocal();
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
CIpsPlgImap4FetchAttachmentOp::~CIpsPlgImap4FetchAttachmentOp()
    {
    FUNC_LOG;
    iRetryTimer.Close();
    delete iSelection;
    delete iProgReport;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsPlgImap4FetchAttachmentOp::ConstructL( const CMsvEntrySelection& aSel )
    {
    FUNC_LOG;
    BaseConstructL( KUidMsgTypeIMAP4 );
    iSelection = aSel.CopyL();
    DoConnectL();
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsPlgImap4FetchAttachmentOp::DoConnectL()
    {
    FUNC_LOG;
    iState = EStateConnecting;
    iStatus = KRequestPending;

    CIpsPlgImap4ConnectOp* connOp = CIpsPlgImap4ConnectOp::NewL(
        iMsvSession,
        KFetchOpPriority,
        iStatus, 
        iService,
        *iActivityTimer,
        iFSMailboxId,
        iFSOperationObserver,
        iFSRequestId,
        NULL, // event handler not needed whin plain connect
        ETrue,
        EFalse );
        
    delete iOperation;
    iOperation = connOp;

    SetActive();
    }
    
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsPlgImap4FetchAttachmentOp::RunL()
    {
    FUNC_LOG;
    TRAPD(err, DoRunL());
    if(err != KErrNone)
        {
        iProgress().iGenericProgress.iErrorCode = err;
        Complete();
        }
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsPlgImap4FetchAttachmentOp::DoRunL()
    {
    FUNC_LOG;
    switch( iState )
        {
        case EStateConnecting:
            {
            TBool connected = STATIC_CAST(
                CIpsPlgImap4ConnectOp*, iOperation)->Connected();
            if(!connected)
                {
                CompleteObserver( KErrCouldNotConnect );
                return;
                }
            iRetryCount = 0;
            DoFetchAttachmentL();
            break;
            }
        case EStateWaiting:
            {
            DoFetchAttachmentL();
            break;
            }
        case EStateFetching:         
            {
            delete iProgReport;
            iProgReport = NULL;
            
            TInt err = iStatus.Int();

            // If the server was busy, try again after a short delay, up to
            // some retry limit.
            if ( err == KErrServerBusy && iRetryCount < KIpsAttaFetchRetryCount )
                {
                iRetryCount++;
                INFO_1( "CIpsPlgImap4FetchAttachmentOp::DoRunL: iRetryCount = %d", iRetryCount );
                iState = EStateWaiting;
                iStatus = KRequestPending;
                iRetryTimer.After( iStatus, KIpsAttaFetchRetryInterval );
                SetActive();
                }
            else
                {
                if( err != KErrNone && iOperation )
                    {
                    iFetchErrorProgress = iOperation->ProgressL().AllocL();
                    }
                
                iState = EStateIdle;
                CompleteObserver( err );
                }
            break;
            }
        default:
            break;
        }
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
const TDesC8& CIpsPlgImap4FetchAttachmentOp::ProgressL()
    {
    FUNC_LOG;
    if(iFetchErrorProgress && (iState == EStateIdle))
        {
        // Completed, but with an error during fetch.
        return *iFetchErrorProgress;
        }
    
    if ( iOperation )
        {
        iProgress.Copy( iOperation->ProgressL() );
        }
    else
        {
        TImap4CompoundProgress progg;
        progg.iGenericProgress.iErrorCode = KErrNone;
        progg.iGenericProgress.iState = TImap4GenericProgress::EIdle;
        iProgress.Copy( TPckgBuf<TImap4CompoundProgress>(progg) );
        }
           
    return iProgress;
    }
    
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsPlgImap4FetchAttachmentOp::ReportProgressL()
    {
    FUNC_LOG;
    TInt error = KErrNone;
    TFSProgress fsProgress = { TFSProgress::EFSStatus_Waiting, 0, 0, KErrNone };
    if ( iOperation && iState == EStateFetching )
        {
        TRAP(error, iProgress.Copy( iOperation->ProgressL() ) );
        }
    
    if ( error == KErrNone )
        {
        const TImap4GenericProgress& progg = iProgress().iGenericProgress;
        fsProgress.iProgressStatus = TFSProgress::EFSStatus_Status;
        fsProgress.iMaxCount = progg.iBytesToDo;
        fsProgress.iCounter = progg.iBytesDone;
        fsProgress.iError = progg.iErrorCode;
        }
    else if ( error == KErrNotReady )
        {
        // This error indicates that operation not started yet
        // and waiting to other operation compleion
        fsProgress.iProgressStatus = TFSProgress::EFSStatus_Waiting;
        fsProgress.iMaxCount = 1;
        fsProgress.iCounter = 0;
        fsProgress.iError = KErrNone;
        }
    else
        {
        User::Leave( error );
        }
    
    iFSOperationObserver.RequestResponseL( fsProgress, iFSRequestId );
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
const TDesC8& CIpsPlgImap4FetchAttachmentOp::GetErrorProgressL(TInt /*aError*/ )
    {
    return *iFetchErrorProgress;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
TFSProgress CIpsPlgImap4FetchAttachmentOp::GetFSProgressL() const
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

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsPlgImap4FetchAttachmentOp::Complete()
    {
    FUNC_LOG;
    TRequestStatus* observer=&iObserverRequestStatus;
    User::RequestComplete(observer, KErrNone);
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsPlgImap4FetchAttachmentOp::DoFetchAttachmentL( )
    {
    FUNC_LOG;
    iState = EStateFetching;

    // Switch operations.
    delete iOperation;
    iOperation = NULL;
    iStatus = KRequestPending;

    iProgReport = CIpsFetchProgReport::NewL( *this );

    // Filters are not used when performing 'fetch' operation, 
    // use normal getmail info instead
    TPckg<TImImap4GetMailInfo> param(iGetMailInfo);
    InvokeClientMtmAsyncFunctionL( iFunctionId, *iSelection, iService, param );
    SetActive();
    }
    
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------    
TInt CIpsPlgImap4FetchAttachmentOp::GetEngineProgress( const TDesC8& aProgress )
    {
    FUNC_LOG;
    if( !aProgress.Length() )
        {
        return KErrNone;
        }
    else
        {
        TPckgBuf<TImap4CompoundProgress> paramPack;
        paramPack.Copy( aProgress );
        const TImap4GenericProgress& progress = paramPack().iGenericProgress;

        return progress.iErrorCode;
        }    
    }        


// End of File

