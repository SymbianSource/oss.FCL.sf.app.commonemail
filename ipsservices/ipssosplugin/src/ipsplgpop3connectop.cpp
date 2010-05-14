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
* Description:  POP3 connect operation
*
*/

#include "emailtrace.h"
#include "ipsplgheaders.h"


// Constants and defines
// <qmail> rename const
const TInt KDefaultPopulateLimit( 50 );
const TInt KPopulateAlgorithmBytesInKilo( 1024 );
const TInt KPopulateAlgorithmRowLength( 75 );


// ----------------------------------------------------------------------------
// CIpsPlgPop3ConnectOp::NewL()
// ----------------------------------------------------------------------------
// <qmail> MFSMailRequestObserver& changed to pointer
// <qmail> aSignallingAllowed parameter removed
CIpsPlgPop3ConnectOp* CIpsPlgPop3ConnectOp::NewL(
    CMsvSession& aMsvSession,                           
    TRequestStatus& aObserverRequestStatus,
    TMsvId aService,
    TBool aForcePopulate,
    CIpsPlgTimerOperation& aActivityTimer,
    TFSMailMsgId aFSMailBoxId,
    MFSMailRequestObserver* aFSOperationObserver,
    TInt aFSRequestId,
    CIpsPlgEventHandler* aEventHandler )
    {
    FUNC_LOG;
    CIpsPlgPop3ConnectOp* op = new(ELeave) CIpsPlgPop3ConnectOp(
        aMsvSession,
        aObserverRequestStatus,
        aService,
        aForcePopulate,
        aActivityTimer,
        aFSMailBoxId,
        aFSOperationObserver,
        aFSRequestId,
        aEventHandler );

    CleanupStack::PushL( op );
    op->ConstructL();
    CleanupStack::Pop( op );
    return op;
    }

// ----------------------------------------------------------------------------
// CIpsPlgPop3ConnectOp::~CIpsPlgPop3ConnectOp()
// ----------------------------------------------------------------------------
//
CIpsPlgPop3ConnectOp::~CIpsPlgPop3ConnectOp()
    {
    FUNC_LOG;
    // <qmail> removed iEntry;
    // <qmail> removed iSelection;
    // <qmail> removed state setting
    }

// ----------------------------------------------------------------------------
// CIpsPlgPop3ConnectOp::ProgressL()
// ----------------------------------------------------------------------------
//
const TDesC8& CIpsPlgPop3ConnectOp::ProgressL()
    {
    FUNC_LOG;
    if( iError != KErrNone )
        {
        return GetErrorProgressL( iError );
        }
    else if( iSubOperation )
        {
        return iSubOperation->ProgressL();
        }
    
    iProgress().iErrorCode = KErrNone;
    return iProgress;    
    }   

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
void CIpsPlgPop3ConnectOp::DoCancel()
    {
    FUNC_LOG;
    if( iSubOperation )
        {
        iSubOperation->Cancel();
        }
    CompleteObserver( KErrCancel );
    }

// ----------------------------------------------------------------------------
// CIpsPlgPop3ConnectOp::DoRunL()
// ----------------------------------------------------------------------------
//
void CIpsPlgPop3ConnectOp::DoRunL()
    {
    FUNC_LOG;
    // <qmail> move TInt err declaration later, and narrow the scope

    // <qmail> remove EQueryingDetails state
    if ( iState == EStartConnect )
        {
        if ( iSubOperation )
            {
            // operation exist and it means 
            // disconnect operation was ongoing.
            delete iSubOperation;
            iSubOperation = NULL;
            }
        
// <qmail> ValidateL removed
        // Begin Connect.
        DoConnectL();
        iState = EConnected;
        }
    else if ( iState == EConnected )
        {
        // Connect completed
// <qmail> Login details checking removed
        User::LeaveIfError( iStatus.Int() );
        if ( iForcePopulate && Connected() )
            {
            iState = EPopulate;
            DoPopulateL();
            }
        else
            {
            // <qmail>
            // start disconnecting
            iState = EDisconnecting;
            DoDisconnect();
            // </qmail>
            }
        }
    else if ( iState == EPopulate )
        {
        // <qmail> suboperation has completed, the result is in 'this->iStatus'
        if ( iEventHandler )
            {
            iEventHandler->SetNewPropertyEvent( iService, KIpsSosEmailSyncCompleted, iStatus.Int() );
            }
        CIpsPlgSyncStateHandler::SaveSuccessfulSyncTimeL( iMsvSession, iService );

        // <qmail>
        // start disconnecting
        iState = EDisconnecting;
        DoDisconnect();
        // </qmail>
        }
    // <qmail>
    else if ( iState == EDisconnecting )
        {
        // we're done here
        CompleteObserver( KErrNone );
        }
    // </qmail>
    // <qmail> removed state EErrInvalidDetails
    else
        {
        User::Panic( KIpsPlgPopConnectPanic ,KErrNotFound);
        }
    }

// ----------------------------------------------------------------------------
// CIpsPlgPop3ConnectOp::GetErrorProgressL
// ----------------------------------------------------------------------------
//
const TDesC8& CIpsPlgPop3ConnectOp::GetErrorProgressL( TInt aError )
    {
    FUNC_LOG;
    iError = aError;
    if ( iSubOperation && iError == KErrNone )
        {
        return iSubOperation->ProgressL();
        }
    TPop3Progress& progress = iProgress();
    progress.iPop3Progress = TPop3Progress::EPopConnecting;
    progress.iTotalMsgs = 0;
    progress.iMsgsToProcess = 0;
    progress.iBytesDone = 0;
    progress.iTotalBytes = 0;
    progress.iErrorCode = iError;
    progress.iPop3SubStateProgress = TPop3Progress::EPopConnecting;
    progress.iServiceId = iService;
    return iProgress;
    }

// ----------------------------------------------------------------------------
// CIpsPlgPop3ConnectOp::GetFSProgressL()
// ----------------------------------------------------------------------------
//
TFSProgress CIpsPlgPop3ConnectOp::GetFSProgressL() const
    {
    FUNC_LOG;
    TFSProgress result = { TFSProgress::EFSStatus_Waiting, 0, 0, KErrNone };
    switch( iState )
        {
        // <qmail> removed queryingdetails cases
        case EStartConnect:
        case EConnected:
            result.iProgressStatus = TFSProgress::EFSStatus_Connecting;
            break;
        case EPopulate:
            // staus is used to inform fetching in base plugin
            result.iProgressStatus = TFSProgress::EFSStatus_Status;
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
// CIpsPlgPop3ConnectOp::CIpsPlgPop3ConnectOp
// ----------------------------------------------------------------------------
//
// <qmail> priority parameter has been removed
// <qmail> MFSMailRequestObserver& changed to pointer
// <qmail> aSignallingAllowed parameter removed
CIpsPlgPop3ConnectOp::CIpsPlgPop3ConnectOp(
    CMsvSession& aMsvSession,
    TRequestStatus& aObserverRequestStatus,
    TMsvId aServiceId,
    TBool aForcePopulate,
    CIpsPlgTimerOperation& aActivityTimer,
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
    iState( EIdle ),
    iForcePopulate( aForcePopulate ),
    iEventHandler( aEventHandler ),
    iAlreadyConnected( EFalse )
    {
    iService = aServiceId; 
    }

// ----------------------------------------------------------------------------
// CIpsPlgPop3ConnectOp::ConstructL()
// ----------------------------------------------------------------------------
//
void CIpsPlgPop3ConnectOp::ConstructL()
    {
    FUNC_LOG;
    BaseConstructL( KUidMsgTypePOP3 );

    // <qmail> iSelection construction has been removed

    CMsvEntry* cEntry = iMsvSession.GetEntryL( iService );
    User::LeaveIfNull( cEntry );
    // NOTE: Not using cleanupStack as it is not needed in this situation:
    const TMsvEntry tentry = cEntry->Entry();
    delete cEntry;
    cEntry = NULL;

    if ( tentry.iType.iUid != KUidMsvServiceEntryValue )
        {
        // should we panic with own codes?
        User::Leave( KErrNotSupported );
        }

    // <qmail> no need to have population limit as member variable
    // <qmail> it is read from settings when needed
    
    if ( tentry.Connected() )
        {      
        iState = EConnected;
		iAlreadyConnected = ETrue;
        }
    else
        {
        iState = EStartConnect;
        }
    // <qmail> SetActive(); moved inside CompleteThis();
    CompleteThis();
    }

// ----------------------------------------------------------------------------
// CIpsPlgPop3ConnectOp::DoConnectL()
// ----------------------------------------------------------------------------
//
void CIpsPlgPop3ConnectOp::DoConnectL()
    {
    FUNC_LOG;
    // <qmail> unnecessary: iStatus = KRequestPending;
    InvokeClientMtmAsyncFunctionL( KPOP3MTMConnect, iService ); // <qmail> 1 param removed
    SetActive();

// <qmail>
    if ( iEventHandler )
        {
        iEventHandler->SetNewPropertyEvent( iService, KIpsSosEmailSyncStarted, KErrNone );
        }
// </qmail>
    }

// ----------------------------------------------------------------------------
// CIpsPlgPop3ConnectOp::DoPopulateL()
// ----------------------------------------------------------------------------
//
void CIpsPlgPop3ConnectOp::DoPopulateL()
    {
    FUNC_LOG;
    // <qmail> unnecessary: iStatus = KRequestPending;

    // Prepare parameters and include filtering
    TImPop3PopulateOptions pop3GetMailInfo;
    pop3GetMailInfo.SetMaxEmailSize( KMaxTInt32 );
// <qmail>
    pop3GetMailInfo.SetPopulationLimit( GetPopulateLimitFromSettingsL() );
    TPckgBuf<TImPop3PopulateOptions> params( pop3GetMailInfo );

    // <qmail> selection is no longer a member variable
    CMsvEntrySelection* selection = new(ELeave) CMsvEntrySelection;
    CleanupStack::PushL( selection );
    selection->InsertL( 0, iService );
    iBaseMtm->SwitchCurrentEntryL( iService );
    // Start the fetch operation
    InvokeClientMtmAsyncFunctionL( KPOP3MTMPopulateAll, *selection, params ); // <qmail> 1 param removed
    SetActive();
    CleanupStack::PopAndDestroy( selection );
// <qmail> iEventHandler->SetNewPropertyEvent call removed from here
// </qmail>
    }
// <qmail> Connected() moved to baseclass

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// <qmail> return type
TIpsOpType CIpsPlgPop3ConnectOp::IpsOpType() const
    {
    FUNC_LOG;
    return EIpsOpTypePop3SyncOp;
    }

// <qmail> removed QueryUsrPassL() from here as unneeded
// <qmail> removed ValidateL() (did nothing)
// <qmail> removed TInt CIpsPlgPop3ConnectOp::GetOperationErrorCodeL( )
// <qmail> removed CIpsPlgImap4ConnectOp::CredientialsSetL

//<qmail> new functions
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsPlgPop3ConnectOp::DoDisconnect()
    {
    FUNC_LOG;
    // <qmail> unnecessary: iStatus = KRequestPending;
    InvokeClientMtmAsyncFunctionL( KPOP3MTMDisconnect, iService ); // <qmail> 1 param removed
    SetActive();
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
TInt CIpsPlgPop3ConnectOp::GetPopulateLimitFromSettingsL()
    {
    FUNC_LOG;
    TInt limit( KDefaultPopulateLimit );
        
    CImPop3Settings* settings = new(ELeave) CImPop3Settings();
    CleanupStack::PushL( settings );
    TPopAccount popAccountId;
    CEmailAccounts* accounts = CEmailAccounts::NewLC();
    accounts->GetPopAccountL( iService , popAccountId );
    accounts->LoadPopSettingsL( popAccountId, *settings );
    limit = settings->PopulationLimit();
    if ( limit > 0 )
        {
        // basically doing a _very_rough_ conversion from kilobyte value to number-of-rows
    limit = ( limit * KPopulateAlgorithmBytesInKilo ) / KPopulateAlgorithmRowLength;
        }
    CleanupStack::PopAndDestroy( 2, settings );
    return limit;
    }

//</qmail>
