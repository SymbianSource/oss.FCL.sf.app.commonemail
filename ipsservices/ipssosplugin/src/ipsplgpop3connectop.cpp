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
const TInt KIpsPlgPop3PopulateLimitInitValue = 50;

_LIT( KIpsPlgPopConnectPanic, "PopConnectOp" );

// ----------------------------------------------------------------------------
// CIpsPlgPop3ConnectOp::NewL()
// ----------------------------------------------------------------------------
//
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
    CIpsPlgPop3ConnectOp* op = 
        new(ELeave) CIpsPlgPop3ConnectOp( aMsvSession, aObserverRequestStatus, 
            aService, aForcePopulate, aActivityTimer, aFSMailBoxId,
            aFSOperationObserver, aFSRequestId, 
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
    delete iEntry;
    delete iSelection;
    iState = EIdle;
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
    // handle these error situations properly, 
    // and report somehow back to framework
    TInt err( KErrNone );
 
// <qmail> remove EQueryingDetails state
    if ( iState == EStartConnect )
        {
        if ( iSubOperation )
            {
            // operation exist and it means 
            // disconnect operation was ongoing.
            // How handle errors
            delete iSubOperation;
            iSubOperation = NULL;
            }
        
        // Begin Connect.
        DoConnectL();
        iState = EConnected;
        }
    else if ( iState == EConnected )
        {
        // Connect completed
        err = GetOperationErrorCodeL( );
        User::LeaveIfError( err );
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
        err = GetOperationErrorCodeL( );
        if ( iEventHandler )
            {
            iEventHandler->SetNewPropertyEvent( iService, KIpsSosEmailSyncCompleted, err );
            }
        CIpsPlgSyncStateHandler::SaveSuccessfulSyncTimeL( iMsvSession, iService );

        // <qmail>
        // start disconnecting
        iState = Disconnecting;
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
    else if ( iState == EErrInvalidDetails )
        {
        // proper error code;
        CompleteObserver( KErrGeneral );
        }
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
    // might not never called, but gives something reasonable if called
    TFSProgress result = { TFSProgress::EFSStatus_Waiting, 0, 0, KErrNone };
    result.iError = KErrNone;
    switch( iState )
        {
        // <qmail> removed queryingdetails case
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
    iEntry( NULL ),
    iPopulateLimit( KIpsPlgPop3PopulateLimitInitValue ),
    iForcePopulate( aForcePopulate ),
    iSelection( NULL ),
    iEventHandler( aEventHandler )
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
   	
    iEntry = iMsvSession.GetEntryL( iService );
    if ( !iEntry )
        {
        User::Leave( KErrGeneral );
        }
    iSelection = new(ELeave) CMsvEntrySelection;
    const TMsvEntry& tentry = iEntry->Entry();
    
    if ( tentry.iType.iUid != KUidMsvServiceEntryValue )
        {
        User::Leave( KErrNotSupported );
        }
    
    // get correct populate and sync limet values from settings
    CImPop3Settings* settings = new(ELeave) CImPop3Settings();
    CleanupStack::PushL( settings );
    TPopAccount popAccountId;
    CEmailAccounts* accounts = CEmailAccounts::NewLC();
    accounts->GetPopAccountL( iService , popAccountId );
    accounts->LoadPopSettingsL( popAccountId, *settings );
    iPopulateLimit = settings->PopulationLimit();
    if ( iPopulateLimit > 0 )
        {
        iPopulateLimit = ( iPopulateLimit * 1024 ) / 75;
        }
    else
        {
        iPopulateLimit = KMaxTInt;
        }
    
    CleanupStack::PopAndDestroy( 2, settings );
       
    if ( tentry.Connected() )
        {      
        iState = EConnected; 
        // <qmail> SetActive(); moved inside CompleteThis();
        CompleteThis();
        }
    else
        {
        iState = EStartConnect; 
        // <qmail> SetActive(); moved inside CompleteThis();
        CompleteThis();
        }    
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
    
    if ( iEventHandler )
        {
        iEventHandler->SetNewPropertyEvent( 
                iService, KIpsSosEmailSyncStarted, KErrNone );
        } 
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
    pop3GetMailInfo.SetPopulationLimit( iPopulateLimit );
    TPckgBuf<TImPop3PopulateOptions> params( pop3GetMailInfo );

    iSelection->InsertL(0, iService);
    iBaseMtm->SwitchCurrentEntryL( iService );
    // Start the fetch operation
    InvokeClientMtmAsyncFunctionL( KPOP3MTMPopulateAll, *iSelection, params ); // <qmail> 1 param removed

    SetActive();
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
// <qmail> ValidateL() removed (did nothing)

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------    
// 
TInt CIpsPlgPop3ConnectOp::GetOperationErrorCodeL( )
    {
    FUNC_LOG;
    if ( !iSubOperation )
        {
        return KErrNotFound;
        }
    if ( !iSubOperation->IsActive() && iSubOperation->iStatus.Int() != KErrNone )
        {
        return iSubOperation->iStatus.Int();
        }
    
    TPckgBuf<TPop3Progress> paramPack;
    paramPack.Copy( iSubOperation->ProgressL() );
    const TPop3Progress& progress = paramPack();
    
    return progress.iErrorCode;
    }

// <qmail> Removing CIpsPlgImap4ConnectOp::CredientialsSetL 

//<qmail> new state
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsPlgPop3ConnectOp::DoDisconnect()
    {
    FUNC_LOG;
    // <qmail> unnecessary: iStatus = KRequestPending;
    InvokeClientMtmAsyncFunctionL( KPOP3MTMDisconnect, iService ); // <qmail> 1 param removed
    SetActive();
    }
//</qmail>
